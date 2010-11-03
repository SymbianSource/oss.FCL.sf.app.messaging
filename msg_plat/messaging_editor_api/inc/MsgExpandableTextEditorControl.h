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
* Description:  MsgExpandableTextEditorControl  declaration
*
*/



#ifndef CMSGEXPANDABLETEXTEDITORCONTROL_H
#define CMSGEXPANDABLETEXTEDITORCONTROL_H

// ========== INCLUDE FILES ================================

#include <MsgBaseControl.h>                // for CMsgBaseControl
#include <coecobs.h>                       // for MCoeControlObserver

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
* Defines a base class for common functionality on controls containing text editor.
*/
class CMsgExpandableTextEditorControl : public CMsgBaseControl,
                                        public MCoeControlObserver,
                                        public MEikEdwinSizeObserver,
                                        public MEikEdwinObserver
    {
    public:

        /**
        * Constructor.
        */
        IMPORT_C CMsgExpandableTextEditorControl();

        /**
        * Constructor.
        */
        IMPORT_C CMsgExpandableTextEditorControl( MMsgBaseControlObserver& aBaseControlObserver );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMsgExpandableTextEditorControl();

        /**
        * Sets the plain text mode on or off.
        * @param aMode
        */
        IMPORT_C virtual void SetPlainTextMode( TBool aMode );

        /**
        * Checks if the plain text mode is on and returns ETrue if it is.
        * @return
        */
        IMPORT_C virtual TBool IsPlainTextMode() const;
        
        /**
        * Returns a reference to text content of the editor.
        * @return
        */
        IMPORT_C virtual CRichText& TextContent() const;

        /**
        * Copies aInText to aOutText.
        * @param aInText
        * @param aOutText
        */
        IMPORT_C virtual void CopyDocumentContentL( CGlobalText& aInText, 
                                                    CGlobalText& aOutText );
        
        /**
        * Sets maximun number of characters.
        * @param aMaxNumberOfChars
        */
        IMPORT_C virtual void SetMaxNumberOfChars( TInt aMaxNumberOfChars );
                
        /**
        * Sets the text editor cursor’s position.
        * @param aCursorPos New cursor position.
        */
        IMPORT_C virtual void SetCursorPosL( TInt aCursorPos );
     
    public: // Functions from base classes
    
        /**
        * From CCoeControl. Sets the control as ready to be drawn. This implementation
        * should be called by the derived classes implementing their
        * own ActivateL function.
        */
        IMPORT_C void ActivateL();
        
        /**
        * From CMsgBaseControl. Performs the internal scrolling of text editor control if needed. 
        * 
        * @param aPixelsToScroll Amount of pixels to scroll.
        * @param aDirection      Scrolling direction.
        *
        * @return Amount of pixels the where scrolled. Zero value means the component cannot
        *         be scrolled to that direction anymore and view should be moved.
        */
        IMPORT_C TInt ScrollL( TInt aPixelsToScroll, TMsgScrollDirection aDirection );        
         
        /**
        * From CMsgBaseControl. Returns the current control rect.
        * @return
        */
        IMPORT_C TRect CurrentLineRect();
         
        /**
        * From CMsgBaseControl. Sets base control observer.
        * @param aBaseControlObserver
        */
        IMPORT_C void SetBaseControlObserver( MMsgBaseControlObserver& aBaseControlObserver );

         
        /**
        * From CMsgBaseControl. Handles clipboard operation.
        * @param aFunc
        */
        IMPORT_C void ClipboardL( TMsgClipboardFunc aFunc );
         
        /**
        * From CMsgBaseControl. Returns edit permission flags.
        * @return
        */
        IMPORT_C TUint32 EditPermission() const;
         
        /**
        * From CMsgBaseControl. Handles editing operation.
        * @param aFunc
        */
        IMPORT_C void EditL( TMsgEditFunc aFunc );

        /**
        * CFrom CMsgBaseControl. hecks if the cursor location is on the topmost or downmost position
        * and returns ETrue if it is.
        * @param aLocation
        * @return
        */
        IMPORT_C TBool IsCursorLocation( TMsgCursorLocation aLocation ) const;
         
        /**
        * From CMsgBaseControl. Returns approximate height of the control.
        * @return
        */
        IMPORT_C TInt VirtualHeight();

        /**
        * From CMsgBaseControl. Returns a topmost visible text position.
        * @return
        */
        IMPORT_C TInt VirtualVisibleTop();

        /**
        * From MEikEdwinSizeObserver. Handles edwin size event. Sets new size for this control and reports
        * the observer if the control editor's size has changed.
        * 
        * @param aEdwin Changed edwin.
        * @param aEventType Change event type.
        * @param aDesirableEdwinSize Desired editor size.
        * 
        * @return Returns ETrue if redraw is required.
        */
        IMPORT_C TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin,
                                              TEdwinSizeEvent aEventType,
                                              TSize aDesirableEdwinSize );
        
        /**
        * From MEikEdwinObserver. Handle edwin events.
        * @param aEdwin Edwin where the event originates.
        * @param aEventType Event type.
        */
        IMPORT_C void HandleEdwinEventL( CEikEdwin* aEdwin, TEdwinEvent aEventType );
        
    protected: // Functions from base classes
    
        /**
        * From MCoeControlObserver. Handle control event.
        * @param aControl
        * @param aEventType
        */
        IMPORT_C void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );
         
    protected:

        CMsgExpandableControlEditor* iEditor;
        TInt                         iMaxNumberOfChars;    
        TInt                         iCursorPos;
        TInt                         iEditorTop;
    };

#endif // CMSGEXPANDABLETEXTEDITORCONTROL_H

// End of File
