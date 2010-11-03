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
* Description:  MsgBody implementation
*
*/



// ========== INCLUDE FILES ================================

#include <AknUtils.h>                      // for AknUtils

#include "MsgEditorCommon.h"               //
#include "MsgBody.h"                       // for CMsgBody
#include "MsgControlArray.h"               // for CMsgControlArray

#include "MsgEditorLogging.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgBody::CMsgBody
//
// Constructor.
// ---------------------------------------------------------
//
CMsgBody::CMsgBody( const TMargins& aMargins )
    : CMsgFormComponent( aMargins )
    {
    }

// ---------------------------------------------------------
// CMsgBody::~CMsgBody
//
// Destructor.
// ---------------------------------------------------------
//
CMsgBody::~CMsgBody()
    {
    }

// ---------------------------------------------------------
// CMsgBody::NewL
//
// Factory method that creates this control.
// ---------------------------------------------------------
//
CMsgBody* CMsgBody::NewL( const CCoeControl& aParent, const TMargins& aMargins )
    {
    CMsgBody* self = new ( ELeave ) CMsgBody( aMargins );

    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CMsgBody::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CMsgBody::ConstructL( const CCoeControl& aParent )
    {
    CMsgFormComponent::BaseConstructL( aParent );
    }

// ---------------------------------------------------------
// CMsgBody::SetAndGetSizeL
//
// Calculates and sets the size for the body and returns new size as reference
// aSize. If aInit == ETrue, sets also size for the controls by calling
// their SetAndGetSizeL functions.
// ---------------------------------------------------------
//
void CMsgBody::SetAndGetSizeL( TSize& aSize, TBool aInit )
    {
    aSize = MsgEditorCommons::MsgBodyPane().Size();

    TInt marginsDeltaHeight = iMargins.iTop + iMargins.iBottom;
    TInt marginsDeltaWidth = iMargins.iLeft + iMargins.iRight;
    
    TSize bodySize( aSize.iWidth, marginsDeltaHeight );
    TSize controlSize( 0, 0 );

    controlSize.iWidth = aSize.iWidth - marginsDeltaWidth;
    CMsgBaseControl* control;
    TInt controls( iControls->Count() );

    for ( TInt cc = 0; cc < controls; cc++ )
        {
        controlSize.iHeight = aSize.iHeight;
        control = (*iControls)[cc];

        if ( aInit )
            {
            control->SetAndGetSizeL( controlSize );
            }
        else
            {
            controlSize = control->Size();
            }

        bodySize.iHeight += control->DistanceFromComponentAbove() + controlSize.iHeight;

        //MEBLOGGER_WRITEF(_L("MEB: CMsgBody::SetAndGetSizeL: bodySize.iHeight %d "), bodySize.iHeight);
        }

    SetSizeWithoutNotification( bodySize );

    aSize = bodySize;
    }

// ---------------------------------------------------------
// CMsgBody::CountComponentControls
//
// Returns a number of controls.
// ---------------------------------------------------------
//
TInt CMsgBody::CountComponentControls() const
    {
    return CMsgFormComponent::CountComponentControls();
    }

// ---------------------------------------------------------
// CMsgBody::ComponentControl
//
// Returns a control of index aIndex.
// ---------------------------------------------------------
//
CCoeControl* CMsgBody::ComponentControl( TInt aIndex ) const
    {
    return CMsgFormComponent::ComponentControl( aIndex );
    }

// ---------------------------------------------------------
// CMsgBody::SizeChanged
//
// Sets new position for all the controls.
// ---------------------------------------------------------
//
void CMsgBody::SizeChanged()
    {
    MEBLOGGER_ENTERFN("CMsgBody::SizeChanged");

    CMsgFormComponent::SizeChanged();

    MEBLOGGER_LEAVEFN("CMsgBody::SizeChanged");
    }

//  End of File
