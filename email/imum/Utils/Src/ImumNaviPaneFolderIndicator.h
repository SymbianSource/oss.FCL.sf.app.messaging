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
* Description:  Folder navigation depth decorator for navipane
*
*/


#ifndef IMUMNAVIPANEOLDERINDICATOR_H
#define IMUMNAVIPANEOLDERINDICATOR_H

// INCLUDES

#include <coecntrl.h>
#include <AknUtils.h>
//#include <MSVAPI.H>


// CONSTANTS
const TInt KImumNaviPaneMaxDepth = 5;

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

/**
*  This class implements a navipane decorator for displaying folder navigation
*  depth indicator
*
*/
NONSHARABLE_CLASS( CImumNaviPaneFolderIndicator ) : public CCoeControl
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CImumNaviPaneFolderIndicator* NewL(
            TBool aPhoneMemory,
            const TInt aDepth );

        /**
        * Two-phased constructor.
        */
        static CImumNaviPaneFolderIndicator* NewLC(
            TBool aPhoneMemory,
            const TInt aDepth );

        /**
         *  Destructor.
         */
        virtual ~CImumNaviPaneFolderIndicator();


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
        CImumNaviPaneFolderIndicator(
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
        TFixedArray<TAknLayoutRect, KImumNaviPaneMaxDepth> iBitmapLayout;
        TFixedArray<TAknLayoutText, KImumNaviPaneMaxDepth + 1> iTextLayout;
        TInt iDepth;

    };

#endif // IMUMNAVIPANEOLDERINDICATOR_H

// End of File
