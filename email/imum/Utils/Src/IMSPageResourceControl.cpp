/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IMSPageResourceControl.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <cstack.h>                     // CStack

#include "IMSPageResourceControl.h"
#include "ImumUtilsLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
enum TIMSResourcePagePanicCode
    {
    EIRPPNoError = 0,
    EIRPPWrongType,
    EIRPPStackEmpty,
    EIRPPStackNull,
    EIRPPArrayNull,
    EIRPPUnknownType
    };

_LIT( KIMSResourcePagePanic, "IMSResourcePagePanic" );

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
void Panic( const TIMSResourcePagePanicCode aReason );

// FORWARD DECLARATIONS

// ============================ LOCAL FUNCTIONS ===============================

void Panic( const TIMSResourcePagePanicCode aReason )
    {
    User::Panic( KIMSResourcePagePanic, aReason );
    }

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIMSPageResourceControl::CIMSPageResourceControl()
// ----------------------------------------------------------------------------
//
CIMSPageResourceControl::CIMSPageResourceControl()
    {
    IMUM_CONTEXT( CIMSPageResourceControl::CIMSPageResourceControl, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSPageResourceControl::~CIMSPageResourceControl()
// ----------------------------------------------------------------------------
//
CIMSPageResourceControl::~CIMSPageResourceControl()
    {
    IMUM_CONTEXT( CIMSPageResourceControl::~CIMSPageResourceControl, 0, KLogUi );

    if ( iPageResources )
        {
        iPageResources->ResetAndDestroy();
        }

    delete iPageResources;
    iPageResources = NULL;
    }

// ----------------------------------------------------------------------------
// CIMSPageResourceControl::ConstructL()
// ----------------------------------------------------------------------------
//
void CIMSPageResourceControl::ConstructL()
    {
    IMUM_CONTEXT( CIMSPageResourceControl::ConstructL, 0, KLogUi );

    iPageResources = new ( ELeave ) CResourceStackArray( EIPRLastPage );
    }

// ----------------------------------------------------------------------------
// CIMSPageResourceControl::NewL()
// ----------------------------------------------------------------------------
//
CIMSPageResourceControl* CIMSPageResourceControl::NewL()
    {
    IMUM_STATIC_CONTEXT( CIMSPageResourceControl::NewL, 0, utils, KLogUi );

    CIMSPageResourceControl* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIMSPageResourceControl::NewLC()
// ----------------------------------------------------------------------------
//
CIMSPageResourceControl* CIMSPageResourceControl::NewLC()
    {
    IMUM_STATIC_CONTEXT( CIMSPageResourceControl::NewLC, 0, utils, KLogUi );

    CIMSPageResourceControl* self = new ( ELeave ) CIMSPageResourceControl();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

/******************************************************************************



******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSPageResourceControl::PushResourceL()
// ----------------------------------------------------------------------------
//
void CIMSPageResourceControl::PushResourceL(
    const TInt aType,
    const TInt aPageResource )
    {
    IMUM_CONTEXT( CIMSPageResourceControl::PushResourceL, 0, KLogUi );

    Stack( aType ).AppendL( aPageResource );
    }

// ----------------------------------------------------------------------------
// CIMSPageResourceControl::Resource()
// ----------------------------------------------------------------------------
//
TInt CIMSPageResourceControl::Resource(
    const TInt aType,
    const TBool aRemove )
    {
    IMUM_CONTEXT( CIMSPageResourceControl::Resource, 0, KLogUi );

    CResourceStack& stack = Stack( aType );
    TInt index = stack.Count() - 1;
    if ( !aRemove || aRemove && !index )
        {
        return stack[index];
        }
    else
        {
        TInt resource = stack[index];
        stack.Delete( index );
        return resource;
        }
    }

// ----------------------------------------------------------------------------
// CIMSPageResourceControl::Count()
// ----------------------------------------------------------------------------
//
TInt CIMSPageResourceControl::Count( const TInt aType )
    {
    IMUM_CONTEXT( CIMSPageResourceControl::Count, 0, KLogUi );

    return Stack( aType ).Count();
    }

// ----------------------------------------------------------------------------
// CIMSPageResourceControl::CreateStackL()
// ----------------------------------------------------------------------------
//
CResourceStack* CIMSPageResourceControl::CreateStackL(
    const TInt aPageResource )
    {
    IMUM_CONTEXT( CIMSPageResourceControl::CreateStackL, 0, KLogUi );

    __ASSERT_DEBUG( iPageResources != NULL, Panic( EIRPPArrayNull ) );

    CResourceStack* stack = new ( ELeave ) CResourceStack( EIPRLastPage );
    CleanupStack::PushL( stack );

    iPageResources->AppendL( stack );
    stack->AppendL( aPageResource );

    CleanupStack::Pop( stack );

    return stack;
    }

// ----------------------------------------------------------------------------
// CIMSPageResourceControl::operator[]()
// ----------------------------------------------------------------------------
//
TInt CIMSPageResourceControl::operator[]( const TInt aType )
    {
    IMUM_CONTEXT( CIMSPageResourceControl::operator, 0, KLogUi );

    return Resource( aType );
    }

/******************************************************************************

    TOOLS

******************************************************************************/


// ----------------------------------------------------------------------------
// CIMSPageResourceControl::Stack()
// ----------------------------------------------------------------------------
//
CResourceStack& CIMSPageResourceControl::Stack( const TInt aType )
    {
    IMUM_CONTEXT( CIMSPageResourceControl::Stack, 0, KLogUi );

    __ASSERT_DEBUG( iPageResources->Count(), Panic( EIRPPStackEmpty ) );
    __ASSERT_DEBUG( aType < iPageResources->Count(), Panic( EIRPPUnknownType ) );
    __ASSERT_DEBUG( ( *iPageResources )[aType] != NULL, Panic( EIRPPStackNull ) );
    __ASSERT_DEBUG( iPageResources != NULL, Panic( EIRPPArrayNull ) );

    return *( *iPageResources )[aType];
    }

// End of File
