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
* Description:  MsgAttachmentUtils implementation
*
*/



// ========== INCLUDE FILES ================================

#include <featmgr.h>
#include <data_caging_path_literals.hrh>

#include <eikenv.h>                 // for CEikonEnv
#include <eikproc.h>                // for CEikProcess
#include <gulicon.h>
#include <apparc.h>                 // for CApaApplication
#include <apgcli.h>                 // for RApaLsSession
#include <apacln.h>                 // for TApaDocCleanupItem
#include <txtetext.h>               // for CPlainText
#include <gdi.h>
#include <bautils.h>                // NearestLanguageFile

#include <avkon.rsg>                // for R_QTN_SIZE_xx
#include <aknlayout.cdl.h> // LAF
#include <AknUtils.h>

#include <AknCommonDialogsDynMem.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include <NpdApi.h>                 // Notepad API
#include <StringLoader.h>           // StringLoader

#include <MGFetch.h>               // for MediaGallery

#include <NewFileServiceClient.h>
#include <NewFileService.hrh>
#include <AiwCommon.h>

#include "MsgEditorCommon.h"        // for KMsgEditorTempDir
#include "MsgAttachmentUtils.h"
#include "MsgEditorModelPanic.h"    // Panics
#include <MsgEditorAppUi.rsg>       // resouce identifiers

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

const TInt KMsgAttaFileNameLength = 20;
_LIT( KTxtAttaFilenameExtension, ".txt" );
_LIT( KMsgEditorAppUiResourceFileName, "msgeditorappui.rsc" );

_LIT( KMsgEditorTempDir, ":\\system\\temp\\msgeditor_temp\\" );

const TInt KLocalKilo = 1024;

const TUid KUidCamcorder = { 0x101F857A };
const TUid KUidVoiceRec  = { 0x100058CA };
//const TUid KUidCamera    = { 0x1000593F };

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// MsgAttachmentUtils::FetchFile
// ---------------------------------------------------------
//
EXPORT_C TBool MsgAttachmentUtils::FetchFileL (
    TMsgAttachmentFetchType aType,
    TFileName& aFileName,
    TBool aCheckDiskSpace )
    {
    return FetchFileL( aType, aFileName, KNullUid, aCheckDiskSpace );
    }

// ---------------------------------------------------------
// MsgAttachmentUtils::FetchFile
// ---------------------------------------------------------
//
EXPORT_C TBool MsgAttachmentUtils::FetchFileL (
    TMsgAttachmentFetchType aType,
    TFileName& aFileName,
    const TUid& /*aAppUid*/,
    TBool aCheckDiskSpace )
    {
    TBool result = EFalse;
    CDesCArrayFlat* files = new ( ELeave ) CDesC16ArrayFlat( 1 );
    CleanupStack::PushL( files );
    result = FetchFileL( aType, aFileName, *files, aCheckDiskSpace, EFalse, NULL );
    CleanupStack::PopAndDestroy( ); //files
    return result;
    }

// ---------------------------------------------------------
// MsgAttachmentUtils::FetchFile
// ---------------------------------------------------------
//
EXPORT_C TBool MsgAttachmentUtils::FetchFileL (
    TMsgAttachmentFetchType aType,
    TFileName& aFileName,
    CDesCArray& aSelectedFiles,    
    TBool aCheckDiskSpace,
    TBool aMultiSelect,
    MMGFetchVerifier* aVerifier )
    {
    TBool result = FetchFileL( aType, aSelectedFiles, NULL, aCheckDiskSpace, aMultiSelect, aVerifier );
    if ( aSelectedFiles.MdcaCount() )
        {
        aFileName.Zero();
        aFileName = aSelectedFiles.MdcaPoint( 0 );
        }
    return result;
    }
    
// ---------------------------------------------------------
// MsgAttachmentUtils::FetchFile
// ---------------------------------------------------------
//
EXPORT_C TBool MsgAttachmentUtils::FetchFileL (
    TMsgAttachmentFetchType aType,
    CDesCArray& aSelectedFiles,
    CAiwGenericParamList* aParams,
    TBool /*aCheckDiskSpace*/,
    TBool aMultiSelect,
    MMGFetchVerifier* aVerifier )
    {
    TBool result = EFalse;

    switch ( aType )
        {
        case EImage:
            {
            result = FetchExistingL( EImageFile, aSelectedFiles, aMultiSelect, aVerifier );
            break;
            }
        case EAudio:
            {
            result = FetchExistingL( EAudioFile, aSelectedFiles, aMultiSelect, aVerifier );
            break;
            }
        case EVideo:
            {
            result = FetchExistingL( EVideoFile, aSelectedFiles, aMultiSelect, aVerifier );
            break;
            }
        case ENewAudio:
            {
            result = FetchNewL( ENewFileServiceAudio, aSelectedFiles, aParams, aMultiSelect );
            break;
            }
        case ENewImage:
            {
            result = FetchNewL( ENewFileServiceImage, aSelectedFiles, aParams, aMultiSelect );
            break;
            }
        case ENewVideo:
            {
            result = FetchNewL( ENewFileServiceVideo, aSelectedFiles, aParams, aMultiSelect );
            break;
            }
        case ENote:
            {
            result = FetchNoteL( aSelectedFiles );
            break;
            }
        case ESVG:
            {
            result = FetchExistingL( EPresentationsFile, aSelectedFiles, aMultiSelect, aVerifier );
            break;
            }
        default:
            {
            break;
            }
        }

    return result;
    }

// ---------------------------------------------------------
// MsgAttachmentUtils::FetchExistingL
//
//
// ---------------------------------------------------------

TBool MsgAttachmentUtils::FetchExistingL( TMediaFileType aFileType,
                                         CDesCArray& aSelectedFiles,
                                         TBool aMultiSelect,
                                         MMGFetchVerifier* aVerifier )

    {
    return MGFetch::RunL( aSelectedFiles, aFileType, aMultiSelect, aVerifier );
    }

// ---------------------------------------------------------
// MsgAttachmentUtils::FetchNoteL
//
//
// ---------------------------------------------------------
TBool MsgAttachmentUtils::FetchNoteL( CDesCArray& aSelectedFiles )
    {
    TBool result = EFalse;
    CEikonEnv* eikonEnv = CEikonEnv::Static();

    // get memo content from notepad to buffer.
    HBufC* title = eikonEnv->AllocReadResourceLC( R_QTN_NMAKE_FETCH_MEMO_PRMPT );
    HBufC* buf = CNotepadApi::FetchMemoL( title );
    CleanupStack::PopAndDestroy( title );

    if ( buf )
        {
        CleanupStack::PushL( buf );

        RFs& fs = eikonEnv->FsSession();

        TFileName filename;
        TInt max = KMsgAttaFileNameLength;

        // first try to create filename from memo text.
        MsgAttachmentUtils::GetFileNameFromBuffer (
            filename, *buf, max, &KTxtAttaFilenameExtension );

        if ( filename.Length() == 0 )
            {
            eikonEnv->ReadResourceL( filename, R_QTN_MMS_NO_NAME_FOR_FILE );
            filename.Append( KTxtAttaFilenameExtension );
            }

        TFileName pathname;
        GetMsgEditorTempPath( pathname, &filename );

        // check the file name for validity and possible name duplicates.
        TInt err = CApaApplication::GenerateFileName( fs, pathname );

        if ( err == KErrNone )
            {
            // write buffer to text file (unicode).
            CPlainText* text = CPlainText::NewL();
            CleanupStack::PushL( text );
            text->InsertL( 0, *buf );
            text->ExportAsTextL( pathname, CPlainText::EOrganiseByParagraph, 0 );
            CleanupStack::PopAndDestroy( text );
            aSelectedFiles.AppendL( pathname );
            result = ETrue;
            }

        CleanupStack::PopAndDestroy( buf );
        }
    return result;
    }

// ---------------------------------------------------------
// MsgAttachmentUtils::FetchNewL
//
//
// ---------------------------------------------------------
TBool MsgAttachmentUtils::FetchNewL( TNewServiceFileType aType,
                                    CDesCArray& aSelectedFiles,
                                    CAiwGenericParamList* aParams,
                                    TBool aMultiSelect )
    {
    TBool result = EFalse;    
    CNewFileServiceClient* fileClient = NewFileServiceFactory::NewClientL();
    CleanupStack::PushL( fileClient );

    TUid appUid = KUidCamcorder; 
    switch ( aType )
        {
        case ENewFileServiceImage:
        case ENewFileServiceVideo:
            {
            appUid = KUidCamcorder;
            break;
            }
        case ENewFileServiceAudio:
            {
            appUid = KUidVoiceRec;
            break;
            }
        default:
            User::Leave( KErrArgument );
            break;
        }
    
    result = fileClient->NewFileL(
        appUid, 
        aSelectedFiles,
        aParams,
        aType,
        aMultiSelect );
    CleanupStack::PopAndDestroy( fileClient ); // params
    return result;
    }

// ---------------------------------------------------------
// MsgAttachmentUtils::FetchAnyFileL
//
//
// ---------------------------------------------------------
EXPORT_C TBool MsgAttachmentUtils::FetchAnyFileL( TFileName& aFileName,
                                                  CCoeEnv& aCoeEnv,
                                                  MAknFileSelectionObserver* aVerifier,
                                                  MAknFileFilter* aFilter )
    {
    TBool result( EFalse );
    TInt resourceFileOffset( -1 );
    
    if ( !aCoeEnv.IsResourceAvailableL( R_MEB_MEMORY_SELECTION_DIALOG ) )
        {
        TParse parse;
        parse.Set( KMsgEditorAppUiResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
        
        TFileName fileName( parse.FullName() );
        BaflUtils::NearestLanguageFile( aCoeEnv.FsSession(), fileName );
        
        resourceFileOffset = aCoeEnv.AddResourceFileL( fileName );
        }

    TInt supportedTypes = AknCommonDialogsDynMem::EMemoryTypePhone | 
                          AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage | 
                          AknCommonDialogsDynMem::EMemoryTypeMMCExternal;
                           
    result = AknCommonDialogsDynMem::RunSelectDlgLD( supportedTypes,
                                                     aFileName,
                                                     R_MEB_MEMORY_SELECTION_DIALOG,
                                                     aFilter,
                                                     aVerifier );
    
    if ( resourceFileOffset != -1 )
        {
        aCoeEnv.DeleteResourceFile( resourceFileOffset );
        }
        
    return result;
    }

// ---------------------------------------------------------
// MsgAttachmentUtils::CombineIconsL
//
//
// ---------------------------------------------------------
//
EXPORT_C CFbsBitmap* MsgAttachmentUtils::CombineIconsL(
    const CFbsBitmap* aFirstBmp,
    const CFbsBitmap* aSecondBmp,
    const CFbsBitmap* aSecondBmpMask )
    {
    __ASSERT_DEBUG( aFirstBmp, Panic( ENullPointer ) );

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

    TInt cleanupCount = 0;

    TDisplayMode dispMode = aFirstBmp->DisplayMode();
    if ( dispMode == EGray2 )
        {
        // Convert hard masks to soft masks:
        CFbsBitmap* temp = InvertBitmapL( aFirstBmp );
        CleanupStack::PushL( temp );
        aFirstBmp = temp;
        cleanupCount++;
        dispMode = EGray256;
        }

    // Create target bitmap, device and gc
    CFbsBitmap* bmp = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bmp );
    User::LeaveIfError( bmp->Create( iconLayout.Rect().Size(), dispMode ) );
    CFbsBitmapDevice* pBmpDev = CFbsBitmapDevice::NewL( bmp );
    CleanupStack::Pop(); // bmp
    CleanupStack::PushL( pBmpDev );
    CBitmapContext* pBmpCtx = NULL;
    User::LeaveIfError( pBmpDev->CreateBitmapContext( pBmpCtx ) );
    CleanupStack::PushL( pBmpCtx );
    
    // Clear the target bitmap.
    // Black background for soft masks
    pBmpCtx->SetBrushColor( ( dispMode == EGray256 )
        ? KRgbBlack
        : KRgbWhite );
    pBmpCtx->Clear();

    // First draw the source bitmap...
    const TSize bmpSize1( aFirstBmp->SizeInPixels() );
    const TPoint origin1(
        ( iconLayout.Rect().Width() - bmpSize1.iWidth ) / 2,
        ( iconLayout.Rect().Height() - bmpSize1.iHeight ) / 2 );

    pBmpCtx->BitBlt( origin1, aFirstBmp );

    if ( aSecondBmp )
        {
        if ( aSecondBmp->DisplayMode() == EGray2 )
            {
            // Convert hard masks to soft masks:
            CFbsBitmap* temp = InvertBitmapL( aSecondBmp );
            CleanupStack::PushL( temp );
            aSecondBmp = temp;
            cleanupCount++;
            }

        // ...then draw the other bitmap on top.
        const TSize bmpSize2( aSecondBmp->SizeInPixels() );
        const TPoint origin2( 0, iconLayout.Rect().Height() - bmpSize2.iHeight );

        if ( !aSecondBmpMask )
            {
            pBmpCtx->BitBlt( origin2, aSecondBmp );
            }
        else
            {
            pBmpCtx->BitBltMasked(
                origin2,
                aSecondBmp,
                TRect( bmpSize2 ),
                aSecondBmpMask,
                ETrue );
            }
        }

    // Cleanup.
    CleanupStack::PopAndDestroy( 2 ); // pBmpDev, pBmpCtx
    CleanupStack::PopAndDestroy( cleanupCount );

    // Return the new bitmap.
    return bmp;
    }

// ---------------------------------------------------------
// MsgAttachmentUtils::InvertBitmapL
//
//
// ---------------------------------------------------------
//
CFbsBitmap* MsgAttachmentUtils::InvertBitmapL( const CFbsBitmap* aBitmap )
    {
    // Create target bitmap, device and gc
    CFbsBitmap* bmp = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bmp );
    User::LeaveIfError( bmp->Create( aBitmap->SizeInPixels(), aBitmap->DisplayMode() ) );
    CFbsBitmapDevice* pBmpDev = CFbsBitmapDevice::NewL( bmp );
    CleanupStack::Pop(); // bmp
    CleanupStack::PushL( pBmpDev );
    CBitmapContext* pBmpCtx = NULL;
    User::LeaveIfError( pBmpDev->CreateBitmapContext( pBmpCtx ) );
    CleanupStack::Pop(); // pBmpDev

    // Set drawing mode to "NOTPEN" -> bitmap will be inverted
    pBmpCtx->SetDrawMode( CGraphicsContext::EDrawModeNOTPEN );
    pBmpCtx->BitBlt( TPoint( 0, 0 ), aBitmap );

    // Cleanup.
    delete pBmpCtx;
    delete pBmpDev;

    // Return the new bitmap.
    return bmp;
    }


// ---------------------------------------------------------
// MsgAttachmentUtils::FileSizeToStringL
//
//
// ---------------------------------------------------------
//
EXPORT_C void MsgAttachmentUtils::FileSizeToStringL(
    TDes& aTarget, TInt aFileSize, TBool aKilobytesMinimum /*= EFalse*/ )
    {
    TInt fileSize = aFileSize;
    TInt resId = R_QTN_SIZE_B;

    if ( aKilobytesMinimum || aFileSize >= KLocalKilo )
        {
        resId = R_QTN_SIZE_KB;
        fileSize /= KLocalKilo;
        if ( aFileSize % KLocalKilo )
            {
            fileSize++;
            }
        }

    HBufC *buf = StringLoader::LoadLC( resId, fileSize );
    aTarget.Zero();
    aTarget.Append( *buf );

    CleanupStack::PopAndDestroy( buf );
    }

//----------------------------------------------------------------------------
// from: TBool CImRecvConvert::IsIllegalChar(const TUint aChar)
//----------------------------------------------------------------------------
LOCAL_C TBool IsIllegalChar(const TUint aChar)
    {
    return (
        aChar == '*'  ||
        aChar == '\\' ||
        aChar == '<'  ||
        aChar == '>'  ||
        aChar == ':'  ||
        aChar == '.'  ||
        aChar == '"'  ||
        aChar == '/'  ||
        aChar == '|'  ||
        aChar == '?'  ||
        aChar == CEditableText::EParagraphDelimiter  ||
        aChar == CEditableText::ELineBreak  ||
        aChar <  ' ' );
    }

// ---------------------------------------------------------
// MsgAttachmentUtils::GetFileNameFromBuffer
//
//
// ---------------------------------------------------------
//
EXPORT_C void MsgAttachmentUtils::GetFileNameFromBuffer(
    TFileName& aFileName,
    const TDesC& aBuffer,
    TInt aMaxLength,
    const TDesC* aExt /*= NULL*/ )
    {
    if ( aExt != NULL )
        {
        aMaxLength -= aExt->Length();
        }

    TInt len = aBuffer.Length();
    TInt max = Min( len, aMaxLength );

    __ASSERT_DEBUG( max > 0, Panic( EMsgZeroLength ) );

    aFileName.Zero();

    TInt cc = 0;
    TUint ch;
    TUint ch1 = 0;
    TBool spaces = EFalse;
    for ( TInt i = 0; i < len && cc < max; i++ )
        {
        ch = aBuffer[i];

        // ignore spaces from beginning of the buffer until first
        // non-space is encountered.
        if ( !spaces && ch != ' ' )
            {
            spaces = ETrue;
            }

        if ( i > 0 )
            {
            ch1 = aBuffer[i - 1];
            }

        // strip illegal chars away.
        // checks also if previous and current chars are '.'
        if ( spaces && ! IsIllegalChar( ch ) )
            {
            if ( !( i > 0 && ch == '.' && ch1 == '.' ) )
                {
                aFileName.Append( ch );
                cc++;
                }
            }
        }

    aFileName.Trim();

    // If filename is empty at this point, do not append extension either.
    // Instead, empty filename is returned so that caller can use whatever
    // default s/he has for it.
    if ( aFileName.Length() > 0 && aExt != NULL )
        {
        aFileName.Append( *aExt );
        }
    }

// ---------------------------------------------------------
// MsgAttachmentUtils::GetMsgEditorTempPath
//
// Returns temp path used by msg editors and optionally
// appends file name to it.
// ---------------------------------------------------------
//
EXPORT_C void MsgAttachmentUtils::GetMsgEditorTempPath(
    TFileName& aTempPath,
    const TDesC* aFileName /*= NULL*/ )
    {
    TInt driveNumber( EDriveD );
    
#ifdef RD_MULTIPLE_DRIVE
    if ( DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRam, driveNumber ) != KErrNone )
        {
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, driveNumber );
        }
#endif

    TChar driveCharacter;
    CEikonEnv::Static()->FsSession().DriveToChar( driveNumber, driveCharacter );

    aTempPath.Append( driveCharacter );
    aTempPath.Append( KMsgEditorTempDir );
    
    if ( aFileName )
        {
        aTempPath.Append( *aFileName );
        }
    }

// End of File
