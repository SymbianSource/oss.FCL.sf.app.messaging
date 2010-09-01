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
* Description:  MsgBodyControlEditor  declaration
*
*/



#ifndef CMSGBODYCONTROLEDITOR_H
#define CMSGBODYCONTROLEDITOR_H

// ========== INCLUDE FILES ================================

#include <ItemFinder.h>
#include <itemfinderobserver.h>
#include "MsgExpandableControlEditor.h"            // for CMsgExpandableControlEditor
#include "MsgEditor.hrh"

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================
class CMsgEditorCustomDraw;

// ========== CLASS DECLARATION ============================


/**
* Defines an editor for the body control.
*/
class CMsgBodyControlEditor : public CMsgExpandableControlEditor,
                              public MItemFinderObserver,
                              public MAknItemFinderObserver
    {
    public:

        /**
        * Constructor.
        * @param aFlags
        */
        CMsgBodyControlEditor(
            const CCoeControl*       aParent,
            TUint32&                 aFlags,
            MMsgBaseControlObserver* aBaseControlObserver );

        /**
        * Destructor.
        */
        virtual ~CMsgBodyControlEditor();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Calculates and sets the size of the control and returns new size as reference aSize.
        * @param aSize
        */
        void SetAndGetSizeL( TSize& aSize );

        /**
        * Inserts a character to the editor.
        * @param aCharacter
        */
        void InsertCharacterL( const TChar& aCharacter );

        /**
        * Inserts text to the editor.
        * @param aText
        */
        void InsertTextL( const TDesC& aText );
        
        /**
        * Prepares focus transition.
        */
        void PrepareFocusTransition();

    public:  // from CMsgExpandableControlEditor

        /**
        * Checks if the cursor is on the last line.
        * @return
        */
        TBool CursorInLastLine();
        
        /**
        * Reads layout data from "LAF".
        */
        void ResolveLayouts();
        
        /**
        * Sets text format.
        */
        void LayoutEdwin();
        
        /**
        * Sets the currently applicable text skin color ID. 
        * Overwrites expandable editor behaviour and sets
        * only single text color independent of the control state.
        */
        void SetTextSkinColorIdL();
        
        /**
         * For handling dynamic layout switch.
         */
         void HandleResourceChange( TInt aType );
         
         /**
         * Preparing for readonly/ viewers
         */
         void PrepareForReadOnlyL( TBool aReadOnly );
        
    public:  // from CEikRichTextEditor

        /**
        * Handles key events.
        * @param aKeyEvent
        * @param aType
        * @return
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From CCoeControl. See coecntrl.h
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
    public: // from MItemFinderObserver

        void HandleParsingComplete();
        
    public: // from MAknItemFinderObserver  
       
        void HandleFindItemEventL(
                        const CItemFinder::CFindItemExt& aItem,
                        MAknItemFinderObserver::TEventFlag aEvent,
                        TUint aFlags = 0 );

    public: // new

        /**
        * 
        */
        TBool IsFocusChangePossibleL( TMsgFocusDirection aDirection );

        /**
        * Should be called when focus is changed to body. 
        * @param aBeginning: ETrue if search started from the beginning of the body text, EFalse if from the end
        */
        void SetupAutomaticFindAfterFocusChangeL( TBool aBeginning );

    protected: // new

        /**
        *
        */
        void SetHighlightL( TInt aCursorPos, TMsgFocusDirection aDirection, TBool aInit = EFalse );

    private:  // from CCoeControl

        /**
        *
        */
        void ActivateL();

    private:

        /**
        * Constructor (not available).
        */
        CMsgBodyControlEditor();

        /**
        * Performs parsing complete handling.
        */
        void DoHandleParsingCompleteL();
        
    private:

        TBool iTextParsed;
        TBool iFocusChangedBeforeParseFinish;
        TBool iInitTop; // ETrue if start from top, EFalse if bottom
        
        CMsgEditorCustomDraw* iCustomDraw;
        
        TInt iPreviousItemStart;
        TInt iPreviousItemLength;
    };

#endif // CMSGBODYCONTROLEDITOR_H

// End of File
