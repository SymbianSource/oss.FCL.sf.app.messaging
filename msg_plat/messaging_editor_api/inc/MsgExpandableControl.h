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
* Description:  MsgExpandableControl  declaration
*
*/



#ifndef CMSGEXPANDABLECONTROL_H
#define CMSGEXPANDABLECONTROL_H

// ========== INCLUDE FILES ================================

#include <eikedwob.h>                      // for MEikEdwinSizeObserver
#include <aknenv.h>                        // for CAknEnv
#include <AknUtils.h>
#include <eikcapc.h>
#include <fepbase.h>

#include "MsgExpandableTextEditorControl.h"                // for CMsgBaseControl

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgExpandableControlEditor;
class CEikLabel;
class CRichText;
class CGlobalText;
class CEikRichTextEditor;
class CAknButton;

// ========== CLASS DECLARATION ============================

/**
* Defines an expandable control for message editors/viewers.
*/
class CMsgExpandableControl : public CMsgExpandableTextEditorControl, 
                              public MCoeCaptionRetrieverForFep 
    {
    public:

        /**
        * Constructor.
        */
        IMPORT_C CMsgExpandableControl();

        /**
        * Constructor.
        */
        IMPORT_C CMsgExpandableControl( MMsgBaseControlObserver& aBaseControlObserver );

        /**
        * Destructor.
        */
        IMPORT_C ~CMsgExpandableControl();

        /**
        * Returns a reference to the editor control.
        * @return
        */
        IMPORT_C CEikRichTextEditor& Editor() const;

        /**
        * Returns a reference to the label control.
        * @return
        */
        IMPORT_C CEikLabel& Caption() const;

        /**
        * Sets text content of the control editor from the contents of 
        * a rich text object. Old content is discarded. Copies also
        * formatting from given rich text object if plain text mode
        * is disabled.
        *
        * @param aText New text content.
        */
        IMPORT_C void SetTextContentL( CRichText& aText );
        
        /**
        * Sets text content of the control editor from the contents of 
        * a descriptor. Old content is discarded.
        *
        * @since 3.2
        *
        * @param aText New text content.
        */
        IMPORT_C void SetTextContentL( const TDesC& aText );
                
        /**
        * Copies stripped content to aBuf. Control chars, newlines and if
        * aNoExtraSemicolons is ETrue, also two consecutive semicolons are
        * replaced with semicolon and space.
        * @param aBuf               buffer where stripped content will be copied.
        * @param aMaxLen            max length of aBuf.
        * @param aNoExtraSemicolons ETrue if two consecutive semicolons must be handled.
        */
        IMPORT_C void TextContentStrippedL( TDes& aBuf, TInt aMaxLen, TBool aNoExtraSemicolons = ETrue );

        /**
        * Calls iEditors HandleFrameSizeChangeL trapped
        */
        void HandleFrameSizeChange();
        
        /**
        * Returns a pointer to the button control.
        *
        * @since 3.2
        *
        * @return Pointer to button control if available, otherwise NULL.
        */
        CAknButton* Button() const;
        
    public:  // from CMsgBaseControl

        /**
        * Creates this control from resource.
        * @param aResourceId
        */
        void ConstructFromResourceL( TInt aResourceId );

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
        * Calculates and sets the size of the control and returns new size as
        * reference aSize.
        * @param aSize
        */
        IMPORT_C void SetAndGetSizeL( TSize& aSize );

        /**
        * Checks if the focus change up or down is possible and
        * returns ETrue if it is.
        * @param aDirection
        * @return
        */
        IMPORT_C TBool IsFocusChangePossible( TMsgFocusDirection aDirection ) const;
        
    public:  // from CCoeControl

        /**
        * Sets container window.
        * @param aContainer
        */
        IMPORT_C void SetContainerWindowL( const CCoeControl& aContainer );

        /**
         * Handles key events.
         * @param aKeyEvent
         * @param aType
         * @return
         */
        IMPORT_C TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                              TEventCode aType );

        /**
        * Returns minimum size of the control.
        * @return
        */
        IMPORT_C TSize MinimumSize();

        /**
        * Returns control's input capabilities.
        * @return
        */
        IMPORT_C TCoeInputCapabilities InputCapabilities() const;

        /**
        * For handling dynamic layout switch.
        */
        IMPORT_C void HandleResourceChange( TInt aType );

    public:  // from MCoeControlObserver

        /**
        * Handle control event.
        * @param aControl
        * @param aEventType
        */
        IMPORT_C void HandleControlEventL( CCoeControl* aControl, 
                                           TCoeEvent aEventType );
    public:  // from MCoeCaptionRetrieverForFep
    
        /**
        * Returns a caption for called
        * @param aCaption On return, contains the caption.
        */
        void GetCaptionForFep( TDes &aCaption ) const;
    protected:

        /**
        * Creates the editor for the control.
        * @return
        */
        CMsgExpandableControlEditor* CreateEditorL();

        /**
        * Reads control properties from resource.
        * @param aReader
        */
        virtual void ReadControlPropertiesFromResourceL( TResourceReader& aReader );

        /**
        * Creates caption for the control from resource.
        * @param aReader
        * @return
        */
        virtual CEikLabel* CreateCaptionFromResourceL( TResourceReader& aReader );

        /**
        * Creates editor for the control from resource.
        * @param aReader
        * @return
        */
        virtual CMsgExpandableControlEditor* CreateEditorFromResourceL(
            TResourceReader& aReader );
        
        /**
        * Sets the correct layout for button.
        */    
        virtual void LayoutButton();       
        
        /**
        * Gets ToolBar rect.
        */        
        TRect GetToolBarRect();
    protected:  // from CMsgBaseControl

        /**
        * Prepares for read only or non read only state.
        * @param aReadOnly
        */
        void PrepareForReadOnly( TBool aReadOnly );

        /**
        * Reads layout data from "LAF".
        */
        virtual void ResolveLayoutsL();

    protected:  // from CCoeControl

        /**
        * Returns a number of controls.
        * @return
        */
        IMPORT_C TInt CountComponentControls() const;

        /**
        * Returns a control of index aIndex.
        * @param aIndex
        * @return
        */
        IMPORT_C CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * Sets new position for all the controls.
        */
        IMPORT_C void SizeChanged();

        /**
        * This is called when the focus of the control is changed.
        * @param aDrawNow
        */
        IMPORT_C void FocusChanged( TDrawNow aDrawNow );

        /**
        * Draw control.
        * @param aRect
        */
        IMPORT_C void Draw( const TRect& aRect ) const;
        
        /**
        * Sets the control as ready to be drawn. This implementation
        * should be called by the derived classes implementing their
        * own ActivateL function.
        */
        IMPORT_C void ActivateL();
        
    private:
        
        /**
        * Performs view event handling.
        */
        void DoNotifyViewEventL( TMsgViewEvent aEvent, TInt aParam );
        
        /**
        * Updates text color for caption from current skin.
        */
        void UpdateCaptionTextColorL();
        
    protected:

        CEikLabel*                   iCaption;
        
        TAknLayoutText               iCaptionLayout;
        
        // Offset between control and caption. Used for highlight etc.
        TInt                         iCaptionTop;
        
        TAknLayoutText               iEditorLayout;
        
        // Offset between control and editor. Used for highlight etc.
        TAknLayoutRect               iFrameLayout;
        
        CAknButton*                  iButton;
        TAknLayoutRect               iButtonLayout;
        TInt                         iButtonTop;
        TInt                         iCtrltype;
    };

#endif // CMSGEXPANDABLECONTROL_H

// End of File
