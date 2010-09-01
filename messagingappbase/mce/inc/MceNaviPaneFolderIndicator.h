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
* Description:   Folder navigation depth decorator for navipane
*
*/



#ifndef MCENAVIPANEOLDERINDICATOR_H
#define MCENAVIPANEOLDERINDICATOR_H

// INCLUDES

#include <coecntrl.h>
#include <AknUtils.h>
#include <msvapi.h>
#include "MceBitmapResolver.h"

// CONSTANTS
const TInt KMceNaviPaneMaxDepth = 5;

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

/**
*  This class implements a navipane decorator for displaying folder navigation
*  depth indicator
*
*/
class CMceNaviPaneFolderIndicator : public CCoeControl
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMceNaviPaneFolderIndicator* NewL( 
            TBool aPhoneMemory,
            const TInt aDepth );

        /**
        * Two-phased constructor.
        */
        static CMceNaviPaneFolderIndicator* NewLC( 
            TBool aPhoneMemory,
            const TInt aDepth );

        /**
         *  Destructor.
         */
        virtual ~CMceNaviPaneFolderIndicator();


    public:

        /**
        * Specifies how many subfolder icons are displayed.
        * Depth range is 0..n ( 0 = root, 1 = root+subfolder, etc. )
        * Depth over 4 is displayed just as tree dots at the end of navipane.       
        * @param aDepth set current depth
        */
        void SetFolderDepth( const TInt aDepth );        

        /**
        * Changes the active root        
        * @param aRoot for the root ( ETrue == phone memory, EFalse == memory card )
        */
        void ChangeRootL( TBool aPhoneMemory );

        /**
        * From CCoeControl Receives notification on resource changes
        * @param aType type of the resource change
        */
        void HandleResourceChange( TInt aType );

    protected: // Functions from base classes

        /**
        * From CCoeControl Handles the size change events.
        */
        void SizeChanged();

        /**
        * From CCoeControl Draws a control.
        * @param aRect The region of the control to be redrawn.
        */
        void Draw( const TRect& aRect ) const;
        

    private:

        /**
        * C++ default constructor.
        */
        CMceNaviPaneFolderIndicator( 
            TBool aPhoneMemory,
            const TInt aDepth );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Load folder bitmap
        */        
        void LoadFolderBitmapL();

        /**
        * Check whether phone is in Landscape mode or not
        * Since 5.0
        */ 
        TBool IsLandscapeScreenOrientation() const;

    public: 
        /**
        * Notifies CCoeControl to redraw the pane        
        */
        void ReportChange();

        
    private: // Data

        CFbsBitmap* iFolderBitmap;
        CFbsBitmap* iFolderMask;        
        TFileName           iFilename;
        HBufC* iSeparator;
        HBufC* iEndChar;
        TInt                iMaxDepth;
        CFbsBitmap* iRootBitmap;
        CFbsBitmap* iRootMask;
        TBool               iPhoneMemory; // ETrue if msg store on phone memory
        TFixedArray<TAknLayoutRect, KMceNaviPaneMaxDepth> iBitmapLayout;
        TFixedArray<TAknLayoutText, KMceNaviPaneMaxDepth + 1> iTextLayout;
        TInt iDepth;        


    };

#endif // MCENAVIPANEOLDERINDICATOR_H

// End of File
