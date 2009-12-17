/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Defines implementation of CUniSlideLoader class methods.
*
*/



// INCLUDE FILES
#include "UniSlideLoader.h"

#include <mtclbase.h>

#include <eikrted.h>
#include <data_caging_path_literals.hrh> 
#include <barsread.h>

#include <AknUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknsConstants.h>
#include <AknBitmapAnimation.h>

#include <AudioPreference.h>

#include <MsgEditorView.h>
#include <MsgEditorCommon.h>
#include <MsgBaseControl.h>
#include <MsgBodyControl.h>
#include <msgasynccontrol.h>
#include <msgxhtmlbodycontrol.h>

#include <MsgMediaInfo.h>
#include <MsgMimeTypes.h>
#include <fileprotectionresolver.h>

#include <DRMCommon.h>
#include <DRMNotifier.h>

#include <mmsconst.h>

#include <uniutils.mbg>
#include <UniUtils.rsg>

#include <uniobject.h>
#include <unidatamodel.h>
#include <unimodelconst.h>
#include <unimimeinfo.h>
#include <unidrminfo.h>
#include <unitextobject.h>

#include <msgmediacontrol.h>
#include <msgaudiocontrol.h>
#include <msgimagecontrol.h>
#include <msgvideocontrol.h>

#ifdef RD_SVGT_IN_MESSAGING
#include <msgsvgcontrol.h>
#endif

#include "UniUtils.h"
#include "UniEditorLogging.h"

// ==========================================================

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

const TInt KViewerAnimationLoopCount = -1;
const TInt KEditorAnimationLoopCount = 1;

// MACROS

// LOCAL CONSTANTS AND MACROS
enum TUniIconType
    {
    EUniIconNoRightsSendable,     // no rights and can be sent
    EUniIconNoRightsNotSendable,  // no rights but cannot be sent
    EUniIconDrmSendable,          // drm protected content and can be sent
    EUniIconDrmNotSendable,        // drm protected content and cannot be sent
    EUniIconCorrupted
    };

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CUniSlideLoader::CUniSlideLoader
//
// Constructor.
// ---------------------------------------------------------
//
CUniSlideLoader::CUniSlideLoader(   MUniObjectObserver*         aUniObjectObserver,
                                    MMsgAsyncControlObserver& aControlObserver,
                                    CUniDataModel& aDataModel,
                                    CMsgEditorView& aView,
                                    TUniControlMode aControlMode ) : 
    CActive( EPriorityStandard ),
    iControlObserver( aControlObserver ),
    iDataModel( aDataModel ),
    iView( aView ),
    iControlMode( aControlMode ),
    iResourceLoader( *iView.ControlEnv() ),
    iUniObjectObserver ( aUniObjectObserver )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CUniSlideLoader::ConstructL
//
//
// ---------------------------------------------------------
//
void CUniSlideLoader::ConstructL()
    {
    if ( !iView.ControlEnv()->IsResourceAvailableL( R_UNUT_EDITOR_WAIT_ANIM ) )
        {
        TParse parse;
        parse.Set( KUniUtilsResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
        TFileName fileName( parse.FullName() );
        iResourceLoader.OpenL( fileName );
        }
    }


// ---------------------------------------------------------
// CUniSlideLoader::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CUniSlideLoader* CUniSlideLoader::NewL(
        MMsgAsyncControlObserver& aControlObserver,
        CUniDataModel& aDataModel,
        CMsgEditorView& aView,
        TUniControlMode aControlMode )
    {
    return CUniSlideLoader::NewL( NULL, aControlObserver, aDataModel, aView, aControlMode );
    }


// ---------------------------------------------------------
// CUniSlideLoader::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CUniSlideLoader* CUniSlideLoader::NewL(
        MUniObjectObserver*         aUniObjectObserver,                            
        MMsgAsyncControlObserver&   aControlObserver,
        CUniDataModel&              aDataModel,
        CMsgEditorView&             aView,
        TUniControlMode             aControlMode )
    {
    CUniSlideLoader* self = new ( ELeave ) CUniSlideLoader( aUniObjectObserver,
                                                            aControlObserver, 
                                                            aDataModel, 
                                                            aView, 
                                                            aControlMode );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CUniSlideLoader::~CUniSlideLoader
//
// Destructor
// ---------------------------------------------------------
//
CUniSlideLoader::~CUniSlideLoader()
    {
    Cancel();
    
    iResourceLoader.Close();

    RemoveObservers( );
    }

// ---------------------------------------------------------
// CUniSlideLoader::LoadSlideL
// ---------------------------------------------------------
EXPORT_C void CUniSlideLoader::LoadSlideL(
        MUniSlideLoaderObserver& aSlideObserver,
        TInt aSlideNum )
    {
    TInt slideCount = iDataModel.SmilModel().SlideCount();
    if ( slideCount && 
        ( aSlideNum < 0 || aSlideNum >= slideCount ) )
        {
        User::Leave( KErrArgument );
        }
    iSlideObserver = &aSlideObserver;
    iLoadedSlide = aSlideNum;
    iObjectNum = 0;
    iOperation = ELoadSlide;
    iError = KErrNone;
    CompleteSelf();
    }

// ---------------------------------------------------------
// CUniSlideLoader::LoadObject
// ---------------------------------------------------------
//
EXPORT_C void CUniSlideLoader::LoadObject( 
        MUniSlideLoaderObserver& aSlideObserver,
        CUniObject* aObject )
    {
    iSlideObserver = &aSlideObserver;
    iOperation = ELoadObject;
    iObjectNum = 1;
    iLoadedObject = aObject;
    iError = KErrNone;
    CompleteSelf();
    }
    
// ---------------------------------------------------------
// CUniSlideLoader::ResetViewL
// ---------------------------------------------------------
//
EXPORT_C void CUniSlideLoader::ResetViewL()
    {
    RemoveObservers( );
    
    // remove control from message text object
    CUniObject* obj = iDataModel.SmilModel().GetObject( iLoadedSlide, EUniRegionText );
    if ( obj && 
         ( obj->MediaType() == EMsgMediaText ||
           obj->MediaType() == EMsgMediaXhtml ) )
        {
        static_cast<CUniTextObject*>( obj )->SetText( NULL );
        }
    iView.DeleteControlL( EMsgComponentIdBody );
    iView.DeleteControlL( EMsgComponentIdImage );
    iView.DeleteControlL( EMsgComponentIdVideo );
    iView.DeleteControlL( EMsgComponentIdAudio );
#ifdef RD_SVGT_IN_MESSAGING     
    iView.DeleteControlL( EMsgComponentIdSvg );
#endif    
    }

// ---------------------------------------------------------
// CUniSlideLoader::DoCancel
//
// ---------------------------------------------------------
//
void CUniSlideLoader::DoCancel()
    {
    iSlideObserver->SlideLoadReady( KErrCancel );
    }

// ---------------------------------------------------------
// CUniSlideLoader::RunL
//
// ---------------------------------------------------------
//
void CUniSlideLoader::RunL()
    {
    if ( iOperation == ELoadSlide )
        {
        LoadSlideStepL();
        }
    else
        {
        LoadObjectStepL();
        }
    }

// ---------------------------------------------------------
// CUniSlideLoader::RunError
//
// ---------------------------------------------------------
//
TInt CUniSlideLoader::RunError( TInt aError )
    {
    iError = aError;
    CompleteSelf();
    return KErrNone;
    }

// ---------------------------------------------------------
// CUniSlideLoader::CompleteSelf
//
// Completes current step of state machine
// ---------------------------------------------------------
//
void CUniSlideLoader::CompleteSelf()
    {
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, KErrNone );
    }

// ---------------------------------------------------------
// CUniSlideLoader::SetSlideStepL
// ---------------------------------------------------------
//
void CUniSlideLoader::LoadSlideStepL()
    {
    CUniObject* obj = iDataModel.SmilModel().GetObjectByIndex( iLoadedSlide, iObjectNum );
    // must be set before leaving
    iObjectNum++;
    if ( obj )
        {
        LoadObjectL( obj );
        }
    else
        {
        if (!iDataModel.SmilModel().GetObject( iLoadedSlide, EUniRegionText ) )
            {
            // Add empty text control
            LoadTextL( NULL );
            }
        iSlideObserver->SlideLoadReady( iError );
        }
    }

// ---------------------------------------------------------
// CUniSlideLoader::SetObjectStepL
// ---------------------------------------------------------
//
void CUniSlideLoader::LoadObjectStepL()
    {
    if ( iObjectNum )
        {
        // must be set before leaving
        iObjectNum--;
        LoadObjectL( iLoadedObject );
        }
    else
        {
        iSlideObserver->SlideLoadReady( iError );
        }
    }

// ---------------------------------------------------------
// CUniSlideLoader::LoadObjectL
// ---------------------------------------------------------
//
void CUniSlideLoader::LoadObjectL( CUniObject* aObject )
    {
    switch ( aObject->Region() )
        {
        case EUniRegionText:
            {
            LoadTextL( aObject );
            break;
            }
        case EUniRegionImage:
            {
            if ( aObject->MediaType() == EMsgMediaVideo ||
                 aObject->MediaTypeBySmilTag() == EMsgMediaVideo ) 
                {
                LoadVideoL( aObject );
                }
#ifdef RD_SVGT_IN_MESSAGING
            else if ( aObject->MediaType() == EMsgMediaSvg ||
                     aObject->MediaTypeBySmilTag() == EMsgMediaSvg )
                {
                LoadSvgL( aObject );
                }
#endif                
            else
                {
                LoadImageL( aObject );
                }
            break;
            }
        case EUniRegionAudio:
            {
            LoadAudioL( aObject );
            break;
            }
        case EUniRegionUnresolved:
        default:
            {
            break;
            }
        }

    if ( iStatus != KRequestPending )
        {
        CompleteSelf();
        }
    }

// ---------------------------------------------------------
// CUniSlideLoader::LoadTextL
// ---------------------------------------------------------
//
void CUniSlideLoader::LoadTextL( CUniObject* aObject )
    {
    if ( aObject && 
         aObject->MediaType() == EMsgMediaXhtml )
        {
        CMsgXhtmlBodyControl* textControl = CMsgXhtmlBodyControl::NewL( iView, this );
        CleanupStack::PushL( textControl );
        
        LoadControlL( *textControl, aObject );
        
        AddToViewL( textControl, EMsgComponentIdBody, EMsgMediaXhtml, aObject );
        CleanupStack::Pop( textControl );
        
        CUniTextObject* obj = static_cast<CUniTextObject*>( aObject );
        obj->SetText( &( textControl->Editor() ) );
        
        SetActive();
        iStatus= KRequestPending;
        }
    else
        {
        CMsgBodyControl* textControl = CMsgBodyControl::NewL( &iView );
        CleanupStack::PushL( textControl );
        
        textControl->SetControlId( EMsgComponentIdBody );
        
        if ( aObject )
            {        
            TUint attaCharset = aObject->MimeInfo()->Charset();
            if ( !attaCharset )
                {
                //assume US-ASCII - mandated by RFC 2046
                attaCharset = KMmsUsAscii;
                }

            TUint charconvCharsetID = 0;
            charconvCharsetID = iDataModel.DataUtils().MibIdToCharconvIdL( attaCharset );

            RFile file = CUniDataUtils::GetAttachmentFileL( iDataModel.Mtm(), aObject->AttachmentId() );
            CleanupClosePushL( file );
            
            RFileReadStream stream( file );
            CleanupClosePushL( stream );
            
            CPlainText::TImportExportParam param;
            param.iForeignEncoding = charconvCharsetID;
            param.iOrganisation = CPlainText::EOrganiseByParagraph; 
            
            CPlainText::TImportExportResult result;
            
            TRAPD( err, textControl->Editor().Text()->ImportTextL( 0, stream, param, result ) );
            if ( err != KErrNone )
                {
                textControl->Reset();
                User::Leave( err );
                }
            
            CleanupStack::PopAndDestroy( 2, &file );
            
            CUniTextObject* obj = static_cast<CUniTextObject*>( aObject );
            obj->SetText( &( textControl->Editor() ) );
            }
        else 
            {
            if ( iDataModel.Mtm().Body().DocumentLength() > 0 )
                {
                TRAPD( err, 
                       {
                       textControl->SetTextContentL( iDataModel.Mtm().Body() );
                       if ( !iDataModel.SmilModel().IsSlide( 0 ) )
                           {
                           // Add slide
                           iDataModel.SmilModel().AddSlideL( 0 );
                           }
                       
                       iDataModel.SmilModel().AddTextObjectL( 0, &( textControl->Editor() ) );
                       } );
                                
                if ( err != KErrNone )
                    {
                    textControl->Reset();
                    User::Leave( err );
                    }
                
                iDataModel.Mtm().Body().Reset();
                }
            }
        
        AddToViewL( textControl, EMsgComponentIdBody, EMsgMediaText, aObject );
        CleanupStack::Pop( textControl );
        }
    }

// ---------------------------------------------------------
// CUniSlideLoader::LoadImageL
// ---------------------------------------------------------
//
void CUniSlideLoader::LoadImageL( CUniObject* aObject )
    {
    CMsgImageControl* imageControl = CMsgImageControl::NewL( iView, this );
    CleanupStack::PushL( imageControl );
    
    if ( LoadControlL( *imageControl, aObject ) &&
         ConsumeDrmRights( aObject ) )
        {
        if ( iControlMode == EUniControlViewerMode )
            {
            imageControl->SetAnimationLoopCount( KViewerAnimationLoopCount );
            }
        else
            {
            imageControl->SetAnimationLoopCount( KEditorAnimationLoopCount );
            }
            
        imageControl->PlayL();    
        }
        
    UpdateControlIconL( *imageControl, aObject );
    
    AddToViewL( imageControl, EMsgComponentIdImage, EMsgMediaImage, aObject );
    
    CleanupStack::Pop( imageControl );
    }

// ---------------------------------------------------------
// CUniSlideLoader::LoadAudioL
// ---------------------------------------------------------
//
void CUniSlideLoader::LoadAudioL( CUniObject* aObject )
    {
    CMsgAudioControl* audioControl = CMsgAudioControl::NewL( iView, 
                                                             this, 
                                                             TMdaPriority( KAudioPriorityMmsViewer ),
                                                             TMdaPriorityPreference( KAudioPrefMmsViewer ) );
    CleanupStack::PushL( audioControl );
    
    if ( iControlMode == EUniControlViewerMode )
        {
        LoadControlL( *audioControl, aObject );
        }
    
    UpdateControlIconL( *audioControl, aObject );
    
    AddToViewL( audioControl, EMsgComponentIdAudio, EMsgMediaAudio, aObject );
    
    CleanupStack::Pop( audioControl );
    }

// ---------------------------------------------------------
// CUniSlideLoader::LoadVideoL
// ---------------------------------------------------------
//
void CUniSlideLoader::LoadVideoL( CUniObject* aObject )
    {
    TInt flags = 0;
    if ( iControlMode ==  EUniControlEditorMode )
        {
        flags |= CMsgVideoControl::ENoPlayback;
        }
    
    CMsgVideoControl* videoControl = CMsgVideoControl::NewL( iView,     
                                                             this, 
                                                             TMdaPriority( KAudioPriorityMmsViewer ),
                                                             TMdaPriorityPreference( KAudioPrefMmsViewer ),
                                                             flags );
    CleanupStack::PushL( videoControl );
    
    if ( iControlMode == EUniControlViewerMode )
        {
        LoadControlL( *videoControl, aObject );
        }
    
    UpdateControlIconL( *videoControl, aObject );
    
    AddToViewL( videoControl, EMsgComponentIdVideo, EMsgMediaVideo, aObject );
    CleanupStack::Pop( videoControl );
    }

// ---------------------------------------------------------
// CUniSlideLoader::LoadSvgL
// ---------------------------------------------------------
//
#ifdef RD_SVGT_IN_MESSAGING
void CUniSlideLoader::LoadSvgL( CUniObject* aObject )
    {
    CMsgSvgControl* svgControl = CMsgSvgControl::NewL( iView, this );
    CleanupStack::PushL( svgControl );
    
    if ( LoadControlL( *svgControl, aObject ) )
        {
        TParse* parser = new( ELeave ) TParse;
        CleanupStack::PushL( parser );
        
        parser->Set( KUniUtilsMBMFileName, &KDC_APP_BITMAP_DIR, NULL );
    
        svgControl->LoadIndicatorIconL( KAknsIIDQgnGrafMmsPres,
                                        parser->FullName(),
                                        EMbmUniutilsQgn_graf_mms_pres,
                                        EMbmUniutilsQgn_graf_mms_pres + 1 );

        CleanupStack::PopAndDestroy( parser );
        }
        
    UpdateControlIconL( *svgControl, aObject );
    
    AddToViewL( svgControl, EMsgComponentIdSvg, EMsgMediaSvg, aObject );
    CleanupStack::Pop( svgControl );
    }
#else
void CUniSlideLoader::LoadSvgL( CUniObject* /*aObject*/ )
    {
    }
#endif
    
// ---------------------------------------------------------
// CUniSlideLoader::LoadControlL
// ---------------------------------------------------------
//
TBool CUniSlideLoader::LoadControlL( MMsgAsyncControl& aMediaControl, CUniObject* aObject )
    {
    UNILOGGER_ENTERFN( "SlideLoader: LoadControlL" );
    
    TBool result( EFalse );
    
    // Observer is added even if loading fails.
    aMediaControl.AddObserverL( iControlObserver );
    if ( iUniObjectObserver )
        {
        aObject->SetObserverL( iUniObjectObserver );
        }
    TBool drmRightsValid( DrmRightsValid( aObject ) );
    if ( aObject )
        {
        aObject->SetDrmRightsWhenLoaded( drmRightsValid );
        }
    if ( aObject && drmRightsValid )
        {
        RFile file = CUniDataUtils::GetAttachmentFileL( iDataModel.Mtm(), aObject->AttachmentId() );
        CleanupClosePushL( file );

        TRAPD( error, aMediaControl.LoadL( file ) );
        
        CleanupStack::PopAndDestroy( &file );
        
        // DRM errors are handled on UpdateControlIconL
        if ( error != KErrNone )
            {
            UNILOGGER_WRITEF( _L("SlideLoader: LoadL = %d "), error );
            result = EFalse;
            if ( !IsDRMError( error ) )
                {
                UNILOGGER_WRITE( "SlideLoader: Object set corrupted " );
                aObject->SetCorrupted( ETrue );
                }
            }
        else
            {                                     
            result = ETrue;
            }
        }
    
    UNILOGGER_LEAVEFN( "SlideLoader: LoadControlL" );
    return result;
    }


// ---------------------------------------------------------
// CUniSlideLoader::ReLoadControlL
// ---------------------------------------------------------
//
EXPORT_C void CUniSlideLoader::ReLoadControlL( CMsgMediaControl* aControl, CUniObject* aObject )
    {
    UNILOGGER_ENTERFN( "SlideLoader: ReLoadControlL" );
    if (    !aControl
        ||  !aObject )
        {
        User::Leave( KErrNotFound );
        }   
       
    TInt controlId = aControl->ControlId( );
    if (    controlId == EMsgComponentIdImage 
        ||  controlId == EMsgComponentIdVideo
        ||  controlId == EMsgComponentIdAudio 
#ifdef RD_SVGT_IN_MESSAGING
        ||  controlId == EMsgComponentIdSvg
#endif
        )
        {
        aControl->Stop();
        aControl->Close();
        TBool reLoaded = DoReLoadControlL( *aControl, aObject );

        // if controls coded perfectly there should not be need to remove
        // or add control.
        if (    controlId == EMsgComponentIdImage 
            &&  reLoaded )
            {
            // Image does not fit area of icon control
            iView.RemoveControlL( EMsgComponentIdImage );
            AddToViewL( aControl, EMsgComponentIdImage, EMsgMediaImage, aObject );
            
            ConsumeDrmRights( aObject );
            static_cast<CMsgImageControl*>( aControl )->PlayL();
            }
        else if (   controlId == EMsgComponentIdVideo
                &&  reLoaded )
            {
            iView.RemoveControlL( EMsgComponentIdVideo );
            AddToViewL( aControl, EMsgComponentIdVideo, EMsgMediaVideo, aObject );
            }
#ifdef RD_SVGT_IN_MESSAGING
        else if (   controlId == EMsgComponentIdSvg )
            {
            iView.RemoveControlL( EMsgComponentIdSvg );
            AddToViewL( aControl, EMsgComponentIdSvg, EMsgMediaSvg, aObject );
            }
#endif
        UpdateControlIconL( *aControl, aObject );
        }
    
    }

// ---------------------------------------------------------
// CUniSlideLoader::DoReLoadControlL
// ---------------------------------------------------------
//
TBool CUniSlideLoader::DoReLoadControlL( MMsgAsyncControl& aMediaControl, CUniObject* aObject )
    {
    UNILOGGER_ENTERFN( "SlideLoader: DoReLoadControlL" );
    
    TBool result( EFalse );
    
    TBool drmRightsValid( DrmRightsValid( aObject ) );
    
    if ( aObject )
        {
        aObject->SetDrmRightsWhenLoaded( drmRightsValid );    
        }
    if ( aObject && drmRightsValid )
        {
        RFile file = CUniDataUtils::GetAttachmentFileL( iDataModel.Mtm(), aObject->AttachmentId() );
        CleanupClosePushL( file );

        TRAPD( error, aMediaControl.LoadL( file ) );
        
        CleanupStack::PopAndDestroy( &file );
        
        // DRM errors are handled on UpdateControlIconL
        if ( error != KErrNone )
            {
            UNILOGGER_WRITEF( _L("SlideLoader: LoadL = %d "), error );
            result = EFalse;
            if ( !IsDRMError( error ) )
                {
                UNILOGGER_WRITE( "SlideLoader: Object set corrupted " );
                aObject->SetCorrupted( ETrue );
                }
            }
        else
            {                                     
            result = ETrue;
            }
        }
    
    UNILOGGER_LEAVEFN( "SlideLoader: DoReLoadControlL" );
    return result;
    }


// ---------------------------------------------------------
// CUniSlideLoader::UpdateControlIconL
// ---------------------------------------------------------
//
EXPORT_C void CUniSlideLoader::UpdateControlIconL( CMsgMediaControl& aMediaControl, CUniObject* aObject )
    {
    if ( !aObject )
        {
        return;
        }
        
    TUniIconType iconType( EUniIconNoRightsNotSendable );
    if ( aObject->MediaType() == EMsgMediaUnknown ||
         aObject->Corrupted() )
        {
        iconType = EUniIconCorrupted;
        }
    else
        {
        if ( aObject->MediaInfo()->Protection() == EFileProtNoProtection )
            {
            iconType = EUniIconNoRightsSendable;
            }
        else if ( aObject->MediaInfo()->Protection() & EFileProtSuperDistributable )
            {
            iconType = EUniIconDrmSendable;
            }
        else if (   aObject->MediaInfo()->Protection() & EFileProtForwardLocked
                ||  aObject->MediaInfo()->Protection() & EFileProtClosedContent )
            {
            iconType = EUniIconDrmNotSendable;
            }
        // else - no more choices left
        }
    
    TBool validDrmRights( DrmRightsValid( aObject ) );
        
    TAknsItemID iconId;
    TInt bitmapId( -1 );
    
    // value must be in sync with iconSize variable
    TAknWindowLineLayout layOut( AknLayoutScalable_Apps::msg_data_pane_g7().LayoutLine() );    
    
    CAknBitmapAnimation* waitAnimation = NULL;
    
#ifdef RD_MSG_LOAD_ANIMATION
    if ( aMediaControl.State() ==  EMsgAsyncControlStateOpening )
        {
        // Control at loading state so specify wait animation.
        waitAnimation = CAknBitmapAnimation::NewL();    
        CleanupStack::PushL( waitAnimation );
    
        TResourceReader resourceReader;
        iView.ControlEnv()->CreateResourceReaderLC( resourceReader, R_UNUT_EDITOR_WAIT_ANIM );
    
        waitAnimation->ConstructFromResourceL( resourceReader );
        
        waitAnimation->ExcludeAnimationFramesFromCache();
        
        CleanupStack::PopAndDestroy(); //  resourceReader 
        }
#endif

    switch ( aMediaControl.ControlId( ) )
        {
        case EMsgComponentIdAudio:
            {
            switch( iconType )
                {
                case EUniIconNoRightsSendable:
                    {
                    if ( iControlMode == EUniControlViewerMode )
                        {
                        iconId = KAknsIIDQgnGrafMmsAudioPlay;
                        bitmapId = EMbmUniutilsQgn_graf_mms_audio_play;
                        }
                    else
                        {
                        iconId = KAknsIIDQgnGrafMmsAudioInserted;
                        bitmapId = EMbmUniutilsQgn_graf_mms_audio_inserted;
                        }
                    break;
                    }
                case EUniIconDrmSendable:
                    {
                    if ( validDrmRights )
                        {
                        iconId = KAknsIIDQgnGrafMmsAudioDrmValidSend;
                        bitmapId = EMbmUniutilsQgn_graf_mms_audio_drm_valid_send;
                        }
                    else
                        {
                        iconId = KAknsIIDQgnGrafMmsAudioDrm;
                        bitmapId = EMbmUniutilsQgn_graf_mms_audio_drm;
                        }
                    break;
                    }
                case EUniIconDrmNotSendable:
                    {
                    if ( validDrmRights )
                        {
                        iconId = KAknsIIDQgnGrafMmsAudioDrmValidSendForbid;
                        bitmapId = EMbmUniutilsQgn_graf_mms_audio_drm_valid_send_forbid;
                        }
                    else
                        {
                        iconId = KAknsIIDQgnGrafMmsAudioDrmInvalidSendForbid;
                        bitmapId = EMbmUniutilsQgn_graf_mms_audio_drm_invalid_send_forbid;
                        }
                    break;
                    }
                case EUniIconCorrupted:
                    {
                    iconId = KAknsIIDQgnGrafMmsAudioCorrupted;
                    bitmapId = EMbmUniutilsQgn_graf_mms_audio_corrupted;
                    break;
                    }
                case EUniIconNoRightsNotSendable:
                default:
                    {
                    iconId = KAknsIIDQgnGrafMmsAudioDrmValidSendForbid;
                    bitmapId = EMbmUniutilsQgn_graf_mms_audio_drm_valid_send_forbid;
                    break;
                    }
                }

            layOut = AknLayoutScalable_Apps::msg_data_pane_g6().LayoutLine();
            break;
            }
        case EMsgComponentIdImage:
            {
            if ( !validDrmRights )
                {
                // Only show icon if rights are not valid. Otherwise show
                // image itself.
                switch( iconType )
                    {
                    case EUniIconNoRightsSendable:
                        {
                        // does not occur i.e. Image itself
                        break;
                        }
                    case EUniIconDrmSendable:
                        {
                        iconId = KAknsIIDQgnGrafMmsImageDrm;
                        bitmapId = EMbmUniutilsQgn_graf_mms_image_drm;
                        break;
                        }
                    case EUniIconDrmNotSendable:
                        {
                        iconId = KAknsIIDQgnGrafMmsImageDrmInvalidSendForbid;
                        bitmapId = EMbmUniutilsQgn_graf_mms_image_drm_invalid_send_forbid;
                        break;
                        }
                    case EUniIconCorrupted:
                        {
                        iconId = KAknsIIDQgnGrafMmsImageCorrupted;
                        bitmapId = EMbmUniutilsQgn_graf_mms_image_corrupted;
                        break;
                        }
                    case EUniIconNoRightsNotSendable:
                    default:
                        {
                        iconId = KAknsIIDQgnGrafMmsImageDrmInvalidSendForbid;
                        bitmapId = EMbmUniutilsQgn_graf_mms_image_drm_invalid_send_forbid;
                        break;
                        }
                    }
                }
            else
                {
                switch( iconType )
                    {
                    case EUniIconCorrupted:
                        {
                        iconId = KAknsIIDQgnGrafMmsImageCorrupted;
                        bitmapId = EMbmUniutilsQgn_graf_mms_image_corrupted;
                        break;
                        }
                    default:
                        {
                        // Image itself.
                        break;
                        }
                    }
                }
            
            layOut = AknLayoutScalable_Apps::msg_data_pane_g7().LayoutLine();
            break;
            }
        case EMsgComponentIdVideo:
            {
            switch ( iconType )
                {
                case EUniIconNoRightsSendable:
                    {
                    if ( iControlMode == EUniControlViewerMode )
                        {
                        iconId = KAknsIIDQgnGrafMmsInsertedVideoView;
                        bitmapId = EMbmUniutilsQgn_graf_mms_inserted_video_view;
                        }
                    else
                        {
                        iconId = KAknsIIDQgnGrafMmsInsertedVideoEdit;
                        bitmapId = EMbmUniutilsQgn_graf_mms_inserted_video_edit;
                        }
                    break;
                    }
                case EUniIconDrmSendable:
                    {
                    if ( validDrmRights )
                        {
                        iconId = KAknsIIDQgnGrafMmsVideoDrmValidSend;
                        bitmapId = EMbmUniutilsQgn_graf_mms_video_drm_valid_send;
                        }
                    else
                        {
                        iconId = KAknsIIDQgnGrafMmsVideoDrm;
                        bitmapId = EMbmUniutilsQgn_graf_mms_video_drm;
                        }
                    break;
                    }
                case EUniIconDrmNotSendable:
                    {
                    if ( validDrmRights )
                        {
                        iconId = KAknsIIDQgnGrafMmsVideoDrmValidSendForbid;
                        bitmapId = EMbmUniutilsQgn_graf_mms_video_drm_valid_send_forbid;
                        }
                    else
                        {
                        iconId = KAknsIIDQgnGrafMmsVideoDrmInvalidSendForbid;
                        bitmapId = EMbmUniutilsQgn_graf_mms_video_drm_invalid_send_forbid;
                        }
                    break;
                    }
                case EUniIconCorrupted:
                    {
                    iconId = KAknsIIDQgnGrafMmsVideoCorrupted;
                    bitmapId = EMbmUniutilsQgn_graf_mms_video_corrupted;
                    break;
                    }
                case EUniIconNoRightsNotSendable:
                default:
                    {
                    iconId = KAknsIIDQgnGrafMmsVideoDrmValidSendForbid;
                    bitmapId = EMbmUniutilsQgn_graf_mms_video_drm_valid_send_forbid;
                    break;
                    }
                }
            layOut = AknLayoutScalable_Apps::msg_data_pane_g7().LayoutLine();
            break;
            }
#ifdef RD_SVGT_IN_MESSAGING            
        case EMsgComponentIdSvg:
            {
            if ( !validDrmRights )
                {
                switch ( iconType )
                    {
                    // not needed - case EUniIconNoRightsSendable:
                    case EUniIconDrmSendable:
                        {
                        if ( !validDrmRights )
                            {
                            iconId = KAknsIIDQgnGrafMmsPresDrmInvalidSend;
                            bitmapId = EMbmUniutilsQgn_graf_mms_pres_drm_invalid_send;
                            }
                        else
                            {
                            iconId = KAknsIIDQgnGrafMmsPres;
                            bitmapId = EMbmUniutilsQgn_graf_mms_pres;
                            }
                        break;                            
                        }                                            
                    case EUniIconDrmNotSendable:
                        {
                        if ( !validDrmRights )
                            {
                            iconId = KAknsIIDQgnGrafMmsPresDrmInvalidSendForbid;
                            bitmapId = EMbmUniutilsQgn_graf_mms_pres_drm_invalid_send_forbid;
                            }
                        else
                            {
                            iconId = KAknsIIDQgnGrafMmsPres;
                            bitmapId = EMbmUniutilsQgn_graf_mms_pres;
                            }                                                
                        break;
                        }
                    case EUniIconCorrupted:
                        {
                        iconId = KAknsIIDQgnGrafMmsPresCorrupted;
                        bitmapId = EMbmUniutilsQgn_graf_mms_pres_corrupted;
                        break;
                        }
                    default:
                        {
                        break;
                        }
                    }
                }
            else
                {
                switch( iconType )
                    {
                    case EUniIconCorrupted:
                        {
                        iconId = KAknsIIDQgnGrafMmsPresCorrupted;
                        bitmapId = EMbmUniutilsQgn_graf_mms_pres_corrupted;
                        break;
                        }
                    default:
                        {
                        // Svg itself.
                        break;
                        }
                    }
                }

            layOut = AknLayoutScalable_Apps::msg_data_pane_g7().LayoutLine();
            break;
            }
#endif
        default:
            {
            break;
            }
        }
    
    TAknLayoutRect placeholderLayout;
    placeholderLayout.LayoutRect( MsgEditorCommons::MsgDataPane(),
                                  layOut );
    
    if ( waitAnimation )
        {
        aMediaControl.SetAnimationL( waitAnimation ); // Ownership transferred
        CleanupStack::Pop( waitAnimation );
    
        aMediaControl.SetAnimationSizeL( placeholderLayout.Rect().Size() );
        }
        
    if ( bitmapId != -1 )
        {
        TParse* parser = new( ELeave ) TParse;
        CleanupStack::PushL( parser );
        
        User::LeaveIfError( parser->Set( KUniUtilsMBMFileName, &KDC_APP_BITMAP_DIR, NULL ) );

        aMediaControl.LoadIconL( parser->FullName(), iconId, bitmapId, bitmapId + 1 );
        CleanupStack::PopAndDestroy( parser );

        aMediaControl.SetIconSizeL( placeholderLayout.Rect().Size() );
        }
    
    if ( waitAnimation )
        {
        aMediaControl.SetAnimationVisibleL( ETrue );
        aMediaControl.DrawDeferred();
        }
    else if ( bitmapId != -1 )
        {
        aMediaControl.SetIconVisible( ETrue );
        aMediaControl.DrawDeferred();
        }
    }

// ---------------------------------------------------------
// CUniSlideLoader::AddToViewL
// ---------------------------------------------------------
//
void CUniSlideLoader::AddToViewL( CMsgBaseControl* aControl, 
                                  TInt aControlId, 
                                  TMsgMediaType aType,
                                  CUniObject* aObject )
    {
    TInt indexAdd = iView.ControlById( EMsgComponentIdAudio ) ? 1 : 0;
    TInt index = EMsgAppendControl;
    TUniLayout layout = iDataModel.SmilModel().Layout();

    switch ( aType )
        {
        case EMsgMediaText:
        case EMsgMediaXhtml:
            {
            if ( layout == EUniTextFirst )
                {
                index = EMsgFirstControl + indexAdd;
                }
            break;
            }
        case EMsgMediaImage:
            {
            if ( layout == EUniImageFirst )
                {
                index = EMsgFirstControl + indexAdd;
                }
            break;
            }
        case EMsgMediaVideo:
            {
            if ( layout == EUniImageFirst )
                {
                index = EMsgFirstControl + indexAdd;
                }
            break;
            }
#ifdef RD_SVGT_IN_MESSAGING            
        case EMsgMediaSvg:
            {
            if ( layout == EUniImageFirst )
                {
                index = EMsgFirstControl + indexAdd;
                }
            break;
            }
#endif                        
        case EMsgMediaAudio:
            {
            index = EMsgFirstControl;
            break;
            }
        default:
            {
            break;
            }
        }
        
    iView.AddControlL( aControl, aControlId, index, EMsgBody );
    
    if ( aObject )
        {
        aObject->SetUniqueControlHandle( aControl->UniqueHandle() );
        }
    }

// ---------------------------------------------------------
// CUniSlideLoader::IsDRMError
// ---------------------------------------------------------
//
TBool CUniSlideLoader::IsDRMError( TInt aError ) const
    {
    if ( aError == DRMCommon::EGeneralError ||
         aError == DRMCommon::EUnknownMIME ||
         aError == DRMCommon::EVersionNotSupported ||
         aError == DRMCommon::ESessionError ||
         aError == DRMCommon::ENoRights ||
         aError == DRMCommon::ERightsDBCorrupted ||
         aError == DRMCommon::EUnsupported ||
         aError == DRMCommon::ERightsExpired ||
         aError == DRMCommon::EInvalidRights )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CUniSlideLoader::DrmRightsValid
// ---------------------------------------------------------
//
TBool CUniSlideLoader::DrmRightsValid( CUniObject* aObject ) const
    {
    UNILOGGER_ENTERFN( "SlideLoader: DrmRightsValid" );
    TBool result( ETrue );
    
    if ( aObject->DrmInfo() )
        {
        UNILOGGER_WRITE( "SlideLoader: Checking rights " );
        TBool alreadyConsumed( EFalse );        
        
        result = ( aObject->DrmInfo()->EvaluateRights( alreadyConsumed ) == KErrNone ? ETrue : 
                                                                                       EFalse );        
        if ( alreadyConsumed )
            {
            UNILOGGER_WRITE( "SlideLoader: rights already consumed " );
            // Rights already consumed so rights are always valid.
            result = ETrue;
            }
        }
    
    UNILOGGER_LEAVEFN( "SlideLoader: DrmRightsValid" );
    return result;
    }

// ---------------------------------------------------------
// CUniSlideLoader::ConsumeDrmRights
// ---------------------------------------------------------
//
TBool CUniSlideLoader::ConsumeDrmRights( CUniObject* aObject ) const
    {
    UNILOGGER_ENTERFN( "SlideLoader: ConsumeDrmRights" );
    TBool result( ETrue );
    
    if ( aObject->DrmInfo() )
        {
        UNILOGGER_WRITE( "SlideLoader: Consuming rights " );
        
        if ( aObject->DrmInfo()->ConsumeRights() != KErrNone )
            {
            UNILOGGER_WRITE( "SlideLoader: Consuming rights failed!" );
            aObject->SetCorrupted( ETrue );
            
            result = EFalse;
            }
        }
    
    UNILOGGER_LEAVEFN( "SlideLoader: ConsumeDrmRights" );
    return result;
    }

// ---------------------------------------------------------
// CUniSlideLoader::MsgAsyncControlStateChanged
// 
// Called when media control's state changes. Performs icon and
// animation updating corresponding to current state.
// ---------------------------------------------------------
//
void CUniSlideLoader::MsgAsyncControlStateChanged( CMsgBaseControl& aControl,
                                                   TMsgAsyncControlState aNewState,
                                                   TMsgAsyncControlState aOldState )
    {
    UNILOGGER_WRITEF( _L("SlideLoader: MsgAsyncControlStateChanged: aNewState: %d"), aNewState );
    UNILOGGER_WRITEF( _L("SlideLoader: MsgAsyncControlStateChanged: aOldState: %d"), aOldState );
    
    CMsgMediaControl* mediaControl = NULL;
    CMsgXhtmlBodyControl* xhtmlControl = NULL;
    
    ResolveCorrectControlType( aControl, mediaControl, xhtmlControl );
   
    if ( mediaControl )
        {
#ifdef USE_LOGGER
        if ( mediaControl->Error() )
            {
            UNILOGGER_WRITEF( _L("SlideLoader: MsgAsyncControlStateChanged: error: %d"), mediaControl->Error() );
            }
#endif
        // When DSP is used for audio/video playing, dsp resources are not always available.
        // It is handled by setting normal 'ready' icon. No error notes occur.
        // If set, no icon update, normal icon stays...
        TInt dspResourceLack( EFalse );
        switch ( aNewState )
            {
            case EMsgAsyncControlStateCorrupt:
            case EMsgAsyncControlStateNoRights:
            case EMsgAsyncControlStateError:
                { 
                // Release the animation.
                mediaControl->ReleaseAnimation();
                
                TInt controlId = mediaControl->ControlId();
                
                TUniRegion region( EUniRegionUnresolved );
                switch ( controlId )
                    {
                    case EMsgComponentIdImage:
                        {
                        region = EUniRegionImage;
                        break;
                        }
                    case EMsgComponentIdAudio:
                        {
                        region = EUniRegionAudio;

                        if ( ( aOldState == EMsgAsyncControlStateAboutToPlay ||  
                               aOldState == EMsgAsyncControlStatePlaying ||  
                               aOldState == EMsgAsyncControlStateReady ||  
                               aOldState == EMsgAsyncControlStateStopped ) &&  
                             ( mediaControl->Error() == KErrSessionClosed ||  
                               mediaControl->Error() == KErrAccessDenied ) )
                            {
                            dspResourceLack = ETrue;
                            }
                        break;
                        }
                    case EMsgComponentIdVideo:
                        {
                        region = EUniRegionImage;
                        
                        if ( ( aOldState == EMsgAsyncControlStateAboutToPlay ||  
                               aOldState == EMsgAsyncControlStatePlaying ||  
                               aOldState == EMsgAsyncControlStateReady ||  
                               aOldState == EMsgAsyncControlStateStopped ) &&  
                             ( mediaControl->Error() == KErrSessionClosed ||  
                               mediaControl->Error() == KErrAccessDenied ) )
                            {
                            dspResourceLack = ETrue;
                            }
                        break;
                        }
    #ifdef RD_SVGT_IN_MESSAGING                    
                    case EMsgComponentIdSvg:
                        {
                        region = EUniRegionImage;
                        break;
                        }
    #endif
                    default:
                        {
                        break;
                        }
                    }
                    
                if ( dspResourceLack )
                    {
                    break;
                    }
                    
                if ( region == EUniRegionUnresolved )
                    {
                    return;
                    }
                       
                TInt countObjectsOnSlide( iDataModel.SmilModel().SlideObjectCount( iLoadedSlide ) );
                
                for ( TInt i = 0;  i < countObjectsOnSlide; i++ )
                    {
                    CUniObject* object = iDataModel.SmilModel().GetObject( iLoadedSlide, region );
                    if ( object )
                        {
                        if ( aNewState == EMsgAsyncControlStateCorrupt ||
                             aNewState == EMsgAsyncControlStateError )
                            {
                            object->SetCorrupted( ETrue );
                            }
                            
                        TRAP_IGNORE( UpdateControlIconL( *mediaControl, object ) );
                        break;
                        }
                    }
                break;
                }
            case EMsgAsyncControlStateReady:
                {
                UNILOGGER_WRITE_TIMESTAMP( "CUniSlideLoader::MsgMediaControlStateChanged" );
                
                if ( mediaControl->IconBitmapId() != KErrNotFound )
                    {
                    mediaControl->SetIconVisible( ETrue );
                    }
                
                // Release the animation.
                mediaControl->ReleaseAnimation();
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else if ( xhtmlControl )
        {
        switch ( aNewState )
            {
            case EMsgAsyncControlStateError:
            case EMsgAsyncControlStateReady:
                {
                if ( iStatus == KRequestPending )
                    {
                    TRequestStatus* pStatus = &iStatus;
                    User::RequestComplete( pStatus, xhtmlControl->Error() );
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// ---------------------------------------------------------
// CUniSlideLoader::MsgAsyncControlResourceChanged
//
// ---------------------------------------------------------
//
void CUniSlideLoader::MsgAsyncControlResourceChanged( CMsgBaseControl& aControl, TInt aType )
    {
    TRAP_IGNORE( DoMsgAsyncControlResourceChangedL( aControl, aType ) );
    }

// ---------------------------------------------------------
// CUniSlideLoader::DoMsgAsyncControlResourceChangedL
//
// ---------------------------------------------------------
//
void CUniSlideLoader::DoMsgAsyncControlResourceChangedL( CMsgBaseControl& aControl, TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        CMsgMediaControl* mediaControl = NULL;
        CMsgXhtmlBodyControl* xhtmlControl = NULL;
    
        ResolveCorrectControlType( aControl, mediaControl, xhtmlControl );
        
        if ( mediaControl )
            {
            SetIconLayoutL( *mediaControl );
            }
        }        
    }

// ---------------------------------------------------------
// CUniSlideLoader::SetIconLayoutL
//
// ---------------------------------------------------------
//
void CUniSlideLoader::SetIconLayoutL( CMsgMediaControl& aMediaControl )
    {
    if ( aMediaControl.IconBitmapId() != KErrNotFound )
        {
        TAknWindowLineLayout layOut( KErrNotFound );
        
        switch ( aMediaControl.ControlId( ) )
            {
            case EMsgComponentIdAudio:
                {
                layOut = AknLayoutScalable_Apps::msg_data_pane_g6().LayoutLine();
                break;
                }
            case EMsgComponentIdImage:
            case EMsgComponentIdVideo:
            case EMsgComponentIdSvg:
            default:
                {
                layOut = AknLayoutScalable_Apps::msg_data_pane_g7().LayoutLine();
                break;
                }
            }
        
        TAknLayoutRect iconLayout;
        iconLayout.LayoutRect( MsgEditorCommons::MsgDataPane(),
                               layOut );
        
        TSize placeholderSize( iconLayout.Rect().Size() );
        
        aMediaControl.SetIconSizeL( placeholderSize );
        aMediaControl.SetAnimationSizeL( placeholderSize );
        }
    }

// ---------------------------------------------------------
// CUniSlideLoader::ResolveCorrectControlType
// ---------------------------------------------------------
//
void CUniSlideLoader::ResolveCorrectControlType( CMsgBaseControl& aControl,
                                                 CMsgMediaControl*& aMediaControl,
                                                 CMsgXhtmlBodyControl*& aXhtmlControl )
    {
    aMediaControl = NULL;
    aXhtmlControl = NULL;
    
    TInt controlType = aControl.ControlType();
    
    if ( controlType == EMsgImageControl ||
         controlType == EMsgVideoControl ||
         controlType == EMsgAudioControl ||
         controlType == EMsgSvgControl )
        {
        aMediaControl = static_cast<CMsgMediaControl*>( &aControl );
        }
    else if ( controlType == EMsgXhtmlBodyControl )
        {
        aXhtmlControl = static_cast<CMsgXhtmlBodyControl*>( &aControl );
        }
    }
    
// ---------------------------------------------------------
// RemoveObservers(
// ---------------------------------------------------------
//
void  CUniSlideLoader::RemoveObservers( )
    {
    if ( iUniObjectObserver )
        {
        CUniObject* obj =  ObjectByBaseControl( iView.ControlById( EMsgComponentIdBody ) );
        if ( obj )
            {
            obj->RemoveObserver( iUniObjectObserver );
            }
        obj =  ObjectByBaseControl( iView.ControlById( EMsgComponentIdImage ) );
        if ( obj )
            {
            obj->RemoveObserver( iUniObjectObserver );
            }
        obj =  ObjectByBaseControl( iView.ControlById( EMsgComponentIdVideo ) );
        if ( obj )
            {
            obj->RemoveObserver( iUniObjectObserver );
            }
        obj =  ObjectByBaseControl( iView.ControlById( EMsgComponentIdAudio ) );
        if ( obj )
            {
            obj->RemoveObserver( iUniObjectObserver );
            }
#ifdef RD_SVGT_IN_MESSAGING     
        obj =  ObjectByBaseControl( iView.ControlById( EMsgComponentIdSvg ) );
        if ( obj )
            {
            obj->RemoveObserver( iUniObjectObserver );
            }
#endif
        }
    }

    
// ---------------------------------------------------------
// ObjectByBaseControl
// ---------------------------------------------------------
// 
CUniObject* CUniSlideLoader::ObjectByBaseControl( CMsgBaseControl* aControl )
    {    
    if ( aControl )
        {
        TInt controlId = aControl->ControlId( );
        TUniRegion region( EUniRegionUnresolved );
        switch ( controlId )
            {
            case EMsgComponentIdImage:
                region = EUniRegionImage;
                break;
            case EMsgComponentIdAudio:
                region = EUniRegionAudio;
                break;
            case EMsgComponentIdVideo:
                region = EUniRegionImage;
                break;
    #ifdef RD_SVGT_IN_MESSAGING
            case EMsgComponentIdSvg:
                region = EUniRegionImage;
                break;
    #endif            
            case EMsgComponentIdBody:
                region = EUniRegionText;
                break;
            default:
                break;
            }
        if ( region == EUniRegionUnresolved )
            {
            return NULL;
            }
                  
        TInt countObjectsOnSlide( 
                iDataModel.SmilModel().SlideObjectCount( iLoadedSlide ) );
        for ( TInt i = 0;  i < countObjectsOnSlide; i++ )
            {
            CUniObject* object = iDataModel.SmilModel().GetObject( iLoadedSlide, region );
            if ( object )
                {
                return object;
                }
            }
        }
    return NULL;
    }

//  End of File
