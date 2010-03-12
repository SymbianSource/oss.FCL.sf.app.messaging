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
* Description:  
*     Item array for the message items to be displayed by mce.
*     Main view items are kept in memory (CArrayFixFlat<TMceListItem>) when created but
*     Message view list items are fetched from the msgs when needed in MdcaPoint function
*
*/



// INCLUDE FILES

#include <AknUtils.h>       // AknTextUtils
#include <mtudcbas.h>
#include <msvids.h>
#include <NpdApi.h>         // CNotepadApi
#include "MtmRegistryObserver.h" // MMtmUiDataRegistryObserver
#include <StringLoader.h>   // StringLoader
#include <mce.rsg>
#include <SenduiMtmUids.h>  // mtm uids
#include <eikenv.h>

#include "MceMessageListItemArray.h"
#include "MceBitmapResolver.h"
#include "MceIds.h" // KMceDocumentsEntryIdValue
#include "McePanic.h"
#include "MceCommands.hrh"
#include "MceLogText.h"

#include <MTMStore.h>
#include <commonphoneparser.h>

#include <aknappui.h>
#include <akncontext.h>
#include <aknconsts.h>
#include <e32math.h>

#include <bldvariant.hrh>
#include <featmgr.h>
#include <NumberGrouping.h>
#include <mmsconst.h>
#include <mmsgenutils.h>
#include <mmsclient.h>
#include <mtclreg.h>
#include <muiu.mbg>
#include <mtudreg.h>        // CMtmUiDataRegistry

// security data caging
#include <data_caging_path_literals.hrh>
#include <miutset.h>
#include <msvapi.h>
#include <ImumInternalApi.h>            // CImumInternalApi
#include <ImumInSettingsData.h>         // CImumInSettingsData

#include <muiuflags.h>

//FS email stuff
#include <centralrepository.h>
#include <MessagingDomainCRKeys.h>
const TInt KMceListArrayGranuality = 4;

// CONSTANTS

const TInt KMceArabicSemiColon                = {0x061B};
const TInt KMceSemiColon                      = {0x003B};
const TInt KMceMaxFormatFactor                = 2;
const TText KMceRLM = 0x200F;
const TText KMcePDF = 0x202C;

const TUid KUidMsvMessageListSortOrder={0x10005359};

_LIT( KSortSubjectSkipString, "*:*" );
const TChar KWhiteSpaceCharacter(' ');
const TInt KSortSubjectStringLength = 100;

// LOCAL CONSTANTS AND MACROS
enum
    {
    KMceListArrayDocAmount = 0x0,
    KMceListArrayDocAmountMany,
    KMceListArrayDocAmountTemp,
    KMceListArrayDocAmountTemps,
    KMceListArrayTimeFormatBuffer,
    KMceListArrayDateFormatBuffer
    };

//cmail update
#define KUidMsgTypeFsMtmVal               0x2001F406

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CMceMessageListItemArray::CMceMessageListItemArray(
    CMsvSessionPtr aSession,
    TMsvId aFolderId,
    TMceListType aListType,
    CMceBitmapResolver& aBitmapResolver ) :
//    CArrayFixFlat<TMceListItem>( KMceListGranuality ),
    iSession( aSession ),
    iFolderId( aFolderId ),
    iListType( aListType ),
    iBitmapResolver( aBitmapResolver ),
    iEmailClientIntegration(EFalse),
    iIntegratedEmailAppExcist(EFalse),
    iEmailFramework(EFalse)
    {
    iDescriptionLength = iBitmapResolver.DescriptionLength();
    }

// ----------------------------------------------------
// CMceMessageListItemArray::ConstructL
// Symbian OS default constructor can leave.
// ----------------------------------------------------
void CMceMessageListItemArray::ConstructL()
    {
    CRepository* repository = NULL;
    // read phone number grouping feature flag
    iNumberGroupingEnabledFlag = MceUtils::NumberGroupingEnabled();
        
    iPreviousSelectionOrdering = TMsvSelectionOrdering(
            KMsvGroupByStandardFolders, EMsvSortByDateReverse);
    // Added for FS Email stuff
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework ) )
        {
        iEmailFramework = ETrue;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdEmailMceIntegration) )
        {
        iEmailClientIntegration = ETrue;

        // Check if there is other than platform email application registered to handle S60 mailboxes   
        repository = NULL;
        TInt mtmPluginID = 0;
        TRAPD( ret, repository = CRepository::NewL(
               KCRUidSelectableDefaultEmailSettings ) );

        if ( ret == KErrNone )
            {
            TInt err = repository->Get( KIntegratedEmailAppMtmPluginId ,
                                        mtmPluginID );
            if ( err != KErrNone )
                {
                mtmPluginID = 0;
                }
            }
        delete repository;

        if ( mtmPluginID != 0 )
            {
            iIntegratedEmailAppExcist = ETrue;
            }
        }

    
    FeatureManager::UnInitializeLib();
    iFolderEntry = CMsvEntry::NewL(
        *iSession,
        iFolderId,
        iPreviousSelectionOrdering );

    iNumberOfMessagesStrings = new(ELeave)CDesCArrayFlat( KMceListGranuality );
    TBuf<KMcePrintableTextLength> tempString;
    CCoeEnv* eikonEnv = CEikonEnv::Static();
    StringLoader::Load( tempString, R_MCE_DOC_AMOUNT, eikonEnv );
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_DOC_AMOUNT_MANY, eikonEnv );
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_DOC_AMOUNT_TEMP, eikonEnv );
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_DOC_AMOUNT_TEMPS, eikonEnv );
    iNumberOfMessagesStrings->AppendL( tempString );


    HBufC* timeFormat = eikonEnv->
        AllocReadResourceLC( R_QTN_TIME_USUAL_WITH_ZERO );
    iNumberOfMessagesStrings->AppendL( *timeFormat );
    CleanupStack::PopAndDestroy();
    
    HBufC* dateFormat = eikonEnv->
        AllocReadResourceLC( R_QTN_DATE_WITHOUT_YEAR_WITH_ZERO );
    iNumberOfMessagesStrings->AppendL( *dateFormat );
    CleanupStack::PopAndDestroy();
    
    // TODO
    iListBoxText = HBufC::NewL( iDescriptionLength*3 ); 
    iUiRegistry = CMtmUiDataRegistry::NewL(*iSession); 
    //FS Email stuff
    iMsgIndex = new ( ELeave )CArrayFixFlat<TInt>( KMceListArrayGranuality );
    }

// Two-phased constructor.
CMceMessageListItemArray* CMceMessageListItemArray::NewL(
    CMsvSessionPtr aSession,
    TMsvId aFolderId,
    TMceListType aListType,
    CMceBitmapResolver& aBitmapResolver )
    {
    CMceMessageListItemArray* self = new (ELeave) CMceMessageListItemArray(
        aSession, aFolderId, aListType, aBitmapResolver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }


// Destructor
CMceMessageListItemArray::~CMceMessageListItemArray( )
    {
    delete iFolderEntry;
    delete iListBoxText;
    delete iNumberOfMessagesStrings;
    delete iUiRegistry;
    }


// ----------------------------------------------------
// CMceMessageListItemArray::ItemIndex
// ----------------------------------------------------
TInt CMceMessageListItemArray::ItemIndex(TMsvId aItemId) const
    {
    TInt foundItem = KErrNotFound;
    TInt count = iFolderEntry->Count();
    if ( (iEmailClientIntegration && iIntegratedEmailAppExcist) || iEmailFramework ) 
        {
        TRAP_IGNORE( count = CreateEntryIndexL() );
        }
    for( TInt loop = 0; loop < count && foundItem == KErrNotFound ; loop++)
        {
        if ( (*iFolderEntry)[ GetEntryIndex( loop ) ].Id() == aItemId )
            {
            foundItem = loop;
            }
        }
    return foundItem;
    }

// ----------------------------------------------------
// CMceMessageListItemArray::ItemId
// ----------------------------------------------------
TMsvId CMceMessageListItemArray::ItemId( TInt aIndex ) const
    {
    TMsvId id = KErrNotFound;
    if ( aIndex >= 0 && iFolderEntry->Count() > aIndex )
        {
        id = (*iFolderEntry)[ GetEntryIndex( aIndex ) ].Id();
        }
    return id;
    }

// ----------------------------------------------------
// CMceMessageListItemArray::Item
// ----------------------------------------------------
const TMceListItem CMceMessageListItemArray::Item( TInt aIndex ) const
    {
    TMceListItem tempItem;
    tempItem.iMsvId = (*iFolderEntry)[ aIndex ].Id();
    tempItem.iExtraItem = EFalse;
    return tempItem;
    }


// ----------------------------------------------------
// CMceMessageListItemArray::GetEntry
// ----------------------------------------------------
TInt CMceMessageListItemArray::GetEntry( TMsvId aEntryId, TMsvEntry& aEntry )
    {
    TMsvId serviceId;
    return iSession->GetEntry( aEntryId, serviceId, aEntry );
    }


// ----------------------------------------------------
// CMceMessageListItemArray::SetFolderL
// ----------------------------------------------------
void CMceMessageListItemArray::SetFolderL( TMsvId aFolderId )
    {
    // this can only happen when iListType == EMceListTypeMessage
    iFolderEntry->SetEntryL( aFolderId );
    iFolderId = aFolderId;
    }

// ----------------------------------------------------
// CMceMessageListItemArray::IsItemFolder
// ----------------------------------------------------
TBool CMceMessageListItemArray::IsItemFolder( TMsvId aItemId ) const
    {
    TBool isFolder = EFalse;
    TMsvId serviceId;
    TMsvEntry entry;
    if ( iSession->GetEntry( aItemId, serviceId, entry ) == KErrNone &&
         entry.iType == KUidMsvFolderEntry )
        {
        isFolder = ETrue;
        }
    return isFolder;
    }

// ----------------------------------------------------
// CMceMessageListItemArray::MdcaCount
// ----------------------------------------------------
TInt CMceMessageListItemArray::MdcaCount() const
    {
    TInt retval=iFolderEntry->Count();
    if (( iEmailClientIntegration && iIntegratedEmailAppExcist )||(iEmailFramework))
        {
        if( iFolderId == KMsvDraftEntryId || 
                iFolderId == KMsvSentEntryId || 
                iFolderId == KMsvDeletedEntryFolderEntryId || 
                iFolderId == KMsvGlobalInBoxIndexEntryId || iFolderId == KMsvGlobalOutBoxIndexEntryId)
            {

            TRAP_IGNORE( retval = CreateEntryIndexL() );
            }
        }   
    return ( retval );
    }

// ----------------------------------------------------
// CMceMessageListItemArray::MdcaPoint
// ----------------------------------------------------
TPtrC CMceMessageListItemArray::MdcaPoint( TInt aIndex ) const
    {
    return MakeStringForMessageView( aIndex );
    }

// ----------------------------------------------------
// CMceMessageListItemArray::MakeStringForMessageView
// ----------------------------------------------------
TPtrC CMceMessageListItemArray::MakeStringForMessageView( TInt aIndex ) const
    {
    __ASSERT_DEBUG( iListType == EMceListTypeMessage, Panic(EMceListItemArrayNotMessageViewList ) );
    const TMsvEntry* child;
    child = &(*iFolderEntry)[ GetEntryIndex( aIndex ) ];
    TPtr tempText = iListBoxText->Des();

    tempText.Zero();
    const TInt index = iBitmapResolver.BitmapIndex( *child );
    tempText.AppendNum( index );
    tempText.Append( KColumnListSeparator );

    if ( child->iType == KUidMsvFolderEntry )
        {
        AppendFolderString( tempText, *child );
        }
    else // should be KUidMsvMessageEntryValue
        {
        AppendMessageString( tempText, *child );
        }
    return tempText;

    }

// ----------------------------------------------------
// CMceMessageListItemArray::AppendMessageString
// ----------------------------------------------------
void CMceMessageListItemArray::AppendMessageString( TDes& aBuffer, const TMsvEntry& aEntry ) const
    {
    __ASSERT_DEBUG( aEntry.iType == KUidMsvMessageEntry, Panic(EMceListItemArrayEntryNotMessage ) );

    if ( aEntry.iMtm == KSenduiMtmImap4Uid || aEntry.iMtm == KSenduiMtmPop3Uid
        || aEntry.iMtm == KSenduiMtmSmtpUid )
        {
        StripAndAppendString( aBuffer, aEntry.iDetails );
        }
    else
        {
        TRAPD( error, AppendRecipientStringL( aBuffer, aEntry ) );
        if ( error != KErrNone )
            {
            // leave is not needed to handle; if leaves, nothing is shown
            }        
        }   
    
    aBuffer.Append( KColumnListSeparator );

    if ( ( iFolderEntry->Entry().Id() == KMsvGlobalInBoxIndexEntryId ) &&
        ( aEntry.iMtm == KUidMsgMMSNotification ) ) 
        {
        TRAP_IGNORE( MakeInboxStatusStringL( aBuffer, aEntry ) );
        }

    else if ( iFolderEntry->Entry().Id() == KMsvGlobalOutBoxIndexEntryId )
        {
        // in outbox show status of the message
        TRAP_IGNORE(MakeSendStatusStringL(aBuffer, aEntry));
        }
    else
        {
        TRAP_IGNORE( AppendTimeStringL( aBuffer, aEntry ) );

        if ( aEntry.iMtm==KSenduiMtmIrUid || aEntry.iMtm==KSenduiMtmBtUid )
            {
            TRAP_IGNORE( AppendBtIrMessageStringL( aBuffer, aEntry ) );
            }
         else
            {
            StripAndAppendString( aBuffer, aEntry.iDescription );
            }
        }

    aBuffer.Append( KColumnListSeparator );

    if ( aEntry.iMtm != KSenduiMtmMmsUid &&
         aEntry.Attachment() )
        {
        aBuffer.AppendNum( (TInt) EMceBitmapIndexAttachment );
	    aBuffer.Append( KColumnListSeparator );
        }
        
    TMsvPriority priority = aEntry.Priority();
    if ( priority == EMsvHighPriority )
        {
        aBuffer.AppendNum( EMceBitmapIndexPriorityHigh );
        aBuffer.Append( KColumnListSeparator );
        }
    else if ( priority == EMsvLowPriority )
        {
        aBuffer.AppendNum( EMceBitmapIndexPriorityLow );
        aBuffer.Append( KColumnListSeparator );
        }
       
    }

// ----------------------------------------------------
// CMceMessageListItemArray::AppendFolderString
// ----------------------------------------------------
void CMceMessageListItemArray::AppendFolderString( TDes& aBuffer, const TMsvEntry& aEntry ) const
    {
    __ASSERT_DEBUG( aEntry.iType == KUidMsvFolderEntry, Panic(EMceListItemArrayEntryNotFolder ) );

    StripAndAppendString( aBuffer, aEntry.iDetails );
    aBuffer.Append( KColumnListSeparator );
    if ( aEntry.Id() == KMceTemplatesEntryId )
        {        
        TInt templatesCount = CNotepadApi::NumberOfTemplates();
        templatesCount = templatesCount < 0 ? 0 : templatesCount;
        if ( templatesCount == 1 )
            {
            TBuf<KMcePrintableTextLength> tempString;
            tempString.Append( (*iNumberOfMessagesStrings)[
                KMceListArrayDocAmountTemp] );
            AknTextUtils::LanguageSpecificNumberConversion( tempString );         
            aBuffer.Append( tempString );
            }
        else
            {
            TBuf<KMcePrintableTextLength> tempString;
            StringLoader::Format( tempString,
                (*iNumberOfMessagesStrings)[KMceListArrayDocAmountTemps],
                -1, // no index in the key string
                templatesCount );
            AknTextUtils::LanguageSpecificNumberConversion( tempString );
            aBuffer.Append( tempString );
            }
        }
    else
        {
        TRAP_IGNORE( HandleFolderMessagesStringL( aBuffer, aEntry ) );        
        }
    aBuffer.Append( KColumnListSeparator );
    }


// ----------------------------------------------------
// CMceMessageListItemArray::HandleSessionEventL
// ----------------------------------------------------
void CMceMessageListItemArray::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    // TODO: why?
    switch (aEvent)
        {
    case EMsvEntriesCreated:
    case EMsvEntriesChanged:
    case EMsvEntriesDeleted:
    case EMsvMediaChanged:
    case EMsvMediaUnavailable:
    case EMsvMediaAvailable:
    case EMsvCloseSession:        
        break;

    default:
        break;
        }
    }

// ----------------------------------------------------
// CMceMessageListItemArray::SetRegistryObserver
// ----------------------------------------------------
void CMceMessageListItemArray::SetRegistryObserver( MMtmUiDataRegistryObserver* aRegistryObserver )
    {
    iRegistryObserver = aRegistryObserver;
    }

// ----------------------------------------------------
// CMceMessageListItemArray::MakeSendStatusStringL
// ----------------------------------------------------
void CMceMessageListItemArray::MakeSendStatusStringL(TDes& aText, const TMsvEntry& aEntry) const
    {
    if ( iRegistryObserver )
        {
        CBaseMtmUiData* data=iRegistryObserver->GetMtmUiDataL(aEntry.iMtm);
        if ( data )
            {
            HBufC* statusText=data->StatusTextL(aEntry);
            if ( statusText->Length() )
                {
                aText.Append( statusText->Des() );
                }
            else
                {
                aText.Append( CEditableText::ESpace );
                }
            delete statusText;
            }
        }
    }


// ----------------------------------------------------
// CMceMessageListItemArray::StripAndAppendString
// ----------------------------------------------------
void CMceMessageListItemArray::StripAndAppendString(TDes& aTargetString, const TDesC& aSourceString) const
    {
    TRAP_IGNORE( MceUtils::StripAndAppendStringL(
        aTargetString,
        aSourceString,
        iDescriptionLength ) );
    }

// ----------------------------------------------------
// CMceMessageListItemArray::IsFolderRemote
// return ETrue, if remote folder
//
// ----------------------------------------------------
TBool CMceMessageListItemArray::IsFolderRemote() const
    {
    return IsItemRemote( iFolderId );
    }
    
// ----------------------------------------------------
// CMceMessageListItemArray::
// ----------------------------------------------------
TBool CMceMessageListItemArray::IsItemRemote( TMsvId aItemId ) const
    {
    TMsvId service = KMsvLocalServiceIndexEntryId;
    TMsvEntry entry;
    if ( iSession->GetEntry(aItemId,service,entry) == KErrNone )
    	{
    	return KMsvLocalServiceIndexEntryId != service;
    	}
    else
    	{
    	return EFalse;
    	}
    }

// ----------------------------------------------------
// CMceMessageListItemArray::FolderId
// return folder id
//
// ----------------------------------------------------
TMsvId CMceMessageListItemArray::FolderId() const
    {
    return iFolderId;
    }
    
// ----------------------------------------------------
// CMceMessageListItemArray::
// ----------------------------------------------------
const TMsvEntry& CMceMessageListItemArray::FolderEntry() const
    {
    return iFolderEntry->Entry();
    }
    


// ----------------------------------------------------
// CMceMessageListItemArray::IsConnected
// return ETrue, if account is connected
//
// ----------------------------------------------------
TBool CMceMessageListItemArray::IsConnected() const
    {
    TMsvEntry entry;
    entry = iFolderEntry->Entry();

    TMsvEntry serviceEntry;
    TMsvId serviceId;
    if ( iSession->GetEntry(entry.iServiceId,serviceId,serviceEntry) == KErrNone )
    	{
    	return serviceEntry.Connected();
    	}
    else
    	{
    	return EFalse;
    	}
    }


// ----------------------------------------------------
// CMceMessageListItemArray::FindEntryIndex
// Finds entry index in message list
//
// ----------------------------------------------------
TInt CMceMessageListItemArray::FindEntryIndex( const TDesC& aMatchString ) const
    {
    TInt foundIndex = KErrNotFound;
    TMsvSelectionOrdering ordering( KMsvNoGrouping, EMsvSortByDateReverse, EFalse );
    ordering = iFolderEntry->SortType();

    const TInt count = iFolderEntry->Count();
    const TInt length = aMatchString.Length();

    if ( (ordering.Sorting()==EMsvSortByDetails) ||
    	 (ordering.Sorting()==EMsvSortByDetailsReverse) )
        { 
        for( TInt loop = 0; loop < count && foundIndex == KErrNotFound ; loop++ )
            {
            if ( aMatchString.CompareF((*iFolderEntry)[loop].iDetails.Left( length ) ) == 0 )
                {
                foundIndex = loop;
                }
            }
        }
  
    else if ( (ordering.Sorting()==EMsvSortByDescription) ||
    		  (ordering.Sorting()==EMsvSortByDescriptionReverse) )
        {
        for( TInt loop = 0; loop < count && foundIndex == KErrNotFound ; loop++)
            {
            TBufC<100> tmpString( (*iFolderEntry)[loop].iDescription.Left(KSortSubjectStringLength) );
            TInt start = SkipPrefix( tmpString.Des() );
            if ( start < tmpString.Length() && tmpString.Length() >= (start + length) )
                {
                TPtrC mid = (*iFolderEntry)[loop].iDescription.Mid( start, length );
                if ( aMatchString.CompareF( mid ) == 0 )
                    {
                    foundIndex = loop;
                    }
                }
            }
        }
    else
        {
        //EMsvSortByReverse, nothing cannot be searched          
        }

    return foundIndex;
    }

// ----------------------------------------------------
// CMceMessageListItemArray::SetOrderingL
// Sets sort order in folder entry
//
// ----------------------------------------------------
void CMceMessageListItemArray::SetOrderingL( TInt aOrdering )
    { 
    TMsvSorting sorting = EMsvSortByDateReverse;
    TInt grouping = KMsvGroupByStandardFolders; 
    switch ( aOrdering )
        {	
        case EMceCmdSortByRecipient:
        case EMceCmdSortBySender:
            sorting=EMsvSortByDetails;		    		
            break;
        case EMceCmdSortBySubject:
            sorting=EMsvSortByDescription;            
            break;        
        case EMceCmdSortByDate:
        default:
            sorting=EMsvSortByDateReverse;           
            break;        
              
        }

    const TMsvSelectionOrdering newOrdering( grouping, sorting, EFalse );
    iFolderEntry->SetSortTypeL( newOrdering );   
    }



// ----------------------------------------------------
// CMceMessageListItemArray::DeleteDiscard
// return ETrue, if delete command should be discarded
//
// ----------------------------------------------------
TBool CMceMessageListItemArray::DeleteDiscard( TMsvId aItemId ) const
    {
    TMsvEntry entry;        
    TMsvId serviceId;
    if ( iSession->GetEntry(aItemId,serviceId,entry) != KErrNone )
    	{
    	return ETrue;
    	}
    
    //mail or syncml folder cannot be deleted
    if ( ( entry.iType == KUidMsvFolderEntry ) &&
        ( entry.iMtm == KSenduiMtmImap4Uid ||
        entry.iMtm == KSenduiMtmSyncMLEmailUid ) )
        {
        return ETrue;
        }

    if ( iFolderEntry->Entry().Id() == KMsvGlobalOutBoxIndexEntryId )
        {
        if ( entry.SendingState() == KMsvSendStateSending )
            {
            return ETrue;
            }

        // Message in waiting state cannot be deleted except e-mail.
        // For e-mail, waiting, resend and scheduled states are presented
        // to user as queued state (by CSmtpMtmUiData). Delete is allowed
        // from the queued state.
        if ( ( entry.SendingState() == KMsvSendStateWaiting ) &&
            ( entry.iMtm != KSenduiMtmSmtpUid ) )
            {
            return ETrue;
            }

        if ( entry.iMtm == KSenduiMtmSmsUid ||
            entry.iMtm == KSenduiMtmMmsUid )
            {
            if ( entry.Connected() || ( entry.SendingState() == KMsvSendStateUnknown )
                || ( entry.SendingState() == KMsvSendStateNotApplicable ) )
                {
                return ETrue;
                }
            }
        }
    return EFalse;
    }

// ----------------------------------------------------
// CMceMessageListItemArray::AppendRecipientStringL
// handles recipient string
//
// ----------------------------------------------------
void CMceMessageListItemArray::AppendRecipientStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const
    {

    //
    // The following processes the recipient(s) for languages which require number
    // conversion and/or number grouping
    //
    if ( AknLayoutUtils::LayoutMirrored() || iNumberGroupingEnabledFlag || 
           EDigitTypeWestern != AknTextUtils::NumericEditorDigitType() )
        {

        //
        // semicolon separator
        //
        TBuf<1> semiColon;              
        semiColon.Append( KMceSemiColon );
        TInt posForSemiColon = KErrNotFound;

        //
        // arabic semicolon separator
        //
        TBuf<1> arabicSemiColon;              
        arabicSemiColon.Append( KMceArabicSemiColon );
        TInt posForArabicSemiColon = KErrNotFound;

        //
        // The following are used for keeping track of which type of separator we're
        // currently processing
        //
        TBuf<1> separator;
        TInt posForSeparator = KErrNotFound;

        TInt len = aEntry.iDetails.Length();

        //
        // recipient buffer is made to be larger than the length of the entry details
        // to allow additional room needed for number grouping
        //
        HBufC* recipientBuf = HBufC::NewLC(	KMceMaxFormatFactor*len );
        TPtr recipientPtr = recipientBuf->Des();

        //
        // remaining details buffer is initialized to the same as the entry details
        // and we will remove processed info until this buffer becomes empty
        //
        HBufC* remainingDetailsBuf = HBufC::NewLC( len );
        TPtr remainingDetailsPtr = remainingDetailsBuf->Des();
        remainingDetailsPtr.Copy( aEntry.iDetails );

        //
        // result buffer is made to be larger than the length of the entry details
        // to allow additional room needed for number grouping
        //
        HBufC* resultBuf = HBufC::NewLC( KMceMaxFormatFactor*len );        
        TPtr resultPtr = resultBuf->Des();

        //
        // Construct the result buffer from the entry details. Each recipient is
        // extracted from the entry details and valid phone numbers are number
        // grouped and converted as needed before they are concatenated into a
        // string separated by semicolon/arabic semicolon
        //
        do
            {

            // find the first occurrence of semi colon in the remaining details buffer 
            posForSemiColon = remainingDetailsPtr.Find( semiColon );

            // find the first occurrence of arabic semicolon in the remaining details
            // buffer
            posForArabicSemiColon = remainingDetailsPtr.Find( arabicSemiColon );

            //
            // 5 possible cases:
            //
            // 1) Neither semicolon nor arabic semicolon is found in the remaining
            //    details buffer:
            //
            //    we will take the first KPhCltNameBufferLength to the recipient buffer
            //    to proceed with our number grouping and/or conversion. At the end of
            //    this iteration, there will not be any in the remaining details buffer
            //    to be processed. This is either the single recipient case or we are
            //    processing the last of the recipients
            //
            if ( posForSemiColon == KErrNotFound && posForArabicSemiColon == KErrNotFound )
                {
                posForSeparator = KErrNotFound;
                }

            // 2) found arabic semicolon in the remaining details buffer:
            //
            //    we will take everything upto where we find the arabic semicolon to the
            //    recipient buffer and we strip this recipient from our remaining details
            //    buffer for the next round of processing. (this is the multiple
            //    recipients case where each recipient is separated by arabic semicolon)
            //
            else if ( posForSemiColon == KErrNotFound )
                {
                posForSeparator = posForArabicSemiColon;
                separator.Copy( arabicSemiColon );
                }

            // 3) found semicolon in the remaining details buffer:
            //
            //    similar to the above. We will take everything upto where we find the
            //    semicolon to the recipient buffer and we strip this recipient from our
            //    remaining details buffer. (this is also the multiple recipients case
            //    where each recipient is separated by semicolon)
            //
            else if ( posForArabicSemiColon == KErrNotFound )
                {
                posForSeparator = posForSemiColon;
                separator.Copy( semiColon );
                }

            // The following 2 blocks are categorized under case 4).
            //
            // 4) found both semicolon and arabic semicolon: (This is the multiple
            //    recipients case where each recipient is separated by mix of semicolon
            //    and arabic semicolon).
            //    This is further broken down into 2 cases:
            //
            //    a) semicolon found is before arabic semicolon: treat it the same way
            //       as 3)
            //
            else if ( posForSemiColon < posForArabicSemiColon )
                {
                posForSeparator = posForSemiColon;
                separator.Copy( semiColon );
                }
        
            //    b) semicolon found is after arabic semicolon: treat it the same way
            //       as 2)
            //
            else
                {
                posForSeparator = posForArabicSemiColon;
                separator.Copy( arabicSemiColon );
                }

            //
            // take the first KPhCltNameBufferLength to the recipient buffer or take
            // everything upto where we find the separator
            //
            recipientPtr.Copy(
                remainingDetailsPtr.Left(
                    ( posForSeparator == KErrNotFound ) ? KPhCltNameBufferLength : posForSeparator ) );

            //
            // remove the recipient we're processing from the remaining details buffer.
            // remove the separator as well (this is accomplished by advancing the position
            // by 1)
            //
            remainingDetailsPtr.Copy(
                remainingDetailsPtr.Mid(
                    ( posForSeparator == KErrNotFound ) ? remainingDetailsPtr.Length() : posForSeparator + 1 ) );

            //
            // process the recipient buffer
            //
            if ( recipientPtr.Length() > 0 )
                {

                TBool validNumber = CommonPhoneParser::IsValidPhoneNumber( 
                    recipientPtr, CommonPhoneParser::ESMSNumber );

                //
                // if the recipient is specified as a valid number, we will
                // perform number grouping and/or conversion as needed
                //
                if ( validNumber )
                    {

                    // perform number grouping as needed
                    DoNumberGroupingL( recipientPtr );

                    // perform Arabic & Hebrew conversion as needed
                    AknTextUtils::LanguageSpecificNumberConversion( recipientPtr );

                    }

                //
                // for Arabic/Hebrew, if the recipient isn't the last in the list, we need to
                // put KMceRLM marker before the recipient name and McePDF marker before the
                // arabic semicolon, e.g.
                //
                // <KMceRLM><recipient name><KMcePDF><arabic semicolon>
                //
                if ( !validNumber &&
                     ( posForSeparator != KErrNotFound ) &&
                     ( separator.Compare( arabicSemiColon ) == 0 ) )
                   {
                   resultPtr.Append( KMceRLM );
                   }

                //
                // we've finished processing this recipient, append it to the result buffer
                //
                resultPtr.Append( recipientPtr );

                //
                // append separator if there are subsequent recipients
                //
                if ( posForSeparator != KErrNotFound )
                    {

                    //
                    // for Arabic/Hebrew, if the recipient isn't the last in the list, we need to
                    // put KMceRLM marker before the recipient name and McePDF marker before the
                    // arabic semicolon, e.g.
                    //
                    // <KMceRLM><recipient name><KMcePDF><arabic semicolon>
                    //
                    if ( !validNumber &&
                         ( separator.Compare( arabicSemiColon ) == 0 ) )
                        {
                        resultPtr.Append( KMcePDF );
                        }

                    resultPtr.Append( separator );
                    }

                }

            } while ( remainingDetailsPtr.Length() > 0 );

        //
        // we've processed the entire entry details, append our result to aBuffer
        //
        if ( len )
            {
            StripAndAppendString( aBuffer, resultPtr );
            }
        else
            {
            HBufC* text = StringLoader::LoadLC( R_MCE_PUSH_INBOX_SENDER_UNKW );
            aBuffer.Append( *text );
            CleanupStack::PopAndDestroy(); // text
            }

        CleanupStack::PopAndDestroy( 3 ); // recipientBuf, remainingDetailsBuf, resultBuf

        }

    //
    // The following processes the recipient(s) for languages which do not require number
    // conversion or number grouping for performance consideration
    //
    else
        {
        if ( aEntry.iDetails.Length() )
            {
        StripAndAppendString( aBuffer, aEntry.iDetails );
            }
        else
            {
            HBufC* text = StringLoader::LoadLC( R_MCE_PUSH_INBOX_SENDER_UNKW );
            aBuffer.Append( *text );
            CleanupStack::PopAndDestroy(); // text
            }
        }

    }

// ----------------------------------------------------
// CMceMessageListItemArray::DoNumberGroupingL
//
// This method uses CPNGNumberGrouping to reformat the given
// valid phone number if all of the following are true:
//
// 1) number grouping feature is turned on
//
// 2) the given number does not already exceed the maximum
//    displayable length for number grouping
//
// If not all of the above are true, the given phone number
// remains unchanged.
//
// This method leaves if unable to instantiate a CPNGNumberGrouping
// object.
// ----------------------------------------------------
void CMceMessageListItemArray::DoNumberGroupingL( TDes& aPhoneNumber ) const
    {

    if ( iNumberGroupingEnabledFlag )
        {

        CPNGNumberGrouping* numberGrouping =
            CPNGNumberGrouping::NewL( KPhCltTelephoneNumberLength );
        CleanupStack::PushL( numberGrouping );

        if ( aPhoneNumber.Length() <= numberGrouping->MaxDisplayLength() )
            {

            //
            // Remove extra space for multiple recipients
            //
            TInt lengthBeforeTrimming = aPhoneNumber.Length();
            aPhoneNumber.Trim();
            TInt lengthAfterTrimming = aPhoneNumber.Length();

            numberGrouping->Set( aPhoneNumber );

            // reset aPhoneNumber
            aPhoneNumber.Zero();

            //
            // put the space prefix back if one was stripped before
            //
            if ( lengthBeforeTrimming != lengthAfterTrimming )
                {
                aPhoneNumber.Append( CEditableText::ESpace );
                }

            aPhoneNumber.Append( numberGrouping->FormattedNumber() );

            } // end if aPhoneNumber

        CleanupStack::PopAndDestroy(); // numberGrouping

        } // end if iNumberGroupingEnabledFlag

    }

// ----------------------------------------------------
// CMceMessageListItemArray::MakeInboxStatusStringL
// ----------------------------------------------------
void CMceMessageListItemArray::MakeInboxStatusStringL( TDes& aText, const TMsvEntry& aEntry ) const
    {
    AppendTimeStringL( aText, aEntry );
    if ( iRegistryObserver )
        {
        CBaseMtmUiData* data=iRegistryObserver->GetMtmUiDataL( aEntry.iMtm );
        if ( data )
            {
            HBufC* statusText=data->StatusTextL( aEntry );
            StripAndAppendString( aText, statusText->Des() );            
            delete statusText;
            }
        }
    }

// ----------------------------------------------------
// CMceMessageListItemArray::HandleFolderMessagesStringL
// ----------------------------------------------------
void CMceMessageListItemArray::HandleFolderMessagesStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const
    {
    CMsvEntry* folderCEntry = NULL;
    folderCEntry = iFolderEntry->ChildEntryL( aEntry.Id() );
   
	TUid technologyType = KNullUid;

    if ( aEntry.iMtm.iUid != 0 && aEntry.iMtm != KUidMsvLocalServiceMtm &&
         iUiRegistry->IsPresent( aEntry.iMtm ) )
        {
        technologyType = iUiRegistry->TechnologyTypeUid( aEntry.iMtm );
        }

    // Is Mail MTM
    if ( KSenduiTechnologyMailUid == technologyType )
        {
        // Count subfolders
        CMsvEntrySelection* sel=folderCEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
        CleanupStack::PushL(sel);
        TInt folderCount = sel->Count();               

        // Count mail messages
        CMsvEntrySelection* mesSel=folderCEntry->ChildrenWithTypeL( KUidMsvMessageEntry );
        CleanupStack::PushL( mesSel );
        TInt messagesCount = mesSel->Count();
        CleanupStack::PopAndDestroy(); // mesSel

        CArrayFix<TInt>* indexArray =
            new( ELeave ) CArrayFixFlat<TInt>( KMceListArrayGranuality );             
                    
        CleanupStack::PushL( indexArray );
           
       if (folderCount >  0)
           {
           TInt cnt;
           for(cnt=0;cnt<folderCount;cnt++)
               {
               CMsvEntry* folderSubEntry = iSession->GetEntryL( sel->At( cnt ) );
               CleanupStack::PushL( folderSubEntry );
               CMsvEntrySelection* mesSel = folderSubEntry->ChildrenWithTypeL( KUidMsvMessageEntry );
               CleanupStack::PushL( mesSel );        
               messagesCount+=mesSel->Count();              
               CleanupStack::PopAndDestroy( mesSel );
               CleanupStack::PopAndDestroy( folderSubEntry );
               }
           }
        indexArray->AppendL( folderCount );
        indexArray->AppendL( messagesCount );           

        HBufC* tempString = NULL;               

        if ( folderCount == 1 )
            {
            if ( messagesCount == 1 )
                {
                tempString = StringLoader::LoadLC(
                    R_MCE_MAILBOX_AMOUNT_FOLDER_MAIL, *indexArray );
                }
            else
                {
                tempString = StringLoader::LoadLC(
                    R_MCE_MAILBOX_AMOUNT_FOLDER_MAILS, *indexArray );
                }
            }
        else
            {
            if ( messagesCount == 1 )
                {
                tempString = StringLoader::LoadLC(
	                R_MCE_MAILBOX_AMOUNT_FOLDERS_MAIL, *indexArray );
                }
            else
                {
                tempString = StringLoader::LoadLC(
                    R_MCE_MAILBOX_AMOUNT_FOLDERS_MAILS, *indexArray );
                }
            }
           
        aBuffer.Append( tempString->Des() );
        CleanupStack::PopAndDestroy( 2 ); // indexArray, tempString
       CleanupStack::PopAndDestroy(); //sel 
        }
    else
        {
        CMsvEntrySelection* mesSel=folderCEntry->ChildrenWithTypeL( KUidMsvMessageEntry );
        CleanupStack::PushL( mesSel );
        TInt messagesCount = mesSel->Count();
        CleanupStack::PopAndDestroy(); // mesSel
        
        // Append Message count
        if ( messagesCount == 1 )
            {
            TBuf<KMcePrintableTextLength> tempString;
            tempString.Append( (*iNumberOfMessagesStrings)[
                KMceListArrayDocAmount] );
            AknTextUtils::LanguageSpecificNumberConversion( tempString );
            aBuffer.Append( tempString );
            }
        else
            {
            TBuf<KMcePrintableTextLength> tempString;
            StringLoader::Format( tempString,
                (*iNumberOfMessagesStrings)[KMceListArrayDocAmountMany],
                -1, // no index in the key string
                messagesCount );
            AknTextUtils::LanguageSpecificNumberConversion( tempString );
            aBuffer.Append( tempString );
            }
        }
    delete folderCEntry;
    }

// ----------------------------------------------------
// CMceMessageListItemArray::SetOrderingL
// Sets sort order in folder entry, overloaded version
// ----------------------------------------------------
void CMceMessageListItemArray::SetOrderingL( TInt aSortType, TBool aOrdering )
    { 
    TMsvSorting sorting = EMsvSortByDateReverse;
    TInt grouping = KMsvGroupByStandardFolders; 
    switch ( aSortType )
        {
        case EMceCmdSortByType:            
            grouping=KMsvGroupByStandardFolders|KMsvGroupByMtm; 
            sorting = aOrdering ? EMsvSortByIdReverse: EMsvSortById;
            break;  
        case EMceCmdSortByRecipient:           
        case EMceCmdSortBySender:
            sorting= aOrdering ? EMsvSortByDetailsReverse: EMsvSortByDetails;
            break;
        case EMceCmdSortBySubject:
            sorting= aOrdering ? EMsvSortByDescriptionReverse: EMsvSortByDescription;
            break;        
        case EMceCmdSortByDate:
        default:            
            sorting= aOrdering ? EMsvSortByDate: EMsvSortByDateReverse;
            break;
        }

    const TMsvSelectionOrdering newOrdering( grouping, sorting, EFalse );
    iFolderEntry->SetSortTypeL( newOrdering );
    // Ignore possible error. If this leaves worst thing is that ordering is not saved to root for message iterator.
    TRAP_IGNORE( SaveOrderingL( newOrdering ) );
    iPreviousSelectionOrdering = newOrdering;
    }
    
// ----------------------------------------------------
// CMceMessageListItemArray::SaveOrderingL
// Write the sorting order into the message store, MUIU accesses and
// uses this for CMessageIterator
// ----------------------------------------------------
void CMceMessageListItemArray::SaveOrderingL( const TMsvSelectionOrdering& aNewOrdering ) const
    {
    // TMsvSelectionOrdering class does not have != operator
    if ( !(iPreviousSelectionOrdering == aNewOrdering ) )
        {
        CMsvEntry* entry=iSession->GetEntryL( KMsvRootIndexEntryId ); // Access the entry
        CleanupStack::PushL( entry );
        CMsvStore* store = entry->EditStoreL();
        CleanupStack::PushL( store );
        RMsvWriteStream writeStream;
        writeStream.AssignLC( *store, KUidMsvMessageListSortOrder );
        aNewOrdering.ExternalizeL( writeStream );
        writeStream.CommitL();
        store->CommitL();
        CleanupStack::PopAndDestroy( 3 );
        }
    }

// ----------------------------------------------------
// CMceMessageListItemArray::SortType
// returns folder's sort type
//
// ----------------------------------------------------
TMsvSelectionOrdering CMceMessageListItemArray::SortType(  )
    {
    return iFolderEntry->SortType();
    }
        
// ----------------------------------------------------
// CMceMessageListItemArray::StripPrefix
// Removes the "Re:" or "Fw:" prefixes from the given 
// text string
// ----------------------------------------------------
TInt CMceMessageListItemArray::SkipPrefix( TPtrC aTextString ) const
    {
    TPtrC searchString( aTextString );
    TPtrC searchStringOrig( aTextString );
    TInt matchInd = searchString.Match( KSortSubjectSkipString );
    TInt matchStart = 0;
    TInt lenRight = searchString.Length() - matchInd - 1;
    
    // Find the last prefix character
    while ( matchInd != KErrNotFound  && lenRight > 0 )
        {
        TPtrC searchString2 = searchString.Right( lenRight );
        matchStart += matchInd + 1;
        matchInd = searchString2.Match( KSortSubjectSkipString );
        lenRight = searchString2.Length() - matchInd - 1;
        searchString.Set( searchString2 );
        }
        
    // Skip the white space after the prefix character
    matchInd = searchStringOrig.Locate(KWhiteSpaceCharacter);
    if ( matchInd!=KErrNotFound && matchStart==matchInd )
        {
        ++matchStart;
        }    
    
    return matchStart;
    }

// ----------------------------------------------------
// CMceMessageListItemArray::AppendBtIrMessageStringL
// ----------------------------------------------------
void CMceMessageListItemArray::AppendBtIrMessageStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const
    {
    HBufC *fileName = AknTextUtils::ConvertFileNameL( aEntry.iDescription );
    StripAndAppendString( aBuffer, *fileName ); // does not leave
	delete fileName;
    }
    
#ifdef RD_READ_MESSAGE_DELETION      
// ----------------------------------------------------
// CMceMessageListItemArray::GetParentFolder
// ----------------------------------------------------
TMsvId CMceMessageListItemArray::GetParentFolderId() const
    {
    return iFolderEntry->Entry().Parent();
    }
#endif // RD_READ_MESSAGE_DELETION  


// ----------------------------------------------------
// CMceMessageListItemArray::
// ----------------------------------------------------
void CMceMessageListItemArray::AppendMessageCountString( TDes& aBuffer, TInt aMessageCount ) const
    {
    aBuffer.Append( KColumnListSeparator );
    if ( aMessageCount == 1 )
        {
        aBuffer.Append( (*iNumberOfMessagesStrings)[
            KMceListArrayDocAmount] );
        }
    else
        {
        TBuf<KMcePrintableTextLength> tempString;
        StringLoader::Format( tempString,
            (*iNumberOfMessagesStrings)[KMceListArrayDocAmountMany],
            -1, // no index in the key string
            aMessageCount );
        aBuffer.Append( tempString );
        }

    }

// ----------------------------------------------------
// CMceMessageListItemArray::
// ----------------------------------------------------
void CMceMessageListItemArray::AppendTimeStringL( TDes& aText, const TMsvEntry& aEntry ) const
    {
//        TBuf<KMceMaxDateString> timeBuffer;
    TBuf<30> dateBuffer;
    TTime timeNow;
    timeNow.UniversalTime();
    
    TTime home;
    home.HomeTime();
    TTimeIntervalMinutes timeZoneDifference;
    TInt err = home.MinutesFrom( timeNow, timeZoneDifference );
    if ( err )
        {
        timeZoneDifference = 0;
        }
    
    timeNow = timeNow + timeZoneDifference;
    TDateTime dateTime = timeNow.DateTime();
    dateTime.SetHour( 0 );
    dateTime.SetMinute( 0 );
    dateTime.SetSecond( 0 );
    dateTime.SetMicroSecond( 0 );
    timeNow = dateTime;
    
    TTime entryTime = aEntry.iDate + timeZoneDifference;

    TTimeIntervalMinutes oneDay( 1440 );
    TInt bufferInt = KMceListArrayDateFormatBuffer; 
    TTimeIntervalMinutes minutesfrom;
    err = entryTime.MinutesFrom( timeNow, minutesfrom );
    if ( err == KErrNone && minutesfrom <= oneDay && minutesfrom.Int() >= 0 )
        {
        // show time
        bufferInt = KMceListArrayTimeFormatBuffer; 
        }
    entryTime.FormatL( dateBuffer, (*iNumberOfMessagesStrings)[bufferInt] ); 
    AknTextUtils::LanguageSpecificNumberConversion( dateBuffer );
    aText.Append( dateBuffer );
    aText.Append( CEditableText::ESpace );
    aText.Append( CEditableText::ESpace );
    }

// ----------------------------------------------------
// CMceMessageListItemArray::
// ----------------------------------------------------
TBool CMceMessageListItemArray::FindFirstUnreadMessageL( TMsvId& aEntryId ) const
    {
    aEntryId = KErrNotFound;
    TInt numberOfUnreadMessages = 0;          
    const TInt itemCount = iFolderEntry->Count();    
    for ( TInt loop = 0; loop < itemCount; loop++ )
        {
        if ( (*iFolderEntry)[loop].iType == KUidMsvMessageEntry &&
             (*iFolderEntry)[loop].Unread() )
            {
            if ( aEntryId == KErrNotFound )
                {
                aEntryId = (*iFolderEntry)[loop].Id();
                }
            numberOfUnreadMessages++;
            if ( numberOfUnreadMessages > 1 )
                {
                // we are only interested of first unread message and info that if
                // there are more than one unread messages.
                // So we can stop if there are more than one.
                break;
                }
            }
        }
    return (numberOfUnreadMessages == 1);
    }

// ----------------------------------------------------
// CMceMessageListItemArray::
// ----------------------------------------------------
TInt CMceMessageListItemArray::MessageCount()
    {
    TInt numberOfMessages = 0;          
    const TInt itemCount = iFolderEntry->Count();    
    for ( TInt loop = 0; loop < itemCount; loop++ )
        {
        if ( (*iFolderEntry)[loop].iType == KUidMsvMessageEntry )
            {
            numberOfMessages++;
            }
        }
    return numberOfMessages;
    }
    
// ----------------------------------------------------
// CMceMessageListItemArray::
// ----------------------------------------------------
TInt CMceMessageListItemArray::FolderCount()
    {
    TInt numberOfFolders = 0;          
    const TInt itemCount = iFolderEntry->Count();    
    for ( TInt loop = 0; loop < itemCount; loop++ )
        {
        if ( (*iFolderEntry)[loop].iType == KUidMsvFolderEntry )
            {
            numberOfFolders++;
            }
        else if ( (*iFolderEntry)[loop].iType == KUidMsvMessageEntry )
            {
            // folders are grouped first, so we can stop when first message is reached.
            break; //loop = itemCount;
            }
        }
    return numberOfFolders;
    }


// ----------------------------------------------------
// CMceMessageListItemArray::CreateEntryIndex()
// ----------------------------------------------------
TInt CMceMessageListItemArray::CreateEntryIndexL() const
    {
    TInt visiblecount;
    TInt itemcount = iFolderEntry->Count();

    CMsvEntrySelection *sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( sel );
     
    //Find email messages 
    CMsvEntrySelection *smtpselection = iFolderEntry->ChildrenWithMtmL( KUidMsgTypeSMTP );
    CleanupStack::PushL( smtpselection );
    
    CMsvEntrySelection *pop3selection = NULL;
    CMsvEntrySelection *imapselection = NULL ;
    CMsvEntrySelection *cmailselection = NULL ;
    
    if(!iEmailFramework)
        {
        pop3selection = iFolderEntry->ChildrenWithMtmL( KUidMsgTypePOP3 );
        CleanupStack::PushL( pop3selection );
        imapselection = iFolderEntry->ChildrenWithMtmL( KUidMsgTypeIMAP4 );
        CleanupStack::PushL( imapselection );
        }
    else
        {
        cmailselection = iFolderEntry->ChildrenWithMtmL( 
                TUid::Uid(KUidMsgTypeFsMtmVal));
        CleanupStack::PushL( cmailselection );
        }
    
    sel->Reset();
    iMsgIndex->Reset();

    //store all email id's to sel array
    if ( smtpselection!=0 && smtpselection->Count()>0 )
        {
        for( TInt i=0 ; i < smtpselection->Count() ; i++ )
            {
            sel->AppendL( smtpselection->At(i) );
            }
        }
    if(!iEmailFramework)
        {
        if ( pop3selection!=0 && pop3selection->Count()>0 )
            {
            for( TInt i=0 ; i < pop3selection->Count() ; i++ )
                {
                sel->AppendL( pop3selection->At(i) );
                }
            }
        if ( imapselection!=0 && imapselection->Count()>0 )
            {
            for( TInt i=0 ; i < imapselection->Count() ; i++ )
                {
                sel->AppendL( imapselection->At(i) );
                }
            }
        }
    else
        {
        if ( cmailselection!=0 && cmailselection->Count()>0 )
            {
            for( TInt i=0 ; i < cmailselection->Count() ; i++ )
                {
                sel->AppendL( cmailselection->At(i) );
                }
            }
        }
    visiblecount = itemcount - sel->Count();

    TInt i = 0;
    TInt j = visiblecount;

    if ( visiblecount < itemcount )
        {
        do
            {
            if ( sel->Find( (*iFolderEntry)[i].Id() ) == KErrNotFound )
                {
                //add everything else but emails to msg index
                iMsgIndex->AppendL( i );
                j--;
                }
            i++;

            } while ( j > 0 && i < itemcount);    
        }
    
    if(iEmailFramework)
        {
        CleanupStack::PopAndDestroy( cmailselection );       
        }
    else
        {
        CleanupStack::PopAndDestroy(imapselection);
        CleanupStack::PopAndDestroy( pop3selection );
        }
    CleanupStack::PopAndDestroy( smtpselection );
    CleanupStack::PopAndDestroy( sel );

    return visiblecount; 
    }

// ----------------------------------------------------
// CMceListItemArray::GetEntryIndex()
// ----------------------------------------------------
TInt CMceMessageListItemArray::GetEntryIndex( TInt aIndex ) const
    {
    TInt tmpindex = aIndex;

    if ( (iEmailClientIntegration && iIntegratedEmailAppExcist) || iEmailFramework ) 
        {
        if( ( iFolderId == KMsvDraftEntryId || 
                iFolderId == KMsvSentEntryId || 
                iFolderId == KMsvDeletedEntryFolderEntryId || 
                iFolderId == KMsvGlobalInBoxIndexEntryId || iFolderId == KMsvGlobalOutBoxIndexEntryId ) &&
                iMsgIndex->Count() > 0 )
            {
            tmpindex = iMsgIndex->At( aIndex );
            }
        }

    return tmpindex;
    }

//  End of File
