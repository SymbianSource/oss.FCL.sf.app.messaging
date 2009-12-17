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
* Description:  MsgAttachmentItemDrawer  declaration
*
*/



#ifndef CMSGATTACHMENTITEMDRAWER_H
#define CMSGATTACHMENTITEMDRAWER_H

// ========== INCLUDE FILES ================================
#include <aknlists.h>       // for CAknDoubleLargeStyleListBox

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgAttachmentModel;
class CMsgAttachmentsListBox;
class CFormattedCellListBoxData;
class CFont;
class MTextListBoxModel;

// ========== CLASS DECLARATION ============================

/**
* 
*/
class CMsgAttachmentItemDrawer : public CDoubleLargeStyleItemDrawer
    {
    public:

        CMsgAttachmentItemDrawer(
            MTextListBoxModel*          aTextListBoxModel,
            const CFont*                aFont,
            CFormattedCellListBoxData*  aFormattedCellData,
            CMsgAttachmentModel&        aAttachmentModel,
            CMsgAttachmentsListBox*     aAttaListBox);


    public: // From CDoubleLargeStyleItemDrawer

        void DrawItem(
            TInt    aItemIndex,
            TPoint  aItemRectPos,
            TBool   aItemIsSelected,
            TBool   aItemIsCurrent,
            TBool   aViewIsEmphasized,
            TBool   aViewIsDimmed) const;

    private: // data

        CMsgAttachmentModel&         iAttachmentModel;
    };

#endif // CMSGATTACHMENTITEMDRAWER_H

// End of File
