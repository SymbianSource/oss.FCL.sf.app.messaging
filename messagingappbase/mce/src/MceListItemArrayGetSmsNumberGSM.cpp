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
*     Defines methods for CMceListItemArray
*
*/


#if 0
// INCLUDE FILES
#include <txtetext.h>
#include <smuthdr.h>
#ifdef RD_MSG_LIST_IMPROVEMENT
#include "MceMessageListItemArray.h"
#else
#include "MceListItemArray.h"
#endif
// ----------------------------------------------------
// CMceListItemArray::GetSmsNumberL
// Gets phone number from SMS entry
// ----------------------------------------------------
#ifdef RD_MSG_LIST_IMPROVEMENT
void CMceMessageListItemArray::GetSmsNumberL( const TMsvEntry& aEntry, TPhCltTelephoneNumber& aNumber ) const
#else
void CMceListItemArray::GetSmsNumberL( const TMsvEntry& aEntry, TPhCltTelephoneNumber& aNumber ) const
#endif
    {
    TMsvId entryId = aEntry.Id();
    CMsvEntry* parentEntry = iSession->GetEntryL( entryId );
    CleanupStack::PushL( parentEntry );
    CPlainText* nullString = CPlainText::NewL();
    CleanupStack::PushL( nullString );
    CSmsHeader* header = CSmsHeader::NewL( CSmsPDU::ESmsDeliver, *nullString );
    CleanupStack::PushL( header );
    CMsvStore* store = parentEntry->ReadStoreL();
    CleanupStack::PushL( store );
    header->RestoreL( *store );
    aNumber = header->FromAddress();
    CleanupStack::PopAndDestroy( 4 ); // store, header, parentEntry, nullString
    }
#endif

//  End of File
