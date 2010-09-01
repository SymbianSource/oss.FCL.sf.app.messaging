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
* Description:  
*     Container for CRichbio.
*
*/



#ifndef CMSGDETAILWINDOW_H
#define CMSGDETAILWINDOW_H

// INCLUDES

#include <coecntrl.h>

class CRichBio;

// CLASS DECLARATION

/**
* This class is a window owning container, which is used to hold 
* richbio control.
*/
class CMsgDetailWindow : public CCoeControl
    {

    public: // Construction and destruction

        /**
        * Create a new detailwindow.
        * @return a pointer to the new object.
        */
        IMPORT_C static CMsgDetailWindow* NewL();
        
        /**
        * Destructor.
        * Deletes the child control.
        */
        IMPORT_C ~CMsgDetailWindow();

    public: // New

        /**
        * Set the control to be the detail window child.
        * @param aRichBio pointer to the richbio, ownership is changed.
        */
        IMPORT_C void SetControlL( CRichBio* aRichBio );
        
        /**
        * Get the reference to the owned control.
        * @return a reference to the richbio owned by detail window.
        */
        IMPORT_C CRichBio& Control() const;
  
        /**
        * Show the detail window.
        */
        IMPORT_C void Show();

        /**
        * Hide the detail window.
        */
        IMPORT_C void Hide();
    
    public: // from CCoeControl

        /**
        * Get the subcontrol count.
        * @return Count of controls in this component.
        */
        TInt CountComponentControls() const;

        /**
        * Get the desired child control.
        * @return pointer to component in question.  
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
        /**
        * Called when the control size is changed.
        */
        void SizeChanged();

        /**
        * Handle key event.
        * @param aKeyEvent event.
        * @param aType key event type.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
            TEventCode aType );

    protected: // from CCoeControl

        /**
        * Called when the focus has changed.
        * @param aDrawNow indicates a redraw.
        */
        void FocusChanged(TDrawNow aDrawNow);

    private:

        /**
        * Constructor.
        */
        CMsgDetailWindow();
        
        /**
        * Do the construction.
        */
        void ConstructL();
        
        /**
        * Clears the window area.
        */
        void Draw(const TRect& aRect) const;

    private:
    
        /// Pointer to the richbio.
        CRichBio* iRichBio;          
    };

#endif // CMSGDETAILWINDOW_H

// End of file
