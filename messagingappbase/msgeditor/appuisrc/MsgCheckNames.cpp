/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Alias name matching functionality.
*
*/



// INCLUDE FILES
#include "MsgCheckNames.h"

#include <eiklbv.h>
#include <aknlistquerydialog.h>
#include <CPbkContactEngine.h>       // CPbkContactEngine
#include <CPbkIdleFinder.h>          // CPbkIdleFinder
#include <CPbkContactItem.h>         // Phonebook Contact
#include <cntdef.h>                  //CheckNames
#include <CPbkSmsAddressSelect.h>    //CheckNames
#include <CPbkEmailAddressSelect.h>  //CheckNames
#include <CPbkMmsAddressSelect.h>    //CheckNames
#include <MsgAddressControl.h>       // CMsgRecipientArray/CheckNames

#include <MsgEditorAppUi.rsg>        // resource identifiers

#include <VPbkContactStoreUris.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkStoreContact.h>
#include <RVPbkContactFieldDefaultPriorities.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CPbk2StoreConfiguration.h>
#include <MVPbkContactLink.h>
#include <CVPbkFieldTypeRefsList.h>
#include <CVPbkContactStoreUriArray.h>

#include <CVPbkFieldTypeSelector.h>
#include <VPbkContactViewFilterBuilder.h>
#include <AiwContactSelectionDataTypes.h>

#include <TVPbkWordParserCallbackParam.h>
#include <AiwContactSelectionDataTypes.h>
#include <RVPbkContactFieldDefaultPriorities.h>

#include <featmgr.h>

// MACROS
_LIT( KItemStart, "1\t" );    //For creation of checkbox list

const TUint KLatinCharEnd = 0xFF;

_LIT( KPanicText, "Check names -panic" );


// ============================= LOCAL FUNCTIONS ===============================

// ---------------------------------------------------------
// FindWordSplitterL
// ---------------------------------------------------------
//
TInt FindWordSplitterL( TAny* aParams )
	{
	TVPbkWordParserCallbackParam* parser = 
        static_cast<TVPbkWordParserCallbackParam*>( aParams );
        
    const TText* ptr = parser->iStringToParse->Ptr();
	const TText* end = ptr + parser->iStringToParse->Length();

	const TText* startOfWord=NULL;
	FOREVER
		{
		if ( ptr==end || TChar(*ptr).IsSpace() )
			{
			if ( startOfWord )
				{
				TPtrC addWord( startOfWord,ptr - startOfWord );
				parser->iWordArray->AppendL( addWord );
				startOfWord = NULL;
				}
			if ( ptr == end )
                {
				break;
                }
			}
		else if ( !startOfWord )
            {
			startOfWord = ptr;
            }
		ptr++;
		}
	return( KErrNone );
	}



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgCheckNames::CMsgCheckNames
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgCheckNames::CMsgCheckNames()
    {
    }

// -----------------------------------------------------------------------------
// CMsgCheckNames::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgCheckNames::ConstructL()
    {
    iFoundContacts = CVPbkContactLinkArray::NewL();
  
    iStoreConfiguration = CPbk2StoreConfiguration::NewL();
    
    CVPbkContactStoreUriArray* storeArray = iStoreConfiguration->SearchStoreConfigurationL();
    CleanupStack::PushL( storeArray );
           
    iCommonUtils = CContactMatcher::NewL( &CCoeEnv::Static()->FsSession() );//parameter for RFs&
    iCommonUtils->OpenStoreL( *storeArray );
    iServiceHandler = CAiwServiceHandler::NewL();
    iServiceHandler->AttachL ( R_CHECKNAMES_FETCH_INTEREST );
    
    iStoreConfiguration->AddObserverL( *this );
    
    CleanupStack::PopAndDestroy( storeArray );
    
    FeatureManager::InitializeLibL();
    
    if ( FeatureManager::FeatureSupported( KFeatureIdChinese ) )
        {
        iUseAdvancedMatch = ETrue;
        }
    
    FeatureManager::UnInitializeLib();
    }


// -----------------------------------------------------------------------------
// CMsgCheckNames::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMsgCheckNames* CMsgCheckNames::NewL()
    {
    CMsgCheckNames* self = CMsgCheckNames::NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CMsgCheckNames::NewLC
//
// Factory method
// -----------------------------------------------------------------------------
//
EXPORT_C CMsgCheckNames* CMsgCheckNames::NewLC()
    {
	CMsgCheckNames* self = new (ELeave) CMsgCheckNames();	
	CleanupStack::PushL(self);

	self->ConstructL();
	
	return self;
    }
    
// -----------------------------------------------------------------------------
// CMsgCheckNames::~CMsgCheckNames
// -----------------------------------------------------------------------------
//
EXPORT_C CMsgCheckNames::~CMsgCheckNames()
    {
    delete iFoundContacts;
    delete iCommonUtils;
    
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();         
        }
        
    delete iServiceHandler;
    delete iStoreConfiguration;
    }

// -----------------------------------------------------------------------------
// CMsgCheckNames::FindAndCheckByNameL
//
// Finds the contact items that match the given search string, checks if they
// are valid and if there are more than one valid matches shows them in a dialog
// where user can select one or more of the contacts. If shudown flag is set 
// leaves with KLeaveExit code to make the application to shutdown
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMsgCheckNames::FindAndCheckByNameL( const TDesC& aText, 
                                                    TMsgAddressSelectType aAddressSelectType,
                                                    CMsgRecipientArray& aRecipientArray, 
                                                    TInt aIndex )
    {
    TBool returnValue = EFalse;
    
    TAiwAddressSelectType addressType = ConvertToPbk2Type( aAddressSelectType );
    
    CVPbkFieldTypeRefsList* fieldTypes = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( fieldTypes );

    DefineSearchFieldsL( *fieldTypes ); 

    iFoundContacts->ResetAndDestroy();

    //search the given string from the fields
    CDesCArray* wordArray = SplitFindStringL( aText );
    CleanupStack::PushL( wordArray );
    TCallBack findParser( FindWordSplitterL );
    
    iCommonUtils->MatchDataL( *wordArray, *fieldTypes, *iFoundContacts, findParser );
        
    CleanupStack::PopAndDestroy( wordArray );
    
    // If simple matching did not find any result and advanced flag is
    // on perform advanced matching.
    if ( iUseAdvancedMatch &&
         iFoundContacts &&
         iFoundContacts->Count() == 0 &&
         UseAdvancedMatch( aText ) )
        {
        PerformAdvancedMatchL( aText, fieldTypes );
        }
    
    TInt contactAmount = iFoundContacts->Count();

    // Offer the user a list of matching contacts
    if ( contactAmount > 1 )
        {                         
        CDesCArray* items = new ( ELeave ) CDesCArrayFlat(4);
        CleanupStack::PushL( items );
       
        CVPbkContactLinkArray* helperArray =  CVPbkContactLinkArray::NewL();
        CleanupStack::PushL( helperArray );

        //go through every contact, search the names for them and insert into items array
        for ( TInt i = 0; i<contactAmount; i++ )
            {
            MVPbkStoreContact* storeContact = NULL;
            iCommonUtils->GetStoreContactL( iFoundContacts->At(i), &storeContact ); 
            storeContact->PushL();
            
            if ( KErrNotFound != iCommonUtils->ContactHasFieldOfTypeL( addressType, *storeContact ) ) 
                {
                MVPbkStoreContactFieldCollection& fieldCollection = storeContact->Fields();
                
                HBufC* contactName = iCommonUtils->GetNameL( fieldCollection );
                if ( contactName )
                    {
                    contactName = contactName->ReAllocL( contactName->Des().Length()+2 );
                    CleanupStack::PushL( contactName );
                
                    TPtr contactNamePtr = contactName->Des();

                    /*
                     * Fix the split contact name issue::ECYU-7FPC93.
                     * Replace any enter-key characters with space characters.
                     * Start
                     */
                    
                    TInt enterKeyPos;
                    TChar enterChar( CEditableText::EParagraphDelimiter );
                    TChar spaceChar( CEditableText::ESpace );
                    TInt nameLength = contactNamePtr.Length();
                    
                    enterKeyPos = contactNamePtr.Locate(enterChar);
                    while( enterKeyPos != KErrNotFound && enterKeyPos < nameLength )
                        {
                        contactNamePtr[enterKeyPos] = spaceChar;
                        enterKeyPos = contactNamePtr.Locate(enterChar);
                        }
                    // Fix: End
                    // We need to add the checkbox
                    contactNamePtr.Insert( 0, KItemStart );
            
                    // Pay attention to the order of the items
                    TInt pos = items->InsertIsqAllowDuplicatesL( contactNamePtr, ECmpCollated ); 
                    CleanupStack::PopAndDestroy( contactName );
                
                    const MVPbkContactLink& preservedLink = iFoundContacts->At( i );
                
                    MVPbkContactLink* link = preservedLink.CloneLC();
                    helperArray->InsertL( link, pos );
                
                    CleanupStack::Pop();
                    }
                }
            
            CleanupStack::PopAndDestroy( storeContact );
            }
            
        iFoundContacts->ResetAndDestroy();
        
        if ( items->Count() )
            {
            CListBoxView::CSelectionIndexArray* matchingRecipients = 
                            new ( ELeave ) CArrayFixFlat<TInt>( contactAmount );
            CleanupStack::PushL( matchingRecipients );
            
            CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog( matchingRecipients );
            dlg->PrepareLC( R_MEB_CHECKNAMES_QUERY );
            dlg->SetItemTextArray( items );
            dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );

            if ( dlg->RunLD() )
                {
                // We must now read which one / ones of the contacts the user selected
                const TInt recipientCount = matchingRecipients->Count();
                MVPbkContactLink* link = NULL;
                
                for ( TInt i = 0; i < recipientCount; ++i )
                    {
                    // Copying entries
                    link = helperArray->At( matchingRecipients->At( i ) ).CloneLC();
                    
                    // Inserts entry, shifts original items
                    iFoundContacts->InsertL( link, i );
                    CleanupStack::Pop();
                    } 
                }
            CleanupStack::PopAndDestroy(); // matchingRecipients
            }
        CleanupStack::PopAndDestroy( 2, items);//helperArray
        }

    CleanupStack::PopAndDestroy( fieldTypes ); // fieldtypes
    
    if ( iFoundContacts->Count() > 0 )  
        {
        ExecuteSingleItemFetchL( 
                TAiwSingleItemSelectionDataV1().SetAddressSelectType( addressType ) );
        
        CAknInputBlock* inputBlock = CAknInputBlock::NewCancelHandlerLC( this );
        
        iWait.Start(); // Make this whole op look like synchronous
        
        CleanupStack::PopAndDestroy( inputBlock );
        
        if ( iShutdown )
            {
            User::Leave( KLeaveExit );
            }
        
        if ( iFoundContacts->Count() > 0 )//the process might have been cancelled
            {
            PutContactsToArrayL( aRecipientArray, aIndex );
            returnValue = ETrue;    
            }
        }
        
    //return false if no contacts        
    return returnValue;
    }

// ----------------------------------------------------------------------------
// CMsgCheckNames::FetchRecipientsL
//
// Fetches the recipients, and searches the address from the selected recipients
// Starts input blocker to "eat" all key press events to editor during
// phonebook operations. If shudown flag is set leaves with KLeaveExit code
// to make the application to shutdown
// ----------------------------------------------------------------------------    
//
EXPORT_C void CMsgCheckNames::FetchRecipientsL( CMsgRecipientArray& aRecipientList,
                                                TMsgAddressSelectType aAddressSelectType )
    {
    TAiwAddressSelectType addressType = ConvertToPbk2Type( aAddressSelectType );
    
    // Construct empty filter
    CVPbkFieldTypeSelector* contactViewFilter =
        CVPbkFieldTypeSelector::NewL( iCommonUtils->FieldTypes() );
    CleanupStack::PushL( contactViewFilter );

    // Append the filter object with suitable criteria            
    if ( addressType ==  EAiwEMailSelect )            
        {
        VPbkContactViewFilterBuilder::BuildContactViewFilterL( *contactViewFilter, 
                                                               EVPbkContactViewFilterEmail,
                                                               iCommonUtils->GetContactManager() );
        }
            
    else if ( addressType ==  EAiwMMSSelect )
        {
        VPbkContactViewFilterBuilder::BuildContactViewFilterL( *contactViewFilter, 
                                                               EVPbkContactViewFilterPhoneNumber,
                                                               iCommonUtils->GetContactManager() );
                    
        VPbkContactViewFilterBuilder::BuildContactViewFilterL( *contactViewFilter, 
                                                               EVPbkContactViewFilterEmail,
                                                               iCommonUtils->GetContactManager() );
        }
            
    else
        {
        VPbkContactViewFilterBuilder::BuildContactViewFilterL( *contactViewFilter, 
                                                               EVPbkContactViewFilterPhoneNumber,
                                                               iCommonUtils->GetContactManager() );
        }
    
    iFoundContacts->ResetAndDestroy();
    
    TAiwMultipleItemSelectionDataV1 selectionData;
    selectionData.SetFetchFilter( contactViewFilter );
    selectionData.SetAddressSelectType( addressType );
            
    ExecuteMultipleItemFetchL( selectionData );
    
    CleanupStack::PopAndDestroy( contactViewFilter );
    
    CAknInputBlock* inputBlock = CAknInputBlock::NewCancelHandlerLC( this );
    
    iWait.Start();
    
    if ( iFoundContacts->Count() > 0 )
        {
        // The process might have been cancelled in which case there
        // will not be any contacts.
        PutContactsToArrayL( aRecipientList, -1 );
        }
    
    CleanupStack::PopAndDestroy( inputBlock );    
    
    if ( iShutdown )
        {
        User::Leave( KLeaveExit );
        }
    }

// ----------------------------------------------------------------------------
// CMsgCheckNames::FetchRecipientsL
//
// Fetches the recipients. If shudown flag is set leaves with KLeaveExit code
// to make the application to shutdown
// ----------------------------------------------------------------------------    
//
EXPORT_C void CMsgCheckNames::FetchRecipientsL( CVPbkContactLinkArray& aContactLinkArray,
                                                TMsgAddressSelectType /*aAddressSelectType*/ )
    {
    ExecuteMultipleEntryFetchL( TAiwMultipleEntrySelectionDataV2().SetFetchFilter( NULL ).SetFlags( 0 ) );
    
    CAknInputBlock* inputBlock = CAknInputBlock::NewCancelHandlerLC( this );
    
    iWait.Start();  // Wait for the user to check off the names.
    
    CleanupStack::PopAndDestroy( inputBlock );
    
    if ( iShutdown )
        {
        User::Leave( KLeaveExit );
        }
    
    if ( iFoundContacts->Count() )
        {
        for ( TInt n = 0; n < iFoundContacts->Count(); n++ )
            {
            aContactLinkArray.AppendL( iFoundContacts->At( n ).CloneLC() );
            CleanupStack::Pop();    // Cloned link.
            }
        }
    }
    
// ----------------------------------------------------
// CMsgCheckNames::GetAlias
// ----------------------------------------------------
//
 
EXPORT_C void CMsgCheckNames::GetAliasL(const TDesC& aNumber,TDes& aAlias,TInt aMaxLength)
    {                           
         // Match contacts to the from address
        CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC();
        iCommonUtils->MatchPhoneNumberL(aNumber,7,CVPbkPhoneNumberMatchStrategy::EVPbkStopOnFirstMatchFlag,*linkArray );
        TInt tmpcnt = linkArray->Count();
        if ( linkArray->Count() > 0 )
           {
            MVPbkStoreContact* contact;
            iCommonUtils->GetStoreContactL(linkArray->At(0), &contact);
            contact->PushL();
         
            MVPbkStoreContactFieldCollection& coll = contact->Fields();       
            HBufC* alias = iCommonUtils->GetNameL( coll );                        
            
            if ( alias )
                {
                 if ( alias->Length() > aMaxLength )
                    {
                    aAlias = alias->Left( aMaxLength );
                    }        
                else
                    {
                    aAlias = *alias;
                    }
                delete alias;
                alias = NULL;
                }                           
            CleanupStack::PopAndDestroy( contact ); // contact
            }
        else            
           {
           aAlias = KNullDesC();
           }
        CleanupStack::PopAndDestroy( 1); //  linkArray, ,        
    }

// ----------------------------------------------------------------------------
// CMsgCheckNames::HandleNotifyL
// ----------------------------------------------------------------------------
//
TInt CMsgCheckNames::HandleNotifyL( TInt aCmdId,
                                    TInt aEventId,
                                    CAiwGenericParamList& aEventParamList,
                                    const CAiwGenericParamList& /*aInParamList*/ )
    {
    // the trapd is to enable stopping the iWait in case of a leave
    // trapping a lot of code to make future extensions easier
    __ASSERT_ALWAYS( iFoundContacts, User::Panic(KPanicText,0) );
    
    TInt retVal = KErrNone;
    
    TRAPD( err, 
        {
        iFoundContacts->ResetAndDestroy();
        if ( aEventId == KAiwEventCompleted )
            {
            TInt index = 0;
            const TAiwGenericParam* param =
                aEventParamList.FindFirst( index, EGenericParamContactLinkArray );
            if ( param )
                {
                TPtrC8 contactLinks = param->Value().AsData();
                if ( contactLinks.Length() ) // If the event was cancelled
                    {
                    CVPbkContactLinkArray* links = 
                            CVPbkContactLinkArray::NewLC( contactLinks, 
                                                          iCommonUtils->GetContactStoresL() );
                    CleanupStack::Pop(); //links
                    
                    // Replace iFoundContacts
                    delete iFoundContacts;
                    iFoundContacts = links;                    
                    }
                else
                    {
                    retVal = KErrArgument;    
                    }
                }
            else 
                {
                retVal = KErrArgument;
                }  
            }
        else if ( aEventId == KAiwEventOutParamCheck )
            {
            if ( aCmdId == KAiwCmdSelect )
                {
                retVal = 1;//this value going back to aiw-component to inform param to be ok
                }
            }

        else 
            {
            retVal = KErrCompletion;
            }
            
        if ( retVal != 1 && iWait.IsStarted() )
            {
            iWait.AsyncStop();         
            }
        } );

    if ( err != KErrNone )
        {
        if ( iWait.IsStarted() )
            {
            iWait.AsyncStop();         
            }
        User::Leave( err );
        }
        
    return retVal;        
    }                    
    

// ----------------------------------------------------
// CMsgCheckNames::ExecuteMultipleEntryFetchL
//
// Uses aiw to fetch contact(s) from vpbk
// ----------------------------------------------------
//   
void CMsgCheckNames::ExecuteMultipleEntryFetchL( TAiwMultipleEntrySelectionDataV2 aData )
    {
    CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    inParamList.AppendL( TAiwGenericParam( 
                                EGenericParamContactSelectionData,
                                TAiwVariant( TAiwMultipleEntrySelectionDataV2Pckg( aData ) ) ) );
            
    iServiceHandler->ExecuteServiceCmdL( KAiwCmdSelect, 
                                         inParamList, 
                                         iServiceHandler->OutParamListL(),
                                         0,
                                         this );            
    }

// ----------------------------------------------------------------------------
// CMsgCheckNames::ExecuteSingleItemFetchL
// ----------------------------------------------------------------------------
//
void CMsgCheckNames::ExecuteSingleItemFetchL( TAiwSingleItemSelectionDataV1 aData )
    {
    RVPbkContactFieldDefaultPriorities defaultPriorities;
    CleanupClosePushL( defaultPriorities );    
    
    SetDefaultPrioritiesL( aData.AddressSelectType(), defaultPriorities );
    
    aData.SetDefaultPriorities( defaultPriorities );
        
    CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();    
    inParamList.AppendL( TAiwGenericParam( 
                                EGenericParamContactSelectionData,
                                TAiwVariant( TAiwSingleItemSelectionDataV1Pckg( aData ) ) ) );

    // the validity of iFoundContacts is checkeck by the calling function
    HBufC8* packedLinks = iFoundContacts->PackLC();
        
    inParamList.AppendL( TAiwGenericParam( EGenericParamContactLinkArray,
                                           TAiwVariant( *packedLinks ) ) );

    iServiceHandler->ExecuteServiceCmdL( KAiwCmdSelect, 
                                         inParamList, 
                                         iServiceHandler->OutParamListL(),
                                         0,
                                         this );
    CleanupStack::PopAndDestroy( 2, &defaultPriorities ); 
    }

// ----------------------------------------------------------------------------
// CMsgCheckNames::ExecuteMultipleItemFetchL
// ----------------------------------------------------------------------------
//
void CMsgCheckNames::ExecuteMultipleItemFetchL( TAiwMultipleItemSelectionDataV1& aData )
    {
    RVPbkContactFieldDefaultPriorities defaultPriorities;
    CleanupClosePushL( defaultPriorities );    
    
    SetDefaultPrioritiesL( aData.AddressSelectType(), defaultPriorities );
    
    aData.SetDefaultPriorities( defaultPriorities );
        
    CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();    
    inParamList.AppendL( TAiwGenericParam( 
                                EGenericParamContactSelectionData,
                                TAiwVariant( TAiwMultipleItemSelectionDataV1Pckg( aData ) ) ) );

    // the validity of iFoundContacts is checkeck by the calling function
    HBufC8* packedLinks = iFoundContacts->PackLC();
        
    inParamList.AppendL( TAiwGenericParam( EGenericParamContactLinkArray,
                                           TAiwVariant( *packedLinks ) ) );

    iServiceHandler->ExecuteServiceCmdL( KAiwCmdSelect, 
                                         inParamList, 
                                         iServiceHandler->OutParamListL(),
                                         0,
                                         this );
    CleanupStack::PopAndDestroy( 2, &defaultPriorities ); 
    }
    
// ----------------------------------------------------------------------------
// CMsgCheckNames::PutContactsToArrayL
// ----------------------------------------------------------------------------
//
void CMsgCheckNames::PutContactsToArrayL( CMsgRecipientArray& aRecipientArray, TInt aIndex )
    {
    __ASSERT_ALWAYS( iFoundContacts, User::Panic(KPanicText,0) );
    //delete the original entry, if there was one        
    if ( aIndex > -1 && 
         aIndex < aRecipientArray.Count() )
        {
        delete aRecipientArray.At( aIndex );
        aRecipientArray.Delete( aIndex );   
        }
    else
        {
        aIndex = 0;
        }
        
    MVPbkStoreContact* storeContact = NULL;
    MVPbkStoreContactField* selectedField = NULL;
    TInt recipientsNotHandled = iFoundContacts->Count();
       
    for ( TInt i = 0; recipientsNotHandled > i; i++ )
        {
        const MVPbkContactLink& changedContactLink = iFoundContacts->At( i );
        iCommonUtils->GetStoreContactL( changedContactLink, &storeContact ); 
        storeContact->PushL();
        MVPbkStoreContactFieldCollection& fieldCollection = storeContact->Fields();  

        selectedField = fieldCollection.RetrieveField( iFoundContacts->At(i) );

        if ( selectedField )//gets the selected field and writes it to the reciItem
            {
            CMsgRecipientItem* reciItem = CMsgRecipientItem::NewLC( KNullDesC, KNullDesC ); 
            
            // Set field data
            const MVPbkContactFieldTextData* textData =
                &MVPbkContactFieldTextData::Cast( selectedField->FieldData() );
                
            reciItem->SetAddressL( textData->Text() );
            
            HBufC* contactName = iCommonUtils->GetNameL( fieldCollection );
            if ( contactName )
                {
                reciItem->SetNameL( contactName->Des() );
                delete contactName;
                }
                
            reciItem->SetContactLink( &changedContactLink );
            aRecipientArray.InsertL( aIndex, reciItem );
            
            CleanupStack::Pop( reciItem );
            aIndex++;
            }
        CleanupStack::PopAndDestroy( storeContact );
        }
    }

// -----------------------------------------------------------------------------
// CMsgCheckNames::SplitFindStringL
//
// Divides search string into several pieces if it includes spaces.
// -----------------------------------------------------------------------------
//
CDesCArray* CMsgCheckNames::SplitFindStringL(const TDesC& aFindString)
    {
	CDesCArray* wordArray = new ( ELeave ) CDesCArrayFlat( 10 );
	CleanupStack::PushL( wordArray );	
	
    TVPbkWordParserCallbackParam parser( &aFindString, wordArray );
	FindWordSplitterL( &parser );
	
	CleanupStack::Pop(); // wordArray
	return parser.iWordArray;
    }
    
// -----------------------------------------------------------------------------
// CMsgCheckNames::IdleFindCallback
//
// Asynchronous find observer callback.
// -----------------------------------------------------------------------------
//
void CMsgCheckNames::IdleFindCallback()
    {
    }

// -----------------------------------------------------------------------------
// CMsgCheckNames::DefineSearchFieldsL
//
// Defines that from which fiels the data is searched from 
// -----------------------------------------------------------------------------
//
void CMsgCheckNames::DefineSearchFieldsL( CVPbkFieldTypeRefsList& aOwnList )    
    {
    const MVPbkFieldTypeList& refList = iCommonUtils->FieldTypes();
    TVPbkFieldVersitProperty prop;
    prop.SetName( EVPbkVersitNameN ); 
    prop.SetSubField( EVPbkVersitSubFieldFamilyName ); 
    const MVPbkFieldType* foundType = refList.FindMatch( prop, 0 );
    
    if ( foundType )
        {
        aOwnList.AppendL( *foundType );   
        }
    
    prop.SetSubField( EVPbkVersitSubFieldGivenName );
    
    foundType = refList.FindMatch( prop, 0 );
    if ( foundType )
        {
        aOwnList.AppendL( *foundType );   
        }
    
    prop.SetName( EVPbkVersitNameORG );
    prop.SetSubField( EVPbkVersitSubFieldOrgName );
    
    foundType = refList.FindMatch( prop, 0 );
    if ( foundType )
        {
        aOwnList.AppendL( *foundType );   
        }
    }

// -----------------------------------------------------------------------------
// CMsgCheckNames::ConvertToPbk2Type
//
// Defines that from which fiels the data is searched from 
// -----------------------------------------------------------------------------
//
TAiwAddressSelectType CMsgCheckNames::ConvertToPbk2Type( 
                                        TMsgAddressSelectType aAddressSelectType )
   
    {
    TAiwAddressSelectType addressType = EAiwPhoneNumberSelect;
    
    switch ( aAddressSelectType )
        {
        case EMsgTypeSms:
            {
            addressType = EAiwPhoneNumberSelect;
            break;
            }
        case EMsgTypeMms:
            {
            addressType = EAiwMMSSelect;
            break;
            }
        case EMsgTypeMail:
            {
            addressType = EAiwEMailSelect;
            break;
            }
        case EMsgTypeAll:
            {
            addressType = EAiwAllItemsSelect;
            break;
            }
        default:
            {
            addressType = EAiwPhoneNumberSelect;
            break;
            }
        }
        
    return addressType;
    }
              
// -----------------------------------------------------------------------------
// CMsgCheckNames::SetDefaultPrioritiesL
// -----------------------------------------------------------------------------
//
void CMsgCheckNames::SetDefaultPrioritiesL( TAiwAddressSelectType aAddressType, 
                                            RVPbkContactFieldDefaultPriorities& aPriorities ) const
    {
    switch ( aAddressType )
        {
        case EAiwMMSSelect:
            {
            User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeMms ) );
            User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeSms ) );
            User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeEmailOverSms ) );
            break;
            }
        case EAiwPhoneNumberSelect:
            {
            User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeSms ) );
            break;
            }
        case EAiwEMailSelect:
            {
            User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeEmail ) );
            break;
            }
        case EAiwAllItemsSelect:
            {
            User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeMms ) );
            User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeSms ) );
            User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeEmail ) );
            User::LeaveIfError( aPriorities.Append( EVPbkDefaultTypeEmailOverSms ) );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMsgCheckNames::UseAdvancedMatch
//
// If string does not contain spaces and contains only non-latin characters
// perform matching advanced match algorithm.
// -----------------------------------------------------------------------------
//
TBool CMsgCheckNames::UseAdvancedMatch( const TDesC& aFindString )
    {
    TBool containsSpace( EFalse );
    TBool nonLatinString( ETrue );
    
    for ( TInt current = 0; current < aFindString.Length(); current++ )
        {
        TUint currentChar( aFindString[current] );
        if ( TChar( currentChar ).IsSpace() )
            {
            containsSpace = ETrue;
            }
            
        if ( currentChar < KLatinCharEnd )
            {
            nonLatinString = EFalse;
            break;
            }
        }
    
    return !containsSpace && nonLatinString;
    }

// -----------------------------------------------------------------------------
// CMsgCheckNames::PerformAdvancedMatchL
//
// Perform matching by trying to set space between every character. 
// This is done because in written chinese it is common to write contact names without spaces and
// symbian's contact DB requires search text as a word list.
// -----------------------------------------------------------------------------
//
void CMsgCheckNames::PerformAdvancedMatchL( const TDesC& aFindString,
                                            CVPbkFieldTypeRefsList* aFieldTypes )
    {
    HBufC* firstBuffer = HBufC::NewLC( aFindString.Length() );
    HBufC* secondBuffer = HBufC::NewLC( aFindString.Length() );
    
    TPtr firstBufferPtr = firstBuffer->Des();
    TPtr secondBufferPtr = secondBuffer->Des();
    
    CDesCArray* wordArray = new( ELeave ) CDesCArrayFlat( 2 );
	CleanupStack::PushL( wordArray );
    
    TCallBack findParser( FindWordSplitterL );
    
    TInt count( Min( aFindString.Length(), 4) );
    
    for ( TInt index = 1; index < count; index++ )
        {
        firstBufferPtr.Insert( 0, aFindString.Mid( 0, index ) );
        secondBufferPtr.Insert( 0, aFindString.Mid( index, 
                                                    aFindString.Length() - index ) );
        
        wordArray->AppendL( firstBufferPtr );
        wordArray->AppendL( secondBufferPtr );
        
        iCommonUtils->MatchDataL( *wordArray, 
                                  *aFieldTypes, 
                                  *iFoundContacts, 
                                  findParser );
        
        wordArray->Reset();
        
        if ( iFoundContacts->Count() != 0 )
            {
            break;
            }
            
        firstBufferPtr.Zero();
        secondBufferPtr.Zero();                
        }                
    
    CleanupStack::PopAndDestroy( wordArray );
    
    CleanupStack::PopAndDestroy( secondBuffer );
    CleanupStack::PopAndDestroy( firstBuffer );
    }

// -----------------------------------------------------------------------------
// CMsgCheckNames::AknInputBlockCancel
//
// Cancels the AIW service and sets the shutdown flag. This is needed since
// we must wait for the AIW service to notify us about the cancel before
// application can be shutdown. Otherwise AIW service might try to use
// freed resources.
// -----------------------------------------------------------------------------
//
void CMsgCheckNames::AknInputBlockCancel()
    {
    if ( iWait.IsStarted() )
        {
        TRAPD( error, iServiceHandler->ExecuteServiceCmdL( KAiwCmdSelect, 
                                                           iServiceHandler->InParamListL(), 
                                                           iServiceHandler->OutParamListL(),
                                                           KAiwOptCancel,
                                                           this ) );
        if ( error == KErrNone )
            {
            iShutdown = ETrue;
            }
        }
    }

// ---------------------------------------------------------
// CMsgCheckNames::CleanupResetAndDestroy
// ---------------------------------------------------------
//
void CMsgCheckNames::CleanupResetAndDestroy( TAny* aMatchedRecips )
	{
	CMsgRecipientArray* matches = NULL;
    
    matches = STATIC_CAST( CMsgRecipientArray*, aMatchedRecips );
	matches->ResetAndDestroy();
	
	delete matches;
    matches = NULL;
	}

// ---------------------------------------------------------
// CMsgCheckNames::ConfigurationChanged
// ---------------------------------------------------------
//
void CMsgCheckNames::ConfigurationChanged()
    {
    }

// ---------------------------------------------------------
// CMsgCheckNames::ConfigurationChangedComplete
// ---------------------------------------------------------
//
void CMsgCheckNames::ConfigurationChangedComplete()
    {
    TRAP_IGNORE( DoConfigurationChangedCompleteL() );
    }

// ---------------------------------------------------------
// CMsgCheckNames::DoConfigurationChangedCompleteL
//
// If store configuration has changed (i.e. used memories (phone, SIM etc.)).
// open the currently selected stores.
// ---------------------------------------------------------
//
void CMsgCheckNames::DoConfigurationChangedCompleteL()
    {
    CVPbkContactStoreUriArray* storeArray = iStoreConfiguration->SearchStoreConfigurationL();
    CleanupStack::PushL( storeArray );
    
    iCommonUtils->CloseStoresL();     
    iCommonUtils->OpenStoreL( *storeArray );
    
    CleanupStack::PopAndDestroy( storeArray );
    }

//  End of File  
