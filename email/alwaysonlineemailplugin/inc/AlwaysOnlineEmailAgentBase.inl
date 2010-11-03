/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Inline functions for CAlwaysOnlineEmailAgentBase
*
*
*
*/


#include <ImumInSettingsData.h>
#include <ImumInSettingsDataCollection.h>

//-----------------------------------------------------------------------------
// IsImap4
//-----------------------------------------------------------------------------
inline TBool CAlwaysOnlineEmailAgentBase::IsImap4() const
    {
    return iFlags->Flag( EAOBFIsImap4 );
    }
    
//-----------------------------------------------------------------------------
// IsEmn
//-----------------------------------------------------------------------------
inline TBool CAlwaysOnlineEmailAgentBase::IsEmn() const
    {
    return ( EmnState() != TImumDaSettings::EValueNotificationsOff );
    }
    
// ---------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::LoadSettingL()
// ---------------------------------------------------------------------------
//
template<class T>
T CAlwaysOnlineEmailAgentBase::LoadSettingL( 
    const TUint aKey, 
    const TBool aConnectionSetting ) const
    {
    T state;
    
    if ( aConnectionSetting )
        {
        User::LeaveIfError( 
            iMailboxSettings->GetInSetL( 0 ).GetAttr( aKey, state ) );
        }
    else
        {
        User::LeaveIfError( iMailboxSettings->GetAttr( aKey, state ) );
        }
        
    return state;        
    }
 
