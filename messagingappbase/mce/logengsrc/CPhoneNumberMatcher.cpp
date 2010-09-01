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
* Description:  
*     Phonenumber matching wrapper for contact model MatchPhoneNumberL
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldsInfo.h>
#include <commonphoneparser.h>          // Common phone number validity checker


#include <contactmatcher.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkStoreContact.h>
#include <CVPbkPhoneNumberMatchStrategy.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>

#include <centralrepository.h>
#include <telconfigcrkeys.h>        // KCRUidTelephonyConfiguration
#include <commonphoneparser.h>      // Common phone number validity checker



#include "CPhoneNumberMatcher.h"

// CONSTANTS
const TInt KPhoneNumberDigitsMatch = 7; // max amount of digits in matching

// ================= MEMBER FUNCTIONS =======================


CPhoneNumberMatcher::CPhoneNumberMatcher()
                    
    {
    }


EXPORT_C CPhoneNumberMatcher* CPhoneNumberMatcher::NewL(
    RFs& aFsSession )
                                        
    {
    CPhoneNumberMatcher* self = new ( ELeave ) CPhoneNumberMatcher();
    CleanupStack::PushL( self );
    self->ConstructL( aFsSession );
    CleanupStack::Pop();
    return self;
    }

void CPhoneNumberMatcher::ConstructL(
    RFs& aFsSession )
    {
    // Create contact matcher and open stores
    iContactMatcher = CContactMatcher::NewL( &aFsSession );

    iMatchDigitCount = KPhoneNumberDigitsMatch; 

    // Read the amount of digits to be used in contact matching
    // The key is owned by PhoneApp
    CRepository* repository = CRepository::NewLC(KCRUidTelConfiguration);
    if ( repository->Get(KTelMatchDigits, iMatchDigitCount) == KErrNone )
    {
        // Min is 7
        iMatchDigitCount =
            Max(iMatchDigitCount, KPhoneNumberDigitsMatch );
        }
    CleanupStack::PopAndDestroy(); // repository
    }


EXPORT_C CPhoneNumberMatcher::~CPhoneNumberMatcher()
    {
    delete iContactMatcher;
    delete iName;
    }


EXPORT_C TPtrC CPhoneNumberMatcher::GetUniqueName()
    {
    if( iName )
        {
        return iName->Des();
        }
    return KNullDesC();
    }

EXPORT_C TInt CPhoneNumberMatcher::GetFieldId()
    {
    if( iName ) // if name exists then previous match was succesfull, else return KErrNone
        {
        return iFieldId;
        }
    return KErrNotFound;
    }

EXPORT_C TInt CPhoneNumberMatcher::GetContactId( TContactItemId& aContactId )
    {
    if( iName ) // if name exists then previous match was succesfull, else return KErrNone
        {
        aContactId = iContactId;
        return KErrNone;
        }
    return KErrNotFound;
    }


EXPORT_C TInt CPhoneNumberMatcher::MatchCountL( const TDesC& aNumber )
    {
    TInt result (KErrNone);


    // Match contacts to the from address
    CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC();
    iContactMatcher->MatchPhoneNumberL(
        aNumber,
        iMatchDigitCount,
        CVPbkPhoneNumberMatchStrategy::EVPbkStopOnFirstMatchFlag,
        *linkArray );

    if ( linkArray->Count() > 0 )
        {

        // There should be only one match since EVPbkStopOnFirstMatchFlag was used.
        // Use first match and read contact from the store.
        MVPbkStoreContact* contact;
        iContactMatcher->GetStoreContactL(linkArray->At(0), &contact);
        contact->PushL();
        
        MVPbkStoreContactFieldCollection& coll = contact->Fields();

        iName = iContactMatcher->GetNameL( coll );
    //    iName->Copy( alias->Left( 100 ) );
    //    delete alias;
    //    alias = NULL;
        CleanupStack::PopAndDestroy( contact ); // contact
        result = 1;
        }
    CleanupStack::PopAndDestroy( linkArray ); // linkArray
    return result;
    }
    
    
void CPhoneNumberMatcher::OpenDefaultMatchStoresL( TRequestStatus& aStatus )
    {
    CancelOperation();
    iContactMatcher->OpenDefaultMatchStoresL( aStatus );
    }
    
void CPhoneNumberMatcher::CancelOperation()
    {
    iContactMatcher->CancelOperation();
    }
    
// End of File

