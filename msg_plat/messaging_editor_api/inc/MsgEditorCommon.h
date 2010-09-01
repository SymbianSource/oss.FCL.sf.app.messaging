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
* Description:  MsgEditorCommon  declaration
*
*/



#ifndef INC_MSGEDITORCOMMON_H
#define INC_MSGEDITORCOMMON_H

// ========== INCLUDE FILES ================================

// ========== CONSTANTS ====================================

// MsgEditor bitmaps.
_LIT( KMsgEditorMbm, "\\resource\\apps\\msgeditor.mbm" );
_LIT( KMsgEditorMbm_AH, "\\resource\\apps\\msgeditor_ah.mbm" );

const TText KArabicSemicolon = 0x061B;
const TText KSemicolon = 0x003B;
const TInt KUniLongTapStartDelay( 500000 ); // 0.5s
const TInt KUniLongTapTimeDelay( 1500000 ); // 1.5s

// ========== MACROS =======================================

/**
 * Some macros to tag places where additional implementation is needed.
 * Use like:
 * <pre>
 * #pragma TBI(message)
 * </pre>
 * "message" will be displayed during compilation and at least in
 * Visual Studio it is easy to move to tagged place in code by
 * simply double clicking message line in Output-window.
 */
#define STR(x) #x
#define XSTR(x) STR(x)
#define TBI(x) message(__FILE__ "(" XSTR(__LINE__) ") : warning TBI: " #x)

// =========================================================

// ========== DATA TYPES ===================================

// Message priority enumerations.
enum TMsgEditorMsgPriority
    {
    EMsgEditorMsgPriorityLow,
    EMsgEditorMsgPriorityNormal,
    EMsgEditorMsgPriorityHigh
    };

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

// CLASS DECLARATION
/**
* MsgEditorCommons is a collection of commonly used 'utility' functions.
*/
class MsgEditorCommons
    {
    public:

    /**
    * Gets consts for different build variants.
    * @param none.
    */
    IMPORT_C static TInt MaxBodyHeight();

    /**
    * Gets consts for different build variants.
    * @param none.
    */
    IMPORT_C static TInt MsgBaseLineDelta();
    IMPORT_C static TInt MsgBaseLineOffset();

    /**
    *
    */
    IMPORT_C static TInt ScreenWidth();
    IMPORT_C static TInt ScreenHeigth();


    /**
    *
    */
    IMPORT_C static TInt EditorViewWidth();
    IMPORT_C static TInt EditorViewHeigth();

    IMPORT_C static TRect MsgMainPane();
    IMPORT_C static TRect MsgDataPane();
    IMPORT_C static TRect MsgHeaderPane();
    IMPORT_C static TRect MsgBodyPane();
    
    /**
     * Rounds given value to next multiple of line height.
     * @value aValue Rounded value.
     * @value aLineHeight Current line height
     */
    IMPORT_C static void RoundToNextLine( TInt& aValue, TInt aLineHeight );
    
    /**
     * Rounds given value to previous multiple of line height.
     * @value aValue Rounded value.
     * @value aLineHeight Current line height
     */
    IMPORT_C static void RoundToPreviousLine( TInt& aValue, TInt aLineHeight );
    };

#endif //ifndef INC_MSGEDITORCOMMON_H

// End of File
