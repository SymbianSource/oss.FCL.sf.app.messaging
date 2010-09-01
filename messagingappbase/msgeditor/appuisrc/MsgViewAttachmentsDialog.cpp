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
* Description:  MsgViewAttachmentsDialog implementation
*
*/



// ========== INCLUDE FILES ================================
#include <msvapi.h>
#include <eikspane.h>
#include <eikmenup.h>
#include <AknDef.h>
#include <aknappui.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <aknlists.h>
#include <gulicon.h>
#include <coeutils.h>               // for ConeUtils::FileExists
#include <bautils.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>          // CAknWaitDialog
#include <DocumentHandler.h>
#include <StringLoader.h>           // StringLoader
#include <AiwCommon.h>
#include <NpdApi.h>

#include <mmsvattachmentmanager.h>

#include <avkon.hrh>

#include <aknlayout.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include <AknUtils.h>
#include <AknsUtils.h>          // skinned icons
#include <AknsConstants.h>      // skinned icon ids

#include "MsgEditorCommon.h"        // for KMsgEditorMbm
#include "MsgEditorDocument.h"
#include "MsgAttachmentInfo.h"
#include "MsgAttachmentModel.h"
#include "MsgViewAttachmentsDialog.h"
#include "MsgAttachmentUtils.h"

#include <msgeditor.mbg>
#include "MsgEditorAppUi.hrh"
#include <MsgEditorAppUi.rsg>       // resouce identifiers

#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

_LIT8( KTextDataType, "text/plain" );
const TUid KNotepadUID3 = { 0x1000599d };

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::CMsgViewAttachmentsDialog
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgViewAttachmentsDialog::CMsgViewAttachmentsDialog(
    TDesC& aTitle, TInt aMenuId, CMsgAttachmentModel& aAttachmentModel )
    : iAttachmentModel( aAttachmentModel ),
      iNewTitle( aTitle ),
      iMenuId( aMenuId )
    {
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::~CMsgViewAttachmentsDialog
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgViewAttachmentsDialog::~CMsgViewAttachmentsDialog()
    {
    delete iOldTitle;

    // because it's not possible to set externally owned icon array
    // for listbox, we delete our icon array here and set listbox
    // icon array to NULL.
    delete iIconArray;
    if ( iListBox )
        {
        TRAP_IGNORE( iListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( NULL ) );
        }

    delete iListItems;
    delete iDocHandler;

    // remove empty navi pane.
    if ( iNaviPane )
        {
        iNaviPane->Pop();
        }
    delete iOpenParamList;
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::ConstructL()
    {
    DoConstructL();
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::ConstructL( CMsvEntry& aMessageEntry )
    {
    iMessageEntry = &aMessageEntry;
    DoConstructL();
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::DoConstructL
// ---------------------------------------------------------
//
void CMsgViewAttachmentsDialog::DoConstructL()
    {
    CAknDialog::ConstructL( R_MEB_ATTACHMENTS_MENUBAR );

    iDocHandler = CDocumentHandler::NewL( iEikonEnv->Process() );

    // replace navi pane with empty one.
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( iEikonEnv->AppUiFactory()->StatusPane()->ControlL(
            TUid::Uid( EEikStatusPaneUidNavi ) ) );
    // CAknNavigationControlContainer internal array of decorators seem to
    // have big enough granularity that it does not need realloc
    // and therefore PushDefaultL does not leave.
    iNaviPane->PushDefaultL();

    iDocHandler->SetExitObserver( this );
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::CurrentListItemIndex
//
//
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgViewAttachmentsDialog::CurrentListItemIndex()
    {
    return iListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::UpdateIcon
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::UpdateIcon( TInt aListIndex )
    {
    iListBox->DrawItem( aListIndex );
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::ProcessCommandL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::ProcessCommandL( TInt aCommandId )
    {
    HideMenu();

    TFileName filename;
    TInt index = iListBox->CurrentItemIndex();

    switch ( aCommandId )
        {
        case EMsgViewAttachmentCmdOpen:
            {
            if ( iBusy )
                {
                return;
                }
            
            if ( index != KErrNotFound )
                {
                iBusy = ETrue;
                CleanupStack::PushL( TCleanupItem( CleanupPointer, &iBusy ) );
                CMsgAttachmentInfo& attInfo =
                    iAttachmentModel.AttachmentInfoAt( index );

                TDataType datatype( attInfo.DataType() );
                RFile file = iAttachmentModel.GetAttachmentFileL( index );
                
                TInt ret( KErrNone );
                                               
                if ( datatype == KTextDataType() )
                    {
                                        
                    TInt charset( attInfo.CharacterSet() );
                    
                    CleanupClosePushL( file );
                    
                    // file handle ownership transferred.
                    ret = CNotepadApi::ExecFileViewerL( file, 
                                                        NULL,
                                                        ETrue,
                                                        charset != KErrNotFound ? EFalse : ETrue,
                                                        charset );
                    
                    CleanupStack::Pop( &file );
                    }
                else
                    {
                    CleanupClosePushL( file );
                    
                    if ( iOpenParamList )
                        {
                        TRAP( ret,iDocHandler->OpenFileEmbeddedL( file, datatype, *iOpenParamList ) );
                        }
                    else
                        {
                        TRAP( ret,iDocHandler->OpenFileEmbeddedL( file, datatype ) );
                        }
                    
                    CleanupStack::PopAndDestroy( &file );
                    }
                // sets iBusy to EFalse.
               CleanupStack::PopAndDestroy(); // CleanupPointer
                
                switch ( ret )
                    {
                    case KErrNone:
                        {
                        if (datatype != KTextDataType())
                            {
                                iBusy = ETrue;
                            }
                            
                        }
                    case KUserCancel:
                        {
                        break;
                        }
                    case KErrNoMemory:
                    case KErrDiskFull:
                        {
                        User::Leave( ret );
                        }
                    default:
                        {
                        CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
                        HBufC* text = StringLoader::LoadLC( R_QTN_MMS_INFO_CANNOT_OPEN, iCoeEnv );
                        note->ExecuteLD( *text );
                        CleanupStack::PopAndDestroy(); // text
                        }
                    }
                }
            break;
            }
        case EMsgViewAttachmentCmdSave:
            {
            if ( index != KErrNotFound )
                {
                // Save to the default location of this type
                TInt ret = KErrNone;
                CMsgAttachmentInfo& attInfo = iAttachmentModel.AttachmentInfoAt( index );
                TDataType datatype( attInfo.DataType() );

                RFile file  = iAttachmentModel.GetAttachmentFileL( index );
                CleanupClosePushL( file );
                
                if ( datatype == KTextDataType() )
                    {
                    ret = DoSaveTextFileL( file, attInfo.CharacterSet() );
                    }
                else
                    {
                    TFileName dst;
                    User::LeaveIfError( file.Name( dst ) );
                    ret = iDocHandler->CopyL( file, dst, datatype, KEntryAttNormal );
                    }
                
                CleanupStack::PopAndDestroy( &file );
                
                if ( ret != KErrNone && ret != KUserCancel )
                    {
                    User::LeaveIfError( ret );
                    }
                else if ( ret == KErrNone )
                    {
                    attInfo.SetSaved( ETrue );
                    }
                }
            break;
            }
        case EMsgViewAttachmentCmdRemove:
            {
            if ( index != KErrNotFound )
                {
                RemoveAttachmentL( index );
                ItemRemovedL( index );
                }
            break;
            }
        case EMsgViewAttachmentCmdInsertImage:
            {
            if ( MsgAttachmentUtils::FetchFileL(
                MsgAttachmentUtils::EImage, filename ) )
                {
                InsertAttachmentL( filename, aCommandId );
                }
            break;
            }
        case EMsgViewAttachmentCmdInsertAudio:
            {
            if ( MsgAttachmentUtils::FetchFileL(
                MsgAttachmentUtils::EAudio, filename ) )
                {
                InsertAttachmentL( filename, aCommandId );
                }
            break;
            }
        case EMsgViewAttachmentCmdInsertVideo:
            {
            if ( MsgAttachmentUtils::FetchFileL(
                MsgAttachmentUtils::EVideo, filename ) )
                {
                InsertAttachmentL( filename, aCommandId );
                }
            break;
            }
        case EMsgViewAttachmentCmdInsertNote:
            {
            if ( MsgAttachmentUtils::FetchFileL(
                MsgAttachmentUtils::ENote, filename ) )
                {
                InsertAttachmentL( filename, aCommandId );
                }
            break;
            }
        case EAknCmdExit:
        case EEikCmdExit:
            {
            static_cast<MEikCommandObserver*>( iEikonEnv->EikAppUi() )
                ->ProcessCommandL( EAknCmdExit );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::DynInitMenuPaneL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::DynInitMenuPaneL(
    TInt aMenuId, CEikMenuPane* aMenuPane )
    {
    if ( aMenuId == R_MEB_ATTACHMENTS_MENUPANE )
        {
        // Add menu items from given menu.
        aMenuPane->AddMenuItemsL( iMenuId, 0, ETrue );
        // Delete our dummy first menu item.
        aMenuPane->DeleteMenuItem( 0 );
        }
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::HandleResourceChangeL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::HandleResourceChange(TInt aType)
    {
    if ( aType == KEikDynamicLayoutVariantSwitch ||
        aType == KAknsMessageSkinChange )
        {
        TRAP_IGNORE( iAttachmentModel.LoadResourcesL() );
        }    
    CEikDialog::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::HandleServerAppExit
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::HandleServerAppExit( TInt /*aReason*/)
    {
    // replace navi pane with empty one.
    TRAP_IGNORE(iNaviPane->PushDefaultL());
    iBusy = EFalse;
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::HandleControlStateChangeL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::HandleControlStateChangeL( TInt aControlId )
    {
    if ( aControlId == EMsgAttachmentListBoxId )
        {
        DoUpdateUiL();
        }
        
    CAknDialog::HandleControlStateChangeL( aControlId );
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::OkToExitL
//
//
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgViewAttachmentsDialog::OkToExitL( TInt aButtonId )
    {
    if ( iBusy )
        {
        return EFalse;
        }

    if ( aButtonId == EAknSoftkeyBack )
        {
        iTitlePane->SetTextL( *iOldTitle );
        }
    else if ( aButtonId == EAknSoftkeyOpen )
        {
        if ( iAttachmentModel.NumberOfItems() != 0 &&
             !iAttachmentModel.AttachmentInfoAt( CurrentListItemIndex() ).IsEmptyAttachment() )
                {
                ProcessCommandL( EMsgViewAttachmentCmdOpen );
                }
                
        return EFalse;
        }

    return CAknDialog::OkToExitL( aButtonId );
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::CreateCustomControlL
//
//
// ---------------------------------------------------------
//
EXPORT_C SEikControlInfo CMsgViewAttachmentsDialog::CreateCustomControlL( TInt aControlType )
    {
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;

    if ( aControlType == EMsgAttachmentListBox )
        {
        controlInfo.iControl = new ( ELeave ) CMsgAttachmentsListBox( iAttachmentModel );
        }

    return controlInfo;
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::HandleListBoxEventL
//
//
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
EXPORT_C void CMsgViewAttachmentsDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/, 
                                                              TListBoxEvent aEventType )
    {
    if ( aEventType == MEikListBoxObserver::EEventItemSingleClicked &&
         !iAttachmentModel.AttachmentInfoAt( CurrentListItemIndex() ).IsEmptyAttachment() )
        {
        ProcessCommandL( EMsgViewAttachmentCmdOpen );
        }
    }
#else
EXPORT_C void CMsgViewAttachmentsDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/, 
                                                              TListBoxEvent /*aEventType*/ )
    {
    }
#endif // RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::PreLayoutDynInitL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::PreLayoutDynInitL()
    {
    CEikStatusPane *sp = static_cast<CAknAppUi*>
        ( iEikonEnv->EikAppUi() )->StatusPane();
    iTitlePane = static_cast<CAknTitlePane*>
        ( sp->ControlL( TUid::Uid(EEikStatusPaneUidTitle ) ) );

    const TDesC* title = iTitlePane->Text();
    iOldTitle = title->AllocL();

    iTitlePane->SetTextL( iNewTitle );

    iListBox = static_cast<CMsgAttachmentsListBox*>
        ( Control( EMsgAttachmentListBoxId ) );

    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    
    iListBox->SetListBoxObserver( this );
    
    LoadFormDataL();
    

    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::LoadFormDataL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::LoadFormDataL()
    {
    // iconarray for two icons.
    // index 0: application icon.
    // index 1: indicator.
    iIconArray = new ( ELeave ) CArrayPtrFlat<CGulIcon>( 2 );

    iListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( iIconArray );

    // list items is not actually an array but object that returns
    // list item text for listbox in MdcaPoint method.
    iListItems = CMsgAttaListItemArray::NewL( iAttachmentModel, iIconArray );
    iListBox->Model()->SetItemTextArray( iListItems );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::ItemAddedL
//
//
// ---------------------------------------------------------
EXPORT_C void CMsgViewAttachmentsDialog::ItemAddedL( TInt aIndex )
    {
    iListBox->View()->CalcBottomItemIndex();
    
    TInt itemIndex = aIndex;
    
    if ( aIndex == -1 )
        {
        itemIndex = Max( iListBox->Model()->NumberOfItems() - 1, 0 );
        }
    
    iListBox->SetCurrentItemIndex( itemIndex );
    
    if ( !iAttachmentModel.AttachmentInfoAt( itemIndex ).IsEmptyAttachment() )
        {
        if ( !ButtonGroupContainer().IsCommandVisible( EAknSoftkeyOpen ) )
            {
            // Set middle button visible if invisible and inserted attachment
            // has content (i.e. it is not empty)
            UpdateButtonVisiblity( EAknSoftkeyOpen, ETrue );
            }
        }
    else
        {
        if ( ButtonGroupContainer().IsCommandVisible( EAknSoftkeyOpen ) )
            {
            // Set middle button invisible if visible and inserted attachment
            // does not hava content (i.e. it is empty)
            UpdateButtonVisiblity( EAknSoftkeyOpen, EFalse );
            }
        }
    
    iListBox->HandleItemAdditionL();
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::ItemRemovedL
//
//
// ---------------------------------------------------------
EXPORT_C void CMsgViewAttachmentsDialog::ItemRemovedL( TInt aIndex )
    {
    iListBox->HandleItemRemovalL();
    TInt lastItem = Max( iListBox->Model()->NumberOfItems() - 1, 0 );

    if ( aIndex - 1 == lastItem )
        {
        TInt numItems = iListBox->View()->NumberOfItemsThatFitInRect(
            iListBox->View()->ViewRect() );
        TInt topItem  = Max( lastItem - numItems, 0 );
        iListBox->SetTopItemIndex( topItem );
        }

    iListBox->SetCurrentItemIndex( Min( aIndex, lastItem ) );
    iListBox->DrawNow();
    
    if ( iAttachmentModel.NumberOfItems() == 0 || 
         iAttachmentModel.AttachmentInfoAt( CurrentListItemIndex() ).IsEmptyAttachment() )
        {
        UpdateButtonVisiblity( EAknSoftkeyOpen, EFalse );
        }
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::InsertAttachmentL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::InsertAttachmentL(
    const TFileName& aFileName,
    TInt /*aCommandId*/,
    TInt aIndex /*= -1*/ )
    {
    iBusy = ETrue;
    // sets iBusy to EFalse on leave.
    CleanupStack::PushL( TCleanupItem( CleanupPointer, &iBusy ) );

    iWaitDialog = new ( ELeave ) CAknWaitDialog(
        reinterpret_cast<CEikDialog**>( &iWaitDialog ),
        ETrue );
    CleanupStack::PushL( TCleanupItem( CleanupWaitDialog, (TAny**)&iWaitDialog ) );

    iWaitDialog->PrepareLC( R_MEB_WAIT_NOTE );

    HBufC* text = StringLoader::LoadLC( R_QTN_MAIL_WAIT_INSERT_ATTACHMENT );
    iWaitDialog->SetTextL( *text );
    CleanupStack::PopAndDestroy(); // text

    iWaitDialog->RunLD();

    // find file size
    TInt filesize = 0;
    RFile file;
    RFs& fs = iEikonEnv->FsSession();

    TInt err = file.Open( fs, aFileName, EFileShareAny );
    if ( err == KErrAccessDenied || err == KErrInUse )
        {
        err = file.Open( fs, aFileName, EFileShareReadersOnly );
        }
    if ( err == KErrNone )
        {
        file.Size( filesize );
        }
    file.Close();

    iAttachmentModel.AddAttachmentL( aFileName, filesize, 0, ETrue, aIndex );

    ItemAddedL( aIndex );

    // will delete iWaitDialog.
    CleanupStack::PopAndDestroy(); // CleanupWaitDialog

    // sets iBusy to EFalse.
    CleanupStack::PopAndDestroy(); // CleanupPointer
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::RemoveAttachmentL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::RemoveAttachmentL( TInt aIndex )
    {
    // delete item from attachment model (notifies its observer about deletion).
    iAttachmentModel.DeleteAttachment( aIndex );
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::CleanupPointer
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::CleanupPointer( TAny* aItem )
    {
    TInt* item = static_cast<TInt*>( aItem );
    *item = EFalse;
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::CleanupWaitDialog
//
//
// ---------------------------------------------------------
//
void CMsgViewAttachmentsDialog::CleanupWaitDialog( TAny* aAny )
    {
    CAknWaitDialog** dialog = (CAknWaitDialog**) aAny;
    if ( dialog && *dialog )
        {
        delete *dialog;
        }
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::SetOpenParamList
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgViewAttachmentsDialog::SetOpenParamList(CAiwGenericParamList* aOpenParamList)
    {
    delete iOpenParamList;
    iOpenParamList = aOpenParamList;
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::OpenParamList
//
//
// ---------------------------------------------------------
//
EXPORT_C CAiwGenericParamList* CMsgViewAttachmentsDialog::OpenParamList( )
    {
    return iOpenParamList;
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::DoSaveTextFileL
// ---------------------------------------------------------
//
TInt CMsgViewAttachmentsDialog::DoSaveTextFileL( RFile& aFile, TInt aCharSet )
    {
    TInt result( KErrNone );
    
    if ( aCharSet == KErrNotFound )
        {
        TRAP( result, CNotepadApi::SaveFileAsMemoL( aFile ) );
        }
    else
        {
        TRAP( result, CNotepadApi::SaveFileAsMemoL( aFile, 
                                                    aCharSet ) );
        }

    // Show confirmation note
    RApaLsSession appList;
    if ( result == KErrNone && 
         appList.Connect() == KErrNone )
        {
        CleanupClosePushL( appList );
        
        TApaAppInfo appInfo;
        if ( appList.GetAppInfo( appInfo, KNotepadUID3 ) == KErrNone )
            {
            HBufC* text = StringLoader::LoadLC( R_MSG_FILE_SAVED_TO, appInfo.iCaption );
            
            CAknConfirmationNote* note = new( ELeave ) CAknConfirmationNote( ETrue );
            note->ExecuteLD( *text );
            
            CleanupStack::PopAndDestroy( text );
            }
        
        CleanupStack::PopAndDestroy( &appList );
        }
    
    return result;
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::DoUpdateUiL
// ---------------------------------------------------------
//
void CMsgViewAttachmentsDialog::DoUpdateUiL()
    {
    if ( iAttachmentModel.NumberOfItems() == 0 || 
         iAttachmentModel.AttachmentInfoAt( CurrentListItemIndex() ).IsEmptyAttachment() )
        {
        UpdateButtonVisiblity( EAknSoftkeyOpen, EFalse );
        }
    else
        {
        UpdateButtonVisiblity( EAknSoftkeyOpen, ETrue );
        }
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::UpdateButtonVisiblity
// ---------------------------------------------------------
//
void CMsgViewAttachmentsDialog::UpdateButtonVisiblity( TInt aCommandId, TBool aValue )
    {
    if ( ButtonGroupContainer().ControlOrNull( aCommandId ) )
        {
        ButtonGroupContainer().MakeCommandVisible( aCommandId, aValue );
        }
    }

// End of File
