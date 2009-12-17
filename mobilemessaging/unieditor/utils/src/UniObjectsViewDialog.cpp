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
*       Defines implementation of CUniObjectsViewDialog class methods.
*
*/



// ========== INCLUDE FILES ================================
#include "UniObjectsViewDialog.h"

#include <gulicon.h>
#include <eikenv.h>                         // Eikon Enviroment
#include <eikdll.h>                         // Application start
#include <eikmenup.h>                       // for CEikMenuPane
#include <eikmenub.h>                       // for CEikMenuBar
#include <bautils.h>                        // NearestLanguageFile
#include <aknlists.h>
#include <apgcli.h>                         // RApaLsSession
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <DocumentHandler.h>                // doc handler
#include <StringLoader.h>                   // StringLoader
#include <data_caging_path_literals.hrh> 
#include <txtrich.h>                        // CRichText
#include <avkon.hrh>

#include <mmsvattachmentmanager.h>
#include <msvstore.h>
#include <msvstd.h>                         // TMsvId
#include <biodb.h>                          // CBioDatabase
#include <mtclbase.h>

#include <NpdApi.h>

#include <sendui.h>
#include <SenduiMtmUids.h>
#include <Sendnorm.rsg>
#include <CMessageData.h>

#include <msgtextutils.h>
#include <MsgMediaInfo.h>

#include <MsgEditorAppUi.rsg>               // atta dlg base resouce identifiers
#include <UniUtils.rsg>                    // obj view resouce identifiers
#include <AiwGenericParam.h>

// Features
#include <featmgr.h>    

#include <fileprotectionresolver.h>

// Help
#include <csxhelp/unif.hlp.hrh>
#include <csxhelp/mms.hlp.hrh>
#include <hlplch.h>   // For HlpLauncher

#include "UniUtils.h"

#include <unimodelconst.h>
#include <unidatamodel.h>
#include <unidatautils.h>
#include <uniobject.h>
#include <unidrminfo.h>
#include <unismilmodel.h>
#include <uniobjectlist.h>
#include <unimimeinfo.h>
#include <uniobjectsinfo.h>
#include <uniobjectsmodel.h>

#include <MsgEditorCommon.h>
#include <MsgMimeTypes.h>

#include <MsgEditorAppUi.hrh>               // atta dlg base commands
#include "UniObjectsViewDialog.hrh"         // obj view commands

#include <UniUtils.rsg>

#include "UniEditorLogging.h"

#include "UniObjectsListArray.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

_LIT( KMmsFormatTwoDigits, "%x_%x\\" );

const TUid KNotepadUID3 = { 0x1000599d };
const TUid KUidMmsViewer = { 0x100058DF };
const TUid KUniEditorAppId = { 0x102072D8 };

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniObjectsViewDialog::ObjectsViewL
// ---------------------------------------------------------
//
EXPORT_C TInt CUniObjectsViewDialog::ObjectsViewL( TUniObjectsDialogType aDialogType,
                                                   CUniObjectsModel& aAttachmentModel,
                                                   CEikonEnv* aEikonEnv,
                                                   TUniObjectsDialogExitCmd& aExitCmd,
                                                   CUniDataModel& aDataModel,
                                                   TInt aFocusedItemIndex )
    {
    TFileName* fileName = new ( ELeave ) TFileName;
    CleanupStack::PushL( fileName );
    
    TParse parse;
    User::LeaveIfError( parse.Set( KUniUtilsResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL ) );
    *fileName = parse.FullName();
    
    BaflUtils::NearestLanguageFile( aEikonEnv->FsSession(), *fileName );
    TInt offset = aEikonEnv->AddResourceFileL( *fileName );
    
    CleanupStack::PopAndDestroy( fileName );
    
    TInt menuId = R_UNUT_EDITOR_OBJECTS_MENU;

    switch ( aDialogType )
        {
        case EViewer:
            {
            menuId = R_UNUT_VIEWER_OBJECTS_MENU;
            break;
            }
        case EForward:
            {
            menuId = R_UNUT_FORWARD_OBJECTS_MENU;
            break;
            }
        case EEditor:
            {
            menuId = R_UNUT_EDITOR_OBJECTS_MENU;
            break;
            }
        default:
            {
            break;
            }
        }

    HBufC* title = aEikonEnv->AllocReadResourceLC( R_UNUT_OBJECTS_TITLE );
    TInt ret = 0;
    
    PopulateAttachmentModelL( aAttachmentModel, aDataModel );
    
    CUniObjectsViewDialog* dlg = new ( ELeave ) CUniObjectsViewDialog (
        *title, 
        aDialogType, 
        menuId, 
        aAttachmentModel,  
        offset, 
        aExitCmd,
        aDataModel,
        aFocusedItemIndex );
    
    CleanupStack::PushL( dlg );
    dlg->ConstructL( aDataModel.Mtm().Entry() );
    CleanupStack::Pop( dlg );
        
    ret = dlg->ExecuteLD( R_MEB_VIEW_ATTACHMENT_DIALOG ); // wait dialog.
    CleanupStack::PopAndDestroy( title ); // store, title

    return ret;
    }


// ---------------------------------------------------------
// CUniObjectsViewDialog::CUniObjectsViewDialog
// ---------------------------------------------------------
//
CUniObjectsViewDialog::CUniObjectsViewDialog( TDesC& aTitle,
                                              TUniObjectsDialogType aDialogType,
                                              TInt aMenuId,
                                              CUniObjectsModel& aAttachmentModel,
                                              TInt aResourceOffset,
                                              TUniObjectsDialogExitCmd& aExitCmd,
                                              CUniDataModel& aDataModel,
                                              TInt aFocusedItemIndex ) :
    CMsgViewAttachmentsDialog( aTitle, aMenuId, aAttachmentModel ),
    iDialogType( aDialogType ),
    iResourceOffset( aResourceOffset ),
    iDataModel( aDataModel ),
    iSupportedFeatures( 0 ),
    iExitCmd( aExitCmd ),
    iFocusedItemIndex( aFocusedItemIndex )
    {
    iExitCmd = EUniObjectsViewBack;
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::~CUniObjectsViewDialog
// ---------------------------------------------------------
//
CUniObjectsViewDialog::~CUniObjectsViewDialog()
    {
    if ( iResourceOffset != -1 )
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        }

    // Remove drm info observer
    CUniObjectsModel& model( static_cast<CUniObjectsModel&>( iAttachmentModel ) );
    TInt countObjects = model.NumberOfItems( );
    for ( TInt i = 0; i < countObjects; i++ )
        {
        CUniObject* obj = 
            static_cast<CUniObjectsInfo&>
                ( model.AttachmentInfoAt( i ) ).Object();
        if ( obj )
            {
            obj->RemoveObserver( this );
            }
        }

    delete iSendUi;
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::ConstructL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::ConstructL( CMsvEntry& aEntry )
    {
    FeatureManager::InitializeLibL();
    
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        iSupportedFeatures |= EUniFeatureHelp;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdDrmFull ) )
        {
        iSupportedFeatures |= EUniFeatureDrmFull;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdJapanese ) )
        {
        iSupportedFeatures |= EUniFeatureJapanese;
        }

    FeatureManager::UnInitializeLib();

    CMsgViewAttachmentsDialog::ConstructL( aEntry );
    
    if ( iDialogType == EViewer )
        {
        iSendUi = CSendUi::NewL();
        }
        
    // Add drm info observer
    CUniObjectsModel& model( static_cast<CUniObjectsModel&>( iAttachmentModel ) );
    TInt countObjects = model.NumberOfItems( );
    for ( TInt i = 0; i < countObjects; i++ )
        {
        CUniObject* obj = 
            static_cast<CUniObjectsInfo&>
                ( model.AttachmentInfoAt( i ) ).Object();
        if ( obj )
            {
            // concept of 'DRM rights when loaded' is different
            // current status applies; consumed status does not matter
            obj->SetDrmRightsWhenLoadedObjects( IsRightsNow( *obj ) );
            obj->SetObserverL( this );
            }

        }
    // Enable task swapper to options menu after launch has been completed.
    }

// ---------------------------------------------------------
// CMsgViewAttachmentsDialog::LoadFormDataL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::LoadFormDataL()
    {
    // iconarray for two icons.
    // index 0: application icon.
    // index 1: indicator.
    iIconArray = new ( ELeave ) CArrayPtrFlat<CGulIcon>( 2 );

    iListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( iIconArray );

    // list items is not actually an array but object that returns
    // list item text for listbox in MdcaPoint method.
    iListItems = CUniObjectListItemArray::NewL( iAttachmentModel, iIconArray, iDataModel.DataUtils() );
    iListBox->Model()->SetItemTextArray( iListItems );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );    
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::PreLayoutDynInitL
//
// Sets the intial focus if given. Also enables open middle softkey.
// Assumes that empty slides are not set initially focused.
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::PreLayoutDynInitL()
    {    
    CMsgViewAttachmentsDialog::PreLayoutDynInitL();    
    
    if ( iFocusedItemIndex != -1)
        {
        iListBox->SetCurrentItemIndex( iFocusedItemIndex );
        if ( ButtonGroupContainer().ControlOrNull( EAknSoftkeyOpen ) )
            {
            ButtonGroupContainer().MakeCommandVisible( EAknSoftkeyOpen, ETrue );
            }
        }
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::ProcessCommandL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::ProcessCommandL( TInt aCommandId )
    {
    HideMenu();

    TInt index = CurrentListItemIndex();

    switch ( aCommandId )
        {
        case EMmsObjectsTextFirst:
        case EMmsObjectsTextSecond:
            ChangeOrderL( aCommandId, 0 );
            break;
        case EMsgViewAttachmentCmdOpen:
            ProcessCommandOpenL( index );
            break;
        case EMsgViewAttachmentCmdSave:
            ProcessCommandSaveL( index );
            break;
        case EMsgViewAttachmentCmdRemove:
            {
            CUniObjectsInfo& selectedObject = static_cast<CUniObjectsInfo&>
                        ( iAttachmentModel.AttachmentInfoAt( CurrentListItemIndex() ) );   
            TBool emptySlide = ( selectedObject.Flags() & CUniObjectsInfo::EMmsSlide );
            if(emptySlide)
                break;
            ProcessCommandRemoveL( index );
            }
            break;

        case EAknCmdHelp:
            {
            LaunchHelpL();
            }
            break;
        
        case EMmsObjectsSendVia:
            {
            CArrayFix<TUid>* uids = new ( ELeave ) CArrayFixFlat<TUid>( 1 );
            CleanupStack::PushL( uids );
            
            CUniObjectsInfo& selectedObject = static_cast<CUniObjectsInfo&>
                ( iAttachmentModel.AttachmentInfoAt( index ) );
            CUniObject* obj = selectedObject.Object();
            
            // TODO bio support
            TSendingCapabilities requiredCapabilities = KCapabilitiesForAllServices;
            requiredCapabilities.iFlags =       TSendingCapabilities::ESupportsAttachments;
            
            if ( obj->MediaType() == EMsgMediaText )
                {
                requiredCapabilities.iFlags |= TSendingCapabilities::ESupportsBodyText;
                }
            
            if ( iDialogType != EViewer )
                {
                uids->AppendL( KSenduiMtmMmsUid );   // MMS menu item, if already in editor
                }

            CMessageData* msgData = CMessageData::NewLC();
            RFile fileHandle; 
            if ( obj->AttachmentId() == KMsvNullIndexEntryId )
                {
                fileHandle = CUniDataUtils::GetAttachmentFileL( iDataModel.Mtm(), obj->AttachmentId() );
                }
            else
                {
                // Always pass full file path to SendUi. Even
                // in case of attachments.
                CMsvStore* store = iMessageEntry->ReadStoreL();
                CleanupStack::PushL( store );
                MMsvAttachmentManager& manager = store->AttachmentManagerL();
                fileHandle = manager.GetAttachmentFileL( obj->AttachmentId() );
                CleanupStack::PopAndDestroy( store ); 
                }
            CleanupClosePushL( fileHandle );

            TUid bioUid;
            bioUid.iUid = 0;
            if ( fileHandle.SubSessionHandle() )
                {
                msgData->AppendAttachmentHandleL( fileHandle );
                if ( obj->MediaInfo() )
                    {
                    if ( BioMessageTypeL( obj->MediaInfo()->MimeType(), bioUid ) )
                        {
                        requiredCapabilities.iFlags |= TSendingCapabilities::ESupportsBioSending;
                        }
                    }
                }

            TUid serviceId = iSendUi->ShowSendQueryL( msgData, 
                                                      requiredCapabilities, 
                                                      uids );
            ProcessCommandSendL( *msgData, index, serviceId, bioUid );

            CleanupStack::PopAndDestroy( 3, uids ); // + fileHandle, msgData
            }
            break;
        
        case EEikCmdExit:
        case EAknCmdExit:
            iExitCmd = EUniObjectsViewExit;
            // Fallthrough
        default:
                // call parent class to process other commands.
            CMsgViewAttachmentsDialog::ProcessCommandL( aCommandId ); 
            break;
        }

    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::ProcessCommandOpenL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::ProcessCommandOpenL( TInt aIndex )
    {
    CUniObjectsInfo& selectedObject = 
        static_cast<CUniObjectsInfo&>( iAttachmentModel.AttachmentInfoAt( aIndex ) );
    CUniObject* obj = selectedObject.Object();    
    
    if ( obj->MediaType() == EMsgMediaText )
        {
        if ( !iBusy )
            {
            iBusy = ETrue;
            CleanupStack::PushL( TCleanupItem( CleanupPointer, &iBusy ) );
            
            TUint mibCharset = obj->MimeInfo()->Charset();
            if ( !mibCharset )
                {
                //assume US-ASCII - mandated by RFC 2046
                mibCharset = KCharacterSetMIBEnumUsAscii;
                }
            TUint charconvCharsetID = iDataModel.DataUtils().MibIdToCharconvIdL( mibCharset );

            RFile file = CUniDataUtils::GetAttachmentFileL( iDataModel.Mtm(), obj->AttachmentId() );
            CleanupClosePushL( file );
            // Takes ownership of "file":
            CNotepadApi::ExecFileViewerL( file, 
                                      NULL,
                                      ETrue,
                                      EFalse,
                                      charconvCharsetID );
            CleanupStack::Pop( &file ); 
            
            // sets iBusy to EFalse.
            CleanupStack::PopAndDestroy(); // CleanupPointer
            }       
        }
    else
        {
        if ( iDialogType == EViewer )
            {
            if ( selectedObject.IsSaved() )
                {
                SetOpenParamList( NULL );
                }
            else if ( !OpenParamList() )
                {
                CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
				//Applications that require Media Player to show the save 
                //option must set the EGenericParamAllowSave to ETrue. 
                //If they don't need the save option there is no need to set 
                //the parameter to EFalse.
                TAiwVariant allowSaveVariant( ETrue );
                TAiwGenericParam genericParamAllowSave( EGenericParamAllowSave,
                                                               allowSaveVariant );
                paramList->AppendL( genericParamAllowSave );      
                //paramList->AppendL( EGenericParamAllowSave );
                SetOpenParamList( paramList );
                CleanupStack::Pop( paramList );
                }
            }
                        
        CMsgViewAttachmentsDialog::ProcessCommandL( EMsgViewAttachmentCmdOpen ); 
        }
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::ProcessCommandSaveL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::ProcessCommandSaveL( TInt aIndex )
    {
    CUniObjectsInfo& selectedObject = static_cast<CUniObjectsInfo&>
        ( iAttachmentModel.AttachmentInfoAt( aIndex ) );
    CUniObject* obj = selectedObject.Object();
    if ( obj->MediaType() == EMsgMediaText )
        {
        // Save text files directly to Notepad.
        // This is needed because automatic character set recognation
        // is not too reliable.
        TUint mibCharset = obj->MimeInfo()->Charset();
        if ( !mibCharset )
            {
            //assume US-ASCII - mandated by RFC 2046
            mibCharset = KCharacterSetMIBEnumUsAscii;
            }
        TUint charconvCharsetID = iDataModel.DataUtils().MibIdToCharconvIdL( mibCharset );

        RFile file = CUniDataUtils::GetAttachmentFileL( iDataModel.Mtm(), obj->AttachmentId() );
        CleanupClosePushL( file );
        CNotepadApi::SaveFileAsMemoL(
            file, 
            charconvCharsetID );
        CleanupStack::PopAndDestroy(); // file

        // Show confirmation note
        RApaLsSession appList;
        if ( appList.Connect() == KErrNone )
            {
            TApaAppInfo appInfo;
            if ( appList.GetAppInfo( appInfo, KNotepadUID3 ) == KErrNone )
                {
                HBufC* text = StringLoader::LoadLC( R_UNUT_FILE_SAVED_TO, appInfo.iCaption );
                CAknConfirmationNote* note = new ( ELeave ) CAknConfirmationNote( ETrue );
                note->ExecuteLD( *text );
                CleanupStack::PopAndDestroy( text );
                }
            appList.Close();
            }

        // Set saved information
        selectedObject.SetSaved( ETrue );
        if ( obj ) 
            {
            obj->SetSaved( ETrue );
            }
        }
    else
        {
        CMsgViewAttachmentsDialog::ProcessCommandL( EMsgViewAttachmentCmdSave ); 

        // Set saved information
        if (    selectedObject.IsSaved( )
            &&  obj )
            {
            obj->SetSaved( ETrue );
            }
        }
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::ProcessCommandRemoveL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::ProcessCommandRemoveL( TInt aIndex )
    {
    if ( ( aIndex != -1 ) && iAttachmentModel.NumberOfItems() )
        {
        CMsgAttachmentInfo& attInfo = iAttachmentModel.AttachmentInfoAt( aIndex );

        // Get the file name from atta info full path name.
        TParsePtrC parser( attInfo.FileName() );
        TFileName fileName2( parser.NameAndExt() );

        if ( fileName2.Length() == 0 )
            {
            fileName2 = iDataModel.DataUtils().DefaultFileName();
            }

        HBufC* prompt = StringLoader::LoadLC( 
            R_UNUT_QUERY_COMMON_CONF_REMOVE, 
            fileName2, 
            iCoeEnv );

        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        TInt ret = dlg->ExecuteLD( R_UNUT_REMOVE_OBJECT_QUERY, *prompt );

        CleanupStack::PopAndDestroy( prompt );

        if ( ret )
            {
            TBool removingLastOfSlide = EFalse;
            TBool setSeparatorToNext = EFalse;
            if ( iDataModel.SmilModel().SlideObjectCount( SlideNumber( aIndex ) ) == 1 )
                {
                removingLastOfSlide = ETrue;
                }
            else if ( ( ( aIndex + 1 ) < iAttachmentModel.NumberOfItems() ) &&
                      ( SlideNumber( aIndex ) == SlideNumber( aIndex + 1 ) ) )
                {
                CUniObjectsInfo& object = static_cast<CUniObjectsInfo&>
                    ( iAttachmentModel.AttachmentInfoAt( aIndex ) );
                if ( object.IsSeparator() )
                    {
                    setSeparatorToNext = ETrue;
                    }
                }

            // updates listbox after calling RemoveAttachmentL.
            CMsgViewAttachmentsDialog::ProcessCommandL( EMsgViewAttachmentCmdRemove );

            if ( removingLastOfSlide )
                {
                //removing the last object of the slide
                CUniObjectsModel& objModel = 
                    static_cast<CUniObjectsModel&>( iAttachmentModel );

                CUniObjectsInfo* info = objModel.AddEmptySlideL(
                    iDataModel.DataUtils().EmptyPageString(),
                    aIndex );
                if ( aIndex )
                    {
                    info->SetSeparator( ETrue );
                    }
                ItemAddedL( aIndex );
                }
            else if ( setSeparatorToNext )
                {
                CUniObjectsInfo& object2 = static_cast<CUniObjectsInfo&>
                    ( iAttachmentModel.AttachmentInfoAt( aIndex ) );
                object2.SetSeparator( ETrue );
                }
            }
        }
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::ProcessCommandSendL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::ProcessCommandSendL( CMessageData&   aMsgData, 
                                                 TInt aIndex, 
                                                 TUid aServiceId,
                                                 TUid aBioUid )
    {
    CUniObjectsInfo& selectedObject = static_cast<CUniObjectsInfo&>
        ( iAttachmentModel.AttachmentInfoAt( aIndex ) );
    CUniObject* obj = selectedObject.Object();

    TFileName fileName;
    CRichText* text = NULL;
    CFileMan* fileManager = NULL;
    TSendingCapabilities serviceCapabilities;
    iSendUi->ServiceCapabilitiesL(  aServiceId,
                                    serviceCapabilities);
    TInt supportsBodyText( serviceCapabilities.iFlags & TSendingCapabilities::ESupportsBodyText );

    if (    // send as body text
            (   obj->MediaType() == EMsgMediaText 
            &&  supportsBodyText )
            // send as atta
        ||  (   obj->MediaType() == EMsgMediaText &&
                obj->MimeInfo()->Charset() != KCharacterSetMIBEnumUTF16BE &&
                obj->MimeInfo()->Charset() != KCharacterSetMIBEnumUTF16LE &&
                obj->MimeInfo()->Charset() != KCharacterSetMIBEnumUTF16 ) )
        {
        aMsgData.ClearAttachmentArray();
        
        fileManager = CFileMan::NewL( iEikonEnv->FsSession() );
        // CSa2
        CleanupStack::PushL( fileManager );
        CreateTempTextFileL( obj, fileName );
        if ( supportsBodyText )
            {
            // send as body
            text = CRichText::NewL( iEikonEnv->SystemParaFormatLayerL(), 
                                    iEikonEnv->SystemCharFormatLayerL(), 
                                    CEditableText::EFlatStorage );
            // CSa3
            CleanupStack::PushL(text);
            text->ImportTextFileL(0,fileName,CPlainText::EOrganiseByParagraph);

            // does not take ownership
            aMsgData.SetBodyTextL( text );
            }
        else
            {
            aMsgData.AppendAttachmentL( fileName );
            }
        }
    // else - aMsgData has already the needed handle
    
    TRAPD( err, iSendUi->CreateAndSendMessageL( aServiceId, &aMsgData, aBioUid ) );
    if ( text )
        {
        CleanupStack::PopAndDestroy( text );
        }
    if ( fileManager )
        {
        // Remove temp file & dir
        fileManager->Delete( fileName );
        fileManager->RmDir( fileName ); // remove directory
        CleanupStack::PopAndDestroy( fileManager ); 
        }
    User::LeaveIfError( err );
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::CreateTempTextFileL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::CreateTempTextFileL( CUniObject* aObject, TFileName& aTempFileName )
    {
    CreateTempPathL( aObject->MediaInfo()->FullFilePath(), aTempFileName );

    TUint mibCharset = aObject->MimeInfo()->Charset();
    if ( !mibCharset )
        {
        //assume US-ASCII - mandated by RFC 2046
        mibCharset = KCharacterSetMIBEnumUsAscii;
        }
    TUint charconvCharsetID = iDataModel.DataUtils().MibIdToCharconvIdL( mibCharset );

    // Import text
    CPlainText* text = CPlainText::NewL();
    CleanupStack::PushL( text );

    RFile file = CUniDataUtils::GetAttachmentFileL( iDataModel.Mtm(), aObject->AttachmentId() );
    CleanupClosePushL( file );

    RFileReadStream input_stream( file );
    CleanupClosePushL( input_stream );
    
    CPlainText::TImportExportParam param;
    param.iForeignEncoding = charconvCharsetID;
    CPlainText::TImportExportResult result;
    text->ImportTextL( 0, input_stream, param, result );

    CleanupStack::PopAndDestroy( 2 ); // input_stream, file
    
    // Export text as UCS-2
    text->ExportAsTextL( aTempFileName, CPlainText::EOrganiseByParagraph, 0 );
    CleanupStack::PopAndDestroy(); //text
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::CreateTempPathL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::CreateTempPathL( const TDesC& aFileName, TFileName& aTempFileName )
    {
    // Create temp path
    TParsePtrC parser( aFileName );
    MsgAttachmentUtils::GetMsgEditorTempPath( aTempFileName );
    aTempFileName.Append( KMmsFormatTwoDigits );
    TTime time;
    time.UniversalTime();
    TBuf<KMaxFileName> tempDir;
    tempDir.Format( aTempFileName, I64HIGH( time.Int64() ), I64LOW( time.Int64() ) );
    iEikonEnv->FsSession().MkDir( tempDir );
    aTempFileName = tempDir;
    aTempFileName.Append( parser.NameAndExt() );
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::OfferKeyEventL
//
// Handles key events. 
// ---------------------------------------------------------
//
TKeyResponse CUniObjectsViewDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                    TEventCode aType )
    {
    if ( aType == EEventKey && !MenuShowing() )
        {
        CUniObjectsInfo& selectedObject = static_cast<CUniObjectsInfo&>
            ( iAttachmentModel.AttachmentInfoAt( CurrentListItemIndex() ) );

        TBool emptySlide = ( selectedObject.Flags() & CUniObjectsInfo::EMmsSlide );
        switch ( aKeyEvent.iCode )
            {
            case EKeyEnter:
            case EKeyOK:
                if ( !emptySlide )
                    {
                    ProcessCommandL(EMsgViewAttachmentCmdOpen);
                    }
                return EKeyWasConsumed;

            default:
                break;
            }

        switch ( aKeyEvent.iScanCode )
            {
            case EStdKeyBackspace:
            case EStdKeyDelete:
                {
                if ( ( iDialogType == EEditor && 
                       !emptySlide ) ||
                     ( iDialogType == EForward && 
                       iDataModel.AttachmentList().Count() != 0 &&
                       SlideNumber( CurrentListItemIndex() == SlideNumber( iAttachmentModel.NumberOfItems() - 1 ) ) ) )
                    {
                    // Delete object if in editor mode and something else that empty slide is focuse.
                    // On forward mode deleting is enabled when attachment is focused.
                    ProcessCommandL(EMsgViewAttachmentCmdRemove);
                    }
                // else - No operation
                return EKeyWasConsumed;
                }
            case EStdKeyYes:
            case EStdKeyNo:
                {
                // No operation
                return EKeyWasConsumed;
                }
            default:
                {
                // No operation by default
                break;
                }
            }
        }

    return CMsgViewAttachmentsDialog::OfferKeyEventL(aKeyEvent, aType);
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::DynInitMenuPaneL( TInt aMenuId, 
                                              CEikMenuPane* aMenuPane)
    {
    CMsgViewAttachmentsDialog::DynInitMenuPaneL( aMenuId, aMenuPane );

    TInt index = CurrentListItemIndex();
    TInt currentSlide = SlideNumber( index );

    if ( !(iSupportedFeatures & EUniFeatureHelp) )
        {
        aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
        }

    switch ( aMenuId )
        {
        case R_MEB_ATTACHMENTS_MENUPANE:
            if ( index == -1 || ! iAttachmentModel.NumberOfItems() )
                {
                // no selection or empty list.

                aMenuPane->SetItemDimmed( EMsgViewAttachmentCmdOpen, ETrue );

                if ( iDialogType == EEditor )
                    {
                    aMenuPane->SetItemDimmed( EMsgViewAttachmentCmdRemove, ETrue );
                    }
                else if ( iDialogType == EViewer )
                    {
                    aMenuPane->SetItemDimmed( EMsgViewAttachmentCmdSave, ETrue );
                    }
                }
            else
                {
                CUniObjectsInfo& selectedObject = static_cast<CUniObjectsInfo&>
                    ( iAttachmentModel.AttachmentInfoAt( index ) );

                if ( iDialogType == EEditor )
                    {
                    if ( selectedObject.Flags() & CUniObjectsInfo::EMmsSlide )
                        {
                        aMenuPane->SetItemDimmed( EMsgViewAttachmentCmdOpen, ETrue );
                        aMenuPane->SetItemDimmed( EMsgViewAttachmentCmdRemove, ETrue );
                        }
                    TUniLayout layout = iDataModel.SmilModel().Layout();
                    if ( layout == EUniTextFirst )
                        {
                        aMenuPane->SetItemDimmed( EMmsObjectsTextFirst, ETrue );
                        }
                    else if ( layout == EUniImageFirst )
                        {
                        aMenuPane->SetItemDimmed( EMmsObjectsTextSecond,  ETrue );
                        }
                    
                    // either image or text object present but no both: no order menus.
                    // Not shown on Japanese variant ever.
                    if ( !IsImageAndTextPresent() ||
                         iSupportedFeatures & EUniFeatureJapanese )
                        {
                        aMenuPane->SetItemDimmed( EMmsObjectsTextFirst, ETrue );
                        aMenuPane->SetItemDimmed( EMmsObjectsTextSecond, ETrue );
                        }
                    }
                else if ( iDialogType == EViewer )
                    {
                    CUniObject* obj = selectedObject.Object();
                    TBool showSendMenu = ETrue;

                    if ( selectedObject.Flags() & CUniObjectsInfo::EMmsSlide )
                        {
                        aMenuPane->SetItemDimmed( EMsgViewAttachmentCmdOpen, ETrue );
                        aMenuPane->SetItemDimmed( EMsgViewAttachmentCmdSave, ETrue );
                        showSendMenu = EFalse;
                        }
                    if ( index != -1 )
                        {
                        // Catch this leave. If something peculiar coming in msg headers
                        // and atta not natively handled by viewer CanSave might leave.
                        // ETC. application/x-epocinstall
                        TBool canSave = EFalse;
                        TRAPD( err, canSave = iDocHandler->CanSaveL( selectedObject.DataType() ) );
                        if ( err )
                            {
                            canSave = EFalse;
                            }
                        if ( !canSave )
                            {
                            aMenuPane->SetItemDimmed( EMsgViewAttachmentCmdSave, ETrue );
                            }

                        // Send menu if full OMA DRM and mime type does not belong to CCL
                        if ( !obj ||  
                            ( ! ( iSupportedFeatures & EUniFeatureDrmFull ) && 
                                  obj->MediaInfo()->Protection() != EFileProtNoProtection )
                            // SD closed content objects can be forwarded.
                            // UI spec allows FL items to be sent from objects view,
                            // however, Send UI blocks sending.
                            ||  ( iSupportedFeatures & EUniFeatureDrmFull && 
                                obj->MediaInfo()->Protection() == EFileProtClosedContent ) )
                              {
                            showSendMenu = EFalse;
                            }
                        }

                    if ( iSendUi && showSendMenu )
                        {
                        TInt pos = 0;
                        aMenuPane->ItemAndPos( EAknCmdExit, pos );

                        if ( iSupportedFeatures & EUniFeatureHelp )
                            {
                            pos -= 1;
                            }
                        // TSendingCapabilities::ESupportsAttachments is enough
                        // although bio and body text are possible but they are 
                        // subset of attachment support
                        iSendUi->AddSendMenuItemL(
                            *aMenuPane,
                            pos,
                            EMmsObjectsSendVia,
                            TSendingCapabilities( 
                                0, 
                                0, 
                                TSendingCapabilities::ESupportsAttachments )
                            );
                        }
                    }
                else if ( iDialogType == EForward )
                    {
                    // show remove for attachments...
                    if ( !( iDataModel.AttachmentList().Count() &&
                            currentSlide == SlideNumber( iAttachmentModel.NumberOfItems() - 1 ) ) )
                        {
                        // the attachments are in the last "slide"
                        aMenuPane->SetItemDimmed( EMsgViewAttachmentCmdRemove, ETrue );
                        }
                    }
                }
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::HandleServerAppExit
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::HandleServerAppExit( TInt aReason )
    {
    CMsgViewAttachmentsDialog::HandleServerAppExit( aReason );
 
    TInt currentIndex( CurrentListItemIndex() );
    
    CUniObjectsInfo& selectedObject = 
        static_cast<CUniObjectsInfo&>( iAttachmentModel.AttachmentInfoAt( currentIndex ) );
    CUniObject* obj = selectedObject.Object();
    
    CMsgAttachmentInfo::TDRMDataType drmType = CMsgAttachmentInfo::ENoLimitations;
    TRAPD( error, drmType = static_cast<CMsgAttachmentInfo::TDRMDataType>( GetObjectsInfoDRMTypeL( *obj ) ) );
                        
    if ( error == KErrNone && 
         drmType != selectedObject.DRMDataType() )
        {
        selectedObject.SetDRMDataType( drmType );
        UpdateIcon( currentIndex );
        }
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::SlideNumber
// ---------------------------------------------------------
//
TInt CUniObjectsViewDialog::SlideNumber( TInt aIndex )
    {
    TInt currentSlide = 0;
    for ( TInt i = 0; i <= aIndex; i++ )
        {
        CUniObjectsInfo& object = static_cast<CUniObjectsInfo&>
            ( iAttachmentModel.AttachmentInfoAt( i ) );
        if ( object.IsSeparator() )
            {
            currentSlide++;
            }
        }
    return currentSlide;
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::IsImageAndTextPresent
// ---------------------------------------------------------
//
TBool CUniObjectsViewDialog::IsImageAndTextPresent()
    {
    TBool textFound = EFalse;
    TBool imageFound = EFalse;
    TInt slides = iDataModel.SmilModel().SlideCount();
    for ( TInt i = 0; i < slides && !( textFound && imageFound ); i++ )
        {
        if ( !textFound && iDataModel.SmilModel().GetObject( i, EUniRegionText ) )
            {
            textFound = ETrue;
            }
        if ( !imageFound && iDataModel.SmilModel().GetObject( i, EUniRegionImage ) )
            {
            imageFound = ETrue;
            }
        }
    return ( textFound && imageFound );
    }


// ---------------------------------------------------------
// CUniObjectsViewDialog::RemoveAttachmentL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::RemoveAttachmentL( TInt aIndex )
    {
    CUniObjectsModel& objModel = static_cast<CUniObjectsModel&>( iAttachmentModel );

    // delete item from attachment model (notifies its observer about deletion).
    objModel.DeleteObjectL( aIndex, ETrue );
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::ChangeOrderL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::ChangeOrderL( TInt aCommandId, 
                                          TInt /*aNewPosition*/ )
    {
    CUniObjectsModel& objModel = static_cast<CUniObjectsModel&>( iAttachmentModel );

    TInt count = iAttachmentModel.NumberOfItems();
    TInt currentSlide = 0;
    TInt i = 0;

    while ( i < count )
        {
        CUniObjectsInfo& firstObject = static_cast<CUniObjectsInfo&>
            ( iAttachmentModel.AttachmentInfoAt( i ) );
        i++;
        if ( i < count )
            {
            if ( currentSlide == SlideNumber( i ) )
                {
                CUniObjectsInfo& secondObject = static_cast<CUniObjectsInfo&>
                    ( iAttachmentModel.AttachmentInfoAt( i ) );
                if ( secondObject.Type() != MsgAttachmentUtils::EAudio )
                    {
                    if ( firstObject.IsSeparator() )
                        {
                        firstObject.SetSeparator( EFalse );
                        secondObject.SetSeparator( ETrue );
                        }
                    objModel.ChangeOrderL( i, i - 1 );
                    iListBox->HandleItemAdditionL();
                    i++;
                    if ( i < count )
                        {
                        if ( currentSlide == SlideNumber( i ) )
                            {
                            // there was 3rd object on the same slide
                            i++;
                            }
                        }
                    }
                else
                    {
                    // second object was audio
                    // its always the last object of the slide
                    i++;
                    }
                }
            }
        currentSlide++;
        }
    
    TUniLayout layout( EUniTextFirst );
    if ( aCommandId == EMmsObjectsTextSecond )
        {
        layout = EUniImageFirst;
        }
    objModel.NotifyChangeOrderL( layout );
    }

// ----------------------------------------------------
// CUniObjectsViewDialog::LaunchHelpL
// launch help using context
// ----------------------------------------------------
//
void CUniObjectsViewDialog::LaunchHelpL()
    {
    if ( iSupportedFeatures & EUniFeatureHelp )
        {
        CCoeAppUi* editorAppUi = static_cast<CCoeAppUi*>( ControlEnv()->AppUi() );

        CArrayFix<TCoeHelpContext>* helpContext = editorAppUi->AppHelpContextL();
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
        }
    }

// ----------------------------------------------------
// CUniObjectsViewDialog::GetHelpContext
// returns helpcontext as aContext
// ----------------------------------------------------
//
void CUniObjectsViewDialog::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    if ( iSupportedFeatures & EUniFeatureHelp )
        {
        if ( iDialogType == EViewer )
            {
            aContext.iContext = KMMS_HLP_OBJECTS_VIEW_VIEWER;
	        aContext.iMajor = KUidMmsViewer;
            }
        else
            {
            aContext.iContext = KUNIFIED_HLP_ED_OBJECTS;
			aContext.iMajor = KUniEditorAppId; 
            }
        }
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::BioMessageTypeL
// ---------------------------------------------------------
//
TBool CUniObjectsViewDialog::BioMessageTypeL( TPtrC8    aMimeType,
                                              TUid&     aBioUid )
    {
    // Check bio message type
    TBool isBio = EFalse;
    HBufC* sample = HBufC::NewLC( aMimeType.Length() );
    TPtr samplePtr = sample->Des();
    samplePtr.Copy( aMimeType );
    
    // need to create local RFs for BIO otherwise raises exception
    RFs& fs( CCoeEnv::Static()->FsSession() );
    CBIODatabase* bioDB = CBIODatabase::NewLC( fs );

    // IsBioMessageL returns KErrNone if found or KErrNotFound if not found 
    if ( bioDB->IsBioMessageL( EBioMsgIdIana, samplePtr, NULL, aBioUid ) == KErrNone)
        {
        isBio = ETrue;
        }
    CleanupStack::PopAndDestroy( 2, sample ); // + bioDb

    return isBio;
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::PopulateAttachmentModelL
//
// Creates new attachment model from smilmodel content for 
// objectsview usage
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::PopulateAttachmentModelL( CUniObjectsModel& aObjectsViewModel,
                                                      CUniDataModel& aUniDataModel )
    {
    TInt slideCount = 0;
    TUniSmilType smilType( aUniDataModel.SmilType() );
    
    if ( smilType == EMmsSmil ||
         smilType == ENoSmil ||
         smilType == EMultipleSmil )
        {
        slideCount = aUniDataModel.SmilModel().SlideCount();
        // Populate attamodel from smilmodel
        for ( TInt i = 0; i < slideCount; i++ )
            {
            if ( !aUniDataModel.SmilModel().SlideObjectCount( i ) )
                {
                CUniObjectsInfo* info = aObjectsViewModel.AddEmptySlideL(
                                            aUniDataModel.DataUtils().EmptyPageString() );
                if ( i ) // no separator for first slide
                    {
                    info->SetSeparator(ETrue);
                    }
                }
                
            for ( TInt ii = 0; ii < aUniDataModel.SmilModel().SlideObjectCount( i ); ii++ )
                {
                CUniObjectsInfo* info = CreateObjectsInfoL( 
                                            aObjectsViewModel,
                                            *aUniDataModel.SmilModel().GetObjectByIndex( i, ii ),
                                            EFalse,
                                            i );
                // Slide separators. Drawn on top of list item -> needs to be drawn for
                // first item in slide. Don't draw for the first slide.
                if ( info && i > 0 && ii == 0 )
                    {
                    info->SetSeparator( ETrue );
                    }
                }
            }
        }
    else
        {
        slideCount = Max( aUniDataModel.ObjectList().Count(), 1 ); 
        for ( TInt j = 0; j < aUniDataModel.ObjectList().Count(); j++ )
            {
            CreateObjectsInfoL( aObjectsViewModel, 
                                *aUniDataModel.ObjectList().GetByIndex( j ),
                                EFalse,
                                0 );
            }
        }

    //add attachments to the end
    for ( TInt k = 0; k < aUniDataModel.AttachmentList().Count(); k++ )
        {
        CUniObjectsInfo* info = CreateObjectsInfoL( aObjectsViewModel, 
                                                    *aUniDataModel.AttachmentList().GetByIndex( k ),
                                                    ETrue,
                                                    -1 );
        info->SetSupported( EFalse );
        if ( k == 0 && slideCount )
            {
            info->SetSeparator( ETrue );
            }
        }
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::CreateObjectsInfoL
//
// Creates CUniObjectsInfo object ands it to objects view
// model. Objects view model has the ownership of the
// new CUniObjectsInfo object.
// ---------------------------------------------------------
//
CUniObjectsInfo* CUniObjectsViewDialog::CreateObjectsInfoL( CUniObjectsModel& aObjectsViewModel,
                                                            CUniObject& aObject,
                                                            TBool aAttachmentObject,
                                                            TInt aSlideNumber )
    {
    CUniObjectsInfo* info = NULL;
    TInt drmType = GetObjectsInfoDRMTypeL( aObject );
    info = aObjectsViewModel.AddObjectL( aObject, drmType, aAttachmentObject, aSlideNumber );

    TBool supported = ETrue;
    switch ( aObject.MediaType() )
        {
        case EMsgMediaText:
            info->SetType( MsgAttachmentUtils::ENote );
            break;
        case EMsgMediaImage:
            info->SetType( MsgAttachmentUtils::EImage );
            break;
        case EMsgMediaVideo:
            info->SetType( MsgAttachmentUtils::EVideo );
            break;
        case EMsgMediaAudio:
            info->SetType( MsgAttachmentUtils::EAudio );
            break;
#ifdef RD_SVGT_IN_MESSAGING            
        case EMsgMediaSvg:
            {
            info->SetType( MsgAttachmentUtils::ESVG );
            break;
            }
#endif
        default:
            info->SetType( MsgAttachmentUtils::EUnknown );
            supported = EFalse;
            break;
        }
    info->SetSupported( supported );
    return info;
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::GetObjectsInfoDRMTypeL
// ---------------------------------------------------------
//
TInt CUniObjectsViewDialog::GetObjectsInfoDRMTypeL( CUniObject& aObject )
    {
    if ( aObject.DrmInfo() &&
         ( aObject.MediaInfo()->Protection() &
           ( EFileProtForwardLocked | EFileProtSuperDistributable ) ) )
        {
        // Changed EFalse->ETrue. 
        // Use case to motivate this: object has not no rights -> Objects view ->
        // player launched -> rights are retrieved -> back to objects view ->
        // back to viewer -> object view again: in this phase wrong icon
        // would be shown unless rights are resolved again
        if ( aObject.DrmInfo()->RightsValidL( ETrue ) )
            {
            if ( aObject.MediaInfo()->Protection() & EFileProtForwardLocked )
                {
                return CMsgAttachmentInfo::EForwardLockedOrCombinedDelivery;
                }
            else // EMmsSuperDistributable
                {
                return CMsgAttachmentInfo::ESeparateDeliveryValidRights;
                }
            }
        else
            {
            if ( aObject.MediaInfo()->Protection() & EFileProtForwardLocked )
                {
                return CMsgAttachmentInfo::ECombinedDeliveryInvalidRights;
                }
            else // EMmsSuperDistributable
                {
                return CMsgAttachmentInfo::ESeparateDeliveryInvalidRights;
                }
            }
        }
    return CMsgAttachmentInfo::ENoLimitations;
    }

// ---------------------------------------------------------
// CUniObjectsViewDialog::GetObjectsInfoDRMTypeL
// ---------------------------------------------------------
//
void CUniObjectsViewDialog::HandleUniObjectEvent(   CUniObject&  aUniObject,
                                                    TUniObjectEvent aUniEvent )
    {
    UNILOGGER_ENTERFN( "CUniObjectsViewDialog::HandleUniObjectEvent" );
    if (    aUniEvent == EUniObjectEventDrmRightsChanged
        &&  aUniObject.DrmInfo()
        &&  (   (   IsDrmRightsWhenLoaded( aUniObject )
                &&  !IsRightsNow( aUniObject ) )
            ||  (   !IsDrmRightsWhenLoaded( aUniObject )
                &&  IsRightsNow( aUniObject ) )
            )
        )
        {        
        CMsgAttachmentInfo::TDRMDataType drmType = CMsgAttachmentInfo::ENoLimitations;
        if ( IsRightsNow( aUniObject ) )
            {
            if ( aUniObject.MediaInfo()->Protection() & EFileProtForwardLocked )
                {
                drmType = CMsgAttachmentInfo::EForwardLockedOrCombinedDelivery;
                }
            else // EMmsSuperDistributable
                {
                drmType = CMsgAttachmentInfo::ESeparateDeliveryValidRights;
                }
            }
        else
            {
            if ( aUniObject.MediaInfo()->Protection() & EFileProtForwardLocked )
                {
                drmType = CMsgAttachmentInfo::ECombinedDeliveryInvalidRights;
                }
            else // EMmsSuperDistributable
                {
                drmType = CMsgAttachmentInfo::ESeparateDeliveryInvalidRights;
                }
            }
        UNILOGGER_WRITEF( _L("CUniObjectsViewDialog::HandleUniObjectEvent: drmType = %d "), drmType );

        // when 'DRM rights when loaded info'
        aUniObject.SetDrmRightsWhenLoadedObjects( IsRightsNow( aUniObject ) );

        // search for the CUniObjectsInfo 
        CUniObjectsModel& model( static_cast<CUniObjectsModel&>( iAttachmentModel ) );
        TInt countObjects = model.NumberOfItems( );
        for ( TInt i = 0; i < countObjects; i++ )
            {
            CUniObjectsInfo&  uniObjectInfo =
                static_cast<CUniObjectsInfo&>( model.AttachmentInfoAt( i ) );

            CUniObject* obj = uniObjectInfo.Object();
            if ( obj == &aUniObject )
                {
                if ( drmType != uniObjectInfo.DRMDataType() )
                    {
                    UNILOGGER_WRITE( "CUniObjectsViewDialog::HandleUniObjectEvent setting new icon " );
                    uniObjectInfo.SetDRMDataType( drmType );
                    }
                }
            }
        DrawNow( );
        }
    }
    
// ---------------------------------------------------------
// IsRightsNow
// ---------------------------------------------------------
//
TBool CUniObjectsViewDialog::IsRightsNow( CUniObject& aObject ) const
    {
    TBool rightsNow( EFalse );
    if (  aObject.DrmInfo( ) )
        {
        TInt consumed( EFalse );
        // KErrNone: rights exist
        rightsNow = !aObject.DrmInfo( )->EvaluateRights( consumed );
        }
    UNILOGGER_WRITEF( _L("CUniObjectsViewDialog::IsRightsNow: rightsNow = %d "), rightsNow );
    return rightsNow;
    }
    
// ---------------------------------------------------------
// IsDrmRightsWhenLoaded
// ---------------------------------------------------------
//
TBool CUniObjectsViewDialog::IsDrmRightsWhenLoaded( CUniObject& aObject ) const
    {
    // Separate function just because of LOG traces
    TBool rightsWhenLoaded = aObject.IsDrmRightsWhenLoadedObjects();
    UNILOGGER_WRITEF( _L("CUniObjectsViewDialog::IsDrmRightsWhenLoaded: rightsWhenLoaded = %d "), rightsWhenLoaded );
    return rightsWhenLoaded;
    }

// End of File
