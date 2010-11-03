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
* Description:  IMASCenRepControl.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <centralrepository.h>          // CRepository
#include "MuiuDynamicHeader.h"

#include "IMASCenRepControl.h"
#include "ImumPanic.h"
#include "ImumUtilsLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
enum TICRCErrors
    {
    EICRCNoError = 0,
    EICRCNoCenRepSession
    };

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIMASCenRepControl::CIMASCenRepControl()
// ----------------------------------------------------------------------------
//
CIMASCenRepControl::CIMASCenRepControl()
    {
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::~CIMASCenRepControl()
// ----------------------------------------------------------------------------
//
CIMASCenRepControl::~CIMASCenRepControl()
    {
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::ConstructL()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::NewL()
// ----------------------------------------------------------------------------
//
CIMASCenRepControl* CIMASCenRepControl::NewL()
    {
    IMUM_STATIC_CONTEXT( CIMASCenRepControl::NewL, 0, utils, KLogData );

    CIMASCenRepControl* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::NewLC()
// ----------------------------------------------------------------------------
//
CIMASCenRepControl* CIMASCenRepControl::NewLC()
    {
    IMUM_STATIC_CONTEXT( CIMASCenRepControl::NewLC, 0, utils, KLogData );

    CIMASCenRepControl* self = new ( ELeave ) CIMASCenRepControl();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

/******************************************************************************

    Storer Tools

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMASCenRepControl::SetActiveCentralRepository()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::SetActiveCentralRepository(
    CRepository& aRepository )
    {
    iCenRep = &aRepository;
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::SetActiveBaseKey()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::SetActiveBaseKey(
    const TUint32 aAccountId,
    const TUid& aProtocol )
    {
    iBaseKey = IMASAccountControl::CreateBaseKeyFromAccountId(
        aAccountId, aProtocol );
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::SetActiveBaseKey()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::SetActiveBaseKey(
    const TUint32 aExtendedAccountId )
    {
    iBaseKey = IMASAccountControl::CreateBaseKeyFromExtendedAccountId(
        aExtendedAccountId );
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::CreateToAccountL()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::CreateToAccountL(
    const TUint32 aSetting,
    const TInt aValue )
    {
    __ASSERT_DEBUG( iCenRep != NULL,
        User::Panic( KIMASCenRepCtrlPanic, EICRCNoCenRepSession ) );

    // Determine the setting id and create the new key
    TUint32 settingId = iBaseKey + aSetting;
    TInt err = iCenRep->Create( settingId, aValue );
    // if the setting already exists, this means that there are some lingering
    // extended settings from a mailbox which was e.g. deleted using
    // CEmailAccounts. We can just overwrite the setting
    if( err == KErrAlreadyExists )
    	{
    	SetToAccountL( aSetting, aValue );
    	}
    else
    	{
    	User::LeaveIfError( err );
    	}
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::CreateToAccountL()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::CreateToAccountL(
    const TUint32 aSetting,
    const TDesC& aString )
    {
    __ASSERT_DEBUG( iCenRep != NULL,
        User::Panic( KIMASCenRepCtrlPanic, EICRCNoCenRepSession ) );

    // Determine the setting id and create the new key
    TUint32 settingId = iBaseKey + aSetting;
    TInt err = iCenRep->Create( settingId, aString );
    // if the setting already exists, this means that there are some lingering
    // extended settings from a mailbox which was e.g. deleted using
    // CEmailAccounts. We can just overwrite the setting
    if( err == KErrAlreadyExists )
    	{
    	SetToAccountL( aSetting, aString );
    	}
    else
    	{
    	User::LeaveIfError( err );
    	}
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::CreateToAccountL()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::CreateToAccountL(
    const TInt aSettingHigh,
    const TInt aSettingLow,
    const TInt64 aValue )
    {
    // The high and low part has to be stored separately
    CreateToAccountL( aSettingHigh, I64HIGH( aValue ) );
    CreateToAccountL( aSettingLow, I64LOW( aValue ) );
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::DeleteFromAccountL()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::DeleteFromAccountL(
    const TInt aSetting ) const
    {
    __ASSERT_DEBUG( iCenRep != NULL,
        User::Panic( KIMASCenRepCtrlPanic, EICRCNoCenRepSession ) );

    // Load the values
    TUint32 settingId = iBaseKey + aSetting;
    User::LeaveIfError( iCenRep->Delete( settingId ) );
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::DeleteFromAccountL()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::DeleteFromAccountL(
    const TInt aSettingHigh,
    const TInt aSettingLow ) const
    {
    // Load the values
    TUint32 settingId = iBaseKey + aSettingHigh;
    User::LeaveIfError( iCenRep->Delete( settingId ) );

    settingId = iBaseKey + aSettingLow;
    User::LeaveIfError( iCenRep->Delete( settingId ) );
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::SetToAccountL()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::SetToAccountL(
    const TUint32 aSetting,
    const TInt aValue )
    {
    __ASSERT_DEBUG( iCenRep != NULL,
        User::Panic( KIMASCenRepCtrlPanic, EICRCNoCenRepSession ) );

    // Determine the setting id and create the new key
    TUint32 settingId = iBaseKey + aSetting;
    User::LeaveIfError( iCenRep->Set( settingId, aValue ) );
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::SetToAccountL()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::SetToAccountL(
    const TUint32 aSetting,
    const TDesC& aString )
    {
    __ASSERT_DEBUG( iCenRep != NULL,
        User::Panic( KIMASCenRepCtrlPanic, EICRCNoCenRepSession ) );

    // Determine the setting id and create the new key
    TUint32 settingId = iBaseKey + aSetting;
    User::LeaveIfError( iCenRep->Set( settingId, aString ) );
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::SetToAccountL()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::SetToAccountL(
    const TInt aSettingHigh,
    const TInt aSettingLow,
    const TInt64 aSetting )
    {
    // The high and low part has to be stored separately
    SetToAccountL( aSettingHigh, I64HIGH( aSetting ) );
    SetToAccountL( aSettingLow, I64LOW( aSetting ) );
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::GetFromAccountL()
// ----------------------------------------------------------------------------
//
TInt64 CIMASCenRepControl::GetFromAccountL(
    const TInt aSettingHigh,
    const TInt aSettingLow ) const
    {
    TInt64 value = KErrNone;
    User::LeaveIfError( GetFromAccount( aSettingHigh, aSettingLow, value ) );

    return value;
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::GetFromAccountL()
// ----------------------------------------------------------------------------
//
TInt CIMASCenRepControl::GetFromAccountL(
    const TInt aSetting ) const
    {
    TInt value = KErrNotFound;
    User::LeaveIfError( GetFromAccount( aSetting, value ) );

    return value;
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::GetTextFromAccountL()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::GetTextFromAccountL(
    TDes& aText,
    const TInt aSetting ) const
    {
    User::LeaveIfError( GetTextFromAccount( aText,  aSetting ) );
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::SetActiveDefaultDataKey()
// ----------------------------------------------------------------------------
//
void CIMASCenRepControl::SetActiveDefaultDataKey()
    {
    iBaseKey = 0;
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::GetFromAccount()
// ----------------------------------------------------------------------------
//
TInt CIMASCenRepControl::GetFromAccount( const TInt aSettingHigh,
    const TInt aSettingLow,
    TInt64& aSettingValue ) const
    {
    __ASSERT_DEBUG( iCenRep != NULL,
        User::Panic( KIMASCenRepCtrlPanic, EICRCNoCenRepSession ) );

    // Prepare the number
    TInt high = 0;
    TInt low = 0;
    TInt error = KErrNotFound;

    // Load the values
    TUint32 settingId = iBaseKey + aSettingHigh;
    error = iCenRep->Get( settingId, high );

    settingId = iBaseKey + aSettingLow;

    if ( error == KErrNone )
        {
        error = iCenRep->Get( settingId, low );
        }
    // Return the combined value
    if ( error == KErrNone )
        {
        aSettingValue = MAKE_TINT64( high, low );
        }

    return error;
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::GetFromAccount()
// ----------------------------------------------------------------------------
//
TInt CIMASCenRepControl::GetFromAccount(
    const TInt aSetting, TInt& aSettingValue ) const
    {
    __ASSERT_DEBUG( iCenRep != NULL,
        User::Panic( KIMASCenRepCtrlPanic, EICRCNoCenRepSession ) );

    // Load the values
    TUint32 settingId = iBaseKey + aSetting;
    return iCenRep->Get( settingId, aSettingValue );
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::GetTextFromAccount()
// ----------------------------------------------------------------------------
//
TInt CIMASCenRepControl::GetTextFromAccount(
    TDes& aText,
    const TInt aSetting ) const
    {
    __ASSERT_DEBUG( iCenRep != NULL,
        User::Panic( KIMASCenRepCtrlPanic, EICRCNoCenRepSession ) );

    // Load the values
    TUint32 settingId = iBaseKey + aSetting;
    return iCenRep->Get( settingId, aText );
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::GetTextFromAccount()
// ----------------------------------------------------------------------------
//
TInt CIMASCenRepControl::GetTextFromAccount(
    TDes8& aText,
    const TInt aSetting ) const
    {
    __ASSERT_DEBUG( iCenRep != NULL,
        User::Panic( KIMASCenRepCtrlPanic, EICRCNoCenRepSession ) );

    // Load the values
    TUint32 settingId = iBaseKey + aSetting;
    return iCenRep->Get( settingId, aText );
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::GetFromAccountOrCreateL()
// ----------------------------------------------------------------------------
//
TInt CIMASCenRepControl::GetFromAccountOrCreateL(
    const TUint32 aSetting, TInt aDefaultValue )
    {
    TInt value = aDefaultValue;
    TInt error = GetFromAccount( aSetting, value );
    
    if( error == KErrNotFound )
        {
        CreateToAccountL( aSetting, aDefaultValue );
        }
    else
        {
        User::LeaveIfError( error );
        }

    return value;
    }

// ----------------------------------------------------------------------------
// CIMASCenRepControl::GetFromAccountOrCreateL()
// ----------------------------------------------------------------------------
//
TInt64 CIMASCenRepControl::GetFromAccountOrCreateL(
    const TUint32 aSettingHigh,
    const TUint32 aSettingLow,
    TInt64 aDefaultValue )
    {
    TInt64 value = aDefaultValue;
    TInt error = GetFromAccount( aSettingHigh, aSettingLow, value );
    
    if( error == KErrNotFound )
        {
        CreateToAccountL( aSettingHigh, aSettingLow, aDefaultValue );
        }
    else
        {
        User::LeaveIfError( error );
        }

    return value;
    }
	
// End of File
