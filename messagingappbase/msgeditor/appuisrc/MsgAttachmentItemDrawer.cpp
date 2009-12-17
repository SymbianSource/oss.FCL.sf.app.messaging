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
* Description:  MsgAttachmentItemDrawer implementation
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
// CMsgAttachmentItemDrawer::CMsgAttachmentItemDrawer
//
//
// ---------------------------------------------------------
//
CMsgAttachmentItemDrawer::CMsgAttachmentItemDrawer(
    MTextListBoxModel* aTextListBoxModel,
    const CFont* aFont,
    CFormattedCellListBoxData* aFormattedCellData,
    CMsgAttachmentModel& aAttachmentModel,
    CMsgAttachmentsListBox* aAttaListBox )
    : CDoubleLargeStyleItemDrawer( aTextListBoxModel, aFont, aFormattedCellData, aAttaListBox ),
      iAttachmentModel( aAttachmentModel )
    {
    }

// ---------------------------------------------------------
// CMsgAttachmentItemDrawer::DrawItem
//
//
// ---------------------------------------------------------
//
void CMsgAttachmentItemDrawer::DrawItem(
    TInt aItemIndex,
    TPoint aItemRectPos,
    TBool aItemIsSelected,
    TBool aItemIsCurrent,
    TBool aViewIsEmphasized,
    TBool aViewIsDimmed ) const
    {   
    CDoubleLargeStyleItemDrawer::DrawItem(
        aItemIndex,
        aItemRectPos,
        aItemIsSelected,
        aItemIsCurrent,
        aViewIsEmphasized,
        aViewIsDimmed );

    CMsgAttachmentInfo& currAttInfo =
            iAttachmentModel.AttachmentInfoAt( aItemIndex );

    if ( currAttInfo.IsSeparator() )
        {
        // draw separator line before item.
        
        // Resolve separator layout
        TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
        TAknLayoutRect listPaneLayout;
        listPaneLayout.LayoutRect(
            mainPane,
            AknLayoutScalable_Avkon::list_gen_pane( 0 ).LayoutLine() );
        TAknLayoutRect doubleGraphicLayout;
        doubleGraphicLayout.LayoutRect(
            listPaneLayout.Rect(),
            AknLayoutScalable_Avkon::list_double_large_graphic_pane( 0 ).LayoutLine() );
        TAknLayoutRect separator;
        separator.LayoutRect(             
            doubleGraphicLayout.Rect(),
            AknLayoutScalable_Avkon::list_double_large_graphic_pane_g5( 1 ).LayoutLine() );

        MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal( iGc );
        if ( transApi )
            {
            TRect lineRect( TPoint( separator.Rect().iTl.iX, aItemRectPos.iY ),
                            TPoint( separator.Rect().iBr.iX, aItemRectPos.iY + separator.Rect().Height() ) );
            transApi->BeginRedraw( MAknListBoxTfxInternal::EListItem, lineRect, aItemIndex );
            transApi->StartDrawing( MAknListBoxTfxInternal::EListItem );
            }
        
        iGc->SetPenColor( separator.Color() );
        iGc->DrawLine( TPoint( separator.Rect().iTl.iX, aItemRectPos.iY ),
                       TPoint( separator.Rect().iBr.iX, aItemRectPos.iY ) );
                       
        // Reset gc because empty area is drawn after this.
        // Otherwise lines in empty area is drawn with line color!
        iGc->Reset();
        
        if ( transApi )
            {
            transApi->StopDrawing();
            transApi->EndRedraw( MAknListBoxTfxInternal::EListItem, aItemIndex );
            }
        }
    }

// End of File
