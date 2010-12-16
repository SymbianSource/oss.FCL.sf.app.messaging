/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides Uni Editor App UI methods. 
*
*/


 
// ========== INCLUDE FILES ================================

// Symbian OS
#include <apmstd.h>                 // TDataType, KMaxDataTypeLength 
#include <data_caging_path_literals.hrh> 
#include <eiklbv.h>

// Messaging
#include <msvapi.h>
#include <msvuids.h>                // Msgs Uids
#include <msvstd.hrh>               // KUidMsvMessageEntryValue
#include <mtmdef.h>                 // TMsvPartList
#include <mtmuidef.hrh>             // EMtmUiFlagEditorPreferEmbedded
#include <MtmExtendedCapabilities.hrh>  // function id of MessageInfo

// Eikon
#include <eikedwin.h>               // Editor TextLenght etc
#include <eikrted.h>                // for CEikRichTextEditor
#include <eikcolib.h>
#include <DocumentHandler.h>

// Avkon
#include <aknenv.h>
#include <aknnavi.h>
#include <aknnavide.h>              // CAknNavigationDecorator
#include <aknnavilabel.h>
#include <aknEditStateIndicator.h> 
#include <AknIndicatorContainer.h> 
#include <akntitle.h>
#include <aknclearer.h>
#include <AknsConstants.h>
#include <AknInfoPopupNoteController.h>
#include <akninputblock.h>            // for CAknInputBlock
#include <aknbutton.h>

#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include <aknnotewrappers.h>        // CAknNoteWrappers
#include <aknnotedialog.h>          // CAknNoteDialog
#include <AknQueryDialog.h>         // CAknQueryDialog
#include <AknWaitDialog.h>          // CAknWaitDialog
#include <akncheckboxsettingpage.h>     // also CSelectionItemList and CSelectableItem
#include <aknsettingpage.h>         // for Priority bit dialog
#include <aknradiobuttonsettingpage.h>
#include <akntoolbar.h>
#include <akntoolbarextension.h>
#include <AknIconArray.h>           // CAknIconArray
#include <avkon.mbg>                // 
#include <AknStatuspaneUtils.h>     // AknStatuspaneUtils
#include <AknUtils.h>

#include <AknFepGlobalEnums.h>              // Global Fep enums (ELatin, ENumber)
#include <AknFepInternalCRKeys.h>

// Misc
#include <txtrich.h>                // for CRichText
#include <badesca.h>                // CDesC16Array
#include <barsread.h>               // TResReader

// MUIU
#include <MuiuOperationWait.h>      // CMuiuOperationWait
#include <MuiuMsvUiServiceUtilities.h>  //Disk space check, offline check
#include <muiumsvuiserviceutilitiesinternal.h> 

#include <fileprotectionresolver.h>

// Common components
#include <StringLoader.h>           // for StringLoader (load and foramt strings from resources)
#include <AiwGenericParam.h>        // CAiwGenericParamList

// Base editor 
#include <MsgRecipientItem.h>       // for CMsgRecipientItem
#include <MsgAttachmentUtils.h>     // for MsgAttachmentUtils
#include <MsgBaseControl.h>         // for CMsgBaseControl
#include <MsgBodyControl.h>         // for CMsgBodyControl
#include <MsgAddressControl.h>      // for CMsgAddressControl
#include <MsgEditorView.h>          // for CMsgEditorView
#include <MsgEditor.hrh>            // for TMsgIndicatorFlags
#include <msgimagecontrol.h>        // for CMsgImageControl
#include <MsgAttachmentModel.h>     // for CMsgAttachmentModel
#include <MsgEditorCommon.h>

#include <MsgEditorAppUiExtension.h>// for iMsgEditorAppUiExtension
#include <MsgEditorSettingCacheUpdatePlugin.h>

// MmsEngine
#include <mmsgenutils.h>

// Features
#include <featmgr.h>    
#include <messagingvariant.hrh>

#include <centralrepository.h>    // link against centralrepository.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys
#include <CoreApplicationUIsSDKCRKeys.h>

#include <RCustomerServiceProfileCache.h>   // CSP bits

// HELP
#include <hlplch.h>
#include <csxhelp/unif.hlp.hrh>

// SMIL Player
#include <MMediaFactoryFileInfo.h>
#include <SmilPlayerDialog.h>

// Other SMIL stuff
#include <gmxmldocument.h>

// MsgMedia
#include <MsgMimeTypes.h>
#include <MsgAudioInfo.h>
#include <MsgVideoInfo.h>
#include <MsgImageInfo.h>
#include <MsgTextInfo.h>
#include <MsgMediaResolver.h>
#include <MsgMedia.hrh>
#include <MmsConformance.h>

// Resource identifiers
#include <UniEditor.rsg>
#include <MsgEditorAppUi.rsg>


#include "UniSendingSettings.h"
#include "UniPluginApi.h"
#include "UniSmsPlugin.h"
#include "UniClientMtm.h"

/// UniEditor other component
#include "uniutils.h"               // Object type flags, MBM filename
#include <uniutils.mbg>  
#include <unimsventry.h>          
#include "unislideloader.h"
#include "unidatautils.h"
#include "uniobjectlist.h"
#include "unidatamodel.h"             // CUniDataModel
#include "unismilmodel.h"           // CUniSmilModel
#include "unimodelconst.h"          // Uni constants
#include "uniobjectsmodel.h"        // Model part of the objects view 
#include "unitextobject.h"
#include "unimimeinfo.h"
#include "uniobjectsviewdialog.h"
#include "unidrminfo.h"

// This must be included after unidatamodel.h as both GMXMParserxxx and this define KNewLine
#include <aknconsts.h>              // KAvkonBitmapFile

// UniEditor application
#include "UniEditorApp.h"           // To get app UID
#include "UniEditorEnum.h"          // Panic codes
#include "UniEditorApp.hrh"            // application specific commands
#include "UniEditorAppUi.h"         // MmsEditorAppUi class
#include "UniEditorHeader.h"
#include "UniEditorChangeSlideOperation.h"
#include "UniEditorInsertOperation.h"
#include "UniEditorLaunchOperation.h"
#include "UniEditorSaveOperation.h"
#include "UniEditorSendOperation.h"
#include "UniEditorVCardOperation.h"
#include "UniEditorAddHeaderDialog.h"
#include "UniSendingSettingsDialog.h"
#include "UniEditorLogging.h"

#include <gsmuelem.h>     // Turkish SMS-PREQ2265 specific
#include <aknstyluspopupmenu.h> //Added for displaying object specific stylus menus in editor
// ========== LOCAL CONSTANTS AND MACROS ===================

// SMS related

// Character count used in unicode check buffer
const TInt KUnicodeCheckChars = 10;

// Downwards arrow with tip leftwards
// This does not count as unconvertible character as it will be
// replaced by paragraph separator or line feed by UI
const TUint KSmsDownwardsArrowLeft = 0x21B2;

const TInt  KSmsEdPDUInfoCalcReplaceCharacterCount = 2;
const TUint KSmsEdUnicodeLFSupportedByBasicPhones = 0x000A;
const TUint KSmsEnterCharacter = 0x2029;
// Unicode char codes for GSM 03.38 7 bit ext table characters 
const TUint KUniEdEuroSymbol = 0x20ac;
const TUint KUniEdLeftSquareBracket = 0x5b;
const TUint KUniEdReverseSolidus = 0x5c;
const TUint KUniEdRightSquareBracket = 0x5d;
const TUint KUniEdCircumflexAccent = 0x5e;
const TUint KUniEdLeftCurlyBracket = 0x7b;
const TUint KUniEdVerticalLine = 0x7c;
const TUint KUniEdRightCurlyBracket = 0x7d;
const TUint KUniEdTilde = 0x7e;
const TUid  KUniEditorAppId = { 0x102072D8 };

// Amount of chars reserved for msg length and message count indication text
const TInt KMsgLengthIndicationBuffer = 64;

// Bytes. Estimated disk space needed in addition to filesize when file is created as an attachment.
const TInt  KUniAttachmentOverhead          = 1024; 

// Some functions determine and return the info note 
// resource id at error situations. When not determined
// this value is returned.  
const TInt  KUniInvalidResourceId           = -1;   

// Non-empty par tag + one media object takes ~90 bytes
const TInt  KEmptySlideSize                 = 90;  

// Kilobyte is 1024 not 1000 bytes 
const TInt  KBytesInKilo                    = 1024; 

// Kilobyte limit when to stop showing message size
// with kilo accuracy 
const TInt  KOneDigitLimit                  = 10;   

// BodyText + subject, needed when counted the message size
const TInt KUniEdNumberOfEditors            = 2; 

// Extra length added to SMS length when there's subject
const TInt KUniEdExtraLenCausedBySubject    = 2; 

const TInt KUniEdMaxSubjectLen              = 40;

// For creation of checkbox list
_LIT(KItemStart, "1\t");                    

// Exiting in launch. Let this time to show error notes
const TInt  KDelayedExitShort = 100000;

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------
// CUniEditorAppUi::CUniEditorAppUi
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CUniEditorAppUi::CUniEditorAppUi() :   
    iDisplaySize( -1 ),
    iWaitResId( -1 ),
    iOriginalSlide( -1 ),
    iNextFocus( EMsgComponentIdNull ),
    iEditorFlags( EShowInfoPopups ),
    iMskResId( R_UNIEDITOR_OPTIONS_CLOSE ),
    iOptimizedFlow(EFalse),
    iSingleJpegImageProcessing(EFalse)
    {
    }
    
// ---------------------------------------------------------
// CUniEditorAppUi::ConstructL
// Second phase contructor
// NOTE: Don't call here any function that uses iMtm 
//       and/or iSmilModel members, because they are 
//       initialized at LaunchViewL.
// ---------------------------------------------------------
//
void CUniEditorAppUi::ConstructL()
    {
    iFinalizeLaunchL = EFalse;
    // We don't have any app ui if launched from idle
    if ( iEikonEnv->EikAppUi() )
        {
        ActivateInputBlockerL( NULL );
        }
        
    CMsgEditorAppUi::ConstructL();

	SetKeyEventFlags( EDisableSendKeyShort | EDisableSendKeyLong );
    SetCloseWithEndKey( ETrue );
    
    FeatureManager::InitializeLibL();
    
    // Get supported features from feature manager.    
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        iSupportedFeatures |= EUniFeatureHelp;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
        {
        iSupportedFeatures |= EUniFeatureOffline;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdDrmFull ) )
        {
        iSupportedFeatures |= EUniFeatureDrmFull;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdCamera ) )
        {
        iSupportedFeatures |= EUniFeatureCamcorder;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdJapanese ) )
        {
        iSupportedFeatures |= EUniFeatureJapanese;
        }
    //Turkish SMS-PREQ2265 specific
    if( FeatureManager::FeatureSupported( KFeatureIdNltSupport ) )
        {
        iNLTFeatureSupport = ETrue;
        }
    else
        {
        iNLTFeatureSupport = EFalse;
        }
    
    FeatureManager::UnInitializeLib();

    iMuiuSettRepository = CRepository::NewL( KCRUidMuiuSettings );

    TInt featureBitmask = 0;
    
    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, featureBitmask );
    
    if ( featureBitmask & KMmsFeatureIdEditorSubjectField )
        {
        iSupportedFeatures |= EUniFeatureSubject;
        }
        
    if ( featureBitmask & KMmsFeatureIdPrioritySupport )
        {
        iSupportedFeatures |= EUniFeaturePriority;
        }    
    
    iInputTxtLangRepository = CRepository::NewL(  KCRUidAknFep   );     
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this,
                                                 *iInputTxtLangRepository,                                                 
                                                 CCenRepNotifyHandler::EIntKey,
                                                 KAknFepInputTxtLang );
    featureBitmask = 0;    
    repository->Get( KMuiuUniEditorFeatures, featureBitmask );
    
    if ( featureBitmask & KUniEditorFeatureIdHideMsgTypeChangedPopup )
        {
        iEditorFlags &= ~EShowInfoPopups;
        }    
        
    if ( featureBitmask & KUniEditorFeatureIdHideSizeCounter )
        {
        iEditorFlags |= EHideSizeCounter;
        }    
    if ( featureBitmask & KUniEditorFeatureIdHideMessageTypeOption )
		{
		iEditorFlags |= EHideMessageTypeOption;
		}     
    repository->Get( KMuiuSmsFeatures, featureBitmask );
    
    if ( featureBitmask & KSmsFeatureIdShowConcatenatedQuery )
        {
        iEditorFlags |= EShowSmsSentInManyParts;
        }  
       
    delete repository;  
    
    iNotifyHandler->StartListeningL();    
    // Disable task swapper from options menu during launch
    MenuBar()->SetMenuType( CEikMenuBar::EMenuOptionsNoTaskSwapper );
    
    iTitleSms = StringLoader::LoadL( R_UNIEDITOR_TITLE_SMS, iCoeEnv );    
    iTitleMms = StringLoader::LoadL( R_UNIEDITOR_TITLE_MMS, iCoeEnv );    
    
    // Load msg length format string from resource
    iSmsLengthFormatBuffer = StringLoader::LoadL( R_UNIEDITOR_SMS_LENGTH, iCoeEnv );
    iMmsLengthFormatBuffer = StringLoader::LoadL( R_UNIEDITOR_MMS_LENGTH, iCoeEnv );

    CEikStatusPane* statusPane = StatusPane();
    
    // Set generic title text as message type is not known yet.
    HBufC* titleString = StringLoader::LoadLC( R_UNIEDITOR_TITLE, iCoeEnv );    
        
    iTitlePane = static_cast<CAknTitlePane*>( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    iTitlePane->SetTextL( titleString->Des(), ETrue );
    
    CleanupStack::PopAndDestroy( titleString );
    
    iNaviPane = static_cast<CAknNavigationControlContainer*>( statusPane->ControlL( 
                                                                TUid::Uid( EEikStatusPaneUidNavi ) ) );
    
    iNaviDecorator = iNaviPane->CreateEditorIndicatorContainerL();
    
    TParse fileParse;
    fileParse.Set( KUniUtilsMBMFileName, &KDC_APP_BITMAP_DIR, NULL );

    iIconSms = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                                          KAknsIIDQgnPropMceSmsTitle,
                                          fileParse.FullName(),
                                          EMbmUniutilsQgn_prop_mce_sms_title,
                                          EMbmUniutilsQgn_prop_mce_sms_title_mask );

    iIconMms = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                                          KAknsIIDQgnPropMceMmsTitle,
                                          fileParse.FullName(),
                                          EMbmUniutilsQgn_prop_mce_mms_title,
                                          EMbmUniutilsQgn_prop_mce_mms_title_mask );
    
    SetTitleIconsSizeL();
    
    if ( Document()->CreationMode() == EMmsCreationModeWarning )
        {
        iEditorFlags |= EShowGuidedConf;
        }
        
    // Prepare to launch msgeditorview to display the message
    if ( !iEikonEnv->StartedAsServerApp() )
        {
        Document()->PrepareToLaunchL( this );
        }

    iScreenClearer = CAknLocalScreenClearer::NewLC( ETrue );
    CleanupStack::Pop( iScreenClearer );        
	CreateFixedToolbarL( ETrue);
    
    // Priority must be lower than the priority of iWrappedWaitDialog
    // Constructed here to ensure exit on low memory situations.
    iIdle = CPeriodic::NewL( EPriorityNormal - 1 );
    
#ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() )
        {
            User::LeaveIfError( iPeninputServer.Connect() );
            iPeninputServer.AddPenUiActivationHandler( this, EPluginInputModeAll );
            iPrevSmsLength = -1;    
            iMsgLenToVKB = ETrue; 
        }
#endif
    	iLongTapDetector = CAknLongTapDetector::NewL( this );
		iLongTapDetector->SetTimeDelayBeforeAnimation( KUniLongTapStartDelay );
		iLongTapDetector->SetLongTapDelay( KUniLongTapTimeDelay );
		iTapConsumed = EFalse;
    }


// ---------------------------------------------------------
// CUniEditorAppUi::~CUniEditorAppUi
// Destructor
// ---------------------------------------------------------
//
CUniEditorAppUi::~CUniEditorAppUi()
    {
    iEditorFlags |= EEditorExiting;
    
	//sendui+jepg optimization changes
    if(iLaunchOperation)
        {
        // check Is iLaunchOperation still attached to 
        // iSlideLoader / iHeader
        // Set the CUniEditorAppUi
        // instance to NULL, to avoid crash.
        if(iLaunchOperation->GetHeader())
            {
            iHeader = NULL;            
            }
        if(iLaunchOperation->GetSlideLoader())
            {
            iSlideLoader = NULL;            
            }
        }
    if ( iView )
        {
        // To prevent focus changes caused by input blocker deletion & toolbar extension
        // closing.
        iEikonEnv->EikAppUi()->RemoveFromStack( iView );
        
        if ( iEditorFlags & ELaunchSuccessful )
            {    
            TInt id( EMsgComponentIdNull );
            
            // remember input mode:
            // observer does not get event, if editor is closed 
            CMsgBaseControl* ctrl = iView->FocusedControl();  // ctrl can be NULL.
            if ( ctrl )
                {
                id = ctrl->ControlId();
                if ( iHeader &&  
                     !iHeader->IsAddressControl( id ) )
                    {
                    id = EMsgComponentIdNull;
                    }
                }
                
            if ( id == EMsgComponentIdNull )
                { 
                id = EMsgComponentIdTo;
                }
            
            CUniBaseHeader::THeaderFields headerField = CUniBaseHeader::EHeaderAddressTo;
            
            if ( id == EMsgComponentIdCc )
                {
                headerField = CUniBaseHeader::EHeaderAddressCc;
                }
            else if ( id == EMsgComponentIdBcc )
                {
                headerField = CUniBaseHeader::EHeaderAddressBcc;
                }
            
            CMsgAddressControl* rec = 
                iHeader ? static_cast<CMsgAddressControl*>( iHeader->AddressControl( headerField ) ) :
                        NULL;

            if ( rec ) 
                {
                TInt inputMode = rec->Editor().AknEditorCurrentInputMode();
                
                if ( iMuiuSettRepository )
                    {
                    iMuiuSettRepository->Set( KMuiuToInputMode, inputMode );
                    }
                }
            }
        }
    
    delete iLaunchOperation;
    delete iSaveOperation;
    delete iSendOperation;
    delete iInsertOperation;
    delete iChangeSlideOperation;
    delete iVCardOperation;
    
    delete iParser;
    delete iDom;
    
    delete iInputBlocker;

#ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() )
         {
          iPeninputServer.Close();
         }
#endif
    
    delete iTitleSms;
    delete iTitleMms;

    delete iIconSms;
    delete iIconMms;
    
    delete iSmsLengthFormatBuffer;
    delete iMmsLengthFormatBuffer;
    
    delete iPopupNote;
    delete iPopupChangedMmsBuffer;
    delete iPopupChangedSmsBuffer;
    
    delete iMuiuSettRepository;

    delete iWaitDialog;
    delete iIdle;

    delete iInsertingMedia;
    delete iHeader;
    delete iSlideLoader;
    delete iScreenClearer;

    delete iDocHandler;
    delete iStoredBodyControl;
    
    delete iFixedToolbar;
    if( iNotifyHandler )
        {
         iNotifyHandler->StopListening();         
          delete iNotifyHandler;
         }
    
   if(iInputTxtLangRepository)
    {
    delete iInputTxtLangRepository;
    }
  if(iNLTFeatureSupport)
    {   
    delete iPrevBuffer;
    }

    delete iPopupSmsSizeAboveLimitBuffer;
    delete iPopupSmsSizeBelowLimitBuffer;
	if(iLongTapDetector)
	{
    delete iLongTapDetector;
	}
	if(iEmbeddedObjectStylusPopup)
	{
    delete iEmbeddedObjectStylusPopup;
	iEmbeddedObjectStylusPopup = NULL;
	}
   
    }

// ---------------------------------------------------------
// CUniEditorAppUi::LaunchViewL
//
// Called by the document when application view launch should be
// started.
// ---------------------------------------------------------
//
void CUniEditorAppUi::LaunchViewL()
    {

    // Initialize these members here.
    iMtm = &( Document()->Mtm() );

    const TMsvEntry& entry = Document()->Entry();
    if ( entry.iType.iUid != KUidMsvMessageEntryValue )
        {
        // Fixing: JHUA-5SCF8W
        // Backup / Restore restarts editor without proper
        // context. Leave with KErrGeneral.
        User::Leave( KErrGeneral );
        }
        
    if ( entry.Parent() != KMsvDraftEntryIdValue )
        {
        User::Leave( KErrNotSupported );
        }
    if ( !( Document()->LaunchFlags() & EMsgCreateNewMessage ) )
        {
        TRAPD( error, iMtm->LoadMessageL() );
        if ( error )
            {
            if ( error != KErrNoMemory &&
                 error != KErrDiskFull )
                {
                ShowErrorNoteL( R_UNIEDITOR_ERROR_MSG_CORRUPT, ETrue );
                }
            User::Leave( error );
            }
        }    
    
    Document()->SetMessageType();

    if ( TUniMsvEntry::IsMmsUpload( Document()->Entry() ) )
        {
        Document()->SetCreationMode( EMmsCreationModeFree );
        
        if ( iFixedToolbar )
            {
            iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarAddRecipient, ETrue, EFalse );
            }
        }
    
    iView = CMsgEditorView::NewL( *this, CMsgEditorView::EMsgDoNotUseDefaultBodyControl );
    iView->SetEdwinObserverL( this );
    

    iLaunchOperation = CUniEditorLaunchOperation::NewL( *this,
                                                        *this,
                                                        *Document(),
                                                        *iView,
                                                        *this,
                                                        FsSession() );

    iLaunchOperation->Launch();
    }

// ---------------------------------------------------------
// CUniEditorAppUi::FinalizeLaunchL
//
// Performs all the last actions before editor launch ends.
// ---------------------------------------------------------
//
void CUniEditorAppUi::FinalizeLaunchL()
    {
	// In all normal cases other then Sendui+Jepeg
	// iOptimizedFlow will be false and flow should be 
	// same as the normal launch
    if(iOptimizedFlow)
        {
		//if iOptimizedFlow is True, it means
		//sendui+Jepg and this is partial complete call
        iSingleJpegImageProcessing = ETrue;
        }
		
    iFinalizeLaunchL = ETrue;
    iSmilModel = &Document()->DataModel()->SmilModel();
	
    if(!iOptimizedFlow)
        {
		//detach the iHeader and iSlideLoader
        iHeader = iLaunchOperation->DetachHeader();
        iSlideLoader = iLaunchOperation->DetachSlideLoader();
        }
    else
        {
		// get reference to complete partial lauch operation
        iHeader = iLaunchOperation->GetHeader();
        iSlideLoader = iLaunchOperation->GetSlideLoader();
        }
    
    SetMessageTypeLockingL();
    
    TInt headersVariation = iHeader->AddHeadersVariation();

    if ( Document()->EmailOverSmsSupported() ||
         ( headersVariation & EUniFeatureSubject && 
           !( headersVariation & EUniFeatureSubjectConfigurable ) ) )
        { 
        // Enable subject in SMS when it's not selectable in additional headers
        // and when EmailOverSms is supported
        iEditorFlags |= ESubjectOkInSms;
        }
    
    SetAddressSize();
    SetSubjectSize();
    
    // "Remember input mode" feature
    TInt inputMode = EAknEditorNumericInputMode;
    if ( iMuiuSettRepository->Get( KMuiuToInputMode, inputMode ) != KErrNone )
        {
        inputMode = EAknEditorNumericInputMode;
        }

    SetInputModeToAddressFields( inputMode );
    
    CheckLockedSmsWithAttaL();
    
    if ( iSupportedFeatures & EUniFeatureJapanese &&
         Document()->MessageType() != EUniForward )
        {
        // Image first layout is always used in Japanese variant when dealing
        // with any other messages than those that where originally forwarded by user.
        // With forwarded messages the original layout is kept unchanged.
        ChangeOrderL( EUniImageFirst );
        iSmilModel->SetLayoutL( EUniImageFirst );
        }
    
    TMsgControlId focusedControlId = ResolveLaunchFocusedControlL();
            
    
    CEikStatusPane* statusPane = StatusPane();
    
    if ( focusedControlId == EMsgComponentIdBody )
        {
        BodyCtrl()->SetCursorPosL( BodyCtrlEditor()->TextLength() );
        }
    
    if( iNLTFeatureSupport )
        {
        iPrevBuffer = NULL;
        //Turskish SMS-PREQ2265 specific
        TSmsEncoding alternateEncodingType;
        TUniMessageCharSetSupport charSet;
        TInt inputLang = iAvkonEnv->SettingCache().InputLanguage();
        CUniEditorDocument* doc = Document();
        
        alternateEncodingType = doc->GetLanguageSpecificAltEncodingType(inputLang);
        charSet = doc->CharSetSupport();
        
        doc->SetAlternativeEncodingType(alternateEncodingType);
        if(SmsPlugin())
            {
            //Initialize corresponding language alternative encoding types and character set selected
            SmsPlugin()->SetEncodingSettings(EFalse, alternateEncodingType, charSet);
            }
        }
    
    CheckBodyForMessageTypeL();
    CheckHeaderForMessageTypeL();
    
    SetOrRemoveMaxSizeInEdwin();
    
    InitNaviPaneL();
    SetTitleL();
    if ( Document()->MessageType() == EUniSendUi )
        {
        // Show invalid contact notes and verify unvalidated addresses.
        iHeader->ShowInvalidContactNotesL();
        
        TBool unused;
        VerifyAddressesL( unused );
        }
        
    if ( iEditorFlags & EShowInfoPopups )
        {
        iPopupNote = CAknInfoPopupNoteController::NewL();
        iPopupNote->SetTimeDelayBeforeShow( 0 );
        iPopupNote->SetTimePopupInView( 3000 );

        iPopupChangedMmsBuffer = StringLoader::LoadL( R_UNIEDITOR_POPUP_MSG_CHANGED_MMS, iCoeEnv );
        iPopupChangedSmsBuffer = StringLoader::LoadL( R_UNIEDITOR_POPUP_MSG_CHANGED_SMS, iCoeEnv );

        //Korean Req: 415-5434        
        iPopupSmsSizeAboveLimitBuffer = StringLoader::LoadL( R_UNIEDITOR_POPUP_SMS_SIZE_ABOVE_LIMIT, Document()->SmsSizeWarningBytes(), iCoeEnv );
        iPopupSmsSizeBelowLimitBuffer = StringLoader::LoadL( R_UNIEDITOR_POPUP_SMS_SIZE_UNDER_LIMIT, Document()->SmsSizeWarningBytes(), iCoeEnv );

        }

    CheckMaxRecipientsAndShowNoteL();
    
    // Enable task swapper to options menu after launch has been completed.
    MenuBar()->SetMenuType( CEikMenuBar::EMenuOptions );
    
    UpdateToolbarL();

	// partial launch need to call execute to make
	//the editor visible
    if(iOptimizedFlow)
        {
        iView->ExecuteL( ClientRect(), focusedControlId );
        }
    else// not optmized Flow, common flow
        {
        // partial launch, dont set the flag
        iEditorFlags |= ELaunchSuccessful;
        
		// in case of sendui+jepg , again finalize launch will be called 
		//after image processing, no need to call iView->ExecuteL
		// slide will be loaded already by slide loader.
        if(!iSingleJpegImageProcessing)
            {
			//normal flow
            iView->ExecuteL( ClientRect(), focusedControlId );
            }
        
        //after the lauch complete for sendui+jepg
        //rest it.
        iSingleJpegImageProcessing = EFalse;        
        }
    
    delete iScreenClearer;
    iScreenClearer = NULL;
   
    // show note inserting 
    if(iOptimizedFlow)
        {
        ShowWaitNoteL( R_QTN_UNI_WAIT_INSERTING );
        }
 
	
    }
    
// ---------------------------------------------------------
// CUniEditorAppUi::ShowLaunchNotesL
//
// Tries to resolve correct note(s) to be shown when editor is
// launched. First disables showing of guided conformance note if
// message is forwarded with non-conformant content as viewer should
// be responsible of showing this. After this goes through 
// error list and sets correct error resource depending of 
// editor mode and message content. Priority & plural solving
// is performed on SetErrorResource function. Some notes can
// be grouped together so that if error notes from different
// groups are set then general error note should be shown instead.
// After correct error note resource is resolved then notes 
// related to conformance are shown. Do note that there is 
// a EShowGuidedConf flag that controls the visibility of guided
// note. After this if still appropriate and specified
// error note is shown to user.
// ---------------------------------------------------------
//
void CUniEditorAppUi::ShowLaunchNotesL( TBool& aShutDown )
    {
    TInt errRes( KErrNotFound );
    
    if ( Document()->LaunchFlags() & EMsgForwardMessage &&
         ( Document()->UpdatedNonConformantCount() > 0 || 
           Document()->DataModel()->SmilType() == ETemplateSmil ||
           Document()->DataModel()->SmilType() == E3GPPSmil ) )
        {
        iEditorFlags &= ~EShowGuidedConf;
        }
    
    // Tells whether message has files included.
    TBool containsFiles = Document()->DataModel()->ObjectList().Count() > 0 || 
                          Document()->DataModel()->AttachmentList().Count() > 0;
    
    TBool useRModeStrings = Document()->CreationModeUserChangeable() &&
                            Document()->CreationMode() == EMmsCreationModeRestricted;
    
    CArrayFixFlat<TInt>* errors = iLaunchOperation->GetErrors();
    
    if ( Document()->DataModel()->ParseResult() & EObjectRemoved )
        {
        SetErrorResource( errRes, R_UNIEDITOR_INFO_MAX_SLIDES_SOME );
        }
    
    for ( TInt i = 0; i < errors->Count(); i++ )
        {
        switch ( errors->At( i ) )
            {
            case KUniLaunchAbortPresRestricted:
                {
                // Set only for 3GPP or template SMIL presentations when creation
                // mode is restricted.
                if ( useRModeStrings )
                    {
                    SetErrorResource( errRes, R_UNIEDITOR_ERROR_RMODE_CANNOT_OPEN );
                    }
                else
                    {
                    SetErrorResource( errRes, R_UNIEDITOR_ERROR_CANNOT_OPEN );
                    }
                    
                break;
                }
            case KUniLaunchPresGuided:
                {
                // Set only for 3GPP or template SMIL presentations when creation
                // mode is guided.
                SetErrorResource( errRes, R_UNIEDITOR_QUEST_GUIDED_PRESENTATION );
                break;
                }
            case EUniProcessImgOutOfMemory:
            case EUniProcessImgOutOfDisk:
            case EUniProcessImgNotFound:
            case EUniProcessImgNonConformant:
            case EUniProcessImgScalingFailed:
            case EUniProcessImgCompressFailed:
            case EUniProcessImgCouldNotScale:
                {                
                if ( useRModeStrings )
                    {
                    SetErrorResource( errRes, R_UNIEDITOR_INFO_RMODE_SCALING_FAILED );
                    }
                else
                    {
                    SetErrorResource( errRes, R_UNIEDITOR_INFO_SCALING_FAILED );
                    }    
                break;
                }
            case EUniSendUiUnsupported:
                {                
                if ( containsFiles )
                    {
                    if ( useRModeStrings )
                        {
                        SetErrorResource( errRes, R_UNIEDITOR_INFO_RMODE_SOME_NOT_SUPPORTED );
                        }
                    else
                        {
                        SetErrorResource( errRes, R_UNIEDITOR_INFO_SOME_NOT_SUPPORTED );
                        }   
                    }
                else
                    {
                    if ( useRModeStrings )
                        {
                        SetErrorResource( errRes, R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECT );
                        }
                    else
                        {
                        SetErrorResource( errRes, R_UNIEDITOR_INFO_UNSUPPORTED_OBJECT );
                        }   
                    }
                break;
                }
            case EUniSendUiForbidden:
                {
                // Note showing should be handled by SendUI in this case.
                break;
                }
            case EUniSendUiTooBig:
                {
                if ( containsFiles )
                    {
                    SetErrorResource( errRes, R_UNIEDITOR_INFO_OBJECT_SEND_AS_TOO_BIG );
                    }
                else
                    {
                    SetErrorResource( errRes, R_UNIEDITOR_INFO_OBJECT_TOO_BIG );
                    }
                    
                break;
                }
            case EUniEditorExit:
            case EUniProcessImgUserAbort:
                {
                // Program error or user has selected to abort image processing at some stage.
                // -> shutdown immediatelly.
                aShutDown = ETrue;  
                return;
                }
            default:
                {
                // e.g leave in startup 
                break;
                }
            }
        }
        
    if ( errRes == R_UNIEDITOR_ERROR_CANNOT_OPEN ||
         errRes == R_UNIEDITOR_ERROR_RMODE_CANNOT_OPEN )
        {
        // Message cannot be opened.
        ShowConfirmableInfoL( errRes );
        
        aShutDown = ETrue;
        return;
        }   
    else if ( errRes == R_UNIEDITOR_INFO_OBJECT_TOO_BIG ||
              errRes == R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECT ||
              errRes == R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECTS ||
              errRes == R_UNIEDITOR_INFO_UNSUPPORTED_OBJECT ||
              errRes == R_UNIEDITOR_INFO_UNSUPPORTED_OBJECTS )
        {
        ShowInformationNoteL( errRes, ETrue );
        
        aShutDown = ETrue;
        return;
        }
    else if ( errRes == R_UNIEDITOR_INFO_OBJECT_SEND_AS_TOO_BIG || 
              errRes == R_UNIEDITOR_INFO_RMODE_SOME_NOT_SUPPORTED ||
              errRes == R_UNIEDITOR_INFO_SOME_NOT_SUPPORTED )
        {
        ShowInformationNoteL( errRes, ETrue );
        }
    else if ( errRes == R_UNIEDITOR_INFO_MAX_SLIDES_SOME )
        {
        ShowConfirmableInfoL( errRes );
        }
    
    if ( errRes == R_UNIEDITOR_QUEST_GUIDED_PRESENTATION )
        {
        if ( !ShowGuidedModeConfirmationQueryL( errRes ) ) 
            {
            aShutDown = ETrue;
            }
        }
    else
        {
        if ( Document()->CreationMode() != EMmsCreationModeFree )
            {
            TInt nonConformantObjects = iLaunchOperation->ParseResult();
            
            if ( nonConformantObjects > 0 )
                {
                if ( Document()->CreationMode() == EMmsCreationModeRestricted )
                    {
                    if ( Document()->CreationModeUserChangeable() )
                        {
                        ShowConfirmableInfoL( R_UNIEDITOR_ERROR_RMODE_CANNOT_OPEN );
                        }
                    else
                        {
                        ShowConfirmableInfoL( R_UNIEDITOR_ERROR_CANNOT_OPEN );
                        }
                        
                    aShutDown = ETrue;
                    return;
                    }
                else if ( Document()->SuperDistributableObjectCount() == 0 ||  
                          Document()->MessageType() != EUniSendUi )
                    {
                    // Not shown if DRM note has been shown already by SendUi.
                    TInt confRes( KErrNotFound );
                    
                    if ( Document()->Saved() )
                        {
                        confRes = R_UNIEDITOR_QUEST_GUIDED_INC_OBJ;
                        if ( nonConformantObjects > 1 )
                            {
                            confRes = R_UNIEDITOR_QUEST_GUIDED_INC_OBJS;
                            }
                        }
                    else
                        {
                        confRes = R_UNIEDITOR_QUEST_GUIDED_OBJ;
                        if ( nonConformantObjects > 1 )
                            {
                            confRes = R_UNIEDITOR_QUEST_GUIDED_OBJS;
                            }
                        }
                    
                    if ( !ShowGuidedModeConfirmationQueryL( confRes ) ) 
                        {
                        // Query not accepted
                        aShutDown = ETrue;
                        return;
                        }
                    }
                }
            }
        }
        
    if ( errRes == R_UNIEDITOR_INFO_RMODE_SCALING_FAILED ||
         errRes == R_UNIEDITOR_INFO_SCALING_FAILED )
        {
        ShowInformationNoteL( errRes, ETrue );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::InsertPresentation
// ---------------------------------------------------------
//
void CUniEditorAppUi::InsertPresentationL( TBool aEditable )
    {
    if ( Document()->DataModel()->SmilType() != EMmsSmil )
        {
        //Delete body control. OK if dooesn't exist.
        iView->DeleteControlL( EMsgComponentIdBody );
        
        CMsgImageControl* imgCtrl = CMsgImageControl::NewL( *iView,
                                                            this );
        CleanupStack::PushL( imgCtrl );
        
        TAknsItemID id = KAknsIIDQgnGrafMmsUnedit;
        TInt icon = EMbmUniutilsQgn_graf_mms_unedit;
        TInt mask = EMbmUniutilsQgn_graf_mms_unedit_mask;

        if ( aEditable )
            {
            id.Set( KAknsIIDQgnGrafMmsEdit );
            icon = EMbmUniutilsQgn_graf_mms_edit;
            mask = EMbmUniutilsQgn_graf_mms_edit_mask;
            }

        TParse fileParse;
        fileParse.Set( KUniUtilsMBMFileName, &KDC_APP_BITMAP_DIR, NULL );
        imgCtrl->LoadIconL( fileParse.FullName(), id, icon, mask );
        
        TAknLayoutRect iconLayout;
        iconLayout.LayoutRect( MsgEditorCommons::MsgDataPane(),
                               AknLayoutScalable_Apps::msg_data_pane_g4().LayoutLine() );
        
        imgCtrl->SetIconSizeL( iconLayout.Rect().Size() );
        
        //Draw it
        imgCtrl->SetIconVisible( ETrue );
        iView->AddControlL( imgCtrl, EMsgComponentIdImage, EMsgFirstControl, EMsgBody );

        CleanupStack::Pop( imgCtrl );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::MsgLengthToNavipaneL
// ---------------------------------------------------------
//
void CUniEditorAppUi::MsgLengthToNavipaneL( TBool aForceDraw )
    {
    UNILOGGER_WRITE( "-> CUniEditorAppUi::MsgLengthToNavipaneL" );
    
    TBuf<KMsgLengthIndicationBuffer> sizeString;

    // If message is SMS type, iSmsBodyLength + iSmsSubjectLnegth will be used as length
    // If EUnicodeMode is set, they will be handled as unicode length
    if ( !( iEditorFlags & EHideSizeCounter ) )
        { 
        CUniEditorDocument* doc = Document();
        
        // It's ok to show the counter
        if ( doc->UniState() == EUniSms )
            {
           
            CUniDataModel* model = doc->DataModel();           
                       
         
                TInt charsLeft,msgsParts;
                TInt lengthOne = 0;
                TInt lengthMany = 0;
                
                doc->SmsCharacterLimits( lengthOne, lengthMany );
                TInt maxSmsCharacters = doc->AbsoluteMaxSmsCharacters();
                TInt maxSmsParts = doc->AbsoluteMaxSmsParts();
                 
                if ( maxSmsCharacters <= 0 )
                    {
                    // No absolute maximum characters defined -> Calculate
                    // character maximum using maximum SMS part limit.
                    maxSmsCharacters = maxSmsParts * lengthMany;
                    }
                CalculateSMSMsgLen(charsLeft,msgsParts);
                if( model->ObjectList().Count() == 0 &&
                    model->AttachmentList().Count() == 1 &&
                    iSmilModel->SlideCount() == 1 &&
                    iSmsSubjectLength == 0)
                        {
                     
                         sizeString.AppendNum( msgsParts );                        
                        
                        }
                else if ( maxSmsCharacters <= lengthOne )
                    {
                    // Maximum less or equal to length of
                    // one part -> show only characters left value.
                    sizeString.AppendNum( charsLeft );
                    }
                else
                    {
                    HBufC* tempBuf = iSmsLengthFormatBuffer;

                    if ( AknLayoutUtils::LayoutMirrored() )
                        { 
                        // Add the RTL marker in the format buffer
                        tempBuf = iSmsLengthFormatBuffer->AllocL();
                        tempBuf = tempBuf->ReAllocL( tempBuf->Length() + 2 );
                        CleanupStack::PushL( tempBuf );
                        TPtr tempPtr = tempBuf->Des();
                        TChar KUniRLMUnicode( 0x200F ); // Right to Left Marker
                        TBuf<1> charBuf;
                        charBuf.Append(KUniRLMUnicode);
                        tempPtr.Insert( 4, charBuf );
                        tempPtr.Insert( 9, charBuf );
                        }

                    StringLoader::Format( sizeString, tempBuf->Des(), 0, charsLeft );

                    if( tempBuf != iSmsLengthFormatBuffer )
                        {
                        CleanupStack::PopAndDestroy( tempBuf );
                        }
                        
                    HBufC* temp = sizeString.AllocLC();
                    
                    StringLoader::Format( sizeString, *temp, 1, msgsParts );

                    CleanupStack::PopAndDestroy( temp );
                    }
                iEditorFlags |= ESmsCounterShownLast;
                
                // Now the string is in buf     
                
#ifdef RD_SCALABLE_UI_V2
                if ( ( AknLayoutUtils::PenEnabled() && iPrevSmsLength != charsLeft ) || ( AknLayoutUtils::PenEnabled() &&  aForceDraw ))
                    {
                    if(!(iPrevSmsLength == -1 && !UpdateMsgLenToPenInput() ) ) // converted from MMS back to SMS
                        {                     
	                    iPrevSmsLength = charsLeft;
	                    iPeninputServer.UpdateAppInfo( sizeString, EAppIndicatorMsg );
                        }
                    }
#endif
               
            // Reset this so that MMS size is drawn when it's shown next time
            iDisplaySize = 0; 
            }
        else
            { 
            // OK we are dealing with MMS now
            // Just take the size of the whole message and show it in the navipane
            TInt bytes = MessageSizeInBytes();   

            TInt integer = iDisplaySize / 10;
            TInt fragment = iDisplaySize % 10;

            CreateMmsNaviSizeStringL(      bytes,      sizeString,
                                        integer,    fragment );

            TInt tempDispSize = integer * 10 + fragment;

            if ( !aForceDraw && iDisplaySize == tempDispSize && !( iEditorFlags & ESmsCounterShownLast ) )
                {
                // No changes to navipane
                return;
                }
#ifdef RD_SCALABLE_UI_V2
            if (  AknLayoutUtils::PenEnabled() && (iEditorFlags & ESmsCounterShownLast ))
                {
                     iPeninputServer.UpdateAppInfo( KNullDesC, EAppIndicatorMsg );
                     iPrevSmsLength = -1;
                }
#endif

            iEditorFlags &= ~ESmsCounterShownLast;
            iDisplaySize = tempDispSize;
        
            }
        }

    // Update both Edwin indicator pane and navi indicator pane
    MAknEditingStateIndicator* editIndi = iAvkonEnv->EditingStateIndicator();
    
    CAknIndicatorContainer* naviIndi = 
        static_cast<CAknIndicatorContainer*>( iNaviDecorator->DecoratedControl() );
    
    if( editIndi )
        {
        CAknIndicatorContainer* indiContainer = editIndi->IndicatorContainer();
        if ( indiContainer )
            {
            if( iEditorFlags & EHideSizeCounter )
                { 
                // Just hide it
                indiContainer->SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength), 
                        EAknIndicatorStateOff);
                }
            else
                {
                indiContainer->SetIndicatorValueL(TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength), sizeString);
                indiContainer->SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength), 
                        EAknIndicatorStateOn);
                }            
            }
        }
        
    if( naviIndi )
        {
        if( iEditorFlags & EHideSizeCounter )
            { 
            // Just hide it
            naviIndi->SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength), 
                    EAknIndicatorStateOff);
            }
        else
            {
            naviIndi->SetIndicatorValueL(TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength), sizeString);
            naviIndi->SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength), 
                        EAknIndicatorStateOn);
            }
        }
    
    UNILOGGER_WRITE( "<- CUniEditorAppUi::MsgLengthToNavipaneL" );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::CalculateSMSMsgLen
// ---------------------------------------------------------
//
void CUniEditorAppUi::CalculateSMSMsgLen(TInt& charsLeft, TInt& msgsParts)
    {
    CUniEditorDocument* doc = Document();
    if ( doc->UniState() == EUniSms ) 
        {
        CUniDataModel* model = doc->DataModel();
        TInt lengthOne = 0;
        TInt lengthMany = 0;
        
        doc->SmsCharacterLimits( lengthOne, lengthMany );
        
        TInt totalLen = iSmsBodyLength + iSmsSubjectLength;

        if ( doc->EmailOverSmsSupported()  )
            { 
            // Email over Sms so get the longest email address from To field
            TInt emailLen = iHeader->LongestEmailAddress();
            if ( emailLen > 0 )
                { 
                // increase by one so that the separator sign is included too
                emailLen++;
                }
                
            totalLen += emailLen;
            }
        
        TInt maxSmsCharacters = doc->AbsoluteMaxSmsCharacters();
        TInt maxSmsParts = doc->AbsoluteMaxSmsParts();
        
        if ( maxSmsCharacters <= 0 )
            {                    
             // maxSmsCharacters is set to minimum lengthOne.            
             if( maxSmsParts > 1 )
             	{
                maxSmsCharacters = maxSmsParts * lengthMany;
                }
             else
                {
                maxSmsCharacters = lengthOne;
                }
            }
            
        TInt thisPartChars = 0;
        TInt maxThisPartChars = 0;
   
        if( iNLTFeatureSupport )
            {
            //Turkish SMS-PREQ2265 specific
            msgsParts = iNumOfPDUs;
            maxThisPartChars = iCharsLeft;
            thisPartChars = totalLen;

            if(msgsParts > 1)
                {
                // More characters than one SMS segment.                    
                // Calculate characters that are located on this part.
                thisPartChars = lengthMany - maxThisPartChars;
                }                
            }
        else
            {
            if ( totalLen <= lengthOne )
                {
                // Less or equal characters than one SMS segment.
                
                // All the characters are located in this part
                thisPartChars = totalLen;
                
                maxThisPartChars = lengthOne - thisPartChars;
                msgsParts = 1;
                }
            else
                {
                // More characters than one SMS segment.
                
                // Calculate characters that are located on
                // this part.
                thisPartChars = totalLen % lengthMany;
                
                if ( thisPartChars == 0 )
                    {
                    // Last part has maximum characters.
                    thisPartChars = lengthMany;
                    }
                    
                maxThisPartChars = lengthMany - thisPartChars;
                
                msgsParts = totalLen / lengthMany;
                if ( thisPartChars != lengthMany )
                    { 
                    // Chars in this part so lets add one part.
                    msgsParts++;
                    }
                }                
            }
             
        if ( totalLen <= maxSmsCharacters )
            { 
            // Calculate the characters left in this part.
            charsLeft = maxSmsCharacters -  ( msgsParts - 1 ) * lengthMany - thisPartChars;
            // Bound check.
            charsLeft = Min( charsLeft, maxThisPartChars );
            }
        else
            { 
            // More than max -> show -xx(yy)
            charsLeft = maxSmsCharacters - totalLen;
            }
        if( model->ObjectList().Count() == 0 &&
            model->AttachmentList().Count() == 1 &&
            iSmilModel->SlideCount() == 1 &&
            iSmsSubjectLength == 0)
            {                     
            charsLeft =  msgsParts;                        
            }      
        }
    }
    

// ---------------------------------------------------------
// CUniEditorAppUi::HandleCommandL
// ---------------------------------------------------------
//
void CUniEditorAppUi::HandleCommandL( TInt aCommand )
    {
    if ( iFixedToolbar )
        {
        SetFixedToolbarDimmed();
        }
     if ( !IsLaunched() || ( iEditorFlags & ( EEditorClosing | EEditorExiting ) ) )
        {
        if ( aCommand == EEikCmdExit )
            {           
            TRAPD( error, DoHandleCommandL( EEikCmdExit ) );
            User::LeaveIfError(error);                     
            }
        }   
     else
        {                
        TRAPD( error, DoHandleCommandL( aCommand ) );
        
        if ( iFixedToolbar )
            {
            UpdateToolbarL();
            }
        
        User::LeaveIfError( error );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoHandleCommandL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoHandleCommandL( TInt aCommand )
    {
    if ( !IsLaunched() || ( iEditorFlags & ( EEditorClosing | EEditorExiting | EMsgEditInProgress ) ) )
        {
        if ( aCommand != EEikCmdExit )
            {
            // Do not handle other than exit command if application has not
            // finished launching.
            return;
            }
        }

    switch (aCommand)
        {
        // Main options menu commands
        case EUniCmdViewImage:
        case EUniCmdPlayAudio:
        case EUniCmdPlayVideo:
        case EUniCmdPlaySvg:
        case EUniCmdPlayPres:
            {
            PlayFocusedItemL();
            break;
            }
        case EUniCmdRemovePres:
            {
            RemoveTemplateL();
            break;
            }
        case EUniCmdSendSMS:
        case EUniCmdSendMMS:
            {
            if ( iHeader->HasRecipients() > 0 )
                {
                // has addresses -> Send
                DoUserSendL();
                }
            else
                {
                DoUserAddRecipientL();
                }
            break;
            }
        case EUniCmdAddRecipient:
            {
            DoUserAddRecipientL();
            break;
            }
        case EUniCmdAddRecipientMSK:
            {
            // Call this function so that check name functionality is called first
            DoSelectionKeyL();
            break;
            }
        case EUniCmdInsertMedia:
            {
            DoUserInsertMediaL();
            break;
            }
        case EUniCmdInsertTemplateText:
            {
            DoUserInsertTextL( ETemplateText );
            break;
            }
        case EUniCmdInsertNote:
            {
            DoUserInsertTextL( EMemoText );
            break;
            }
        case EUniCmdPlaceTextFirst:
            {
            DoUserChangeOrderL( EUniTextFirst );
            break;
            }
        case EUniCmdPlaceTextSecond:
            {
            DoUserChangeOrderL( EUniImageFirst );
            break;
            }
        case EUniCmdMovePage:
            {
            DoUserMoveOrSelectPageL( ETrue );
            break;
            }
        case EUniCmdSelectPage:
            {
            DoUserMoveOrSelectPageL( EFalse );
            break;
            }
        case EUniCmdAddHeaders:
            {
            DoUserAddHeadersL();
            break;
            }
        case EUniCmdObjects:
            {
            DoUserObjectsViewL( EFalse );
            break;
            }
        case EUniCmdObjectsAttachment:
            {
            DoUserObjectsViewL( ETrue );
            break;
            }
        case EUniCmdSendingOptions:
            {
            DoUserSendingOptionsL();
            break;
            }
        case EUniCmdMessageInfo:
            {
            DoUserMessageInfoL();
            break;
            }
        case EUniCmdRemovePage:
            {
            DoUserRemoveSlideL();
            break;
            }
        case EUniCmdRemoveText:
            {
            RemoveCurrentTextObjectL();
            break;
            }
        case EUniCmdChangePriority:
            {
            DoEditMmsPriorityL();
            break;        
            }
        case EUniCmdRemoveImage:
            {
            DoUserRemoveMediaL( EMsgComponentIdImage, EUniRegionImage );
            break;
            }
        case EUniCmdRemoveAudio:
            {
            DoUserRemoveMediaL( EMsgComponentIdAudio, EUniRegionAudio );
            break;
            }
        case EUniCmdRemoveVideo:
            {
            DoUserRemoveMediaL( EMsgComponentIdVideo, EUniRegionImage );
            break;
            }
        case EUniCmdRemoveSvg:
            {
            DoUserRemoveMediaL( EMsgComponentIdSvg, EUniRegionImage );
            break;
            }
        case EUniCmdRemove:
            {
            CMsgBaseControl* ctrl = iView->FocusedControl();  // ctrl can be NULL.
			if(ctrl)
			{
            TMsgControlId controlId = TMsgControlId(ctrl->ControlId()); 
			if(controlId == EMsgComponentIdAudio)
			{
                DoUserRemoveMediaL( controlId, EUniRegionAudio );
			}
            else if ( controlId == EMsgComponentIdImage ||
			          controlId == EMsgComponentIdVideo ||
                      controlId == EMsgComponentIdSvg )
			{
				if( (Document()->DataModel()->SmilType() == ETemplateSmil) || (Document()->DataModel()->SmilType() == E3GPPSmil))
				   {
				     // focus is on SMIL presentation icon
				      RemoveTemplateL();
				   }
				else
				    {
                		DoUserRemoveMediaL( controlId, EUniRegionImage );
				    }
			}
			} 
            break;
            }
        case EMsgDispSizeAutomatic:
        case EMsgDispSizeLarge:
        case EMsgDispSizeNormal:
        case EMsgDispSizeSmall:
            {
            HandleLocalZoomChangeL( (TMsgCommonCommands)aCommand );
            break;
            }
        case EEikCmdExit:
            {
            RemoveWaitNote();
            ExitAndSaveL();
            break;
            }
        case EAknSoftkeyClose:
            {
            DoBackSaveL();
            break;
            }
        case EAknCmdHelp:
            {
            LaunchHelpL();
            break;
            }
        default:
            {
            CMsgEditorAppUi::HandleCommandL(aCommand);
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::EditorObserver
// ---------------------------------------------------------
//
void CUniEditorAppUi::EditorObserver( TMsgEditorObserverFunc aFunc, 
                                      TAny* aArg1, 
                                      TAny* aArg2, 
                                      TAny* aArg3)
    {
    TRAPD( error, DoEditorObserverL( aFunc, aArg1, aArg2, aArg3 ) );
    if ( error == KLeaveExit )
        {
        User::Leave( KLeaveExit );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoEditorObserverL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoEditorObserverL( TMsgEditorObserverFunc aFunc, 
                                         TAny* aArg1, 
                                         TAny* aArg2, 
                                         TAny* aArg3)
    {
    
    UNILOGGER_WRITE( "-> CUniEditorAppUi::DoEditorObserverL" );
    TInt slides = iSmilModel->SlideCount();
    TInt currSlide = Document()->CurrentSlide();

    switch( aFunc )
        {
        case EMsgScrollParts:
            {
            TInt* parts = static_cast<TInt*>( aArg1 );
            *parts = slides;
            break;
            }
        case MMsgEditorObserver::EMsgHandleFocusChange:
            {
            TMsgFocusEvent* event = static_cast<TMsgFocusEvent*>( aArg1 );
            TMsgAfterFocusEventFunc* after = static_cast<TMsgAfterFocusEventFunc*>( aArg2 );
            TInt* currPart = static_cast<TInt*>( aArg3 );

            *after = EMsgAfterFocusNone;
            UNILOGGER_WRITE( "-> DoEditorObserverL ->MMsgEditorObserver::EMsgHandleFocusChange " );
            
            switch ( *event )
                {
                case EMsgFocusAtBottom:
                    {
                    UNILOGGER_WRITE( "-> DoEditorObserverL ->EMsgFocusAtBottom ");
                    if ( currSlide + 1 < slides )
                        {
#ifdef RD_SCALABLE_UI_V2
                        DoUserChangeSlideL( *currPart );
                        *after = EMsgCursorToBodyBeginning;
                        *currPart = Document()->CurrentSlide();
#else
                        DoUserChangeSlideL( currSlide + 1 );
                        *after = EMsgCursorToBodyBeginning;
                        *currPart = currSlide + 1;
#endif
                        }
                    break;
                    }
                case EMsgFocusAtTop:
                    {
                     UNILOGGER_WRITE( "-> DoEditorObserverL ->EMsgFocusAtTop ");
                    if ( currSlide > 0 )
                        {
#ifdef RD_SCALABLE_UI_V2
                        DoUserChangeSlideL( *currPart );
                        *after = EMsgCursorToBodyEnd;
                        *currPart = Document()->CurrentSlide();
#else
                         DoUserChangeSlideL( currSlide - 1 );
                        *after = EMsgCursorToBodyEnd;
                        *currPart = currSlide - 1;
#endif
                        }
                    break;
                    }
                case EMsgFocusMovingFrom:
                    {
                    UNILOGGER_WRITE( "-> DoEditorObserverL ->EMsgFocusMovingFrom ");
                    TInt id( EMsgComponentIdNull );
                    CMsgBaseControl* ctrl = iView->FocusedControl();  // ctrl can be NULL.
                    if ( !ctrl )
                        {
                        break;
                        }

                    id = ctrl->ControlId();
                    if ( id == EMsgComponentIdAudio ||
                         id == EMsgComponentIdImage ||
                         id == EMsgComponentIdVideo )
                        {
                        break;
                        }

                    // not in first slide -> return
                    if ( currSlide > 0 )
                        {
                        break;
                        }
                        
                    if ( iHeader &&  
                         iHeader->IsAddressControl( id ) )
                        {
                        // We are moving from address control -> set max edwin sizes..
                        SetOrRemoveMaxSizeInEdwin();
                        
                        CMsgAddressControl* rec = iView ? 
                            static_cast<CMsgAddressControl*>( iView->ControlById( id ) ) :
                            NULL;

                        if ( rec ) 
                            {
                            TInt inputMode = rec->Editor().AknEditorCurrentInputMode();
                            SetInputModeToAddressFields( inputMode );
                            }
                        }
                    break;
                    }
                case EMsgFocusMovedTo:
                    {
                    UNILOGGER_WRITE( "-> DoEditorObserverL ->EMsgFocusMovedTo ");
                    TInt id( EMsgComponentIdNull );
                    CMsgBaseControl* ctrl = iView->FocusedControl();  // ctrl can be NULL.
                    if ( !ctrl )
                        {
                        break;
                        }
                    
                    id = ctrl->ControlId();
                    if ( id == EMsgComponentIdAudio ||
                         id == EMsgComponentIdImage ||
                         id == EMsgComponentIdVideo ||
                         id == EMsgComponentIdAttachment ||
                         id == EMsgComponentIdSvg )
                        {
                        iNaviPane->PushL( *iNaviDecorator );
                        }
                        
                    UpdateMiddleSoftkeyL();
                    break;
                    }
                case EMsgFocusToBody:
                case EMsgFocusToHeader:
                    {
                    UNILOGGER_WRITE( "-> DoEditorObserverL ->EMsgFocusToBody or EMsgFocusToHeader  ");
                    
                    SmsMsgLenToPenInputL();
                    UpdateMiddleSoftkeyL(); // fallthrough
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            break;
            }
        case MMsgEditorObserver::EMsgControlPointerEvent:
            {
            TPointerEvent* event = static_cast<TPointerEvent*>( aArg2 );
            CMsgBaseControl* control = static_cast<CMsgBaseControl*>( aArg1 );
            TWsEvent* wsEvent = static_cast<TWsEvent*>(aArg2);
            if ( event->iType == TPointerEvent::EButton1Down )
                {
                iFocusedControl = control;
                if ( control && 
                        ( control->ControlId() == EMsgComponentIdAudio ||
                                control->ControlId() == EMsgComponentIdImage ||
                                control->ControlId() == EMsgComponentIdVideo ||
                                control->ControlId() == EMsgComponentIdAttachment ||
                                control->ControlId() == EMsgComponentIdSvg ) )
                    {
                    if(iLongTapDetector)
                        {
                        iLongTapDetector->EnableLongTapAnimation(ETrue);
                        iLongTapDetector->PointerEventL( *event );
                        }
                    }
                iTapConsumed = EFalse;
                }
            else if ( (!iTapConsumed) && (event->iType == TPointerEvent::EButton1Up) )
                {
                iLongTapDetector->MonitorWsMessage(*wsEvent);
                iTapConsumed = ETrue;
                if ( control && 
                     iFocusedControl == control &&
                     ( control->ControlId() == EMsgComponentIdAudio ||
                       control->ControlId() == EMsgComponentIdImage ||
                       control->ControlId() == EMsgComponentIdVideo ||
                       control->ControlId() == EMsgComponentIdAttachment ||
                       control->ControlId() == EMsgComponentIdSvg ) )
                    {
                    DoSelectionKeyL();
                    }
                }
            break;
            }
        case MMsgEditorObserver::EMsgButtonEvent:
            {
            CMsgBaseControl* control = static_cast<CMsgBaseControl*>( aArg1 );
            
            if ( control &&
                 ( control->ControlId() != EMsgComponentIdAttachment ||
                   iFocusedControl != control ) )
                {
                DoSelectionKeyL();
                }
            break;
            }
        default:
            {
            // Other events not handled.
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::FetchFileL
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::FetchFileL( MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType )
    {
    // do not start if()ing. Sets parameter always
    TAiwVariant sizeLimit( Document()->MaxMessageSize() 
                            - ( MessageSizeInBytes() + KUniAttachmentOverhead ) );
    TAiwGenericParam sizeLimitParam( EGenericParamMMSSizeLimit, sizeLimit );

    CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
    paramList->AppendL( sizeLimitParam );
    
    CDesCArrayFlat* files = new( ELeave ) CDesC16ArrayFlat( 1 );
    CleanupStack::PushL( files );
    
    TBool fetchOK = MsgAttachmentUtils::FetchFileL( aFetchType,
                                                    *files,
                                                    paramList,
                                                    EFalse, // disk space checked in VerifySelectionL or CUniEditorInsertOperation, if image scaled/compressed
                                                    EFalse,
                                                    this );

    switch ( aFetchType )
        {
        case MsgAttachmentUtils::ENewImage:
        case MsgAttachmentUtils::ENewAudio:
        case MsgAttachmentUtils::ENewVideo:
        case MsgAttachmentUtils::ENote:
            {
            if ( fetchOK )
                {
                TFileName* fileName = new( ELeave ) TFileName;
                CleanupStack::PushL( fileName );
    
                if ( files->MdcaCount() > 0 )
                    {
                    *fileName = files->MdcaPoint( 0 );
                    }
                    
                // Something was fetched
                __ASSERT_DEBUG( !iInsertingMedia, Panic( EUniIllegalMediaObject ) );
                fetchOK = CreateMediaInfoForInsertL( iInsertingType, *fileName ) ? ETrue : 
                                                                                   EFalse;
                CleanupStack::PopAndDestroy( fileName );
                }
            break;
            }
        case MsgAttachmentUtils::EImage:
        case MsgAttachmentUtils::EAudio:
        case MsgAttachmentUtils::EVideo:
        case MsgAttachmentUtils::ESVG:
        default:
            {
            // CreateMediaInfoForInsertL is called in VeritySelectionL
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( 2, paramList );    // + files
    
    return fetchOK;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::VerifySelectionL
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::VerifySelectionL( const MDesCArray* aSelectedFiles )
    {
    CAknInputBlock::NewLC();

    TBool ret = EFalse;
    if ( aSelectedFiles->MdcaCount() == 1 )
        {
        __ASSERT_DEBUG( !iInsertingMedia, Panic( EUniIllegalMediaObject ) );
        if ( CreateMediaInfoForInsertL( iInsertingType, aSelectedFiles->MdcaPoint( 0 ) ) )
            {
            ret = ETrue;
            }
        }
    
    CleanupStack::PopAndDestroy(); // CAknInputBlock
    return ret;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DynInitToolbarL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar )
    {
    if ( aResourceId == EUniCmdFixedToolbarInsert )
        {
        CAknToolbarExtension* extension = aToolbar->ToolbarExtension();
        
        if ( iSmilModel )
            {
            if ( iSmilModel->SlideCount() >= iSmilModel->MaxSlideCount() )
                {
                if ( ObjectsAvailable() & ( EUniImageFlag |  EUniSvgFlag | EUniVideoFlag ) )
                    {
                    SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtInsertImage, ETrue );
                    SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtNewImage, ETrue );
                    }
               else
                    {
                    SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtInsertImage, EFalse );
                    SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtNewImage, EFalse );
                    }
                    
                if ( ObjectsAvailable() & ( EUniImageFlag |  EUniSvgFlag | EUniVideoFlag | EUniAudioFlag ) )
                    {
                    SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtInsertVideo, ETrue );
                    SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtNewVideo, ETrue );
                    }
                else
                    {
                    SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtInsertVideo, EFalse );
                    SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtNewVideo, EFalse );
                    }
                
                if ( ObjectsAvailable() & ( EUniVideoFlag | EUniAudioFlag | EUniSvgFlag ) )
                    {
                    SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtInsertAudio, ETrue );
                    SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtNewAudio, ETrue );
                    }
                else 
                    {
                    SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtInsertAudio, EFalse );
                    SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtNewAudio, EFalse );
                    }
                
                SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtInsertSlide, ETrue );
                }
            else 
                {
                SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtInsertImage, EFalse );
                SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtNewImage, EFalse );
                SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtInsertVideo, EFalse );
                SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtNewVideo, EFalse );
                SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtInsertAudio, EFalse );
                SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtNewAudio, EFalse );
                SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtInsertSlide, EFalse );                
                }
            }
        
        if ( !( iSupportedFeatures & EUniFeatureCamcorder ) )
            {
            SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtNewImage, ETrue );
            SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtNewVideo, ETrue );
            }

        if ( Document()->CreationMode() == EMmsCreationModeRestricted )
             {
             SetExtensionButtonDimmed( extension, EUniCmdFixedToolbarExtNewSmil, ETrue );             
             }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::OfferToolbarEventL
// ---------------------------------------------------------
//        
void CUniEditorAppUi::OfferToolbarEventL( TInt aCommand )
    {
    if ( iFixedToolbar )
        {
        iFixedToolbar->ToolbarExtension()->SetShown( EFalse );
        }
            
    TRAPD( error, DoOfferToolbarEventL( aCommand ) );
            
    User::LeaveIfError( error );
    }

        
// ---------------------------------------------------------
// CUniEditorAppUi::DoOfferToolbarEventL
// ---------------------------------------------------------
//        
void CUniEditorAppUi::DoOfferToolbarEventL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EUniCmdFixedToolbarSend:
            {
            DoHandleCommandL( EUniCmdSendSMS );
            break;
            }
        case EUniCmdFixedToolbarInsert:
            {
            DoHandleCommandL( EUniCmdInsertMedia );
            break;
            }
        case EUniCmdFixedToolbarAddRecipient:
            {
            DoHandleCommandL( EUniCmdAddRecipient );
            break;
            }
        case EUniCmdFixedToolbarExtInsertImage:
            {
            DoUserInsertImageL( MsgAttachmentUtils::EImage );
            break;
            }
        case EUniCmdFixedToolbarExtInsertVideo:
            {
            DoUserInsertVideoL( MsgAttachmentUtils::EVideo );
            break;
            }
        case EUniCmdFixedToolbarExtInsertAudio:
            {
            DoUserInsertAudioL( MsgAttachmentUtils::EAudio );
            break;
            }
        case EUniCmdFixedToolbarExtNewImage:
            {
            DoUserInsertImageL( MsgAttachmentUtils::ENewImage );
            break;
            }
        case EUniCmdFixedToolbarExtNewVideo:
            {
            DoUserInsertVideoL( MsgAttachmentUtils::ENewVideo );
            break;
            }
        case EUniCmdFixedToolbarExtNewAudio:
            {
            DoUserInsertAudioL( MsgAttachmentUtils::ENewAudio );
            break;
            }
        case EUniCmdFixedToolbarExtInsertSlide:
            {
            DoUserInsertSlideL();
            break;
            }
        case EUniCmdFixedToolbarExtInsertTemplate:
            {
            DoUserInsertTextL( ETemplateText );
            break;
            }
        case EUniCmdFixedToolbarExtInsertNote:
            {
            DoUserInsertTextL( EMemoText );
            break;
            }
        case EUniCmdFixedToolbarExtInsertvCard:
            {
            DoUserInsertVCardL();
            break;
            }
        case EUniCmdFixedToolbarExtOther:
            {
            DoToolbarInsertOtherL();
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::HandleServerAppExit
// ---------------------------------------------------------
// 
void CUniEditorAppUi::HandleServerAppExit( TInt /*aReason*/ )
    {
    CMsgBaseControl* ctrl = iView->FocusedControl();

    if ( ctrl )
        {
        TUniRegion region = EUniRegionImage;
        
        switch ( ctrl->ControlId() )
            {
            case EMsgComponentIdAudio:
                {
                region = EUniRegionAudio;
                // Fallthrough.
                }
            case EMsgComponentIdVideo:
                {
                CUniObject* object = iSmilModel->GetObject( Document()->CurrentSlide(), region );
                
                // Coverty fix, Null pointer return, http://ousrv057/cov.cgi?cid=37172
                if ( object )
                	{
                    if ( object->DrmInfo() )
                       {
                       object->DrmInfo()->FreezeRights();
                       }
                    }
                
                TRAP_IGNORE( iSlideLoader->UpdateControlIconL( 
                                                  *static_cast<CMsgMediaControl*>( ctrl ), 
                                                  object ) );
                ctrl->DrawNow();
                break;
                }
            default:
                {
                break;
                }
            }
        }
    iEditorFlags &= ~EMsgEditInProgress;
    DeactivateInputBlocker();
    }

// ---------------------------------------------------------
// CUniEditorAppUi::HandleEdwinEventL
//
// Performs character counter updating if neccessary. This is needed
// for supporting on-screen keyboard. Normally character input
// is handled already on OfferKeyEventL function but it does no
// harm to do it here again. Character input is not handled if editor
// is not fully launched or it is about to close. Modified flag is turned
// on here since control state change event that should turn it on might 
// come after this event and HandleCharInputL will not work correctly if 
// modified flag is not set. Text update event is expected to come only
// in such cases that control is modified.
// ---------------------------------------------------------
//
void CUniEditorAppUi::HandleEdwinEventL( CEikEdwin* /*aEdwin*/, TEdwinEvent aEventType )
    {
    if ( aEventType == MEikEdwinObserver::EEventTextUpdate &&
         !( iEditorFlags & EEditorClosing ||
            iEditorFlags & EEditorExiting ) &&
         IsLaunched() )
        {
        CMsgBaseControl* focusedControl = iView->FocusedControl();
        if ( focusedControl )
            {
            focusedControl->SetModified( ETrue );
            
            HandleCharInputL();            
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::HandleEntryMovedL
// ---------------------------------------------------------
//
void CUniEditorAppUi::HandleEntryMovedL( TMsvId /*aOldParent*/, TMsvId /*aNewParent*/ )
    {
    }

// ---------------------------------------------------------
// CUniEditorAppUi::HandleKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CUniEditorAppUi::HandleKeyEventL( const TKeyEvent& aKeyEvent, 
                                               TEventCode aType )
    {
    if ( !IsLaunched() )
        {
        // Still launching
        return EKeyWasNotConsumed;
        }
        
    if ( !IsDisplayingMenuOrDialog() &&
         !( iEditorFlags & EEditorClosing ||
            iEditorFlags & EEditorExiting ) )
        {
        switch ( aKeyEvent.iCode )
            {
            case EKeyEnter:
                {
                if ( DoEnterKeyL() )
                    {
                    return EKeyWasConsumed;
                    }
                break;
                }
            case EKeyOK:
                {
                // Selection key: Show context menus
                DoSelectionKeyL();
                return EKeyWasConsumed;
                }
            case EKeyYes:
                {
                // CallCreationKey: Send message if recipients.
                //                  Otherwise fetch recipients
                if ( iHeader->HasRecipients() )
                    {
                    // has addresses -> Send
                    DoUserSendL();
                    }
                else
                    {
                    DoUserAddRecipientL();
                    }
                return EKeyWasConsumed;
                }
            case EKeyBackspace:
            case EKeyDelete:
                {
                CMsgBaseControl* ctrl = iView->FocusedControl();
                if ( ctrl )
                    {
                    switch( ctrl->ControlId() )
                        {
                        case EMsgComponentIdImage:
                            {
                            switch ( Document()->DataModel()->SmilType() )
                                {
                                case ETemplateSmil:
                                case E3GPPSmil:
                                    {
                                    // focus is on SMIL presentation icon
                                    RemoveTemplateL();
                                    break;
                                    }
                                default:
                                    {
                                    // real image
                                    DoUserRemoveMediaL( EMsgComponentIdImage, EUniRegionImage );
                                    break;
                                    }
                                }
                                
                            return EKeyWasConsumed;
                            }
                        case EMsgComponentIdAudio:
                            {
                            DoUserRemoveMediaL( EMsgComponentIdAudio, EUniRegionAudio );
                            return EKeyWasConsumed;
                            }
                        case EMsgComponentIdVideo:
                            {
                            DoUserRemoveMediaL( EMsgComponentIdVideo, EUniRegionImage );
                            return EKeyWasConsumed;
                            }
                        case EMsgComponentIdSvg:
                            {
                            DoUserRemoveMediaL( EMsgComponentIdSvg, EUniRegionImage );
                            return EKeyWasConsumed;
                            }
                        default:
                            {
                            break;
                            }
                        }
                    }
                break;
                }
            default:
                {
                break;
                }
            }
            
        switch ( aKeyEvent.iScanCode )
            {
            case EStdKeyUpArrow:
            case EStdKeyDownArrow:
            case EStdKeyLeftArrow:
            case EStdKeyRightArrow:
            case EStdKeyDevice1: // Close key                
            case EStdKeyRightShift: // Shift
            case EStdKeyApplication0: // Task switching
                {
                return CMsgEditorAppUi::HandleKeyEventL( aKeyEvent, aType );
                }
            default:
                {
                break;
                }
            }

        TKeyResponse resp = CMsgEditorAppUi::HandleKeyEventL( aKeyEvent, aType );

        if ( aType == EEventKeyUp )
            {
            // Check if character inserted to controls
            HandleCharInputL();
            }

        return resp;
        }
    else
        {
        return CMsgEditorAppUi::HandleKeyEventL(aKeyEvent, aType);
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::HandleCharInputL
// Updates model
// ---------------------------------------------------------
//
void CUniEditorAppUi::HandleCharInputL()
    {
    if ( IsLaunched() )
        {
        CMsgBaseControl* ctrl = iView->FocusedControl();
        TBool changed = EFalse;

        if ( ctrl )
            {
            switch ( ctrl->ControlId() )
                {
                case EMsgComponentIdTo:
                case EMsgComponentIdCc:
                case EMsgComponentIdBcc:
                    {
                    if ( ctrl->IsModified() )
                        {
                        Document()->SetHeaderModified( ETrue );
                        CheckHeaderForMessageTypeL();
                        changed = ETrue;
                        }
                    SetAddressSize();
                    break;
                    }
                case EMsgComponentIdSubject:
                    {
                    if ( ctrl->IsModified() )
                        {
                        Document()->SetHeaderModified( ETrue );
                        changed = ETrue;
                        }
                        
                    SetSubjectSize();
                    CheckBodyForMessageTypeL();
                    CheckMaxSmsSizeAndShowNoteL();
                    SetOrRemoveMaxSizeInEdwin();
                    break;
                    }
                case EMsgComponentIdBody:
                    {
                    if ( ctrl->IsModified() )
                        {
                        UpdateSmilTextAttaL();
                        Document()->SetBodyModified( ETrue );
                        CheckBodyForMessageTypeL();
                        CheckMaxSmsSizeAndShowNoteL();
                        SetOrRemoveMaxSizeInEdwin();
                        changed = ETrue;
                        }
                    break;
                    }
                default:
                    {
                    // Text control not focused.
                    break;
                    }
                }
            }
            
        if ( changed )
            {
            MsgLengthToNavipaneL();
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::HandleForegroundEventL
// 
// Updates navipane at editor start when launched from 
// ---------------------------------------------------------
//
void CUniEditorAppUi::HandleForegroundEventL( TBool aForeground )
    {
    UNILOGGER_WRITE( "-> CUniEditorAppUi::HandleForegroundEventL" );
    
    CAknAppUi::HandleForegroundEventL( aForeground );
    
    if ( IsLaunched() &&
         aForeground && 
         !IsDisplayingDialog() &&
         !( iEditorFlags & ( EEditorExiting | EEditorClosing ) ) )
        {
        // This must be init.. not MsgLen... Otherwise returning
        // from embedded app loses msglength
        InitNaviPaneL();
        }
    
    UNILOGGER_WRITE( "-> CUniEditorAppUi::HandleForegroundEventL" );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::UpdateSmilTextAttaL
// 
// NOTE: This function should only be called when text is 
//       added/removed to/from bodycontrol.
// ---------------------------------------------------------
//
void CUniEditorAppUi::UpdateSmilTextAttaL()
    {
    __ASSERT_DEBUG( Document()->DataModel()->SmilType() == EMmsSmil, Panic( EUniIllegalSmilType ) );
    
    TInt slideNum = Document()->CurrentSlide();
    if ( slideNum < 0 || slideNum >= iSmilModel->SlideCount() )
        {
        return;
        }
    
    // Update smilmodel
    if ( !BodyCtrl() || 
         BodyCtrl()->TextContent().DocumentLength() == 0 )
        {
        // Body control does not exist or it is empty
        CUniObject* obj = iSmilModel->GetObject( slideNum, EUniRegionText );
        if ( obj )
            {
            // If there is text atta in presentation it should be removed.
            // Note: Text atta is not removed from store at this point (removed in save)
            iSmilModel->RemoveObjectL( slideNum, obj );
            }
        }
    else
        {
        // Text added -> see if model already has text atta
        if ( !iSmilModel->GetObject( slideNum, EUniRegionText ) )
            {
            if ( !iSmilModel->IsSlide( slideNum ) )
                {
                // Add slide
                iSmilModel->AddSlideL( slideNum );
                }
            iSmilModel->AddTextObjectL( slideNum,
                                        &( BodyCtrl()->Editor() ) );
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    // Confirm app is running properly (needed atleast now)
    if ( !IsLaunched() )
        {
        // If not, hide everything and return
        TInt amountOfItems = aMenuPane->NumberOfItemsInPane();
        if ( amountOfItems )
            {
            aMenuPane->DeleteBetweenMenuItems( 0, amountOfItems-1 );
            }
        return;
        }

    TUint32 objects = ObjectsAvailable();

    switch ( aResourceId ) 
        {
        case R_UNIEDITOR_MAIN_MENU:
            {
            // Features always visible:
            // EEikCmdExit
            // EUniCmdSendingOptions
            // EUniCmdMessageInfo

            // Features checked commonly
            if ( !( iSupportedFeatures & EUniFeatureHelp ) )
                { 
                // Help
                aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
                }
                
            // Features in locked SMS mode
            if ( IsHardcodedSms() )
                { 
                // Dim these items always in locked SMS mode

                aMenuPane->SetItemDimmed( EUniCmdPlayPreview, ETrue );
                aMenuPane->SetItemDimmed( EUniCmdSendMMS, ETrue );
                aMenuPane->SetItemDimmed( EUniCmdInsertMedia, ETrue );
                aMenuPane->SetItemDimmed( EUniCmdPlaceTextFirst, ETrue );
                aMenuPane->SetItemDimmed( EUniCmdPlaceTextSecond, ETrue );
                aMenuPane->SetItemDimmed( EUniCmdMovePage, ETrue );
                aMenuPane->SetItemDimmed( EUniCmdSelectPage, ETrue );
                aMenuPane->SetItemDimmed( EUniCmdChangePriority, ETrue );
                
                if ( Document()->DataModel()->ObjectList().Count() == 0 &&
                     Document()->DataModel()->AttachmentList().Count() == 0 )
                    {   
                    // If no objects -> hide the option
                    aMenuPane->SetItemDimmed( EUniCmdObjects, ETrue );
                    }
                
                if ( !Document()->EmailOverSmsSupported() ||
                     ( iHeader &&  
                       !( iHeader->AddHeadersVariation() & EUniFeatureSubjectConfigurable ) ) )
                    { 
                    // Locked SMS and no email over sms or email over SMS without selectable subject.
                    aMenuPane->SetItemDimmed( EUniCmdAddHeaders, ETrue );
                    }
                
                if ( !BodyCtrl() )
                    {
                    aMenuPane->SetItemDimmed( EUniCmdRemoveSubmenu, ETrue );                    
                    aMenuPane->SetItemDimmed( EUniCmdInsertSubmenu, ETrue );
                    }
                else if ( BodyCtrlEditor()->TextLength() == 0 )
                    {
                    aMenuPane->SetItemDimmed( EUniCmdRemoveSubmenu, ETrue );                    
                    }
                }
            else if ( TUniMsvEntry::IsMmsUpload( Document()->Entry() ) )
                {
                // MMS upload message
                aMenuPane->SetItemDimmed( EUniCmdInsertSubmenu, ETrue );
                aMenuPane->SetItemDimmed( EUniCmdAddHeaders, ETrue );
                aMenuPane->SetItemDimmed( EUniCmdAddRecipient, ETrue );                                    
                aMenuPane->SetItemDimmed( EUniCmdChangePriority, ETrue );
                
                if ( Document()->DataModel()->ObjectList().Count() == 0 &&
                     Document()->DataModel()->AttachmentList().Count() == 0 )
                    {   
                    // If no objects -> hide the option
                    aMenuPane->SetItemDimmed( EUniCmdObjects, ETrue );
                    }
                    
                DynInitFocusedMediaBasedOptionsL( aMenuPane );
                }
            else 
                {
                // It's automatic mode
                aMenuPane->SetItemDimmed( EUniCmdInsertSubmenu, ETrue );
                
                if ( Document()->DataModel()->ObjectList().Count() == 0 &&
                     Document()->DataModel()->AttachmentList().Count() == 0 )
                    {   // If no objects -> hide the option
                    aMenuPane->SetItemDimmed( EUniCmdObjects, ETrue );
                    }
                    
                if ( Document()->UniState() == EUniSms )
                    {
                    // It's currently a SMS message

                    aMenuPane->SetItemDimmed( EUniCmdPlayPreview, ETrue );
                    aMenuPane->SetItemDimmed( EUniCmdSendMMS, ETrue );
                    aMenuPane->SetItemDimmed( EUniCmdPlaceTextFirst, ETrue );
                    aMenuPane->SetItemDimmed( EUniCmdPlaceTextSecond, ETrue );
                    aMenuPane->SetItemDimmed( EUniCmdPlayPreview, ETrue );                    
                    aMenuPane->SetItemDimmed( EUniCmdMovePage, ETrue );  
                    aMenuPane->SetItemDimmed( EUniCmdSelectPage, ETrue );                    
                    aMenuPane->SetItemDimmed( EUniCmdChangePriority, ETrue );
                    if ( !BodyCtrlEditor() || BodyCtrlEditor()->TextLength() == 0 )
                        {
                        aMenuPane->SetItemDimmed( EUniCmdRemoveSubmenu, ETrue );                    
                        }
                    }
                else
                    { 
                    // It's currently a MMS message
					/*
                     * Do not show "Sending Options" menu item if:
                     * 1. It is a MMS and
                     * 2. The feature flag MpMessagingHideMessageTypeId is set
                     * This is because when we are composing a MMS, 
                     * "Character Support" option is not displayed in "Sending Options"
                     * and because of this flag, we are not going to show "Message Type"
                     * option as well - resulting in an empty "Sendings Options" dialog.
                     * Therefore, it was decided to hide this menu option when
                     * composing a MMS (if flag is defined)
                     */
                    if ( iEditorFlags & EHideMessageTypeOption ||
                         iMtm->MessageTypeLocking() == EUniMessageTypeLocked )
                        {
                        // Also, if message is permanently locked ( to mms ) -> no sending options
                        aMenuPane->SetItemDimmed( EUniCmdSendingOptions, ETrue );
                        }
                        
                    if ( !(iSupportedFeatures & EUniFeaturePriority) )
                        {
                        aMenuPane->SetItemDimmed( EUniCmdChangePriority, ETrue );
                        }
                        
                    // Always visible in MMS message:
                    // EUniCmdAddRecipient
                    if ( iHeader &&  
                         !iHeader->AddHeadersVariation() )
                        {
                        aMenuPane->SetItemDimmed( EUniCmdAddHeaders, ETrue );
                        }
                        
                    DynInitFocusedMediaBasedOptionsL( aMenuPane );
                    }
                }
            break;
            }
        case R_UNIEDITOR_REMOVE_SUBMENU:
            {
            // Remove submenu is visible only for MMS SMIL messages
            if ( !( objects & EUniImageFlag &&
                    Document()->DataModel()->SmilType() == EMmsSmil ) )
                {
                aMenuPane->SetItemDimmed( EUniCmdRemoveImage, ETrue );
                }
            if ( !( objects & EUniAudioFlag ) ) 
                {
                aMenuPane->SetItemDimmed( EUniCmdRemoveAudio, ETrue );
                }
            if ( !( objects & EUniVideoFlag ) ) 
                {
                aMenuPane->SetItemDimmed( EUniCmdRemoveVideo, ETrue );
                }
            if ( !( objects & EUniTextFlag ) ) 
                {
                aMenuPane->SetItemDimmed( EUniCmdRemoveText, ETrue );
                }
            if ( !( objects & EUniSvgFlag ) ) 
                {
                aMenuPane->SetItemDimmed( EUniCmdRemoveSvg, ETrue );
                }
                
            if ( Document()->DataModel()->SmilType() != ETemplateSmil &&
                 Document()->DataModel()->SmilType() != E3GPPSmil )
                {
                aMenuPane->SetItemDimmed( EUniCmdRemovePres, ETrue );
                }
                
            if ( iSmilModel->SlideCount() <= 1 )
                {
                aMenuPane->SetItemDimmed( EUniCmdRemovePage, ETrue );
                }
                
            break;
            }
        case R_UNIEDITOR_CONTEXT_MENU:
            {
            if ( Document()->UniState() == EUniSms )
                {
                aMenuPane->SetItemDimmed( EUniCmdSendMMS, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EUniCmdSendSMS, ETrue );
                }
            
            if ( IsHardcodedSms() )
                {
                aMenuPane->SetItemDimmed( EUniCmdInsertMedia, ETrue );
                }
            
            if ( !( iSupportedFeatures & EUniFeatureJapanese ) ||
                 iSmilModel->SlideCount() <= 1 )
                {
                // Only shown on Japanese variant.
                aMenuPane->SetItemDimmed( EUniCmdSelectPage, ETrue );
                }
            
            break;
            }
        case R_UNIEDITOR_ZOOM_SUBMENU:
            {
            TInt zoomLevel = KErrGeneral;
            iMsgEditorAppUiExtension->iSettingCachePlugin.GetValue( KAknLocalZoomLayoutSwitch, 
                                                                    zoomLevel );
            switch ( zoomLevel )
                {
                case EAknUiZoomAutomatic:
                    {
                    aMenuPane->SetItemButtonState( EMsgDispSizeAutomatic,
                                                   EEikMenuItemSymbolOn );
                    break;
                    }
                case EAknUiZoomNormal:
                    {
                    aMenuPane->SetItemButtonState( EMsgDispSizeNormal,
                                                   EEikMenuItemSymbolOn );
                    break;
                    }
                case EAknUiZoomSmall:
                    {
                    aMenuPane->SetItemButtonState( EMsgDispSizeSmall,
                                                   EEikMenuItemSymbolOn );
                    break;
                    }
                case EAknUiZoomLarge:
                    {
                    aMenuPane->SetItemButtonState( EMsgDispSizeLarge,
                                                   EEikMenuItemSymbolOn );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DynInitFocusedMediaBasedOptionsL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DynInitFocusedMediaBasedOptionsL( CEikMenuPane* aMenuPane )
    {
    // Lets dim all the context sensitive options first

    aMenuPane->SetItemDimmed( EUniCmdPlayPreview, ETrue );                    
    aMenuPane->SetItemDimmed( EUniCmdSendSMS, ETrue );
    
    // Completely dim PlaceTextFirst and PlaceTextSecond from options menu
    aMenuPane->SetItemDimmed( EUniCmdPlaceTextFirst, ETrue );
    aMenuPane->SetItemDimmed( EUniCmdPlaceTextSecond, ETrue );

    if ( Document()->DataModel()->SmilType() != EMmsSmil ) 
        { 
        // It's 3GPP presentation
        aMenuPane->SetItemDimmed( EUniCmdInsertMedia, ETrue );    
        }
    
    
    /* This code can be used if PlaceTextFirst and PlaceTextSecond 
     * functionality is needed.
     if ( Document()->DataModel()->SmilType() != EMmsSmil ) 
        { 
        // It's 3GPP presentation
        aMenuPane->SetItemDimmed( EUniCmdInsertMedia, ETrue );
        aMenuPane->SetItemDimmed( EUniCmdPlaceTextFirst, ETrue );
        aMenuPane->SetItemDimmed( EUniCmdPlaceTextSecond, ETrue );
        }
      
     else if ( iSupportedFeatures & EUniFeatureJapanese )
        {
        // Not shown on japanese variant ever.
        aMenuPane->SetItemDimmed( EUniCmdPlaceTextFirst, ETrue );
        aMenuPane->SetItemDimmed( EUniCmdPlaceTextSecond, ETrue );        
        }
     else
        {
        if ( iSmilModel->Layout() == EUniImageFirst )
            {
            aMenuPane->SetItemDimmed( EUniCmdPlaceTextSecond, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EUniCmdPlaceTextFirst, ETrue );
            }
        } */

    if ( Document()->DataModel()->SmilType() != EMmsSmil || 
         iSmilModel->SlideCount() <= 1 )
        { 
        // No move if not MmsSmil or only one page
        aMenuPane->SetItemDimmed( EUniCmdMovePage, ETrue );
        aMenuPane->SetItemDimmed( EUniCmdSelectPage, ETrue );
        }
    else if ( !( iSupportedFeatures & EUniFeatureJapanese ) )
        {
        // Only shown on Japanese variant.
        aMenuPane->SetItemDimmed( EUniCmdSelectPage, ETrue );
        }
    
    if( Document()->DataModel()->SmilType() == EMmsSmil &&
        Document()->DataModel()->ObjectList().Count() == 0 && 
        iSmilModel->SlideCount() == 1 )
        {
        aMenuPane->SetItemDimmed( EUniCmdRemoveSubmenu, ETrue );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::IsMessageEmpty
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::IsMessageEmpty() const 
    {
    TBool smil = EFalse;
    if ( Document()->DataModel()->SmilType() != EMmsSmil &&
         Document()->DataModel()->SmilList().Count() > 0 )
        {
        smil = ETrue;
        }
    return !smil &&
           IsHeaderEmpty() &&
           IsBodyEmpty() &&
           Document()->DataModel()->AttachmentList().Count() == 0;
    }


// ---------------------------------------------------------
// CUniEditorAppUi::DoUserSendL
//
// Message must be unlocked before coming here.
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserSendL()
    {
    // Force FEP Update done only when SMIL is not 3GPP SMIL
    if ( Document()->DataModel()->SmilType() == EMmsSmil )
        {
        if ( BodyCtrl() )
            {
            BodyCtrl()->SetFocus( EFalse );
            }
            
        iCoeEnv->SyncNotifyFocusObserversOfChangeInFocus();
        }
    
    TUniState currentState = Document()->UniState();
    if(currentState == EUniMms)
        {
        TInt PrevSlidecount = iSmilModel->SlideCount();
        iSmilModel->RemoveEmptySlides();
        if( PrevSlidecount != iSmilModel->SlideCount() )
            {
            Document()->SetBodyModified( ETrue );
            CheckBodyForMessageTypeL();  
            }
        }
    
    TBool modified( EFalse );
    if ( !VerifyAddressesL( modified ) )
        {
        // Invalid recipient found  -> abort sending.
        return;
        }
    
    if ( modified &&
         currentState != Document()->UniState() )
        {
        if ( !ShowConfirmationQueryL( R_UNIEDITOR_MSG_TYPE_CHANGED_WHILE_SENDING ) )
            {
            return;
            }
        }
    
    // Check MMS message size
    if ( TUint( MessageSizeInBytes() ) > Document()->MaxMessageSize() )
        {
        TInt maxInKilos = ( Document()->MaxMessageSize() + KBytesInKilo - 1 ) / KBytesInKilo;
        HBufC* string = StringLoader::LoadLC( R_UNIEDITOR_SEND_BIG_TEXT, 
                                              maxInKilos, 
                                              iCoeEnv );
                                              
        CAknInformationNote* note = new ( ELeave ) CAknInformationNote();
        note->ExecuteLD( *string );
        CleanupStack::PopAndDestroy( string );
        return;
        }

    // Check the max sms size
    CheckMaxSmsSizeAndShowNoteL( ETrue );

    if( iEditorFlags & EMaxSmsSizeNoteShown )
        { 
        // This flag was set by the funtion call above and it means an error note was shown to user
        return;
        }

    if( !CheckMaxRecipientsAndShowNoteL( ETrue ) )
        { 
        // Too many recipients. Info was shown so just return
        return;
        }
    
    CUniEditorPlugin* plugin = NULL;
    TInt resourceId = NULL;
    
    // Get the plugin
    if( Document()->UniState() == EUniSms )
        {
        plugin = SmsPlugin();
        resourceId = R_QTN_MSG_WAIT_SENDING_SMS;
        
        // Ask if it's ok to send the message in N parts
        if( !ConfirmSmsSendInMultiplePartsL() )
            {
            return;
            }
        }
    else
        {
        plugin = MmsPlugin();
        resourceId = R_QTN_MSG_WAIT_SENDING_MMS;
        }

    if( plugin )
        {
        TBool checkEmailSettings = EFalse;
        if( Document()->EmailOverSmsSupported() && iHeader->LongestEmailAddress() )
            {
            checkEmailSettings = ETrue;
            }
        
        if ( iFixedToolbar && !plugin->IsServiceValidL() )
            {
            iFixedToolbar->HideItemsAndDrawOnlyBackground( ETrue );
            }
        
        iEditorFlags |= EToolbarHidden;
        CleanupStack::PushL( TCleanupItem( EnableFixedToolbar, this ) );
        if (!plugin->IsServiceValidL())
        	{
        		SetKeyEventFlags(0);
        	}
            
        CleanupStack::PushL( TCleanupItem( DisableSendKey, this ) );
        
        iMsgLenToVKB = EFalse;
        TBool serviceValid( plugin->ValidateServiceL( checkEmailSettings ) );
        
        CleanupStack::PopAndDestroy(2); //DisableSendKey,EnableFixedToolbar
        
        if( !serviceValid )
            { 
            // Settings are not valid (AP/SC missing)
            iMsgLenToVKB = ETrue;
            return;
            }
        }
    else
        {
        // The plugin is not ok -> cannot send the message
        return;
        }

    if( IsPhoneOfflineL() )
        {
        resourceId = R_QTN_WAIT_MSG_SAVED_OUTBOX;
        }

    if (IsObjectsPathValidL())
        {
    	iSendOperation = CUniEditorSendOperation::NewL( *this,
                                                    	*Document(),
                                                    	*iHeader,
                                                    	*plugin,
                                                    	*iView,
                                                    	FsSession() );
        ActivateInputBlockerL(iSendOperation);

        ShowWaitNoteL(resourceId);

        iEditorFlags |= EEditorClosing;

        iSendOperation->Send();
        }
    else
        {
        if (ShowConfirmationQueryL(R_UNIEDITOR_QUEST_CLOSE_OOD))
            {
            // Exit without saving.
            Exit(EAknSoftkeyClose);
            }
        //else nothing.
        }
    }


// ---------------------------------------------------------
// CUniEditorAppUi::DoMsgSaveExitL
// Exit from option menu 
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoMsgSaveExitL() 
    {
    if ( CAknEnv::AppWithShutterRunning() )
        {
        iEditorFlags |= ERunAppShutterAtExit;
        }
    
    delete iLaunchOperation;
    iLaunchOperation = NULL;
    
    delete iSendOperation;
    iSendOperation = NULL;
    
    delete iInsertOperation;
    iInsertOperation = NULL;        
    
    delete iChangeSlideOperation;
    iChangeSlideOperation = NULL;
    
    delete iVCardOperation;
    iVCardOperation = NULL;
    
    if ( IsMessageEmpty() )
        {
        DeleteCurrentEntryL();
        }
    else if ( ( Document()->Modified() || 
                Document()->PrevSaveType() < EClosingSave ) && 
              CanSaveMessageL())
        {
        // Needs saving
        if ( IsForeground() )
            {
            TInt resId = Document()->Saved() ? R_QTN_UNI_WAIT_SAVING_MESSAGE : 
                                               R_QTN_UNI_WAIT_SAVING_MESSAGE_NEW;
            // Cannot trap call below.
            DoSaveL( EClosingSave );
            iEditorFlags |= EEditorClosing;
            
            ShowWaitNoteL( resId );
            }
        else
            {
            TInt err(KErrNone);
            TRAP( err, DoSaveL( EClosingSave ) );
            }
            
        BeginActiveWait( iSaveOperation );
        if ( iEditorFlags & ERunAppShutterAtExit )
            {
            RunAppShutter();
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ExitAndSaveL
// ---------------------------------------------------------
//
void CUniEditorAppUi::ExitAndSaveL()
    {
    if ( iEditorFlags & EEditorClosing )
        {
        Exit( EAknSoftkeyClose );
        return;
        }
        
    if ( CAknEnv::AppWithShutterRunning() )
        {
        iEditorFlags |= ERunAppShutterAtExit;
        }
    if (IsObjectsPathValidL())
        {
        if (iEditorFlags & ELaunchSuccessful && Document()->MediaAvailable())
            {
            DoMsgSaveExitL();
            }

        Exit();
        }
    else
        {
        if (ShowConfirmationQueryL(R_UNIEDITOR_QUEST_CLOSE_OOD))
            {
            // Exit without saving.
            Exit(EAknSoftkeyClose);
            }
        }
    
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoBackSaveL
// Back button implementation
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoBackSaveL()
    {
    if ( IsMessageEmpty() ) 
        {
        if ( iMtm->Entry().Entry().Visible() )
            {
            ShowInformationNoteL( R_UNIEDITOR_INFO_DELETED, ETrue );
            }
        DeleteAndExitL();
        }
    else
        {
        TInt closeVal = ShowCloseQueryL();
        if ( closeVal == EMsgCloseCancel )
            {
            return;
            }
        else if ( closeVal == EMsgCloseDelete )
            {
            ReleaseImage( ETrue );
            DeleteAndExitL();
            }
        else // closeVal == EMsgCloseSave
            {
            // Message has data
            if ( Document()->Modified() || Document()->PrevSaveType() < EClosingSave )
                {
                if ( CanSaveMessageL() && IsObjectsPathValidL())                    
                    {
                    TInt resId = Document()->Saved() ? R_QTN_UNI_WAIT_SAVING_MESSAGE : 
                                                       R_QTN_UNI_WAIT_SAVING_MESSAGE_NEW;
                    DoSaveL( EClosingSave );
                    iEditorFlags |= EEditorClosing;
                    
                    ShowWaitNoteL( resId );
                    }
                else
                    {
                    if ( ShowConfirmationQueryL( R_UNIEDITOR_QUEST_CLOSE_OOD ) )
                        {
                        // Exit without saving.
                        Exit( EAknSoftkeyClose );
                        }
                    //else nothing.
                    }
                }
            else
                {
                // No changes -> just go away.
                Exit( EAknSoftkeyClose );
                }
            }
        }
    }   

// ---------------------------------------------------------
// CUniEditorAppUi::CanSaveMessageL
// Checks whether there's enough diskspace to save the
// message.
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::CanSaveMessageL() const
    {
    TInt bytes = Document()->AddressSize() + Document()->SubjectSize();
    
    if ( Document()->DataModel()->SmilType() == EMmsSmil )
        {
        bytes += Document()->DataModel()->ObjectList().SpaceNeededForSaveAll();
        bytes += Document()->DataModel()->AttachmentList().SpaceNeededForSaveAll();
        bytes += iSmilModel->SmilComposeSize();
        }
        
    return !( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL( Document()->Session(),
                                                                   bytes ) );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoSaveL
// Saves message to OUTBOX.
// Message should be unlocked before coming here.
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoSaveL( TUniSaveType aSaveType ) 
    {
    if ( iSaveOperation && 
         iSaveOperation->IsActive() )
        {
        // To avoid starting save operation when there is already one running.
        return;
        }
        
    if ( !CanSaveMessageL() )
        {
        User::Leave( KErrDiskFull );
        }

    delete iSaveOperation;
    iSaveOperation = NULL;
    iSaveOperation = CUniEditorSaveOperation::NewL(
        *this,
        *Document(),
        *iHeader,
        *iView,
        FsSession() );
    
    iSaveOperation->Save( aSaveType );
    
    ActivateInputBlockerL( iSaveOperation );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::VerifyAddressesL
//
// Walks thru addresses in addresscontrol and counts errors. 
// If unverified address is valid, marks it validated (verified now). 
// Stops to first error.
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::VerifyAddressesL( TBool& aModified )
    {
    if ( TUniMsvEntry::IsMmsUpload( Document()->Entry() ) )
        {
        // Don't check the recipient in "Upload" case.
        return ETrue;
        }
        
    iCoeEnv->SyncNotifyFocusObserversOfChangeInFocus();
    
    aModified = EFalse;

    TBool retVal = iHeader->VerifyAddressesL( aModified, AcceptEmailAddresses() );

    if ( aModified )
        {
        Document()->SetHeaderModified( ETrue );
        CheckHeaderForMessageTypeL();
        
        SetAddressSize();
        MsgLengthToNavipaneL();
        }

    if ( !retVal )
        {        
        //We'll get here if there's illegal addresses
        //when selecting send from not-first slide 
        for (TInt i = CUniEditorHeader::EHeaderAddressTo; 
            i <= CUniEditorHeader::EHeaderAddressBcc; i++ )
            { 
            TBool jumpOut( EFalse );
            if ( iHeader->AddressControl( static_cast<CUniEditorHeader::THeaderFields>( i ) ) )               
                {
                // Search first invalid address
                CMsgRecipientArray* recipients = iHeader->AddressControl( static_cast<CUniEditorHeader::THeaderFields>( i ) )->GetRecipientsL();
                TInt addrCnt = recipients->Count();

                for ( TInt k = 0; k < addrCnt ; k++ )
                    {
                    CMsgRecipientItem* addrItem = recipients->At(k);
                    
                    if (    addrItem 
                        &&  !addrItem->IsValidated() )
                        {     
                        iNextFocus =  iHeader->AddressControlId( 
                                        static_cast<CUniEditorHeader::THeaderFields>( i ) );
                        if ( Document()->CurrentSlide() )
                            {
                            DoUserChangeSlideL( 0 );
                            }
                        else
                            {
                            DoSetFocusL();
                            }
                            
                        TInt ret = iHeader->AddressControl( static_cast<CUniEditorHeader::THeaderFields> (i ) )
                                        ->HighlightUnvalidatedStringL();
                        if ( !ret )
                            {
                            // highlight succeeded
                            jumpOut = ETrue;
                            break;
                            }
                        // else - highlight failed. Should not occur...
                        }
                    }
                    // else - recipient OK. Continue searching
                if ( jumpOut )
                    {
                    break;
                    }
                }
            // else - address control does not exist
            }
        }
    // else - addresses OK

    return retVal;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserMoveOrSelectPageL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserMoveOrSelectPageL( TBool aMovePage )
    {
    // Create listbox    
    CAknSingleGraphicPopupMenuStyleListBox* listBox = new (ELeave) CAknSingleGraphicPopupMenuStyleListBox;
    CleanupStack::PushL( listBox );
    
    // Create popup
    CAknPopupList* popup = CAknPopupList::NewL(  listBox, 
                                                 R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT, 
                                                 AknPopupLayouts::EMenuGraphicWindow );
    CleanupStack::PushL( popup );

    // Construct listbox
    listBox->ConstructL( popup, CEikListBox::ELeftDownInViewRect );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                                        CEikScrollBarFrame::EAuto );
    
    if( iSmilModel->SlideCount() > 6 )
        {
        listBox->CreateScrollBarFrameL( ETrue );
        listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                                            CEikScrollBarFrame::EAuto );
        }
  
    TInt oldSlide = Document()->CurrentSlide();

    // Set title    
    HBufC* title = NULL;
    if ( aMovePage )
        {
        title = StringLoader::LoadLC( R_UNI_MOVE_PAGE_TITLE, oldSlide+1, iCoeEnv );
        }
    else
        {
        title = StringLoader::LoadLC( R_UNI_SELECT_PAGE_TITLE, iCoeEnv );
        }
    popup->SetTitleL( title->Des() );
    CleanupStack::PopAndDestroy( title );

    CAknIconArray* iconArray = RadioButtonArrayL();
    listBox->ItemDrawer()->FormattedCellData()->SetIconArray( iconArray ); // FormattedCellData owns

    listBox->HandleItemAdditionL();

    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( 5 );
    CleanupStack::PushL( array );
    
    for ( TInt i = 0; i < iSmilModel->SlideCount(); i++ )
        {
        HBufC* buf = StringLoader::LoadLC( R_UNIEDITOR_MOVE_PAGE, i + 1, iCoeEnv );

        TBuf<20> itemString;
        if( oldSlide == i )
            { // This one is selected
            itemString.AppendNum( 1 );
            }
        else
            {
            itemString.AppendNum( 0 );
            }
        itemString.Append( _L("\t") );
        itemString.Append( buf->Des() );
        array->AppendL( itemString );
        
        CleanupStack::PopAndDestroy( buf );
        buf = NULL;
        }

    CTextListBoxModel* model = listBox->Model();
    model->SetItemTextArray( array );
    model->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( array ); // model owns it now

    listBox->SetCurrentItemIndexAndDraw( oldSlide );

    TInt result = popup->ExecuteLD();

    if ( result )
        {
        // OK, user chose a new page
        TInt newSlide = listBox->CurrentItemIndex();
         
        if( oldSlide != newSlide )
            {
            if ( aMovePage )
                {
                iSmilModel->MoveSlideL( Document()->CurrentSlide(), newSlide );
                }

            //Set current slide to invalid in order
            //to prevent false events coming to wrong slides
            Document()->SetCurrentSlide( -1 );
            iView->SetScrollParts( iSmilModel->SlideCount() );
            iView->SetCurrentPart( newSlide );

            DoUserChangeSlideL( newSlide );
            
            if ( aMovePage )
                {
                Document()->SetBodyModified( ETrue );
                }
            }
        }

    CleanupStack::Pop(); // for popup
    CleanupStack::PopAndDestroy( listBox );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserObjectsViewL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserObjectsViewL( TBool aFocusAttachments )
    {
    CUniObjectsModel& objectsViewModel =
        static_cast<CUniObjectsModel&>( Document()->AttachmentModel() );

    CAknInputBlock::NewLC();

    if ( Document()->DataModel()->SmilType() == EMmsSmil && 
         Document()->BodyModified() )
        {
        delete iSaveOperation;
        iSaveOperation = NULL;
        
        iSaveOperation = CUniEditorSaveOperation::NewL(
            *this,
            *Document(),
            *iHeader,
            *iView,
            FsSession() );
        
        
        // Save current slide text object as content needs to be up-to-date when
        // it is opened from object's view.
        CUniObject* textObject = iSmilModel->GetObject( Document()->CurrentSlide(), EUniRegionText );
        if ( textObject )
            {
            iSaveOperation->SaveL( *textObject );
            BeginActiveWait( iSaveOperation );
            }
        }

    CMsgBaseControl* imageBase = iView->ControlById( EMsgComponentIdImage );
    if ( imageBase )
        {
        //Close image file (share flag conflict)
        CMsgImageControl* imageControl =
            static_cast<CMsgImageControl*>( imageBase );
        //Closes image, no matter if animation or not
        imageControl->Stop();
        //No need to reopen image file
        }

    iOriginalSlide = Document()->CurrentSlide();
  
    TUniObjectsDialogExitCmd dlgRet = EUniObjectsViewBack;
    // Flag is also used to judge whether toolbar update is done 
    iEditorFlags |= EDoNotUpdateTitlePane;
    
    CUniObjectsViewDialog::TUniObjectsDialogType dialogType;
    if ( Document()->DataModel()->SmilType() == EMmsSmil )
        {
        dialogType = CUniObjectsViewDialog::EEditor;
        }
    else
        {
        dialogType = CUniObjectsViewDialog::EForward;
        }
    
    TInt focusedItemIndex = -1;
    if ( aFocusAttachments )
        {
        TInt objectCount = Document()->DataModel()->ObjectList().Count();
        if ( objectCount == 0 )
            {
            focusedItemIndex = 1;
            }
        else
            {
            focusedItemIndex = objectCount;
            }
        }
    
    if ( iFixedToolbar )
        {
        SetFixedToolbarDimmed();
        }
    
    CleanupStack::PushL( TCleanupItem( UpdateFixedToolbar, this ) );
   	SetKeyEventFlags(0);
    
    CleanupStack::PushL( TCleanupItem( DisableSendKey, this ) );
    
    CUniObjectsViewDialog::ObjectsViewL(
            dialogType, 
            objectsViewModel,
            iEikonEnv,
            dlgRet,
            *Document()->DataModel(),
            focusedItemIndex );
    
    CleanupStack::PopAndDestroy(2); //DisableSendKey,EnableFixedToolbar
    
    iEditorFlags &= ~EDoNotUpdateTitlePane;

    Document()->SetCurrentSlide( iOriginalSlide );
    iOriginalSlide = -1;

    // Check if msg type was changed
    CheckBodyForMessageTypeL(); 
    
    // Update navipane here instead of AddObjectL callback otherwise it 
    // gets drawn while in objectsview
    UpdateIndicatorIconsL();
    
    CheckLockedSmsWithAttaL();
    
    CleanupStack::PopAndDestroy(); // CAknInputBlock
    
    objectsViewModel.Reset();
    MsgLengthToNavipaneL( ETrue );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserAddRecipientL
// Fetch an address from phonebook
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserAddRecipientL()
    {
    TBool invalid = EFalse;
    // We remove this flag so that it can be shown after the adding operation
    iEditorFlags &= ~EMaxRecipientShown;

	if(iEditorFlags & EMsgEditInProgress)
        {
        return;
        }
    if ( iFixedToolbar )
        {
        SetFixedToolbarDimmed();    
        }

    iEditorFlags |= (EMsgEditInProgress);
    CleanupStack::PushL( TCleanupItem( UpdateFixedToolbar, this ) );
    
    SetKeyEventFlags(0);    
    CleanupStack::PushL( TCleanupItem( DisableSendKey, this ) );
    // add to current control or To control
    TBool addressesAdded(EFalse);
    TRAPD( err, addressesAdded = iHeader->AddRecipientL(  iView->FocusedControl(), 
                                                    iView, 
                                                    AcceptEmailAddresses(),
                                                    invalid ) );
    if( err == KLeaveExit )
        {
        CAknEnv::RunAppShutter();
        }

    CleanupStack::PopAndDestroy(2);//DisableSendKey,UpdateFixedToolbar
    iEditorFlags &= ~EMsgEditInProgress;

    TInt focused = iHeader->FocusedAddressField( iView->FocusedControl() );

    if ( addressesAdded )
        {
        Document()->SetHeaderModified( ETrue );

        if ( Document()->CurrentSlide() )
            {
            // Addresses were (tried to be) added
            // -> Change to first slide
            iNextFocus = EMsgComponentIdTo;
            DoUserChangeSlideL( 0 );
            }

        SetAddressSize(); 
        MsgLengthToNavipaneL();
        
        if ( focused == KErrNotFound ||
             iEditorFlags & ELockedSmsWithAtta )
            {
            if ( focused != CUniBaseHeader::EHeaderAddressTo )
                {
                // Focus was not in address control -> Move it
                iView->SetFocus( EMsgComponentIdTo );
                }
                
            // Set bodytext cursor location to start of the text
            CMsgBodyControl* bodyCtrl = BodyCtrl();
            if ( bodyCtrl )
                {
                bodyCtrl->SetCursorPosL( 0 );
                }
            focused = CUniBaseHeader::EHeaderAddressTo;
            }
        else 
            {
        // Increase by one so it will be in the next field
            switch( focused+1 )
                {
                case CUniBaseHeader::EHeaderAddressCc:
                    {
                    if( iHeader->AddressControl( CUniBaseHeader::EHeaderAddressCc ) )
                        {
                        iView->SetFocus( EMsgComponentIdCc );
                        break;
                        }
                    } // otherwise fall through
                case CUniBaseHeader::EHeaderAddressBcc:
                    {
                    if( iHeader->AddressControl( CUniBaseHeader::EHeaderAddressBcc ) )
                        {
                        iView->SetFocus( EMsgComponentIdBcc );
                        break;
                        }
                    } // otherwise fall through
                case CUniBaseHeader::EHeaderSubject:
                    {
                    if( iHeader->SubjectControl() )
                        {
                        iView->SetFocus( EMsgComponentIdSubject );
                        break;
                        }
                    } // otherwise fall through
                default:
                    {
                    if ( Document()->DataModel()->SmilType() == EMmsSmil )
                        {
                        iView->SetFocus( EMsgComponentIdBody );                    
                        
                        // Set bodytext cursor location to end of the text
                        CMsgBodyControl* bodyCtrl = BodyCtrl();
                        if ( bodyCtrl )
                            {
                            bodyCtrl->SetCursorPosL( bodyCtrl->Editor().TextLength() );
                            }
                        }
                    else
                        {
                        iView->SetFocus( EMsgComponentIdImage );
                        }
                    }
                }
            }
        }
    // Check max recipients anyway
    CheckHeaderForMessageTypeL();
    MsgLengthToNavipaneL();

    CheckMaxRecipientsAndShowNoteL();

    if( iEditorFlags & EMaxRecipientShown && addressesAdded )
        { // A note was shown so return the focus back to the original location
        TInt returnFocusTo = EMsgComponentIdTo;
        if( focused == CUniBaseHeader::EHeaderAddressCc )
            {
            returnFocusTo = EMsgComponentIdCc;
            }
        else if( focused == CUniBaseHeader::EHeaderAddressBcc )
            {
            returnFocusTo = EMsgComponentIdBcc;
            }
        iView->SetFocus( returnFocusTo );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserMessageInfoL
// 
// Invokes message info from UI MTM.
// Message must be unlocked before coming here.
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserMessageInfoL()
    {
    CUniEditorDocument* doc = Document();
    
    CUniMtmUi& uiMtm = doc->MtmUiL();

    CAknInputBlock::NewLC();

    iHeader->CopyHeadersToMtmL( doc->HeaderModified(),
                                ETrue );

    // Lets put the current message type into TMsvEntry
    TMsvEntry tEntry = doc->Entry();

    TUniMessageCurrentType curType = EUniMessageCurrentTypeSms;
    if( doc->UniState() == EUniMms )
        {
        curType = EUniMessageCurrentTypeMms;
        
        CUniEditorPlugin* mmsPlugin = doc->MmsPlugin();
            
        if ( mmsPlugin )
            {
            TUniSendingSettings settings;
            mmsPlugin->GetSendingSettingsL( settings );
            switch ( settings.iPriority )
                {
                case TUniSendingSettings::EUniPriorityHigh:
                    {
                    tEntry.SetPriority( EMsvHighPriority );
                    break;
                    }
                case TUniSendingSettings::EUniPriorityLow:
                    {
                    tEntry.SetPriority( EMsvLowPriority );
                    break;
                    }
                default:
                    {
                    tEntry.SetPriority( EMsvMediumPriority );
                    break;
                    }
                }
            }
        }

    TUniMsvEntry::SetCurrentMessageType( tEntry, curType );
    
    doc->CurrentEntry().ChangeL( tEntry );
    
    if ( !iMtm->Entry().HasStoreL() )
        {
        iMtm->SaveMessageL();
        }
    
    // Pack message size
    TInt sizeInKilos = ( MessageSizeInBytes() + KBytesInKilo - 1 ) / KBytesInKilo;
    TPckgBuf<TInt> param( sizeInKilos );

    //These are not used! They just have to be there
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );

    CMsvSingleOpWatcher* watch = CMsvSingleOpWatcher::NewLC( *Document() );
    CMsvOperation* op = uiMtm.InvokeAsyncFunctionL(
        KMtmUiFunctionMessageInfo,
        *selection,
        watch->iStatus,
        param );
    CleanupStack::Pop(); // watch
    Document()->AddSingleOperationL( op, watch );

    CleanupStack::PopAndDestroy( 2 ); // selection, CAknInputBlock

    MsgLengthToNavipaneL();
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserSendingOptionsL
// 
// Invokes Sending Options from UI MTM.
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserSendingOptionsL()
    {
    TInt flags = 0;

    TUniSendingSettings settings;
    
    CUniEditorPlugin* plugin = SmsPlugin();

    TBool currentlyLockedToSms = EFalse;

    if( plugin )
        { 
        // This should be available as if it's not -> sending options not available
        plugin->GetSendingSettingsL( settings );
        }

    //TUniSendingSettings::TUniMessageType origMessageType = settings.iMessageType;
    
    if( iMtm->MessageTypeLocking() == EUniMessageTypeLocked )
        { 
        // Pre-locked to SMS (We never get here in permanent MMS)
        flags |= EUniSettingsPermanentSms;
        currentlyLockedToSms = ETrue;
        }
    else if( iMtm->MessageTypeSetting() == EUniMessageTypeSettingSms && 
             settings.iMessageType != TUniSendingSettings::EUniMessageTypeAutomatic &&
             settings.iMessageType != TUniSendingSettings::EUniMessageTypeMultimedia )
        { 
        // msg type is temporarily locked to sms
        currentlyLockedToSms = ETrue;
        }
    else
        { 
        if( iMtm->MessageTypeSetting() == EUniMessageTypeSettingMms )
            {
            settings.iMessageType = TUniSendingSettings::EUniMessageTypeMultimedia;
            }
        else if( iMtm->MessageTypeSetting() == EUniMessageTypeSettingAutomatic )
            {
            settings.iMessageType = TUniSendingSettings::EUniMessageTypeAutomatic;
            }
        }

    CUniEditorDocument* doc = Document();

    if ( !( IsBodySmsCompatibleL( EFalse ) && iHeader->IsHeaderSmsL() ) )
        {
        flags |= EUniSettingsContentNeedsMms;
        }
      
    TUniSendingSettings::TUniCharSupport origCharSet = TUniSendingSettings::EUniCharSupportReduced;
      
    settings.iCharSupport = TUniSendingSettings::EUniCharSupportReduced;
    
    if ( doc->CharSetSupport() == EUniMessageCharSetFullLocked ||
         doc->CharSetSupport() == EUniMessageCharSetReducedLocked )
        {
        flags |= EUniSettingsHideCharSet;
        }
    else if( doc->CharSetSupport() == EUniMessageCharSetFull )
        {
        settings.iCharSupport = TUniSendingSettings::EUniCharSupportFull;
        origCharSet = TUniSendingSettings::EUniCharSupportFull;
        }

    if( !( doc->CSPBits() & RMobilePhone::KCspProtocolID ) )
        {
        flags |= EUniSettingsCSPBitsOn;
        }

    if ( iEditorFlags & EHideMessageTypeOption )
        {
        flags |= EUniSettingsHideMessageTypeOption;
        }
    TUniState oldState = doc->UniState();
  
    CUniSendingSettingsDialog::TUniExitCode exitCode = CUniSendingSettingsDialog::EUniBack;
    
    CUniSendingSettingsDialog* dlg = new ( ELeave ) CUniSendingSettingsDialog( flags, settings, exitCode, *iTitlePane );
    CleanupStack::PushL( dlg );
    
    dlg->ConstructL( R_UNI_SENDINGSETTINGS_DIALOG_MENUBAR );

    // Flag is also used to judge whether toolbar update is done 
    iEditorFlags |= EDoNotUpdateTitlePane;
    
    CleanupStack::Pop( dlg );
    
    if ( iFixedToolbar )
        {
        SetFixedToolbarDimmed();
        }
    
  
    CleanupStack::PushL( TCleanupItem( UpdateFixedToolbar, this ) );
    SetKeyEventFlags(0);    
    CleanupStack::PushL( TCleanupItem( DisableSendKey, this ) );  
    
    dlg->ExecuteLD( R_UNI_SENDINGSETTINGS_DIALOG );
    
    CleanupStack::PopAndDestroy(2); //DisableSendKey,UpdateFixedToolbar
    
    iEditorFlags &= ~EDoNotUpdateTitlePane;

    // Now store the changed settings into sms plugin if it exists
    // (there's no need to save them in mms plugin as user cannot change anything there)
    if( plugin )
        {
        plugin->SetSendingSettingsL( settings );
        }

    TBool unhideConfigurableFields = EFalse;

    if ( !( flags & EUniSettingsHideCharSet ) )
        { 
        // Ok, it was possible to change the charset in the first place
        if ( settings.iCharSupport != origCharSet )
            { 
            // CharSet has changed -> create new converter
            TUniMessageCharSetSupport charSetSupport = EUniMessageCharSetFull;
            if ( settings.iCharSupport == TUniSendingSettings::EUniCharSupportReduced )
                {
                charSetSupport = EUniMessageCharSetReduced;
                } 
            doc->CreateCharConverterL( charSetSupport ); 
            if( iNLTFeatureSupport )
                {
               	iSettingsChanged = ETrue;                
                //Turkish SMS-PREQ2265 specific
                if(plugin)
                    {
                    //Update the new charset support selected by user.
                    //Reset uniCode mode flag to EFalse(Restarts encoding algorithm in smsPlugin for optimal length calculation) 
                    plugin->SetEncodingSettings(EFalse, doc->AlternativeEncodingType(), charSetSupport); 
                    }                
                }
            }
        }

    // Now lets see the msg type changes

    if( iMtm->MessageTypeLocking() == EUniMessageTypeLocked )
        { 
        // First lets handle pre locked sms
        // no operation here because the msg type does not change
        // but just the bit in the message settings
        }
    else if( settings.iMessageType == TUniSendingSettings::EUniMessageTypeAutomatic )
        { 
        // Message type is now automatic
        iMtm->SetMessageTypeSetting( EUniMessageTypeSettingAutomatic );
        
        if ( IsBodySmsCompatibleL() )
            {
            doc->SetBodyUniState( EUniSms );
            }
        else
            {
            doc->SetBodyUniState( EUniMms );
            }
        
        if ( iHeader->IsHeaderSmsL() )
            {
            doc->SetHeaderUniState( EUniSms );
            }
        else
            {            
            doc->SetHeaderUniState( EUniMms );
            }
    
        if ( currentlyLockedToSms )
            { 
            // Show the fields again
            unhideConfigurableFields = ETrue;
            }
        }
    else if ( settings.iMessageType == TUniSendingSettings::EUniMessageTypeMultimedia )
        {
        iMtm->SetMessageTypeSetting( EUniMessageTypeSettingMms );
        if ( IsBodySmsCompatibleL() )
            {
            doc->SetBodyUniState( EUniSms );
            }
        else
            {
            doc->SetBodyUniState( EUniMms );
            }
            
        if ( currentlyLockedToSms )
            { 
            // Show the fields again
            unhideConfigurableFields = ETrue;
            }
        }
    else
        { 
        // Settings is one of the sms conversions (none, fax, pager)
        if ( iMtm->MessageTypeSetting() != EUniMessageTypeSettingSms )
            {
            iMtm->SetMessageTypeSetting( EUniMessageTypeSettingSms );
            // Also remove those additional headers that need to be removed
            TInt removeHeaders( 0 ); 
            if ( iHeader->AddressControl( CUniBaseHeader::EHeaderAddressCc ) )
                {
                removeHeaders |= EUniFeatureCc;
                }
            if ( iHeader->AddressControl( CUniBaseHeader::EHeaderAddressBcc ) )
                {
                removeHeaders |= EUniFeatureBcc;
                }
            if ( !(iEditorFlags & ESubjectOkInSms ) && iHeader->SubjectControl() )
                {
                removeHeaders |= EUniFeatureSubject;
                }
            iHeader->AddHeadersDeleteL( removeHeaders, EFalse );
            }
        // We can change this here because if it was not then SMS/Fax/Pager would not have been available in options
        // This is needed here in case SMS soft and hard limits are different so we have to "hardcode" the type to SMS
        doc->SetBodyUniState( EUniSms );
        doc->SetHeaderUniState( EUniSms );
        }

    if ( unhideConfigurableFields )
        { 
        // Read the current situation
        TInt addHeaders = iHeader->AddHeadersConfigL();
        // Add them to header
        iHeader->AddHeadersAddL( addHeaders );
        }

    if ( oldState != doc->UniState() )
        {  
         // If the state is not the same anymore, show popup
        UpdateIndicatorIconsL();
        ShowPopupNoteL( doc->UniState() == EUniMms ? ETrue : EFalse );
        }
    
    CheckLockedSmsWithAttaL();
    
    UpdateToolbarL();
    
    SetTitleL();
    MsgLengthToNavipaneL();

    // Removes or sets the max size according to the settings
    SetOrRemoveMaxSizeInEdwin();
    
    if ( exitCode != CUniSendingSettingsDialog::EUniBack )
        {
        CAknAppUi::ProcessCommandL( EAknCmdExit );
        return;
        }

    UpdateMiddleSoftkeyL();
    UpdateIndicatorIconsL();
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserInsertMediaL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserInsertMediaL()
    {
    RArray<TInt> disabledItems;
    CleanupClosePushL( disabledItems );  

    if ( !( iSupportedFeatures & EUniFeatureCamcorder ) )
        {
        disabledItems.Append( EUniCmdInsertMediaNewVideo );
        disabledItems.Append( EUniCmdInsertMediaNewImage );
        }
    
    TInt selectedIndex( 0 );
    CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &selectedIndex );
    dlg->PrepareLC( R_UNIEDITOR_INSERT_QUERY );
    
    CEikFormattedCellListBox* listbox = dlg->ListControl()->Listbox();
    CDesCArray* itemArray = static_cast<CDesCArray*>( listbox->Model()->ItemTextArray() );
    
    if ( disabledItems.Count() > 0 )
        {
        for ( TInt currentItem = 0; currentItem < disabledItems.Count(); currentItem++ )
            {
            itemArray->Delete( disabledItems[ currentItem ] );
            listbox->HandleItemRemovalL();
            }
        }
        
    if ( dlg->RunLD() )
        {
        for ( TInt currentItem = disabledItems.Count() - 1; currentItem >= 0 ; currentItem-- )
            {
            if ( selectedIndex >= disabledItems[ currentItem ] )
                {
                selectedIndex++;
                }
            }
                
        switch ( selectedIndex )    
            {
            case EUniCmdInsertMediaImage:
                {
                DoUserInsertImageL( MsgAttachmentUtils::EImage );
                break;
                } 
            case EUniCmdInsertMediaNewImage:
                {
                DoUserInsertImageL( MsgAttachmentUtils::ENewImage );
                break;
                } 
            case EUniCmdInsertMediaVideo:
                {
                DoUserInsertVideoL( MsgAttachmentUtils::EVideo );
                break;
                } 
            case EUniCmdInsertMediaNewVideo:
                {
                DoUserInsertVideoL( MsgAttachmentUtils::ENewVideo );
                break;
                } 
            case EUniCmdInsertMediaAudio:
                {
                DoUserInsertAudioL( MsgAttachmentUtils::EAudio );
                break;
                } 
            case EUniCmdInsertMediaNewAudio:
                {
                DoUserInsertAudioL( MsgAttachmentUtils::ENewAudio );
                break;
                } 
            case EUniCmdInsertMediavCard:
                {
                DoUserInsertVCardL();
                break;
                }
            case EUniCmdInsertMediaSlide:
                {
                DoUserInsertSlideL();
                break;
                }
            case EUniCmdInsertMediaNote:
                {
                DoUserInsertTextL( EMemoText );
                break;
                }
            case EUniCmdInsertMediaTextAtta:
                {
                DoUserInsertOtherFileL( ETextFile );
                break;
                }
            case EUniCmdInsertMediaTemplate:
                {
                DoUserInsertTextL( ETemplateText );
                break;
                }
//            case EUniCmdInsertMediaSVG:
//                {
//                DoUserInsertSvgL();
//                break;
//                }
            case EUniCmdInsertMediaOtherFile:
                {
                DoUserInsertOtherFileL( EOtherFile );
                break;
                }
            default: 
                {
                __ASSERT_DEBUG( EFalse, Panic( EUniUnsupportedCommand ) );
                break;
                }
            }
        }
        
    CleanupStack::PopAndDestroy( &disabledItems );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoStartInsertL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoStartInsertL( TBool aAddSlide, TBool aAddAsAttachment )
    {
    if ( aAddSlide &&
         iSmilModel->MaxSlideCountReached() )
       {
       ShowConfirmableInfoL( R_UNIEDITOR_INFO_MAX_SLIDES_ALL );
       iEditorFlags &= ~EMsgEditInProgress;
       }
   else
       {
	   //for png reusing iInsertOperation corrupts heap, so delete iInsertOperation and recreate again.
        if ( (iInsertOperation) &&(iInsertingMedia->MimeType() == KMsgMimeImagePng) )
            {
            delete iInsertOperation;
            iInsertOperation = NULL;
            }
		if(!iInsertOperation)
		{
        	iInsertOperation = CUniEditorInsertOperation::NewL(
            	*this,
            	*Document(),
            	*iHeader,
            	*iSlideLoader,
            	*iView,
            	FsSession() );
        }
        if ( aAddSlide )
            {
            iEditorFlags |= EInsertAddsSlide;
            }
        else
            {
            iEditorFlags &= ~EInsertAddsSlide;
            }
            
        ShowWaitNoteL( R_QTN_UNI_WAIT_INSERTING );
        
        iInsertOperation->Insert(
            iInsertingMedia,
            aAddSlide,
            aAddAsAttachment );
        
        ActivateInputBlockerL( iInsertOperation );
        
        iInsertingMedia = NULL;
       }
       // Enable the toolbar since inputblockers will take care now
      if( iFixedToolbar )
        {
        iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, EFalse, ETrue );    
        }
    }


// ---------------------------------------------------------
// CreateMediaInfoForInsertL
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::CreateMediaInfoForInsertL( TMsgMediaType aMediaType,
                                                  const TDesC& aFileName )
    {
    RFile file = Document()->DataModel()->MediaResolver().FileHandleL( aFileName );
    CleanupClosePushL( file );
    
    delete iInsertingMedia;
    iInsertingMedia = NULL;
    
    iInsertingMedia = Document()->DataModel()->MediaResolver().CreateMediaInfoL( file );

    Document()->DataModel()->MediaResolver().ParseInfoDetailsL( iInsertingMedia, file );
    
    TMmsConformance conformance = 
        Document()->DataModel()->MmsConformance().MediaConformance( *iInsertingMedia );
    
    CleanupStack::PopAndDestroy( &file );
    
    TInt errResId = KUniInvalidResourceId;
    TInt confStatus = conformance.iConfStatus;

    // Supported type check
    if ( confStatus & EMmsConfNokNotSupported ||
        ( Document()->CreationMode() == EMmsCreationModeRestricted &&
          confStatus & EMmsConfNokFreeModeOnly ) )
        {
        errResId = R_UNIEDITOR_INFO_UNSUPPORTED_OBJECT;
        if( Document()->CreationModeUserChangeable() &&
           !Document()->CreationMode() )
            {
            errResId = R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECT;
            }
        }
    else if ( confStatus & EMmsConfNokDRM )
        {
        // DRM check
        errResId = R_UNIEDITOR_INFO_SEND_FORBID_1;
        }
    else if ( aMediaType != EMsgMediaUnknown && 
              iInsertingMedia->MediaType() != aMediaType )
        {
        // Keep this after DRM check!
        errResId = R_UNIEDITOR_INFO_UNSUPPORTED_OBJECT;
        if( Document()->CreationModeUserChangeable() &&
            !Document()->CreationMode() )
            {
            errResId = R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECT;
            }
        }
    else if ( !conformance.iCanAdapt &&
        ( confStatus & EMmsConfNokTooBig ||
        TUint( MessageSizeInBytes() + iInsertingMedia->FileSize() )
        > Document()->MaxMessageSize() ) )
        {
        // File is too big and it cannot be adapted (e.g. scaled down)
        errResId = R_UNIEDITOR_INFO_OBJECT_TOO_BIG;
        }

    if ( errResId != KUniInvalidResourceId )
        {
        delete iInsertingMedia;
        iInsertingMedia = NULL;
        ShowInformationNoteL( errResId, EFalse );
        return EFalse;
        }

    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL( Document()->Session(),
                                                              iInsertingMedia->FileSize() ) )
        {
         delete iInsertingMedia;
         iInsertingMedia = NULL;       
         ShowConfirmableInfoL( R_UNIEDITOR_NOT_ENOUGH_MEMORY );
         return EFalse;             
        }
        
    return ETrue;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::SuperDistributionCheckForInsertL
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::SuperDistributionCheckForInsertL()
    {
    // SuperDistribution check & query
    if ( iSupportedFeatures & EUniFeatureDrmFull &&
         iInsertingMedia->Protection() & EFileProtSuperDistributable )
        {
        if ( !ShowConfirmationQueryL( R_UNIEDITOR_QUEST_SEND_WO_RIGHTS ) )
            {
            delete iInsertingMedia;
            iInsertingMedia = NULL;
            return EFalse;
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::CheckMediaInfoForInsertL
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::CheckMediaInfoForInsertL()
    {
    __ASSERT_DEBUG( iInsertingMedia, Panic( EUniNullPointer ) );

    TMmsConformance conformance = 
        Document()->DataModel()->MmsConformance().MediaConformance( *iInsertingMedia );
    
    TInt errResId = KUniInvalidResourceId;
    TInt confStatus = conformance.iConfStatus;
    
    // In "free" mode user can insert images that are larger by dimensions than allowed by conformance
    if ( Document()->CreationMode() != EMmsCreationModeRestricted )
        {
        //Mask "Corrupted" away in free & guided mode.
        confStatus &= ~EMmsConfNokCorrupt;
        
        // If user answers yes to Guided mode confirmation query he/she moves to free mode
        if ( ( confStatus & ( EMmsConfNokFreeModeOnly | EMmsConfNokScalingNeeded ) ) &&
             !( confStatus & ~( EMmsConfNokFreeModeOnly | EMmsConfNokScalingNeeded ) ) )
            {
            if ( iInsertingMedia->Protection() & EFileProtSuperDistributable )
                {
                // SuperDistribution not checked here
                // Mask "FreeModeOnly" and "ScalingNeeded" away in free mode
                confStatus &= ~EMmsConfNokFreeModeOnly;
                confStatus &= ~EMmsConfNokScalingNeeded;
                }
            else if ( ShowGuidedModeConfirmationQueryL( R_UNIEDITOR_QUEST_GUIDED_OBJ ) )
                {
                // Guided query accepted.
                // Mask "FreeModeOnly" and "ScalingNeeded" away in free mode
                confStatus &= ~EMmsConfNokFreeModeOnly;
                confStatus &= ~EMmsConfNokScalingNeeded;
                }
            else
                {
                // Guided query not accepted. Stop insertion.
                delete iInsertingMedia;
                iInsertingMedia = NULL;
                return EFalse;
                }
            }
        }

    if ( confStatus & EMmsConfNokTooBig ||
        TUint( MessageSizeInBytes() + iInsertingMedia->FileSize() )
        > Document()->MaxMessageSize() )
        {
        // Size check
        errResId = R_UNIEDITOR_INFO_OBJECT_TOO_BIG;
        }
    else if ( confStatus != EMmsConfOk )
        {
        // Sanity check
        errResId = R_UNIEDITOR_INFO_UNSUPPORTED_OBJECT;
        if( Document()->CreationModeUserChangeable() &&
            !Document()->CreationMode() )
            {
            errResId = R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECT;
            }
        }

    if ( errResId != KUniInvalidResourceId )
        {
        delete iInsertingMedia;
        iInsertingMedia = NULL;
        ShowInformationNoteL( errResId, EFalse );
        return EFalse;
        }
        
    return ETrue;
    }


// ---------------------------------------------------------
// CUniEditorAppUi::DoUserInsertSlideL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserInsertSlideL()
    {
    __ASSERT_DEBUG( Document()->DataModel()->SmilType() == EMmsSmil, Panic( EUniIllegalSmilType ) );

    if ( TUint( MessageSizeInBytes() + KEmptySlideSize ) > Document()->MaxMessageSize() )
        {
        ShowConfirmableInfoL( R_UNIEDITOR_INFO_OBJECT_TOO_BIG );
        }
    else if ( iSmilModel->MaxSlideCountReached() )
        {
        ShowConfirmableInfoL( R_UNIEDITOR_INFO_MAX_SLIDES_ALL );
        }
    else
        {
        TInt index = Document()->CurrentSlide() + 1;
        iSmilModel->AddSlideL( index );
        Document()->SetBodyModified( ETrue );
        DoUserChangeSlideL( index );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserRemoveSlideL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserRemoveSlideL()
    {
    __ASSERT_DEBUG( Document()->DataModel()->SmilType() == EMmsSmil, Panic( EUniIllegalSmilType ) );

    if ( ShowConfirmationQueryL( R_UNIEDITOR_QUEST_REMOVE_PAGE ) ) 
        {
        DoRemoveSlideL();
        }
        
    Document()->SetBodyModified( ETrue ); 
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoRemoveSlideL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoRemoveSlideL()
    {
    TInt currSlide = Document()->CurrentSlide();
    TMsgControlId focusedControl( EMsgComponentIdNull );
    
    if( Document()->DataModel()->SmilType() == EMmsSmil &&
        ( currSlide == 0 || currSlide == 1 ) )
        { 
        if ( Document()->DataModel()->SmilType() == EMmsSmil )
            {
            CMsgAddressControl* to = iHeader->AddressControl( CUniEditorHeader::EHeaderAddressTo );
            CMsgAddressControl* cc = iHeader->AddressControl( CUniEditorHeader::EHeaderAddressCc );
            CMsgAddressControl* bcc = iHeader->AddressControl( CUniEditorHeader::EHeaderAddressBcc );
            
            if( to && to->GetRecipientsL()->Count() == 0 )
                {
                focusedControl = EMsgComponentIdTo;
                }
            else if( cc && cc->GetRecipientsL()->Count() == 0 )
                {
                focusedControl = EMsgComponentIdCc;
                }
            else if( bcc && bcc->GetRecipientsL()->Count() == 0 )
                {
                focusedControl = EMsgComponentIdBcc;
                }
            else if( iSupportedFeatures & EUniFeatureSubject &&
                     SubjectCtrl() &&
                     SubjectCtrl()->TextContent().DocumentLength() == 0 )
                {
                focusedControl = EMsgComponentIdSubject;
                }
            }
        }
    
    TInt objCount = iSmilModel->SlideObjectCount( currSlide );
    ReleaseImage( ETrue );
    
    for ( TInt i = 0; i < objCount; i++ )
        {
        iSmilModel->RemoveObjectByIndexL( currSlide, 0 );
        }
    iSmilModel->RemoveSlide( currSlide );
    TInt nextSlide = currSlide;
    if ( nextSlide )
        {
        nextSlide--;
        }
    //Set current slide to invalid in order
    //to prevent false events coming to wrong slides
    Document()->SetCurrentSlide( -1 );
    iView->SetScrollParts( iSmilModel->SlideCount() );
    iView->SetCurrentPart( nextSlide );
    iNextFocus = focusedControl;
    DoUserChangeSlideL( nextSlide );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserChangeSlideL
//
// If change is made from first slide containing address fields into
// some other slide and address control is currently focused set
// the current input mode into all address fields. This enables
// correct input mode to be used if focus is set to different
// address field when returning to first slide and also if
// message is closed from different slide.
// After input mode is set operation is created if necessary and
// activated.
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserChangeSlideL( TInt aSlideNum )
    {
    __ASSERT_DEBUG( Document()->DataModel()->SmilType() == EMmsSmil, Panic( EUniIllegalSmilType ) );
    
    CMsgBaseControl* ctrl = iView->FocusedControl();  // ctrl can be NULL.
    if ( ctrl &&
         Document()->CurrentSlide() == 0 &&
         iHeader->IsAddressControl( ctrl->ControlId() ) )
        {
        SetInputModeToAddressFields( static_cast<CMsgAddressControl*>( ctrl )->Editor().AknEditorCurrentInputMode() );
        }
    
    if ( !iChangeSlideOperation )
        {
        iChangeSlideOperation = CUniEditorChangeSlideOperation::NewL(
            *this,
            *Document(),
            *iHeader,
            *iSlideLoader,
            *iView,
            FsSession() );
        }
    
    iChangeSlideOperation->ChangeSlide(  aSlideNum );

    iNextSlide = aSlideNum;
    
    iScreenClearer = CAknLocalScreenClearer::NewLC( EFalse );
    CleanupStack::Pop();
    iScreenClearer->SetExtent( iScreenClearer->Position(), ClientRect().Size() );
    
    ShowWaitNoteL( R_QTN_UNI_WAIT_SLIDE_CHANGE );
    
    ActivateInputBlockerL( iChangeSlideOperation );
        
    BeginActiveWait( iChangeSlideOperation );
    }


// ---------------------------------------------------------
// CUniEditorAppUi::DoSetFocusL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoSetFocusL()
    {
    // 1. to To: control, if empty and exists
    // 2. if not empty, to Subject: control, if empty and exists
    // 3. otherwise to body    
    TMsgControlId focusedControl( EMsgComponentIdTo );
    if ( iNextFocus != EMsgComponentIdNull )
        {
        // iNextFocus is set when changing slide
        focusedControl = iNextFocus;
        iNextFocus = EMsgComponentIdNull;
        }
    else
        {
        // To first empty field. Cc and Bcc are not supported here
        CMsgAddressControl* to = ToCtrl();
        
        if ( Document()->DataModel()->SmilType() == EMmsSmil &&  
             ( !to || 
               to->GetRecipientsL()->Count() > 0 ) )
            {
            // No To-control or there are recipients
            // -> focus to end of bodytext or to Subject field if empty
            focusedControl = EMsgComponentIdBody;
            if ( iSupportedFeatures & EUniFeatureSubject )
                {
                CMsgExpandableControl* subj = SubjectCtrl();
                if ( subj && 
                     subj->TextContent().DocumentLength() == 0 )
                    {
                    focusedControl = EMsgComponentIdSubject;
                    }
                }
            
            if ( !BodyCtrl() )
                {
                focusedControl = EMsgComponentIdTo;
                }
                
            if ( focusedControl == EMsgComponentIdBody )
                {
                BodyCtrl()->SetCursorPosL( BodyCtrlEditor()->TextLength() );
                }
            }
        }
    iView->SetFocus( focusedControl );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::SetTitleL
// ---------------------------------------------------------
//
void CUniEditorAppUi::SetTitleL()
    {
    // If titlepane exists -> set the right title
    HBufC* titleString = Document()->UniState() == EUniMms ? iTitleMms : 
                                                             iTitleSms;
    iTitlePane->SetTextL( titleString->Des(), ETrue );
        
    CGulIcon* image = Document()->UniState() == EUniMms? iIconMms : 
                                                         iIconSms;
    
    // Create duplicates of the icon to be used
    CFbsBitmap* bitmap = new( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmap );
    
    CFbsBitmap* bitmapMask = new( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmapMask );
    
    User::LeaveIfError( bitmap->Duplicate( image->Bitmap()->Handle() ) );
    User::LeaveIfError( bitmapMask->Duplicate( image->Mask()->Handle() ) );

    iTitlePane->SetSmallPicture( bitmap, bitmapMask, ETrue );
    if(!iFinalizeLaunchL)
    iTitlePane->DrawNow();
    
    CleanupStack::Pop( 2, bitmap );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserInsertImageL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserInsertImageL( MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType )
    {
    __ASSERT_DEBUG( Document()->DataModel()->SmilType() == EMmsSmil, Panic( EUniIllegalSmilType ) );
    
    if( iFixedToolbar )
        {
        iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, ETrue, EFalse );    
        }
    iInsertingType = EMsgMediaImage;
    
    TBool fetchFile( ETrue );
    TBool enabletoolbarext = ETrue;
    while ( fetchFile )
        {
        fetchFile = EFalse;
        iEditorFlags |= EMsgEditInProgress;
        TBool stat= EFalse;
        TRAPD(Err,stat = FetchFileL( aFetchType ));
         if (Err == KErrNone && stat)        
            {
            if ( SuperDistributionCheckForInsertL() )
                {
                // CheckMediaInfoForInsertL() done in CUniEditorInsertOperation
                TBool addSlide = EFalse;
                if ( ObjectsAvailable() & 
                    ( EUniImageFlag | EUniVideoFlag | EUniSvgFlag ) )
                    {
                    addSlide = ETrue;
                    }

                DoStartInsertL( addSlide, EFalse );
                enabletoolbarext = EFalse;
                }
            else
                {
                fetchFile = ETrue;
                }
            }
         else if(Err == KLeaveExit)
             {
              User::Leave( Err );
             }
         else
            {
            iEditorFlags &= ~EMsgEditInProgress;           
            }
        }
        if(enabletoolbarext)
            {
                //Enable the insert toolbar 
            if( iFixedToolbar )
                {
                iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, EFalse, ETrue );
                HandleResourceChangeL(KEikDynamicLayoutVariantSwitch);
                }
            }
                                 
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserInsertVideoL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserInsertVideoL( MsgAttachmentUtils::TMsgAttachmentFetchType  aFetchType )
    {
    __ASSERT_DEBUG( Document()->DataModel()->SmilType() == EMmsSmil, Panic( EUniIllegalSmilType ) );
    
   if( iFixedToolbar )
        {
        iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, ETrue, EFalse );    
        }
    iInsertingType = EMsgMediaVideo;
    
    TBool fetchFile( ETrue );
    TBool enabletoolbarext = ETrue;
    while ( fetchFile )
        {
        fetchFile = EFalse;
        iEditorFlags |= EMsgEditInProgress;
        TBool stat= EFalse;
        TRAPD(Err,stat = FetchFileL( aFetchType ));
        if (Err == KErrNone && stat)        
            {
            if ( SuperDistributionCheckForInsertL() )
                {
                if ( CheckMediaInfoForInsertL() )
                    {
                    TBool addSlide = EFalse;
                
                    if ( ObjectsAvailable() & 
                        ( EUniAudioFlag | EUniImageFlag | EUniVideoFlag | EUniSvgFlag ) )
                        {
                        addSlide = ETrue;
                        }
                    DoStartInsertL( addSlide, EFalse );
                    enabletoolbarext = EFalse;
                    }
                else
                    {
                     iEditorFlags &= ~EMsgEditInProgress;
                    }
                }
            else
                {
                fetchFile = ETrue;
                }
            }
        else if(Err == KLeaveExit)
            {
             User::Leave( Err );
            }
        else
            {
            iEditorFlags &= ~EMsgEditInProgress;
            }
        }
      if(enabletoolbarext)
            {
                //Enable the insert toolbar 
            if( iFixedToolbar )
                {
                iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, EFalse, ETrue );    
                HandleResourceChangeL(KEikDynamicLayoutVariantSwitch);
                }
            }
                         
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserInsertAudioL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserInsertAudioL( MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType )
    {
    __ASSERT_DEBUG( Document()->DataModel()->SmilType() == EMmsSmil, Panic( EUniIllegalSmilType ) );
    
   if( iFixedToolbar )
        {
        iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, ETrue, EFalse );    
        }
    iInsertingType = EMsgMediaAudio;
    
    TBool fetchFile( ETrue );
    TBool enabletoolbarext = ETrue;
    while ( fetchFile )
        {
        fetchFile = EFalse;
        iEditorFlags |= EMsgEditInProgress;
        TBool stat= EFalse;
        TRAPD(Err,stat = FetchFileL( aFetchType ));
        if (Err == KErrNone && stat)        
            {
            if ( SuperDistributionCheckForInsertL() )
                {
                if ( CheckMediaInfoForInsertL() )
                    {
                    TBool addSlide = EFalse;
                    if ( ObjectsAvailable() & 
                        ( EUniAudioFlag | EUniVideoFlag | EUniSvgFlag ) )
                        {
                        addSlide = ETrue;
                        }
                    DoStartInsertL( addSlide, EFalse );
                    enabletoolbarext = EFalse ; 
                    }
                else
                    {
                     iEditorFlags &= ~EMsgEditInProgress;
                    }
                }
            else
                {
                fetchFile = ETrue;
                }
            }
        else if(Err == KLeaveExit)
            {
             User::Leave( Err );
            }
         else
            {
            iEditorFlags &= ~EMsgEditInProgress;
            }
        }
        if(enabletoolbarext)
            {
                //Enable the insert toolbar 
            if( iFixedToolbar )
                {
                iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, EFalse, ETrue );    
                HandleResourceChangeL(KEikDynamicLayoutVariantSwitch);
                }
            }
                         
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserInsertTextL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserInsertTextL( TUniEditorInsertTextType aType )
    {
    __ASSERT_DEBUG( Document()->DataModel()->SmilType() == EMmsSmil, Panic( EUniIllegalSmilType ) );
    TInt err( KErrNone );
    TInt errResId = KUniInvalidResourceId;
    TInt sizeLimit( Document()->MaxMessageSize() - MessageSizeInBytes() ); 

     if( iFixedToolbar ) 
        {
        iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, ETrue, EFalse );    
        }
    if ( aType == ETemplateText )
        {
        InsertTemplateL();
        }
    else
        {
        err = InsertTextMemoL( sizeLimit );
        }
        
    if( iFixedToolbar )
        {
        iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, EFalse, EFalse );    
        }
    if ( err == KErrOverflow)
        {
        errResId = R_UNIEDITOR_INFO_OBJECT_TOO_BIG;
        ShowConfirmableInfoL( errResId );
        }
    else
        {
    UpdateSmilTextAttaL();
    
    Document()->SetBodyModified( ETrue );
    CheckBodyForMessageTypeL();
    
    MsgLengthToNavipaneL();
    CheckMaxSmsSizeAndShowNoteL();
    SetOrRemoveMaxSizeInEdwin();
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserInsertOtherFileL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserInsertOtherFileL( TUniEditorInsertOtherType aType )
    {
    TBool addAttachment( EFalse );
     if( iFixedToolbar )
        {
        iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, ETrue, EFalse );    
        }
    
    TBool enabletoolbarext = ETrue;
    if ( aType == ETextFile )
        {
        iInsertingType = EMsgMediaText;
        TBool stat= EFalse;
        TRAPD(Err,stat = FetchFileL( MsgAttachmentUtils::ENote ));
        if (Err == KErrNone && stat)  
            {            
            addAttachment = ETrue;
            }
        }
    else
        {
        TFileName* fileName = new( ELeave ) TFileName;
        CleanupStack::PushL( fileName );
            
        if ( MsgAttachmentUtils::FetchAnyFileL( *fileName, *iEikonEnv ) )
            {
            Document()->DataModel()->MediaResolver().SetCharacterSetRecognition( ETrue );
            
            TRAPD( error, CreateMediaInfoForInsertL( EMsgMediaUnknown, *fileName ) );
            
            Document()->DataModel()->MediaResolver().SetCharacterSetRecognition( EFalse );
            
            User::LeaveIfError( error );
            
            addAttachment = ETrue;
            }
        
        CleanupStack::PopAndDestroy( fileName );
        }
  
    if ( addAttachment && iInsertingMedia )
        {
        iInsertingType = iInsertingMedia->MediaType();
        if (    SuperDistributionCheckForInsertL() )
            {
            // CheckMediaInfoForInsertL() for images done in CUniEditorInsertOperation
            if (    iInsertingType == EMsgMediaImage
                ||  CheckMediaInfoForInsertL() )
                {
                TBool addSlide( EFalse );
                TBool addAsAttachment( ETrue );
                                
                if ( iInsertingType != EMsgMediaText &&
                     iInsertingType != EMsgMediaXhtml && 
                     iSmilModel->IsSupportedMediaType( iInsertingType ) )
                    {
                    addAsAttachment = EFalse;
                    if ( ( ( iInsertingType == EMsgMediaImage || 
                             iInsertingType == EMsgMediaVideo ||
                             iInsertingType == EMsgMediaAnimation ||
                             iInsertingType == EMsgMediaSvg ) &&
                           ( ObjectsAvailable() & ( EUniImageFlag | EUniVideoFlag | EUniSvgFlag ) ) )||
                         ( iInsertingType == EMsgMediaAudio &&
                           ( ObjectsAvailable() & ( EUniAudioFlag | EUniVideoFlag | EUniSvgFlag ) ) ) )
                        {
                        addSlide = ETrue;
                        }
                    }
                DoStartInsertL( addSlide, addAsAttachment );
                enabletoolbarext = EFalse ; 
                }
            }
        }
        if(enabletoolbarext)
            {
            if( iFixedToolbar )
                {
                iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, EFalse, ETrue );    
                }
            }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserInsertVCardL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserInsertVCardL()
    {
    if ( !iVCardOperation )
        {
        iVCardOperation = CUniEditorVCardOperation::NewL( *this,
                                                          *Document(),
                                                          FsSession() );
        }
    
    if ( iFixedToolbar )
        {
          iFixedToolbar->HideItem( EUniCmdFixedToolbarSend, ETrue, ETrue );
          iFixedToolbar->HideItem( EUniCmdFixedToolbarAddRecipient, ETrue, ETrue );        
          iFixedToolbar->HideItem( EUniCmdFixedToolbarInsert, ETrue, ETrue );      
        }

    iEditorFlags |= EToolbarHidden;
    
    iVCardOperation->Start();    
        
    ActivateInputBlockerL( iVCardOperation );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserInsertSvgL
// ---------------------------------------------------------
//
//void CUniEditorAppUi::DoUserInsertSvgL()
//    {
//    __ASSERT_DEBUG( Document()->DataModel()->SmilType() == EMmsSmil, Panic( EUniIllegalSmilType ) );
//    
//     if( iFixedToolbar ) 
//        {
//        iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, ETrue, EFalse );    
//        }
//    iInsertingType = EMsgMediaSvg;
//    
//    TBool fetchFile( ETrue );
//    TBool enabletoolbarext = ETrue;
//    while ( fetchFile )
//        {
//        fetchFile = EFalse;
//        TBool stat= EFalse;
//        TRAPD(Err,stat = FetchFileL( MsgAttachmentUtils::ESVG ));
//        if (Err == KErrNone && stat)  
//            {
//            if ( SuperDistributionCheckForInsertL() )
//                {
//                if ( CheckMediaInfoForInsertL() )
//                    {
//                    TBool addSlide = EFalse;
//                    if ( ObjectsAvailable() & 
//                        ( EUniAudioFlag | EUniVideoFlag | EUniImageFlag | EUniSvgFlag ) )
//                        {
//                        addSlide = ETrue;
//                        }
//                    
//                    DoStartInsertL( addSlide, EFalse );
//                    enabletoolbarext = EFalse; 
//                    }
//                }
//            else
//                {
//                fetchFile = ETrue;
//                }
//            }
//        }
//        if(enabletoolbarext)
//            {
//                //Enable the insert toolbar 
//            if( iFixedToolbar )
//                {
//                iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, EFalse, ETrue );    
//                }
//            }
//    }
    
// ---------------------------------------------------------
// CUniEditorAppUi::InitNaviPaneL
// Draws navipane.
// ---------------------------------------------------------
//
void CUniEditorAppUi::InitNaviPaneL()
    {
    UNILOGGER_WRITE( "-> CUniEditorAppUi::InitNaviPaneL" );
    
    UpdateIndicatorIconsL();
    
    TInt charsLeft = 0;
    TInt msgsParts = 0;               
    CalculateSMSMsgLen(charsLeft,msgsParts);
    iPrevSmsLength = charsLeft;
    MsgLengthToNavipaneL( EFalse );            
    
    UNILOGGER_WRITE( "<- CUniEditorAppUi::InitNaviPaneL" );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::UpdateIndicatorIconsL
//
// ---------------------------------------------------------
//
void CUniEditorAppUi::UpdateIndicatorIconsL()
    {
    UNILOGGER_WRITE( "-> CUniEditorAppUi::UpdateIndicatorIconsL" );
    
    TInt priorityHigh;
    TInt priorityLow;

    if( Document()->UniState() == EUniSms )
        { 
        UNILOGGER_WRITE( "-> CUniEditorAppUi::UpdateIndicatorIconsL SMS" );
        
        // If dealing with SMS -> disable them
        priorityHigh = EAknIndicatorStateOff;
        priorityLow = EAknIndicatorStateOff;
        }
    else 
        {
        TUniSendingSettings settings;
        CUniEditorPlugin* plugin = MmsPlugin();

        if ( plugin )
            {
            plugin->GetSendingSettingsL( settings );
            }

        if ( settings.iPriority == TUniSendingSettings::EUniPriorityLow )
            {
            priorityHigh = EAknIndicatorStateOff;
            priorityLow = EAknIndicatorStateOn;
            }
        else if ( settings.iPriority == TUniSendingSettings::EUniPriorityHigh )
            {
            priorityHigh = EAknIndicatorStateOn;
            priorityLow = EAknIndicatorStateOff;
            }
        else
            {
            priorityHigh = EAknIndicatorStateOff;
            priorityLow = EAknIndicatorStateOff;
            }
        }
    
    if ( iSupportedFeatures & EUniFeaturePriority) 
        {
        UNILOGGER_WRITE( "-> CUniEditorAppUi::UpdateIndicatorIconsL priority" );
        
        MAknEditingStateIndicator* editIndi = iAvkonEnv->EditingStateIndicator();
        
        CAknIndicatorContainer* naviIndi = 
            static_cast<CAknIndicatorContainer*>( iNaviDecorator->DecoratedControl() );
    
        // Update the Edwin indicator pane
        if ( editIndi )
            {
            CAknIndicatorContainer* indiContainer = editIndi->IndicatorContainer();
            if ( indiContainer )
                {
                indiContainer->SetIndicatorState( TUid::Uid(EAknNaviPaneEditorIndicatorMcePriorityHigh), 
                                                  priorityHigh );
                indiContainer->SetIndicatorState( TUid::Uid(EAknNaviPaneEditorIndicatorMcePriorityLow), 
                                                  priorityLow );
                }
            }
            
        // Update the navi indicator pane
        if ( naviIndi )
            {
            naviIndi->SetIndicatorState( TUid::Uid(EAknNaviPaneEditorIndicatorMcePriorityHigh), 
                                         priorityHigh );
            naviIndi->SetIndicatorState( TUid::Uid(EAknNaviPaneEditorIndicatorMcePriorityLow), 
                                         priorityLow );
            }

        }
    
    UNILOGGER_WRITE( "<- CUniEditorAppUi::UpdateIndicatorIconsL" );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ShowInformationNoteL
// ---------------------------------------------------------
//
void CUniEditorAppUi::ShowInformationNoteL( TInt aResourceID, TBool aWaiting )
    {
    HBufC* prompt = NULL;
    
    switch ( aResourceID )
        {
        case R_UNIEDITOR_INFO_OBJECT_TOO_BIG:
        case R_UNIEDITOR_INFO_OBJECT_SEND_AS_TOO_BIG:
            {
            // These are not anymore information notes
            ShowConfirmableInfoL( aResourceID );
            return;
            }
        default:
            {
            prompt = StringLoader::LoadLC( aResourceID, iCoeEnv );
            break;
            }
        }

    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( aWaiting );
    note->ExecuteLD( *prompt );
    
    CleanupStack::PopAndDestroy( prompt );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ShowErrorNoteL
// ---------------------------------------------------------
//
void CUniEditorAppUi::ShowErrorNoteL( TInt aResourceID, TBool aWaiting )
    {
    HBufC* prompt = StringLoader::LoadLC( aResourceID, iCoeEnv );
    
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote( aWaiting );
    note->ExecuteLD( *prompt );
    
    CleanupStack::PopAndDestroy( prompt );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ShowConfirmationQueryL
// ---------------------------------------------------------
//
TInt CUniEditorAppUi::ShowConfirmationQueryL( TInt aResourceID )
    {
    HBufC* prompt = StringLoader::LoadLC( aResourceID, iCoeEnv );
    
    TInt retVal = ShowConfirmationQueryL( *prompt );
    
    CleanupStack::PopAndDestroy( prompt );
    
    return retVal;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ShowConfirmationQueryL
// ---------------------------------------------------------
TInt CUniEditorAppUi::ShowConfirmationQueryL( const TDesC& aText ) const
    {
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    return dlg->ExecuteLD( R_UNIEDITOR_CONFIRMATION_QUERY, aText );    
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ShowConfirmationNoQuestionQueryL
// ---------------------------------------------------------
TInt CUniEditorAppUi::ShowConfirmationNoQuestionQueryL( const TDesC& aText ) const
    {
    // Utilize OK and CANCEL softkeys in confirmation query. Required when 
    // the query does not include a question.
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    return dlg->ExecuteLD( R_UNIEDITOR_CONFIRMATION_NO_QUESTION_QUERY, aText );    
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ShowConfirmableInfoL
// ---------------------------------------------------------
//
TInt CUniEditorAppUi::ShowConfirmableInfoL( TInt aResourceID )
    {
    HBufC* prompt = NULL;
    
    switch ( aResourceID )
        {
        case R_UNIEDITOR_INFO_OBJECT_TOO_BIG:
        case R_UNIEDITOR_INFO_OBJECT_SEND_AS_TOO_BIG:
            {
            TInt maxInKilos = ( Document()->MaxMessageSize() + KBytesInKilo - 1 ) / KBytesInKilo;
            prompt = StringLoader::LoadLC( aResourceID, maxInKilos, iCoeEnv  );
            break;
            }
        case R_UNIEDITOR_INFO_MAX_SLIDES_ALL:
        case R_UNIEDITOR_INFO_MAX_SLIDES_SOME:
            {
            prompt = StringLoader::LoadLC( aResourceID, Document()->DataModel()->SmilModel().MaxSlideCount(), iCoeEnv  );            
            break;
            }
        default:
            {
            prompt = StringLoader::LoadLC( aResourceID, iCoeEnv );
            break;
            }
        }
        
    TInt retVal = KErrNone;
    switch ( aResourceID )
        {
        case R_UNIEDITOR_ERROR_RMODE_CANNOT_OPEN:
            retVal = ShowConfirmableInfoErrorL( *prompt );
            break;        
        default:
            retVal = ShowConfirmableInfoL( *prompt );
        }
        
    CleanupStack::PopAndDestroy( prompt );
    return retVal;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ShowConfirmableInfoL
// ---------------------------------------------------------
//
TInt CUniEditorAppUi::ShowConfirmableInfoL( const TDesC& aText ) const
    {
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    return dlg->ExecuteLD( R_UNIEDITOR_CONFIRMABLE_INFO, aText );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ShowConfirmableInfoErrorL
// ---------------------------------------------------------
//
TInt CUniEditorAppUi::ShowConfirmableInfoErrorL( const TDesC& aText ) const
    {
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    return dlg->ExecuteLD( R_UNIEDITOR_CONFIRMABLE_INFO_ERROR, aText );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ShowGuidedModeConfirmationQueryL
// ---------------------------------------------------------
//
TInt CUniEditorAppUi::ShowGuidedModeConfirmationQueryL( TInt aResourceID )
    {
    TInt retVal = EFalse;
    if ( iEditorFlags & EShowGuidedConf )
        {
        TInt waitResId( KErrNotFound );
        if ( iWaitDialog )
            {
            // Remove wait note for the confirmation query duration.
            waitResId = iWaitResId;
            RemoveWaitNote();
            }
            
        retVal = ShowConfirmationQueryL( aResourceID );
        
        if ( retVal )
            {
            iEditorFlags &= ~EShowGuidedConf;
            }
            
        if ( waitResId != KErrNotFound )
            {
            // Set the wait not again visible after confirmation query is shown
            // if it was visible before.
            ShowWaitNoteL( waitResId );
            }
        }
    else
        {
        retVal = ( Document()->CreationMode() != EMmsCreationModeRestricted );
        }
        
    return retVal;
    }


// ---------------------------------------------------------
// CUniEditorAppUi::ShowWaitNoteL
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::ShowWaitNoteL( TInt aResourceId )
    {
    TInt waitNoteResource = R_UNIEDITOR_WAIT_NOTE;
    TBool waitNoteDelayOff = EFalse;
    
    if ( aResourceId == R_QTN_MSG_WAIT_SENDING_MMS )
        {
        waitNoteResource = R_UNIEDITOR_WAIT_NOTE_MMS_ICON;
        }
    else if ( aResourceId == R_QTN_MSG_WAIT_SENDING_SMS )
        {
        waitNoteResource = R_UNIEDITOR_WAIT_NOTE_SMS_ICON;
        }
        
    HBufC* string = NULL;

    switch ( aResourceId )
        {
        case R_QTN_MSG_WAIT_SENDING_MMS:
        case R_QTN_MSG_WAIT_SENDING_SMS:
        case R_QTN_WAIT_MSG_SAVED_OUTBOX:
        case R_QTN_UNI_WAIT_SAVING_MESSAGE_NEW:
            {
            waitNoteDelayOff = ETrue;
            }
        case R_QTN_UNI_WAIT_OPENING_EDITOR:
        case R_QTN_UNI_WAIT_SAVING_MESSAGE:
        case R_QTN_UNI_WAIT_INSERTING:
            {
            string = StringLoader::LoadLC( aResourceId, iCoeEnv );
            break;
            }
        case R_QTN_UNI_WAIT_SLIDE_CHANGE:
            {
            CArrayFixFlat<TInt>* array = new ( ELeave ) CArrayFixFlat<TInt>( 2 );
            CleanupStack::PushL( array );
            
            array->AppendL( iNextSlide + 1 );
            array->AppendL( iSmilModel->SlideCount() );
            
            string = StringLoader::LoadL( R_QTN_UNI_WAIT_SLIDE_CHANGE,
                                          *array,
                                          iCoeEnv );
            
            CleanupStack::PopAndDestroy( array );
            CleanupStack::PushL( string );
            break;
            }
        default:
            {
            break;
            }
        }

    iWaitDialog = new( ELeave ) CAknWaitDialog( reinterpret_cast<CEikDialog**>( &iWaitDialog ),
                                                waitNoteDelayOff );
                                                
    // Coverty Fix, Forward Null, http://ousrv057/cov.cgi?cid=35691                                            
    if(string)
    	{
    	iWaitDialog->SetTextL( *string );
    	CleanupStack::PopAndDestroy( string ); 
    	}
    
    iWaitDialog->SetCallback( this );
    
    iWaitResId = aResourceId;
    
    iWaitDialog->PrepareLC( waitNoteResource );
    return iWaitDialog->RunLD();
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DialogDismissedL
//
// This is a bit tricky. DialogDismissedL is called when wait dialog
// is really dismissed from the screen. Wait dialog is not neccessary
// dismmissed at once the ProcessFinishedL function is called. This 
// is because of some minimum delayes etc. tied to the visibility of
// wait dialog. But this more complex than that as wait dialog can be dismissed
// by end key or escape key from the screen before the operation has really been
// completed or cancelled. This needs to be taken into account here.
// ---------------------------------------------------------
//
void CUniEditorAppUi::DialogDismissedL( TInt dismissed )
    {
    if ( dismissed == EAknSoftkeyDone )
        {
        switch ( iWaitResId )
            {
            case R_QTN_MSG_WAIT_SENDING_MMS:
            case R_QTN_MSG_WAIT_SENDING_SMS:
            case R_QTN_WAIT_MSG_SAVED_OUTBOX:
                {
                DoSendComplete2ndPhase();
                break;
                }
            case R_QTN_UNI_WAIT_SAVING_MESSAGE_NEW:
            case R_QTN_UNI_WAIT_SAVING_MESSAGE:
                {
                DoSavingComplete2ndPhase();
                break;
                }
            case R_QTN_UNI_WAIT_OPENING_EDITOR:
            case R_QTN_UNI_WAIT_INSERTING:
                {
                break;
                }
            case R_QTN_UNI_WAIT_SLIDE_CHANGE:
                {
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else if ( dismissed == EEikBidCancel )
        {
        iWaitDialog = NULL;
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::RemoveWaitNote
// ---------------------------------------------------------
//
void CUniEditorAppUi::RemoveWaitNote()
    {
    delete iWaitDialog;
    iWaitDialog = NULL;
    iWaitResId = -1;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::SetAddressSize
// ---------------------------------------------------------
//
void CUniEditorAppUi::SetAddressSize()
    {
    if ( !iView )
        {
        return;
        }
    
    TInt tempBytes( 0 );
    TInt entryCnt( 0 );
    TInt sizeInBytes( 0 );
    for ( TInt i = CUniEditorHeader::EHeaderAddressTo; 
          i <= CUniEditorHeader::EHeaderAddressBcc; 
          i++ )
        {    
        if ( iHeader->AddressControl( static_cast<CUniEditorHeader::THeaderFields> (i) ) )
            {
            iHeader->AddressControl( 
                static_cast<CUniEditorHeader::THeaderFields> (i) )->GetSizeOfAddresses( entryCnt, 
                                                                                        tempBytes );
            sizeInBytes += tempBytes;
            }
        }
        
    Document()->SetAddressSize( sizeInBytes );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::SetSubjectSize
// ---------------------------------------------------------
//
void CUniEditorAppUi::SetSubjectSize()
    {
    if ( !iView )
        {
        return;
        }
        
    CMsgExpandableControl* subj = iHeader->SubjectControl();
    
    if ( subj )
        {
        TPtrC subject = subj->TextContent().Read( 0, subj->TextContent().DocumentLength() );
        Document()->SetSubjectSize( CUniDataUtils::UTF8Size( subject ) );
        } 
        else
        {
        Document()->SetSubjectSize( 0 );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::RemoveObjectL
// 
// Intended to be called from Objectsview. Updates screen
// when object removed in Objectsview. Objectsview has already
// delete atta from msgstore if needed.
// ---------------------------------------------------------
//
void CUniEditorAppUi::RemoveObjectL( TUniRegion aRegion, 
                                     TUniLayout aLayout )
    {
    if ( iOriginalSlide != -1 && 
         Document()->CurrentSlide() != iOriginalSlide )
        {
        //nothing to do if the modified slide is not
        //visible
        return;
        }

    TBool removeCtrlFocused( EFalse ); 
    CMsgBaseControl* ctrl = NULL;
    TInt ctrlType = EMsgComponentIdNull;
    
    switch ( aRegion )
        {
        case EUniRegionImage:
            {
            ctrl = iView->RemoveControlL( EMsgComponentIdImage );
            if( !ctrl )
                { // Also video clip is located in image region so if no image -> check video control
                ctrl = iView->RemoveControlL( EMsgComponentIdVideo );
                }
            if ( !ctrl )
                { // Also SVG presentation is located in image region so if no image -> check SVG control
                ctrl = iView->RemoveControlL( EMsgComponentIdSvg );
                }
            
            if( ctrl )
                {
                removeCtrlFocused = ctrl->IsFocused();
                ctrlType = ctrl->ControlId();
                }
                
            delete ctrl;
            break;
            }
        case EUniRegionAudio:
            {
            ctrl = iView->RemoveControlL( EMsgComponentIdAudio );
            
            if( ctrl )
                {
                removeCtrlFocused = ctrl->IsFocused();
                ctrlType = ctrl->ControlId();
                }
                
            delete ctrl;
            break;
            }
        case EUniRegionText:
            {
            // Clear control content
            ctrl = BodyCtrl();
            
            if ( ctrl )
                {
                ctrl->Reset();
                ctrlType = ctrl->ControlId();
                }
                
            // Move control position if needed. Move focus & cursor position
            CMsgImageControl* imgCtrl = ImageCtrl();

            if ( imgCtrl && ( aLayout == EUniTextFirst ) )
                {
                // Move text control after the image. Cursor loc in text should be ok.
                CMsgBaseControl* bodyCtrl = iView->RemoveControlL( EMsgComponentIdBody );
                iView->AddControlL( bodyCtrl, EMsgComponentIdBody, EMsgAppendControl, EMsgBody );
                }
            break;
            }
        case EUniRegionUnresolved:
        default:
            {
            // Should never be here!
            __ASSERT_DEBUG( EFalse, Panic( EUniIllegalComponentType ) );
            break;
            }
        }   
        
    if ( ( aRegion == EUniRegionImage || 
           aRegion == EUniRegionAudio ) )
        {
        // Change focus to the beginning that scroll bar is updated
        // and text editor area removes extra lines
        if ( ToCtrl() )
            {
            iView->SetFocus( EMsgComponentIdTo );
            }
        else if ( iView->ControlById( EMsgComponentIdAudio ) )
            {
            //Audio is always first
            iView->SetFocus( EMsgComponentIdAudio );
            }
        else if ( iSmilModel->Layout() == EUniImageFirst &&  
                  ImageCtrl() )
            {
            // should not be need to update focus anymore
            iView->SetFocus( EMsgComponentIdImage );
            }    
        else 
            {
            // should not be need to update focus anymore
            iView->SetFocus( EMsgComponentIdBody );
            }

        CMsgBaseControl* focusedControl = iView->FocusedControl();
        if ( focusedControl )
            {
            if ( removeCtrlFocused &&  
                 focusedControl->ControlId() != EMsgComponentIdImage &&  
                 ctrlType == EMsgComponentIdAudio &&  
                 iSmilModel->Layout() == EUniImageFirst &&  
                 ImageCtrl() )
                {
                iView->SetFocus( EMsgComponentIdImage );
                }
            else  if ( focusedControl->ControlId() != EMsgComponentIdBody )
                {
                iView->SetFocus( EMsgComponentIdBody );            
                }

            if ( ctrlType == EMsgComponentIdAudio &&  
                 focusedControl->ControlId() == EMsgComponentIdImage )
                {
                // Set focus here so that imagecontrol will draw navipane if focused.
                iView->FocusedControl()->SetFocus( ETrue, EDrawNow );
                }
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserChangeOrderL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserChangeOrderL( TUniLayout aLayout )
    {
    // Both image and video are located in image region so there's only need to check it
    CUniObject* img = iSmilModel->GetObject( Document()->CurrentSlide(), EUniRegionImage );

    if ( img )
        {
        CAknLocalScreenClearer::NewLC( EFalse );
        
        ChangeOrderL( aLayout );
        
        CleanupStack::PopAndDestroy(); //clearer
        }

    iSmilModel->SetLayoutL( aLayout );
    Document()->SetBodyModified( ETrue );
    if ( !img )
        {
        TInt noteId = ( aLayout == EUniTextFirst ) ? R_UNIEDITOR_CONF_TEXTS_FIRST : 
                                                     R_UNIEDITOR_CONF_TEXTS_SECOND;
        ShowInformationNoteL( noteId, EFalse );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ChangeOrderL
// 
// Intended to be called from Objectsview. Updates screen
// when object order changed. SmilModel should already 
// contain correct layout. Also called by AppUi
// ---------------------------------------------------------
//
void CUniEditorAppUi::ChangeOrderL( TUniLayout aLayout )
    {
    TInt focusedId = EMsgComponentIdNull;
    if ( iView && iView->FocusedControl() )
        {
        focusedId = iView->FocusedControl()->ControlId();
        }
        
    TInt index = EMsgAppendControl;
    // Coverty fix: Forward NULL, http://ousrv057/cov.cgi?cid=35695
    if(iView)
    	{
    CMsgBaseControl* ctrl = iView->RemoveControlL( EMsgComponentIdBody ); // Does not leave

    if ( ctrl ) 
        {
        switch( aLayout )
            {
            case EUniImageFirst:
                {
                index = EMsgAppendControl;
                break;
                }
            case EUniTextFirst:
                {
                if ( iView->ControlById ( EMsgComponentIdAudio ) )
                    {
                    index = EMsgFirstControl + 1;
                    }
                else
                    {
                    index = EMsgFirstControl;
                    }
                break;
                }
            case EUniUndefinedLayout:
            default:
                {
                __ASSERT_DEBUG( EFalse, Panic( EUniIllegalLayout ) );
                break;
                }
            }
            
        iView->AddControlL( ctrl, EMsgComponentIdBody, index, EMsgBody );
        }
      }
    
    if ( !( iHeader->IsAddressControl( focusedId ) ||  
            focusedId == EMsgComponentIdSubject ||
            focusedId == EMsgComponentIdAttachment ) &&
         iEditorFlags & ELaunchSuccessful )
        {
        SetFocusToBodyBeginningL();
        }
    //else -> keep focus as is
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ReleaseImage
// 
// If there is image control, sets image lock according 
// to parameter. Otherwise does nothing.
// ---------------------------------------------------------
//
void CUniEditorAppUi::ReleaseImage( TBool aRelease )
    {
    CMsgImageControl* ctrl = ImageCtrl();
    
    if ( ctrl && aRelease )
        {
        ctrl->SetImageFileClosed();
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::MessageSizeInBytes
// ---------------------------------------------------------
//
TInt CUniEditorAppUi::MessageSizeInBytes()
    {
    TBool useEstimate = EFalse;
    
    if ( Document()->DataModel()->SmilType() == EMmsSmil &&
        ( Document()->BodyModified() || 
          iEditorFlags & EUseEstimatedSmilSize ) )
        {
        useEstimate = ETrue;
        iEditorFlags |= EUseEstimatedSmilSize;
        }

    TInt size = Document()->MessageSize( useEstimate );
    CEikRichTextEditor* bodyEditor = BodyCtrlEditor();
    
    if ( bodyEditor )
        {
        TInt limit = ( TUint( size ) >= Document()->MaxMessageSize() ) ? 
            BodyCtrl()->TextContent().DocumentLength() : 
            KMaxTInt;
        bodyEditor->SetTextLimit( limit );
        }
    return size;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoSelectionKeyL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoSelectionKeyL()
    {
    CMsgBaseControl* ctrl = iView->FocusedControl();

    if ( ctrl )
        {
        switch ( ctrl->ControlId() )
            {
            case EMsgComponentIdTo:
            case EMsgComponentIdCc:
            case EMsgComponentIdBcc:
                {
                if ( TUniMsvEntry::IsMmsUpload( Document()->Entry() ) )
                    {
                    break;
                    }
                    
                // Check is there any recipients in address ctrl
                TBool modified = EFalse;
                if ( iHeader->HasRecipients() )
                    {
                    // Recipients found. Verify addresses.
                    if ( !VerifyAddressesL( modified ) )
                        {
                        // Illegal address found.
                        modified = ETrue;
                        }
                    }
                    
                if ( !modified )//else
                    {
                    // Nothing changed on the UI. Open PhoneBook.
                    DoUserAddRecipientL();
                    }
                    
                break;
                }
            case EMsgComponentIdAttachment:
                {
                DoUserObjectsViewL( ETrue );                
                break;
                }
            case EMsgComponentIdImage:
                {
                if ( Document()->DataModel()->SmilType() == E3GPPSmil )
                    {
                    // focus is on "no-edit" SMIL icon
                    PlayPresentationL();
                    break;
                    } 
                if( Document()->DataModel()->SmilType() == ETemplateSmil )
                    {
                    break;
                    }
                // Otherwise fallthrough
                }
            case EMsgComponentIdVideo:
            case EMsgComponentIdAudio:
            case EMsgComponentIdSvg:
                {
                PlayFocusedItemL();
                break;
                }
            case EMsgComponentIdBody:
                {
                MenuBar()->SetMenuTitleResourceId( R_UNIEDITOR_CONTEXT_MENUBAR );
                MenuBar()->SetMenuType(CEikMenuBar::EMenuContext);
                
                TRAPD( err, MenuBar()->TryDisplayMenuBarL() ); 
                
                MenuBar()->SetMenuTitleResourceId( R_UNIEDITOR_MENUBAR );
                MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
                
                User::LeaveIfError( err );                
                break;
                }
            case EMsgComponentIdSubject:
            default:
                {
                break;
                }
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::SetErrorResource
//
// This is a function where error id priorities
// can be defined.
// ---------------------------------------------------------
//
void CUniEditorAppUi::SetErrorResource( TInt& aStoreId, TInt aNewId )
    {
    if ( ErrorPriority( aNewId ) >= ErrorPriority( aStoreId ) )
        {
        if ( ( aStoreId == R_UNIEDITOR_INFO_UNSUPPORTED_OBJECT ||  
               aStoreId == R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECT ) &&
             ( aNewId == R_UNIEDITOR_INFO_UNSUPPORTED_OBJECT ||  
               aNewId == R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECT ||  
               aNewId == R_UNIEDITOR_INFO_UNSUPPORTED_OBJECTS ||  
               aNewId == R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECTS ) )
            {
            // Set plural
            aStoreId = R_UNIEDITOR_INFO_UNSUPPORTED_OBJECTS;
            if ( Document()->CreationModeUserChangeable() &&
                 !Document()->CreationMode() )
                {
                aStoreId = R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECTS;
                }
            }
        else if ( aStoreId == R_UNIEDITOR_QUEST_GUIDED_OBJ &&
                  ( aNewId == R_UNIEDITOR_QUEST_GUIDED_OBJ ||
                    aNewId == R_UNIEDITOR_QUEST_GUIDED_OBJS ) )
            {
            // Set plural
            aStoreId = R_UNIEDITOR_QUEST_GUIDED_OBJS;
            }
        else if ( aStoreId == R_UNIEDITOR_QUEST_GUIDED_INC_OBJ &&
                  ( aNewId == R_UNIEDITOR_QUEST_GUIDED_INC_OBJ ||
                    aNewId == R_UNIEDITOR_QUEST_GUIDED_INC_OBJS ) )
            {
            // Set plural
            aStoreId = R_UNIEDITOR_QUEST_GUIDED_INC_OBJS;
            }
        else if ( aStoreId != -1 && 
                  aStoreId != aNewId )
            {
            aStoreId = aNewId;
            }
        else
            {
            aStoreId = aNewId;
            }
        }
    }
    
// ---------------------------------------------------------
// CUniEditorAppUi::ErrorPriority
//
// This is a function where error id priorities
// can be defined.
// ---------------------------------------------------------
//
TInt CUniEditorAppUi::ErrorPriority( TInt aErrorId )
    {
    TInt priority = 0;
    
    switch ( aErrorId )
        {
        case R_UNIEDITOR_INFO_SEND_FORBID_1:
            {
            priority++; // Fallthrough
            }
        case R_UNIEDITOR_INFO_OBJECT_TOO_BIG:
        case R_UNIEDITOR_INFO_OBJECT_SEND_AS_TOO_BIG:
            {
            priority++; // Fallthrough
            }
        case R_UNIEDITOR_INFO_MAX_SLIDES_SOME:
        case R_UNIEDITOR_INFO_MAX_SLIDES_ALL:
            {
            priority++; // Fallthrough
            }
        case R_UNIEDITOR_QUEST_GUIDED_PRESENTATION:
        case R_UNIEDITOR_ERROR_CANNOT_OPEN:
        case R_UNIEDITOR_ERROR_RMODE_CANNOT_OPEN:
            {
            priority++; // Fallthrough
            }
        case R_UNIEDITOR_QUEST_GUIDED_OBJ:
        case R_UNIEDITOR_QUEST_GUIDED_OBJS:
            {
            priority++; // Fallthrough
            }
        case R_UNIEDITOR_QUEST_GUIDED_INC_OBJ:
        case R_UNIEDITOR_QUEST_GUIDED_INC_OBJS:
            {
            priority++; // Fallthrough
            }
        case R_UNIEDITOR_INFO_SCALING_FAILED:
        case R_UNIEDITOR_INFO_RMODE_SCALING_FAILED:
            {
            priority++; // Fallthrough
            }
        case R_UNIEDITOR_INFO_UNSUPPORTED_OBJECT:
        case R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECT:
        case R_UNIEDITOR_INFO_UNSUPPORTED_OBJECTS:
        case R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECTS:
        case R_UNIEDITOR_INFO_SOME_NOT_SUPPORTED:
        case R_UNIEDITOR_INFO_RMODE_SOME_NOT_SUPPORTED:
            {
            priority++;
            break;
            }
        default:
            {
            break;
            }
        }
        
    return priority;
    }
    
// ---------------------------------------------------------
// CUniEditorAppUi::IsPhoneOfflineL
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::IsPhoneOfflineL() const
    {
    if ( iSupportedFeatures & EUniFeatureOffline )
        {    
        return MsvUiServiceUtilitiesInternal::IsPhoneOfflineL();
        }
    else
        {
        return EFalse;
        }       
    }

// ---------------------------------------------------------
// CUniEditorAppUi::LaunchHelpL
// ---------------------------------------------------------
//
void CUniEditorAppUi::LaunchHelpL()
    {
    // activate Help application
    if ( iSupportedFeatures & EUniFeatureHelp )
        {
        CArrayFix<TCoeHelpContext>* helpContext = AppHelpContextL();
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::HelpContextL
// ---------------------------------------------------------
//
CArrayFix<TCoeHelpContext>* CUniEditorAppUi::HelpContextL() const
    {
    CArrayFix<TCoeHelpContext>* array = new( ELeave ) CArrayFixFlat<TCoeHelpContext>( 1 );
    CleanupStack::PushL( array ); 
  
    array->AppendL(TCoeHelpContext(KUniEditorAppId, KUNIFIED_HLP_EDITOR()));
    
    CleanupStack::Pop( array );
    return array;
    }


// ---------------------------------------------------------
// CUniEditorAppUi::PlayPresentationL
// ---------------------------------------------------------
//
void CUniEditorAppUi::PlayPresentationL()
    {
    RFile smilFile = Document()->DataModel()->SmilList().GetSmilFileByIndexL( 0 );
    if ( !iParser )
        {        
        iParser = CMDXMLParser::NewL( this );
        }
    
    iParser->ParseFile( smilFile );
    // Continues in ParseFileCompleteL
    
    ActivateInputBlockerL( iParser );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ParseFileCompleteL
// ---------------------------------------------------------
//
void CUniEditorAppUi::ParseFileCompleteL()
    {
    // Detach the dom from parser
    if ( iDom )
        {
        delete iDom;
        iDom = NULL;
        }
        
    iDom = iParser->DetachXMLDoc();

    // Delete inputBlocker
    DeactivateInputBlocker();

    // Start playing the presentation    
    DoCompletePlayPresentationL();
    }

// ---------------------------------------------------------
// CUniEditorAppUi::PlayPresentationL
//
// First go through every object in the presentation to find out
// whether audio should be enabled. After this launch SMIL player.
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoCompletePlayPresentationL()
    {
    TBool audio = EFalse;
    CUniObjectList& objects = Document()->DataModel()->ObjectList();
    TInt count = objects.Count();
    while ( count-- )
        {
        CUniObject* object = objects.GetByIndex( count );
        TMsgMediaType media = object->MediaType();
        
        if ( media == EMsgMediaAudio )
            {
            audio = ETrue;
            break;
            }
        else if ( media == EMsgMediaVideo )
            {
            CMsgVideoInfo* info = static_cast<CMsgVideoInfo*>( object->MediaInfo() );
            if ( !info ||
                 info->IsAudio() )
                {
                audio = ETrue;
                break;
                }
            }
        }
        
    CSmilPlayerDialog* smilPlayer = CSmilPlayerDialog::NewL( iDom,
                                                             &objects,
                                                             KNullDesC(),
                                                             ETrue,
                                                             audio );
    
    if ( iFixedToolbar )
        {
        iFixedToolbar->SetToolbarVisibility(EFalse, EFalse);
        }
    
    iEditorFlags |= EToolbarHidden;
    CleanupStack::PushL( TCleanupItem( EnableFixedToolbar, this ) );
    
    SetKeyEventFlags(0);
    
    CleanupStack::PushL( TCleanupItem( DisableSendKey, this ) );
    smilPlayer->ExecuteLD();
    
    CleanupStack::PopAndDestroy(2);// DisableSendKey,EnableFixedToolbar
    
    if ( iFixedToolbar )
        {
        iFixedToolbar->SetToolbarVisibility(ETrue, EFalse);
        }
    // re-calculate all pos and dimensions of layout's widgets, after
    // status pane is set back to usual and fixed-toolbar is made visible.
    HandleResourceChangeL(KEikDynamicLayoutVariantSwitch);
    }

// ---------------------------------------------------------
// CUniEditorAppUi::RemoveTemplateL
// 
// First confirms the operation from user. After user has 
// confirmed the operation remove all the objects and SMILs
// from the message. Enable toolbar and set modified flag on. 
// Then replace the presentation icon with body text control and add
// single slide is no slides exists. Finally check the message type and
// update statuspane according to current message type.
// ---------------------------------------------------------
//
void CUniEditorAppUi::RemoveTemplateL()
    {
    if ( ShowConfirmationQueryL( R_UNIEDITOR_QUEST_REMOVE_TEMPLATE ) )
        {
        CUniObjectList& objectList = Document()->DataModel()->ObjectList();
        objectList.RemoveAllObjectsL();
        
        CMsvStore* editStore = Document()->Mtm().Entry().EditStoreL();
        CleanupStack::PushL( editStore );            
        Document()->DataModel()->SmilList().RemoveSmilL( *editStore );
        editStore->CommitL();
        CleanupStack::PopAndDestroy( editStore );

        Document()->DataModel()->SetSmilType( EMmsSmil );
        
        UpdateToolbarL();
        
        Document()->SetBodyModified( ETrue );

        // Delete icon
        iView->DeleteControlL( EMsgComponentIdImage );  // Does not leave
        
        // Add body
        CMsgBodyControl* bodyC = CMsgBodyControl::NewL(iView);
        CleanupStack::PushL( bodyC );
        
        iView->AddControlL( bodyC, EMsgComponentIdBody, EMsgAppendControl, EMsgBody );
        
        CleanupStack::Pop( bodyC );
        
        iView->SetFocus( EMsgComponentIdBody );

        if ( !iSmilModel->SlideCount() )
            {
            iSmilModel->AddSlideL();
            }
            
        CheckBodyForMessageTypeL();
        InitNaviPaneL();
        SetTitleL();
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::SetFocusToBodyBeginningL
// ---------------------------------------------------------
//
void CUniEditorAppUi::SetFocusToBodyBeginningL()
    {
    if ( ToCtrl() )
        {
        // Set focus always first to "To" in order to 
        // make "To" field visible
        iView->SetFocus( EMsgComponentIdTo );
        }
        
    if ( BodyCtrl() )
        {
        BodyCtrl()->SetCursorPosL( 0 );
        }
        
    TBool focusId = EMsgComponentIdBody;
    
    if ( iView->ControlById( EMsgComponentIdAudio ) )
        {
        //Audio is always first
        focusId = EMsgComponentIdAudio;
        } 
    // When this is called from DoUserChangerOrderL the new layout is not
    // yet updated in SmilModel -> that's why the if's below seem to be mixed
    else if ( iSmilModel->Layout() == EUniTextFirst &&
              iView->ControlById( EMsgComponentIdImage ) )
        {
        focusId = EMsgComponentIdImage;
        }
    else if ( iSmilModel->Layout() == EUniTextFirst &&
              iView->ControlById( EMsgComponentIdVideo ) )
        {
        focusId = EMsgComponentIdVideo;
        }
    //else -> EMsgComponentIdBody

    iView->SetFocus( focusId );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoInsertCompleteL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoInsertCompleteL( TUniEditorOperationEvent aEvent )
    {
    // focus and cursor positions handling
    TBool insertedAsAttachment = EFalse;
    
    if ( aEvent == EUniEditorOperationComplete )
        {  
        if ( iInsertOperation->IsAddedAsAttachment() )
            {
            Document()->SetHeaderModified( ETrue );
            insertedAsAttachment = ETrue;
            }
        else
            {
           SetFocusToBodyBeginningL();
            
            switch ( iInsertingType )
                {
                case EMsgMediaImage:
                    {
                    // Set the focus to Text field so that user will be able to
					// Type the text when image height is too big
                    CMsgBodyControl* bodyCtrl = BodyCtrl();
                    if ( bodyCtrl )
                        {
						// Put the cursor at the end of Text.
                        bodyCtrl->SetCursorPosL( bodyCtrl->Editor().TextLength() );
                        iView->SetFocus( EMsgComponentIdBody );                           
                        }
                    break;
                    }
                case EMsgMediaAudio:
                    {
                    // Set focus always first to the audio control
                    iView->SetFocus( EMsgComponentIdAudio );
                    break;
                    }
                case EMsgMediaVideo:
                    {
                    // Set focus always first to the audio control
                    iView->SetFocus( EMsgComponentIdVideo );
                    break;
                    }
                case EMsgMediaSvg:
                    {
                    // Set focus always first to the audio control
                    iView->SetFocus( EMsgComponentIdSvg );
                    break;
                    }
                default:
                    {
                    // nothing to do
                    break;
                    }
                } 
            }
            
        iInsertingType = EMsgMediaUnknown;
        }
        
    TUniState oldState = Document()->UniState();
    DoInsertComplete2ndPhaseL();
    
    // Lit up display backlight in case it has gone off due to a long operation
    User::ResetInactivityTime();
    
    if( insertedAsAttachment && 
        ( oldState == Document()->UniState() ||  
          !iPopupNote ) )
        {   
        // The message type did not change or change popup note is not shown
        // so show the attachment added note
        ShowInformationNoteL( R_UNIEDITOR_QTN_UNIFIED_ATTACHMENT_ADDED, EFalse );
        }
    
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoInsertComplete2ndPhaseL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoInsertComplete2ndPhaseL()
    {
    RemoveWaitNote();
    
    delete iScreenClearer;
    iScreenClearer = NULL;
    
    CheckBodyForMessageTypeL();
    SetTitleL();
    InitNaviPaneL();
    ShowInsertErrorsL();
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ShowInsertErrorsL
// ---------------------------------------------------------
//
void CUniEditorAppUi::ShowInsertErrorsL()
    {
    TInt errRes( KErrNotFound );
    
    CArrayFixFlat<TInt>* errors = iInsertOperation->GetErrors();
    
    for ( TInt i = 0; i < errors->Count(); i++ )
        {
        switch ( errors->At( i ) )
            {
            case EUniProcessImgOutOfMemory:
            case EUniProcessImgOutOfDisk:
            case EUniProcessImgNotFound:
            case EUniProcessImgNonConformant:
            case EUniProcessImgScalingFailed:
            case EUniProcessImgCompressFailed:
            case EUniProcessImgCouldNotScale:
                {
                if( Document()->CreationModeUserChangeable() &&
                    Document()->CreationMode() == EMmsCreationModeRestricted )
                    {
                    SetErrorResource( errRes, R_UNIEDITOR_INFO_RMODE_SCALING_FAILED );
                    }
                else
                    {
                    SetErrorResource( errRes, R_UNIEDITOR_INFO_SCALING_FAILED );
                    }
                break;
                }
            case EUniInsertTooBig:
                {
                SetErrorResource( errRes, R_UNIEDITOR_INFO_OBJECT_TOO_BIG );
                break;
                }
            case EUniProcessImgUserAbort:
            case EUniInsertUserGuidedAbort:
                {
                return;
                }
            case EUniInsertNotSupported:
            case EUniInsertSlideChangeFailed:
                {
                if( Document()->CreationModeUserChangeable() &&
                    Document()->CreationMode() == EMmsCreationModeRestricted )
                    {
                    SetErrorResource( errRes, R_UNIEDITOR_INFO_RMODE_UNSUPPORTED_OBJECT );
                    }
                else
                    {
                    SetErrorResource( errRes, R_UNIEDITOR_INFO_UNSUPPORTED_OBJECT );
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
        
    if ( errRes != KErrNotFound )
        {
        ShowInformationNoteL( errRes, EFalse );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::EditorOperationEvent
//
// This is marked as non-leaving function since this is leaving with
// KLeaveExit only (also Exit() is not leaving even if it is leaving).
// ---------------------------------------------------------
//
void CUniEditorAppUi::EditorOperationEvent( TUniEditorOperationType aOperation,
                                            TUniEditorOperationEvent aEvent )
    {
    if ( iEditorFlags & EEditorExiting )
        {
        // Do not handle any event if we are exiting from editor.
		// rest values.
        iOptimizedFlow = EFalse;
        iSingleJpegImageProcessing = EFalse;
        return;
        }
    
    if ( aEvent == EUniEditorOperationComplete ||  
         aEvent == EUniEditorOperationError ||
         aEvent == EUniEditorOperationCancel )
        {
		// set iOptimizedFlow
        if( aOperation == EUniEditorOperationLaunch)
            {
            if(iLaunchOperation)
                {
                iOptimizedFlow = iLaunchOperation->IsOptimizedFlagSet();
                }
            }
		// sendui+jepg-> this required after image processing 
        if(!iOptimizedFlow)
            {
            DeactivateInputBlocker();
            iEditorFlags &= ~EMsgEditInProgress;   
            }
        if ( aEvent == EUniEditorOperationCancel &&
             aOperation != EUniEditorOperationSend )
            {
            // Operation by operation should be considered what is proper action 
            // in Cancel situation. Send operation handles the removing of wait note
            // correctly. When this is done with all operations operation checking
            // can be removed.
            EndActiveWait();
            RemoveWaitNote();
        
            delete iScreenClearer;
            iScreenClearer = NULL;
            }    
        }
        
    TRAPD( error, DoEditorOperationEventL( aOperation, aEvent ) );
    if ( error != KErrNone )
        {
		// error handling
        if(iOptimizedFlow)
            {
            DeactivateInputBlocker();
            iEditorFlags &= ~EMsgEditInProgress;   
            }
        iOptimizedFlow = EFalse;
        iSingleJpegImageProcessing = EFalse;
        
        // Handle operation handling error.
        if ( error == KLeaveExit )
            {
            // Leaving with KLeaveExit does not make function leavable. See Exit()
            User::Leave( error );
            }
        else 
            {
            iEikonEnv->HandleError( error );
            
            if ( aOperation == EUniEditorOperationLaunch )
                {
                Exit( EAknSoftkeyClose );
                }
            }
        }
    //sendui+jepg-> after first call to finallizelauch,rest
	// it, so that next call will cover the code finallizelaunch
	//as happened for other launch cases.
    iOptimizedFlow = EFalse;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoEditorOperationEventL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoEditorOperationEventL( TUniEditorOperationType aOperation,
                                               TUniEditorOperationEvent aEvent )
    {
    switch ( aOperation )
        {
        case EUniEditorOperationChangeSlide:
            {
            DoChangeSlideCompleteL();
            break;
            }
        case EUniEditorOperationInsert:
            {
            if ( aEvent == EUniEditorOperationProcessing )
                {
                if ( iEditorFlags & EInsertAddsSlide )
                    {
                    iScreenClearer = CAknLocalScreenClearer::NewLC( EFalse );
                    CleanupStack::Pop();
                    }
                }
            else
                {
                DoInsertCompleteL( aEvent );
                }
            break;
            }
        case EUniEditorOperationLaunch:
            {
            if ( aEvent == EUniEditorOperationComplete )
                {
                DoLaunchCompleteL();
                }
            else
                {
                DoDelayedExit( 0 );
                iEditorFlags |= EEditorClosing;
                }
            break;
            }
        case EUniEditorOperationSave:
            {
            DoSaveCompleteL();
            break;
            }
        case EUniEditorOperationSend:
            {
            DoSendCompleteL();
            break;
            }
        case EUniEditorOperationVCard:
            {
            DoVCardCompleteL();
            break;
            }
        case EUniEditorOperationPreview:
            {
            // Expecting 'processing' but in case of error, other states are aacceptable, too.
            RemoveWaitNote();
            break;
            }
        default:
            {
            // nothing to do
            break;
            }
        }
    }
    
// ---------------------------------------------------------
// CUniEditorAppUi::EditorOperationQuery
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::EditorOperationQuery( TUniEditorOperationType aOperation,
                                             TUniEditorOperationQuery aQuery )
    {
    TBool ret( EFalse );
    TRAP_IGNORE( ret = DoEditorOperationQueryL( aOperation, aQuery ) );
    return ret;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoEditorOperationQueryL
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::DoEditorOperationQueryL( TUniEditorOperationType /*aOperation*/,
                                                TUniEditorOperationQuery aQuery )
    {
    switch ( aQuery )
        {
        case EMEOQueryGuidedInsertLarge:
            {
            return ShowGuidedModeConfirmationQueryL( R_UNIEDITOR_QUEST_INSERT_LARGE );
            }
        case EMEOQueryGuidedObject:
            {
            return ShowGuidedModeConfirmationQueryL( R_UNIEDITOR_QUEST_GUIDED_OBJ );
            }
        default:
            {
            break;
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoChangeSlideCompleteL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoChangeSlideCompleteL()
    {
    EndActiveWait();
    RemoveWaitNote();
    
    if ( iChangeSlideOperation->GetErrors()->Count() )
        {
        delete iScreenClearer;
        iScreenClearer = NULL;
        
        ExitAndSaveL();
        }
    else
        {
        DoSetFocusL();
        
        delete iScreenClearer;
        iScreenClearer = NULL;
        
        CheckBodyForMessageTypeL();
        SetTitleL();
        InitNaviPaneL();
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoLaunchCompleteL
//
// First removes wait note from the screen. After this launch 
// relating note processing is done and if lanch should be aborted
// immediatelly performs delayed exit (i.e. starts idle timer
// to call Exit() function on it's callback). Rest of the launching
// related processing is done at the FinalizeLaunch function.
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoLaunchCompleteL()
    {
	//sendui+jepg -> this required after image processing 
    if(!iOptimizedFlow)
        {
        // Does no harm to call this even if no wait note is set.
        RemoveWaitNote();        
        }
		
    TBool shutDown( EFalse );
	// sendui+jepg-> this required after image processing 
    if(!iOptimizedFlow)
		{
        ShowLaunchNotesL( shutDown );
		}
    
    if ( shutDown )
        {
        // Avoid direct Exit(). It causes actually leaving with -1003. 
        // If non-conformant object is sent from sendui and user is not
        // willing to allow free moded Exit() was called previously and
        // caused CMmsEditorLaunchOperation to leave, because this function
        // is called inside RunL()
        // give time to show notes
        DoDelayedExit( KDelayedExitShort );
        iEditorFlags |= EEditorClosing;
        }
    else
        {
        FinalizeLaunchL();
     // check is Unieditor is launched from conversation by Addachemnt option 
        if( Document()->IsLaunchFromCvAttachment())
            {
            iFixedToolbar->ToolbarExtension()->SetShown(ETrue);
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoDelayedExit
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoDelayedExit( TInt aDelayTime )
    {
    iIdle->Cancel();
    iIdle->Start( aDelayTime,
                  aDelayTime, 
                  TCallBack( DelayedExitL, this ));
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DelayedExit
// ---------------------------------------------------------
//
TInt CUniEditorAppUi::DelayedExitL( TAny* aThis )
    {
    UNILOGGER_WRITE( "-> CUniEditorAppUi::DelayedExitL" );
    
    CUniEditorAppUi* editor = static_cast<CUniEditorAppUi*>( aThis );
    editor->ExitAndSaveL();
    
    UNILOGGER_WRITE( "<- CUniEditorAppUi::DelayedExitL" );
    
    return KErrNone;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoSaveCompleteL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoSaveCompleteL()
    {
    UNILOGGER_WRITE( "-> CUniEditorAppUi::DoSaveCompleteL" );

    if ( iWaitDialog )
        {
        // Processing continued at DialogDismissedL when wait note
        // has dismissed itself.
        iWaitDialog->ProcessFinishedL();
        }    
    else
        {
        DoSavingComplete2ndPhase();
        }
    
    UNILOGGER_WRITE( "<- CUniEditorAppUi::DoSaveCompleteL" );
    }
    
// ---------------------------------------------------------
// CUniEditorAppUi::DoSavingComplete2ndPhase
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoSavingComplete2ndPhase()
    {
    //TODO: Update prev save type!
    CMDXMLDocument* dom = iSaveOperation->DetachDom();
    if ( dom )
        {
        Document()->DataModel()->SetDom( dom );
        }

    EndActiveWait();
    
    if ( iEditorFlags & EEditorClosing )
        {
        if ( Document()->UniState() == EUniMms )
            {              
            CUniEditorPlugin* mmsPlugin = Document()->MmsPlugin();  
            TRAP_IGNORE(mmsPlugin->ConvertToL( Document()->Mtm().Entry().EntryId()));
            }
        else if ( Document()->UniState() == EUniSms )
            {
            CUniEditorPlugin* smsPlugin = Document()->SmsPlugin();  
            TRAP_IGNORE(smsPlugin->ConvertToL( Document()->Mtm().Entry().EntryId()));            
            }
            
        DoDelayedExit( 0 );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoSendCompleteL
//
// Sending operation completed. If wait note is still available
// (i.e. it is not cancelled from the screen by some external event)
// we call ProcessFinishedL so that prosessing is finished. Otherwise 
// we have to complete the operation by ourself.
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoSendCompleteL()
    {
    UNILOGGER_WRITE( "-> CUniEditorAppUi::DoSendCompleteL" );

    if ( iWaitDialog )
        {
        // Processing continued at DialogDismissedL when wait note
        // has dismissed itself.
        iWaitDialog->ProcessFinishedL();
        }    
    else
        {
        DoSendComplete2ndPhase();
        }
    
    UNILOGGER_WRITE( "<- CUniEditorAppUi::DoSendCompleteL" );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoSendComplete2ndPhase
//
// Performs the real send operation completing code.
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoSendComplete2ndPhase()
    {
    UNILOGGER_WRITE( "-> CUniEditorAppUi::DoSendComplete2ndPhase" );

    // Get errors from send operation
    CArrayFixFlat<TInt>* errors = iSendOperation->GetErrors();
    
    if ( errors->Count() > 0 )
        {
        iEikonEnv->HandleError( errors->At( 0 ) );
        iEditorFlags &= ~EEditorClosing;
        return;
        }
        
    DoDelayedExit( 0 );
    
    UNILOGGER_WRITE( "<- CUniEditorAppUi::DoSendComplete2ndPhase" );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoVCardCompleteL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoVCardCompleteL()
    {
    if ( iFixedToolbar )
        {
          iFixedToolbar->HideItem( EUniCmdFixedToolbarSend, EFalse, ETrue );
          iFixedToolbar->HideItem( EUniCmdFixedToolbarAddRecipient, EFalse, ETrue );        
          iFixedToolbar->HideItem( EUniCmdFixedToolbarInsert, EFalse, ETrue );    
        }
    
    iEditorFlags &= ~EToolbarHidden;
    
    TInt addedVCardCount = iVCardOperation->AddedVCardCount();
    TBool oldState = Document()->UniState();

    // Update navipane and type of message        
    CheckBodyForMessageTypeL();
    MsgLengthToNavipaneL();
    if (AknLayoutUtils::PenEnabled() )
        {
           iPeninputServer.UpdateAppInfo( KNullDesC, EAppIndicatorMsg );    
        }
    
    if ( addedVCardCount > 0 )
        {
        Document()->SetHeaderModified( ETrue );
        }
    
    if ( !( oldState != Document()->UniState() && iPopupNote ) )
        {
        // Popup note is not shown, so we can show error or attachment(s)
        // added note. When popup note is not shown, it doesn't matter
        // whether message type was changed or not.
        CArrayFixFlat<TInt>* errors = iVCardOperation->GetErrors();
        
        if ( errors->Count() )
            {
            for ( TInt i = 0; i < errors->Count(); i++ )
                {
                if ( errors->At( i ) == EUniInsertTooBig )
                    {
                    // Tell user that one or more vCards could not be inserted.
                    addedVCardCount > 0 ?
                        ShowInformationNoteL( R_UNIEDITOR_INFO_OBJECT_SEND_AS_TOO_BIG, EFalse ) :
                        ShowInformationNoteL( R_UNIEDITOR_INFO_OBJECT_TOO_BIG, EFalse );
                    break;
                    }
                }
            }
        else if ( addedVCardCount > 0 )
            {
            // Show attachment(s) added note if no errors.
            addedVCardCount == 1 ? 
                ShowInformationNoteL( R_UNIEDITOR_QTN_UNIFIED_ATTACHMENT_ADDED, EFalse ) :
                ShowInformationNoteL( R_UNIEDITOR_QTN_UNIFIED_ATTACHMENTS_ADDED, EFalse );
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::BeginActiveWait
// ---------------------------------------------------------
//
void CUniEditorAppUi::BeginActiveWait( CUniEditorOperation* aOperation )
    {
    if( iWait.IsStarted() )
        {
        // If there is data in recipient and body text fields,
        // presentation is created and read key is pressed in smil editor,
        // smil editor dialog is dismissed and change slide operation starts
        // in NewTemplateL() ( any kind error code is not supplied).
        // After that Exit comes to editor HandleCommandL, which would cause start of
        // save operation. Change slide operation must be terminated first.
        if ( iActiveOperation )
            {
            iActiveOperation->Cancel();
            }
        return;
        }
        
    iActiveOperation = aOperation;
    iWait.Start();
    }

// ---------------------------------------------------------
// CUniEditorAppUi::EndActiveWait
// ---------------------------------------------------------
//
void CUniEditorAppUi::EndActiveWait()
    {
    if( !iWait.IsStarted() )
        {
        return;
        }
        
    iWait.AsyncStop();
    iActiveOperation = NULL;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ObjectsAvailable
// ---------------------------------------------------------
//
TUint32 CUniEditorAppUi::ObjectsAvailable()
    {
    TUint32 objects = EUniNoneFlag;
    if ( BodyCtrl() && BodyCtrl()->TextContent().DocumentLength() )
        {
        // This flag is not currently used for anything.
        objects |= EUniTextFlag;
        }
    if ( iView->ControlById( EMsgComponentIdAudio ) )
        {
        objects |= EUniAudioFlag;
        }
    if ( iView->ControlById( EMsgComponentIdImage ) )
        {
        objects |= EUniImageFlag;
        }
    if ( iView->ControlById( EMsgComponentIdVideo ) )
        {
        objects |= EUniVideoFlag;
        }
    if ( iView->ControlById( EMsgComponentIdSvg ) )
        {
        objects |= EUniSvgFlag;
        }
    return objects;
    }


// ---------------------------------------------------------
// CUniEditorAppUi::DoUserAddHeadersL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserAddHeadersL()
    {
    if ( !iHeader )
        {
        return;
        }
    
    TInt headersVariation = iHeader->AddHeadersVariation();
    TInt headersConfig = 0;
    
    // Check which fields are visible:
    
    // TInt headersConfig = iHeader->AddHeadersConfigL();
    if ( iHeader->SubjectControl() )
        {
        headersConfig |= EUniFeatureSubject;
        }
    if ( iHeader->AddressControl( CUniBaseHeader::EHeaderAddressCc ) )
        {
        headersConfig |= EUniFeatureCc;
        }
    if ( iHeader->AddressControl( CUniBaseHeader::EHeaderAddressBcc ) )
        {
        headersConfig |= EUniFeatureBcc;
        }

    if ( IsHardcodedSms() && Document()->EmailOverSmsSupported() )
        { 
        // Locked SMS and EmailOverSms -> enable only subject field
        if ( headersVariation & EUniFeatureSubjectConfigurable )
            {
            headersVariation = EUniFeatureSubject;
            headersVariation |= EUniFeatureSubjectConfigurable;
            }
        else
            {
            // Should not happen as add headers command should be invisible at this case.
            headersVariation = EUniFeatureSubject;
            }
        }

    // Contains indexes of additional header flags in
    // headersIn array; KErrNotFound if not visible in the UI
    // index: 0 = CC, 1 = BCC, 3 = subject
    CArrayFixFlat<TInt>* indexes = new( ELeave ) CArrayFixFlat<TInt>( 3 );
    CleanupStack::PushL( indexes );
    
    CListBoxView::CSelectionIndexArray* headersOut = 
                                    new( ELeave ) CArrayFixFlat<TInt>( 3 );
    CleanupStack::PushL( headersOut );

    CListBoxView::CSelectionIndexArray* headersIn = 
                                    new( ELeave ) CArrayFixFlat<TInt>( 3 );
    CleanupStack::PushL( headersIn );
    
    CUniEditorAddHeaderDialog* dlg = new( ELeave ) CUniEditorAddHeaderDialog( headersOut );
    
    // Pushes the dialog into stack
    dlg->PrepareLC( R_UNIEDITOR_ADD_HEADERS_LIST_QUERY );

    // Order matters. See Pop below
    CDesCArrayFlat* headerStringsIn = new( ELeave ) CDesCArrayFlat( 3 );
    CleanupStack::PushL( headerStringsIn );
    
    TInt index( 0 );
    for ( TInt i = 0; i < 3; i++ )
        {
        TInt flag( EUniFeatureCc );
        TInt resource( R_UNIEDITOR_QTN_MSG_HEADERS_CC );
        
        if ( i == 1 )
            {
            flag = EUniFeatureBcc;
            resource = R_UNIEDITOR_QTN_MSG_HEADERS_BCC;
            }
        else if ( i == 2 )
            {
            flag = EUniFeatureSubjectConfigurable;
            resource = R_UNIEDITOR_QTN_MSG_HEADERS_SUBJECT;
            }
        
        if ( headersVariation & flag )
            {
            HBufC* fieldTemp = StringLoader::LoadLC( resource, iCoeEnv );
            
            // We need to add the "1\"
            HBufC* fieldBuf = HBufC::NewMaxLC( fieldTemp->Des().Length() + 2 );

            TPtr field = fieldBuf->Des();
            field.Copy( KItemStart );
            field.Append( fieldTemp->Des() );
            
            // FieldBuf will be owned by headersIn 
            headerStringsIn->AppendL( field );

            CleanupStack::PopAndDestroy( 2, fieldTemp );  //  + fieldBuf  
            
            if ( flag == EUniFeatureSubjectConfigurable )
                {
                flag = EUniFeatureSubject;
                }
            
            if ( headersConfig & flag )
                {   
                // Mark it selected
                dlg->ListBox()->View()->SelectItemL( index );
                headersIn->AppendL( ETrue );
                }
            else
                {
                headersIn->AppendL( EFalse );
                }
                
            indexes->AppendL( index );
            index++;
            }
        else
            {
            indexes->AppendL( KErrNotFound );
            }
        }
        
    dlg->SetItemTextArray( headerStringsIn );
    dlg->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( headerStringsIn );

    if ( dlg->RunLD() )
        {
        TInt newSelectionHeaders( 0 ); 
        TInt removeHeaders( 0 ); 
        TInt removeHeadersWithWarning( 0 ); 
        TInt addHeaders( 0 ); 

        TInt countItems( headersOut->Count() );

        // Check if the out selection is the same as in selection
        TBool theSame = ETrue;

        index = 0;
        for ( TInt i = 0; i < 3; i++ )
            {
            if ( indexes->At( i ) != KErrNotFound )
                { 
                // i was one of the choices available in the dialog
                if ( headersIn->At( index ) )
                    { 
                    // It was originally selected
                    if ( countItems < 1 )
                        {
                        theSame = EFalse;
                        }
                        
                    TBool foundIt = EFalse;
                    for ( TInt j = 0; j < countItems; j++ )
                        { 
                        // Lets see the current selection
                        if ( headersOut->At( j ) == index )
                            { 
                            // It's selected after the dialog
                            foundIt = ETrue;
                            }
                        }
                        
                    if ( !foundIt )
                        { 
                        // Originally it was selected so it's not the same selection anymore
                        theSame = EFalse;
                        }
                    }
                else
                    { 
                    // It was originally not selected
                    TBool foundIt = EFalse;
                    for ( TInt j = 0; j < countItems; j++ )
                        { 
                        // Lets see the current selection
                        if ( headersOut->At( j ) == index )
                            { 
                            // It's selected after the dialog
                            foundIt = ETrue;
                            }
                        }
                        
                    if ( foundIt )
                        { 
                        // Originally it was not selected so it's not the same selection anymore
                        theSame = EFalse;
                        }
                    }
                index++;                
                }
            }
            
        CleanupStack::PopAndDestroy( headersIn );

        if ( theSame )
            { 
            // no changes were done so don't proceed with this..
            // we return here so that we don't save incorrect configuration
            // it might happen for example if we have opened reply to all and there are 
            // unselected fields used in the replied message
            CleanupStack::PopAndDestroy( 2, indexes ); // + headersOut
            return;
            }
        
        // Indicates selected fields
        CArrayFixFlat<TInt>* indexesOut = new( ELeave ) CArrayFixFlat<TInt>( 3 );
        CleanupStack::PushL( indexesOut );
        
        indexesOut->AppendL( EFalse );
        indexesOut->AppendL( EFalse );
        indexesOut->AppendL( EFalse );
        
        for ( TInt i = 0; i < countItems; i++ )
            {
            TInt selected = headersOut->At(i);
            for ( TInt j = 0; j < 3; j++ )
                {
                if ( indexes->At( j ) == selected )
                    {
                    indexesOut->At( j ) = ETrue;
                    break;
                    }
                }
            }

        AddHeadersCollectStatisticsL( *indexesOut,
                                      headersVariation,
                                      newSelectionHeaders,
                                      removeHeaders,
                                      removeHeadersWithWarning,
                                      addHeaders );
                                      
        CleanupStack::PopAndDestroy( indexesOut );
        
        TInt countRemove = AddHeadersCount( removeHeadersWithWarning );
        
        TInt res ( 0 );
        if ( countRemove > 1 )
            {
            res = R_UNIEDITOR_QTN_MSG_QRY_HEADERS_REMOVE;
            }
        else if ( countRemove == 1 )
            {
            res = R_UNIEDITOR_QTN_MSG_QRY_HEADER_REMOVE;
            }
            
        if ( res )
            {
            // Confirmation specified for header removing
            if ( ShowConfirmationQueryL( res ) )
                {
                // Remove all headers 
                iHeader->AddHeadersDeleteL( removeHeaders, ETrue );
                Document()->SetHeaderModified( ETrue );
                SetAddressSize();
                SetSubjectSize();
                }
            else
                {
                // Remove empty headers
                removeHeaders &= ( ~removeHeadersWithWarning );
                
                if ( removeHeaders )
                    { 
                    // Still something left to delete
                    iHeader->AddHeadersDeleteL( removeHeaders, ETrue );
                    }
                    
                // But keep the ones that have some content
                newSelectionHeaders |= removeHeadersWithWarning;
                }
            }
        else if ( removeHeaders )
            {
            // Remove one or more headers but none of the removed do not contain any data
            iHeader->AddHeadersDeleteL( removeHeaders, EFalse );            
            }

        iHeader->SaveAddHeadersConfigL( newSelectionHeaders );
        
        // Add new headers
        if ( addHeaders )
            {
            iHeader->AddHeadersAddL(addHeaders);
            }    
            
         // Change focus
        if ( addHeaders || removeHeaders )
            {
            iNextFocus = EMsgComponentIdNull;
            
            // Add headers override removing
            if ( addHeaders )
                {
                if ( addHeaders & EUniFeatureCc )
                    {
                    iNextFocus = EMsgComponentIdCc;
                    }
                else if ( addHeaders & EUniFeatureBcc )
                    {
                    iNextFocus = EMsgComponentIdBcc;
                    }
                else if ( addHeaders & EUniFeatureSubject )
                    {
                    iNextFocus = EMsgComponentIdSubject;
                    }
                }
            else
                {
                CUniEditorHeader::THeaderFields start( CUniEditorHeader::EHeaderAddressBcc );
                
                if ( removeHeaders & EUniFeatureSubject )
                    {
                    start = CUniEditorHeader::EHeaderAddressBcc;
                    }                
                else if ( removeHeaders & EUniFeatureBcc )
                    {
                    start = CUniEditorHeader::EHeaderAddressCc;
                    }
                else if ( removeHeaders & EUniFeatureCc )
                    {
                    start = CUniEditorHeader::EHeaderAddressTo;
                    }
                    
                // Search for the first existing header
                for ( TInt i = start; i >= CUniEditorHeader::EHeaderAddressTo; i-- )
                    {
                    if ( iHeader->AddressControl( static_cast<CUniBaseHeader::THeaderFields> ( i ) ) )
                        {
                        iNextFocus = iHeader->AddressControlId( 
                                                    static_cast<CUniBaseHeader::THeaderFields> ( i ) );
                        break;
                        }
                    }
                }
                
            if ( ToCtrl() )
                {
                DoSetFocusL();
                }
            else
                { 
                DoUserChangeSlideL( 0 );
                }
                
            CheckHeaderForMessageTypeL();
            
            // Also iSmsSubjectLength needs updating..
            CheckBodyForMessageTypeL();
            MsgLengthToNavipaneL();
            }
        }
    else
        {
        CleanupStack::PopAndDestroy( headersIn );
        }
        
    CleanupStack::PopAndDestroy( 2, indexes ); // + headersOut
    }
                                            

// ---------------------------------------------------------
// CUniEditorAppUi::AddHeadersRemovedWithDataFlags
// ---------------------------------------------------------
//
void CUniEditorAppUi::AddHeadersCollectStatisticsL( CArrayFix<TInt>& aIndexesOut, 
                                                    TInt aVariation,
                                                    TInt& aNewSelectionFlags,
                                                    TInt& aRemoveFlags,
                                                    TInt& aRemoveWarningFlags,
                                                    TInt& aAddFlags ) const
    {        
    // now we know selected ones 
    for ( TInt i = 0; i < 3; i++ )  // maximum three three
        {
        if (i == 0 || i == 1 )
            {
            CMsgAddressControl* control = NULL;                
            TInt flag = EUniFeatureCc;
            if ( i == 0 )
                {
                control = iHeader->AddressControl( 
                    static_cast<CUniBaseHeader::THeaderFields> 
                        (CUniBaseHeader::EHeaderAddressCc) );
                }
            else if ( i == 1 )
                {
                flag = EUniFeatureBcc;
                control = iHeader->AddressControl( 
                    static_cast<CUniBaseHeader::THeaderFields> 
                        (CUniBaseHeader::EHeaderAddressBcc) );
                }
            
            if ( !aIndexesOut[i] )
                {
                if ( control )
                    {
                    aRemoveFlags |= flag;
                    CMsgRecipientArray* recipients = control->GetRecipientsL();
                    if (    recipients 
                       &&   recipients->Count() )
                        {
                        aRemoveWarningFlags |= flag;
                        }
                    // else - control, no recipients, no selection => remove
                    }
                // else - no control, no selection => status quo
                }
            else
                {
                aNewSelectionFlags |= flag;
                if ( !control )
                    {
                    // no control, selection
                    aAddFlags |= flag;
                    }
                // else - control and selection => status quo
                }
            }
        else if ( i== 2 )
            {
            if( aVariation & EUniFeatureSubject && aVariation & EUniFeatureSubjectConfigurable )
                { // OK, subject field supported and configured
                CMsgExpandableControl* subject = iHeader->SubjectControl();
                if ( !aIndexesOut[i] )
                    {
                    if ( subject )
                        {
                        aRemoveFlags |= EUniFeatureSubject;
                        if ( subject->TextContent().DocumentLength() )
                            {
                            aRemoveWarningFlags |= EUniFeatureSubject;
                            }
                        // else - control, no subject text, no selection => remove
                        }
                    // else - no control, no selection => status quo
                    }
                else
                    {
                    aNewSelectionFlags |= EUniFeatureSubject;
                    if ( !subject )
                        {
                        // no control, selection
                        aAddFlags |= EUniFeatureSubject;
                        }
                    // else - control and selection => status quo
                    }
                }
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::AddHeadersCount
// ---------------------------------------------------------
//
TInt CUniEditorAppUi::AddHeadersCount( TInt aFlag ) const
    {
    TInt count( 0 );
    if ( aFlag & EUniFeatureCc )
        {
        count++;
        }
    if ( aFlag & EUniFeatureBcc )
        {
        count++;
        }
    if ( aFlag & EUniFeatureSubject )
        {
        count++;
        }
    return count;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::SetCursorPositionsForInsertL
// ---------------------------------------------------------
//
void CUniEditorAppUi::SetCursorPositionsForInsertL()
    {
    // Set body control cursor position, so that scrolling after
    // insert goes ok.
    if ( BodyCtrl() )
        {
        TInt cursorPos = iSmilModel->Layout() == EUniImageFirst ? 0 : 
                                                                  BodyCtrlEditor()->TextLength(); 
        BodyCtrl()->SetCursorPosL( cursorPos );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUserRemoveMediaL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUserRemoveMediaL( TMsgControlId aMediaControlId,
                                          TUniRegion aRegion )
    {
    __ASSERT_DEBUG( Document()->DataModel()->SmilType() == EMmsSmil, Panic( EUniIllegalSmilType ) );

    CMsgBaseControl* ctrl = iView->ControlById( aMediaControlId );
    CUniObject* obj = iSmilModel->GetObject( Document()->CurrentSlide(), aRegion );

    __ASSERT_DEBUG( ( ctrl && obj ), Panic( EUniNullPointer ) );
    __ASSERT_DEBUG( Document()->DataModel()->SmilType() == EMmsSmil, Panic( EUniIllegalSmilType ) );

    TUint32 objects = ObjectsAvailable(); 

    TParsePtrC fileN( obj->MediaInfo()->FullFilePath() );

    HBufC* queryText = StringLoader::LoadLC( R_UNIEDITOR_QUEST_REMOVE_COMMON, fileN.NameAndExt() );

    if ( ShowConfirmationQueryL( *queryText ) ) 
        {  
        iSmilModel->RemoveObjectL( Document()->CurrentSlide(), obj );
        Document()->SetBodyModified( ETrue );

        TBool removeCtrlFocused = ctrl->IsFocused();

        // Delete icon
        iView->DeleteControlL( ctrl->ControlId() );
        
        MsgLengthToNavipaneL();

        // Change focus to the beginning that scroll bar is updated
        // and text editor area removes extra lines
        if ( ToCtrl() )
            {
            iView->SetFocus( EMsgComponentIdTo );
            }
        else if ( iView->ControlById( EMsgComponentIdAudio ) )
            {
            //Audio is always first
            iView->SetFocus( EMsgComponentIdAudio );
            }
        else if ( iSmilModel->Layout() == EUniImageFirst &&  
                  ImageCtrl() )
            {
            // should not be need to update focus anymore
            iView->SetFocus( EMsgComponentIdImage );
            }    
        else 
            {
            // should not be need to update focus anymore
            iView->SetFocus( EMsgComponentIdBody );
            }
 
        // Put focus back to right place
        CMsgBaseControl* focusedControl = iView->FocusedControl();
        if ( focusedControl )
            {
            if ( removeCtrlFocused &&  
                 focusedControl->ControlId() != EMsgComponentIdImage &&  
                 iSmilModel->Layout() == EUniImageFirst &&  
                 ImageCtrl() )
                {
                iView->SetFocus( EMsgComponentIdImage );
                }
            else if ( removeCtrlFocused &&        
                      aMediaControlId == EMsgComponentIdAudio && 
                      iSmilModel->Layout() == EUniImageFirst && 
                      ImageCtrl() )
                {
                iView->SetFocus( EMsgComponentIdImage );
                }
            else if ( focusedControl->ControlId() != EMsgComponentIdBody )
                {
                CMsgBodyControl* bodyCtrl = BodyCtrl();
                if ( bodyCtrl && 
                     aRegion == EUniRegionImage && 
                     ( objects & EUniVideoFlag | removeCtrlFocused ) )
                    { 
                    if ( iSmilModel->Layout() == EUniImageFirst )
                        {
                        // If video was removed, always place the cursor in the end
                        bodyCtrl->SetCursorPosL( 0 );
                        }
                    else
                        {
                        // If video was removed, always place the cursor in the end
                        bodyCtrl->SetCursorPosL( bodyCtrl->Editor().TextLength() );
                        }
                    }
                
                // Needs to be done after cursor setting to ensure that view is 
                // showing the correct position.
                iView->SetFocus( EMsgComponentIdBody );
                }

            if ( focusedControl->ControlId() == EMsgComponentIdImage )
                {
                // Set focus here so that imagecontrol will draw navipane if focused.
                iView->FocusedControl()->SetFocus( ETrue, EDrawNow );
                }
            }
        }
        
    CleanupStack::PopAndDestroy( queryText );
    if(!ObjectsAvailable() && Document()->CurrentSlide())
        {
        DoRemoveSlideL();
        Document()->SetBodyModified( ETrue ); 
        }   
    CheckBodyForMessageTypeL();
    MsgLengthToNavipaneL();
    }

// ---------------------------------------------------------
// CUniEditorAppUi::RemoveCurrentTextObjectL
// ---------------------------------------------------------
//
void CUniEditorAppUi::RemoveCurrentTextObjectL()
    {
    __ASSERT_DEBUG( Document()->DataModel()->SmilType() == EMmsSmil, Panic( EUniIllegalSmilType ) );
    
    if ( ShowConfirmationQueryL( R_UNIEDITOR_QUEST_REMOVE_TEXT ) ) 
        {
        if ( BodyCtrl() || 
             BodyCtrl()->TextContent().DocumentLength() )
            {
            BodyCtrl()->Reset();
            
            Document()->SetBodyModified( ETrue );
            
            UpdateSmilTextAttaL();
            }        
        if((Document()->UniState() == EUniMms) &&(!ObjectsAvailable() && Document()->CurrentSlide()))
            {
            DoRemoveSlideL();
            Document()->SetBodyModified( ETrue ); 
            }        
        CheckBodyForMessageTypeL();
        MsgLengthToNavipaneL();
        SetOrRemoveMaxSizeInEdwin();
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::MsgAsyncControlStateChanged
// ---------------------------------------------------------
//
void CUniEditorAppUi::MsgAsyncControlStateChanged( CMsgBaseControl& /*aControl*/,
                                                   TMsgAsyncControlState /*aNewState*/,
                                                   TMsgAsyncControlState /*aOldState*/ )
    {
    // OK to leave this empty, needed only in Viewer side
    }

// ---------------------------------------------------------
// CUniEditorAppUi::MsgAsyncControlResourceChanged
// ---------------------------------------------------------
//
void CUniEditorAppUi::MsgAsyncControlResourceChanged( CMsgBaseControl& aControl, TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        if ( aControl.ControlType() == EMsgImageControl )
            {
            CMsgImageControl& imageControl = static_cast<CMsgImageControl&>( aControl );
            
            if ( imageControl.IconBitmapId() == EMbmUniutilsQgn_graf_mms_unedit ||
                 imageControl.IconBitmapId() == EMbmUniutilsQgn_graf_mms_edit )
                {
                // 3GPP icons layout setting needs to be handled by ourself
                TAknLayoutRect iconLayout;
                iconLayout.LayoutRect( MsgEditorCommons::MsgDataPane(),
                                       AknLayoutScalable_Apps::msg_data_pane_g4().LayoutLine() );
                
                TRAP_IGNORE( imageControl.SetIconSizeL( iconLayout.Rect().Size() ) );
                }
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::HandleResourceChangeL
// ---------------------------------------------------------
//
void CUniEditorAppUi::HandleResourceChangeL( TInt aType )
    {
    // Base class call must be first
    CMsgEditorAppUi::HandleResourceChangeL(aType);
    
    if ( iHeader )
        {
        iHeader->HandleResourceChange( aType );
        }
    
    if ( aType == KEikDynamicLayoutVariantSwitch && IsLaunched() )
        { 
        // Resize the title icon
        CreateFixedToolbarL( EFalse );
        
        if( !( iEditorFlags & EDoNotUpdateTitlePane ) )
            {
            UpdateToolbarL();

            SetTitleIconsSizeL();
            
            // Redraw it again
            SetTitleL();
            }        
        }
    else if ( aType == KAknsMessageSkinChange )
        {
        TParse fileParse;
        fileParse.Set( KUniUtilsMBMFileName, &KDC_APP_BITMAP_DIR, NULL );
        
        delete iIconSms;
        iIconSms = NULL;
        iIconSms = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                KAknsIIDQgnPropMceSmsTitle,
                fileParse.FullName(),
                EMbmUniutilsQgn_prop_mce_sms_title,
                EMbmUniutilsQgn_prop_mce_sms_title_mask );

        delete iIconMms;
        iIconMms = NULL;
        iIconMms = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                        KAknsIIDQgnPropMceMmsTitle,
                        fileParse.FullName(),
                        EMbmUniutilsQgn_prop_mce_mms_title,
                        EMbmUniutilsQgn_prop_mce_mms_title_mask );
        
        SetTitleIconsSizeL();
        
        if ( IsLaunched() && !( iEditorFlags & EDoNotUpdateTitlePane ) )
            {
            SetTitleL();
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::IsBodySmsCompatibleL
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::IsBodySmsCompatibleL( TBool aInAutoMode /*= ETrue*/ )
    {
    CUniEditorDocument* doc = Document();

    CUniDataModel* model = doc->DataModel();

    if ( Document()->DataModel()->SmilType() != EMmsSmil )
        { // There's 3GPP presentation or something else..
        return EFalse;
        }

    // This sets iSmsLength and possible unicode character mode
    CheckSmsSizeAndUnicodeL();

    if( model->ObjectList().Count() == 0 &&
        model->AttachmentList().Count() == 1 &&
        iSmilModel->SlideCount() == 1 &&
        iSmsSubjectLength == 0)
        {
        CUniObject* obj = model->AttachmentList().GetByIndex( 0 );
        if ( obj &&  
             obj->MimeType().Length() > 0 &&   
             ( obj->MimeType().CompareF( KMsgMimeVCard ) == 0 ||  
#if defined (__I_CAL_SUPPORT) && defined (RD_MESSAGING_ICAL_IMPORT)
               obj->MimeType().CompareF( KMsgMimeICal ) == 0 ||
#endif
               obj->MimeType().CompareF( KMsgMimeVCal ) == 0 ) )
            { 
            // There's only either VCard, VCal or ICal if it is supported.
            iSmsBodyLength = obj->Size();
            
            TInt maxParts = doc->MaxSmsParts();
             if( !aInAutoMode )
                { // Checking if the message fits in "absolute maximum"
                maxParts = doc->AbsoluteMaxSmsParts();
                }
            TInt lengthOne( 0 );
            TInt lengthMany( 0 );    
            doc->SmsCharacterLimits( lengthOne, lengthMany );
            return (iSmsBodyLength > lengthMany * maxParts )?  EFalse :  ETrue;
            }
        }
    
    if( model->ObjectList().Count() > 1 ||
        model->AttachmentList().Count() > 0 ||
        iSmilModel->SlideCount() > 1 )
        { // If more than 1 object or an attachment or a slide -> MMS
        return EFalse;
        }
    // OK, there's just one object
    
    if( model->ObjectList().Count() == 1 &&
        model->ObjectList().GetByIndex(0)->MediaType() != EMsgMediaText )
        { // Just one object and it's not text -> MMS
        return EFalse;
        }
        
    // Lets check the length of the body
    if( !(iEditorFlags & ESubjectOkInSms ) && iSmsSubjectLength > 0 )
        {
        return EFalse;
        }
    
    TInt lengthOne( 0 );
    TInt lengthMany( 0 );
    
    doc->SmsCharacterLimits( lengthOne, lengthMany );

    TInt maxLen = lengthOne;

    TInt maxParts = doc->MaxSmsParts();
    
    if( !aInAutoMode )
        { // Checking if the message fits in "absolute maximum"
        maxParts = doc->AbsoluteMaxSmsParts();
        }
  
    if( maxParts > 1 )
        {
        maxLen = maxParts*lengthMany;
        }
    
    if ( doc->AbsoluteMaxSmsCharacters() > 0 )
        {
        maxLen = doc->AbsoluteMaxSmsCharacters();
        }
    
    TInt emailAddrLen = iHeader->LongestEmailAddress();
    
    if( emailAddrLen )
        {
        emailAddrLen++; // one extra char needed by separator char
        }

    if( iSmsBodyLength + iSmsSubjectLength + emailAddrLen <= maxLen )
        {
        //Korean Req: 415-5434
        //get the size limit in bytes
        TInt warnCharLength = doc->SmsSizeWarningBytes();

        if( warnCharLength > 0 )
            {
            //convert the size limit w.r.t characters based on encoding type
            if ( doc->UnicodeCharacterMode() )
                {
                warnCharLength =  warnCharLength / 2 ;
                }
            else
                {
                warnCharLength = (warnCharLength * 8) / 7;
                }
            
            if( iSmsBodyLength + iSmsSubjectLength + emailAddrLen > warnCharLength )
                {
                //show "limit exceed" note if not shown already
                if( !( iEditorFlags & ESmsSizeWarningNoteShown ) )
                    {
                    //set the bit to indicate limit exceeded and note shown
                    iEditorFlags |= ESmsSizeWarningNoteShown;
                    if( iPopupNote )
                        {
                        iPopupNote->SetTextL( iPopupSmsSizeAboveLimitBuffer->Des() );
                        iPopupNote->ShowInfoPopupNote();
                        }
                    }
                }
            else
                {
                //show "back to below limit" note if not shown already(only in case of size transiton from above limit to below limit)
                if( iEditorFlags & ESmsSizeWarningNoteShown )
                    {
                    //reset the bit to indicate size is below limit and note for transition is showed
                    iEditorFlags &= ~ESmsSizeWarningNoteShown;
                    if( iPopupNote )
                        {
                        iPopupNote->SetTextL( iPopupSmsSizeBelowLimitBuffer->Des() );
                        iPopupNote->ShowInfoPopupNote();
                        }                
                    }
                }          
            }
        
        return ETrue;
        }
  
    // text (body + possible subject) is too long -> body is not sms compatible
    return EFalse;  
    }

// ---------------------------------------------------------
// CUniEditorAppUi::CheckSmsSizeAndUnicodeL
// ---------------------------------------------------------
//
void CUniEditorAppUi::CheckSmsSizeAndUnicodeL()
    {
    UNILOGGER_WRITE_TIMESTAMP("CUniEditorAppUi::CheckSmsSizeAndUnicodeL Start <<<---- ");
    CUniEditorDocument* doc = Document();

    if( iNLTFeatureSupport )
        {
        //Turkish SMS-PREQ2265 specific
        TInt numOfPDUs = 0, numOfRemainingChars = 0, subjectLength = 0;
        TBool unicodeMode = EFalse;
        TSmsEncoding alternativeEncodingType;
        CUniEditorPlugin* plugin = NULL;
       
        plugin = SmsPlugin();
        if ( plugin )
            { 
            HBufC* editorTxt = NULL;
            
            if ( BodyCtrlEditor() )
                {
                editorTxt = BodyCtrlEditor()->GetTextInHBufL();
                }

            //Concatenate subject buffer and body buffer before calculating PDU info.
            if ( SubjectCtrlEditor() )
                {
                HBufC* subjectTxt = SubjectCtrlEditor()->GetTextInHBufL();
                if( subjectTxt )
                    {
                    if( editorTxt )
                        {
                        CleanupStack::PushL( subjectTxt );
                        //If both, body text and suject text is present
                        //Reallocate editor text to include subject buffer and additional chars for subject 
                        TInt reallocLength;
                        reallocLength = editorTxt->Length() + subjectTxt->Length() + KUniEdExtraLenCausedBySubject;
                        editorTxt = editorTxt->ReAllocL(reallocLength);
                        
                        //Modify the editor text to insert subject buffer and additional chars at the begining
                        //Buffer Format is:: "(subject...)body..."
                        TPtr editorBuffPtr = editorTxt->Des();
                        subjectLength = 0;
                        editorBuffPtr.Insert(subjectLength, _L("("));
                        subjectLength += 1;
                        editorBuffPtr.Insert(subjectLength, *subjectTxt);
                        subjectLength += subjectTxt->Length();
                        editorBuffPtr.Insert(subjectLength, _L(")"));
                        subjectLength += 1;
                        //total subject length is actual subject + extra chars '(' and ')'
                        CleanupStack::PopAndDestroy( subjectTxt );                    
                        }
                    else
                        {
                        //if only subject text is present                   
                        //Modify subject buffer directly                    
                        //Reallocate subject text to include subject buffer and additional chars for subject 
                        subjectLength = subjectTxt->Length() + KUniEdExtraLenCausedBySubject;
                        subjectTxt = subjectTxt->ReAllocL(subjectLength);

                        //Modify the editor text to insert additional chars at the begining and end 
                        //Buffer Format is:: "(subject...)"
                        TPtr subjectBuffPtr = subjectTxt->Des();
                        subjectBuffPtr.Insert(0, _L("("));
                        //total subject length is actual subject + extra chars '(' and ')'
                        subjectBuffPtr.Insert(subjectLength - 1, _L(")"));
                        //Use subject buffer as editor buffer
                        editorTxt = subjectTxt;
                        }
                    }
                }
            
            if ( editorTxt )
                {
                TPtr inputBuff = editorTxt->Des();
                TInt buffLength = editorTxt->Length();
                TInt prevBuffLength = 0;
                //replace enter char(enter will be downgraded anyways) with space, before calculating PDU info.
                //This char will be later replaced with line feed or paragraph separator
                TBuf<KSmsEdPDUInfoCalcReplaceCharacterCount> replaceChars;
                replaceChars.Zero();
                replaceChars.Append( KSmsEnterCharacter );
                replaceChars.Append( KSmsDownwardsArrowLeft );
                AknTextUtils::ReplaceCharacters(
                    inputBuff, 
                    replaceChars, 
                    KSmsEdUnicodeLFSupportedByBasicPhones );
                if( iPrevBuffer )
                    {
                    prevBuffLength = iPrevBuffer->Length();
                    if(( prevBuffLength == buffLength )&&(!iSettingsChanged) )
                    {
                        UNILOGGER_WRITE_TIMESTAMP("CUniEditorAppUi::CheckSmsSizeAndUnicodeL Compare Start <<<---- ");                        
                        if( iPrevBuffer->Compare(*editorTxt) == 0 )
                            {
                            UNILOGGER_WRITE_TIMESTAMP("CUniEditorAppUi::CheckSmsSizeAndUnicodeL Compare TRUE End ---->>> ");                        
                            /* Identical buffer, previously calculated values are good enough
                             * Hence do nothing. 
                             */
                            delete iPrevBuffer;
                            iPrevBuffer = editorTxt;
                            return;
                    }
                        UNILOGGER_WRITE_TIMESTAMP("CUniEditorAppUi::CheckSmsSizeAndUnicodeL Compare FALSE End ---->>> ");                        
                        }
                    }
                //This char will be later replaced with line feed or paragraph separator
	          if( buffLength <= prevBuffLength )
                    {
        	  	//Reset the settings back, in case if any unicode/turkish chars were entered and erased 
            	       	//or if entry type is T9 method(multiple key presses to get different chars)
    	               	plugin->SetEncodingSettings(EFalse, doc->AlternativeEncodingType(), doc->CharSetSupport());
                    }                
                //Call the plugin SMS adaptation API to get PDU Info
               TRAPD( err, plugin->GetNumPDUsL( inputBuff, numOfRemainingChars, numOfPDUs, unicodeMode, alternativeEncodingType) );
               if ( err == KErrOverflow )
                    {
                            iSmsBodyLength = buffLength;
                            delete iPrevBuffer;
                            iPrevBuffer = editorTxt;
                            return;                          
                    }
                else if ( err != KErrNone )
                    {
                	        User::Leave(err);
                	}
                	  				     
                //save current buffer 
                delete iPrevBuffer;
                iPrevBuffer = editorTxt;
                iSettingsChanged = EFalse;
                }
            else
                {
                //Reset the settings back, in case if any unicode chars were entered and erased
                plugin->SetEncodingSettings(EFalse, doc->AlternativeEncodingType(), doc->CharSetSupport());
                // Bodytext is zero -> check input language
                delete iPrevBuffer;
                iPrevBuffer = NULL;
                TInt inputLang = iAvkonEnv->SettingCache().InputLanguage();
                switch ( inputLang )
                    {
                    case ELangArabic:
                    case ELangHebrew:
                    case ELangThai:
                    case ELangVietnamese:
                    case ELangFarsi:
                    case ELangHindi:
                    case ELangUrdu:
                    case ELangRussian:
                    case ELangBulgarian:
                    case ELangUkrainian:
                    case ELangTaiwanChinese:
                    case ELangHongKongChinese:
                    case ELangPrcChinese:
                    case ELangJapanese:
                        {
                        // If the InputLanguage is one of above,
                        // we must check the current input mode too 
                        if ( BodyCtrlEditor() )
                            {
                            if ( BodyCtrlEditor()->AknEditorCurrentInputMode() == 
                                                                EAknEditorHalfWidthTextInputMode )
                                {
                                //Unicode Not required. Characters are within western text set.
                                unicodeMode = EFalse;
                                }
                            else
                                {
                                //Unicode Mode
                                unicodeMode = ETrue;
                                }       
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
        TInt lengthOne = 0;
        TInt lengthMany = 0;

        doc->SetUnicodeCharacterMode( unicodeMode );    
        doc->SmsCharacterLimits( lengthOne, lengthMany );

        if(numOfPDUs == 0)
            {
            //Empty buffer, need to calculate correct values of "num of PDUs" and "remaining characters" on our own.
            numOfPDUs = 1;
            numOfRemainingChars = lengthOne;
            }
        iNumOfPDUs = numOfPDUs;
        iCharsLeft = numOfRemainingChars;

        if(iNumOfPDUs == 1)
            {
            iSmsBodyLength = lengthOne - iCharsLeft - subjectLength;
            }
        else
            {
            iSmsBodyLength = lengthMany*iNumOfPDUs - iCharsLeft - subjectLength;
            }
        
        iSmsSubjectLength = subjectLength;
        
        }
    else
        {
        TInt extendedChars = 0;
        TInt extendedBodyChars = 0;
        TInt extendedSubjectChars = 0;
        TBool westernText = ETrue;

        CCnvCharacterSetConverter* conv = doc->CharConverter();    
   
        if ( conv )
            { 
            for( TInt index = 0; index < KUniEdNumberOfEditors; index++ )
                {
                HBufC* editorTxt = NULL;
                extendedChars = 0;
                
                if ( index == 0 )
                    {
                    if ( BodyCtrlEditor() )
                        {
                        editorTxt = BodyCtrlEditor()->GetTextInHBufL();
                        }
                    }
                else
                    {
                    if ( SubjectCtrlEditor() )
                        {
                        editorTxt = SubjectCtrlEditor()->GetTextInHBufL();
                        }
                    }

                if ( editorTxt )
                    {
                    CleanupStack::PushL( editorTxt );
                    
                    TPtr string = editorTxt->Des();
                    TUint uChar;
                    TBuf8<KUnicodeCheckChars> notUsed;

                    TPtrC remainderOfInputString( string );
                    
                    for ( TInt i = 0; i < remainderOfInputString.Length(); i++ )
                        {
                        uChar = TUint( remainderOfInputString[i] );
                        switch ( uChar )
                            {
                            case KUniEdLeftSquareBracket:
                            case KUniEdReverseSolidus:
                            case KUniEdRightSquareBracket:
                            case KUniEdCircumflexAccent:
                            case KUniEdLeftCurlyBracket:
                            case KUniEdVerticalLine:
                            case KUniEdRightCurlyBracket:
                            case KUniEdTilde:
                            case KUniEdEuroSymbol:
                                {
                                extendedChars++;
                                break;
                                }
                            default:
                                {
                                break;
                                }
                            }
                        }
                    
                    // This is needed in case there's KSmsDownwardsArrowLeft in the end and no other unicode chars
                    TBool unicodeWasInUse = EFalse;
                    
                    while ( remainderOfInputString.Length() )
                        {
                        TInt numberOfUnconvertibleCharacters = 0;
                        const TInt returnValue = conv->ConvertFromUnicode( notUsed,
                                                                           remainderOfInputString, 
                                                                           numberOfUnconvertibleCharacters );

                        if ( numberOfUnconvertibleCharacters > 0 )
                                {
                                for ( TInt i = 0; 
                                      i < remainderOfInputString.Length() && numberOfUnconvertibleCharacters > 0;
                                      i++ )
                                    {
                                    uChar = TUint(remainderOfInputString[i]);
                                    if ( uChar == KSmsDownwardsArrowLeft )
                                        {
                                        // these will be converted to paragraph separators later
                                        numberOfUnconvertibleCharacters--; 
                                        if ( doc->UnicodeCharacterMode() )
                                            {
                                            unicodeWasInUse = ETrue;
                                            }
                                        }
                                    }
                                }
                                
                        if ( returnValue < 0 || 
                             numberOfUnconvertibleCharacters > 0 || 
                             ( unicodeWasInUse && 
                               numberOfUnconvertibleCharacters == 0 ) ) 
                            {
                            // if there was an error in trying to do the conversion, or if there was an
                            // unconvertible character (e.g. a Chinese character)
                            westernText = EFalse;
                            break;
                            }
                            
                        remainderOfInputString.Set( remainderOfInputString.Right( returnValue ) );
                        }

                    if ( index == 0 )
                        {
                        extendedBodyChars = extendedChars;
                        }
                    else
                        {
                        extendedSubjectChars = extendedChars;
                        }
                        
                    CleanupStack::PopAndDestroy( editorTxt );
                    }
                else if ( index == 0 )
                    { 
                    // Bodytext is zero -> check input language
                    TInt inputLang = iAvkonEnv->SettingCache().InputLanguage();
                    switch ( inputLang )
                        {
                        case ELangArabic:
                        case ELangHebrew:
                        case ELangThai:
                        case ELangVietnamese:
                        case ELangFarsi:
                        case ELangHindi:
                        case ELangUrdu:
                        case ELangRussian:
                        case ELangBulgarian:
                        case ELangUkrainian:
                        case ELangTaiwanChinese:
                        case ELangHongKongChinese:
                        case ELangPrcChinese:
                        case ELangJapanese:
                            {
                            // If the InputLanguage is one of above,
                            // we must check the current input mode too 
                            if ( BodyCtrlEditor() )
                                {
                                if ( BodyCtrlEditor()->AknEditorCurrentInputMode() == 
                                                                    EAknEditorHalfWidthTextInputMode )
                                    {
                                    westernText = ETrue;
                                    }
                                else
                                    {
                                    westernText = EFalse;
                                    }       
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
                // Now we have westernText and extendedChars set
            }
            
        if ( westernText )
            {
            // Might not infact change the unicode character mode
            // if maximum SMS character limit is set that forces
            // the mode to be always unicode.
            doc->SetUnicodeCharacterMode( EFalse );
            }
        else
            {
            // Unicode mode
            doc->SetUnicodeCharacterMode( ETrue );
            
            // We need unicode so discard extended chars
            extendedBodyChars = 0; 
            extendedSubjectChars = 0;
            }

        static_cast<CUniSmsPlugin*>( SmsPlugin() )->SetUnicodeMode( doc->UnicodeCharacterMode() );
        
        if ( BodyCtrlEditor() )
            {
            iSmsBodyLength = BodyCtrlEditor()->TextLength() + extendedBodyChars;
            }
        else
            {
            iSmsBodyLength = 0;
            }
            
        iSmsSubjectLength = 0;
        
        if( SubjectCtrlEditor() && 
            SubjectCtrlEditor()->TextLength() )
            { 
            // EmailOverSms is supported if when end up here
            iSmsSubjectLength = SubjectCtrlEditor()->TextLength() 
                                + extendedSubjectChars
                                + KUniEdExtraLenCausedBySubject; // the extra because of '(' and ')'
            }        
        }
    UNILOGGER_WRITE_TIMESTAMP("CUniEditorAppUi::CheckSmsSizeAndUnicodeL End ---->>> ");
    }

// ---------------------------------------------------------
// CUniEditorAppUi::CheckHeaderForMessageTypeL
// ---------------------------------------------------------
//
void CUniEditorAppUi::CheckHeaderForMessageTypeL()
    {
    CUniEditorDocument* doc = Document();
    
    if( iMtm->MessageTypeSetting() != EUniMessageTypeSettingAutomatic )
        { // We are in locked mode so refresh email addr length and return
        iHeader->RefreshLongestEmailAddressL();
        return;
        }

    // We are in automatic mode so lets check everything
    TUniState oldState = doc->UniState();

    if( iHeader->IsHeaderSmsL() )
        {   // Set current header state as sms
        doc->SetHeaderUniState( EUniSms );
        }
    else
        {   // Set current header state as mms
        doc->SetHeaderUniState( EUniMms );
        }

    if( oldState != doc->UniState() )
        {   // If the state is not the same anymore, update title and show popup
        SetTitleL();
        UpdateIndicatorIconsL();
        ShowPopupNoteL( doc->UniState()==EUniMms?ETrue:EFalse );
        UpdateToolbarL();
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::CheckBodyForMessageTypeL
// ---------------------------------------------------------
//
void CUniEditorAppUi::CheckBodyForMessageTypeL()
    {
    CUniEditorDocument* doc = Document();
    
    TUniMessageTypeSetting setting = iMtm->MessageTypeSetting();

    if( IsHardcodedSms() )
        { // Just update iSmsLength and possible unicode mode
        CheckSmsSizeAndUnicodeL();
        return;
        }
    
    if( setting == EUniMessageTypeSettingMms )
        { // We are in locked mode so just leave
        UpdateToolbarL();
        return;
        }

    TUniState oldState = doc->UniState();

    if( IsBodySmsCompatibleL() )
        {   // Set current body state as sms
        doc->SetBodyUniState( EUniSms );
        }
    else
        {   // Set current body state as mms
        doc->SetBodyUniState( EUniMms );
        }

    if( oldState != doc->UniState() )
        {   // If the state is not the same anymore, update title and show popup
        SetTitleL();
        UpdateIndicatorIconsL();
        ShowPopupNoteL( doc->UniState()==EUniMms?ETrue:EFalse );
        UpdateToolbarL();
        
        if( iNLTFeatureSupport )
            {
            //Turkish SMS-PREQ2265 specific
            if(doc->UniState() == EUniSms)
                {
                //If the new state is SMS, Reset the SMS Settings and update the message length info 
                if(doc->SmsPlugin())
                    {
                    doc->SmsPlugin()->SetEncodingSettings(EFalse, doc->AlternativeEncodingType(), doc->CharSetSupport());
                    }
                //Force to recalculate the SMS PDU info
                CheckSmsSizeAndUnicodeL();
                MsgLengthToNavipaneL( );
                }
            }
        }
    }

// ----------------------------------------------------
// CUniEditorAppUi::CreateMmsNaviSizeStringL
// 
// Rounds always up
// 0.00000 -> 0
// 0.00001 -> 0.1
// ..
// 0.10000 -> 0.1
// 0.10001 -> 0.2
// ..
// 9.89999 -> 9.9
// 9.90000 -> 9.9
// 9.90001 -> 10
//
// ----------------------------------------------------
//
void CUniEditorAppUi::CreateMmsNaviSizeStringL( const TInt aSizeInBytes,
                                                TDes& aSize,
                                                TInt& aInteger,
                                                TInt& aFragment )
    {
    aSize.Zero();

    // Integer part
    aInteger = aSizeInBytes / KBytesInKilo;
    // Up rounded fragment part -> can be 10:
    aFragment = ( ( aSizeInBytes % KBytesInKilo ) * 10 + KBytesInKilo - 1) / KBytesInKilo;

    if ( aFragment >= 10 )
        {
        aInteger++;
        aFragment = 0;
        }

    if ( aInteger >= KOneDigitLimit )
        {
        // Show msg size without one digit accuracy
        if ( aFragment )
            {
            aInteger++;
            aFragment = 0;
            }
        MsgAttachmentUtils::FileSizeToStringL( aSize, aInteger * KBytesInKilo, ETrue );
        }
    else if ( aInteger != 0 || aFragment != 0 )
        {
        // There is some content and size is between 0 - 10 kB
        TLocale loc;
        TBuf<1> sep;
        sep.Append( loc.DecimalSeparator() );

        StringLoader::Format( aSize, *iMmsLengthFormatBuffer, 0, aInteger );
        HBufC* temp = aSize.AllocLC();
        StringLoader::Format( aSize, *temp, 2, aFragment );
        CleanupStack::PopAndDestroy();
        temp = aSize.AllocLC();
        StringLoader::Format( aSize, *temp, 1, sep );
        CleanupStack::PopAndDestroy();
        temp = NULL;
        }
    else
        {
        // Empty msg
        MsgAttachmentUtils::FileSizeToStringL( aSize, 0, ETrue );
        }
    }
                                        
// ---------------------------------------------------------
// CUniEditorAppUi::DoToolbarInsertOtherL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoToolbarInsertOtherL()
    {
    RArray<TInt> disabledItems;
    CleanupClosePushL( disabledItems );
    
    TInt toolbarResourceId( KErrNotFound );
    
    if ( AknLayoutUtils::PenEnabled() )
        {
        toolbarResourceId = R_UNIEDITOR_INSERT_OTHER_QUERY;

//        if ( Document()->CreationMode() == EMmsCreationModeRestricted )
//            {
//            disabledItems.Append( EUniCmdToolbarOtherFetchSVG );
//            }        
        }    
    
    TInt selectedIndex( 0 );
    CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &selectedIndex );
    dlg->PrepareLC( toolbarResourceId );
    
    CEikFormattedCellListBox* listbox = dlg->ListControl()->Listbox();
    CDesCArray* itemArray = static_cast<CDesCArray*>( listbox->Model()->ItemTextArray() );
    
    if ( disabledItems.Count() > 0 )
        {
        for ( TInt currentItem = 0; currentItem < disabledItems.Count(); currentItem++ )
            {
            itemArray->Delete( disabledItems[ currentItem ] );
            listbox->HandleItemRemovalL();
            }
        }
        
    if ( dlg->RunLD() )
        {
        for ( TInt currentItem = disabledItems.Count() - 1; currentItem >= 0 ; currentItem-- )
            {
            if ( selectedIndex >= disabledItems[ currentItem ] )
                {
                selectedIndex++;
                }
            }
        
        switch ( selectedIndex )    
            {
//            case EUniCmdToolbarOtherFetchSVG:
//                {
//                DoUserInsertSvgL();
//                break;
//                }
            case EUniCmdToolbarOtherFetchTextAtta:
                {
                DoUserInsertOtherFileL( ETextFile );
                break;
                }
            case EUniCmdToolbarOtherFetchOtherFiles:
                {
                DoUserInsertOtherFileL( EOtherFile );
                break;
                }
            default:
                {
                __ASSERT_DEBUG( EFalse, Panic( EUniUnsupportedCommand ) );
                break;
                }
            }
        }
        
    CleanupStack::PopAndDestroy( &disabledItems );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ShowListQueryL
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::ShowListQueryL( TInt aResourceID, TInt& aSelectedIndex ) const
    {
    CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &aSelectedIndex );
    dlg->PrepareLC( aResourceID );
    return dlg->RunLD();
    }

// ---------------------------------------------------------
// CUniEditorAppUi::HidePopupNote
// ---------------------------------------------------------
//
void CUniEditorAppUi::HidePopupNote()
    {
    if( iPopupNote )
        {
        iPopupNote->HideInfoPopupNote();    
        }
    }
    
// ---------------------------------------------------------
// CUniEditorAppUi::ShowPopupNoteL
// ---------------------------------------------------------
//
void CUniEditorAppUi::ShowPopupNoteL( TBool aMms )
    {
    if( iPopupNote )
        {
        iPopupNote->SetTextL( aMms? iPopupChangedMmsBuffer->Des():
                                    iPopupChangedSmsBuffer->Des() );
        iPopupNote->ShowInfoPopupNote();
        }
    }
 
// ---------------------------------------------------------
// CUniEditorAppUi::PlayFocusedItemL
// ---------------------------------------------------------
//
void CUniEditorAppUi::PlayFocusedItemL()
    {
    if ( Document()->DataModel()->SmilType() == E3GPPSmil ||
         Document()->DataModel()->SmilType() == ETemplateSmil )
        { 
        // There's presentation focused -> play it        
        PlayPresentationL();
        return;
        }

    CMsgBaseControl* ctrl = iView->FocusedControl();

    TUniRegion region = EUniRegionImage;

    if ( ctrl )
        {
        switch ( ctrl->ControlId() )
            {
            case EMsgComponentIdAudio:
                {
                region = EUniRegionAudio;
                }
            case EMsgComponentIdImage:
            case EMsgComponentIdVideo:
            case EMsgComponentIdSvg:
                {
                CUniObject* obj = iSmilModel->GetObject( Document()->CurrentSlide(), region );
                // Coverty fix, Null pointer return , http://ousrv057/cov.cgi?cid=37100
                if(obj)
                {
                  if( obj->Corrupted() )
                    { // Object is corrupt -> just show note
                    ShowInformationNoteL( R_UNIEDITOR_CANNOT_OPEN_CORRUPT, EFalse );
                    }
                  else
                    {
                    RFile file = CUniDataUtils::GetAttachmentFileL( Document()->DataModel()->Mtm(), obj->AttachmentId() );
                    CleanupClosePushL( file );
                    TDataType dataType( obj->MimeType() );
                    
                    if ( !iDocHandler )
                        {
                        iDocHandler = CDocumentHandler::NewL( iEikonEnv->Process() );
                        iDocHandler->SetExitObserver( this );
                        }
                    
                    if ( obj->DrmInfo() )
                        {
                        obj->DrmInfo()->ReleaseRights();
                        }
                    
                   // Since Activating input blockers will block the command CEIkCmdEXit. So used the flag 
                   //ActivateInputBlockerL( NULL );
                    iEditorFlags |= EMsgEditInProgress;
                    TRAPD(ret,iDocHandler->OpenFileEmbeddedL( file, dataType ));
                    
                    CleanupStack::PopAndDestroy( &file );
                    
                    if(ret != KErrNone)
                        {
                         iEditorFlags &= ~EMsgEditInProgress;   
                         //  DeactivateInputBlocker();
                        }
                    }
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
// CUniEditorAppUi::CheckMaxSmsSizeAndShowNoteL
// ---------------------------------------------------------
//
void CUniEditorAppUi::CheckMaxSmsSizeAndShowNoteL( TBool aSendCheck /*= EFalse*/ )
    {
    CUniEditorDocument* doc = Document();

    if( !aSendCheck )
        {
        if( !IsHardcodedSms() )
            { // Just return
            iEditorFlags &= ~EMaxSmsSizeNoteShown;
            return;
            }
        }
    else
        {
        if( Document()->UniState() != EUniSms )
            { // UniState is MMS -> just return
            iEditorFlags &= ~EMaxSmsSizeNoteShown;
            return;
            }
        }
    
    TInt lengthOne( 0 );
    TInt lengthMany( 0 );
    
    doc->SmsCharacterLimits( lengthOne, lengthMany );

    TInt maxLen = lengthOne;
  
    if( doc->AbsoluteMaxSmsParts() > 1 )
        {
        maxLen = doc->AbsoluteMaxSmsParts()*lengthMany;
        }
    
    if ( doc->AbsoluteMaxSmsCharacters() > 0 )
        {
        maxLen = doc->AbsoluteMaxSmsCharacters();
        }
    
    TInt totalLen = iSmsBodyLength + iSmsSubjectLength;

    if( doc->EmailOverSmsSupported() )
        {
        // This length is needed by recipient and subject separator chars
        TInt separatorLen = 0;

        TInt emailAddrLen = iHeader->LongestEmailAddress();
        
        if( emailAddrLen )
            {
            separatorLen++; // one extra char needed by separator char
            }

        totalLen += emailAddrLen + separatorLen;
        }

    if( totalLen <= maxLen-1 )
        { // Length is under the max size -> just return
        iEditorFlags &= ~EMaxSmsSizeNoteShown;
        return;
        }

    if( !aSendCheck && iEditorFlags & EMaxSmsSizeNoteShown )
        { // The note has been shown -> just return
        return;                
        }

    iEditorFlags |= EMaxSmsSizeNoteShown;
    
    if( totalLen == maxLen )
        { 
        if( aSendCheck )
            { // SMS is max length so it can still be sent
            iEditorFlags &= ~EMaxSmsSizeNoteShown;
            return;
            }
        else
            { 
            if( iEditorFlags & EShowSmsSizeNoteNextTime )
                {
                // Show "Cannot add text"
                ShowConfirmableInfoL( R_UNIEDITOR_SMS_LENGTH_EXCEEDED );
                // We remove this flag already now
                iEditorFlags &= ~EShowSmsSizeNoteNextTime;
                }
            else
                {
                // Show the note next time
                iEditorFlags |= EShowSmsSizeNoteNextTime;
                // Remove this as it's not shown already
                iEditorFlags &= ~EMaxSmsSizeNoteShown;                
                }
            }
        }
    else if( totalLen == maxLen+1 )
        { // Show "Cannot send. Remove 1"
        ShowConfirmableInfoL( R_UNIEDITOR_SMS_LENGTH_EXCEEDED_1 );
        }
    else
        { // Show "Cannot send. Remove N"
        if( doc->UnicodeCharacterMode() )
            {
            ShowConfirmableInfoL( R_UNIEDITOR_SMS_LENGTH_EXCEEDED_MANY_UNICODE );
            }
        else
            {
            TInt howMany = totalLen - maxLen;
            HBufC* warningString = 
                StringLoader::LoadLC( R_UNIEDITOR_SMS_LENGTH_EXCEEDED_MANY, howMany, iCoeEnv );
            ShowConfirmableInfoL( *warningString );
            CleanupStack::PopAndDestroy( warningString );
            }

        }
    }
 
// ---------------------------------------------------------
// CUniEditorAppUi::SetOrRemoveMaxSizeInEdwin
// ---------------------------------------------------------
//
void CUniEditorAppUi::SetOrRemoveMaxSizeInEdwin()
    {
    CUniEditorDocument* doc = Document();
    
    if( !IsHardcodedSms() )
        { 
        // No max limit so set the max TInt
        if( BodyCtrlEditor() )
            {
            BodyCtrlEditor()->SetTextLimit( KMaxTInt );
            }
        return;
        }
        
    // OK, message type is SMS
    
    TInt lengthOne( 0 );
    TInt lengthMany( 0 );
    
    doc->SmsCharacterLimits( lengthOne, lengthMany );

    TInt maxLen = lengthOne;
  
    if( doc->AbsoluteMaxSmsParts() > 1 )
        {
        maxLen = doc->AbsoluteMaxSmsParts()*lengthMany;
        }
    
    if ( doc->AbsoluteMaxSmsCharacters() > 0 )
        {
        maxLen = doc->AbsoluteMaxSmsCharacters();
        }
    
    CEikEdwin* bodyEd = BodyCtrlEditor();
    CEikEdwin* subjEd = SubjectCtrlEditor();
    
    if( !bodyEd )
        { 
        // There's no body editor available -> just return
        return;
        }

    // This length is needed by recipient and subject separator chars
    TInt separatorLen = 0;

    TInt emailAddrLen = iHeader->LongestEmailAddress();
    
    if( emailAddrLen )
        {
        separatorLen++; // one extra char needed by separator char
        }

    // Max subject length is maxLen decreased by longest email address and body length
    TInt maxSubjectLen = maxLen - emailAddrLen - iSmsBodyLength - separatorLen;
    
    if( maxSubjectLen > KUniEdMaxSubjectLen )
        {
        maxSubjectLen = KUniEdMaxSubjectLen;
        }

    // Max body length is maxLen decreased by longest email address and subject length
    TInt maxBodyLen = maxLen - emailAddrLen - iSmsSubjectLength - separatorLen;
    
    if( bodyEd->TextLength() > maxBodyLen )
        { 
        // If there's already too long body field -> increase the limit to avoid panic in edwin
        maxBodyLen = bodyEd->TextLength();
        }
    
    bodyEd->SetTextLimit( maxBodyLen );
    
    if( subjEd )
        {
        if( subjEd->TextLength() > maxSubjectLen )
            { 
            // If there's already too long subject field -> increase the limit to avoid panic in edwin
            maxSubjectLen = subjEd->TextLength();
            }
        subjEd->SetTextLimit( maxSubjectLen );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::CheckMaxRecipientsAndShowNoteL
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::CheckMaxRecipientsAndShowNoteL( TBool aSendCheck /*= EFalse*/ )
    {
    
    TInt maxRecipients = 0;
    
    CUniEditorDocument* doc = Document();
    
    TUniState state = doc->UniState();
    
    if( IsHardcodedSms() )
        {
        maxRecipients = doc->MaxSmsRecipients();
        }
    else
        {
        maxRecipients = doc->MaxMmsRecipients();
        }
    
    TInt recipientCount = 0;
    
    CMsgAddressControl* addrCntrl = ToCtrl();
    
    if( addrCntrl )
        {
        recipientCount = addrCntrl->GetRecipientsL()->Count();
        }
    
    if( state == EUniMms ) // If the current state is mms,
        { // add also possible CC and BCC recipients
        addrCntrl = iView->ControlById( EMsgComponentIdCc )?
            static_cast<CMsgAddressControl*>(iView->ControlById( EMsgComponentIdCc ) ):NULL;
        if( addrCntrl )
            {
            recipientCount += addrCntrl->GetRecipientsL()->Count();
            }
        addrCntrl = iView->ControlById( EMsgComponentIdBcc )?
            static_cast<CMsgAddressControl*>(iView->ControlById( EMsgComponentIdBcc ) ):NULL;
        if( addrCntrl )
            {
            recipientCount += addrCntrl->GetRecipientsL()->Count();
            }
        }

    if( recipientCount <= maxRecipients )
        { // ok number of recipients
        iEditorFlags &= ~EMaxRecipientShown;
        return ETrue;
        }
        
/* This is to be removed from UI spec (max recipients reached
    if( !aSendCheck )
        {
        if( iEditorFlags & EMaxRecipientShown )
            { // Note is already shown
            return EFalse;
            }
        if( recipientCount == maxRecipients )
            {
            ShowConfirmableInfoL( R_UNIEDITOR_MAX_RECIPIENTS_REACHED );
            iEditorFlags |= EMaxRecipientShown;
            return EFalse;
            }
        }
  
        
    if( recipientCount == maxRecipients )
        { // This time aSendCheck is certainly ETrue so no note is needed
        return ETrue;
        }*/
  
    if( iEditorFlags & EMaxRecipientShown && !aSendCheck )
        { // Note is already shown and this is not send check
        return EFalse;
        }
    
    iEditorFlags |= EMaxRecipientShown;
    
    TInt waitRes( KErrNotFound );
    
    if ( iWaitDialog )
        {
        waitRes = iWaitResId;
        RemoveWaitNote();
        }   
    
    if( recipientCount == maxRecipients + 1 )
        {
        ShowConfirmableInfoL( R_UNIEDITOR_MAX_RECIPIENTS_EXCEEDED_1 );
        }
    else
        {
        HBufC* warningString = 
            StringLoader::LoadLC( R_UNIEDITOR_MAX_RECIPIENTS_EXCEEDED_MANY, 
                                  recipientCount - maxRecipients, 
                                  iCoeEnv );
            
        ShowConfirmableInfoL( *warningString );
        
        CleanupStack::PopAndDestroy( warningString );
        }
    
    if ( waitRes != KErrNotFound )
        {
        ShowWaitNoteL( waitRes );
        }
    
    return EFalse;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::UpdateMiddleSoftkeyL
// ---------------------------------------------------------
//
void CUniEditorAppUi::UpdateMiddleSoftkeyL()
    {
    /* This is just a placeholder so far */
    TInt resId = 0;

    CMsgBaseControl* ctrl = iView->FocusedControl();

    if ( ctrl )
        {
        switch ( ctrl->ControlId() )
            {
            case EMsgComponentIdTo:
            case EMsgComponentIdCc:
            case EMsgComponentIdBcc:
                {
                   if ( TUniMsvEntry::IsMmsUpload( Document()->Entry() ) )
                    {
                    break;
                    }
                
                resId = R_UNI_MSK_BUTTON_ADD;
                }
                break;
            case EMsgComponentIdAttachment:
                {
                resId = R_UNI_MSK_BUTTON_OBJECTS;
                break;
                }
            case EMsgComponentIdImage:
                if ( Document()->DataModel()->SmilType() == E3GPPSmil )
                    { // focus is on "no-edit" SMIL icon
                    resId = R_UNI_MSK_BUTTON_PLAY_PRES;
                    } 
                else
                    { // ordinary image
                    resId = R_UNI_MSK_BUTTON_VIEW_IMAGE;
                    }
                break;
            case EMsgComponentIdVideo:
                {
                resId = R_UNI_MSK_BUTTON_PLAY_VIDEO;
                break;
                }
            case EMsgComponentIdAudio:
                {
                resId = R_UNI_MSK_BUTTON_PLAY_AUDIO;
                break;
                }
            case EMsgComponentIdSvg:
                {
                resId = R_UNI_MSK_BUTTON_PLAY_SVG;
                break;
                }
            case EMsgComponentIdBody:
                {
                resId = R_UNI_MSK_BUTTON_CONTEXT_MENU;
                MenuBar()->SetContextMenuTitleResourceId( R_UNIEDITOR_CONTEXT_MENUBAR );
                break;
                }

            case EMsgComponentIdSubject:
            default:
                break;
            }
        }

    if ( resId != iMskResId )
        {
        const TInt KMskPosition = 3;
        CEikButtonGroupContainer* cba = Cba();
        if( resId != 0 )
            {
            cba->SetCommandL( KMskPosition, resId );
            }
        else
            { // nullify the command
            cba->SetCommandL( KMskPosition, 0, KNullDesC() );
            }
        cba->DrawNow();
        iMskResId = resId;
        }
    return;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::SetInputModeToAddressFields
// ---------------------------------------------------------
//
void CUniEditorAppUi::SetInputModeToAddressFields( TInt aInputMode )
    {
    for (TInt i = CUniEditorHeader::EHeaderAddressTo; 
        i <= CUniEditorHeader::EHeaderAddressBcc; i++ )
        {    
        if ( iHeader &&  
             iHeader->AddressControl( static_cast<CUniEditorHeader::THeaderFields> (i) ) )
            {
            iHeader->AddressControl( 
                static_cast<CUniEditorHeader::THeaderFields> (i) )
                    ->Editor().SetAknEditorCurrentInputMode( aInputMode );
            }
        }
    
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoEditMmsPriorityL
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoEditMmsPriorityL()
    {

    TUniSendingSettings settings;
    CUniEditorPlugin* plugin = MmsPlugin();

    if( plugin )
        {
        plugin->GetSendingSettingsL( settings );
        }

    TInt currentlySelected = 0;
    switch( settings.iPriority )
        {
        case TUniSendingSettings::EUniPriorityNormal:
            currentlySelected = 1;
            break;
        case TUniSendingSettings::EUniPriorityLow:
            currentlySelected = 2;
            break;
        default:
            break;
        }
    
    // Create listbox    
    CAknSingleGraphicPopupMenuStyleListBox* listBox = new (ELeave) CAknSingleGraphicPopupMenuStyleListBox;
    CleanupStack::PushL( listBox );
    
    // Create popup
    CAknPopupList* popup = CAknPopupList::NewL( 
                listBox, 
                R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT, 
                AknPopupLayouts::EMenuGraphicWindow );
    CleanupStack::PushL( popup );

    // Construct listbox
    listBox->ConstructL( popup, CEikListBox::ELeftDownInViewRect );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                                      CEikScrollBarFrame::EAuto );
  
    // Set title    
    HBufC* title = StringLoader::LoadLC( R_UNIEDITOR_PRIORITY_TITLE, iCoeEnv );
    popup->SetTitleL( title->Des() );
    CleanupStack::PopAndDestroy( title );

    CAknIconArray* iconArray = RadioButtonArrayL();
    listBox->ItemDrawer()->FormattedCellData()->SetIconArray( iconArray ); // FormattedCellData owns

    listBox->HandleItemAdditionL();

    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC( reader, R_UNI_PRIORITY_LIST );
    
    CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( KSettingsGranularity );
    CleanupStack::PushL( items );

    // Get the labels from resources
    const TInt count = reader.ReadInt16();

    for ( TInt loop = 0; loop < count; loop++ )
        {
        HBufC* label = reader.ReadHBufCL();        
        CleanupStack::PushL( label );
        TPtr pLabel = label->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( pLabel );
        
        TBuf<20> itemString;
        if( currentlySelected == loop )
            { // This one is selected
            itemString.AppendNum( 1 );
            }
        else
            {
            itemString.AppendNum( 0 );
            }

        itemString.Append( _L("\t") );
        itemString.Append( pLabel );
        items->AppendL( itemString );
        
        CleanupStack::PopAndDestroy( label );
        label = NULL;
        }

    CTextListBoxModel* model = listBox->Model();
    model->SetItemTextArray( items );
    model->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( items ); // model owns it now
    CleanupStack::PopAndDestroy(); // reader 

    listBox->SetCurrentItemIndexAndDraw( currentlySelected );

    TInt result = popup->ExecuteLD();

    if( result )
        {
        // OK, user chose the priority bit
        switch( listBox->CurrentItemIndex() )
            {
            case 0:
                {
                settings.iPriority = TUniSendingSettings::EUniPriorityHigh;
                break;
                }
            case 1:
                {
                settings.iPriority = TUniSendingSettings::EUniPriorityNormal;
                break;
                }
            default:
                {
                settings.iPriority = TUniSendingSettings::EUniPriorityLow;
                break;
                }
            }
            
        if( plugin )
            {
            plugin->SetSendingSettingsL( settings );
            }
            
        UpdateIndicatorIconsL();
        }
    CleanupStack::Pop(); // for popup
    CleanupStack::PopAndDestroy( listBox ); 
    }

// ---------------------------------------------------------
// CUniEditorAppUi::RadioButtonArrayL
// ---------------------------------------------------------
//
CAknIconArray* CUniEditorAppUi::RadioButtonArrayL()
    {
    TFileName* fileName = new( ELeave ) TFileName;
    CleanupStack::PushL( fileName );
    
    fileName->Copy( KAvkonBitmapFile );

    CAknIconArray* iconArray = new (ELeave) CAknIconArray( 2 );
    CleanupStack::PushL( iconArray ); 

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    CGulIcon* icon = NULL;

    TAknsItemID skinId = KAknsIIDQgnPropRadiobuttOff;
    TInt bitmapId = EMbmAvkonQgn_prop_radiobutt_off;

    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                            skinId,
                            bitmap,
                            mask,
                            *fileName,
                            bitmapId,
                            bitmapId+1 );
        
    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( mask );
    icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2, bitmap );
    CleanupStack::PushL( icon ); 
    iconArray->AppendL( icon );
    CleanupStack::Pop();   // icon

    skinId = KAknsIIDQgnPropRadiobuttOn;
    bitmapId = EMbmAvkonQgn_prop_radiobutt_on;

    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                            skinId,
                            bitmap,
                            mask,
                            *fileName,
                            bitmapId,
                            bitmapId+1 );
        
    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( mask );
    
    icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2 );
    CleanupStack::PushL( icon ); 
    
    iconArray->AppendL( icon );
    CleanupStack::Pop();   // icon

    CleanupStack::Pop( iconArray );
    CleanupStack::PopAndDestroy( fileName );
    return iconArray;
    }


// ---------------------------------------------------------
// CUniEditorAppUi::ConfirmSmsSendInMultiplePartsL
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::ConfirmSmsSendInMultiplePartsL()
    {
    // If it's variated off -> return ETrue
    if( !( iEditorFlags & EShowSmsSentInManyParts ) )
        {
        return ETrue;
        }
    
    CUniEditorDocument* doc = Document();
    
    TInt lengthOne( 0 );
    TInt lengthMany( 0 );
    
    doc->SmsCharacterLimits( lengthOne, lengthMany );

    TInt totalLen = iSmsBodyLength + iSmsSubjectLength;

    if ( doc->EmailOverSmsSupported() )
        {
        // This length is needed by recipient and subject separator chars
        TInt separatorLen = 0;

        TInt emailAddrLen = iHeader->LongestEmailAddress();
        
        if( emailAddrLen )
            {
            separatorLen++; // one extra char needed by separator char
            }

        totalLen += emailAddrLen + separatorLen;
        }

    if( totalLen <= lengthOne )
        { // All fits in one message -> return ETrue
        return ETrue;
        }

    TInt messages = totalLen/lengthMany;
    
    if( totalLen%lengthMany )
        { // If remainder is not zero -> add one
        messages++;
        }

    HBufC* qry = StringLoader::LoadLC( R_UNIEDITOR_SMS_SEND_MULTIPLE_PARTS, messages );
    
    if( ShowConfirmationNoQuestionQueryL( *qry ) )
        {
        CleanupStack::PopAndDestroy( qry );
        return ETrue;
        }
        
    CleanupStack::PopAndDestroy( qry );
    return EFalse;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::SetTitleIconsSizeL
//
// Sets the correct size from LAF for title MMS & SMS title icons
// ---------------------------------------------------------
//
void CUniEditorAppUi::SetTitleIconsSizeL()
    {
    SetTitleIconSizeL( iIconSms->Bitmap() );
    SetTitleIconSizeL( iIconMms->Bitmap() );    
    }
 
// ---------------------------------------------------------
// CUniEditorAppUi::AcceptEmailAddresses
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::AcceptEmailAddresses()
    {
    TBool retVal = ETrue;

    if( IsHardcodedSms() && !Document()->EmailOverSmsSupported() )
        { // We are in locked SMS mode and no email over sms -> accept only numbers
          retVal = EFalse;
        }

    return retVal;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::IsHardcodedSms
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::IsHardcodedSms()
    {
    TBool hardSms = EFalse;

    if( iMtm->MessageTypeLocking() == EUniMessageTypeLocked )
        { // the message type is pre-locked
        if( iMtm->MessageTypeSetting() == EUniMessageTypeSettingSms )
            { // to sms
            hardSms = ETrue;
            }
        }
    else if( iMtm->MessageTypeSetting() == EUniMessageTypeSettingSms )
           { // message is not pre-locked, but manually locked to sms
           hardSms = ETrue;
           }

    return hardSms;    
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ResolveLaunchFocusedControlL
// 
// Resolves the control id of control that should be focused after
// launch. Normal case is that first existing and empty control from To, Subject,
// body text control and image control is focused in this order.
//
// Exeptions to this is:
//
// Reply -> Body text field always focused.
// Locked SMS with vCard/vCal -> To address field always focused.
// ---------------------------------------------------------
//
TMsgControlId CUniEditorAppUi::ResolveLaunchFocusedControlL()
    {
    TMsgControlId result = EMsgComponentIdNull;
    
    if ( Document()->MessageType() == EUniReply )
        {
        result = EMsgComponentIdBody;
        }
    else if ( iEditorFlags & ELockedSmsWithAtta )
        {
        result = EMsgComponentIdTo;
        }
    else
        {
        CMsgAddressControl* toControl = ToCtrl();
    
        if ( toControl &&
             toControl->GetRecipientsL()->Count() == 0 )
            {
            result = EMsgComponentIdTo;
            }
        else
            {
            if ( iSupportedFeatures & EUniFeatureSubject )
                {
                CMsgExpandableControl* subj = SubjectCtrl();
                if ( subj && 
                     subj->TextContent().DocumentLength() == 0 )
                    {
                    result = EMsgComponentIdSubject;
                    }
                }
            
            if ( result == EMsgComponentIdNull )
                {
                if ( BodyCtrl() )
                    {
                    result = EMsgComponentIdBody;
                    }
                else if ( ImageCtrl() )
                    {
                    result = EMsgComponentIdImage;
                    }
                
                }
            }
        }
        
    return result;
    }
 
// ---------------------------------------------------------
// CUniEditorAppUi::SetMessageTypeLockingL
// ---------------------------------------------------------
//
void CUniEditorAppUi::SetMessageTypeLockingL()
    {
    if ( SmsPlugin() && !MmsPlugin() )
        { 
        // Sms found, no mms
        iMtm->SetMessageTypeSetting( EUniMessageTypeSettingSms );
        iMtm->SetMessageTypeLocking( EUniMessageTypeLocked );
        }
    else if ( !SmsPlugin() && MmsPlugin() )
        { 
        // Mms found, no sms
        iMtm->SetMessageTypeSetting( EUniMessageTypeSettingMms );
        iMtm->SetMessageTypeLocking( EUniMessageTypeLocked );
        }
    else
        { 
        // If we are here, both sms and mms exist
        if ( TUniMsvEntry::IsMmsUpload( Document()->Entry() ) )
            { 
            // If opening upload message -> lock is as mms
            iMtm->SetMessageTypeSetting( EUniMessageTypeSettingMms );
            iMtm->SetMessageTypeLocking( EUniMessageTypeLocked );
            }
        else
            {
            if ( iMtm->MessageTypeLocking() == EUniMessageTypeLockingNotSet )
                { 
                // It's not yet set -> check the settings of SMS conversion
                iMtm->SetMessageTypeLocking( EUniMessageTypeNotLocked );
                
                TUniSendingSettings settings;
                SmsPlugin()->GetSendingSettingsL( settings );

                if ( settings.iMessageType == TUniSendingSettings::EUniMessageTypeFax  ||
                     settings.iMessageType == TUniSendingSettings::EUniMessageTypePaging )
                    { 
                    // It's temporary locked to sms sub type (fax or pager)
                    iMtm->SetMessageTypeSetting( EUniMessageTypeSettingSms );
                    }
                else
                    { 
                    // Otherwise set it to automatic
                    iMtm->SetMessageTypeSetting( EUniMessageTypeSettingAutomatic );
                    }
                }
            }
        }
    }
 
// ---------------------------------------------------------
// CUniEditorAppUi::ActivateInputBlockerL
// ---------------------------------------------------------
//
void CUniEditorAppUi::ActivateInputBlockerL( CActive* aActiveObjectToCancel )
    {
    delete iInputBlocker;
    iInputBlocker = NULL;
    
    iInputBlocker = CAknInputBlock::NewLC();    
    CleanupStack::Pop( iInputBlocker );
    
    if ( aActiveObjectToCancel )
        {
        iInputBlocker->SetCancelActive( aActiveObjectToCancel );
        }
    }
 
// ---------------------------------------------------------
// CUniEditorAppUi::DeactivateInputBlocker
// ---------------------------------------------------------
//
void CUniEditorAppUi::DeactivateInputBlocker()
    {
    if ( iInputBlocker )
        {
        iInputBlocker->SetCancelActive( NULL );    
        delete iInputBlocker;
        iInputBlocker = NULL;
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::UpdateToolbarL
// ---------------------------------------------------------
//
void CUniEditorAppUi::UpdateToolbarL()
    {
    if ( iFixedToolbar && !iFixedToolbar->IsDimmed()  )
        {
        iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarSend, EFalse, EFalse );
        
        if ( TUniMsvEntry::IsMmsUpload( Document()->Entry() ) )
            {
            iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarAddRecipient, ETrue, EFalse );
            }
        else
            {
            iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarAddRecipient, EFalse, EFalse );
            }
        
        if ( IsHardcodedSms() ||
             ( Document()->UniState() == EUniMms &&
               Document()->DataModel()->SmilType() != EMmsSmil ) )

            {
            iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, ETrue, EFalse );
            }
        else
            {
            iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, EFalse, EFalse );
            }
        
        iFixedToolbar->DrawNow();
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::CheckLockedSmsWithAttaL
// 
// Checks whether message is on locked SMS state with single
// vCard or vCal attachment. In this state text is not allowed
// to be entered into message. First performs state detection.
// If we are on locked SMS with attachment state focus is 
// moved away from subject or body control. Then both subject and
// body text controls are removed. If we are not in locked SMS
// with attachment state subject and body text ccontrol are added
// if appropriate (i.e. they have been removed due to locked SMS
// with attachment state and has not yet been added).
// ---------------------------------------------------------
//
void CUniEditorAppUi::CheckLockedSmsWithAttaL()
    {
    CUniDataModel* model = Document()->DataModel();
    
    TBool lockedSmsWithAtta( EFalse );
    
    if ( IsHardcodedSms() &&
         model->AttachmentList().Count() == 1 )
        {
        CUniObject* obj = model->AttachmentList().GetByIndex( 0 );
        if ( obj &&  
             obj->MimeType().Length() > 0 &&   
             ( obj->MimeType().CompareF( KMsgMimeVCard  ) == 0 || 
#if defined (__I_CAL_SUPPORT) && defined (RD_MESSAGING_ICAL_IMPORT)
               obj->MimeType().CompareF( KMsgMimeICal  ) == 0 ||
#endif
               obj->MimeType().CompareF( KMsgMimeVCal  ) == 0 ) )
            {
            lockedSmsWithAtta = ETrue;
            }
        } 
    
    if ( lockedSmsWithAtta )
        {
        iEditorFlags |= ELockedSmsWithAtta;
        
        CMsgBaseControl* focusedCtrl = iView->FocusedControl();
        if ( focusedCtrl && 
             ( focusedCtrl->ControlId() == EMsgComponentIdSubject ||
               focusedCtrl->ControlId() == EMsgComponentIdBody ) )
            {
            iView->SetFocus( EMsgComponentIdTo );
            }
        
        if ( iEditorFlags & ESubjectOkInSms &&
             SubjectCtrl() )
            {
            iHeader->AddHeadersDeleteL( EUniFeatureSubject, EFalse );
            }
    
        if ( !iStoredBodyControl )
            {   
            iStoredBodyControl = iView->RemoveControlL( EMsgComponentIdBody );
            }
        }
    else
        {  
        if ( iEditorFlags & ELockedSmsWithAtta )
            {
            if ( iEditorFlags & ESubjectOkInSms &&
                 !SubjectCtrl() )
                {
                // Determine if subject field is needed if subject is ok in SMS 
                // and there is no subject field.
                TInt headersVariation = iHeader->AddHeadersVariation();
                TInt headerConfig = iHeader->AddHeadersConfigL();
                
                if ( !( headersVariation & EUniFeatureSubjectConfigurable ) ||
                      ( headersVariation & EUniFeatureSubject &&
                        headerConfig & EUniFeatureSubject ) )
                    {
                    // Insert subject to SMS if it is always visible or selected
                    // and configured on.
                    iHeader->AddHeadersAddL( EUniFeatureSubject );
                    }
                }
            
            if ( iStoredBodyControl )
                {   
                iView->AddControlL( iStoredBodyControl, EMsgComponentIdBody, EMsgFirstControl, EMsgBody );
                iStoredBodyControl = NULL;
                }
            }
        
        iEditorFlags &= ~ELockedSmsWithAtta;
        }
    }
     

// ---------------------------------------------------------
// CUniEditorAppUi::SetFixedToolbarDimmed
// ---------------------------------------------------------
// 
void CUniEditorAppUi::SetFixedToolbarDimmed()
    {
    iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarSend, ETrue, ETrue );
    iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarAddRecipient, ETrue, ETrue );        
    iFixedToolbar->SetItemDimmed( EUniCmdFixedToolbarInsert, ETrue, ETrue );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::EnableFixedToolbar
// ---------------------------------------------------------
// 
void CUniEditorAppUi::EnableFixedToolbar( TAny* aAny )
    {
    static_cast<CUniEditorAppUi*>( aAny )->DoEnableFixedToolbar();
    }
// ---------------------------------------------------------
// CUniEditorAppUi::UpdateFixedToolbar
// ---------------------------------------------------------
// 
void CUniEditorAppUi::UpdateFixedToolbar( TAny* aAny )
    {
    static_cast<CUniEditorAppUi*>( aAny )->DoUpdateFixedToolbar();      
    }
// ---------------------------------------------------------
// CUniEditorAppUi::DisableSendKey
// ---------------------------------------------------------
// 
void CUniEditorAppUi::DisableSendKey(TAny* aAny)
{
	static_cast<CUniEditorAppUi*>( aAny )->SetKeyEventFlags( EDisableSendKeyShort | EDisableSendKeyLong );
}

// CUniEditorAppUi::CreateFixedToolbarL
// ---------------------------------------------------------
// 
void CUniEditorAppUi::CreateFixedToolbarL( TBool aSetDimmed )
    {
#ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() )
        {        
        if ( !iFixedToolbar )
            {
            iFixedToolbar = CAknToolbar::NewL( R_UNIEDITOR_TOOLBAR );
            iFixedToolbar->SetToolbarObserver( this );            
            
             if ( aSetDimmed )
                {
                SetFixedToolbarDimmed();
                }
            if ( !( iEditorFlags & EToolbarHidden ) )
                {
                iFixedToolbar->SetToolbarVisibility( ETrue, EFalse );
                }
            }
        
        CEikAppUiFactory* appUiFactory = static_cast<CEikAppUiFactory*>( iEikonEnv->AppUiFactory() );
        
        CAknToolbar* oldFixedToolbar = appUiFactory->CurrentFixedToolbar();
        if ( oldFixedToolbar != iFixedToolbar )
            {
            oldFixedToolbar->HideItemsAndDrawOnlyBackground( ETrue );
            appUiFactory->SetViewFixedToolbar( iFixedToolbar );     
            }
        
       
        }
    else
        {
        static_cast<CEikAppUiFactory*>( iEikonEnv->AppUiFactory() )->SetViewFixedToolbar( NULL );
        
        delete iFixedToolbar;
        iFixedToolbar = NULL;
        }
#endif // RD_SCALABLE_UI_V2
    }

// ---------------------------------------------------------
// CUniEditorAppUi::SetExtensionButtonDimmed
// ---------------------------------------------------------
// 
void CUniEditorAppUi::SetExtensionButtonDimmed( CAknToolbarExtension* aExtension, 
                                                TInt aButtonId, 
                                                TBool aDimmed )
    { 
    CAknButton* buttonControl = static_cast<CAknButton*>( aExtension->ControlOrNull( aButtonId ) );
    if ( buttonControl &&
         buttonControl->IsDimmed() != aDimmed )
        {
        buttonControl->SetDimmed( aDimmed );             
        }
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoEnableFixedToolbar
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoEnableFixedToolbar()
    {
    if ( iFixedToolbar )
        {
        iFixedToolbar->HideItemsAndDrawOnlyBackground( EFalse );
        }
    
    iEditorFlags &= ~EToolbarHidden;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::DoUpdateFixedToolbar
// Update the Toolbar which was dimmed while launching
// the Dialog 
// ---------------------------------------------------------
//
void CUniEditorAppUi::DoUpdateFixedToolbar()
    {
     TRAP_IGNORE(UpdateToolbarL());    
    }
// ---------------------------------------------------------
// CUniEditorAppUi::DoEnterKeyL
//
// Performs a special handling for enter key when certain control is
// focused. Enter key presses are mapped to selection key presses in
// thse controls.
// ---------------------------------------------------------
// 
TBool CUniEditorAppUi::DoEnterKeyL()
    {
    TBool result( EFalse );
    CMsgBaseControl* ctrl = iView->FocusedControl();

    if ( ctrl )
        {
        switch ( ctrl->ControlId() )
            {
            case EMsgComponentIdAttachment:
            case EMsgComponentIdImage:
            case EMsgComponentIdVideo:
            case EMsgComponentIdAudio:
            case EMsgComponentIdSvg:
                {
                DoSelectionKeyL();
                result = ETrue;
                break;
                }
            default:
                {
                break;
                }
            }
        }
    
    return result;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::HandleNotifyInt
// ---------------------------------------------------------
//
void CUniEditorAppUi::HandleNotifyInt( TUint32 /*aId*/, TInt /*aNewValue*/ )
    {
    if( iNLTFeatureSupport )
        {
        //Turkish SMS-PREQ2265 specific
        //Get the new lang id and corresponding alternative encoding if any
        CUniEditorDocument *doc = Document();
        TInt inputLang;
              
        //Update the setting cache, so that new language id is returned correctly
        iAvkonEnv->SettingCache().Update(KEikInputLanguageChange);
        inputLang = iAvkonEnv->SettingCache().InputLanguage();
        iSettingsChanged = ETrue;
        
        TSmsEncoding alternateEncoding = doc->GetLanguageSpecificAltEncodingType(inputLang);
        
        doc->SetAlternativeEncodingType(alternateEncoding);
        //Reset unicode mode to False and set the corresponding language alternative encoding type accordingly
        if(doc->SmsPlugin())
            {
            doc->SmsPlugin()->SetEncodingSettings(EFalse, alternateEncoding, doc->CharSetSupport());
            }        
        }
	//TODO :: Should add check size and show note???
    TRAP_IGNORE(
        {
        CheckSmsSizeAndUnicodeL();
        MsgLengthToNavipaneL();
        } );
    }

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CUniEditorAppUi::OnPeninputUiActivated
// ---------------------------------------------------------
// 
void CUniEditorAppUi::OnPeninputUiActivated()
    {
    TRAP_IGNORE( SmsMsgLenToPenInputL() );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::OnPeninputUiDeactivated
// ---------------------------------------------------------
//
void CUniEditorAppUi::OnPeninputUiDeactivated()
    {
     if (AknLayoutUtils::PenEnabled() )
        {
           iPeninputServer.UpdateAppInfo( KNullDesC, EAppIndicatorMsg );    
        }
    }
#else
// ---------------------------------------------------------
// CUniEditorAppUi::OnPeninputUiActivated
// ---------------------------------------------------------
//
void CUniEditorAppUi::OnPeninputUiActivated()
    {
    }

// ---------------------------------------------------------
// CUniEditorAppUi::OnPeninputUiDeactivated
// ---------------------------------------------------------
//
void CUniEditorAppUi::OnPeninputUiDeactivated()
    {
    }
#endif    

// ---------------------------------------------------------
// CUniEditorAppUi::SmsMsgLenToPenInputL
// ---------------------------------------------------------
//
void CUniEditorAppUi::SmsMsgLenToPenInputL()
    {
    CMsgBaseControl* ctrl = iView->FocusedControl();
    CUniEditorDocument* doc = Document();
           
    if ( doc->UniState() == EUniSms && ctrl && iMsgLenToVKB)
        {
        if ( UpdateMsgLenToPenInput() )                      
            {
            MsgLengthToNavipaneL( ETrue );
            }        
        }
    else
        { 
        if (AknLayoutUtils::PenEnabled() )
            {
            iPeninputServer.UpdateAppInfo( KNullDesC, EAppIndicatorMsg );    
            }     
        }        
    }
TBool CUniEditorAppUi::UpdateMsgLenToPenInput()
    {
    CMsgBaseControl* ctrl = iView->FocusedControl();
    if ( ctrl->ControlId() == EMsgComponentIdBody || 
       ( ctrl->ControlId() == EMsgComponentIdSubject && iEditorFlags & ESubjectOkInSms ) )                      
       {
       return ETrue;
       }
    else
       {
       return EFalse;           
       }
        
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ExitWithoutSave
//	Exits from the Unified Editor whne there is not sufficient memory to continue
// ---------------------------------------------------------
//
void CUniEditorAppUi::ExitWithoutSave()
    {
     DeactivateInputBlocker();
     EndActiveWait();
     RemoveWaitNote();
     TRAP_IGNORE(ShowConfirmableInfoL( R_UNIEDITOR_NOT_ENOUGH_MEMORY));     
     Exit( EAknSoftkeyClose );
    }
// ---------------------------------------------------------
// CUniEditorAppUi::HandleLongTapEventL
//  Function for handling the long tap events
// ---------------------------------------------------------
//
void CUniEditorAppUi::HandleLongTapEventL(const TPoint& aPenEventLocation, 
										  const TPoint& aPenEventScreenLocation )
    {
    CMsgBaseControl* ctrl = iView->FocusedControl(); // ctrl can be NULL.
    if ( ctrl && 
         ( ctrl->ControlId() == EMsgComponentIdAudio ||
                 ctrl->ControlId() == EMsgComponentIdImage ||
                 ctrl->ControlId() == EMsgComponentIdVideo ||
                 ctrl->ControlId() == EMsgComponentIdSvg ) )
        {
        TRect rect = ctrl->Rect();

        if ((!iTapConsumed)&&rect.Contains(aPenEventLocation))
            {
            if (iEmbeddedObjectStylusPopup)
                {
                delete iEmbeddedObjectStylusPopup;
                iEmbeddedObjectStylusPopup = NULL;
                }
            iEmbeddedObjectStylusPopup = CAknStylusPopUpMenu::NewL(this,aPenEventLocation);
            TResourceReader reader;
            iCoeEnv->CreateResourceReaderLC(reader,R_UNIEDITOR_EMBEDDED_OBJECT_STYLUS_MENU );
            iEmbeddedObjectStylusPopup->ConstructFromResourceL(reader);
            CleanupStack::PopAndDestroy();
            iEmbeddedObjectStylusPopup->SetPosition(aPenEventLocation);
            iEmbeddedObjectStylusPopup->ShowMenu();
            iTapConsumed = ETrue;
            }
        }   
    }

// ---------------------------------------------------------
// CUniEditorAppUi::IsObjectPathValidL
// Checks whether all inserted object's path are valid
// message.
// ---------------------------------------------------------
//
TBool CUniEditorAppUi::IsObjectsPathValidL() const
    {
    TBool bCanSave = ETrue;  
    if ( Document()->DataModel()->SmilType() == EMmsSmil )
        {
        bCanSave = Document()->DataModel()->ObjectList().IsListValid();
        
        if(bCanSave)
            {
            bCanSave = Document()->DataModel()->AttachmentList().IsListValid();
            }
        }      
    return bCanSave;
    }

// End of file

