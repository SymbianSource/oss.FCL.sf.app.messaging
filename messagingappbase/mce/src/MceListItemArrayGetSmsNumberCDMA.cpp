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



// INCLUDE FILES
#include <txtetext.h>
#include <smuthdr.h>                // CSmsHeader
#include <SmsMessageWrappers.h>     // Transport Wrappers
#include "MceMessageListItemArray.h"

// ----------------------------------------------------
// CMceListItemArray::GetSmsNumberL
// This is invoked when the call key is pressed to get
// the callback number from SMS entry if it exists,
// otherwise this method gets the phone number from SMS
// entry.
// ----------------------------------------------------
void CMceMessageListItemArray::GetSmsNumberL( const TMsvEntry& aEntry, TPhCltTelephoneNumber& aNumber ) const
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

    //
    // Get the callback number from the SMS entry if it exists
    //
    HBufC* callbackNumber = header->CdmaMessage().GetCallbackNumberL();
    if ( callbackNumber )
        {
        CleanupStack::PushL( callbackNumber );
        aNumber.Copy(callbackNumber->Des());
        CleanupStack::PopAndDestroy( callbackNumber );
        }

    //
    // SMS entry doesn't contain a callback number, get the number from
    // the FromAddress instead
    //
    if ( aNumber.Length() <= 0 )
        {
        aNumber = header->FromAddress();
        }

    CleanupStack::PopAndDestroy( 4 ); // store, header, parentEntry, nullString
    }

//  End of File
