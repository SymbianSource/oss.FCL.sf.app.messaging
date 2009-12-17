/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SmilPlayerIndicatorController  declaration
*
*/


#ifndef SMILPLAYERINDICATORCONTROLLER_H
#define SMILPLAYERINDICATORCONTROLLER_H

// INCLUDES
#include <coecntrl.h>
#include <coecobs.h>
#include "SmilPlayerTimeObserver.h"
#include "SmilPlayerPauseObserver.h"

// FORWARD DECLARATIONS
class CSmilPlayerPresentationController;
class CSmilPlayerVolumeIndicatorController;
class CSmilPlayerTimeIndicatorTimer;
class CSmilPlayerPauseIndicatorTimer;
class CEikLabel;
class CEikImage;
class CCoeBrushAndPenContext;
class TAknsItemID;

// CLASS DECLARATION
/**
*  Controller class of the indicators.
*/
NONSHARABLE_CLASS(CSmilPlayerIndicatorController) : public CCoeControl,
                                                    public MCoeForegroundObserver,
                                                    public MCoeControlObserver,
                                                    public MSmilPlayerTimeObserver,
                                                    public MSmilPlayerPauseObserver
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        *
        * @param aParent         Parent control.
        * @param aPresController Presentation controller.
        * @param aVolumeEnabled  Specifies whether volume is enabled.
        *
        * @return pointer to the object
        */
        static CSmilPlayerIndicatorController * NewL( const CCoeControl* aParent, 
                                                      CSmilPlayerPresentationController* aPresController,
                                                      TBool aVolumeEnabled );

        /**
        * Destructor.
        */
        virtual ~CSmilPlayerIndicatorController();

    public: // Functions from base classes

        /**
        * From CoeControl
        * See the documentation from the coecntrl.h
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        * See the documentation from the coecntrl.h
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl
        * See the documentation from the coecntrl.h
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );
                                  
        /**
        * From MSmilPlayerTimeObserver
        * See the documentation from the SmilPlayerTimeObserver.h
        */
        void TimeChangedL( const TDesC& aTimeString );
        
        /**
        * From MSmilPlayerTimeObserver
        * See the documentation from the SmilPlayerTimeObserver.h
        */
        TInt CurrentTime() const;
        
        /**
        * From MSmilPlayerTimeObserver
        * See the documentation from the SmilPlayerTimeObserver.h
        */
        TInt PresentationDuration() const;
        
        /**
        * From MSmilPlayerTimeObserver
        * See the documentation from the SmilPlayerTimeObserver.h
        */
        TBool IsDurationFinite() const;
        
        /**
        * From MSmilPlayerPauseObserver
        * See the documentation from the SmilPlayerPauseObserver.h
        */
        void TogglePauseIndicator() const;
        
        /**
        * From MCoeForegroundObserver 
        * See the documentation from the coemain.h
        */   
        void HandleGainingForeground();

        /**
        * From MCoeForegroundObserver 
        * See the documentation from the coemain.h
        */   
        void HandleLosingForeground();
        
        /** 
        * From MCoeControlObserver 
        * See the documentation from the coecobs.h
        */
	    void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );
        
        /** 
        * From CCoeControl 
        * See the documentation from the coecntrl.h
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
        /** 
        * From CCoeControl 
        * See the documentation from the coecntrl.h
        */
        void HandleResourceChange( TInt aType );
        
    public: // new functions
        
        /**
        * Performs approriate tasks when presentation has been stopped.
        *
        * @since 3.0
        */
        void Stop();
        
        /**
        * Performs approriate tasks when presentation has been started.
        *
        * @since 3.0
        */
        void Start();
        
        /**
        * Performs approriate tasks when presentation has been resumed.
        *
        * @since 3.0
        */
        void Resume();
        
        /**
        * Performs approriate tasks when presentation has been paused.
        *
        * @since 3.0
        */
        void Pause();
        
        /**
        * Performs approriate tasks when presentation end has been reached.
        *
        * @since 3.0
        */
        void EndReached();

        /**
        * Returns current volume value.
        *
        * @since 3.0
        *
        * @return Current volume level.
        */
        TInt VolumeValue() const;
        
        /**
        * Sets correct layout for indicator.
        *
        * @since 3.0
        */
        void LayoutIndicators();
        
    private:

        /**
        * C++ constructor.
        */
        CSmilPlayerIndicatorController ( CSmilPlayerPresentationController* aPresController );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const CCoeControl* aParent, 
                         TBool aVolumeEnabled );
        
        /**
        * Initializes indicator.
        */
        void InitializeIconIndicatorL( CEikImage*& aIndicator, 
                                      const TAknsItemID& aItem, 
                                      const TInt aBitmapIndex,
                                      const TInt aMaskIndex ) const;
                                      
        /**
        * Sets pause indicator blinking on/off.
        */
        void SetPauseIndicatorBlinking( TBool aValue );
        
        /**
        * Handles skin change event.
        */
        void DoHandleSkinChangeL();
        
        /**
        * Updates the text color from currently used skin.
        */
        void UpdateTextColorL();
        
        /**
        * Sets correct bitmap for icon indicator.
        */
        void SetIconIndicatorBitmapL( CEikImage*& aIndicator, 
                                      const TAknsItemID& aItem, 
                                      const TInt aBitmapIndex,
                                      const TInt aMaskIndex ) const;
        
        /**
        * Sets specified icons extent to given one.
        */
        void SetIconIndicatorExtent( CEikImage* aIconIndicator,
                                     TRect aExtent ) const;
        
    private: // data

        /** Controller class for this class */
        CSmilPlayerPresentationController* iPresController;
        
        /** Controller class for volume indicator */
        CSmilPlayerVolumeIndicatorController* iVolumeController;
        
        /** Model class for time control. */
        CSmilPlayerTimeIndicatorTimer* iTimeModel;
        
        /** View class for time control*/
        CEikLabel* iTimeIndicator;
        
        /** View class for pause indicator. */
        CEikImage* iPauseIndicator;
        
        /** View class for play indicator. */
        CEikImage* iPlayIndicator;
        
        /** Timer for pause indicator. */
        CSmilPlayerPauseIndicatorTimer* iPauseTimer;
        
        /** Specifies whether time indicator should be shown. */
        TBool iShowTimeIndicator;
    };

#endif // SMILPLAYERINDICATORCONTROLLER_H

// End of File
