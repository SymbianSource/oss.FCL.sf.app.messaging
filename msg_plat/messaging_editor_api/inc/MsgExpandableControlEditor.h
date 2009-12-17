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
* Description:  MsgExpandableControlEditor  declaration
*
*/



#ifndef CMSGEXPANDABLECONTROLEDITOR_H
#define CMSGEXPANDABLECONTROLEDITOR_H

// ========== INCLUDE FILES ================================

#include <eikrted.h>                       // for CEikRichTextEditor
#include <aknenv.h>                        // for CAknEnv
#include <AknUtils.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class MMsgBaseControlObserver;
class CItemFinder;
class CAknsFrameBackgroundControlContext;
class MAknsControlContext;
class TAknTextLineLayout;
class CMsgEditorCustomDraw;
// ========== CLASS DECLARATION ============================

/**
* Defines a rich text editor for CMsgExpandableControl.
*/
class CMsgExpandableControlEditor : public CEikRichTextEditor
    {
    public:

        /**
        * Constructor.
        * @param aFlags
        * @param aBaseControlObserver
        */
        IMPORT_C CMsgExpandableControlEditor(
            TUint32&                 aFlags,
            MMsgBaseControlObserver* aBaseControlObserver);

        /**
        * Constructor.
        * @param aParent
        * @param aFlags
        * @param aBaseControlObserver
        */
        IMPORT_C CMsgExpandableControlEditor(
            const CCoeControl*       aParent,
            TUint32&                 aFlags,
            MMsgBaseControlObserver* aBaseControlObserver );

        /**
        * Destructor.
        */
        IMPORT_C ~CMsgExpandableControlEditor();

        /**
        * 2nd phase constructor.
        */
        IMPORT_C virtual void ConstructL();

        /**
        * Creates this control from resource.
        * @param aReader
        */
        IMPORT_C virtual void ConstructFromResourceL( TResourceReader& aReader );

        /**
        * Calculates and sets the size of the control and returns new size as
        * reference aSize.
        * @param aSize
        */
        IMPORT_C virtual void SetAndGetSizeL( TSize& aSize );

        /**
        * Checks and returns control's edit permissions.
        * @return
        */
        IMPORT_C virtual TUint32 CheckEditPermission() const;

        /**
        * Returns a default one line height of the editor.
        * @return
        */
        IMPORT_C virtual TInt DefaultHeight() const;

        /**
        * Checks if the cursor is on the first line.
        * @return
        */
        IMPORT_C TBool CursorInFirstLine();

        /**
        * Checks if the cursor is on the last line.
        * @return
        */
        IMPORT_C TBool CursorInLastLine();

        /**
        * Returns the current control rect.
        * @return
        */
        IMPORT_C TRect CurrentLineRectL();

        /**
        * Checks if first text line is visible.
        */
        IMPORT_C TBool IsFirstLineVisible();

        /**
        * Checks if last text line is visible.
        */
        IMPORT_C TBool IsLastLineVisible();

        /**
        * Handles clipboard operation.
        * @param aClipboardFunc
        */
        void ClipboardL( CEikEdwin::TClipboardFunc aClipboardFunc );

        /**
        * Sets maximun number of characters.
        * @param aMaxNumberOfChars
        */
        void SetMaxNumberOfChars( TInt aMaxNumberOfChars );

        /**
        * Set text content for the control.
        * @param aText
        */
        IMPORT_C void SetTextContentL( CRichText& aText );

        /**
        * Returns a minimum one line height of the editor.
        * @return
        */
        TInt MinimumDefaultHeight() const;

        /**
        * Resets (= clears) contents of the control.
        */
        IMPORT_C void Reset();

        /**
        * Sets base control observer.
        * @param aBaseControlObserver
        */
        IMPORT_C void SetBaseControlObserver(
            MMsgBaseControlObserver& aBaseControlObserver );

        /**
        *
        */
        IMPORT_C void AdjustLineHeightL();

        /**
        * Returns approximate height of text.
        */
        IMPORT_C TInt VirtualHeight();

        /**
        * Returns a topmost visible text position.
        */
        IMPORT_C TInt VirtualVisibleTop();

        /**
        *  Sets text format.
        */
        virtual void LayoutEdwin();

        /**
        * Returns pointer to CItemFinder object. Can return NULL if automatic find is not used.
        */
        IMPORT_C CItemFinder* ItemFinder() const;
        
        /**
        * Reads layout data from "LAF".
        */
        virtual void ResolveLayouts();

        /**
        * Resets the size of the possible frame context
        */
        void HandleFrameSizeChangeL();
        
        /**
        * Called by the parent control to notify editor about
        * read only state change. Includes common expandable editor 
        * related functionality. Base class implementation should be called 
        * by the derived class.
        *
        * @param aReadOnly ETrue if read only editor. Otherwise EFalse.
        */
        virtual void PrepareForReadOnlyL( TBool aReadOnly );
        
        /**
        * Sets the currently applicable text skin color ID. Default implementation
        * sets EAknsCIQsnTextColorsCG6 if editor is read-only or does not
        * have focus. Otherwise EAknsCIQsnTextColorsCG24 is set. 
        * Derived class should implement this function if different
        * behaviour is wanted. This function is called when focus or read-only state
        * changes. 
        */
        virtual void SetTextSkinColorIdL();
        
        /**
         * Returns whether cursor is currently visible.
         * 
         * @return ETrue/EFalse Visible/Not visible.
         */
        TBool IsCursorVisibleL();
        
         /**
         * Returns pointer to MsgEditorCustomDraw object. .
         */
         CMsgEditorCustomDraw* MsgEditorCustomDraw() const;
         
         /**
         * Sets the control type
         * 
         * @return None.
         */
         void SetControlType(TInt);
         
    public:  // from CEikRichTextEditor

        /**
        * Handles key events.
        * @param aKeyEvent
        * @param aType
        * @return
        */
        IMPORT_C TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent, TEventCode aType );
            
        /**
        * For handling dynamic layout switch.
        */
        void HandleResourceChange( TInt aType );

    public:  // from MEikCcpuEditor

        IMPORT_C TBool CcpuCanCut() const;

        IMPORT_C TBool CcpuCanCopy() const;

        IMPORT_C TBool CcpuCanPaste() const;

        IMPORT_C TBool CcpuCanUndo() const;

        IMPORT_C void CcpuCutL();

        IMPORT_C void CcpuCopyL();

        IMPORT_C void CcpuPasteL();

        IMPORT_C void CcpuUndoL();

    protected:

        /**
        * Checks if the plain text mode is on and returns ETrue if it is.
        * @return
        */
        TBool IsPlainTextMode() const;

        /**
        * Returns a default one line height of the editor.
        * @return
        */
        TInt DefaultHeight( TBool aOneToOne ) const;

        /**
        * Returns used text font.
        * @return
        */
        IMPORT_C virtual const CFont* TextFont() const;     
        
        /**
        * Layouts the edwin correctly.
        */
        void DoLayoutEdwin( const TRect& aEdwinParent, 
                            const TAknTextLineLayout& aLayout );
        
    protected:  // from CCoeControl

        /**
        *
        */
        IMPORT_C void ActivateL();

        /**
        * This is called when the focus of the control is changed.
        * @param aDrawNow
        */
        IMPORT_C void FocusChanged( TDrawNow aDrawNow );

    private:

        /**
        * Constructor (not available).
        */
        CMsgExpandableControlEditor();

        /**
        * Calculates the inner and outer rect of the focus frame
        */
        void OuterAndInnerRects( TRect& aOuter, TRect& aInner );
        
        /**
        * Performs focus change handling.
        */
        void DoFocusChangedL();

    protected:

        const CCoeControl*       iParentControl;
        TUint32&                 iFlags;  // reference to CMsgBaseControl's flags
        MMsgBaseControlObserver* iBaseControlObserver;
        TInt                     iMaxNumberOfChars;
        TCharFormat              iActiveFormat; // not used
        TAknLayoutText           iTextLayout;
	    CItemFinder*             iItemFinder;

        CAknsFrameBackgroundControlContext* iFrameBgContext;
        MAknsControlContext* iBgContext;

       CMsgEditorCustomDraw*    iCustomDraw;
       TInt                     iControlType;
       TInt                     iEditortop;
    };

#endif // CMSGEXPANDABLECONTROLEDITOR_H

// End of File

