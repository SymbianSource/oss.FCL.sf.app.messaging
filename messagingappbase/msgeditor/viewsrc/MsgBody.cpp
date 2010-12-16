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
#include <AknsUtils.h>
#include <gulicon.h>
#include <AknIconUtils.h>
#include <msgeditor.mbg>

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
    delete iBoundary;
    iBoundary = NULL;
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
    SkinChanged();
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
    TBool fontChanged = (aSize.iWidth==0);

    aSize = MsgEditorCommons::MsgBodyPane().Size();

    TInt marginsDeltaHeight = iMargins.iTop + iMargins.iBottom;
    TInt marginsDeltaWidth = iMargins.iLeft + iMargins.iRight;
    
    TSize bodySize( aSize.iWidth, marginsDeltaHeight );

    TSize controlSize( 0, 0 );
    controlSize.iWidth = aSize.iWidth - marginsDeltaWidth;

    TInt controls( iControls->Count() );
    for ( TInt cc = 0; cc < controls; cc++ )
        {
        controlSize.iHeight = aSize.iHeight;
        CMsgBaseControl* control = (*iControls)[cc];

        if(control->IsReadOnly())
            {
            TSize currentSize = control->Size();
            TBool widthChanged = (currentSize.iWidth!=controlSize.iWidth);
            
            if ( aInit && (widthChanged || fontChanged) ) // Just for needed controls.
                {
                control->SetAndGetSizeL( controlSize );
                }
            else
                {
                controlSize = control->Size();
                }
            }
        else
            {
            if ( aInit )
                {
                control->SetAndGetSizeL( controlSize );
                }
            else
                {
                controlSize = control->Size();
                }
            }

        bodySize.iHeight += control->DistanceFromComponentAbove() + controlSize.iHeight;

        //MEBLOGGER_WRITEF(_L("MEB: CMsgBody::SetAndGetSizeL: bodySize.iHeight %d "), bodySize.iHeight);
        }

    SetSizeWithoutNotification( bodySize );
    
    // resize boundary
    TSize size = TSize(Size().iWidth, 2);
    AknIconUtils::SetSize(iBoundary->Bitmap(), size, EAspectRatioNotPreserved);

    aSize = bodySize;
    }

// ---------------------------------------------------------
// CMsgBody::SetBodyIndex
//
// Set body index.
// ---------------------------------------------------------
//
void CMsgBody::SetBodyIndex(TInt aIndex)
    {
    iIndex = aIndex;
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

// ---------------------------------------------------------
// CMsgBody::Draw
//
// Draw from CCoeControl.
// ---------------------------------------------------------
//
void CMsgBody::Draw(const TRect& aRect) const
    {
    // Index will be always 0 when it is editing.
    // Boundary will be needed when it is read only and not the first slide.
    // Boundary will be drawn at the top of the body
    if(iIndex > 0)
        {
        CWindowGc& gc = SystemGc();
        
        TPoint pos = Position()+TPoint(0,iMargins.iTop/2);
        TSize size = TSize(Size().iWidth, 1);
        gc.DrawBitmapMasked(TRect(pos, size),
                            iBoundary->Bitmap(), 
                            TRect(size), 
                            iBoundary->Mask(),
                            ETrue);
        
        CMsgFormComponent::Draw(aRect);
        }
    }

// ---------------------------------------------------------
// CMsgBody::HandleResourceChange
//
// ---------------------------------------------------------
//
void CMsgBody::HandleResourceChange( TInt aType )
    {
    CMsgFormComponent::HandleResourceChange( aType );
    
    switch( aType )
        {
        case KAknsMessageSkinChange:
            SkinChanged();
            break;

        case KEikDynamicLayoutVariantSwitch:
            iSize.iWidth++; // for point out this control's size has changed
            break;

        default:
            break;
        }
    }


void CMsgBody::SkinChanged()
    {    
    delete iBoundary;
    iBoundary = NULL;
    TRAP_IGNORE(iBoundary = AknsUtils::CreateGulIconL(AknsUtils::SkinInstance(),
                                                      KAknsIIDQgnGrafLineMessageHorizontal,
                                                      KMsgEditorMbm,
                                                      EMbmMsgeditorQgn_graf_line_message_horizontal,
                                                      EMbmMsgeditorQgn_graf_line_message_horizontal_mask));
    }

//  End of File
