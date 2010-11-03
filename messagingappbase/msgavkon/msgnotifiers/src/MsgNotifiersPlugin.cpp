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
* Description:  MsgNotifiersPlugin implementation
*
*/



#include <eiknotapi.h>
#include <AknNotifyStd.h>

#include "MsgConfirmSendNotifier.h"

IMPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray();

void CleanupArray( TAny* aArray )
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers =
        static_cast<CArrayPtrFlat<MEikSrvNotifierBase2>*>( aArray );
    notifiers->ResetAndDestroy();
    delete notifiers;
    }

CArrayPtr<MEikSrvNotifierBase2>* DoCreateNotifierArrayL()
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers =
        new ( ELeave ) CArrayPtrFlat<MEikSrvNotifierBase2>( 1 );
    CleanupStack::PushL( TCleanupItem( CleanupArray, notifiers ) );
    // SendAs notifier
    notifiers->AppendL( CMsgConfirmSendNotifier::NewL( ETrue ) );
    // SendUi notifier
    notifiers->AppendL( CMsgConfirmSendNotifier::NewL( EFalse ) );
    CleanupStack::Pop();	// array cleanup
    return( notifiers );
    }

//
// Lib main entry point
//
EXPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
	{
	CArrayPtr<MEikSrvNotifierBase2>* notifiers = NULL;
	TRAP_IGNORE( notifiers = DoCreateNotifierArrayL() );
	return( notifiers );
	}
