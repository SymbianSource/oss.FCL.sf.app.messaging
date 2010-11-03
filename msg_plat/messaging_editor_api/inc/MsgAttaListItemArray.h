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
* Description:  MsgAttaListItemArray  declaration
*
*/



#ifndef CMSGATTALISTITEMARRAY_H
#define CMSGATTALISTITEMARRAY_H

// ========== INCLUDE FILES ================================
#include <e32base.h>
#include <bamdesca.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CGulIcon;
class CMsgAttachmentModel;

// ========== CLASS DECLARATION ============================

/**
* Attachment list item.
*/
class CMsgAttaListItemArray : public CBase,
                              public MDesCArray
    {
    public:

        /**
        *        
        */
        IMPORT_C static CMsgAttaListItemArray* NewL(CMsgAttachmentModel& aAttachmentModel, CArrayPtr<CGulIcon>* aIconArray);

        /**
        *
        */
        IMPORT_C virtual ~CMsgAttaListItemArray();

        /**
        *
        */
        IMPORT_C void ConstructL();

    public: // from MDesCArray

        /**
        *
        */
        IMPORT_C TInt MdcaCount() const;

        /**
        *
        */
        TPtrC MdcaPoint(TInt aIndex) const;

    protected:

        /**
        *
        */
        IMPORT_C CMsgAttaListItemArray(CMsgAttachmentModel& aAttachmentModel, CArrayPtr<CGulIcon>* aIconArray);

    protected: // Data

        CMsgAttachmentModel&    iAttachmentModel;
        CArrayPtr<CGulIcon>*    iIconArray;
        TDes*                   iListItemText;
    };


#endif // CMSGATTALISTITEMARRAY_H

// End of File
