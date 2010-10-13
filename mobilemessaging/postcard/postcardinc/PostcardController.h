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
* Description: PostcardController  declaration
*
*/


#ifndef POSTCARDCONTROLLER
#define POSTCARDCONTROLLER

#include <e32base.h>
#include <coecntrl.h>
#include <AknsItemID.h>
#include "PostcardDocument.h"
#ifdef RD_SCALABLE_UI_V2
#include "PostcardPointerObserver.h"
#endif

// FOWARD DECLARTIONS
class CGulIcon;
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATION
/**
*  Container control class.
*  
*/
class CPostcardController : public CCoeControl
    {
    public: // Constructors and destructor
        /**
        * Constructor.
        * @return pointer to the object
        */
        static CPostcardController* NewL( CPostcardDocument& aDocument
#ifdef RD_SCALABLE_UI_V2
            , MPocaPointerEventObserver& aObserver
#endif
        );

        /**
        * Destructor.
        */
        ~CPostcardController();

    public: // New functions

        /**
        * Returns ETrue if front page is focused.
        */
        TBool Frontpage( );

        /**
        * Returns ETrue if aPart is focused
        */
        TBool IsFocused( TPostcardPart& aPart );

        /**
        * Returns the focused part
        */
        TPostcardPart Focused( );

        /**
        * Sets aPart as focused item
        */
        void SetFocused( TPostcardPart aPart );

        /**
        * Sets the bitmap of part aPart. Takes the ownership.
        * Deletes the previous one.
        */
        void SetBitmap( TPostcardPart aPart, CGulIcon* aIcon );

        /**
        * Removes and deletes bitmap aPart
        */
        void RemoveBitmap( TPostcardPart aPart );

        /**
         * Reloads icons. Called from AppUi, if skin has changed.
        */ 
        void ReLoadIconsL();
        
		/**
		* Rereads the coordinates of bitmaps from LAF
		*/
		void RefreshCoordinates( );

        /**
        * Calls MoveHorizontally or MoveVertically according to aKeyEvent
        */
        void Move( TInt aScanCode );

        /**
        * Changes the focus in the back side if possible
        */
        void MoveHorizontally( TBool aRight );

        /**
        * Changes the focus from back to front or vice versa if possible
        */
        void MoveVertically( TBool aUp );

        /**
        * Makes itself focused, visible and active.
        */
        void ActivateL( );

        /**
        * Gets an array of TRects and sets them as greeting field lines
        */
        void SetGreetingLines( CArrayPtrFlat<TRect>& aGreetingLines );
        
        /**
        * Gets an array of TRects and sets them as recipient field lines
        */
        void SetRecipientLines( CArrayPtrFlat<TRect>& aRecipientLines );

    private: // New functions

        /**
        * Default C++ constructor.
        */
        CPostcardController( CPostcardDocument& aDocument
#ifdef RD_SCALABLE_UI_V2
            , MPocaPointerEventObserver& aObserver
#endif
        );

        /**
        * 2nd phase Constructor.
        */
        void ConstructL( );

    private: // Functions from base classes

        /**
        * From CoeControl,CountComponentControls. Returns always 0.
        */
        TInt CountComponentControls( ) const;

        /**
        * From CCoeControl,ComponentControl. Returns always NULL.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * Calls DrawBackground.
        * Also calls DrawIcons for the objects we have on top of the background
        */
        void Draw( const TRect& aRect ) const;
        
        /**
        * Draws possible skin background.
        * Draws the background icons.
        */
        void DrawBackground( ) const;
        
        /**
        * Draws the image side.
        */
        void DrawImageSide( ) const;

        /**
        * Draws the text side.
        */
        void DrawTextSide( ) const;
        
        /**
        * Draws icon aIcon in rect aRect. Called by DrawImageSide or DrawTextSide.
        */
        void DrawIcon( const CGulIcon& aIcon, const TRect& aRect ) const;

        /**
        * Draws arrow down or up icon based on the side and whether in editor or
        * in viewer mode.
        */
        void DrawScrollArrows() const;

        /**
        * Draws focus lines. Called by DrawTextSide.
        */
        void DrawFocus( ) const;

    protected: // From CCoeControl

        /**
        * Returns the Mop supply object for skin drawing
        */
	    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

#ifdef RD_SCALABLE_UI_V2
        /**
        * Handle pointer event
        */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif

    private:

        /**
        *   LoadIconsL
        *   Calls DoLoadIconL to load icons needed by controller
        */
        void LoadIconsL();

        /**
        *   DoLoadIconL
        *   Calls AknsUtils::CreateIconL to load icons defined by parameters.
        *   Param IN aId - the id of the icon in AknsConstants.h
        *            aFileName - the name of the file where icons are loaded
        *            aFileBitmapId - the id of the bitmap file
        *   Returns - icon created by the function
        */
        CGulIcon* DoLoadIconL(  const TAknsItemID& aId,
                                const TDesC& aFileName,
                                const TInt aFileBitmapId,
                                const TInt aFileMaskId = -1 );
        
        

        /**
        *   DoLoadIconforUpandDownarrowL
        *   Calls AknsUtils::CreateColorIconLC to load icons defined by parameters.
        *   Param IN aId - the id of the icon in AknsConstants.h
        *            aFileName - the name of the file where icons are loaded
        *            aFileBitmapId - the id of the bitmap file
        *   Returns - icon created by the function
        */
        CGulIcon* DoLoadIconforUpandDownarrowL(  const TAknsItemID& aId,
                                   const TDesC& aFileName,
                                   const TInt aFileBitmapId,
                                   const TInt aFileMaskId = -1 );
        
        
                               
    private: //Data

        // Reference to AppUi
        CPostcardDocument& iDocument;

		// Related to frontside
        CGulIcon* iFrontBg;
        CGulIcon* iImage; 
        CGulIcon* iInsertImage;
        CGulIcon* iInsertImageBg;

		// Related to backside
        CGulIcon* iBgBg;
        CGulIcon* iStamp;
        CGulIcon* iEmptyGreetingFocused;
        CGulIcon* iEmptyGreeting;
        CGulIcon* iGreeting;
        CGulIcon* iEmptyAddressFocused;
        CGulIcon* iEmptyAddress;
        CGulIcon* iAddress;

        // Focus lines are here
        CArrayPtrFlat<TRect>*   iGreetingLines;
        CArrayPtrFlat<TRect>*   iRecipientLines;

		// Related to arrows
        CGulIcon* iUpperArrow;
        CGulIcon* iLowerArrow;

		// Related to frontside
        TRect iFrontBgC;
        TRect iImageC;
        TRect iInsertImageC;
        TRect iInsertImageBgC;

		// Related to backside
        TRect iBgBgC;
        TRect iStampC;
        TRect iGreetingC;
        TRect iAddressC;

		// Related to arrows
        TRect iUpperArrowC;
        TRect iLowerArrowC;

        TPostcardPart iFocusedItem;

    	CAknsBasicBackgroundControlContext* iBgContext; // Skin background control context

#ifdef RD_SCALABLE_UI_V2
        TBool iPenEnabled;
        MPocaPointerEventObserver& iEventObserver;
#endif
    };

#endif // POSTCARDCONTROLLER_H

// End of File
