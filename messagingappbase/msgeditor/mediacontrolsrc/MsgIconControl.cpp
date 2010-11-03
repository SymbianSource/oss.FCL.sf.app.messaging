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
*       MsgEditor icon control
*
*/



// INCLUDE FILES
#include "MsgIconControl.h"

#include <gulicon.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknBitmapAnimation.h>
#include <aknlayoutscalable_apps.cdl.h>

// ==========================================================

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgIconControl::CMsgIconControl
//
// Constructor
// ---------------------------------------------------------
//
CMsgIconControl::CMsgIconControl() :
    iFileBitmapId( KErrNotFound ),
    iFileMaskId( KErrNotFound )
    {
    }

// ---------------------------------------------------------
// CMsgIconControl::ConstructL
//
// Constructor
// ---------------------------------------------------------
//
void CMsgIconControl::ConstructL( const CCoeControl& aParent )
    {
    SetContainerWindowL( aParent );
    MakeVisible( EFalse );
    }

// ---------------------------------------------------------
// CMsgIconControl::NewL
//
// Constructor
// ---------------------------------------------------------
//
CMsgIconControl* CMsgIconControl::NewL( const CCoeControl& aParent )
    {
    CMsgIconControl* self = new( ELeave ) CMsgIconControl;
    
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CMsgIconControl::NewL
//
// Destructor
// ---------------------------------------------------------
//
CMsgIconControl::~CMsgIconControl()
    {
    delete iIcon;
    delete iFileName;
    }

// ---------------------------------------------------------
// CMsgIconControl::SetIcon
// ---------------------------------------------------------
//
void CMsgIconControl::SetIcon( CGulIcon* aIcon )
    {
    delete iIcon;
    iIcon = aIcon; 
    
    if ( iIcon == NULL )
        {
        iFileBitmapId = KErrNotFound;
        iFileMaskId = KErrNotFound;
        }
    }
    
// ---------------------------------------------------------
// CMsgIconControl::LoadIconL
// ---------------------------------------------------------
//
void CMsgIconControl::LoadIconL( const TAknsItemID& aId,
                                 const TDesC& aFileName,
                                 const TInt aFileBitmapId,
                                 const TInt aFileMaskId )
    {
    delete iIcon;
    iIcon = NULL;
    
    iIcon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                                       aId,
                                       aFileName,
                                       aFileBitmapId,
                                       aFileMaskId );
    
    if ( aFileBitmapId != iFileBitmapId )
        {
        delete iFileName;
        iFileName = NULL;
    
        iFileName = aFileName.AllocL();
    
        iSkinId = aId;
    
        iFileBitmapId = aFileBitmapId;
        iFileMaskId = aFileMaskId;
        
        // New bitmap. Force size setting. Not propably needed by kept here
        // so that functionality would be the same.
        SetSize( TSize() );
        }
    else
        {
        // Icon reloaded. Set old size.
        SetBitmapSizeL( iSize );
        }
    }



// ---------------------------------------------------------
// CMsgIconControl::BitmapSize
//
// Return the size of the icon
// ---------------------------------------------------------
//
void CMsgIconControl::SetBitmapSizeL( const TSize& aSize )
    {    
    if ( iIcon )
        {
        User::LeaveIfError( AknIconUtils::SetSize( iIcon->Bitmap(), aSize ) );
        SetSize( aSize );
        }
    }

// ---------------------------------------------------------
// CMsgIconControl::BitmapSize
//
// Return the size of the icon
// ---------------------------------------------------------
//
TSize CMsgIconControl::BitmapSize()
    {
    TSize size( 0, 0 );
    if ( iIcon )
        {
        size = iIcon->Bitmap()->SizeInPixels();
        }
    return size;
    }
    
// ---------------------------------------------------------
// CMsgIconControl::IconBitmapId
// ---------------------------------------------------------
//
TInt CMsgIconControl::IconBitmapId() const
    {
    return iFileBitmapId;
    }

// ---------------------------------------------------------
// CMsgIconControl::IconBitmapId
// ---------------------------------------------------------
//
void CMsgIconControl::HandleResourceChange( TInt aType )
    {
    if ( aType == KAknsMessageSkinChange )
        {
        // Reload the current icon.
        TRAP_IGNORE( LoadIconL( iSkinId,
                                *iFileName,
                                iFileBitmapId,
                                iFileMaskId ) );
        }
    }

// ---------------------------------------------------------
// CMsgIconControl::Draw
//
// Draw the icon
// ---------------------------------------------------------
//
void CMsgIconControl::Draw( const TRect& aRect ) const
    {
    if ( iIcon && iIcon->Bitmap() )
        {
        //Intersection of the drawing rect and icon rect
        TRect i = Rect();
        i.Intersection( aRect );
        
        //Top left corner of the intersection
        TPoint tL2 = i.iTl;

        //The relative part of the icon rect to be drawn
        i.Move( -Position().iX, -Position().iY );

        if ( iIcon->Mask() )
            {
            SystemGc().BitBltMasked( 
                tL2, 
                iIcon->Bitmap(), 
                i, 
                iIcon->Mask(), 
                ETrue );
            }
        else
            {
            SystemGc().BitBlt( 
                tL2, 
                iIcon->Bitmap(), 
                i );
            }
        }
    }

//  End of File
