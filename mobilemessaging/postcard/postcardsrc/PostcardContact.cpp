/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*           Postcard application's interface to contacts (Virtual Phonebook
*           and Phonebook2).
*
*/



// INCLUDE FILES

#include <s32file.h>
#include <aknenv.h>
#include <AiwServiceHandler.h>
#include <AiwContactSelectionDataTypes.h>
#include <CPbk2StoreConfiguration.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactLinkArray.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkStoreContact.h>
#include <contactmatcher.h>
#include <akninputblock.h>

#include <Postcard.rsg>
#include "PostcardContact.h"
#include "PostcardPanic.h"

// ================= Static Constant Data ===================

// Table to convert from TLocation to virtual phonebook field type parameter
const TVPbkFieldTypeParameter CPostcardContact::iLocToFieldTypeParam[] =
    {
    EVPbkVersitParamPREF,
    EVPbkVersitParamHOME,
    EVPbkVersitParamWORK
    };

// Table to convert from TAddressField to virtual phonebook subfield type
const TVPbkSubFieldType CPostcardContact::iAddrFieldToSubFieldType[] =
    {
    EVPbkVersitSubFieldStreet,
    EVPbkVersitSubFieldExtendedAddress,
    EVPbkVersitSubFieldPostalCode,
    EVPbkVersitSubFieldLocality,
    EVPbkVersitSubFieldRegion,
    EVPbkVersitSubFieldCountry
    };

// Table to convert from TPostcardControls to TAddressField
const CPostcardContact::TAddressField CPostcardContact::iControlIdToAddrField[] =
    {
    CPostcardContact::EAddressFieldExtendedAddress,
    CPostcardContact::EAddressFieldStreet,
    CPostcardContact::EAddressFieldPostalCode,
    CPostcardContact::EAddressFieldLocality,
    CPostcardContact::EAddressFieldRegion,
    CPostcardContact::EAddressFieldCountry
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
//  Two-phased constructor
// ---------------------------------------------------------
CPostcardContact* CPostcardContact::NewL( RFs& aFs )
    {
    CPostcardContact* self = new (ELeave) CPostcardContact( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
//  Default destructor
// ---------------------------------------------------------
CPostcardContact::~CPostcardContact()
    {
    delete iStoreContact;
    delete iContactMatcher;
    delete iUriArray;
    delete iAiwServiceHandler;
    }

// ---------------------------------------------------------
// CPostcardContact
// ---------------------------------------------------------
CPostcardContact::CPostcardContact( RFs& aFs ) : iFs( aFs )
    {
    }


// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
void CPostcardContact::ConstructL( )
    {
#ifdef __WINS__
    // Use default contact database store 
    iUriArray = CVPbkContactStoreUriArray::NewL();
    TVPbkContactStoreUriPtr uriPtr(VPbkContactStoreUris::DefaultCntDbUri());
    iUriArray->AppendL(uriPtr);
#else
    // Get store configuration from phonebook2
    CPbk2StoreConfiguration* storeConfiguration =
        CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL(storeConfiguration);

    iUriArray = storeConfiguration->CurrentConfigurationL();
    CleanupStack::PopAndDestroy( storeConfiguration ); // storeConfiguration
#endif

    // Create contact matcher
    iContactMatcher = CContactMatcher::NewL( &iFs );
    iContactMatcher->OpenStoreL(*iUriArray);
    
    iShutdown = EFalse;
    }

// ---------------------------------------------------------
// HasLocationL
// ---------------------------------------------------------
TBool CPostcardContact::HasLocationL(TLocation aLocation) const
    {
    // A small buffer to see if the contact has any fields
    // defined for the location aLocation
    const TInt KBufLen = 8;
    TBuf<KBufLen> contactString;

    GetContactStringL(contactString, aLocation);
    return contactString.Length() > 0;
    }

// ---------------------------------------------------------
// GetContactStringL
// ---------------------------------------------------------
void CPostcardContact::GetContactStringL(TDes& aBuffer,
    TLocation aAddressLocation) const
    {
    static const TAddressField addrFields[] =
        {
        EAddressFieldStreet,
        EAddressFieldExtendedAddress,
        EAddressFieldPostalCode,
        EAddressFieldLocality,
        EAddressFieldRegion,
        EAddressFieldCountry
        };
    const TInt KNumAddrFields = sizeof(addrFields) / sizeof(addrFields[0]);

    _LIT(KPostcardAddressSelectionSeparator, ", ");
    TPtrC fieldSeparator(KPostcardAddressSelectionSeparator);

    aBuffer.Zero();
    TInt bufferRemaining = aBuffer.MaxLength();

    // Read address fields from the contact. Add field to buffer if there
    // is space for the separator and at least one more character.
    for(TInt i = 0; i < KNumAddrFields && bufferRemaining > fieldSeparator.Length(); i++)
        {
        HBufC* fieldText = GetAddressFieldLC(aAddressLocation, addrFields[i]);
        if (fieldText->Length() > 0)
            {
            // Add the separator if this is not the first non-empty field
            if (aBuffer.Length() > 0)
                {
                aBuffer.Append(fieldSeparator);
                bufferRemaining -= fieldSeparator.Length();
                }

            if (bufferRemaining > fieldText->Length())
                {
                aBuffer.Append(*fieldText);
                }
            else
                {
                aBuffer.Append(fieldText->Left(bufferRemaining));
                }
            bufferRemaining = aBuffer.MaxLength() - aBuffer.Length();
            }
        CleanupStack::PopAndDestroy( fieldText );  // fieldText
        }
    }

// ---------------------------------------------------------
// GetAddressFieldLC
// ---------------------------------------------------------
HBufC* CPostcardContact::GetAddressFieldLC(TLocation aLocation,
    TAddressField aAddressField) const
    {
    return GetContactFieldLC(EVPbkVersitNameADR,
        AddrFieldToSubFieldType(aAddressField),
        LocToFieldTypeParam(aLocation));
    }

// ---------------------------------------------------------
// FetchContactL
// ---------------------------------------------------------
void CPostcardContact::FetchContactL()
    {
    // Fetch contact from Phonebook using AIW

    if (!iAiwServiceHandler)
        {
        // Create AIW service handler
        iAiwServiceHandler = CAiwServiceHandler::NewL();
        iAiwServiceHandler->AttachL(R_POSTCARD_SINGLE_ENTRY_FETCH_INTEREST);
        }


    // A new contact is selected. Delete contact in case FetchContactL()
    // was called before.
    delete iStoreContact;
    iStoreContact = NULL;

    CAiwGenericParamList& inList = iAiwServiceHandler->InParamListL();
    InitAiwContactFetchParamL( inList );

    iAiwServiceHandler->ExecuteServiceCmdL( KAiwCmdSelect, inList,
        iAiwServiceHandler->OutParamListL(), 0, this);

    // Assign an input blocker, which will call cancel to phonebook
    // e.g. if Postcard is closed via FSW.
    CAknInputBlock* inputBlock = CAknInputBlock::NewCancelHandlerLC( this );

    iActSchedWait.Start(); // wait ExecuteServiceCmdL() to complete

    CleanupStack::PopAndDestroy( inputBlock );
    
    if ( iShutdown )
        {
        // This leave will be catched in AppUi's DoEditAddressL function.
        User::Leave( KLeaveExit );
        }
    }

// ---------------------------------------------------------
// FetchContactL
// ---------------------------------------------------------
void CPostcardContact::FetchContactL(RFile& aFile)
    {
    __ASSERT_DEBUG( !iStoreContact, Panic(EPostcardPanicCoding));

    // Using streams would be more elegant but Phonebook2 impletes it this
    // way. Writes the descriptor to a file directly without a length.
    TInt fileSize;
    User::LeaveIfError( aFile.Size( fileSize ) );
    HBufC8* buf = HBufC8::NewLC( fileSize );
    TPtr8 ptr = buf->Des();
    User::LeaveIfError( aFile.Read( ptr ) );
    CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC(
        *buf, iContactMatcher->GetContactStoresL());
    if( linkArray->Count() == 0 )
        {
        User::Leave( KErrNotFound );
        }
    iContactMatcher->GetStoreContactL( linkArray->At( 0 ), &iStoreContact );
    CleanupStack::PopAndDestroy( 2, buf ); // linkArray, buf
    }

// ---------------------------------------------------------
// HandleNotifyL
// ---------------------------------------------------------
TInt CPostcardContact::HandleNotifyL(TInt /*aCmdId*/, TInt aEventId,
    CAiwGenericParamList& aEventParamList,
    const CAiwGenericParamList& /*aInParamList*/)
    {

    switch(aEventId)
        {
        case KAiwEventOutParamCheck: // fall through
        case KAiwEventQueryExit:// fall through
            // It is always ok for us to exit. Edited postcard will be
            // saved to drafts.
            return ETrue;

        case KAiwEventCompleted:
            // Fetch completed with or without a contact
            TRAP_IGNORE(HandleAiwEventCompletedL(aEventParamList));
            // Leave in HandleAiwEventCompletedL() is not progated further.
            // After that the Postcard wouldn't respond to UI events. Even when
            // AsyncStop() is called. Maybe Phonebook cannot handle a leave
            // from here properly.

            // Fallthrough
        case KAiwEventCanceled:
            // Fetch was cancelled by user or red key pressed
            // Fallthrough
        case KAiwEventError:
            // Fetch is complete, there was an error
            if (iActSchedWait.IsStarted())
                {
                // End nested active scheduler loop
                iActSchedWait.AsyncStop();
                }
            break;
        default:
            Panic(EPostcardPanicAiwEvent);
        }

    return KErrNone;
    }

// ---------------------------------------------------------
// HandleAiwEventCompleted
// ---------------------------------------------------------
void CPostcardContact::HandleAiwEventCompletedL(
    CAiwGenericParamList& aEventParamList)
    {
    // Single item fetch. The result should be a contact link array with
    // 1 contact links and one contact field.
    __ASSERT_ALWAYS(aEventParamList.Count() == 2,
        Panic(EPostcardPanicAiwEventParam));
    const TAiwGenericParam& param = aEventParamList[0];
    __ASSERT_ALWAYS(param.SemanticId() == EGenericParamContactLinkArray,
        Panic(EPostcardPanicAiwEventParam));
    __ASSERT_DEBUG(param.Value().TypeId() == EVariantTypeDesC8,
        Panic(EPostcardPanicAiwEventParam));
    TPtrC8 contactLinks = param.Value().AsData();
    CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC(
        contactLinks, iContactMatcher->GetContactStoresL() );
    TInt linkCount = linkArray->Count();
    __ASSERT_DEBUG(linkCount <= 1, Panic(EPostcardPanicAiwEventParam));
    if (linkCount)  // user selected a contact
        {
        iContactMatcher->GetStoreContactL( linkArray->At( 0 ), &iStoreContact );
        }
    CleanupStack::PopAndDestroy( linkArray );  // linkArray
    }

// ---------------------------------------------------------
// InitAiwContactFetchParamL
// ---------------------------------------------------------
void CPostcardContact::InitAiwContactFetchParamL(
    CAiwGenericParamList& aParamList ) const
    {
    TAiwGenericParam param( EGenericParamContactSelectionData );
    TAiwSingleEntrySelectionDataV1 selectionData;
    param.Value().Set( TAiwSingleEntrySelectionDataV1Pckg( selectionData ) );
    aParamList.AppendL( param );

    param.Reset();
    param.SetSemanticId( EGenericParamContactStoreUriArray );
    param.Value().Set( *iUriArray->PackLC() );
    aParamList.AppendL( param );
    CleanupStack::PopAndDestroy(); // packed uriArray
    }

// ---------------------------------------------------------
// GetNameLC()
// ---------------------------------------------------------
HBufC* CPostcardContact::GetNameLC() const
    {
    // GetNameL() returns NULL if the contact has no name defined
    HBufC* nameText = iContactMatcher->GetNameL(iStoreContact->Fields());
    if (nameText)
        {
        CleanupStack::PushL(nameText);        
        }
    else
        {
        nameText = HBufC::NewLC(0);
        }
    return nameText;
    }

// ---------------------------------------------------------
// GetContactFieldLC
// ---------------------------------------------------------
HBufC* CPostcardContact::GetContactFieldLC(TVPbkFieldTypeName aFieldType,
    TVPbkSubFieldType aSubFieldType,
    TVPbkFieldTypeParameter aFieldTypeParameter) const
    {
    TVPbkFieldVersitProperty prop;
    
    prop.SetName(aFieldType);
    prop.SetSubField(aSubFieldType);

    TVPbkFieldTypeParameters fieldTypeParameters;
    fieldTypeParameters.Add(aFieldTypeParameter);
    prop.SetParameters(fieldTypeParameters);

    // Doesn't take ownership
    const MVPbkFieldType* fieldType =
        iContactMatcher->FieldTypes().FindMatch(prop, 0);

    if (fieldType)
        {
	    TPtrC src =
            iContactMatcher->GetFieldDataTextL(*iStoreContact, *fieldType);
        HBufC *data = HBufC::NewLC(src.Length());
        *data = src;
        data->Des().Trim();
        return data;
        }
    else
        {
        return HBufC::NewLC(0);
        }
    }

// ---------------------------------------------------------
// AknInputBlockCancel
// ---------------------------------------------------------
void CPostcardContact::AknInputBlockCancel()
    {
    if ( iActSchedWait.IsStarted() )
        {
        // Cancels currently outstanding AIW request to phonebook.
        TRAPD( error, iAiwServiceHandler->ExecuteServiceCmdL( KAiwCmdSelect, 
                                                   iAiwServiceHandler->InParamListL(), 
                                                   iAiwServiceHandler->OutParamListL(),
                                                   KAiwOptCancel,
                                                   this ) );
        if ( error == KErrNone )
            {
            iShutdown = ETrue;
            }
        }
    }

// End of File
