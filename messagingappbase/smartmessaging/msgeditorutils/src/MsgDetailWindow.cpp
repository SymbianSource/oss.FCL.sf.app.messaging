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



// INCLUDE FILES

#include "MsgDetailWindow.h"
#include <CRichBio.h>
#include "MsgEditorUtils.pan"           // for MsgEditorUtils panics

// LOCAL CONSTANTS AND MACROS

// Define the richbio size and position according to LAF.
const TInt KRichBioTopLeftX = 0;
const TInt KRichBioTopLeftY = 8;
const TInt KRichBioWidth = 174;
const TInt KRichBioHeight = 125;

// These define detail/overview position and dimensions.
// Detail window must cover the whole display area so that 
// nothing "shines" through.
const TInt KWindowTopLeftX = 1;
const TInt KWindowTopLeftY = 44;
const TInt KWindowHeight = 144;
const TInt KWindowWidth = 174;

// Define the position where window is moved when Hide()-method
// is called.
const TInt KWindowPosOutOfTheWayX = 1000;
const TInt KWindowPosOutOfTheWayY = 1000;

EXPORT_C CMsgDetailWindow* CMsgDetailWindow::NewL()
    {
    CMsgDetailWindow* self = new ( ELeave ) CMsgDetailWindow();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CMsgDetailWindow::~CMsgDetailWindow()
    {
    delete iRichBio;
    }

EXPORT_C void CMsgDetailWindow::SetControlL(CRichBio* aRichBio)
    {
    // Control must not be set twice. Get a reference to the control with
    // Control()-method instead and update it.
    __ASSERT_DEBUG(iRichBio==NULL, Panic( EMEUControlAlreadySet ));
    __ASSERT_DEBUG(aRichBio!=NULL, Panic( EMEUControlNotSet ));

    aRichBio->SetContainerWindowL(*this);
    
    // Set the correct size and position.
    TSize richBioSize(KRichBioWidth, KRichBioHeight);
    TPoint richBioPos(KRichBioTopLeftX, KRichBioTopLeftY);
    aRichBio->SetAndGetSizeL( richBioSize );
    aRichBio->SetSize( richBioSize );
    aRichBio->SetPosition( richBioPos );    
    ActivateL();
    iRichBio = aRichBio;
    }

EXPORT_C CRichBio& CMsgDetailWindow::Control() const
    {
    __ASSERT_DEBUG(iRichBio, Panic( EMEUControlNotSet ));
    return *iRichBio;
    }

EXPORT_C void CMsgDetailWindow::Show()
    {
    TSize size(KWindowWidth, KWindowHeight);
    TPoint windowPos( KWindowTopLeftX, KWindowTopLeftY );
    SetSize(size);
    SetPosition( windowPos );
    }

EXPORT_C void CMsgDetailWindow::Hide()
    {
    TPoint outOfTheWay( KWindowPosOutOfTheWayX, KWindowPosOutOfTheWayY );
    TSize size(KWindowWidth, KWindowHeight);
    SetSize(size);
    SetPosition( outOfTheWay );
    }

TInt CMsgDetailWindow::CountComponentControls() const
    {
    if ( iRichBio )
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

CCoeControl* CMsgDetailWindow::ComponentControl( TInt /*aIndex*/ ) const
    {
    return iRichBio;
    }

void CMsgDetailWindow::SizeChanged()
    {
    if ( iRichBio )
        {
        TSize size(KRichBioWidth, KRichBioHeight);
        // Trap this here. If this leaves just set the 
        // defined size.
        TRAP_IGNORE( iRichBio->SetAndGetSizeL( size ) );
        iRichBio->SetSize( size );
        }
    }

TKeyResponse CMsgDetailWindow::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, 
    TEventCode aType ) 
    {
    if ( iRichBio )
        {
        TKeyResponse response = iRichBio->OfferKeyEventL( aKeyEvent, aType );
        return response;
        }
    else
        {
        return EKeyWasNotConsumed;
        }
    }

void CMsgDetailWindow::FocusChanged(TDrawNow /*aDrawNow*/) 
    {
    if ( iRichBio )
        {
        TBool isFocused = IsFocused();
        iRichBio->SetFocus( isFocused );
        }
    }

CMsgDetailWindow::CMsgDetailWindow()
    {      
    }

void CMsgDetailWindow::ConstructL()
    {
    CreateWindowL();
    TSize windowSize(KWindowWidth, KWindowHeight);
    SetSizeWithoutNotification( windowSize );
    Hide();
    }

void CMsgDetailWindow::Draw(const TRect& /*aRect*/) const
    {
    // Clear the window background.
    CWindowGc& gc = SystemGc();
    gc.Clear(Rect());
    }

//  End of File
