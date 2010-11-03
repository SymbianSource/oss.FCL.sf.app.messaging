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
* Description:  MsgAttachmentModel  declaration
*
*/



#ifndef INC_MSGATTACHMENTMODEL_H
#define INC_MSGATTACHMENTMODEL_H

// ========== INCLUDE FILES ================================

#include <msvstd.h>
#include <apgcli.h>   // RApaLsSession

#include <cmsvattachment.h>
#include <MsgAttachmentInfo.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CGulIcon;
class MMsgAttachmentModelObserver;
class CMsgAttachmentInfo;
class CFbsBitmap;

// ========== DATA TYPES ===================================

typedef CArrayPtrFlat<CMsgAttachmentInfo> CAttInfoArray;

// ========== CLASS DECLARATION ============================

/**
 * Attachment model.
 *
 */
class CMsgAttachmentModel : public CBase
    {
    public:

        /**
         * Factory method that creates this object.
         * @param aReadOnly
         * @return
         */
        IMPORT_C static CMsgAttachmentModel* NewL(TBool aReadOnly);

        /**
         * Destructor.
         */
        IMPORT_C ~CMsgAttachmentModel();

        /**
         * Sets attachment model observer.
         * @param aObserver
         */
        IMPORT_C void SetObserver(MMsgAttachmentModelObserver* aObserver);

        /**
         * Returns number of attachments in the internal array of attachments.
         * @return
         */
        IMPORT_C TInt NumberOfItems() const;

        /**
         * Returns reference to attachment info at given index.
         * @param aIndex
         * @return
         */
        IMPORT_C CMsgAttachmentInfo& AttachmentInfoAt(TInt aIndex) const;

        /**
         * Returns pointer to bitmap of the application that handles given file.
         * If application not found returns default bitmap and sets supported flag
         * of the aAttaInfo to EFalse.
         * @param aAttaInfo
         * @return
         */
        IMPORT_C CGulIcon* BitmapForFileL(CMsgAttachmentInfo& aAttaInfo);

        /**
         * Adds attachment to the internal array. Notifies attachment model
         * observer with EMsgAttachmentAdded parameter.
         * @param aFullName
         * @param aSize
         * @param aAttachmentEntryId
         * @param aFetched
         * @param aIndex
         * @return
         */
        IMPORT_C CMsgAttachmentInfo* AddAttachmentL(
            const TDesC&     aFullName,
            TInt             aSize,
            TMsvAttachmentId aAttachmentEntryId,
            TBool            aFetched,
            TInt             aIndex = -1);

        /**
         * Adds attachment to the internal array. Notifies attachment model
         * observer with EMsgAttachmentAdded parameter.
         * @param aFullName
         * @param aSize
         * @param aAttachmentEntryId
         * @param aFetched
         * @param aDataType
         * @param aIndex
         * @return
         */
        IMPORT_C CMsgAttachmentInfo* AddAttachmentL(
            const TDesC&     aFullName,
            TInt             aSize,
            TMsvAttachmentId aAttachmentEntryId,
            TBool            aFetched,
            const TDataType& aDataType,
            TInt             aIndex = -1);
        
         /**
         * Adds attachment to the internal array. Notifies attachment model
         * observer with EMsgAttachmentAdded parameter.
         * @param aFullName
         * @param aSize
         * @param aAttachmentEntryId
         * @param aFetched
         * @param aDataType
         * @param aDRMDataType
         * @param aIndex
         * @return
         */
        IMPORT_C CMsgAttachmentInfo* AddAttachmentL(
            const TDesC&                     aFullName,
            TInt                             aSize,
            TMsvAttachmentId                 aAttachmentEntryId,
            TBool                            aFetched,
            const TDataType&                 aDataType,
            CMsgAttachmentInfo::TDRMDataType aDRMDataType,
            TInt                             aIndex = -1 );
        
        /**
         * Deletes attachment from internal array. Notifies attachment model
         * observer with EMsgAttachmentRemoved parameter.
         * @param aIndex
         */
        IMPORT_C void DeleteAttachment(TInt aIndex);

        /**
         * Returns true if stored info about attachments is modified.
         * @return
         */
        IMPORT_C TBool IsModified() const;

        /**
         * Sets the modified flag.
         * @param aModified
         */
        IMPORT_C void SetModified(TBool aModified);

        /**
         * Returns ETrue if attachment model was created to be read-only.
         * @return
         */
        IMPORT_C TBool IsReadOnly();

        /**
         * Returns ETrue if given file is found from the internal array.
         * @param aFileName
         * @return
         */
        IMPORT_C TBool IsFileAttached(const TDesC& aFileName) const;

        /**
         * Returns ETrue if given attachment id is found from the internal array.
         * @param aId
         * @return
         */
        IMPORT_C TBool IsIdAttached(TMsvAttachmentId aId) const;

        /**
         * Opens seleceted attachment in corresponding target application.
         * @param aIndex
         */
        IMPORT_C void ViewAttachmentL(TInt aIndex);

        /**
         * Reset the attachment model (empties the internal array).
         */
        IMPORT_C void Reset();

        /**
         * Loads resources, i.e. bitmaps & layouts
         */
        IMPORT_C void LoadResourcesL();

        /*
        * Get file handle from "model observer"
        */
        IMPORT_C RFile GetAttachmentFileL( TInt aIndex );

    protected:

        /**
         * Constructor.
         * @param aReadOnly
         */
        IMPORT_C CMsgAttachmentModel(TBool aReadOnly);

        /**
         * 2nd phase constructor.
         */
        IMPORT_C void ConstructL();

    private:

        /**
         * Loads system default bitmap.
         * @param aSupported
         * @return
         */
        CGulIcon* LoadDefaultBitmapL(TBool aSupported);

        /**
         * Returns app uid of application that handles given file.
         * @param aFileName
         * @return
         */
        TUid AppUidForFileL(const TFileName& aFileName);

        /**
         * Returns app uid of application that handles given data type.
         * @param aDataType
         * @return
         */
        TUid AppUidForDataTypeL(const TDataType& aDataType);

        /**
         * Returns app uid of application that handles given game data.
         * @param aDataType
         * @param aFileName
         * @return
         */
        TUid AppUidFromGameFileL(TDataType& aDataType, const TDesC& aFileName);

        /**
         * Returns pointer to bitmap of given application.
         * @param aAppUid
         * @return
         */
        CGulIcon* BitmapForAppL( const TUid &aAppUid );

        /**
         * Creates new attachment info object.
         * @param aFileName
         * @param aSize
         * @param aFetched
         * @param aAttachedThisSession
         * @param aDataType
         * @param aDRMDataType
         * @return
         */
        virtual CMsgAttachmentInfo* CreateNewInfoL(
            const TDesC&                     aFileName,
            TInt                             aSize,
            TBool                            aFetched,
            TBool                            aAttachedThisSession, 
            TMsvAttachmentId                 aAttachmentId,
            const TDataType&                 aDataType,
            CMsgAttachmentInfo::TDRMDataType aDRMDataType );

        /**
         * Apply fade effect on given bitmap
         * @param aBitmap
         */
        void FadeBitmapL( CFbsBitmap* aBitmap );
        
        /**
         * Makes a copy of the original bitmap
         * @param aBitmap
         */
        CFbsBitmap* CopyBitmapL( const CFbsBitmap* aBitmap );
        
        /**
         * Deletes attachment from given index.
         *
         * @param aIndex Index of the deleted attachment.
         */
        void DoDeleteAttachmentL( TInt aIndex );
        
    protected:

        CAttInfoArray*                     iAttachInfoArray;
        TBool                              iModified;
        MMsgAttachmentModelObserver*       iAttachmentModelObserver;

    private:

        RApaLsSession   iAppList;
        TBool           iReadOnly;
        CGulIcon*       iDRMFWDLockIcon;
        CGulIcon*       iDRMSDIconRightsValid;
        CGulIcon*       iDRMFWDLockIconNoRights;
        CGulIcon*       iDRMSDIconNoRights;
        TSize           iIconSize;
        TSize           iAddIconSize;

    };

#endif

// End of File
