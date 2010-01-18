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
//#include "MtmRegistryObserver.h" // MMtmUiDataRegistryObserver
#include <StringLoader.h>   // StringLoader
#include <mce.rsg>
#include <SenduiMtmUids.h>  // mtm uids
#include <eikenv.h>
#include <centralrepository.h>
#include <messaginginternalcrkeys.h>


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

#include "MceMainViewListItemArrayIncludes.h"
#include <mceextraitems.rsg>
//FS Email stuff
#include <ImumInHealthServices.h>
#include <muiumsvuiserviceutilitiesinternal.h>
#include <MessagingDomainCRKeys.h>
#include <bautils.h>

// CONSTANTS

// LOCAL CONSTANTS AND MACROS
enum
    {
    KMceListArrayDocAmount = 0x0,
    KMceListArrayDocAmountMany,
    KMceListArrayDocAmountTemp,
    KMceListArrayDocAmountTemps,
    KMceListArrayDocNoMessages,
    KMceListArrayNumberOneMessage,
    KMceListArrayNumberOfMsgs, 
    KMceListArrayTimeFormatBuffer,
    KMceListArrayDateFormatBuffer
    };

//cmail update
#define KUidMsgTypeFsMtmVal               0x2001F406

//cmail update

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CMceMainViewListItemArray::CMceMainViewListItemArray(
    CMsvSessionPtr aSession,
    TMsvId /*aFolderId*/,
    TMceListType /*aListType*/,
    CMceBitmapResolver& aBitmapResolver ) :
    CArrayFixFlat<TMceListItem>( KMceListGranuality ),
    iSession( aSession ),
//    KMsvRootIndexEntryId( aFolderId ),
//    iListType( aListType ),
    iBitmapResolver( aBitmapResolver )
    {
    iMailboxArray.Reset();
    iDescriptionLength = iBitmapResolver.DescriptionLength();
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::ConstructL
// Symbian OS default constructor can leave.
// ----------------------------------------------------
void CMceMainViewListItemArray::ConstructL()
    {
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdSelectableEmail ) )
        {
        iSelectableEmail = ETrue;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework ) )
        {
        iEmailFramework = ETrue;
        }
    FeatureManager::UnInitializeLib();
    iCoeEnv = CEikonEnv::Static();
    iUiRegistry = CMtmUiDataRegistry::NewL(*iSession); 
    ReadMainViewItemsFromRepositoryL();
    ReadExtraItemsResourceL();
    
    // read phone number grouping feature flag
    // sort by details sorts remote mailboxes alphabetical order
    iFolderEntry = CMsvEntry::NewL(
        *iSession,
        KMsvRootIndexEntryId,
        TMsvSelectionOrdering(
            KMsvGroupByStandardFolders, EMsvSortByDetails ));
    
    iEmailApi = CreateEmailApiL( &*iSession );
    Reset();
    AddFoldersL();
    iIdleLoader = CMceUtilsIdleClass::NewL( *this );
    iIdleLoader->StartL();
    iNumberOfMessagesStrings = new(ELeave)CDesCArrayFlat( KMceListGranuality );
    TBuf<KMcePrintableTextLength> tempString;
    StringLoader::Load( tempString, R_MCE_DOC_AMOUNT, iCoeEnv );
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_DOC_AMOUNT_MANY, iCoeEnv );
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_DOC_AMOUNT_TEMP, iCoeEnv );
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_DOC_AMOUNT_TEMPS, iCoeEnv );
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_DOC_NO_MESSAGES, iCoeEnv);
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_NUMBER_ONE_MESSAGE, iCoeEnv);
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_NUMBER_OF_MSGS, iCoeEnv);
    iNumberOfMessagesStrings->AppendL( tempString );

    HBufC* timeFormat = iCoeEnv->
        AllocReadResourceLC( R_QTN_TIME_USUAL_WITH_ZERO );
    iNumberOfMessagesStrings->AppendL( *timeFormat );
    CleanupStack::PopAndDestroy();
    
    HBufC* dateFormat = iCoeEnv->
            AllocReadResourceLC( R_QTN_DATE_WITHOUT_YEAR_WITH_ZERO );
    iNumberOfMessagesStrings->AppendL( *dateFormat );
    CleanupStack::PopAndDestroy();
    
    
    // todo
    iListBoxText = HBufC::NewL( iDescriptionLength*3 ); 
    }

// Two-phased constructor.
CMceMainViewListItemArray* CMceMainViewListItemArray::NewL(
    CMsvSessionPtr aSession,
    TMsvId aFolderId,
    TMceListType aListType,
    CMceBitmapResolver& aBitmapResolver )
    {
    CMceMainViewListItemArray* self = new (ELeave) CMceMainViewListItemArray(
        aSession, aFolderId, aListType, aBitmapResolver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }


// Destructor
CMceMainViewListItemArray::~CMceMainViewListItemArray( )
    {
    delete iExtraItemsArray;
    delete iMainViewItems;
    delete iFolderEntry;
    delete iListBoxText;
    delete iNumberOfMessagesStrings;
//    delete iUiRegistry;
    delete iEmailApi;
    iMailboxArray.Reset();
    delete iIdleLoader;
    delete iUiRegistry;
    // iSession deleted elsewhere
    }

// ---------------------------------------------------------------------------
// CMceMainViewListItemArray::MakeItemL()
// ---------------------------------------------------------------------------
//
void CMceMainViewListItemArray::MakeItemL( 
    TMceListItem& aListItem,
    const TMsvEntry& aEntry ) const
    {
    aListItem.iPrintableText.Zero();

    const TMsvId id=aEntry.Id();
    StripAndAppendString( 
        aListItem.iPrintableText,
        aEntry.iDetails );

    aListItem.iMsvId = id;
    aListItem.iExtraItem = EFalse;
    aListItem.iIconIndex = KErrNotFound;    
    }

// ---------------------------------------------------------------------------
// CMceMainViewListItemArray::InsertItemL()
// ---------------------------------------------------------------------------
//
void CMceMainViewListItemArray::InsertItemL( 
    const TMsvEntry& aEntry, 
    const TInt aIndex )
    {
    TMceListItem tempItem;
    MakeItemL( tempItem, aEntry );
    InsertL( aIndex, tempItem );
    }


// ----------------------------------------------------
// CMceMainViewListItemArray::AddItemL
// ----------------------------------------------------
void CMceMainViewListItemArray::AddItemL( const TMsvEntry& aEntry )
    {
    TMceListItem tempItem;
    MakeItemL( tempItem, aEntry );
    AppendL( tempItem );
    }


// ---------------------------------------------------------------------------
// CMceListItemArray::AddFolderItemL()
// ---------------------------------------------------------------------------
//
void CMceMainViewListItemArray::AddFolderItemL( TMsvId aEntryId )
    {
    TMsvId service = KMsvLocalServiceIndexEntryId;
    TMsvEntry entry;
    if ( iSession->GetEntry(aEntryId,service,entry) == KErrNone )
    	{
    	AddItemL( entry );
    	}
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::ItemIndex
// ----------------------------------------------------
TInt CMceMainViewListItemArray::ItemIndex(TMsvId aItemId) const
    {
    TInt foundItem = KErrNotFound;
    const TInt count = Count();

    for( TInt loop = 0; loop < count && foundItem == KErrNotFound ; loop++)
        {
        if ( !At(loop).iExtraItem && At(loop).iMsvId == aItemId )
            {
            foundItem = loop;
            }
        }
    return foundItem;
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::ExtraItemIndex
// ----------------------------------------------------
TInt CMceMainViewListItemArray::ExtraItemIndex( TInt aItemId ) const
    {
    TInt foundItem = KErrNotFound;
    const TInt count = Count();

    for( TInt loop = 0; loop < count && foundItem == KErrNotFound ; loop++)
        {
        if ( At(loop).iExtraItem && At(loop).iMsvId == aItemId )
            {
            foundItem = loop;
            }
        }
    return foundItem;
    }

// ----------------------------------------------------
// CMceListItemArray::ItemId
// ----------------------------------------------------
TMsvId CMceMainViewListItemArray::ItemId( TInt aIndex ) const
    {
    TMsvId id = KErrNotFound;
    if ( aIndex >= 0 && aIndex < Count() )
        {
        if ( !At(aIndex).iExtraItem )
            {
            id = At(aIndex).iMsvId;
            }
        }
    return id;
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::Item
// ----------------------------------------------------
const TMceListItem CMceMainViewListItemArray::Item( TInt aIndex ) const
    {
    TMceListItem tempItem;
    tempItem.iMsvId = At(aIndex).iMsvId;
    tempItem.iExtraItem = At(aIndex).iExtraItem;
    return tempItem;
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::AddExtraItemL
// ----------------------------------------------------
void CMceMainViewListItemArray::AddExtraItemL( TMceListItem& aNewItem, TInt aIndex )
    {
    TMcePrintableText tempText = aNewItem.iPrintableText; // let's save string

    aNewItem.iExtraItem = ETrue;
    // make sure it will not be mixed with items in msgs
    TMceBitmapListItem bitmapItem;
    bitmapItem.iExtraItem = ETrue;
    bitmapItem.iMtm.iUid = KUidMsvLocalServiceMtmValue;
    bitmapItem.iType.iUid = KUidMsvFolderEntryValue;
    bitmapItem.iId = aNewItem.iMsvId;
    const TInt index = iBitmapResolver.BitmapIndex( bitmapItem );
    aNewItem.iPrintableText.Zero();
    aNewItem.iPrintableText.AppendNum( index );
    aNewItem.iPrintableText.Append( KColumnListSeparator );
    aNewItem.iPrintableText.Append( tempText );

    InsertL( aIndex, aNewItem );
    }


// ----------------------------------------------------
// CMceMainViewListItemArray::AddExtraItemWithIconL
// ----------------------------------------------------
void CMceMainViewListItemArray::AddExtraItemWithIconL( 
    TMceListItem& aNewItem, 
    TInt aIndex, 
    TInt aIconIndex,
    TUid aApplicationUid )
    {
    TMcePrintableText tempText = aNewItem.iPrintableText; // let's save string
    TMcePrintableText secondarytxt = aNewItem.iSecondaryText;
    aNewItem.iExtraItem = ETrue;
    // make sure it will not be mixed with items in msgs
    TMceBitmapListItem bitmapItem;
    bitmapItem.iExtraItem = ETrue;
    bitmapItem.iMtm = aApplicationUid ; //KUidMsvLocalServiceMtmValue;
    bitmapItem.iType = aApplicationUid; //KUidMsvFolderEntryValue;
    bitmapItem.iId = aNewItem.iMsvId;
    // here to add loading of the icon...
    _LIT( KMceExtraIconsDirAndFile,"MceExtraIcons.mbm" );
    TParse fp;
    fp.Set( KMceExtraIconsDirAndFile, &KDC_APP_BITMAP_DIR , NULL );
    // trap this and if fails, set index = Unknown.
    TInt error = KErrNone;
    
    // Add the bitmap
    TRAP( error, iBitmapResolver.CreateAndAppendIconL( \
        KAknsIIDDefault,\
        fp.FullName(),\
        aIconIndex,\
        &bitmapItem ) );
    
    // Handle the error
    TInt index;
    if ( error == KErrNone )
    	{
    	index = iBitmapResolver.BitmapIndex( bitmapItem );
    	}
    else
    	{
    	index = EMceBitmapIndexUnknown;
    	}
    
    aNewItem.iPrintableText.Zero();
    aNewItem.iPrintableText.AppendNum( index );
    aNewItem.iPrintableText.Append( KColumnListSeparator );
    aNewItem.iPrintableText.Append( tempText );
    aNewItem.iPrintableText.Append( KColumnListSeparator );
    aNewItem.iPrintableText.Append( secondarytxt );

    InsertL( aIndex, aNewItem );
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::MdcaCount
// ----------------------------------------------------
TInt CMceMainViewListItemArray::MdcaCount() const
    {
    return Count();
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::MdcaPoint
// ----------------------------------------------------
TPtrC CMceMainViewListItemArray::MdcaPoint( TInt aIndex ) const
    {
    const TMceListItem& item = At( aIndex );
    if ( item.iExtraItem )
        {
        if ( item.iMsvId == KMceDeliveryReportsId )
            {
            TPtr tempText = iListBoxText->Des();
            tempText.Copy( item.iPrintableText );
            return tempText;
            }
        else
            {
            return item.iPrintableText;
            }
        }
    else
        {
        return MakeStringForMainView( aIndex );
        }
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::MakeStringForMainView
// ----------------------------------------------------
TPtrC CMceMainViewListItemArray::MakeStringForMainView( TInt aIndex ) const
    {
    TPtr tempText = iListBoxText->Des();
    tempText.Zero();
    TMsvEntry entry;
    TMsvId serviceId;
    TInt  visiblecount=0;
    const TMceListItem& item = At( aIndex );
    if ( iSession->GetEntry( item.iMsvId, serviceId, entry ) == KErrNone )
        {
        if ( item.iIconIndex <= KErrNone || item.iIconIndex >= iBitmapResolver.MtmIconIndex() )
            {
            TInt messageCount = 0;
            CONST_CAST( TMceListItem*, &item)->iIconIndex = iBitmapResolver.BitmapIndex( entry, messageCount );
            CONST_CAST( TMceListItem*, &item)->iMessageCount = messageCount;
            }
        tempText.AppendNum( item.iIconIndex );
        tempText.Append( KColumnListSeparator );
        tempText.Append( item.iPrintableText );
        if((iSelectableEmail)||(iEmailFramework))
            {
         if( item.iMsvId == KMsvDraftEntryId || 
                item.iMsvId == KMsvSentEntryId || 
                item.iMsvId == KMsvDeletedEntryFolderEntryId || 
                item.iMsvId == KMsvGlobalInBoxIndexEntryId || item.iMsvId == KMsvGlobalOutBoxIndexEntryId )
               {
         TRAP_IGNORE( visiblecount = FindVisibleCountL(item.iMsvId) );
        AppendMessageCountString( tempText, visiblecount );
               }
			 else
			 {
			 AppendMessageCountString( tempText, item.iMessageCount );
			 }
         }
        else
        {
            AppendMessageCountString( tempText, item.iMessageCount );
        }
        if ( entry.Connected() )
            {
            if ( iRoaming )
                {
                tempText.Append( KColumnListSeparator );
                tempText.AppendNum( (TInt) EMceBitmapIndexAlwaysRoaming );
                }
            else
                {
                tempText.Append( KColumnListSeparator );
                tempText.AppendNum( (TInt) EMceBitmapIndexMailboxConnectionOn );
                }
            }
        else if (  iAlwaysOnline && ( entry.iMtm == KSenduiMtmImap4Uid || 
            entry.iMtm == KSenduiMtmPop3Uid ) )
            {
            TBool dateTime = EFalse;
            TBool aoAccount = EFalse;
            TRAP_IGNORE( dateTime = AlwaysOnlineDateTimeValidL( serviceId, aoAccount ) );
            if ( dateTime )
                {
                // within the update interval
                tempText.Append( KColumnListSeparator );
                if ( iRoaming )
                    {
                    tempText.AppendNum( (TInt) EMceBitmapIndexAlwaysRoaming );
                    }
                else
                    { 
                    tempText.AppendNum( (TInt) EMceBitmapIndexAlwaysAdd );
                    }
                }
            else if ( aoAccount )
                {
                // outside the interval
                tempText.Append( KColumnListSeparator );
                if ( iRoaming )
                    {
                    tempText.AppendNum( (TInt) EMceBitmapIndexAlwaysRoaming );
                    }
                else
                    {
                    tempText.AppendNum( (TInt) EMceBitmapIndexInactiveAdd );
                    }
                }

            }
        }
    else
        {
        // this should never happend
        __ASSERT_DEBUG( 0, Panic(EMceListItemArrayEntryNotFound ) );
        tempText.AppendNum( (TInt)EMceBitmapIndexUnknown );
        tempText.Append( KColumnListSeparator );
        tempText.Append( item.iPrintableText );
        }
    return tempText;
    }

// ---------------------------------------------------------------------------
// CMceMainViewListItemArray::ShouldAddToListL()
// ---------------------------------------------------------------------------
//
TBool CMceMainViewListItemArray::ShouldAddToListL( 
    const TMsvEntry& aEntry,
    const TMsvId aId )
    {
    TBool add =
        // Is it a folder
        ( aEntry.iType == KUidMsvFolderEntry ) ||
        // Is it a local service id
        ( aId == KMsvLocalServiceIndexEntryId ) ||
        // Is it service but not local one?
        ( ( aEntry.iType == KUidMsvServiceEntry && 
            aEntry.Id() != KMsvLocalServiceIndexEntryId ) );

    // Mailboxes are added later, so ignore them here            
    return add ? !iEmailApi->MailboxUtilitiesL().IsMailMtm( aEntry.iMtm, ETrue ) : EFalse;        
    }

// ---------------------------------------------------------------------------
// CMceMainViewListItemArray::CollectServicesL()
// ---------------------------------------------------------------------------
//
TInt CMceMainViewListItemArray::CollectServicesL( const TMsvId aId )
    {    
    const TMsvEntry* tEntry;
    const TInt count = iFolderEntry->Count();
    for (TInt cc = 0; cc < count; cc++)
        {
        tEntry = &( *iFolderEntry )[cc];
        if ( ShouldAddToListL( *tEntry, aId ) )
            {
            AddItemL( *tEntry );            
            }
        }
        
    return Count();
    }

// ---------------------------------------------------------------------------
// CMceMainViewListItemArray::AddMailboxesL()
// ---------------------------------------------------------------------------
void CMceMainViewListItemArray::AddMailboxesL( const TInt /*aMailboxIndex*/ )
    {
    // add mailboxes only if smtp mtm is installed
    if ( iUiRegistry->IsPresent( KSenduiMtmSmtpUid ) )
        {
        // Get the list of valid IMAP4, POP3, and SyncML mailboxes
        TInt err  = MceUtils::GetHealthyMailboxListL( 
            iEmailApi->HealthServicesL(),
            iMailboxArray,
            ETrue,
            ETrue,
            ETrue,
            ETrue );

    // for use of CMceMainViewListView::AddExtraItemsL() so that 
    // MceUtils::GetHealthyMailboxListL won't be called twice
        iErrGetHealthyMailBoxList = err;
        iMaiboxArrayCount = iMailboxArray.Count(); 
//        MCELOGGER_WRITE_FORMAT("AddMailboxesL: iMaiboxArrayCount: %d", iMaiboxArrayCount );
        
        if ( iMaiboxArrayCount )
            {
            // Add the items to main view array in alphabetical order        
            for ( TInt mbox = 0; mbox < iMaiboxArrayCount; mbox++)
                {   
                // cmail update
                if(iEmailFramework)
                    {                   
                    //IF mtm UID matches with uidMsgTypeFsMtmVal, skip the entry
                    //cmail should not be visible in MCE main view
                    TMsvId service;
                    TMsvEntry entry;
                    User::LeaveIfError( iSession->GetEntry( iMailboxArray[mbox], service, entry ) );
                    if(entry.iMtm.iUid == KUidMsgTypeFsMtmVal)
                        {
                        continue;
                        }
                    }
                //cmail update
                AddItemL( iFolderEntry->ChildDataL( iMailboxArray[mbox] ) );
                }            
            }
        else
            {
            //Cmail update
            // If iEmailFramework is true , donot show "MailBox"
            // even if selectable email is true.
            if(!iEmailFramework)
                {
                if ( !iSelectableEmail  || ( iSelectableEmail && 
                        !MsvUiServiceUtilitiesInternal::OtherEmailMTMExistL( *iSession ) ) )
                    {
                    // add special mailbox entry
                    AddExtraItemFromResourceL( R_MCE_MAIN_VIEW_NO_MAILBOXES, KMceNoMailboxesListId );
                    }
                }
            }
        }
        
    }

// ---------------------------------------------------------------------------
// CMceMainViewListItemArray::AddExtraItemFromResourceL()
// ---------------------------------------------------------------------------
//
void CMceMainViewListItemArray::AddExtraItemFromResourceL( TInt aResourceId, TInt aCommandId )
    {
    TMceListItem tempItem;
    tempItem.iExtraItem = ETrue;
    tempItem.iMsvId = aCommandId;
    StringLoader::Load( tempItem.iPrintableText, aResourceId, iCoeEnv );

    HBufC* line2text = NULL;
    switch ( aResourceId )
        {
        case R_MCE_MAIN_VIEW_DELIVERY_REPORTS:
            line2text = StringLoader::LoadL( R_MCE_MAIN_VIEW_DELIVERY_INFORMATION, iEikonEnv );
        break;
        case R_MCE_MAIN_VIEW_NO_MAILBOXES:
            line2text = StringLoader::LoadL( R_MCE_MAIN_VIEW_NO_MAILBOXES_DEFINED, iEikonEnv );
        break;
        default:
            // others does not have line2...
        break;
        }
    
    if ( line2text )
        {
        tempItem.iPrintableText.Append( KColumnListSeparator );
        tempItem.iPrintableText.Append( *line2text );
        delete line2text;
        line2text = NULL;
        }
    
    // always add at the end
    AddExtraItemL( tempItem, Count() );
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::AddFoldersL
// Adds the children of TMsvId aId to the listbox
// ----------------------------------------------------
void CMceMainViewListItemArray::AddFoldersL( )
    {
    Reset();
    TInt count = iMainViewItems->Count();
    TInt extraItemCommandCount = 0;
    
    for ( TInt i=0; i<count; i++ )
        {
        TMsvId folderId = (*iMainViewItems)[i];
        switch ( folderId )
            {
            case KMceWriteMessageListIdValue:
                AddExtraItemFromResourceL( R_MCE_MAIN_VIEW_WRITE_MESSAGE, folderId );
            break;
            case KMceNoMailboxesListIdValue:
                AddMailboxesL( KErrNotFound );
                //AddExtraItemFromResourceL();
            break;
            case KMceDeliveryReportsListIdValue:
                AddExtraItemFromResourceL( R_MCE_MAIN_VIEW_DELIVERY_REPORTS, folderId );
            break;
            default:
            if ( folderId > KMsvRootIndexEntryIdValue )
                {
                AddFolderItemL( folderId );
                }
             else if ( folderId == KMceExtraItemPlaceHolderValue )
                {
                AddExtraItemCommandL( extraItemCommandCount );
                extraItemCommandCount++;
                }
            break;
            }
        }
    }
    
// ----------------------------------------------------
// CMceMainViewListItemArray::AddInboxDocumentsL
// ----------------------------------------------------
void CMceMainViewListItemArray::AddInboxDocumentsL( )
    {
    iFolderEntry->SetEntryL( KMsvLocalServiceIndexEntryId );
    AddItemL( iFolderEntry->
        ChildDataL( KMsvGlobalInBoxIndexEntryId ) );
    AddItemL( iFolderEntry->ChildDataL( KMceDocumentsEntryId ) );
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::AddDraftSentOutboxL
// ----------------------------------------------------
void CMceMainViewListItemArray::AddDraftSentOutboxL( )
    {
    iFolderEntry->SetEntryL( KMsvLocalServiceIndexEntryId );
    AddItemL( iFolderEntry->ChildDataL( KMsvDraftEntryId ) );
    AddItemL( iFolderEntry->ChildDataL( KMsvSentEntryId ) );
    AddItemL( iFolderEntry->
    ChildDataL( KMsvGlobalOutBoxIndexEntryId ) );
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::HandleSessionEventL
// ----------------------------------------------------
void CMceMainViewListItemArray::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    switch (aEvent)
        {
    case EMsvEntriesCreated:
    case EMsvEntriesChanged:
    case EMsvEntriesDeleted:
    case EMsvMediaChanged:
    case EMsvMediaUnavailable:
    case EMsvMediaAvailable:
        Reset();
        AddFoldersL();
        break;

    case EMsvCloseSession:        
        break;

    default:
        break;
        }
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::StripAndAppendString
// ----------------------------------------------------
void CMceMainViewListItemArray::StripAndAppendString(TDes& aTargetString, const TDesC& aSourceString) const
    {
    TRAP_IGNORE( MceUtils::StripAndAppendStringL(
        aTargetString,
        aSourceString,
        iDescriptionLength ) );
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::IsFolderRemote
// return ETrue, if remote folder
//
// ----------------------------------------------------
TBool CMceMainViewListItemArray::IsFolderRemote() const
    {
    TMsvId service = KMsvLocalServiceIndexEntryId;
    TMsvEntry entry;
    if ( iSession->GetEntry(KMsvRootIndexEntryId,service,entry) == KErrNone )
    	{
    	return KMsvLocalServiceIndexEntryId != service;
    	}
    else
    	{
    	return EFalse;
    	}
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::ConstructMainViewL
// add folders in main view
//
// ----------------------------------------------------

void CMceMainViewListItemArray::ConstructMainViewL()
    {
    delete iFolderEntry;
    iFolderEntry = NULL;
    iFolderEntry = CMsvEntry::NewL(
        *iSession,
        KMsvRootIndexEntryId,
        TMsvSelectionOrdering(
            KMsvGroupByStandardFolders, EMsvSortByDetails ) );
    AddFoldersL();
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::FolderId
// return folder id
//
// ----------------------------------------------------
TMsvId CMceMainViewListItemArray::FolderId() const
    {
    return KMsvRootIndexEntryId;
    }


// ----------------------------------------------------
// CMceMainViewListItemArray::IsConnected
// return ETrue, if account is connected
//
// ----------------------------------------------------
TBool CMceMainViewListItemArray::IsConnected() const
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
// CMceMainViewListItemArray::AlwaysOnlineDateTimeValidL
// 
//
// ----------------------------------------------------
TBool CMceMainViewListItemArray::AlwaysOnlineDateTimeValidL( const TMsvId aAccount, TBool &aoDefined ) const
    {
    // Check if the always online is turned on
    TInt64 flags = 0;    
    iEmailApi->MailboxUtilitiesL().QueryAlwaysOnlineStateL( 
        aAccount, flags );
                    
    aoDefined = 
        ( flags & MImumInMailboxUtilities::EFlagTurnedOn ) ? ETrue : EFalse;            
    return ( flags & MImumInMailboxUtilities::EFlagWaitingToConnect ) ? 
        ETrue : EFalse;
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::AlwaysOnlineAccountL
//
// ----------------------------------------------------
TBool CMceMainViewListItemArray::AlwaysOnlineAccountL( 
	const TMsvId /*aAccount*/, 
    const CImumInSettingsData& aSettingsData ) const
    {
    TInt online = 0;
    aSettingsData.GetAttr( TImumDaSettings::EKeyAutoRetrieval, online );
    return online != TImumDaSettings::EValueAutoOff;
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::SetRoaming
// sets roaming flag
//
// ----------------------------------------------------
void CMceMainViewListItemArray::SetRoaming( TBool aRoaming )
    {
    iRoaming = aRoaming;
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::Roaming
// gets roaming flag
//
// ----------------------------------------------------
TBool CMceMainViewListItemArray::Roaming() const
    {
    return iRoaming;
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::SetAlwaysOnline
// sets always online flag
//
// ----------------------------------------------------
void CMceMainViewListItemArray::SetAlwaysOnline( TBool aAlwaysOnline )
    {
    iAlwaysOnline = aAlwaysOnline;
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::GetEmailApi
// Returns the reference to the email API
// ----------------------------------------------------
CImumInternalApi* CMceMainViewListItemArray::GetEmailApi()
    {
    return iEmailApi;
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::
// ----------------------------------------------------
void CMceMainViewListItemArray::AppendMessageCountString( TDes& aBuffer, TInt aMessageCount ) const
    {
    aBuffer.Append( KColumnListSeparator );
    if ( aMessageCount == 0)
        {
        aBuffer.Append( (*iNumberOfMessagesStrings)[
            KMceListArrayDocNoMessages] );
        }
    else if ( aMessageCount == 1 )
        {
        TBuf<KMcePrintableTextLength> tempString;
        tempString.Append((*iNumberOfMessagesStrings)[
            KMceListArrayNumberOneMessage] );
        AknTextUtils::LanguageSpecificNumberConversion( tempString );
        aBuffer.Append( tempString );
        }
    else
        {
        TBuf<KMcePrintableTextLength> tempString;
        StringLoader::Format( tempString,
            (*iNumberOfMessagesStrings)[KMceListArrayNumberOfMsgs],
            -1, // no index in the key string
            aMessageCount );
        AknTextUtils::LanguageSpecificNumberConversion( tempString );
        aBuffer.Append( tempString );
        }
    }
    

// ----------------------------------------------------
// CMceMainViewListItemArray::GetHealthyMailboxListResult( TInt& aCount )
// ----------------------------------------------------    
TInt CMceMainViewListItemArray::GetHealthyMailboxListResult( TInt& aCount )
    {
    aCount = iMaiboxArrayCount;
    return iErrGetHealthyMailBoxList;
    }  

// ----------------------------------------------------
// CMceMainViewListItemArray::
// ----------------------------------------------------
void CMceMainViewListItemArray::StartListUpdateL()
    {
    const TInt count = Count();
    TInt i = 0;
    for ( i = 0; i < count; i++ )
        {
        At( i ).iIconIndex = KErrNotFound;
        }
    iIdleCounter = 0; // update only item which icon index is not valid
    if ( iIdleLoader )
        {
        iIdleLoader->StartL();
        }
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::
// ----------------------------------------------------
void CMceMainViewListItemArray::ResetIconIndexL( TInt aIndex )
    {
    iIdleCounter = 0;
    At( aIndex ).iIconIndex = KErrNotFound;
    if ( iIdleLoader )
        {
        iIdleLoader->StartL();
        }
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::
// ----------------------------------------------------
TBool CMceMainViewListItemArray::MMceUtilsIdleClassStepL( )
    {
    const TInt count = Count();
    // iIdleCounter set to 0 in functions StartListUpdateL and ResetIconIndexL
    for ( ; iIdleCounter < count; iIdleCounter++ )
        {
        TMceListItem& item = At( iIdleCounter );
        if ( !item.iExtraItem && ( item.iIconIndex <= KErrNone || item.iIconIndex > iBitmapResolver.MtmIconIndex() ) )
            {
            TMsvEntry entry;
            TMsvId serviceId;
            if ( iSession->GetEntry( item.iMsvId, serviceId, entry ) == KErrNone )
                {
                TInt messageCount = 0;
                item.iIconIndex = iBitmapResolver.BitmapIndex( entry, messageCount );
                item.iMessageCount = messageCount;
//                        item.iUnreadMessageCount = unreadMessageCount;
                if ( iListItemArrayObserver )
                    {
                    iListItemArrayObserver->MceListItemArrayChangedL();
                    }
//                    iIdleCounter++;
                break;
                }
            }
        }
    return iIdleCounter < count;
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::
// ----------------------------------------------------
void CMceMainViewListItemArray::SetListItemArrayObserver( MMceMainViewListItemArrayObserver* aObserver )
    {
    iListItemArrayObserver = aObserver;
    }

// ----------------------------------------------------
// CMceMainViewListView::ReadExtraItemsResourceL
// ----------------------------------------------------
void CMceMainViewListItemArray::ReadExtraItemsResourceL()
    {
    if ( !iExtraItemsArray )
        {
        iExtraItemsArray = new( ELeave ) CMceMainViewExtraItemArray( KMceListGranuality );
        }
    else
        {
        iExtraItemsArray->Reset();
        }
    
    _LIT(KDirAndMceExtraItemsResFileName,"mceextraitems.rsc");
            TFileName fileName; 
            TFindFile finder( iCoeEnv->FsSession() );
            //Search Y->A then Z:
            TInt err = finder.FindByDir( KDirAndMceExtraItemsResFileName, KDC_APP_RESOURCE_DIR );
            if(err == KErrNone)
                {
                fileName = finder.File();
                BaflUtils::NearestLanguageFile(iCoeEnv->FsSession(),fileName);
                RConeResourceLoader resources( *iCoeEnv );

                TInt err = KErrNone;
                TRAP( err, resources.OpenL(fileName) );

                if ( err == KErrNone )
                    {
                    TResourceReader reader;
                    iEikonEnv->CreateResourceReaderLC( reader, R_MCE_EXTRA_ITEMS );

                    const TInt count = reader.ReadInt16();
                    TMceMainViewExtraItem extraItem;
                    extraItem.iCommandId = KMceFirstExtraItemId;
                    for ( TInt loop = 0; loop < count; loop++ )
                        {
                        HBufC* txt = reader.ReadHBufCL();   // message text
                        extraItem.iPrintableText.Copy( *txt );
                        delete txt;
                        txt = NULL;
                        txt = reader.ReadHBufCL();   // message text
                        extraItem.iCommandText.Copy( *txt );
                        delete txt;

                        TInt depricatedResource = reader.ReadInt16(); // this is not used anymore, but left in resource so no DC break.
                        extraItem.iIconIndex = reader.ReadInt16();
                        TUid applicationUid;
                        applicationUid.iUid = reader.ReadUint32();
                        extraItem.iApplicationUid = applicationUid.iUid;
                        // secondary text
                        txt = reader.ReadHBufCL();   //secondary text                       
                        if(txt)
                            {
                            extraItem.iSecondaryText.Copy( *txt );
                            delete txt;
                            txt = NULL;
                            }   
                        else
                            {
                            extraItem.iSecondaryText.Copy(KNullDesC());
                            }
                        iExtraItemsArray->AppendL( extraItem );
                        extraItem.iCommandId++;
                        }
                    CleanupStack::PopAndDestroy(); // reader
                    resources.Close();
                    }
                }
            }


// ----------------------------------------------------
// CMceMainViewListView::ExtraItemCommandLC
// ----------------------------------------------------
HBufC* CMceMainViewListItemArray::ExtraItemCommandLC( TInt aCommandId )
    {
    HBufC* commandText = HBufC::NewLC( KMcePrintableTextLength );
    TPtr tempText = commandText->Des();
    
    const TInt count = iExtraItemsArray->Count();
    for (TInt i=0; i<count; i++ )
        {
        if ( aCommandId == (*iExtraItemsArray)[i].iCommandId )
            {
            tempText.Copy( (*iExtraItemsArray)[i].iCommandText );
            i=count;
            }
        }
    
    return commandText;    
    }
    
// ----------------------------------------------------
// CMceMainViewListView::AddExtraItemCommandL
// ----------------------------------------------------
void CMceMainViewListItemArray::AddExtraItemCommandL( TInt aIndex )
    {
    if ( aIndex < iExtraItemsArray->Count() )
        {
        const TMceMainViewExtraItem& extraItem = (*iExtraItemsArray)[aIndex];
        TMceListItem tempItem;
        tempItem.iExtraItem = ETrue;
        tempItem.iMsvId = extraItem.iCommandId;
       	tempItem.iPrintableText.Copy( extraItem.iPrintableText );
       	tempItem.iSecondaryText.Copy(extraItem.iSecondaryText);
       	TUid appUid;
       	appUid.iUid = extraItem.iApplicationUid;
       	AddExtraItemWithIconL( tempItem, Count(), extraItem.iIconIndex, appUid );
        }
    }

// ----------------------------------------------------
// CMceMainViewListView::ReadMainViewItemsFromRepositoryL
// ----------------------------------------------------
void CMceMainViewListItemArray::ReadMainViewItemsFromRepositoryL()
    {
    if ( !iMainViewItems )
        {
        iMainViewItems = new( ELeave ) CMsvEntrySelection();
        }
    else
        {
        iMainViewItems->Reset();        
        }
    CRepository* repository = CRepository::NewLC(KCRUidMuiuSettings);
    for ( TInt i = KMuiuMceMainViewRow1; i<=KMuiuMceMainViewRow16; i++ )
        {
        TInt value = 0;
        if ( repository->Get( i, value ) == KErrNone && value > KErrNotFound )
            {
            iMainViewItems->AppendL( value );
            }
        }
    CleanupStack::PopAndDestroy( repository );
    }

// ----------------------------------------------------
// CMceMainViewListItemArray::FindVisibleCountL()
// ----------------------------------------------------
TInt CMceMainViewListItemArray::FindVisibleCountL(TMsvId serviceId ) const
    {
    TInt visiblecount=0;
    TInt emailcount=0;
    TInt itemcount;
    CMsvEntry* entry = iSession->GetEntryL( serviceId );
    itemcount=entry->Count();
      
    //Find email messages 
    CMsvEntrySelection *smtpselection = entry->ChildrenWithMtmL( KUidMsgTypeSMTP );
    CleanupStack::PushL( smtpselection );

    CMsvEntrySelection *pop3selection = NULL;
    CMsvEntrySelection *imapselection = NULL ;
    CMsvEntrySelection *cmailselection = NULL ;
    
    if(!iEmailFramework)
        {
        pop3selection = entry->ChildrenWithMtmL( KUidMsgTypePOP3 );
        CleanupStack::PushL( pop3selection );
        imapselection = entry->ChildrenWithMtmL( KUidMsgTypeIMAP4 );
        CleanupStack::PushL( imapselection );
        }
    else
        {
        cmailselection = entry->ChildrenWithMtmL( 
                TUid::Uid(KUidMsgTypeFsMtmVal));
        CleanupStack::PushL( cmailselection );
        }

    if ( smtpselection!=0 && smtpselection->Count()>0 )
        {
        emailcount=emailcount+smtpselection->Count();
        }

    if(!iEmailFramework)
        {
        if ( pop3selection!=0 && pop3selection->Count()>0 )
            {
            emailcount=emailcount+pop3selection->Count();
            }
        if ( imapselection!=0 && imapselection->Count()>0 )
            {
            emailcount=emailcount+imapselection->Count();
            }        
        }
    else
        {
        if ( cmailselection!=0 && cmailselection->Count()>0 )
            {
            emailcount=emailcount+cmailselection->Count();
            }
        }

    visiblecount = itemcount - emailcount;

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

    return visiblecount; 
    }
//  End of File
