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
* Description:   Unified Message Editor objects model.
*
*/


#ifndef C_UNIOBJECTSMODEL_H
#define C_UNIOBJECTSMODEL_H

// ========== INCLUDE FILES ================================

#include <MsgAttachmentModel.h>
#include <MsgAttachmentUtils.h>
#include <unimodelconst.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CUniDataUtils;
class CUniObject;
class CUniObjectsInfo;

// ========== DATA TYPES ===================================

// ========== CLASS DECLARATION ============================

/**
* Unified Editor objects model derived from attachment model.
*
* @lib UniUtils.lib
*
* @since S60 3.2
*/
NONSHARABLE_CLASS( CUniObjectsModel ) : public CMsgAttachmentModel
    {

    public:

        /**
         * Factory method that creates this object.
         *
         * @since S60 3.2
         *
         * @param aReadOnly - Boolean flag.
         */
        IMPORT_C static CMsgAttachmentModel* NewL( TBool aReadOnly );

        /**
         * Destructor
         *
         * @since S60 3.2
         */
        virtual ~CUniObjectsModel();

        /**
         * Adds object to the internal array. Notifies attachment model
         * observer with EMsgAttachmentAdded parameter.
         * Overrides CMsgAttachmentModel::AddAttachmentL.
         *
         * @since S60 3.2
         *
         * @param aObject Object to be added.       
         * @param aDRMType DRM type of the object.               
         * @param aAttachmentObject ETrue is object is attachment. Otherwise EFalse. 
         * @param aSlideNumber Slide number where object is located.          
         * @param aIndex Index of the object.
         *             
         * @return Pointer to the added object's attachment info.
         */
        CUniObjectsInfo* AddObjectL(
            CUniObject& aObject,
            TInt aDRMType,
            TBool aAttachmentObject,
            TInt aSlideNumber,
            TInt aIndex = KErrNotFound );
        
        /**
         * Adds empty slide to the internal array. Notifies attachment model
         * observer with EMsgAttachmentAdded parameter.
         *
         * @since S60 3.2
         *
         * @param aEmptySlideName Empty slide name string.
         * @param aIndex Index of the object.
         *             
         * @return Pointer to the added object's attachment info.
         */
        CUniObjectsInfo* AddEmptySlideL(
            const TDesC& aEmptySlideName,
            TInt aIndex = KErrNotFound );

        /**
         * Deletes attachment from internal array. Notifies attachment model
         * observer with EMsgAttachmentRemoved parameter if aNotifyObserver flag is set.
         *
         * @since S60 3.2
         *
         * @param aIndex - index of the object to be deleted.
         * @param aNotifyObserver - Boolean flag to determine whether 
         *                          observer is notified about model change.
         */
        void DeleteObjectL( TInt aIndex, TBool aNotifyObserver );

        /**
         * Change order of attachment.
         *
         * @since S60 3.2
         *
         * @param aIndex - Index of the attachment
         * @param aNewPosition - Index of the new position of the attachment.
         */
        void ChangeOrderL (TInt aIndex, TInt aNewPosition);

        /**
         * Notify observer about the order change.
         *
         * @since S60 3.2
         *
         * @param aLayout - New layout of the message.
         */
        void NotifyChangeOrderL ( TUniLayout aLayout );

        /**
         * Loads mms slide bitmap.
         *
         * @since S60 3.2
         *
         * @return bitmap.
         */
        CGulIcon* BitmapForMmsSlideL();

    protected:

        /**
         * Constructor.
         *
         * @since S60 3.2
         *
         * @param aReadOnly 
         */
        CUniObjectsModel( TBool aReadOnly );

    private:

        /**
         * Creates new attachment (object) info object.
         *
         * @param aFileName            
         * @param aSize                
         * @param aFetched             
         * @param aAttachedThisSession 
         * @param aDataType
         * @param aDRMDataType
         */
        virtual CMsgAttachmentInfo* CreateNewInfoL(
            const TDesC&    aFileName,
            TInt            aSize,
            TBool           aFetched,
            TBool           aAttachedThisSession,
            TMsvAttachmentId aAttachmentId,
            const TDataType& aDataType,
            CMsgAttachmentInfo::TDRMDataType aDRMDataType );

    private:

        CGulIcon* iMmsSlideIcon;
    };

#endif // C_UNIOBJECTSMODEL_H

// End of File
