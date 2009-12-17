/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CUniEditorLaunchOperation
*
*/



// ========== INCLUDE FILES ================================

#include <AknsConstants.h>
#include <data_caging_path_literals.hrh> 
#include <aknlayoutscalable_apps.cdl.h>

// Features
#include <featmgr.h>    
#include <bldvariant.hrh>
#include <centralrepository.h>    // link against centralrepository.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys

#include <mmsconst.h>

#include <MsgEditorView.h>          // for CMsgEditorView
#include <MsgEditorAppUi.h>
#include <MsgEditorCommon.h>
#include <msgimagecontrol.h>
#include <MuiuMsgEditorLauncher.h>

#include <msgasynccontrolobserver.h>

#include <uniutils.mbg>
#include <UniEditor.rsg>

#include "UniPluginApi.h"
#include "UniSendingSettings.h"
#include <unidatamodel.h>
#include <unismilmodel.h>
#include <unislideloader.h>
#include <unimsventry.h>

#include "UniEditorDocument.h"
#include "UniEditorHeader.h"
#include "UniEditorLaunchOperation.h"

_LIT( KUniEditorMbmFile, "uniutils.mbm" );

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

const TInt  KMaxSubjectLength = 40;   // From MMS Conformance Document

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniEditorLaunchOperation::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CUniEditorLaunchOperation* CUniEditorLaunchOperation::NewL(
        MMsgAsyncControlObserver& aControlObserver,
        MUniEditorOperationObserver& aOperationObserver,
        CUniEditorDocument& aDocument,
        CMsgEditorView& aView,
        CMsgEditorAppUi& aAppUi,
        RFs& aFs )
    {
    CUniEditorLaunchOperation* self = 
        new ( ELeave ) CUniEditorLaunchOperation( aControlObserver, aOperationObserver, aDocument, aView, aAppUi, aFs );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::CUniEditorLaunchOperation
//
// Constructor.
// ---------------------------------------------------------
//
CUniEditorLaunchOperation::CUniEditorLaunchOperation(
        MMsgAsyncControlObserver& aControlObserver,
        MUniEditorOperationObserver& aOperationObserver,
        CUniEditorDocument& aDocument,
        CMsgEditorView& aView,
        CMsgEditorAppUi& aAppUi,
        RFs& aFs ) :
    CUniEditorOperation( aOperationObserver, aDocument, aFs, EUniEditorOperationLaunch ),
    iControlObserver( aControlObserver ),
    iView( aView ),
    iAppUi( aAppUi )
    {
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::ConstructL()
    {
    BaseConstructL();
    FeatureManager::InitializeLibL();
    iSmilEditorSupported = FeatureManager::FeatureSupported( KFeatureIdSmilEditor );
    FeatureManager::UnInitializeLib();    
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::CUniEditorLaunchOperation
//
// Destructor.
// ---------------------------------------------------------
//
CUniEditorLaunchOperation::~CUniEditorLaunchOperation()
    {
    Cancel();
    delete iSendUiOperation;
    delete iHeader;
    delete iSlideLoader;

#ifdef RD_MSG_XHTML_SUPPORT 
    delete iTextOperation;
#endif
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::Launch
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::Launch()
    {
    iParseResultTemp= 0;
    ResetErrors();
    iOperationState = EUniLaunchInitializeDoc;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::DoLaunchStepL
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::DoLaunchStepL()
    {
    switch ( iOperationState )
        {
        case EUniLaunchInitializeDoc:
            {
            DoInitializeDocL();
            break;
            }
        case EUniLaunchInitializeModel:
            {
            DoInitializeModelL();
            break;
            }
        case EUniLaunchHandleMessage:
            {
            DoHandleMessageL();
            break;
            }
        case EUniLaunchPrepareHeader:
            {
            DoPrepareHeaderL();
            break;
            }
        case EUniLaunchPrepareBody:
            {
            DoPrepareBodyL();
            break;
            }
        case EUniLaunchEnd:
            {
            iObserver.EditorOperationEvent(
                EUniEditorOperationLaunch,
                EUniEditorOperationComplete ); 
            break;
            }
        default:
            iObserver.EditorOperationEvent(
                EUniEditorOperationLaunch,
                EUniEditorOperationError ); 
            break;
        }

    iOperationState++;
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::DoInitializeDocL
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::DoInitializeDocL()
    {
    TUniMessageCharSetSupport charSet = EUniMessageCharSetFull;

    iDocument.LaunchPlugings();

    CMsvStore* readStore = NULL;
    if ( iDocument.Mtm().Entry().HasStoreL() )
    	{
        readStore = iDocument.Mtm().Entry().ReadStoreL();
    	}
    CleanupStack::PushL( readStore );

    if( iDocument.SmsPlugin() )
        {
        iDocument.SmsPlugin()->LoadHeadersL( readStore );

        // Check the valid charsupport setting
        if( iDocument.MessageType() == EUniOpenFromDraft )
            {
            charSet = TUniMsvEntry::CharSetSupport( iDocument.Mtm().Entry().Entry() );
            }
        else
            {
            TInt features = 0;
            CRepository* storage = CRepository::NewLC( KCRUidSmum );
            
            if ( storage->Get( KSmumCharSupport, features ) == KErrNone )
                {
                if( features == EUniMessageCharSetReduced )
                    {
                    charSet = EUniMessageCharSetReduced;
                    }
                else if( features == EUniMessageCharSetFullLocked )
                    {
                    charSet = EUniMessageCharSetFullLocked;
                    }
                else if( features == EUniMessageCharSetReducedLocked )
                    {
                    charSet = EUniMessageCharSetReducedLocked;
                    }
                else
                    {
                    charSet = EUniMessageCharSetFull;
                    }
                }    
                
            CleanupStack::PopAndDestroy( storage );
            }
        }
        
    if( iDocument.MmsPlugin() )
        {
      	iDocument.MmsPlugin()->LoadHeadersL( readStore );
        }
        
    CleanupStack::PopAndDestroy( readStore );

    iDocument.CreateCharConverterL( charSet );

    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::DoInitializeModelL
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::DoInitializeModelL()
    {
    iDocument.DataModel()->RestoreL( *this );
    SetPending();
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::RestoreReady
//
// RestoreReady: a callback from RestoreL
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::RestoreReady( TInt /*aParseResult*/, TInt aError )
    {
    CompleteOperation( aError );
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::DoHandleMessageL
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::DoHandleMessageL()
    {
    // Synchronize uniobjects that are not yet parsed
    iDocument.DataModel()->FinalizeMediaParse();
    
    // Map NoSmil and MultipleSmil to MmsSmil
    if ( iDocument.DataModel()->SmilType() == ENoSmil ||
         iDocument.DataModel()->SmilType() == EMultipleSmil )
        {
        iDocument.DataModel()->SetSmilType( EMmsSmil );
        }
    
    if ( iDocument.Mtm().Body().DocumentLength() > 0 )
        {
        // When UniEditor message is formed from real SMS message the body text 
        // is not saved to text attachment but copied directly to text control. 
        // Setting the body modified will trigger the saving to be done.
        iDocument.SetBodyModified( ETrue );
        }
    
    switch ( iDocument.MessageType() )    
        {
        case EUniNewMessage:
            {
            //no operation
            break;
            }
        case EUniReply:
            {
            AddSubjectPrefixL( ETrue );
            iDocument.SetHeaderModified( ETrue );  // To get "Save message..." query always
            break;
            }
        case EUniForward:
            {
            // Code runs here when message is forwarded and forwarded message
            // has been saved into Drafts.
            // iDocument.LaunchFlags() & EMsgForwardMessage is ETrue, only when
            // message is being forwarded from Viewer
            if ( ( iDocument.LaunchFlags() & EMsgForwardMessage ) &&
                   !TUniMsvEntry::IsMmsUpload( iDocument.Entry() ) )
                {
                AddSubjectPrefixL( EFalse );
                }
                        
            // If Subject variation is Off, subject has been shown in the viewer
            // but will not be shown and saved in the editor
            iDocument.SetHeaderModified( ETrue );  // To get "Save message..." query always
            
#ifdef RD_MSG_XHTML_SUPPORT 
            iTextOperation = CUniEditorProcessTextOperation::NewL( *this, iDocument, iFs );
            iTextOperation->Start();
            
            SetPending();
            return;
#else
            break;
#endif
            }
        case EUniOpenFromDraft:
            {
            //no operation
            // Added, because of additional header feature.
            // Variation has changed On->Off and message opened from Drafts
            // To get "Save message..." query always
            iDocument.SetHeaderModified( ETrue );
            break;
            }
        case EUniSendUi:
            {
            // NOTE: Write from phoneidle is handled as SendAs
            iDocument.SetHeaderModified( ETrue ); // To get "Save message..." note always
            
            
            iSendUiOperation = CUniEditorSendUiOperation::NewL( *this, iDocument, iFs );
            iSendUiOperation->Start();
            
            SetPending();
            return;
            }
        default:
            {
            // Message type not known
            User::Leave( KErrNotSupported );
            break;
            }
        }
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::DoPrepareHeaderL
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::DoPrepareHeaderL()
    {
    // Header is always drawn and populated
    iHeader = CUniEditorHeader::NewL( iDocument.Mtm(), iDocument, iView, iFs );
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::DoPrepareBodyL
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::DoPrepareBodyL()
    {
    iSlideLoader = CUniSlideLoader::NewL(
        iControlObserver,
        *iDocument.DataModel(),
        iView,
        EUniControlEditorMode );
        
    if ( iDocument.DataModel()->SmilType() == EMmsSmil )
        {
        if ( !iDocument.DataModel()->SmilModel().SlideCount() )
            {
            iDocument.DataModel()->SmilModel().AddSlideL();
            }
            
        iSlideLoader->LoadSlideL( *this, 0 );
        SetPending();
        }
    else
        {
        // All unsupported files should be in
        // attachment list! 
        //
        // We should get here only if the SMIL is
        // more complex than MMS SMIL.
        //
        switch ( iDocument.CreationMode() )
            {
            case EMmsCreationModeRestricted:
                {
                // Other than MMS SMIL not supported in restricted mode
                SetError( KUniLaunchAbortPresRestricted );
                CompleteSelf( KErrNone );
                return;
                }
            case EMmsCreationModeWarning:
                {
                // Should be handled in editor side.
                SetError( KUniLaunchPresGuided );
                }
                // FALLTHROUGH
            case EMmsCreationModeFree:
            default:
                {
                if( TUniMsvEntry::IsForwardedMessage( iDocument.Entry() ) || 
                    !iSmilEditorSupported )
                    {
                    // No editing allowed for forwarded "TemplateSmil"
                    // "TemplateSmil" not supported in restricted mode
                    // "TemplateSmil" not supported if SmilEditor disabled
                    //
                    // Set SMIL type to "3GPPSmil"
                    iDocument.DataModel()->SetSmilType( E3GPPSmil );
                    }
                break;
                }
            }
            
        DoPrepare3GPPBodyL();
        CompleteSelf( KErrNone );
        }
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::DoPrepare3GPPBodyL
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::DoPrepare3GPPBodyL()
    {
    CMsgImageControl* imageControl = CMsgImageControl::NewL( iView,
                                                             &iControlObserver );
    CleanupStack::PushL( imageControl );
    
    imageControl->SetControlId( EMsgComponentIdImage );
    
    TAknsItemID id = KAknsIIDQgnGrafMmsUnedit;
    TInt icon = EMbmUniutilsQgn_graf_mms_unedit;
    TInt mask = EMbmUniutilsQgn_graf_mms_unedit_mask;

    if ( iDocument.DataModel()->SmilType() == ETemplateSmil )
        {
        id.Set( KAknsIIDQgnGrafMmsEdit );
        icon = EMbmUniutilsQgn_graf_mms_edit;
        mask = EMbmUniutilsQgn_graf_mms_edit_mask;
        }

    TParse fileParse;
    fileParse.Set( KUniEditorMbmFile, &KDC_APP_BITMAP_DIR, NULL );
    imageControl->LoadIconL( fileParse.FullName(), id, icon, mask );
    
    TAknLayoutRect iconLayout;
    iconLayout.LayoutRect( MsgEditorCommons::MsgDataPane(),
                           AknLayoutScalable_Apps::msg_data_pane_g4().LayoutLine() );
    
    imageControl->SetIconSizeL( iconLayout.Rect().Size() );
    imageControl->SetIconVisible( ETrue );
    
    //The ownership of imageControl is transferred to iView
    iView.AddControlL( imageControl, EMsgComponentIdImage, EMsgFirstControl, EMsgBody );
    CleanupStack::Pop( imageControl );
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::AddSubjectPrefixL
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::AddSubjectPrefixL( TBool aReply )
    {
    HBufC* newSubject = NULL;
    
    if( iDocument.Mtm().SubjectL().Length() > 0 )
        { 
        // Add prefix only if there's some content in subject field
        newSubject = iAppUi.CreateSubjectPrefixStringL( iDocument.Mtm().SubjectL(),
                                                        aReply );
        }
    
    if ( newSubject )
        {
        CleanupStack::PushL( newSubject );
        iDocument.Mtm().SetSubjectL( newSubject->Left( KMaxSubjectLength ) );
        iDocument.SetHeaderModified( ETrue );  // To get "Save message..." query always
        CleanupStack::PopAndDestroy( newSubject );
        }
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::DoCancelCleanup
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::DoCancelCleanup()
    {
    if ( iSlideLoader )
        {
        iSlideLoader->Cancel();
        }
        
    if ( iSendUiOperation )
        {
        iSendUiOperation->Cancel();
        }

#ifdef RD_MSG_XHTML_SUPPORT 
    if ( iTextOperation )
        {
        iTextOperation->Cancel();
        }
#endif
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::RunL
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::RunL()
    {
    PrintOperationAndState();
    if ( iStatus.Int() != KErrNone )
        {
        // if EUniEditorExit, next error code is reason to exit
        SetError( EUniEditorExit );
        SetErrorAndReport( iStatus.Int() );
        }
    else
        {
        DoLaunchStepL();
        }
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::RunError
// ---------------------------------------------------------
//
TInt CUniEditorLaunchOperation::RunError( TInt aError )
    {
    // If EUniEditorExit, next error code is reason to exit
    SetError( EUniEditorExit );
    SetError( aError );
    
    iObserver.EditorOperationEvent( EUniEditorOperationLaunch,
                                    EUniEditorOperationError );
    return KErrNone;
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::SlideLoadReady
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::SlideLoadReady( TInt aError )
    {
    CompleteOperation( aError );
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::HandleOperationEvent
// ---------------------------------------------------------
//
void CUniEditorLaunchOperation::HandleOperationEvent( TUniEditorOperationType aOperation,
                                                      TUniEditorOperationEvent /*aEvent*/ )
    {
    if ( aOperation == EUniEditorOperationSendUi )
        {
        CArrayFixFlat<TInt>* errors = iSendUiOperation->GetErrors();
        for ( TInt i = 0; i < errors->Count(); i++ )
            {
            if ( errors->At( i ) == EUniProcessImgUserAbort )
                {
                iOperationState = EUniLaunchEnd;
                }
            else if ( errors->At( i ) == EUniProcessImgCouldNotScale &&
                      iDocument.CreationMode() != EMmsCreationModeRestricted )
                {
                // This occurs when animated or transparent gif. Other cases?
                // Gif type is by default conformant
                iParseResultTemp++;
                }
            SetError( errors->At( i ) );
            }
        }
#ifdef RD_MSG_XHTML_SUPPORT 
    else
        {
        CArrayFixFlat<TInt>* errors = iTextOperation->GetErrors();
        for ( TInt i = 0; i < errors->Count(); i++ )
            {
            SetError( errors->At( i ) );
            }
        }
#endif

    CompleteOperation( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::ParseResult
// ---------------------------------------------------------
//
TInt CUniEditorLaunchOperation::ParseResult()
    {
    TInt parseResult = iDocument.UpdatedNonConformantCount();
    parseResult += iParseResultTemp;
    
    return parseResult;
    }


// EOF
