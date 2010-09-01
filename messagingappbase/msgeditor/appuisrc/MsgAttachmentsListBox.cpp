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
* Description:  MsgAttachmentsListBox implementation
*
*/



// ========== INCLUDE FILES ================================
#include <msvapi.h>
#include <eikspane.h>
#include <eikmenup.h>
#include <AknDef.h>
#include <aknappui.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <aknlists.h>
#include <gulicon.h>
#include <coeutils.h>               // for ConeUtils::FileExists
#include <bautils.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>          // CAknWaitDialog
#include <DocumentHandler.h>
#include <StringLoader.h>           // StringLoader
#include <AiwCommon.h>
#include <NpdApi.h>

#include <mmsvattachmentmanager.h>

#include <avkon.hrh>

#include <aknlayout.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include <AknUtils.h>
#include <AknsUtils.h>          // skinned icons
#include <AknsConstants.h>      // skinned icon ids

#include "MsgEditorCommon.h"        // for KMsgEditorMbm
#include "MsgEditorDocument.h"
#include "MsgAttachmentInfo.h"
#include "MsgAttachmentModel.h"
#include "MsgViewAttachmentsDialog.h"
#include "MsgAttachmentUtils.h"
#include "MsgAttaListItemArray.h"

#include <msgeditor.mbg>
#include "MsgEditorAppUi.hrh"
#include <MsgEditorAppUi.rsg>       // resouce identifiers

#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgAttachmentsListBox::CMsgAttachmentsListBox
//
//
// ---------------------------------------------------------
//
CMsgAttachmentsListBox::CMsgAttachmentsListBox( CMsgAttachmentModel& aAttachmentModel )
    : CAknDoubleLargeStyleListBox(),
      iAttachmentModel( aAttachmentModel )
    {
    }

// ---------------------------------------------------------
// void CMsgAttachmentsListBox::CreateItemDrawerL
//
//
// ---------------------------------------------------------
//
void CMsgAttachmentsListBox::CreateItemDrawerL()
    {
    CFormattedCellListBoxData* cellData = CFormattedCellListBoxData::NewL();
    CleanupStack::PushL( cellData );
    iItemDrawer = new ( ELeave ) CMsgAttachmentItemDrawer(
        Model(),
        iEikonEnv->NormalFont(),
        cellData,
        iAttachmentModel,
        this );
    CleanupStack::Pop(); // cellData
    }

// End of File
