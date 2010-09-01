/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SmilPlayerPresentationController.inl*
*/



#include "SmilPlayerPresentationControl.h"
#include "SmilPlayerMediaFactory.h"
#include <smiltransitionfactory.h>

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::BufferBitmap
// ----------------------------------------------------------------------------
inline CFbsBitmap* CSmilPlayerPresentationController::BufferBitmap()
    {
    return iBufferBitmap;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::BufferContext
// ----------------------------------------------------------------------------
inline CFbsBitGc* CSmilPlayerPresentationController::BufferContext()
    {
    return iBufferContext;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::Presentation
// ----------------------------------------------------------------------------
inline CSmilPresentation* CSmilPlayerPresentationController::Presentation()
    {
    return iPresentation;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::ScrollingDeactivationEnabled
// ----------------------------------------------------------------------------
inline TBool CSmilPlayerPresentationController::ScrollingDeactivationEnabled() const
    {
    #ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() )
    return EFalse;
    #endif
    return iPresentation->MediaSelected();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::OpenLinkL
// ----------------------------------------------------------------------------
inline void CSmilPlayerPresentationController::OpenLinkL()
    {
    iPresentation->ActivateFocusedL();
    }
    
// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::GetMediaFactory
// Returns media factory (implementation from SMIL Engine).
// ----------------------------------------------------------------------------
inline MSmilMediaFactory* CSmilPlayerPresentationController::GetMediaFactory(
                            const CSmilPresentation* /*aPres*/) const
    {
    return iMediaFactory;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::GetTransitionFactory
// Returns transition effect factory (implementation from SMIL Engine).
// ----------------------------------------------------------------------------
//
inline MSmilTransitionFactory* CSmilPlayerPresentationController::GetTransitionFactory( 
                             const CSmilPresentation* /*aPres*/ ) const
    {
    return iTransitionFactory;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::EvaluateContentControlAttribute
// ----------------------------------------------------------------------------
inline TBool CSmilPlayerPresentationController::EvaluateContentControlAttribute(
                            const TDesC& /*aName*/, const TDesC& /*aValue*/)
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::GetDimensions
// ----------------------------------------------------------------------------
inline TRect CSmilPlayerPresentationController::GetDimensions(
                            const CSmilPresentation* /*aPres*/ )
    {
    return iPresentationControl->Rect();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::GetControl
// ----------------------------------------------------------------------------
inline CCoeControl* CSmilPlayerPresentationController::GetControl(
                            const CSmilPresentation* /*aPres*/ )
    {
    return iPresentationControl;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::Redraw
// ----------------------------------------------------------------------------
inline void CSmilPlayerPresentationController::Redraw(
                            TRect aArea, CSmilPresentation* /*aPres*/ )
    {
    RedrawArea(aArea);
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::ObjectFocused
// ----------------------------------------------------------------------------
inline TBool CSmilPlayerPresentationController::ObjectFocused() const
    {
    return ( iPresentation->FocusedObject() != NULL );
    }
