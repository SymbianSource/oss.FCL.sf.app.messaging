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
* Description:  MsgBodyControl  declaration
*
*/



#ifndef CMSGBODYCONTROL_H
#define CMSGBODYCONTROL_H

// ========== INCLUDE FILES ================================

#include <eikedwob.h>                      // for MEikEdwinSizeObserver

#include "MsgExpandableTextEditorControl.h" // for CMsgExpandableTextEditorControl

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgBodyControlEditor;
class CRichText;
class CGlobalText;
class CEikRichTextEditor;
class CItemFinder;

// ========== CLASS DECLARATION ============================

/**
* Defines a body control for message editors/viewers.
*/
class CMsgBodyControl : public CMsgExpandableTextEditorControl, 
                        public MCoeCaptionRetrieverForFep, 
                        public MItemFinderObserver
    {
    public:
        
        /**
        * Factory method that creates this control.
        * @return
        */
        IMPORT_C static CMsgBodyControl* NewL( CCoeControl* aParent );
        
        /**
        * Destructor.
        */
        virtual ~CMsgBodyControl();

    public:

        /**
        * Returns a reference to the editor control.
        * @return
        */
        IMPORT_C CEikRichTextEditor& Editor() const;

        /**
        * Sets text content to the control editor.
        * @param aText
        */
        IMPORT_C void SetTextContentL( CRichText& aText );

        /**
        * Inserts a character to the editor.
        * @param aCharacter
        */
        IMPORT_C void InsertCharacterL( const TChar& aCharacter );

        /**
        * Inserts text to the editor.
        * @param aText
        */
        IMPORT_C void InsertTextL( const TDesC& aText );
        
        /**
         * Returns pointer to item finder.
         * 
         * @return Item finder or NULL.
         */
        CItemFinder* ItemFinder();

         /**
         * Should be called when focus is changed to body. 
         * 
         * @param aBeginning  ETrue if search started from the beginning of the body text, 
         *                    EFalse if from the end
         */
         void SetupAutomaticFindAfterFocusChangeL( TBool aBeginning );
        
    public:  // from CMsgBaseControl

        /**
        * Prepares control for viewing.
        * @param aEvent
        * @param aParam
        */
        void NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam );

        /**
        * Resets (= clears) contents of the control.
        */
        IMPORT_C void Reset();

        /**
        * Calculates and sets the size of the control and returns new size as reference aSize.
        * @param aSize
        */
        IMPORT_C void SetAndGetSizeL( TSize& aSize );

        /**
        * Checks if the focus change up or down is possible and returns ETrue if it is.
        * @param aDirection
        * @return
        */
        IMPORT_C TBool IsFocusChangePossible( TMsgFocusDirection aDirection ) const;
        
    public:  // from CCoeControl

        /**
        * Handles key events.
        * @param aKeyEvent
        * @param aType
        * @return
        */
        IMPORT_C TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Returns control's input capabilities.
        * @return
        */
        IMPORT_C TCoeInputCapabilities InputCapabilities() const;

        /**
        * For handling dynamic layout switch.
        */
        void HandleResourceChange( TInt aType );
        
    public:  // from MCoeCaptionRetrieverForFep
    
        /**
        * Returns a caption for called
        * @param aCaption On return, contains the caption(ICF prompt text) of body control.
        */
        void GetCaptionForFep( TDes &aCaption ) const;
        
    public: // from MItemFinderObserver

        /**
        * HandleParsingComplete
        */
        void HandleParsingComplete();
        
    protected:  // from CMsgBaseControl

        /**
        * Prepares for read only or non read only state.
        * @param aReadOnly
        */
        void PrepareForReadOnly( TBool aReadOnly );

    protected:  // from CCoeControl

        /**
        * Returns a number of controls.
        * @return
        */
        TInt CountComponentControls() const;

        /**
        * Returns a control of index aIndex.
        * @param aIndex
        * @return
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * Sets new position for all the controls.
        */
        void SizeChanged();

        /**
        * This is called when the focus of the control is changed.
        * @param aDrawNow
        */
        void FocusChanged( TDrawNow aDrawNow );

        /**
        * Sets container window.
        * @param aContainer
        */
        void SetContainerWindowL( const CCoeControl& aContainer );
        
    protected:
        
        /**
        * Constructor.
        */
        CMsgBodyControl( MMsgBaseControlObserver& aBaseControlObserver );
        
        /**
        * 2nd phase constructor.
        */
        void ConstructL();
    
    private:
        
        /**
        *
        */
        void CheckCorrectControlYPosL();

        /**
        * Reads layout data from "LAF".
        */
        void ResolveFontMetrics();
        
        /**
        * Performs view event handling.
        */
        void DoNotifyViewEventL( TMsgViewEvent aEvent, TInt aParam );
        /**
        * Sets the captiuon(ICF prompt text) of Body control
        * @param: None
        * @rreturn: None
        */
        void SetIcfPromptTextL();
    
        /**
        * Utility function to cast editor to correct type.
        */
        CMsgBodyControlEditor* BodyControlEditor() const;
    private:   
    
        HBufC*   iCaptionText; 
    };

#endif // CMSGBODYCONTROL_H

// End of File
