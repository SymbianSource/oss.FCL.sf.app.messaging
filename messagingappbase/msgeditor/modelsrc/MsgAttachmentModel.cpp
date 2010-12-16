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
* Description:  MsgAttachmentModel implementation
*
*/



// ========== INCLUDE FILES ================================
#include <eikenv.h>
#include <gulicon.h>
#include <apgicnfl.h>           // apamasked bitmaps
#include <coeutils.h>           // for ConeUtils::FileExists
#include <DocumentHandler.h>    // for KGameMimeType
#include <data_caging_path_literals.hrh>
#include <msvids.h>             // KMsvNullIndexEntryId

#include <AknUtils.h>
#include <AknsUtils.h>          // skinned icons
#include <AknsConstants.h>      // skinned icon ids
#include <aknlayout.cdl.h> // LAF

#include <msgeditor.mbg>
#include <msgeditor_ah.mbg>

#include "MsgEditorCommon.h"
#include "MsgAttachmentModel.h"
#include "MsgAttachmentModelObserver.h"
#include "MsgAttachmentInfo.h"
#include "MsgAttachmentUtils.h"

#include "MsgEditorLogging.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

const TUint KAttachArrayGranularity = 16;

const TInt32 KMaxGameEngineDataID = 1000;
const TInt   KGameEngineUID = 0x101F5EDA;

/**
* UID of the messaging centre to show correct icon for email
* attachments
*/
const TInt KMceUID = 0x100058C5;
_LIT8( KEMailMessageMimeType, "message/rfc822" );

_LIT8( KTextDataType, "text/plain" );
const TInt KNotepadUID3 = 0x1000599d;


// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgAttachmentModel::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CMsgAttachmentModel* CMsgAttachmentModel::NewL( TBool aReadOnly )
    {
    CMsgAttachmentModel* self = new ( ELeave ) CMsgAttachmentModel( aReadOnly );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::~CMsgAttachmentModel
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgAttachmentModel::~CMsgAttachmentModel()
    {
    delete iDRMFWDLockIcon;
    delete iDRMSDIconRightsValid;
    delete iDRMFWDLockIconNoRights;
    delete iDRMSDIconNoRights;

    if ( iAttachInfoArray )
        {
        iAttachInfoArray->ResetAndDestroy();
        }
    delete iAttachInfoArray;

    iAppList.Close();
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::SetObserver
//
// Sets attachment model observer.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentModel::SetObserver(
    MMsgAttachmentModelObserver* aObserver )
    {
    iAttachmentModelObserver = aObserver;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::NumberOfItems
//
// Returns number of attachments.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgAttachmentModel::NumberOfItems() const
    {
    return iAttachInfoArray->Count();
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::AttachmentInfoAt
//
// Returns reference to attachment info at given index.
// ---------------------------------------------------------
//
EXPORT_C CMsgAttachmentInfo& CMsgAttachmentModel::AttachmentInfoAt(
    TInt aIndex ) const
    {
    return *iAttachInfoArray->At( aIndex );
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::BitmapForFileL
//
// Returns pointer to bitmap of the application that handles given file.
// If application not found returns default bitmap and sets supported flag
// of the aAttaInfo to EFalse.
// ---------------------------------------------------------
//
EXPORT_C CGulIcon* CMsgAttachmentModel::BitmapForFileL(
    CMsgAttachmentInfo& aAttaInfo )
    {
    MEBLOGGER_ENTERFN( "BitmapForFileL" );

    TUid appUid( KNullUid );
    // Get bitmap based on datatype:
    TInt err = iAppList.AppForDataType( aAttaInfo.DataType(), appUid );
    /*
    // Get bitmap based on recognition:
    // How to handle this with file handles?!
    TDataType dataType;
    TInt err = iAppList.AppForDocument( aAttaInfo.FileName(), appUid, dataType );
    TInt err = iAppList.AppForDocumentAndService(
    	const TDesC& aFileName,
    	RFs& aFSession,
    	RFile& aFile,
    	TUid aServiceUid,
    	TUid& aAppUid,
    	TDataType& aDataType);

    if ( dataType.Des8().FindF( KGameMimeType ) != KErrNotFound )
        {
        appUid = AppUidFromGameFileL( dataType, aAttaInfo.FileName() );
        }
    */
    if ( aAttaInfo.DataType().Des8().Compare( KEMailMessageMimeType ) == 0 )
        {
        appUid.iUid = KMceUID;
        err = KErrNone;
        }
    else if ( aAttaInfo.DataType().Des8().Compare( KTextDataType ) == 0 )
        {
        appUid.iUid = KNotepadUID3;
        err = KErrNone;
        }
    
    TBool supported( ( err == KErrNone ) && ( appUid.iUid != 0 ) );
    
    // Case Imelody: content type text/x-imelody. AppForDataType() returns 'no-app'.
    // But Imelody should be opened into text editor.
    // AppForDocument resolves correct application icon.
    // Content type length check removed from if()
    if ( !supported )
        {
        TDataType dataType;
        TMsvAttachmentId attachmentId = aAttaInfo.AttachmentId();
        // OPEN: is there need to handle attachmentId == 0 differently?
        RFile attachmentFile;
        TRAP( err, attachmentFile = iAttachmentModelObserver->GetAttachmentFileL( attachmentId ) );
        if ( err == KErrNone && attachmentFile.SubSessionHandle() != KNullHandle )
            {
            err = iAppList.AppForDocument( attachmentFile, appUid, dataType );
            attachmentFile.Close();
            }
        supported = ( ( err == KErrNone ) && ( appUid.iUid != 0 ) );
        }
    
    aAttaInfo.SetSupported( supported );

    CGulIcon* addIcon = NULL;
    CGulIcon* icon = NULL;
    if ( aAttaInfo.DRMDataType() )
        {
        MAknsSkinInstance* skins = AknsUtils::SkinInstance();
        switch ( aAttaInfo.DRMDataType() )
            {
            case CMsgAttachmentInfo::ECombinedDeliveryInvalidRights:
                {
                if ( !iDRMFWDLockIconNoRights )
                    {
                    iDRMFWDLockIconNoRights = AknsUtils::CreateGulIconL( 
                                        skins, 
                                        KAknsIIDQgnPropDrmExpForbidSuper, 
                                        KMsgEditorMbm, 
                                        EMbmMsgeditorQgn_prop_drm_exp_forbid_super, 
                                        EMbmMsgeditorQgn_prop_drm_exp_forbid_super_mask );  
                    AknIconUtils::SetSize( iDRMFWDLockIconNoRights->Bitmap(), iAddIconSize );
                    }                
                addIcon = iDRMFWDLockIconNoRights;
                }
                break;
                
            case CMsgAttachmentInfo::ESeparateDeliveryInvalidRights:
                {
                if ( !iDRMSDIconNoRights )
                    {
                    iDRMSDIconNoRights = AknsUtils::CreateGulIconL( 
                                        skins, 
                                        KAknsIIDQgnPropDrmRightsExpSuper, 
                                        KMsgEditorMbm, 
                                        EMbmMsgeditorQgn_prop_drm_rights_exp_super, 
                                        EMbmMsgeditorQgn_prop_drm_rights_exp_super_mask );  
                    AknIconUtils::SetSize( iDRMSDIconNoRights->Bitmap(), iAddIconSize );
                    }                
                addIcon = iDRMSDIconNoRights;
                }
                break;

            case CMsgAttachmentInfo::EForwardLockedOrCombinedDelivery:
                {
                // OPEN: What should the addIcon be if file is not supported?
                // Now it will be "iDRMFWDLockIcon"
                if ( !iDRMFWDLockIcon )
                    {
                    iDRMFWDLockIcon = AknsUtils::CreateGulIconL( 
                                        skins, 
                                        KAknsIIDQgnPropDrmSendForbidSuper, 
                                        KMsgEditorMbm, 
                                        EMbmMsgeditorQgn_prop_drm_send_forbid_super, 
                                        EMbmMsgeditorQgn_prop_drm_send_forbid_super_mask );  
                    AknIconUtils::SetSize( iDRMFWDLockIcon->Bitmap(), iAddIconSize );
                    }
                
                addIcon = iDRMFWDLockIcon;
                }
                break;

            case CMsgAttachmentInfo::ESeparateDeliveryValidRights:
                {
                if ( !iDRMSDIconRightsValid )
                    {
                    iDRMSDIconRightsValid = AknsUtils::CreateGulIconL( 
                                        skins, 
                                        KAknsIIDQgnPropDrmRightsValidSuper, 
                                        KMsgEditorMbm, 
                                        EMbmMsgeditorQgn_prop_drm_rights_valid_super, 
                                        EMbmMsgeditorQgn_prop_drm_rights_valid_super );  
                    AknIconUtils::SetSize( iDRMSDIconRightsValid->Bitmap(), iAddIconSize );
                    }
                
                addIcon = iDRMSDIconRightsValid;
                }
                break;

            default:
                break;
            }
        }
    if ( !icon )
        {
        icon = supported
            ? BitmapForAppL( appUid )
            : LoadDefaultBitmapL( EFalse );
        }

    CleanupStack::PushL( icon );    
    AknIconUtils::SetSize( icon->Bitmap(), iIconSize );

    CGulIcon* finalIcon = NULL;

    if ( addIcon )
        {
        // Create two new bitmaps (app icon and mask) that have document
        // icon drawn on top.
        CFbsBitmap* newMask = MsgAttachmentUtils::CombineIconsL(
            icon->Mask(),
            addIcon->Mask(),
            addIcon->Mask() );
        CleanupStack::PushL( newMask );
        CFbsBitmap* newBitmap = MsgAttachmentUtils::CombineIconsL(
            icon->Bitmap(),
            addIcon->Bitmap(),
            addIcon->Mask() );
        CleanupStack::PushL( newBitmap );

        finalIcon = CGulIcon::NewL( newBitmap, newMask );
        CleanupStack::Pop( 2 ); //newBitmap, mask
        CleanupStack::PopAndDestroy(); //icon
        CleanupStack::PushL( finalIcon );
        }
    else
        {
        finalIcon = icon;
        }

    // fade icon if not fetched.
    if ( !aAttaInfo.IsFetched() )
        {
        // Create copy of the original application icon bitmap & mask
        // so that original data is not modified.
        CFbsBitmap* newMask = CopyBitmapL( finalIcon->Mask() );
        CleanupStack::PushL( newMask );
        CFbsBitmap* newBitmap = CopyBitmapL( finalIcon->Bitmap() );
        CleanupStack::PushL( newBitmap );

        CGulIcon* tempIcon = CGulIcon::NewL( newBitmap, newMask );
        
        CleanupStack::Pop( 2, newMask );
        CleanupStack::PopAndDestroy( addIcon ? finalIcon : icon );
        finalIcon = tempIcon;
        CleanupStack::PushL( finalIcon );
        
        FadeBitmapL( finalIcon->Bitmap() );
        }
    CleanupStack::Pop( finalIcon );

    MEBLOGGER_LEAVEFN( "BitmapForFileL" );

    return finalIcon;
    }


// ---------------------------------------------------------
// CMsgAttachmentModel::FadeBitmapL
// ---------------------------------------------------------
//
void CMsgAttachmentModel::FadeBitmapL( CFbsBitmap* aBitmap )
    {
    TRegionFix<1> area( aBitmap->SizeInPixels() );

    CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL( aBitmap );

    CFbsBitGc* bitGc = NULL;
    bmpDevice->CreateContext( bitGc );

    if ( bitGc )
        {
        // TODO: Magic numbers here to stay?
        TUint8 blackMap = 128;
        TUint8 whiteMap = 255;
        bitGc->SetFadingParameters( blackMap, whiteMap );
        bitGc->FadeArea( &area );

        delete bitGc;
        }
    delete bmpDevice;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::CopyBitmapL
// ---------------------------------------------------------
//
CFbsBitmap* CMsgAttachmentModel::CopyBitmapL( const CFbsBitmap* aBitmap )
    {
    if ( !aBitmap )
        {
        User::Leave( KErrArgument );
        }
    
    CFbsBitmap* newBitmap = new( ELeave ) CFbsBitmap();
    CleanupStack::PushL( newBitmap );
    
    User::LeaveIfError( newBitmap->Create( aBitmap->SizeInPixels(), aBitmap->DisplayMode() ) );
    
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL( newBitmap );
    
    CleanupStack::Pop( newBitmap );
    CleanupStack::PushL( bitmapDevice );
    
    CBitmapContext* bitmapContext = NULL;
    User::LeaveIfError( bitmapDevice->CreateBitmapContext( bitmapContext ) );
    CleanupStack::PushL( bitmapContext );
    
    bitmapContext->BitBlt( TPoint(), aBitmap );

    CleanupStack::PopAndDestroy( 2, bitmapDevice );    
    return newBitmap;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::AddAttachmentL
//
// Adds attachment to the internal array. Notifies attachment model
// observer with EMsgAttachmentAdded parameter.
// ---------------------------------------------------------
//
EXPORT_C CMsgAttachmentInfo* CMsgAttachmentModel::AddAttachmentL(
    const TDesC& aFullName,
    TInt aSize,
    TMsvAttachmentId aAttachmentEntryId,
    TBool aFetched,
    TInt aIndex /*= -1*/ )
    {
    TDataType dataType;

    if ( ConeUtils::FileExists( aFullName ) )
        {
        TUid appUid( KNullUid );
        iAppList.AppForDocument( aFullName, appUid, dataType );
        }

    return AddAttachmentL(
        aFullName, aSize, aAttachmentEntryId, aFetched, dataType, aIndex );
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::AddAttachmentL
//
// Adds attachment to the internal array. Notifies attachment model
// observer with EMsgAttachmentAdded parameter.
// ---------------------------------------------------------
//
EXPORT_C CMsgAttachmentInfo* CMsgAttachmentModel::AddAttachmentL(
    const TDesC& aFullName,
    TInt aSize,
    TMsvAttachmentId aAttachmentEntryId,
    TBool aFetched,
    const TDataType& aDataType,
    TInt aIndex )
    {
    return AddAttachmentL( aFullName, 
                           aSize,
                           aAttachmentEntryId,
                           aFetched,
                           aDataType,
                           CMsgAttachmentInfo::ENoLimitations,
                           aIndex);
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::AddAttachmentL
//
// Adds attachment to the internal array. Notifies attachment model
// observer with EMsgAttachmentAdded parameter.
// ---------------------------------------------------------
//
EXPORT_C CMsgAttachmentInfo* CMsgAttachmentModel::AddAttachmentL(
    const TDesC& aFullName,
    TInt aSize,
    TMsvAttachmentId aAttachmentEntryId,
    TBool aFetched,
    const TDataType& aDataType,
    CMsgAttachmentInfo::TDRMDataType aDRMDataType,
    TInt aIndex /*= -1*/ )
    {
    CMsgAttachmentInfo* info = CreateNewInfoL( aFullName, 
                                               aSize, 
                                               aFetched, 
                                               ETrue, 
                                               aAttachmentEntryId, 
                                               aDataType, 
                                               aDRMDataType );

    CleanupStack::PushL( info );

    info->SetAttachmentId( aAttachmentEntryId );

    if ( aIndex == -1 )
        {
        iAttachInfoArray->AppendL( info );
        }
    else
        {
        iAttachInfoArray->InsertL( aIndex, info );
        }

    iModified = ETrue;

    if ( ( iAttachmentModelObserver != NULL ) && ( NumberOfItems() > 0 ) )
        {
        iAttachmentModelObserver->NotifyChanges(
            MMsgAttachmentModelObserver::EMsgAttachmentAdded, info );
        }

    CleanupStack::Pop(); // info

    return info;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::DeleteAttachment
//
// Deletes attachment from internal array. Notifies attachment model
// observer with EMsgAttachmentRemoved parameter.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentModel::DeleteAttachment( TInt aIndex )
    {
    TRAP_IGNORE( DoDeleteAttachmentL( aIndex ) );
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::DoDeleteAttachmentL
//
// Deletes attachment from internal array. Notifies attachment model
// observer with EMsgAttachmentRemoved parameter.
// ---------------------------------------------------------
//
void CMsgAttachmentModel::DoDeleteAttachmentL( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iAttachInfoArray->Count() )
        {
        CEikonEnv* eikonEnv = CEikonEnv::Static();
        
        CFileMan* fileMan = CFileMan::NewL( eikonEnv->FsSession() );
        CleanupStack::PushL( fileMan );
                
        CMsgAttachmentInfo* info = iAttachInfoArray->At( aIndex );

        if ( iAttachmentModelObserver )
            {
            iAttachmentModelObserver->NotifyChanges(
                MMsgAttachmentModelObserver::EMsgAttachmentRemoved, info );
            }

        // delete the information block.
        delete info;

        // delete the array entry.
        iAttachInfoArray->Delete( aIndex );
        // delete files from our temp directory.

        TFileName temppath;
        MsgAttachmentUtils::GetMsgEditorTempPath( temppath );
        fileMan->Delete( temppath );
        
        CleanupStack::PopAndDestroy( fileMan );
            
        iModified = ETrue;
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::IsModified
//
// Returns true if stored info about attachments is modified.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgAttachmentModel::IsModified() const
    {
    return iModified;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::SetModified
//
// Sets the modified flag.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentModel::SetModified( TBool aModified )
    {
    iModified = aModified;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::IsReadOnly
//
// Returns ETrue if attachment model was created to be read-only.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgAttachmentModel::IsReadOnly()
    {
    return iReadOnly;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::IsFileAttached
//
// Returns ETrue if given file is found from the internal array.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgAttachmentModel::IsFileAttached( const TDesC& aFileName ) const
    {
    TInt cc = iAttachInfoArray->Count() - 1;

    // TODO: Make this understandable!
    for ( ; ( cc >= 0 ) && ( iAttachInfoArray->At( cc )->FileName().CompareF( aFileName ) != 0 ); --cc )
        ;

    return cc >= 0;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::IsIdAttached
//
// Returns ETrue if given attachment id is found from the internal array.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgAttachmentModel::IsIdAttached( TMsvAttachmentId aId ) const
    {
    TInt cc = iAttachInfoArray->Count() - 1;

    // TODO: Make this understandable!
    for (; ( cc >= 0 ) && ( iAttachInfoArray->At( cc )->AttachmentId() != aId ); --cc )
        ;

    return cc >= 0;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::ViewAttachmentL
//
// Opens seleceted attachment in corresponding target application.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentModel::ViewAttachmentL( TInt aIndex )
    {
    CMsgAttachmentInfo& attInfo = AttachmentInfoAt( aIndex );
    TFileName filename = attInfo.FileName();
    TThreadId id;
    User::LeaveIfError( iAppList.StartDocument( filename, id ) );
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::Reset
//
// Reset the attachment model (empties the internal array).
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentModel::Reset()
    {
    if ( iAttachInfoArray )
        {
        iAttachInfoArray->ResetAndDestroy();
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::CMsgAttachmentModel
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgAttachmentModel::CMsgAttachmentModel( TBool aReadOnly )
    : iReadOnly( aReadOnly )
    {
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::ConstructL
//
// 2nd phase constructor. Creates attachment info array.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentModel::ConstructL()
    {
    iAttachInfoArray = new ( ELeave ) CAttInfoArray( KAttachArrayGranularity );
    User::LeaveIfError( iAppList.Connect() );
    LoadResourcesL();
    }

EXPORT_C void CMsgAttachmentModel::LoadResourcesL()
    {
    // Resolve icon layout
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutRect listPaneLayout;
    listPaneLayout.LayoutRect(
        mainPane,
        AknLayout::list_gen_pane( 0 ) );
    TAknLayoutRect doubleGraphicLayout;
    doubleGraphicLayout.LayoutRect(
        listPaneLayout.Rect(),
        AknLayout::list_double_large_graphic_pane_list_double2_large_graphic_pane_list_single_big_large_graphic_pane( 0 ) );
    TAknLayoutRect iconLayout;
    iconLayout.LayoutRect(             
        doubleGraphicLayout.Rect(),
        AknLayout::List_pane_elements__double_large_graphic__Line_8() );
    TAknLayoutRect addIconLayout;
    addIconLayout.LayoutRect(             
        doubleGraphicLayout.Rect(),
        AknLayout::List_pane_elements__double_large_graphic__Line_9( 0 ) );   

    iIconSize = iconLayout.Rect().Size();
    iAddIconSize = addIconLayout.Rect().Size();

    MAknsSkinInstance* skins = AknsUtils::SkinInstance();

    delete iDRMFWDLockIcon;
    iDRMFWDLockIcon = NULL;
    delete iDRMSDIconRightsValid;
    iDRMSDIconRightsValid = NULL;
    delete iDRMFWDLockIconNoRights;
    iDRMFWDLockIconNoRights = NULL;
    delete iDRMSDIconNoRights;
    iDRMSDIconNoRights = NULL;

    TInt count = iAttachInfoArray
        ? iAttachInfoArray->Count()
        : 0;
    while ( count-- )
        {
        // Update possibly already existing icons
        iAttachInfoArray->At( count )->ChangeSizeAndIconL();
        }
    }


// ---------------------------------------------------------
// CMsgAttachmentModel::GetAttachmentFileL
// ---------------------------------------------------------
//
EXPORT_C RFile CMsgAttachmentModel::GetAttachmentFileL( TInt aIndex )
    {
    CMsgAttachmentInfo* attInfo = iAttachInfoArray->At( aIndex ); 

    if ( !attInfo )
        {
        User::Leave( KErrNotFound );
        }

    TMsvAttachmentId attachmentId = attInfo->AttachmentId();
    RFile file;
    if ( attachmentId == KMsvNullIndexEntryId )
        {   
        RFs& fs = CEikonEnv::Static()->FsSession();
        TInt err = file.Open( fs, attInfo->FileName(), EFileRead | EFileShareAny );
        if ( err )
            {
            User::LeaveIfError( file.Open( fs, attInfo->FileName(), EFileRead | EFileShareReadersOnly ) );
            }
        }
    else
        {
        file = iAttachmentModelObserver->GetAttachmentFileL( attachmentId );
        }
    return file;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::LoadDefaultBitmapL
//
// Loads system default bitmap.
// ---------------------------------------------------------
//
CGulIcon* CMsgAttachmentModel::LoadDefaultBitmapL( TBool /*aSupported*/ )
    {
    MEBLOGGER_ENTERFN( "LoadDefaultBitmapL" );
    if( AknLayoutUtils::LayoutMirrored() )
        {
        return AknsUtils::CreateGulIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDQgnPropUnknown,
            KMsgEditorMbm_AH,
            EMbmMsgeditor_ahQgn_prop_unknown,
            EMbmMsgeditor_ahQgn_prop_unknown_mask );
        }
    else
        {
        return AknsUtils::CreateGulIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDQgnPropUnknown,
            KMsgEditorMbm,
            EMbmMsgeditorQgn_prop_unknown,
            EMbmMsgeditorQgn_prop_unknown_mask );        
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::AppUidForFileL
//
// Returns app ui of application that handles given file.
// ---------------------------------------------------------
//
TUid CMsgAttachmentModel::AppUidForFileL( const TFileName& aFileName )
    {
    TUid appUid( KNullUid );
    TDataType dataType;

    User::LeaveIfError( iAppList.AppForDocument( aFileName, appUid, dataType ) );

    if ( appUid.iUid == 0 )
        {
        User::LeaveIfError( iAppList.AppForDataType( dataType, appUid ) );
        }

    return appUid;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::AppUidForDataTypeL
//
// Returns app ui of application that handles given data type.
// ---------------------------------------------------------
//
TUid CMsgAttachmentModel::AppUidForDataTypeL( const TDataType& aDataType )
    {
    TUid appUid( KNullUid );

    User::LeaveIfError( iAppList.AppForDataType( aDataType, appUid ) );

    return appUid;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::AppUidFromGameFileL
//
// Returns app uid of application that handles given game data.
// ---------------------------------------------------------
TUid CMsgAttachmentModel::AppUidFromGameFileL(
    TDataType& aDataType, const TDesC& aFileName )
    {
    TUid appUid( KNullUid );
    RFile file;
    RFs& fs = CEikonEnv::Static()->FsSession();

    User::LeaveIfError( file.Open( fs, aFileName, EFileRead ) );
    CleanupClosePushL(file);
    RFileReadStream reader( file );
    CleanupClosePushL(reader);
    TInt32 gameId = reader.ReadInt32L();
    
    CleanupStack::PopAndDestroy(2,&file); //reader , file 

    if ( gameId > KMaxGameEngineDataID )
        {
        TBuf8<256> completedType = aDataType.Des8();
        completedType.Append(_L("-"));
        completedType.AppendNumFixedWidth( gameId, EHex, 8 );
        aDataType = TDataType( completedType );
        }

    iAppList.AppForDataType( aDataType, appUid );

    if ( appUid.iUid == 0 )
        {
        appUid.iUid = KGameEngineUID;
        }

    return appUid;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::BitmapForAppL
//
// Returns pointer to bitmap of given application.
// ---------------------------------------------------------
//
CGulIcon* CMsgAttachmentModel::BitmapForAppL( const TUid &aAppUid )
    {
    MEBLOGGER_ENTERFN("BitmapForAppL");
    
    CFbsBitmap* tempBitmap = NULL;
    CFbsBitmap* tempMask = NULL;

    AknsUtils::CreateAppIconLC(
        AknsUtils::SkinInstance(),
        aAppUid,
        EAknsAppIconTypeList,
        tempBitmap,
        tempMask );

    CGulIcon* bitmap = CGulIcon::NewL( tempBitmap, tempMask );
    CleanupStack::Pop( 2 ); // tempBitmap, tempMask

    MEBLOGGER_LEAVEFN( "BitmapForAppL" );

    return bitmap;
    }

// ---------------------------------------------------------
// CMsgAttachmentModel::CreateNewInfoL
//
// Creates new attachment info object.
// ---------------------------------------------------------
//
CMsgAttachmentInfo* CMsgAttachmentModel::CreateNewInfoL(
    const TDesC& aFileName,
    TInt aSize,
    TBool aFetched,
    TBool aAttachedThisSession,
    TMsvAttachmentId aAttachmentId,
    const TDataType& aDataType,
    CMsgAttachmentInfo::TDRMDataType aDRMDataType )
    {
    return CMsgAttachmentInfo::NewL(
        *this,
        aFileName,
        aSize,
        aFetched,
        aAttachedThisSession,
        aAttachmentId,
        aDataType,
        aDRMDataType );
    }

// ========== OTHER EXPORTED FUNCTIONS =====================

//  End of File
