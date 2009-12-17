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
* Description:  
*       MsgEditor icon control
*
*/



#ifndef MSGICONCONTROL_H
#define MSGICONCONTROL_H

// INCLUDES

#include <coecntrl.h>
#include <AknsItemID.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CGulIcon;
class CMsgMediaControl;
class TAknsItemID;


// ==========================================================

/**
*  CMsgIconControl is a class for drawing an icon
*/
NONSHARABLE_CLASS( CMsgIconControl ) : public CCoeControl
    {

    public:  // Constructor and destructor

        /**
        * Symbian constructor
        * @param aParent parent control
        */
        static CMsgIconControl* NewL( const CCoeControl& aParent );

        /**
        * Destructor.
        */
        virtual ~CMsgIconControl();

    public:

        /**
        * Loads icon from specified file using bitmap id and mask id.
        *
        * @param aId            Item Id.
        * @param aFileName      Name of the file that includes wanted icon bitmap
        * @param aFileBitmapId  ID of the icon bitmap
        * @param aFileMaskId    ID of the icon mask. -1 (default) if no mask needed.
        */
        void LoadIconL( const TAknsItemID& aId,
                        const TDesC& aFileName,
                        const TInt aFileBitmapId,
                        const TInt aFileMaskId = -1 );

        /**
        * Sets the icon, used by svgt thumbnail
        * 
        * @param aIcon is the icon
        */
        void SetIcon( CGulIcon* aIcon );
        
        /**
        * Sets size to the icon
        * 
        * @param aSize New size.
        */
        void SetBitmapSizeL( const TSize& aSize );
        
        /**
        * Size of the bitmap owned by icon control
        * @return size
        */
        TSize BitmapSize();
        
        /**
        * Returns the currently loaded icon's bitmap id.
        */
        TInt IconBitmapId() const;
    
    public: // Functions from base classes
    
        /**
        * From CCoeControl. Handles resource change events.
        */
        void HandleResourceChange( TInt aType );
    
    protected: // from CCoeControl

        /**
        * From CCoeControl Draws control
        */
        void Draw( const TRect& aRect ) const;
    
    private: // Constructors

        /**
        * C++ default constructor.
        */
        CMsgIconControl();
        
        /**
        * By default Symbian constructor is private.
        */
        void ConstructL( const CCoeControl& aParent );

    private: //Data

        CGulIcon*               iIcon;
        const CMsgMediaControl*    iParent;
        
        TAknsItemID     iSkinId;
        HBufC*          iFileName;
        TInt            iFileBitmapId;
        TInt            iFileMaskId;
    };


#endif // MSGICONCONTROL_H
