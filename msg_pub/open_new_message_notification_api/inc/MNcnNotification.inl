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
* Description:   Inline methods for ECom plug-in use.
*
*/



// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// MNcnNotification::~MNcnNotification
//
// Destructor.
// ----------------------------------------------------------------------------
//
inline MNcnNotification::~MNcnNotification()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// ----------------------------------------------------------------------------
// MNcnNotification::CreateMNcnNotificationL
//
// ECom factory method.
// ----------------------------------------------------------------------------
//
inline MNcnNotification* MNcnNotification::CreateMNcnNotificationL()
    {
    TInt32 keyOffset = _FOFF( MNcnNotification,
                              iDtor_ID_Key );
    _LIT8(KDefaultImplementation,"NcnNotification");
	TEComResolverParams resolverParams;
	resolverParams.SetDataType( KDefaultImplementation() );
	resolverParams.SetWildcardMatch (ETrue);
    TAny* obj =
        REComSession::CreateImplementationL( KNcnNotificationInterfaceUid,
                                             keyOffset,
                                             resolverParams );
    return reinterpret_cast<MNcnNotification*> ( obj );
    }

// End of file
