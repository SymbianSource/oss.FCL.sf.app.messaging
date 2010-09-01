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
* Description:   Inline methods for ECom plug-in use.
*
*/



// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// MNcnInternalNotification::~MNcnInternalNotification
//
// Destructor.
// ----------------------------------------------------------------------------
//  
inline MNcnInternalNotification::~MNcnInternalNotification()
    {
    // ECom session already notified by the base class.
    }

// ----------------------------------------------------------------------------
// MNcnInternalNotification::CreateMNcnInternalNotificationL
//
// ECom factory method.
// ----------------------------------------------------------------------------
//  
inline MNcnInternalNotification* 
MNcnInternalNotification::CreateMNcnInternalNotificationL()
    {
    MNcnNotification* obj = CreateMNcnNotificationL();
    return reinterpret_cast<MNcnInternalNotification*> ( obj );
    }
    
// End of file    
