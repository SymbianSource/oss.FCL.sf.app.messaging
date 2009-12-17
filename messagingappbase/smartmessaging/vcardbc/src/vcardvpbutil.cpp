/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   VirtualPhonebook support for importing VCard messages
*
*/



// system includes
#include <AknQueryDialog.h>     // CAknTextQueryDialog
#include <vcardbc.rsg>          // resource definitions

//Phonebook 2 API
#include <CPbk2StoreConfiguration.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkVCardEng.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactStoreInfo.h>
#include <CPbk2SortOrderManager.h>
#include <MPbk2ContactNameFormatter.h>
#include <Pbk2ContactNameFormatterFactory.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <Pbk2UIControls.rsg>
#include <VPbkSyncConstants.h>
#include <TVPbkFieldVersitProperty.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkContactStoreUri.h>
#include <stringloader.h>           // StringLoader
#include <AknNoteWrappers.h>
#include <pbk2commonui.rsg>
#include "VCardLog.h"
#include <s32file.h> // for RFileReadStream
#include <barsread.h> // for ResourceReader

// user includes
#include "vcardvpbutil.h"

// local constants & enumerations
_LIT(KClassName, "CVCardVpbUtil");
_LIT(KUnnamedText, "Unnamed");
_LIT(KDEFAULT_CNTDB_URI, "cntdb://c:contacts.cdb");

enum TPanicCode
	{
	ENoContactImported,
	EInvalidIndex,
    EPanicPostCond_ReallocBufferL
	};

/// Unnamed namespace for this-file-only helper funtions
namespace {

// Initial size for the conversion buffer
const TInt KBufferLength = 128;
_LIT8(KWvAddressVersitExtensionName, "X-WV-ID");

/**
 * Searches Versit property aArray for field aName.
 */
TBool ContainsFieldType(
        TArray<TVPbkFieldVersitProperty> aArray,
        TVPbkFieldTypeName aName)
    {
    TBool ret = EFalse;
    const TInt size = aArray.Count();
    for (TInt i=0; i<size; ++i)
        {
        if (aArray[i].Name() == aName)
            {
            ret = ETrue;
            break;
            }
        }
    return ret;
    }

/**
 * Searches Versit property array for a field aName
 * which has aExtensionName.
 */
TBool ContainsFieldTypeAndExtensionName(
        TArray<TVPbkFieldVersitProperty> aArray,
        TVPbkFieldTypeName aName,
        const TDesC8& aExtensionName)
    {
    TBool ret = EFalse;
    const TInt size = aArray.Count();
    for (TInt i=0; i<size; ++i)
        {
        if (aArray[i].Name() == aName &&
            aArray[i].ExtensionName().CompareF(aExtensionName) == 0)
            {
            ret = ETrue;
            break;
            }
        }
    return ret;
    }


/**
 * Inspects is number conversion allowed. It is not allowed
 * for certain fields, like for email field.
 */
inline TBool IsNumberConversionAllowed(const MVPbkFieldType& aFieldType)
    {
    TBool ret = ETrue;

    // Perform number conversion if allowed
    if (ContainsFieldType(aFieldType.VersitProperties(), EVPbkVersitNameURL))
        {
        ret = EFalse;
        }
    else if (ContainsFieldType(aFieldType.VersitProperties(), EVPbkVersitNameEMAIL))
        {
        ret = EFalse;
        }
    // search for the X-WV-ID field
    else if (ContainsFieldTypeAndExtensionName(
        aFieldType.VersitProperties(), EVPbkVersitNameX, KWvAddressVersitExtensionName))
        {
        ret = EFalse;
        }

    return ret;
    }


} // namespace


// ======== MEMBER FUNCTIONS ========

CVCardVpbUtil* CVCardVpbUtil::NewL()
    {
    CVCardVpbUtil* self = new( ELeave ) CVCardVpbUtil();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CVCardVpbUtil::~CVCardVpbUtil()
	{
    delete iAvailableUris;
    
	iContactsToShow.ResetAndDestroy();

	iContactsToStore.ResetAndDestroy();

	if ( iDefaultContactStore )
        {
        iDefaultContactStore->Close( *this );
        }

    if ( iShowContactStore )
        {
        iShowContactStore->Close( *this );
        }
        
    if(iStoreList)
        {
        iStoreList->CloseAll( *this );
        }

    delete iContactManager;
    delete iVCardEngine;
    delete iBuffer;
	}

void CVCardVpbUtil::ImportVCardL( TVCardBCBusinessCardType aVCardType, RFile aFileHandle )
	{
	RFileReadStream stream;
	stream.Attach( aFileHandle );
    CleanupClosePushL( stream );
	iVCardType = aVCardType;
    
    //import is done to both store at this point as the fileHandle is not
    //accessible later
	VCardToPhoneStoreL( stream );
	VCardToDefaultStoreL( stream );
	CleanupStack::Pop(); //stream

	//were done with stream as the data has been imported
	//this will also close the fileHandle
	stream.Release();
	stream.Close();
	}

TBool CVCardVpbUtil::CommitVCardToStoreL()
	{
	MVPbkContactOperationBase* ope( NULL );	
	if( DefaultStoreSupportsAllFieldsL()&& !StoreFullL() )
		{
		ope =
        	iContactManager->CommitContactsL( iContactsToStore.Array(), *this );
		}
	else
		{
		//dialog to ask user if contact data is to be stored to phone memory
		//this is the case if the contact data can not be fitted to the
		//default saving store ie. SIM store
		CAknQueryDialog* qDlg = CAknQueryDialog::NewL();
	    if ( qDlg->ExecuteLD( R_VCARD_STORE_QUERY ) )
	    	{
        	ope =
        	iContactManager->CommitContactsL( iContactsToShow.Array(), *this );
	    	}
		}
	if( ope )
		{
		CleanupDeletePushL( ope );
		
		//the active scheduler needs two iterations
		for ( TInt i = 0; i < 2; ++i )
        	{
        	StartSchedulerAndDecideToLeaveL();
        	}
        CleanupStack::PopAndDestroy();
    	return ETrue;
		}
	return EFalse;
	}

MVPbkStoreContact* CVCardVpbUtil::ContactData() const
	{
	__ASSERT_DEBUG( iContactsToShow.Count() > 0, Panic( ENoContactImported ) );
	return iContactsToShow[0];
	}

TBool CVCardVpbUtil::IsContactItemEmpty()
	{
	const MVPbkStoreContactFieldCollection& fields = ContactData()->Fields();
    TInt count = fields.FieldCount();

    TBool ret( ETrue );
    while( count )
        {
        __ASSERT_DEBUG( count <= fields.FieldCount() && count > 0,
                        Panic( EInvalidIndex ) );

        const MVPbkStoreContactField& field = fields.FieldAt( count - 1 );

        if( !field.FieldData().IsEmpty() )
            {
            ret = EFalse;
            break;
            }
        count--;
        }
    return ret;
	}

void CVCardVpbUtil::OpenComplete()
	{
	//nothing to do
	}

void CVCardVpbUtil::StoreReady( MVPbkContactStore& /*aContactStore*/ )
	{
	if( iRespondToObserverEvent )
	    {
    	iLastError = KErrNone;
        CActiveScheduler::Stop();
	    }
	}

void CVCardVpbUtil::StoreUnavailable(
				MVPbkContactStore& aContactStore,
				TInt /*aReason*/ )
	{
	// Remove the store from the available uri list. This is the case e.g. in when SIM card 
    // is set to be default saving memory and SIM card is not inserted to phone.
    TVPbkContactStoreUriPtr uri = aContactStore.StoreProperties().Uri();
    iAvailableUris->Remove( uri );
	if( iRespondToObserverEvent )
	    {
	    //we do not consider an unavailable store as leaving error situation
	    iLastError = KErrNone;
	    CActiveScheduler::Stop();
	    }
	}

void CVCardVpbUtil::HandleStoreEventL(
                MVPbkContactStore& /*aContactStore*/,
                TVPbkContactStoreEvent /*aStoreEvent*/ )
	{
	if( iRespondToObserverEvent )
	    {
	    CActiveScheduler::Stop();
	    }
	}

void CVCardVpbUtil::VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& /*aOperation*/,
                MVPbkStoreContact* /*aContact*/ )
	{
	if( iRespondToObserverEvent )
	    {
	    CActiveScheduler::Stop();
	    }
	}

void CVCardVpbUtil::VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& /*aOperation*/,
                TInt aError )
	{
	if( iRespondToObserverEvent )
	    {
	    iLastError = aError;
	    CActiveScheduler::Stop();
	    }
	}

void CVCardVpbUtil::StepComplete(
							MVPbkContactOperationBase& /*aOperation*/,
                            TInt /*aStepSize*/ )
    {
	if( iRespondToObserverEvent )
	    {
        iLastError = KErrNone;
	    }
    }


TBool CVCardVpbUtil::StepFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt /*aStepSize*/,
        TInt aError )
    {
	if( iRespondToObserverEvent )
	    {
        iLastError = aError;
        CActiveScheduler::Stop();

        //since we trigger a leave we can also cancel the
        //ongoing operation
        return EFalse;
	    }
    return ETrue;	    
    }

void CVCardVpbUtil::OperationComplete(
						MVPbkContactOperationBase& /*aOperation*/ )
    {
	if( iRespondToObserverEvent )
	    {
        iLastError = KErrNone;
        CActiveScheduler::Stop();
	    }
    }

CVCardVpbUtil::CVCardVpbUtil()
	{
	}

void CVCardVpbUtil::ConstructL()
	{
	CPbk2StoreConfiguration* storeConfiguration = CPbk2StoreConfiguration::NewL();
	CleanupStack::PushL( storeConfiguration );
    iAvailableUris = storeConfiguration->CurrentConfigurationL();
    
	//if default contact store is not included in uriarray we will add it manually
	//this happens if the store is not enabled in phonebook settings
	TVPbkContactStoreUriPtr defaultCntDbUriPtr( VPbkContactStoreUris::DefaultCntDbUri() );
	if ( !iAvailableUris->IsIncluded( defaultCntDbUriPtr ) )
        {
        iAvailableUris->AppendL( defaultCntDbUriPtr );
        }

	iContactManager = CVPbkContactManager::NewL( *iAvailableUris );
	iStoreList = &iContactManager->ContactStoresL();
	TInt storeCount ( iStoreList->Count() );
	iStoreList->OpenAllL( *this );

	// Every store sends EStoreReady or EOpenError event
    for ( TInt i = 0; i < storeCount; ++i )
        {
        StartSchedulerAndDecideToLeaveL();
        }

	iVCardEngine = CVPbkVCardEng::NewL( *iContactManager );
    
	if ( iAvailableUris->Count() > 0 )
        {
        // It is assumed that phone memory store is always succesfully opened.
        
        // Default saving memory might not be available. E.g. if it set to SIM and SIM is
        // not inserted to phone.
        TVPbkContactStoreUriPtr defaultsavingstore = storeConfiguration->DefaultSavingStoreL();
        if ( iAvailableUris->IsIncluded( defaultsavingstore ) )
            {
            iDefaultContactStore =
                iContactManager->ContactStoresL().Find( defaultsavingstore );        
            }
        else
            {
            iDefaultContactStore = 
                iContactManager->ContactStoresL().Find( defaultCntDbUriPtr );
            }
        
        iShowContactStore =
            iContactManager->ContactStoresL().Find( defaultCntDbUriPtr );
        }
    else
        {
        // Even the phone memory store could not be opened. We can leave, since it is impossible
        // to open vCard.
        User::Leave( KErrCouldNotConnect );
        }

    iBuffer = HBufC::NewL(KBufferLength);
    CleanupStack::PopAndDestroy( storeConfiguration );
	}

TBool CVCardVpbUtil::DefaultStoreSupportsAllFieldsL()
	{
	TBool retVal( ETrue );
	TInt shownCount = iContactsToShow[0]->Fields().FieldCount();
	TInt storedCount = iContactsToStore[0]->Fields().FieldCount();

	CPbk2SortOrderManager* sortOrderManager = CPbk2SortOrderManager::NewL( iContactManager->FieldTypes() );
	CleanupStack::PushL( sortOrderManager );

	MPbk2ContactNameFormatter* nameFormatter = Pbk2ContactNameFormatterFactory::CreateL(
            KUnnamedText, iContactManager->FieldTypes(),
            *sortOrderManager );
    CleanupStack::PushL( nameFormatter );

	TInt shownTitleFields( 0 );
	for( TInt counter = 0; counter < shownCount; counter++ )
		{
		const MVPbkBaseContactField& srcField = iContactsToShow[0]->Fields().FieldAt( counter );
		const MVPbkFieldType* fieldType = srcField.BestMatchingFieldType();
		if( nameFormatter->IsTitleFieldType( *fieldType ) )
			shownTitleFields++;
		}

	TInt storedTitleFields( 0 );
	for( TInt counter = 0; counter < storedCount; counter++ )
		{
		const MVPbkBaseContactField& srcField = iContactsToStore[0]->Fields().FieldAt( counter );
		const MVPbkFieldType* fieldType = srcField.BestMatchingFieldType();
		if( nameFormatter->IsTitleFieldType( *fieldType ) )
			storedTitleFields++;
		}

    CleanupStack::Pop( 2 );
    delete nameFormatter;
    delete sortOrderManager;

    //check if any of the dropped fields was title field
	if( shownCount - shownTitleFields > storedCount - storedTitleFields )
		retVal = EFalse;

	return retVal;
	}


void CVCardVpbUtil::VCardToPhoneStoreL( RFileReadStream& aStream )
	{
	if( iContactsToShow.Count() > 0 )
		{
		iContactsToShow.ResetAndDestroy();
    	iContactsToShow.Close();
		}

	//always ensure that the read stream is in the beginning
	//of the file
	aStream.Source()->SeekL( MStreamBuf::ERead, EStreamBeginning, 0 );
	MVPbkContactOperationBase* op;
	if( iVCardType == EVCard )
		{
		op = iVCardEngine->ImportVCardL(
				iContactsToShow,
    			*iShowContactStore,
    			aStream,
    			*this);
		}
	else
		{
		op =
    		iVCardEngine->ImportCompactBusinessCardL(
    		                        iContactsToShow,
    								*iShowContactStore,
    								aStream,
    								*this);
		}
    CleanupDeletePushL( op );
    StartSchedulerAndDecideToLeaveL();
    CleanupStack::PopAndDestroy();

	}

void CVCardVpbUtil::VCardToDefaultStoreL( RFileReadStream& aStream )
	{
	if( iContactsToStore.Count() > 0 )
		{
		iContactsToStore.ResetAndDestroy();
    	iContactsToStore.Close();
		}

	//always ensure that the read stream is in the beginning
	//of the file
	aStream.Source()->SeekL( MStreamBuf::ERead, EStreamBeginning, 0 );
	MVPbkContactOperationBase* op;
	if( iVCardType == EVCard )
		{
		op =
    		iVCardEngine->ImportVCardL(
    		                        iContactsToStore,
    								*iDefaultContactStore,
    								aStream,
    								*this);
		}
	else
		{
		op =
    		iVCardEngine->ImportCompactBusinessCardL(
    		                        iContactsToStore,
    								*iDefaultContactStore,
    								aStream,
    								*this);
		}
    CleanupDeletePushL( op );
    StartSchedulerAndDecideToLeaveL();
    CleanupStack::PopAndDestroy();

	}

void CVCardVpbUtil::StartSchedulerAndDecideToLeaveL()
	{
	//we want to control the responding to observable events to only happen 
	//to requests made from this module
	iRespondToObserverEvent = ETrue;
	CActiveScheduler::Start();
	User::LeaveIfError( iLastError );
	iRespondToObserverEvent = EFalse;
	}

TPtrC CVCardVpbUtil::FormatFieldContentL
        (const MVPbkBaseContactField& aField)
    {
    const MVPbkFieldType* aFieldType = aField.BestMatchingFieldType();
    TPtr text(iBuffer->Des());

    switch (aField.FieldData().DataType())
        {
        case EVPbkFieldStorageTypeText:
            {
            const MVPbkContactFieldTextData* fieldData =
                    &MVPbkContactFieldTextData::Cast(aField.FieldData());
            TPtrC fieldText(fieldData->Text());
            
            if (IsFieldTypeL(*aFieldType, R_MESSAGING_PHONENUMBER_SELECTOR))
                {
                // telephony field
                
                // convert number according to active number setting
                if ( fieldText.Length() <= KBufferLength )
                    {
                    HBufC* formattedPhoneNumber = fieldText.AllocLC();
                    TPtr formatterNumber = formattedPhoneNumber->Des();
                    
                    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
                        formatterNumber );
                    text.Set(ReallocBufferL(formatterNumber.Length()+4));
                    _LIT( Kstr, " " );
                    TDesC16 tmp = Kstr;
                    text.Copy(tmp);
                    text.Append(0x202A);
                    text.Append(formatterNumber);
                    text.Append(0x202C);
                    CleanupStack::PopAndDestroy(formattedPhoneNumber);
                    }
                else
                    {
                    return  fieldData->Text();
                    }
                }
            else if (IsFieldTypeL(*aFieldType, R_MESSAGING_SYNCRONIZATION_SELECTOR))
                {
                // syncronization field
                if (!fieldText.CompareF(KVPbkContactSyncPublic))
                    {
                    HBufC* textBuffer = CCoeEnv::Static()->AllocReadResourceLC(
                            R_QTN_CALE_CONFIDENT_PUBLIC);
                    TPtr localizedSyncText = textBuffer->Des();
                    text.Set(ReallocBufferL(localizedSyncText.Length()));
                    text.Copy(localizedSyncText);
                    CleanupStack::PopAndDestroy(textBuffer);
                    }
                else if (!fieldText.CompareF(KVPbkContactSyncNoSync))
                    {
                    HBufC* textBuffer = CCoeEnv::Static()->AllocReadResourceLC(
                            R_QTN_CALE_CONFIDENT_NONE);
                    TPtr localizedSyncText = textBuffer->Des();
                    text.Set(ReallocBufferL(localizedSyncText.Length()));
                    text.Copy(localizedSyncText);
                    CleanupStack::PopAndDestroy(textBuffer);
                    }
                else
                    {
                    // otherwise sync setting is the default value - private
                    HBufC* textBuffer = CCoeEnv::Static()->AllocReadResourceLC(
                            R_QTN_CALE_CONFIDENT_PRIVATE);
                    TPtr localizedSyncText = textBuffer->Des();
                    text.Set(ReallocBufferL(localizedSyncText.Length()));
                    text.Copy(localizedSyncText);
                    CleanupStack::PopAndDestroy(textBuffer);
                    }
                }
            else
                {
                text.Set(ReallocBufferL(fieldText.Length()));
                text.Copy(fieldText);

                // Check is there need to display the digits in the
                // text with foreign characters
                // Perform number conversion if allowed
                if (IsNumberConversionAllowed(*aFieldType))
                    {
                    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
                            text);
                    }
                }
            break;
            }
        case EVPbkFieldStorageTypeDateTime:
            {
            TInt error = KErrNone;
            do
                {
                HBufC* timeFormat = CCoeEnv::Static()->AllocReadResourceLC(R_QTN_DATE_USUAL);
                const MVPbkContactFieldDateTimeData* fieldData =
                        &MVPbkContactFieldDateTimeData::Cast(aField.FieldData());
                TRAPD(error, fieldData->DateTime().FormatL(text, *timeFormat));
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion(text);
                CleanupStack::PopAndDestroy(timeFormat);  // timeFormat
                if (error == KErrOverflow)
                    {
                    // allocate bigger buffer for formatting text
                    text.Set(ReallocBufferL(2*text.MaxLength()));
                    }
                else if (error)
                    {
                    // rethrow other errors
                    User::Leave(error);
                    }
                } while (error == KErrOverflow);
            break;
            }
        default:
            {
            text.Zero();
            break;
            }
        }

    return text;
    }

/**
 * Checks the field type.
 */
TBool CVCardVpbUtil::IsFieldTypeL(
        const MVPbkFieldType& aFieldType,
        TInt aResourceId) const
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC(reader, aResourceId);

    CVPbkFieldTypeSelector* selector = CVPbkFieldTypeSelector::NewL(
            reader, iContactManager->FieldTypes());
    // Check if the field type is the one needed
    TBool ret = selector->IsFieldTypeIncluded(aFieldType);
    CleanupStack::PopAndDestroy(); // resource buffer
    delete selector;
    return ret;
    }

TPtr CVCardVpbUtil::ReallocBufferL
        (TInt aNewSize)
    {
    const TInt currMaxLength = iBuffer->Des().MaxLength();
    if (aNewSize > currMaxLength)
        {
        iBuffer = iBuffer->ReAllocL(aNewSize);
        }

    //PostCond:
    __ASSERT_DEBUG(iBuffer->Des().MaxLength() >= aNewSize,
        Panic(EPanicPostCond_ReallocBufferL));

    return iBuffer->Des();
    }

void CVCardVpbUtil::Panic( TInt aReason )
    {
    User::Panic( KClassName, aReason );
    }
/**
 * Check whether default store is Full or Not.
 */
TBool CVCardVpbUtil::StoreFullL()
{
    LOG("CVCardVpbUtil::StoreFullL");
    TBool ret( EFalse );
    const MVPbkContactStore* iTargetStore;
    HBufC* textBuffer;
    CPbk2StoreConfiguration* storeConfiguration = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL( storeConfiguration );
    TVPbkContactStoreUriPtr defaultsavingstore = storeConfiguration->DefaultSavingStoreL();
    iTargetStore = iContactManager->ContactStoresL().Find(defaultsavingstore );    
    const MVPbkContactStoreInfo& storeInfo = iTargetStore->StoreInfo();    
    if ( storeInfo.MaxNumberOfContactsL() != KVPbkStoreInfoUnlimitedNumber &&
    storeInfo.MaxNumberOfContactsL() <= storeInfo.NumberOfContactsL() )
        { 
        CVPbkContactStoreUri* uri = CVPbkContactStoreUri::NewL( defaultsavingstore );
        CleanupStack::PushL( uri ); 
        if(uri->Uri().UriDes().CompareC(KDEFAULT_CNTDB_URI) == 0 )
            {
            LOG("CVCardVpbUtil::StoreFullL, Phone memory full");            
            textBuffer = StringLoader::LoadLC( R_QTN_PHOB_PB_INFO_MEMORY_PHONE );
            }            
        else
            {
            LOG("CVCardVpbUtil::StoreFullL, SIM memory full");
            textBuffer = StringLoader::LoadLC( R_QTN_PHOB_PB_INFO_MEMORY_SIM );            
            }            
        ShowNoteL( R_QTN_PHOB_NOTE_STORE_FULL, textBuffer->Des() ); 
        CleanupStack::PopAndDestroy(textBuffer);   
        CleanupStack::PopAndDestroy();//destroy uri
        ret = ETrue;
        }
    CleanupStack::PopAndDestroy( storeConfiguration );
    return ret;
}

/**
 * Shows popup Note.
 */
void CVCardVpbUtil::ShowNoteL( TInt aResourceId, const TDesC& aString )
{
    LOG("CVCardVpbUtil::ShowNoteL");
    HBufC* prompt = NULL;    
    if( aString.Length() )        
        prompt = StringLoader::LoadLC( aResourceId, aString );        
    else        
        prompt = StringLoader::LoadLC( aResourceId );        
    CAknInformationNote* dlg = new(ELeave) CAknInformationNote(ETrue);
    dlg->ExecuteLD(*prompt);
    CleanupStack::PopAndDestroy(); // prompt
}

