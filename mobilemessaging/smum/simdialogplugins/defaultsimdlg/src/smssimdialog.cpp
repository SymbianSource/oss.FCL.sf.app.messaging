/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: sms sim dlg implementation
 *
 */



// INCLUDES
#include <eikmenup.h>
#include <aknViewAppUi.h>               // iAvkonViewAppUi
#include <AknWaitDialog.h>              // CAknWaitDialog
#include <eikprogi.h>                   // CEikProgressInfo
#include <AknQueryDialog.h>             // CAknQueryDialog
#include <aknlists.h>
#include <StringLoader.h>               // StringLoader
#include <avkon.mbg>                    // bitmaps
#include <akntitle.h>                   // CAknTitlePane
#include <AknIconArray.h>               // CAknIconArray
#include <hlplch.h>                     // For HlpLauncher 
#include <bldvariant.hrh>
#include <featmgr.h>                    // FeatureManager
#include <data_caging_path_literals.hrh>
// message includes
#include <msvapi.h>                     // for CMsvSession
#include <akninputblock.h>				// CAknInputBlock
#include <MsgFolderSelectionDialog.h>   // CMsgFolderSelectionDialog
#include <MuiuOperationWait.h>          // CMuiuOperationWait
#include <muiu.mbg>                     // bitmaps
#include <smscmds.h>                    // for async sms commands
#include <smutset.h>                    // for TSmsProgress

#include <defaultsimdialog.rsg>                     // for resource ids
#include <csxhelp/mce.hlp.hrh>
#include <aknconsts.h>                  // KAvkonBitmapFile
#include <AknsUtils.h>
#include <AknIconUtils.h> 				// AknIconUtils::AvkonIconFileName()
#include <AknsConstants.h>
#include <AknUtils.h>
#include <aknnotewrappers.h>              //for displayingnote
#include <Profile.hrh>                    // for profile state
#include <ProfileEngineSDKCRKeys.h>
#include <centralrepository.h>    
#include <bautils.h>  // BaflUtils
#include <ConeResLoader.h> // RConeResourceLoader

#include "simdeletecopyoperation.h"    // for CSimDeleteCopyOperation
#include "SMSU.H"                          // for CSmsMtmUi
#include "smssimdialog.h"                  // for CSmsSimDialog
#include "SMSU.HRH"            // for command ids
//For logging
#include "SmumLogging.h"

//  LOCAL CONSTANTS AND MACROS
const TInt KSimMessageBitmapArrayGranularity = 4;
const TInt KSimMessageTempArrayGranularity = 4;
const TInt KSmumSimDialogMessageBitmapIndex = 1;

_LIT( KSimDlgResourceFile, "defaultsimdialog.rsc" );
_LIT( KMuiuBitmapFile, "Muiu.MBM" );
_LIT( KSmsiMarkReplacement, "1");

//
//
// CSimMessageArray
//
//

// ----------------------------------------------------
// CSimMessageArray::CSimMessageArray
// ----------------------------------------------------
CSimMessageArray::CSimMessageArray( CMsvSession& aSession ):
iSession( aSession )
                    {
                    }

// ----------------------------------------------------
// CSimMessageArray::NewL
// ----------------------------------------------------
CSimMessageArray* CSimMessageArray::NewL( CMsvSession& aSession )
    {
    SMUMLOGGER_ENTERFN(" CSimMessageArray::NewL");
    CSimMessageArray* self = new (ELeave) CSimMessageArray(aSession);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    SMUMLOGGER_LEAVEFN(" CSimMessageArray::NewL");
    return self;
    }

// ----------------------------------------------------
// CSimMessageArray::ConstructL
// ----------------------------------------------------
void CSimMessageArray::ConstructL()
    {
    SMUMLOGGER_ENTERFN(" CSimMessageArray::ConstructL");
    TMsvSelectionOrdering ordering = TMsvSelectionOrdering(
            KMsvNoGrouping,
            EMsvSortByDescription,
            ETrue );

    iSmsServiceEntry = CMsvEntry::NewL( 
            iSession, 
            KMsvRootIndexEntryId, 
            ordering );

    iListBoxText = HBufC::NewL( KSmumMaxDisplayTextLength );
    SMUMLOGGER_LEAVEFN(" CSimMessageArray::ConstructL");
    }


// ----------------------------------------------------
// CSimMessageArray::~CSimMessageArray
// ----------------------------------------------------
CSimMessageArray::~CSimMessageArray()
    {
    SMUMLOGGER_ENTERFN(" CSimMessageArray::~CSimMessageArray");   
    delete iSmsServiceEntry;
    delete iListBoxText;
    SMUMLOGGER_LEAVEFN(" CSimMessageArray::~CSimMessageArray");
    }

// ----------------------------------------------------
// CSimMessageArray::MdcaCount
// ----------------------------------------------------
TInt CSimMessageArray::MdcaCount() const
{
SMUMLOGGER_ENTERFN(" CSimMessageArray::MdcaCount");
if ( iSmsServiceEntry->EntryId() == KMsvRootIndexEntryId )
    {
    SMUMLOGGER_LEAVEFN(" CSimMessageArray::MdcaCount - 0");
    return 0;
    }
else
    {
    SMUMLOGGER_LEAVEFN(" CSimMessageArray::MdcaCount - COUNT");
    return iSmsServiceEntry->Count();
    }
}

// ----------------------------------------------------
// CSimMessageArray::MdcaPoint
// ----------------------------------------------------
TPtrC CSimMessageArray::MdcaPoint(TInt aIndex) const
{
SMUMLOGGER_ENTERFN(" CSimMessageArray::MdcaCount(INDEX)");
// prepare 
TPtr tempText = iListBoxText->Des();
tempText.Zero();
const TMsvEntry* child = NULL;
child = &(*iSmsServiceEntry)[ aIndex ];

// construct listbox-string
// bitmap
tempText.AppendNum( KSmumSimDialogMessageBitmapIndex );
tempText.Append( KColumnListSeparator );
// sender-information
if ( child->iDetails.Length() > KSmumMaxVisibleChars )
    {
    tempText.Append( child->iDetails.Left( KSmumMaxVisibleChars-1 ) );
    tempText.Append( CEditableText::EEllipsis );
    }
else
    {
    tempText.Append( child->iDetails );
    }

// msg "subject"
tempText.Append( KColumnListSeparator );
if ( child->iDescription.Length() > KSmumMaxVisibleChars )
    {
    tempText.Append( child->iDescription.Left( KSmumMaxVisibleChars-1 ) );
    tempText.Append( CEditableText::EEllipsis );
    }
else
    {
    tempText.Append( child->iDescription );
    }

SMUMLOGGER_LEAVEFN(" CSimMessageArray::MdcaCount(INDEX)");
return tempText;
}


// ----------------------------------------------------
// CSimMessageArray::EntryId
// ----------------------------------------------------
TMsvId CSimMessageArray::EntryId( TInt aIndex ) const
{
return (*iSmsServiceEntry)[ aIndex ].Id();
}

// ----------------------------------------------------
// CSimMessageArray::RefreshArrayL
// ----------------------------------------------------
void CSimMessageArray::RefreshArrayL( TMsvId aFolderId )
    {
    iSmsServiceEntry->SetEntryL( KMsvDeletedEntryFolderEntryId );
    iSmsServiceEntry->SetEntryL( aFolderId );
    }



//
//
// CSmsSimDialog
//
//

// ----------------------------------------------------
// CSmsSimDialog::CSmsSimDialog
// ----------------------------------------------------
CSmsSimDialog::CSmsSimDialog(CSmsMtmUi&  aSmsMtmUi, CMsvSession& aSession, TInt& aExitCode):
iSmsMtmUi(aSmsMtmUi),
iSession(aSession),
iExitCode( aExitCode ),
iDialogDismissed( EFalse ),
iHelpFeatureSupported( EFalse ),
iMsgCountBeforeOperation( 0 ),
iResourceFileOffset(0),
iContextCommand(EFalse)
        {
        SMUMLOGGER_ENTERFN(" CSmsSimDialog::CSmsSimDialog");
        iExitCode = KErrNone;
        iMarkReplacementText.Copy( KSmsiMarkReplacement );
        SMUMLOGGER_LEAVEFN(" CSmsSimDialog::CSmsSimDialog");
        }

// ----------------------------------------------------
// CSmsSimDialog::~CSmsSimDialog
// ----------------------------------------------------
CSmsSimDialog::~CSmsSimDialog()
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::~CSmsSimDialog");
    if( iResourceFileOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
        }

    delete iOldTitleText;
    delete iEnumerationOperation;
    delete iCopyOrDeleteOperation;
    delete iProgressDialog;
    delete iWaitDialog;
    delete iMessages;
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::~CSmsSimDialog");
    }

// ----------------------------------------------------
// CSmsSimDialog::NewL
// ----------------------------------------------------
CSmsSimDialog* CSmsSimDialog::NewL(CSmsMtmUi&  aSmsMtmUi, CMsvSession& aSession, TInt& aExitCode)
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::NewL");
    CSmsSimDialog* self= new(ELeave) CSmsSimDialog(aSmsMtmUi, aSession, aExitCode);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::NewL");
    return self;
    }

// ----------------------------------------------------
// CSmsSimDialog::ConstructL
// ----------------------------------------------------
void CSmsSimDialog::ConstructL()
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::ConstructL");

    // Resource loader
    CEikonEnv* env = CEikonEnv::Static();    
    TParse fp;
    fp.Set( KSimDlgResourceFile, &KDC_RESOURCE_FILES_DIR , NULL );
    TFileName filename = fp.FullName();    
    BaflUtils::NearestLanguageFile(env->FsSession(), filename);
    iResourceFileOffset = env->AddResourceFileL(filename); 

    CAknDialog::ConstructL( R_SMS_DEFAULT_SIM_DIALOG_MENUBAR );
    iMessages = CSimMessageArray::NewL( iSession );

    // Prepare FeatureManager, read values to members and uninitialize FeatureManager
    FeatureManager::InitializeLibL();
    iHelpFeatureSupported = FeatureManager::FeatureSupported( KFeatureIdHelp );
    FeatureManager::UnInitializeLib();
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::ConstructL");
    }

// ----------------------------------------------------
// CSmsSimDialog::PreLayoutDynInitL
// ----------------------------------------------------
void CSmsSimDialog::PreLayoutDynInitL()                        
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::PreLayoutDynInitL");
    CEikColumnListBox* listBox = ListBox();
    listBox->SetListBoxObserver( this );

    // Create scroll bar
    listBox->CreateScrollBarFrameL();
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    // Set data
    listBox->Model()->SetItemTextArray(iMessages);
    listBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
    HBufC* noMessages = StringLoader::LoadLC( R_SMS_DEFAULT_SIM_DIALOG_EMPTY, iCoeEnv );
    listBox->View()->SetListEmptyTextL( *noMessages );
    CleanupStack::PopAndDestroy( noMessages );
    noMessages = NULL;

    StoreTitleTextAndSetNewL();

    // create icon array
    CArrayPtr< CGulIcon >* icons = 
    new( ELeave ) CAknIconArray( KSimMessageBitmapArrayGranularity );
    CleanupStack::PushL( icons );
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();

    TFileName fileName;
    fileName.Copy( KAvkonBitmapFile );
    icons->AppendL( 
            AknsUtils::CreateGulIconL( skinInstance, 
                    KAknsIIDQgnIndiMarkedAdd,
                    fileName, 
                    EMbmAvkonQgn_indi_marked_add, 
                    EMbmAvkonQgn_indi_marked_add_mask ) );
    // Parse the filename
    TParse tp;
    tp.Set( KMuiuBitmapFile, &KDC_APP_BITMAP_DIR, NULL );
    fileName.Copy( tp.FullName() );
    icons->AppendL( 
            AknsUtils::CreateGulIconL( skinInstance, 
                    KAknsIIDQgnPropMceSmsRead,
                    fileName, 
                    EMbmMuiuQgn_prop_mce_sms_read, 
                    EMbmMuiuQgn_prop_mce_sms_read_mask ) );
    // Set the icon array. takes ownership
    reinterpret_cast<CAknDoubleGraphicStyleListBox*> 
    (listBox)->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
    CleanupStack::Pop(); // icons
    // Start the enumeration
    EnumerateSimMessagesL();
    iMenuBar->SetContextMenuTitleResourceId( R_SMS_DEFAULT_SIM_DIALOG_CONTEXTMENUBAR );    
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::PreLayoutDynInitL");
    }

// ----------------------------------------------------
// CSmsSimDialog::OkToExitL
// ----------------------------------------------------
TBool CSmsSimDialog::OkToExitL(TInt aButtonId)
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::OkToExitL");
    CAknInputBlock::NewLC(); // Absorb keys
    TInt returnValue = 0;

    switch(aButtonId)
        {
        case EAknSoftkeyEmpty:
            {
            returnValue = EFalse;
            }
            break;
        case EAknSoftkeyOptions:
            {
            iMenuBar->SetMenuType(CEikMenuBar::EMenuOptions);  
            iMenuBar->TryDisplayMenuBarL();
            returnValue = EFalse;
            }
            break;
        case EAknSoftkeyContextOptions:
            {
			iContextCommand = ETrue;
            // Middle softkey was pressed. Display context menu.
            iMenuBar->SetMenuType(CEikMenuBar::EMenuContext);
            iMenuBar->TryDisplayContextMenuBarL();
            returnValue = EFalse;
            }
            break;
        default:
            {
            TBool okToExit = CAknDialog::OkToExitL( aButtonId );
            if ( okToExit )
                {
                TRAP_IGNORE( DeleteSimFolderL() );
                if ( aButtonId != EEikBidCancel )
                    {
                    TRAP_IGNORE( RestoreTitleTextL() );
                    }
                }
            returnValue = okToExit;
            }
            break;

        }      

    CleanupStack::PopAndDestroy(); // CAknInputBlock
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::OkToExitL");
    return returnValue;
    }

// ----------------------------------------------------
// CSmsSimDialog::ProcessCommandL
// ----------------------------------------------------
void CSmsSimDialog::ProcessCommandL( TInt aCommandId )
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::ProcessCommandL");
    CAknDialog::ProcessCommandL( aCommandId ) ;
    CEikColumnListBox* listBox = ListBox();
    CRepository* repository = CRepository::NewLC( KCRUidProfileEngine );
    TInt defaultType( EProfileGeneralId );
    User::LeaveIfError( repository->Get( KProEngActiveProfile, defaultType ) );
    CleanupStack::PopAndDestroy( repository );

    switch ( aCommandId )
        {
        case ESmsMtmUiCommandIdCopy:
            //General system error pops up when deleting a SIM message in offline mode
            if ( defaultType == EProfileOffLineId )
                {            	
                // Show note and exit 
                HBufC* noteString = StringLoader::LoadLC( R_QTN_OFFLINE_NOT_POSSIBLE, iCoeEnv );
                CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );                
                note->ExecuteLD( *noteString );
                CleanupStack::PopAndDestroy(); // noteString 
                TryExitL( EAknSoftkeyBack );               
                }
            else
                {   	      
                HandleCopyFromSimToLocalFolderL();
                }
            break;
        case ESmsMtmUiCommandIdDelete:
            //General system error pops up when deleting a SIM message in offline mode
            if ( defaultType == EProfileOffLineId )
                {               
                // Show note and exit 
                HBufC* noteString = StringLoader::LoadLC( R_QTN_OFFLINE_NOT_POSSIBLE, iCoeEnv );
                CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );                
                note->ExecuteLD( *noteString );
                CleanupStack::PopAndDestroy(); // noteString
                TryExitL( EAknSoftkeyBack );                                
                }
            else
                {    
                HandleDeleteFromSimL();
                }
            break;
        case ESmsMtmUiCommandIdEditMark:
            {
#ifdef _DEBUG
            RDebug::Print(_L("CSmsSimDialog::ProcessCommandL: mark item %d"), 
                    listBox->CurrentItemIndex() );
#endif
            listBox->View()->SelectItemL( listBox->CurrentItemIndex() );
            break;
            }
        case ESmsMtmUiCommandIdEditUnmark:
            {
#ifdef _DEBUG
            RDebug::Print(_L("CSmsSimDialog::ProcessCommandL: unmark item %d"), 
                    listBox->CurrentItemIndex() );
#endif
            listBox->View()->DeselectItem( listBox->CurrentItemIndex() );
            break;
            }
        case ESmsMtmUiCommandIdEditUnmarkAll:
            {
#ifdef _DEBUG
            RDebug::Print(_L("CSmsSimDialog::ProcessCommandL: unmark all") );
#endif

            listBox->View()->ClearSelection();
            break;
            }
        case ESmsMtmUiCommandIdEditMarkAll:
            {
#ifdef _DEBUG
            RDebug::Print(_L("CSmsSimDialog::ProcessCommandL: mark all") );
#endif

            CArrayFixFlat<TInt>* indexArray = new( ELeave ) CArrayFixFlat<TInt>( 
                    KSimMessageTempArrayGranularity );
            CleanupStack::PushL( indexArray );
            const TInt count = listBox->Model()->NumberOfItems();
            for ( TInt loop = 0; loop < count; loop++ )
                {
                indexArray->AppendL( loop );
                }
            listBox->View()->SetSelectionIndexesL( indexArray );
            CleanupStack::PopAndDestroy(); // indexArray
            break;
            }
        case EAknCmdHelp:
            {
            LaunchHelpL();                
            }
            break;
        case ESmsSettingsChoiceMenuExit:
            {
            iExitCode = EEikCmdExit;
            TryExitL( EAknCmdExit );
            }
            break;

        default :
            break;
        }
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::ProcessCommandL");
    }
// ----------------------------------------------------
// CSmsSimDialog::DynInitMenuPaneL
// ----------------------------------------------------
void CSmsSimDialog::DynInitMenuPaneL(
        TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::DynInitMenuPaneL - 2");
    if ( aResourceId == R_SMS_DEFAULT_SIM_DIALOG_MENUPANE )
        {
        if ( ListBox()->Model()->NumberOfItems() == 0 )
            {
            aMenuPane->DeleteMenuItem( ESmsMtmUiCommandIdEdit );
            }

        aMenuPane->SetItemDimmed( EAknCmdHelp, !iHelpFeatureSupported );                      
        }
    else if ( aResourceId == R_MCE_DEFAULT_SIM_DIALOG_EDIT_CASCADE )
        {
        DynInitEditMenuL( aMenuPane );
        }
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::DynInitMenuPaneL - 2");
    }

// ----------------------------------------------------
// CSmsSimDialog::OfferKeyEventL
// ----------------------------------------------------
TKeyResponse CSmsSimDialog::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::OfferKeyEventL");
    CEikColumnListBox* listBox = ListBox();
    if ( listBox && aType == EEventKey && aKeyEvent.iCode == EKeyBackspace &&
            listBox->Model()->NumberOfItems() > 0 )
        {
        HandleDeleteFromSimL();
        return EKeyWasConsumed;
        }
    if ( aType == EEventKey && aKeyEvent.iCode == EKeyOK )
        {
        if ( ListBox()->Model()->NumberOfItems() )
            {
            iMenuBar->SetMenuType(CEikMenuBar::EMenuContext);
            iMenuBar->TryDisplayContextMenuBarL();                
            }        	
        return EKeyWasConsumed;
        }
    if( aType == EEventKeyUp )
        {
        UpdateMskL( );
        }

    // Exit if waitnote is cancelled & dismissed
    if ( iDialogDismissed && iEnumerationOperation )
        {
        TryExitL( EAknSoftkeyBack );
        iDialogDismissed = EFalse;
        return EKeyWasConsumed;
        }

    return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
    }


// ----------------------------------------------------
// CSmsSimDialog::DynInitEditMenuL
// ----------------------------------------------------
void CSmsSimDialog::DynInitEditMenuL(  CEikMenuPane* aMenuPane )
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::DynInitEditMenuL(MEUPANE)");
    CEikColumnListBox* listBox = ListBox();

    // check what state item under "cursor" is
    const CListBoxView::CSelectionIndexArray* items = 
    listBox->SelectionIndexes();
    TKeyArrayFix key(0, ECmpTInt);  
    TInt count = items->Count();
    if ( 0 == count )
        {
        //if no selection, disable unmarkall
        aMenuPane->SetItemDimmed( ESmsMtmUiCommandIdEditUnmarkAll, ETrue );
        }   
    if ( count == listBox->Model()->NumberOfItems() )
        {
        //if all selected, disable markall
        aMenuPane->SetItemDimmed( ESmsMtmUiCommandIdEditMarkAll, ETrue );
        } 
    if(iContextCommand)
        {
        aMenuPane->SetItemDimmed( ESmsMtmUiCommandIdEditMark, EFalse );
        aMenuPane->SetItemDimmed( ESmsMtmUiCommandIdEditUnmark, EFalse );
        iContextCommand = EFalse;
        }
    else
        {
        aMenuPane->SetItemDimmed( ESmsMtmUiCommandIdEditMark, ETrue );
        aMenuPane->SetItemDimmed( ESmsMtmUiCommandIdEditUnmark, ETrue );
        } 
    TInt pos;
    if ( !items->Find(listBox->CurrentItemIndex(), key, pos))
        {
        aMenuPane->DeleteMenuItem( ESmsMtmUiCommandIdEditMark );
        }
    else
        {
        aMenuPane->DeleteMenuItem( ESmsMtmUiCommandIdEditUnmark );
        }
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::DynInitEditMenuL(MEUPANE)");
    }

// ----------------------------------------------------
// CSmsSimDialog::HandleListBoxEventL
// ----------------------------------------------------
void CSmsSimDialog::HandleListBoxEventL(
        CEikListBox* /*aListBox*/, 
        TListBoxEvent aEventType )
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::HandleListBoxEventL");
    switch(aEventType)
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            iContextCommand = ETrue;
            TryExitL( EAknSoftkeyContextOptions );
            break;
        default:
            break;
        }
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::HandleListBoxEventL");
    }

// ----------------------------------------------------
// CSmsSimDialog::DeleteSimFolderL
// ----------------------------------------------------
void CSmsSimDialog::DeleteSimFolderL()
    {
    if ( iSimFolderId != KMsvNullIndexEntryId )
        {
        SMUMLOGGER_ENTERFN(" CSmsSimDialog::DeleteSimFolderL");
        CMsvEntry* entry = iSession.GetEntryL(iSimFolderId);
        CleanupStack::PushL(entry);
        TMsvSelectionOrdering ordering = entry->SortType();
        ordering.SetShowInvisibleEntries( ETrue );
        entry->SetSortTypeL( ordering );
        entry->SetEntryL( entry->OwningService() );   // now entry points to SMS service
        CAknInputBlock::NewLC();
        CMuiuOperationWait* waiter =
        CMuiuOperationWait::NewLC();
        CMsvOperation* deleteOperation = 
        entry->DeleteL(iSimFolderId, waiter->iStatus);
        waiter->Start();
        delete deleteOperation;
        CleanupStack::PopAndDestroy(3, entry);  // entry, waiter, CAknInputBlock
        iSimFolderId = KMsvNullIndexEntryId;
        }
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::DeleteSimFolderL");
    }



// ----------------------------------------------------
// CSmsSimDialog::HandleCopyFromSimToLocalFolderL
// ----------------------------------------------------
void CSmsSimDialog::HandleCopyFromSimToLocalFolderL()
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::HandleCopyFromSimToLocalFolderL");

    TMsvId destinationId = 0;
    HBufC* title = StringLoader::LoadLC(R_SMS_DEFAULT_SIM_DIALOG_COPY_MESSAGES, iCoeEnv);
    if ( !CMsgFolderSelectionDialog::SelectFolderL( destinationId, *title ) )
        {
        CleanupStack::PopAndDestroy( title );
        return;
        }

    CleanupStack::PopAndDestroy( title );

    LaunchNoteAndOperationL( destinationId );
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::HandleCopyFromSimToLocalFolderL");
    }

// ----------------------------------------------------
// CSmsSimDialog::CreateSelectionArrayLC
// ----------------------------------------------------
CMsvEntrySelection* CSmsSimDialog::CreateSelectionArrayLC()
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::CreateSelectionArrayLC");
    CEikColumnListBox* listBox = ListBox();
    const TInt selectionCount = listBox->View()->SelectionIndexes()->Count();

    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection();
    CleanupStack::PushL( selection );
    if ( selectionCount )
        {
        // selected items
        const CListBoxView::CSelectionIndexArray* items = 
        listBox->SelectionIndexes();
        const TInt count = items->Count();
        for ( TInt loop = 0; loop < count; loop++ )
            {
            selection->AppendL( iMessages->EntryId( items->At( loop ) ) );
            }
        }
    else
        {
        // item under cursor
        const TInt index = ListBox()->CurrentItemIndex();
        selection->AppendL( iMessages->EntryId(index) );
        }
#ifdef _DEBUG
    const TInt count2 = selection->Count();
    RDebug::Print(_L("CSmsSimDialog::CreateSelectionArrayLC: items %d"), 
            count2 );
    for ( TInt loop2 = 0; loop2 < count2; loop2++ )
        {
        RDebug::Print(_L("item %d id %d"), 
                loop2, selection->At( loop2 ) );
        }
#endif
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::CreateSelectionArrayLC");
    return selection;
    }

// ----------------------------------------------------
// CSmsSimDialog::HandleDeleteFromSimL
// ----------------------------------------------------
void CSmsSimDialog::HandleDeleteFromSimL()
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::HandleDeleteFromSimL");
    HBufC* messageText = NULL;
    const TInt messageCount = ListBox()->View()->SelectionIndexes()->Count();
    if ( messageCount == 1 || messageCount == 0 )
        {
        messageText = StringLoader::LoadL( 
                R_DEFAULT_SIM_MESSAGE_DELETE_CONFIRMATION_ONE_MESSAGE, iCoeEnv);
        }
    else
        {
        messageText = StringLoader::LoadL( 
                R_DEFAULT_SIM_MESSAGE_DELETE_CONFIRMATION_MANY_MESSAGES, 
                messageCount, 
                iCoeEnv);
        }
    CleanupStack::PushL( messageText );
    CAknQueryDialog* confDialog = CAknQueryDialog::NewL( );
    if ( !confDialog->ExecuteLD( R_DEFAULT_SIM_MESSAGE_DELETE_CONFIRMATION, *messageText ) )
        {
        CleanupStack::PopAndDestroy( messageText );
        SMUMLOGGER_LEAVEFN(" CSmsSimDialog::HandleDeleteFromSimL - 1");
        return;
        }
    CleanupStack::PopAndDestroy( messageText );

    LaunchNoteAndOperationL();
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::HandleDeleteFromSimL - 2");
    }

// ----------------------------------------------------
// CSmsSimDialog::UpdateSimMessagesArrayL
// ----------------------------------------------------
void CSmsSimDialog::UpdateSimMessagesArrayL()
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::UpdateSimMessagesArrayL");
    const TInt index = ListBox()->CurrentItemIndex();
    iMessages->RefreshArrayL( iSimFolderId );

    ListBox()->View()->ClearSelection();
    const TInt count = iMessages->MdcaCount();
    if ( iMsgCountBeforeOperation < count )
        { // msg count increased
        ListBox()->HandleItemAdditionL();
        }
    else if ( iMsgCountBeforeOperation > count )
        { // msg count decreased
        if ( index == KErrNotFound && count )
            {
            ListBox()->SetCurrentItemIndex( count - 1 );
            }
        ListBox()->HandleItemRemovalL();
        }

    // update msg count
    iMsgCountBeforeOperation = iMessages->MdcaCount();
    UpdateMskL();
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::UpdateSimMessagesArrayL");
    }

// ----------------------------------------------------
// CSmsSimDialog::ListBox
// ----------------------------------------------------
CEikColumnListBox* CSmsSimDialog::ListBox() const
{
return((CEikColumnListBox*)Control(EMsgControlIdSimMessageListBox));
}

// ----------------------------------------------------
// CSmsSimDialog::EnumerateSimMessagesL
// ----------------------------------------------------
void CSmsSimDialog::EnumerateSimMessagesL()
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::EnumerateSimMessagesL");
    delete iWaitDialog;
    iWaitDialog = NULL;
    iWaitDialog = new (ELeave) CAknWaitDialog(
            (REINTERPRET_CAST(CEikDialog**, &iWaitDialog)),
            ETrue
    );
    CleanupStack::PushL(TCleanupItem(CleanupWaitDialog, 
            (TAny**)&iWaitDialog));
    iWaitDialog->SetCallback( this );
    iWaitDialog->ExecuteLD( R_DEFAULT_SIM_DIALOG_WAIT_NOTE );

    // Find the service entry
    CMsvEntry* serviceEntry = iSession.GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(serviceEntry);
    TMsvId serviceId;
    TSmsUtilities::ServiceIdL(*serviceEntry, serviceId, KUidMsgTypeSMS);

    // Enumerate sim messages
    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection();
    CleanupStack::PushL(selection);
    selection->AppendL(serviceId);

    CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL(singleOpWatcher);
    TBuf8<1> foo (KNullDesC8);
    CAknInputBlock::NewLC();  // no need to store the pointer returned, It's on the Cleanup Stack
    CMsvOperation* operation = iSmsMtmUi.BaseMtm().InvokeAsyncFunctionL(
            ESmsMtmCommandEnumeratePhoneStores,
            *selection, 
            foo, 
            singleOpWatcher->iStatus);
    singleOpWatcher->SetOperation( operation );
    iEnumerationOperation = singleOpWatcher;
    CleanupStack::PopAndDestroy(); // CAknInputBlock
    CleanupStack::Pop( singleOpWatcher );
    CleanupStack::PopAndDestroy( 2, serviceEntry ); // serviceEntry, selection
    CleanupStack::Pop(); // TCleanupItem - everything ok so don't delete iWaitDialog
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::EnumerateSimMessagesL");
    }


// ----------------------------------------------------
// CSmsSimDialog::StoreTitleTextAndSetNewL
// ----------------------------------------------------
void CSmsSimDialog::StoreTitleTextAndSetNewL()
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::StoreTitleTextAndSetNewL");
    CEikStatusPane* sp = ((CAknAppUi*)CEikonEnv::Static()->EikAppUi())
    ->StatusPane();
    // Fetch pointer to the default title pane control
    CAknTitlePane* title=(CAknTitlePane *)sp->ControlL(
            TUid::Uid(EEikStatusPaneUidTitle));

    delete iOldTitleText;
    iOldTitleText = NULL;
    iOldTitleText = HBufC::NewL( title->Text()->Length() + 1 );
    TPtr tempText = iOldTitleText->Des();
    tempText.Copy( (*title->Text()) );

    HBufC* newTitle = StringLoader::LoadLC( 
            R_SMS_DEFAULT_SIM_DIALOG_TITLE_TEXT, 
            iCoeEnv );
    title->SetTextL( *newTitle );
    CleanupStack::PopAndDestroy( newTitle );
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::StoreTitleTextAndSetNewL");
    }

// ----------------------------------------------------
// CSmsSimDialog::RestoreTitleTextL
// ----------------------------------------------------
void CSmsSimDialog::RestoreTitleTextL()
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::RestoreTitleTextL");
    CEikStatusPane* sp = ((CAknAppUi*)CEikonEnv::Static()->EikAppUi())
    ->StatusPane();
    // Fetch pointer to the default title pane control
    CAknTitlePane* title=(CAknTitlePane *)sp->ControlL(
            TUid::Uid(EEikStatusPaneUidTitle));

    title->SetTextL( iOldTitleText->Des() );
    delete iOldTitleText;
    iOldTitleText = NULL;
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::RestoreTitleTextL");
    }

// ----------------------------------------------------
// CSmsSimDialog::OpCompleted
// ----------------------------------------------------
void CSmsSimDialog::OpCompleted(
        CMsvSingleOpWatcher& aOpWatcher,
        TInt aCompletionCode )
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::OpCompleted");
    TBool tryToExit = EFalse;
    CMsvOperation* op=&aOpWatcher.Operation();
    TRAP_IGNORE( tryToExit = DoOperationCompletedL(op, aCompletionCode ) );  
    if ( tryToExit )
        {
        return;
        }

    if ( iEnumerationOperation->Operation().Id()==op->Id() )
        {
#ifdef _DEBUG
        RDebug::Print(_L("CSmsSimDialog::OpCompleted, completed enumerate operation") );
#endif
        delete iEnumerationOperation;
        iEnumerationOperation = NULL;
        }
    else if ( iCopyOrDeleteOperation->Operation().Id()==op->Id() )
        {
#ifdef _DEBUG
        RDebug::Print(_L("CSmsSimDialog::OpCompleted, completed copy/delete operation") );
#endif
        delete iCopyOrDeleteOperation;
        iCopyOrDeleteOperation = NULL;
        delete iProgressDialog;
        iProgressDialog = NULL;
        }

    if ( !iEnumerationOperation && !iCopyOrDeleteOperation )
        {
        delete iWaitDialog;
        iWaitDialog = NULL;
        delete iProgressDialog;
        iProgressDialog = NULL;
        }
    ListBox()->DrawDeferred();
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::OpCompleted");
    }

// ----------------------------------------------------
// CSmsSimDialog::DoOperationCompletedL
// ----------------------------------------------------
TBool CSmsSimDialog::DoOperationCompletedL(
        CMsvOperation* aOperation, 
        TInt /*aCompletionCode*/ )
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::DoOperationCompletedL");
    if ( !aOperation )
        {
        SMUMLOGGER_LEAVEFN(" CSmsSimDialog::DoOperationCompletedL - FALSE -0");
        return EFalse;
        }

    TSmsProgressBuf progressBuf;
    progressBuf.Copy(aOperation->ProgressL());
    TSmsProgress progress = progressBuf();
    iSmsMtmUi.DisplayProgressSummary(progressBuf);
    if ( iEnumerationOperation )
        {
#ifdef _DEBUG
        RDebug::Print(_L("CSmsSimDialog::DoOperationCompletedL, enumeration completed %d"), progress.iError );
#endif
        if ( !progress.iError)
            {
            iSimFolderId = progress.iEnumerateFolder;
            UpdateSimMessagesArrayL();
            }
        else
            {
            delete iWaitDialog;
            iWaitDialog = NULL;
            TryExitL( EAknSoftkeyBack );
            SMUMLOGGER_LEAVEFN(" CSmsSimDialog::DoOperationCompletedL - TRUE");
            return ETrue;
            }
        }

    if ( iCopyOrDeleteOperation )
        {
#ifdef _DEBUG
        RDebug::Print(_L("CSmsSimDialog::DoOperationCompletedL, delete/copy completed %d"), progress.iError );
#endif
        EnumerateSimMessagesL();
        }
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::DoOperationCompletedL - FALSE");
    return EFalse;
    }

// ----------------------------------------------------
// CSmsSimDialog::CleanupWaitDialog
// ----------------------------------------------------
void CSmsSimDialog::CleanupWaitDialog(TAny* aAny)
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::CleanupWaitDialog");
    CEikDialog** dialog = (CEikDialog**) aAny;
    if (dialog && *dialog)
        {
        delete *dialog;
        *dialog = NULL;
        }
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::CleanupWaitDialog");
    }

// ----------------------------------------------------
// CSmsSimDialog::DialogDismissedL
// ----------------------------------------------------
void CSmsSimDialog::DialogDismissedL( TInt /*aButtonId*/ )
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::DialogDismissedL");
    if ( iEnumerationOperation )
        {
#ifdef _DEBUG
        RDebug::Print(_L("CSmsSimDialog::DialogDismissedL, cancelling enumerate operation") );
#endif
        iDialogDismissed = ETrue;
        }
    else if ( iCopyOrDeleteOperation )
        {
#ifdef _DEBUG
        RDebug::Print(_L("CSmsSimDialog::DialogDismissedL, cancelling copy or delete operation") );
#endif
        CSimDeleteCopyOperation* oper = STATIC_CAST(CSimDeleteCopyOperation*, &iCopyOrDeleteOperation->Operation());
        oper->CancelAfterCurrentCompleted();
        }
#ifdef _DEBUG
    RDebug::Print(_L("CSmsSimDialog::DialogDismissedL, finish") );
#endif
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::DialogDismissedL");
    }

// ----------------------------------------------------
// CSmumMainSettingsDialog::LaunchHelpL
// launch help using context
// 
// ----------------------------------------------------
void CSmsSimDialog::LaunchHelpL()
    {   
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::LaunchHelpL");
    if ( iHelpFeatureSupported )
        {
        CCoeAppUi* editorAppUi = STATIC_CAST(CCoeAppUi*, ControlEnv()->AppUi());     
        CArrayFix<TCoeHelpContext>* helpContext = editorAppUi->AppHelpContextL();   
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
        }
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::LaunchHelpL");
    }


// ----------------------------------------------------
// CSmumMainSettingsDialog::GetHelpContext
// returns helpcontext as aContext
// 
// ----------------------------------------------------
void CSmsSimDialog::GetHelpContext
(TCoeHelpContext& aContext) const
{  
const TUid KUidMce ={0x100058C5};      
aContext.iMajor = KUidMce;
aContext.iContext = KMCE_HLP_SIM;         
}

// ----------------------------------------------------
// CSmsSimDialog::LaunchNoteAndOperationL
// ----------------------------------------------------
void CSmsSimDialog::LaunchNoteAndOperationL( TMsvId aDestinationId )
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::LaunchNoteAndOperationL");
    // store msg count before operation
    iMsgCountBeforeOperation = iMessages->MdcaCount();

    delete iProgressDialog;
    iProgressDialog = NULL;
    iProgressDialog = new (ELeave) CAknProgressDialog(
            (REINTERPRET_CAST(CEikDialog**, &iProgressDialog)),
            ETrue
    );
    CleanupStack::PushL(TCleanupItem(CleanupWaitDialog, 
            (TAny**)&iProgressDialog));
    iProgressDialog->SetCallback( this );
    CEikImage* icon = NULL;
    icon = new (ELeave) CEikImage;
    CleanupStack::PushL(icon);   
    TFileName fileName = AknIconUtils::AvkonIconFileName();  //avkon.mbm file name and path

    //select which icon to show   
    const TInt bitmapIndex = aDestinationId ? 
        EMbmAvkonQgn_note_copy : EMbmAvkonQgn_note_erased;
    const TInt maskIndex = aDestinationId ? 
        EMbmAvkonQgn_note_copy_mask : EMbmAvkonQgn_note_erased_mask;

    const TAknsItemID id = aDestinationId ? 
        KAknsIIDQgnNoteCopy : KAknsIIDQgnNoteErased;

    CFbsBitmap* targetBitmap;
    CFbsBitmap* targetBitmapMask;
    AknsUtils::CreateIconL( AknsUtils::SkinInstance(), id , targetBitmap,
            targetBitmapMask, fileName, bitmapIndex, maskIndex );

    //set icon
    icon->SetPicture(targetBitmap, targetBitmapMask);
    iProgressDialog->SetImageL(icon);
    CleanupStack::Pop(); // icon   
    iProgressDialog->ExecuteLD( R_DEFAULT_SIM_DIALOG_PROGRESS_NOTE );

    CMsvEntrySelection* selection = CreateSelectionArrayLC();
    CAknInputBlock::NewLC(); // no need to store the pointer returned, It's on the Cleanup Stack
    CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL(singleOpWatcher);

    CMsvOperation* operation = NULL;
    if ( aDestinationId )
        { // copy operation 
        operation = CSimDeleteCopyOperation::NewL(
                iSession, 
                iSmsMtmUi, 
                singleOpWatcher->iStatus, 
                selection,
                aDestinationId, 
                *iProgressDialog, 
                iCoeEnv );
        }
    else
        { // delete operation
        operation = CSimDeleteCopyOperation::NewL(
                iSession, 
                iSmsMtmUi, 
                singleOpWatcher->iStatus, 
                selection,
                *iProgressDialog, 
                iCoeEnv );
        }

    singleOpWatcher->SetOperation( operation );
    iCopyOrDeleteOperation = singleOpWatcher;
    CleanupStack::Pop( singleOpWatcher );
    CleanupStack::PopAndDestroy( 2, selection ); // CAknInputBlock
    CleanupStack::Pop( ); // CleanupWaitDialog
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::LaunchNoteAndOperationL");
    }

// ----------------------------------------------------
// CSmsSimDialog::UpdateMskL
// ----------------------------------------------------    
void CSmsSimDialog::UpdateMskL()
    {
    SMUMLOGGER_ENTERFN(" CSmsSimDialog::UpdateMskL");
    TInt resId = 0;
    const TInt KMskPosition = 3;

    if( ListBox()->Model()->NumberOfItems() == 0 ) 
        {//show empty button
        resId = R_DEFAULT_SIM_MSK_BUTTON_EMPTY;
        }
    else
        {
        resId = R_DEFAULT_SIM_MSK_BUTTON_CONTEXTMENU;
        }

    CEikButtonGroupContainer &cba = ButtonGroupContainer();
    cba.SetCommandL( KMskPosition, resId );
    cba.DrawDeferred();
    SMUMLOGGER_LEAVEFN(" CSmsSimDialog::UpdateMskL");
    }
//  End of File
