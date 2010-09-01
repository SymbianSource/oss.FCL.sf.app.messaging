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
* Description:   Contains collection of commonly used 'utility' functions.
*
*/




// INCLUDE FILES
#include <aknenv.h>
#include <AknUtils.h>
#include <aknlayout.cdl.h>
#include <applayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknStatuspaneUtils.h>     // for AknStatuspaneUtils

#include "MsgEditorCommon.h"
#include "MsgEditorModelPanic.h"        // Panics

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// MsgEditorCommons::MaxBodyHeight
//
// Gets the value for maximum body height for variants.
// Replaces const TInt KMsgMaxBodyHeight definition in msgeditorcommon.h
// ---------------------------------------------------------
//
EXPORT_C TInt MsgEditorCommons::MaxBodyHeight()
    {
    return ( EditorViewHeigth() / MsgBaseLineDelta() ) * MsgBaseLineDelta();
    }

// ---------------------------------------------------------
// MsgEditorCommons::MsgBaseLineDelta
//
// Gets the value for maximum base line for variants.
// Replaces const TInt KMsgBaseLineDelta definition in msgeditorcommon.h
// ---------------------------------------------------------
//
EXPORT_C TInt MsgEditorCommons::MsgBaseLineDelta()
    {
    TInt baseLine = 0;

    TAknLayoutRect msgTextPane;
    msgTextPane.LayoutRect(
        MsgEditorCommons::MsgDataPane(),
        AknLayoutScalable_Apps::msg_text_pane( 0 ).LayoutLine() );
    baseLine = msgTextPane.Rect().Height();

    // We cannot handle non-positive baseLine values. Panic
    // intentionally if such baseLine is present! This indicates
    // serious problems with the layout data.
    __ASSERT_ALWAYS( baseLine > 0, Panic( EMsgLayoutUndetermined ) );
    return baseLine;
    }

// ---------------------------------------------------------
// MsgEditorCommons::MsgBaseLineOffset
// ---------------------------------------------------------
//
EXPORT_C TInt MsgEditorCommons::MsgBaseLineOffset()
    {
    TInt baseLineOffset = 0;
    TInt baseLine = MsgEditorCommons::MsgBaseLineDelta();
    baseLineOffset = MsgEditorCommons::MsgMainPane().Height() % baseLine;
        
    return baseLineOffset;
    }


EXPORT_C TInt MsgEditorCommons::ScreenWidth()
    {
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );
    return screen.Width();
    }

EXPORT_C TInt MsgEditorCommons::ScreenHeigth()
    {
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );
    return screen.Height();
    }

EXPORT_C TInt MsgEditorCommons::EditorViewWidth()
    {
    return MsgEditorCommons::MsgMainPane().Width();
    }

EXPORT_C TInt MsgEditorCommons::EditorViewHeigth()
    {
    return MsgEditorCommons::MsgMainPane().Height();
    }

EXPORT_C TRect MsgEditorCommons::MsgMainPane()
    {
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    
    TAknLayoutRect msgMainLayout;
    msgMainLayout.LayoutRect( mainPaneRect,
                              AknLayoutScalable_Apps::main_msg_pane().LayoutLine() );

    return msgMainLayout.Rect();
    }

EXPORT_C TRect MsgEditorCommons::MsgDataPane()
    {
    TRect msgMainPane = MsgEditorCommons::MsgMainPane();
    
#ifdef RD_SCALABLE_UI_V2
    msgMainPane.Move( -msgMainPane.iTl );
#endif // RD_SCALABLE_UI_V2

    TAknLayoutRect msgDataLayout;
    msgDataLayout.LayoutRect(
        msgMainPane,
        AknLayoutScalable_Apps::msg_data_pane().LayoutLine() );
        
    return msgDataLayout.Rect();
    }

EXPORT_C TRect MsgEditorCommons::MsgHeaderPane()
    {
    return MsgEditorCommons::MsgDataPane();
    }

EXPORT_C TRect MsgEditorCommons::MsgBodyPane()
    {
    return MsgEditorCommons::MsgDataPane();
    }

EXPORT_C void MsgEditorCommons::RoundToNextLine( TInt& aValue, TInt aLineHeight )
    {
    TInt remainder( aValue % aLineHeight );
    if ( remainder != 0 )
        {
        aValue < 0 ? aValue -= aLineHeight + remainder :
                     aValue += aLineHeight - remainder;
        }
    }

EXPORT_C void MsgEditorCommons::RoundToPreviousLine( TInt& aValue, TInt aLineHeight )
    {
    TInt remainder( aValue % aLineHeight );
    if ( remainder != 0 )
        {
        aValue -= remainder;
        }
    }

//  End of File  
