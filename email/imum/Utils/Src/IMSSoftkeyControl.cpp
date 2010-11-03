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
* Description:  IMSSoftkeyControl.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <StringLoader.h>           // StringLoader

#include "IMSSoftkeyControl.h"
#include "ImumPanic.h"
#include "ImumUtilsLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
enum TIMCSoftkeyErrors
    {
    EIMCLeftSoftkeyFailure = 0,
    EIMCMiddleSoftkeyFailure,
    EIMCRightSoftkeyFailure
    };

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::CIMSSoftkeyControl()
// ----------------------------------------------------------------------------
//
CIMSSoftkeyControl::CIMSSoftkeyControl(
     CEikButtonGroupContainer& aSoftkeys )
     :
     iSoftkeys( aSoftkeys ),
     iFlags( 0 )
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::CIMSSoftkeyControl, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::~CIMSSoftkeyControl()
// ----------------------------------------------------------------------------
//
CIMSSoftkeyControl::~CIMSSoftkeyControl()
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::~CIMSSoftkeyControl, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::ConstructL()
// ----------------------------------------------------------------------------
//
void CIMSSoftkeyControl::ConstructL()
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::ConstructL, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::NewL()
// ----------------------------------------------------------------------------
//
CIMSSoftkeyControl* CIMSSoftkeyControl::NewL(
     CEikButtonGroupContainer& aSoftkeys )
    {
    IMUM_STATIC_CONTEXT( CIMSSoftkeyControl::NewL, 0, utils, KLogUi );

    CIMSSoftkeyControl* self = NewLC( aSoftkeys );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::NewLC()
// ----------------------------------------------------------------------------
//
CIMSSoftkeyControl* CIMSSoftkeyControl::NewLC(
     CEikButtonGroupContainer& aSoftkeys )
    {
    IMUM_STATIC_CONTEXT( CIMSSoftkeyControl::NewLC, 0, utils, KLogUi );

    CIMSSoftkeyControl* self = new ( ELeave ) CIMSSoftkeyControl( aSoftkeys );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::ChangeObserver()
// ----------------------------------------------------------------------------
//
TInt CIMSSoftkeyControl::ChangeObserver(
    const TBool aOn,
    const TISCObservedKeys aSoftkey,
    MEikCommandObserver& aCommandObserver )
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::ChangeObserver, 0, KLogUi );

    TInt error = KErrNone;

    if ( aOn )
        {
        error = SetObserver( aSoftkey, aCommandObserver );
        }
    else
        {
        RemoveObserver( aSoftkey );
        }

    return error;
    }

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::SetObserver()
// ----------------------------------------------------------------------------
//
TInt CIMSSoftkeyControl::SetObserver(
    const TISCObservedKeys aSoftkey,
    MEikCommandObserver& aCommandObserver )
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::SetObserver, 0, KLogUi );

    TRAPD( error,
        iSoftkeys.UpdateCommandObserverL( aSoftkey, aCommandObserver ) );
    iFlags.SetFlag( aSoftkey );

    return error;
    }

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::SetObserver()
// ----------------------------------------------------------------------------
//
TInt CIMSSoftkeyControl::SetObserver(
    const TBool aLeftSoftkey,
    const TBool aRightSoftkey,
    const TBool aMiddleSoftkey,
    MEikCommandObserver& aCommandObserver )
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::SetObserver, 0, KLogUi );

    // Set observer for the left softkey
    TInt error = ChangeObserver(
        aLeftSoftkey, EISCLeftSoftkey, aCommandObserver );
    __ASSERT_DEBUG( error == KErrNone,
        User::Panic( KIMCSoftkeyControllerPanic, EIMCLeftSoftkeyFailure ) );

    // Set observer for the right softkey
    if ( error == KErrNone )
        {
        error = ChangeObserver(
            aRightSoftkey, EISCRightSoftkey, aCommandObserver );

        __ASSERT_DEBUG( error == KErrNone, User::Panic(
            KIMCSoftkeyControllerPanic, EIMCRightSoftkeyFailure ) );
        }

    // Set observer for the middle softkey
    if ( error == KErrNone )
        {
        error = ChangeObserver(
            aMiddleSoftkey, EISCMiddleSoftkey, aCommandObserver );
        __ASSERT_DEBUG( error == KErrNone, User::Panic(
            KIMCSoftkeyControllerPanic, EIMCMiddleSoftkeyFailure ) );
        }

    return error;
    }

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::RemoveObserver()
// ----------------------------------------------------------------------------
//
void CIMSSoftkeyControl::RemoveObserver(
    const TISCObservedKeys aSoftkey )
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::RemoveObserver, 0, KLogUi );

    iSoftkeys.RemoveCommandObserver( aSoftkey );
    iFlags.ClearFlag( aSoftkey );
    }

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::operator()()
// ----------------------------------------------------------------------------
//
CEikButtonGroupContainer& CIMSSoftkeyControl::operator()()
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::operator, 0, KLogUi );

    return iSoftkeys;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::DefineCustomSoftkeyL()
// ----------------------------------------------------------------------------
//
void CIMSSoftkeyControl::DefineCustomSoftkeyL(
    const TInt aLabelResource,
    const TISCObservedKeys aSoftkeyPos,
    const TInt aSoftkeyCmd,
    MEikCommandObserver* aCommandObserver )
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::DefineCustomSoftkeyL, 0, KLogUi );

    // Define buttons 'Options' and 'Start'
    HBufC* options = StringLoader::LoadL( aLabelResource );
    CleanupStack::PushL( options );
    DefineCustomSoftkeyL(
        *options, aSoftkeyPos, aSoftkeyCmd, aCommandObserver );
    CleanupStack::PopAndDestroy( options );
    options = NULL;
    }


// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::DefineCustomSoftkeyL()
// ----------------------------------------------------------------------------
//
void CIMSSoftkeyControl::DefineCustomSoftkeyL(
    const TDesC& aSoftkeyLabel,
    const TISCObservedKeys aSoftkeyPos,
    const TInt aSoftkeyCmd,
    MEikCommandObserver* aCommandObserver )
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::DefineCustomSoftkeyL, 0, KLogUi );

    // Make sure there is matching command for the position
    if ( iSoftkeys.PositionById( aSoftkeyCmd ) > KErrNotFound )
        {
        // First, remove the observer for the key
        if ( iFlags.Flag( aSoftkeyPos ) )
            {
            RemoveObserver( aSoftkeyPos );
            }

        // Remove the key information and then add new command
        iSoftkeys.RemoveCommandFromStack( aSoftkeyPos, aSoftkeyCmd );
        iSoftkeys.AddCommandToStackL( aSoftkeyPos, aSoftkeyCmd, aSoftkeyLabel );

        if ( aCommandObserver )
            {
            // Set as observer and set the flags
            SetObserver( aSoftkeyPos, *aCommandObserver );
            }
        }
    }

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::MSKLabelling()
// ----------------------------------------------------------------------------
//
TBool CIMSSoftkeyControl::MSKLabelling()
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::MSKLabelling, 0, KLogUi );

    return iFlags.Flag( EISCAllowMiddleSoftkeyLabelChange );
    }

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::SetMSKLabelling()
// ----------------------------------------------------------------------------
//
void CIMSSoftkeyControl::SetMSKLabelling( const TBool aNewState )
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::SetMSKLabelling, 0, KLogUi );

    return iFlags.ChangeFlag( EISCAllowMiddleSoftkeyLabelChange, aNewState );
    }

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::UpdateMiddleSoftkeyLabelL()
// ----------------------------------------------------------------------------
//
void CIMSSoftkeyControl::UpdateMiddleSoftkeyLabelL(
    const TInt aLabelResource,
    const TInt aCommandId,
    MEikCommandObserver* aCommandObserver )
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::UpdateMiddleSoftkeyLabelL, 0, KLogUi );

    // If labelling allowed, change the label for the middle soft key
    if ( MSKLabelling() && iPreviousMskLabelResource != aLabelResource )
        {
        iPreviousMskLabelResource = aLabelResource;
        DefineCustomSoftkeyL(
            aLabelResource,
            EISCMiddleSoftkey,
            aCommandId,
            aCommandObserver );
        }
    }

// ----------------------------------------------------------------------------
// CIMSSoftkeyControl::SetVisibility()
// ----------------------------------------------------------------------------
//
void CIMSSoftkeyControl::SetVisibility(
    const TBool aVisible,
    MEikCommandObserver* aCommandObserver )
    {
    IMUM_CONTEXT( CIMSSoftkeyControl::SetVisibility, 0, KLogUi );
    IMUM_IN();
    if ( !aVisible )
        {
        TBufC<1> dummy;
        TRAP_IGNORE( DefineCustomSoftkeyL( dummy, EISCLeftSoftkey, 0, aCommandObserver ) );
        TRAP_IGNORE( DefineCustomSoftkeyL( dummy, EISCRightSoftkey, 0, aCommandObserver ) );
        TRAP_IGNORE( DefineCustomSoftkeyL( dummy, EISCMiddleSoftkey, 0, aCommandObserver ) );
        }

    IMUM_OUT();
    }

// End of File
