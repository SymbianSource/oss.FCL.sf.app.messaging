/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgAttachmentUtils  declaration
*
*/



#ifndef MSGATTACHMENTUTILS_H
#define MSGATTACHMENTUTILS_H

// ========== INCLUDE FILES ================================

#include <e32std.h>
#include <apparc.h>
#include <badesca.h>                // for CDesCArray
#include <MediaFileTypes.hrh>       // for MediaGallery
#include <NewFileService.hrh>
#include <MMGFetchVerifier.h>
#include <MAknFileSelectionObserver.h>
#include <MAknFileFilter.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CFbsBitmap;
class CAiwGenericParamList;
class CCoeEnv;

// ========== CLASS DECLARATION ============================

/**
 * Attachment utilities.
 *
 */
class MsgAttachmentUtils
    {
    public:
        enum TMsgAttachmentFetchType
            {
            EUnknown = 0,
            EImage,
            EAudio,
            EVideo,
            ENote,
            ENewImage,
            ENewAudio,
            ENewVideo,
            ESVG
            };

        /**
         * Fetches an attachment of given type from external application.
         * @param aType           Fetch type.
         * @param aFileName       OUT: filename of the fetched file.
         * @param aCheckDiskSpace Deprecated! No disk space check done ever.
         * @return EFalse if fetch canceled.
         */
        IMPORT_C static TBool FetchFileL(
            TMsgAttachmentFetchType aType,
            TFileName&              aFileName,
            TBool                   aCheckDiskSpace = EFalse );

        /**
         * Fetches an attachment of given type from external application.
         * @param aType           Fetch type.
         * @param aFileName       OUT: filename of the fetched file.
         * @param aAppUid         app uid used for filtering sound files in audio fetch.
         * @param aCheckDiskSpace Deprecated! No disk space check done ever.
         * @return EFalse if fetch canceled.
         */
        IMPORT_C static TBool FetchFileL(
            TMsgAttachmentFetchType aType,
            TFileName&              aFileName,
            const TUid&             aAppUid,
            TBool                   aCheckDiskSpace = EFalse );

        /**
         * Fetches an attachment of given type from external application.
         * @param aType           Fetch type.
         * @param aFileName       OUT: filename of the fetched file.
         * @param aSelectedFiles  OUT: array of selected files.
         * @param aCheckDiskSpace Deprecated! No disk space check done ever.
         * @param aMultiSelect    if ETrue Media Gallery allows multiselect in file list.
         * @param aVerifier       To verify selection.       
         * @return EFalse if fetch canceled.
         */

        IMPORT_C static TBool FetchFileL(
            TMsgAttachmentFetchType aType,
            TFileName&              aFileName,
            CDesCArray&             aSelectedFiles,
            TBool                   aCheckDiskSpace = EFalse,
            TBool                   aMultiSelect = EFalse,
            MMGFetchVerifier*       aVerifier = NULL );

        /**
         * Fetches an attachment of given type from external application.
         * @param aType           Fetch type.
         * @param aSelectedFiles  OUT: array of selected files.
         * @param aParams         Generic AIW parameters
         * @param aCheckDiskSpace Deprecated! No disk space check done ever.
         * @param aMultiSelect    if ETrue Media Gallery allows multiselect in file list.
         * @param aVerifier       To verify selection.       
         * @return EFalse if fetch canceled.
         */

        IMPORT_C static TBool FetchFileL(
            TMsgAttachmentFetchType aType,
            CDesCArray&             aSelectedFiles,
            CAiwGenericParamList*   aParams,
            TBool                   aCheckDiskSpace = EFalse,
            TBool                   aMultiSelect = EFalse,
            MMGFetchVerifier*       aVerifier = NULL );

        /**
         * Fetches an any attachment file from external application.
         * @param aFileName       OUT: filename of the fetched file.
         * @param aCoeEnv         Control environment
         * @param aFilter         To filter shown files.
         * @param aVerifier       To verify selection.       
         * @return EFalse if fetch canceled.
         */
        IMPORT_C static TBool FetchAnyFileL( TFileName& aFileName,
                                             CCoeEnv& aCoeEnv,
                                             MAknFileSelectionObserver* aVerifier = NULL,
                                             MAknFileFilter* aFilter = NULL );
                                               
        /**
         * Creates a new bitmap by drawing first and second on top of each other.
         *
         * The given bitmaps must be valid, i.e. in case they are SVG-bitmaps
         * SetSize must be called before the icons can be combined.
         *
         * @param aFirstBmp      first bitmap to be drawn.
         * @param aSecondBmp     second bitmap to be drawn on top of first.
         * @param aSecondBmpMask second bitmap mask.
         * @return new bitmap.
         */
        IMPORT_C static CFbsBitmap* CombineIconsL(
            const CFbsBitmap* aFirstBmp,
            const CFbsBitmap* aSecondBmp,
            const CFbsBitmap* aSecondBmpMask = NULL );

        /**
         * Creates a properly formatted string from size.
         * @param aTarget           OUT: size string.
         * @param aFileSize         file size
         * @param aKilobytesMinimum if ETrue sizes smaller than 1024 bytes are
         *                          returned as 1 kB.
         */
        IMPORT_C static void FileSizeToStringL(
            TDes& aTarget,
            TInt  aFileSize,
            TBool aKilobytesMinimum = EFalse );

        /**
         * Generates a legal filename from given text buffer.
         * @param aFileName  OUT: generated filename.
         * @param aBuffer    text buffer.
         * @param aMaxLength maximum length for filename.
         * @param aExt       optional file extension.
         */
        IMPORT_C static void GetFileNameFromBuffer(
            TFileName&      aFileName,
            const TDesC&    aBuffer,
            TInt            aMaxLength,
            const TDesC*    aExt = NULL );

        /**
         * Returns temp path used by msg editors and optionally
         * appends file name to it.
         * @param aTempPath  OUT: generated temp path and filename.
         * @param aFileName  optionally appended file name.
         */
        IMPORT_C static void GetMsgEditorTempPath(
            TFileName&      aTempPath,
            const TDesC*    aFileName = NULL );

    private:

        static CFbsBitmap* InvertBitmapL( const CFbsBitmap* aFirstBmp );
        
        static TBool FetchNoteL( CDesCArray& aSelectedFiles );
            
        static TBool FetchNewL(
            TNewServiceFileType     aType,
            CDesCArray&             aSelectedFiles,
            CAiwGenericParamList*   aParams,
            TBool                   aMultiSelect );
            
        static TBool FetchExistingL(
            TMediaFileType          aFileType,
            CDesCArray&             aSelectedFiles,
            TBool                   aMultiSelect,
            MMGFetchVerifier*       aVerifier );
    };

#endif // MSGATTACHMENTUTILS_H

// End of File
