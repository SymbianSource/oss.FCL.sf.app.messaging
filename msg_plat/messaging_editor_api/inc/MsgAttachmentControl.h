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
* Description:  MsgAttachmentControl  declaration
*
*/



#ifndef MSGATTACHMENTCONTROL_H
#define MSGATTACHMENTCONTROL_H

// ========== INCLUDE FILES ================================

#include <cntdef.h>             // for TContactItemId

#include "MsgExpandableControl.h"

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CMsgAttachmentModel;
class CMsgEditorView;

// ========== DATA TYPES ===================================

// ========== CLASS DECLARATION ============================

/**
* Defines an Attachment control for message editors/viewers.
*
*/
class CMsgAttachmentControl : public CMsgExpandableControl
    {
    public:

        /**
        * Factory method that creates this control.
        * 
        * @param aBaseControlObserver   Base control observer
        * @param aView                  Message editor view
        *
        * @return pointer to created object.
        */
        IMPORT_C static CMsgAttachmentControl* NewL( MMsgBaseControlObserver& aBaseControlObserver,
                                                     CMsgEditorView& aView );
        
        /**
        * Constructor.
        *
        * @param aBaseControlObserver Base control observer.
        */
        CMsgAttachmentControl( MMsgBaseControlObserver& aBaseControlObserver, 
                               CMsgEditorView& aView );

        /**
        * Destructor.
        */
        virtual ~CMsgAttachmentControl();

    public:

        /**
        * Appends one attachment to the end of the attachment control.
        *
        * @param aName Attachment name.
        */
        IMPORT_C void AppendAttachmentL( const TDesC& aName );
        
        /**
        * Inserts one attachment to specific index on attachment control.
        *
        * @param aName  Attachment name.
        * @param aIndex Position of new attachment.
        */
        IMPORT_C void InsertAttachmentL( const TDesC& aName, TInt aIndex );
        
        /**
        * Adds attachments from the array to the attachment control
        * removing old attachments.
        *
        * @param aNames Attachment name array.
        */
        IMPORT_C void AddAttachmentsL( const MDesCArray& aNames );

        /**
        * Adds attachments from the attachment model to the attachment control
        * removing old attachments.
        * 
        * @param aModel Attachment model.
        */
        IMPORT_C void AddAttachmentsL( const CMsgAttachmentModel& aModel );
        
        /**
        * Clears content of the attachment control (non-leaving version; from CMsgBaseControl).
        */
        IMPORT_C void Reset();
        
        /**
        * Returns current number of attachments on attachment control
        *
        * @return Number of attachments on attachment control
        */
        IMPORT_C TInt Count() const;
        
        /**
        * Returns attachment from given index
        *
        * @return Attachment on given index
        */
        IMPORT_C TPtrC Attachment( TInt aIndex ) const;
        
        /**
        * Removes first attachment with given name from the attachment control.
        *
        * @param aName Attachment name
        */
        IMPORT_C void RemoveAttachmentL( const TDesC& aName );
        
        /**
        * Removes attachment from specific index on attachment control.
        *
        * @param aIndex Attachment index
        */
        IMPORT_C void RemoveAttachmentL( TInt aIndex );
        
    public:   // from CMsgExpandableControl

        /**
        * Creates this control from resource.
        *
        * @param aResourceId 
        */
        void ConstructFromResourceL( TInt aResourceId );

        /**
        * Checks and returns control's edit permissions. Edit permissions are needed
        * to check in order to know whether some key press is allowed to pass
        * to the control or not.
        *
        * @return
        */
        TUint32 EditPermission() const;
        
        /**
        * Prepares control for viewing.
        * @param aEvent
        * @param aParam
        */
        void NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam );

    public:   // from CCoeControl

        /**
        * Handles key events.
        *
        * @param aKeyEvent
        * @param aType
        *
        * @return
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    
        /**
        * For handling dynamic layout switch.
        */
        void HandleResourceChange( TInt aType );
        
        /**
        * From CCoeControl. See coecntrl.h
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
    protected: // from CMsgExpandableControl
    
        /**
        * Reads layout data from "LAF".
        */
        void ResolveLayoutsL();
        
    protected: // from CMsgBaseControl
    
        /**
        * Prepares for read only or non read only state.
        *        
        * @param aReadOnly
        */
        void PrepareForReadOnly( TBool aReadOnly );
    
        /**
        * Sets the correct layout for button.
        */    
        void LayoutButton();   
        
    protected: // from CCoeControl

        /**
        * Returns a number of controls.
        *
        * @return
        */
        TInt CountComponentControls() const;

        /**
        * Returns a control of index aIndex.
        *
        * @param aIndex
        *
        * @return
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * This is called when the focus of the control is changed.
        *
        * @param aDrawNow
        */
        void FocusChanged( TDrawNow aDrawNow );

        /**
        * Draws the control.
        *
        * @param aRect
        */
        void Draw( const TRect& aRect ) const;
        
        /**
        * Sets control ready to be drawn.
        */
        void ActivateL();
        
    private:
        
        
        /**
        * Default C++ constructor
        */
        CMsgAttachmentControl();
        
        /**
        * Symbian second phase constructor.
        */
        void ConstructL();
        
        /**
        * Creates and sets text for editor.
        */
        void SetTextForEditorL( TBool aContentChanged, 
                                TBool aClipText );
        
        /**
        * Updates control internal state.
        */
        void UpdateControlL();
        
        /**
        * Creates attachment icons.
        */
        CGulIcon* CreateAttachmentIconsL();
        
        /**
        * Creates button from attachment icon.
        */
        void CreateButtonL();
        
        /**
        * Callback function to scroll text
        */
        static TInt DoScrollTextL( TAny* aObject );
        
        /**
        * Performs text scrolling.
        */
        void ScrollTextL();
        
        /**
        * Disables automatic scrolling.
        */
        void DisableAutomaticScrolling();
        
        /**
        * Enables automatic scrolling.
        */
        void EnableAutomaticScrolling();
        
        /**
        * Starts automatic scrolling.
        */
        void StartAutomaticScrolling( TInt aStartDelay );
        
        /**
        * Stops automatic scrolling.
        */
        void StopAutomaticScrolling();
        
        /**
        * Clips the given text to fit to the given width if needed.        
        */
        void ClipTextL( TPtr aText, TInt aClipWidth );
        
        /**
        * Updates field's focus.
        */
        void UpdateFocusL();
        
        /**
        * Performs resource change handling before base class.
        */
        void DoHandleResourceChangeBeforeL( TInt aType );
        
        /**
        * Performs resource change handling after base class.
        */
        void DoHandleResourceChangeAfterL( TInt aType );
        
        /**
        * Adds name to attachment array.
        */
        void AddToAttachmentArrayL( const TDesC& aName, TInt aIndex );
        
    private:
        
        enum TScrollPosition
            {
            EScrollPosBeginning,
            EScrollPosEnd,
            EScrollPosOther
            };
        
        //
        CDesCArrayFlat* iAttachmentArray;
        
        //
        CGulIcon* iLabelIcon;

        //
        TAknLayoutRect iIconLayout;
        
        // Indicates whether automatic scrolling is enabled.
        TBool iAutomaticScrolling;
        
        TScrollPosition iScrollPos;
        
        CPeriodic* iScrollTimer;
    };


#endif // MSGATTACHMENTCONTROL_H

// End of File
