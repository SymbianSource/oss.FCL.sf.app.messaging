/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Object info derived from attachment info.
*
*/



#ifndef C_UNIOBJECTLISTARRAY_H
#define C_UNIOBJECTLISTARRAY_H

// ========== INCLUDE FILES ================================

#include <MsgViewAttachmentsDialog.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================
class CUniObject;
class CUniDataUtils;
class CMsgAttachmentModel;
class CGulIcon;

// ========== DATA TYPES ===================================

// ========== CLASS DECLARATION ============================

/**
*  Object info list item array derived from attachment list item array.
*
*  @lib UniUtils.lib
*
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CUniObjectListItemArray ) : public CMsgAttaListItemArray
    {
    public:

        /**
         * Factory method that creates this object.
         *
         * @since S60 3.2
         *
         * @param aAttachmentModel Reference to attachment model
         * @param aIconArray Object item's icon array.
         * @param aDataUtils Reference to data utils.
         *
         * @return Pointer to the new CMsgAttaListItemArray object    
         */
        static CMsgAttaListItemArray* NewL( CMsgAttachmentModel& aAttachmentModel,
                                            CArrayPtr<CGulIcon>* aIconArray,
                                            CUniDataUtils& aDataUtils );

    public: // From MDesCArray

        /**
         * From MDesCArray.
         *
         * @since S60 3.2
         * 
         * @return Object name string.
         */
        TPtrC MdcaPoint( TInt aIndex ) const;

    private:

        /**
         * Constructor.
         *
         * @param aAttachmentModel 
         * @param aIconArray       
         */
        CUniObjectListItemArray( CMsgAttachmentModel& aAttachmentModel,
                                 CArrayPtr<CGulIcon>* aIconArray,
                                 CUniDataUtils& aDataUtils );

    private: // data
    
        CUniDataUtils& iDataUtils;
    };
    
#endif // C_UNIOBJECTLISTARRAY_H

// End of File
