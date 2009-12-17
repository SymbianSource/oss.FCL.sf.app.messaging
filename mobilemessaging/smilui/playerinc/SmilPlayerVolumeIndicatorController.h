/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     Volume indicator controller class
*
*/



#ifndef SMILPLAYERVOLUMEINDICATORCONTROLLER_H
#define SMILPLAYERVOLUMEINDICATORCONTROLLER_H

//  INCLUDES
#include <coecntrl.h>
#include <coecobs.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CEikImage;
class TAknsItemID;
class CRepository;
class TAknLayoutRect;
class CAknVolumePopup;

// DATA TYPES

// CLASS DECLARATION

/**
* Controller class of volume indicator,
*
* @lib smilplayer.lib
* @since 3.0
*/
NONSHARABLE_CLASS(CSmilPlayerVolumeIndicatorController) : public CCoeControl,
                                                          public MCoeControlObserver
    {
    public: // Constructors and destructor
    
        /**
        * Two-phased constructor.
        *
        * @param aParent        A refence to the parent class.
        *
        * @return pointer to created CSmilPlayerVolumeIndicatorController
        */
        static CSmilPlayerVolumeIndicatorController* NewL( const CCoeControl* aParent );

        /**
        * Destructor.
        */
        virtual ~CSmilPlayerVolumeIndicatorController();

    public: // new functions

        /**
        * Returns current volume value
        *
        * @since 3.0
        *
        * @return Current volume level.
        */
        TInt VolumeValue() const;
                
    public: // Functions from base classes
        
        /**
        *  From CCoeControl
        *  See the documentation from the coecntrl.h
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType);
        
        /** 
        * From MCoeControlObserver 
        * See the documentation from the coecobs.h
        */
	    void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );
        
    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const CCoeControl* aParent );  

        /**
        * C++ constructor.
        */
        CSmilPlayerVolumeIndicatorController();
                
        /**
        * Returns the initial volume level.
        */
        TInt InitialVolumeLevelL() const;
        
        /**
        * Stores volume level to central repository.        
        */
        void StoreAudioVolume();
        
    private: // New functions

    private:
        
        // Used for storing volume level
		CRepository* iRepository;
		        
        // Stores the initial volume level
        TInt iInitialVolumeLevel;
        
        // Volume popup
        CAknVolumePopup* iVolumePopup;
    };

#endif  // SMILPLAYERVOLUMEINDICATORCONTROLLER_H
            
// End of File
