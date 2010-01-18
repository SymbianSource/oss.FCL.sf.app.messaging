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
* Description:   Check id any of given numbers is caller's own number
*
*/




// INCLUDE FILES
#include    "mmsownnumber.h"
#include    "mmsgenutils.h"
#include    <e32std.h>
#include    <badesca.h>
#include    <centralrepository.h>
#include    <telconfigcrkeys.h>

#include    <CVPbkContactManager.h>
#include    <VPbkContactStoreUris.h>
#include    <CVPbkContactStoreUriArray.h>
#include    <MVPbkContactLinkArray.h>
#include    <MVPbkContactLink.h>
#include    <TVPbkContactStoreUriPtr.h>
#include    <MVPbkContactStoreList.h>
#include    <MVPbkContactOperationBase.h>
#include    <MVPbkStoreContact.h>
#include    <MVPbkFieldType.h>
#include    <MVPbkContactFieldTextData.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsOwnNumber::CMmsOwnNumber
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsOwnNumber::CMmsOwnNumber()
    : CActive( EPriorityNormal ),
    iIndex( KErrNotFound )
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CMmsOwnNumber::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsOwnNumber::ConstructL( RFs* aFs )
    {
    iFs = aFs;
    
    // The function is supposed to get a descriptor with packed URIs.
    // As we only have one URI we assume it is packed enough
    CVPbkContactStoreUriArray* storeArray = CVPbkContactStoreUriArray::NewLC();
    TVPbkContactStoreUriPtr uriPtr( VPbkContactStoreUris::SimGlobalOwnNumberUri() );
    storeArray->AppendL( uriPtr );
    
    // TEST TRY ALSO DEFAULT DATABASE AS SIM DATABASE DOES NOT WORK IN WINS
    
/*    
    TVPbkContactStoreUriPtr uriPtr2( VPbkContactStoreUris::DefaultCntDbUri() );
    storeArray->AppendL( uriPtr2 );
*/    
    // END OF TEST
    
    iNumberOfStores = storeArray->Count();
    iNumberOfOpenStores = iNumberOfStores; // otimistic
    
    // Open the database with the URI that specifies our own number
    iContactManager = CVPbkContactManager::NewL( *storeArray, aFs );
    
	CleanupStack::PopAndDestroy( storeArray );
	
	iContactList = &(iContactManager->ContactStoresL());
	
	//Let's find the number of digits to match
    iDigitsToMatch = KMmsNumberOfDigitsToMatch ;

    CRepository* repository = NULL;
    TInt error = KErrNone;
    TRAP( error, repository = CRepository::NewL( KCRUidTelConfiguration ));
    if ( error == KErrNone)
        {
        error = repository->Get( KTelMatchDigits, iDigitsToMatch );
        delete repository;
        if( error != KErrNone )
            {
            iDigitsToMatch=KMmsNumberOfDigitsToMatch;
            }
    	}
    }

// -----------------------------------------------------------------------------
// CMmsOwnNumber::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmsOwnNumber* CMmsOwnNumber::NewL( RFs* aFs )
    {
    CMmsOwnNumber* self = new( ELeave ) CMmsOwnNumber;
    
    CleanupStack::PushL( self );
    self->ConstructL( aFs );
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CMmsOwnNumber::~CMmsOwnNumber()
    {
	Cancel();
	delete iContact;
	delete iOperation;
	delete iResultArray;
	delete iContactManager;
    }
    
// ---------------------------------------------------------
// CMmsOwnNumber::DoCancel
// ---------------------------------------------------------
//
void CMmsOwnNumber::DoCancel()
    {
	delete iOperation;
	iOperation = NULL;
	TRequestStatus* s=&iStatus;
    User::RequestComplete(s, KErrCancel);
    }

// ---------------------------------------------------------
// CMmsOwnNumber::RunL
// ---------------------------------------------------------
//
void CMmsOwnNumber::RunL()
    {
    
    delete iOperation;
    iOperation = NULL;
    
    if ( iStatus != KErrNone )
        {
#ifdef _DEBUG
        TMmsGenUtils::Log( _L("OwnNumber: RunL status: %d "), iStatus.Int() );
#endif

       	// Exit current scheduler loop, resuming execution of the StartL() function
      	iActiveSchedulerWait.AsyncStop();
      	return;
        }
    
    switch ( iState )
        {
        case EMmsOwnNumberOpening:
            {
            iState = EMmsOwnNumberSearching;
            iMatch = EFalse;
            SearchL();
            break;
            }
        case EMmsOwnNumberSearching:
            {
            iState = EMmsOwnNumberCheckingDuplicates;
            CheckDuplicatesL();
            break;
            }
        case EMmsOwnNumberCheckingDuplicates:
            {
            iState = EMmsOwnNumberIdle;
            iContactList->CloseAll( *this );
            break;
            }
        default:
            {
            break;
            }
        }
    if ( !IsActive() )
        {
       	// Exit current scheduler loop, resuming execution of the StartL() function
        delete iResultArray;
        iResultArray = NULL;
      	iActiveSchedulerWait.AsyncStop();
        }
    }
    
// ---------------------------------------------------------
// CMmsOwnNumber::RunError
// ---------------------------------------------------------
//

#ifdef _DEBUG
TInt CMmsOwnNumber::RunError( TInt aError )
#else
TInt CMmsOwnNumber::RunError( TInt /* aError */ )
#endif
    {
#ifdef _DEBUG
    TMmsGenUtils::Log( _L("OwnNumber: RunError: %d "), aError );
    TMmsGenUtils::Log( _L("OwnNumber: state: %d "), iState );
#endif
    delete iOperation;
    iOperation = NULL;
    delete iResultArray;
    iResultArray = NULL;
  	iActiveSchedulerWait.AsyncStop();	//Exit current scheduler loop, resuming execution of the Start() function
  	return KErrNone;
    }

// ---------------------------------------------------------
// CMmsOwnNumber::FindCompleteL
// ---------------------------------------------------------
//
void CMmsOwnNumber::FindCompleteL( MVPbkContactLinkArray* aResults )
    {
#ifdef _DEBUG
    TMmsGenUtils::Log( _L("OwnNumber::FindCompleteL ") );
#endif
    iResultArray = aResults;
    iContactToMatch = iResultArray->Count(); // point beyound the array
    iMatch = EFalse;
    
    // check the result array for matches and set our index accordingly
    if ( iResultArray->Count() == 0 )
        {
        // we are responsible for the results array if we did not leave.
        delete iResultArray;
        iResultArray = 0;
        // no match found - this is not our own number
        iNumberToMatch++;
        if ( iNumberToMatch < iNumberList->MdcaCount() )
            {
            // try the next one
            iState = EMmsOwnNumberOpening;
            }
        }
    else
        {
        // we found at least one match.
        // What do we do now...
        // Analyse the result in detail or just say "This is our own number"...
        
        // First approximation: Say this is our own.
        // Actually exact match cases should be checked, too (short number)
        // And possible email match, but that needs a different search function
                
        // save the match and return to RunL
        // It will stop the active scheduler and allow the code to return to caller
        iIndex = iNumberToMatch;
        if ( iNumberList->MdcaPoint( iNumberToMatch ).Length() < iDigitsToMatch ||
            iResultArray->Count() > 1 )
            {
            // need exact match, start from first item in the array
            iContactToMatch = 0;
            }
        }
    TRequestStatus* s=&iStatus;
    User::RequestComplete( s, KErrNone );
    }
    
// ---------------------------------------------------------
// CMmsOwnNumber::FindFailed
// ---------------------------------------------------------
//
void CMmsOwnNumber::FindFailed(TInt aError)
    {
#ifdef _DEBUG
    TMmsGenUtils::Log( _L("OwnNumber: FindFailed: %d "), aError );
#endif
    iIndex = aError;
 	TRequestStatus* s=&iStatus;
    iNumberToMatch++;
    if ( iNumberToMatch < iNumberList->MdcaCount() )
        {
        // try the next one
        iState = EMmsOwnNumberOpening;
        }
    User::RequestComplete( s, KErrNone );
    }
    
// ---------------------------------------------------------
// CMmsOwnNumber::OpenComplete
// ---------------------------------------------------------
//
void CMmsOwnNumber::OpenComplete()
    {
    // if opening fails, StoreUnavailable is called
#ifdef _DEBUG
    TMmsGenUtils::Log( _L("OwnNumber::OpenCompleteL") );
#endif
	TRequestStatus* s=&iStatus;
    User::RequestComplete( s, KErrNone );
    }
    
// ---------------------------------------------------------
// CMmsOwnNumber::StoreReady
// ---------------------------------------------------------
//
void CMmsOwnNumber::StoreReady(MVPbkContactStore& /*aContactStore*/)
    {
    // Nothing to do here
#ifdef _DEBUG
    TMmsGenUtils::Log( _L("OwnNumber::StoreReady") );
#endif
    }
    
// ---------------------------------------------------------
// CMmsOwnNumber::StoreUnavailable
// ---------------------------------------------------------
//
void CMmsOwnNumber::StoreUnavailable(MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/)
    {
#ifdef _DEBUG
    TMmsGenUtils::Log( _L("OwnNumber::StoreUnavailable") );
#endif
    iNumberOfOpenStores--;
    }
    
// ---------------------------------------------------------
// CMmsOwnNumber::HandleStoreEventL
// ---------------------------------------------------------
//
void CMmsOwnNumber::HandleStoreEventL(
    MVPbkContactStore& /*aContactStore*/, 
    TVPbkContactStoreEvent /*aStoreEvent*/)
    {
    
    }
    
// ---------------------------------------------------------
// CMmsOwnNumber::VPbkSingleContactOperationComplete
// ---------------------------------------------------------
//
void CMmsOwnNumber::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& /*aOperation*/,
    MVPbkStoreContact* aContact)    
    {
#ifdef _DEBUG
    TMmsGenUtils::Log( _L("OwnNumber::VPbkSingleContactOperationComplete") );
#endif
    iContact = aContact;
    
    MVPbkBaseContactFieldCollection& fields = iContact->Fields();
    
    TInt fieldCount = fields.FieldCount();
    
    TInt i;
    
/*    
    const TInt maxMatchPriority = iContactManager->FieldTypes().MaxMatchPriority();
*/    
    for ( i = 0; i < fieldCount && !iMatch ; i++ )
        {
        const MVPbkBaseContactField& field = fields.FieldAt( i );
/*        
        const MVPbkFieldType* fieldType = field.MatchFieldType( maxMatchPriority );
*/        
        const MVPbkContactFieldData& fieldData =  field.FieldData();
        
        // I could not figure out how to check the actual field type
        // (phone number or email or what), we must try all text fields
        if ( fieldData.DataType() == EVPbkFieldStorageTypeText )
            {
            const MVPbkContactFieldTextData& textData = MVPbkContactFieldTextData::Cast(fieldData);
            // Now we finally have the data we can compare with our own data
            if ( textData.Text().Compare( iNumberList->MdcaPoint( iNumberToMatch ) ) == 0 )
                {
                // exact match.
                iMatch = ETrue;
                }
            }
        }
    
    delete iContact;
    iContact = NULL;
    
    iContactToMatch++;
    
    if ( !iMatch && iContactToMatch < iResultArray->Count() )
        {
        iState = EMmsOwnNumberSearching;
        }
    else if ( !iMatch )
        {
        // This was not an exact match
        iIndex = KErrNotFound;
        delete iResultArray;
        iResultArray = 0;
        iNumberToMatch++;
        if ( iNumberToMatch < iNumberList->MdcaCount() )
            {
            // try the next one if any left
            iState = EMmsOwnNumberOpening;
            }
        }
    else
        {
        // keep LINT happy.
        }
        
 	TRequestStatus* s=&iStatus;
    User::RequestComplete( s, KErrNone );
    }

// ---------------------------------------------------------
// CMmsOwnNumber::
// ---------------------------------------------------------
//
void CMmsOwnNumber::VPbkSingleContactOperationFailed(
    MVPbkContactOperationBase& /*aOperation*/, 
#ifdef _DEBUG
    TInt aError)
#else    
    TInt /*aError*/)
#endif
    {
#ifdef _DEBUG
    TMmsGenUtils::Log( _L("OwnNumber: ContactOperationFailed: %d "), aError );
#endif
    // If not found iMatch stays false
    iContactToMatch++;
    
 	TRequestStatus* s=&iStatus;
    User::RequestComplete( s, KErrNone );
    }

    
// ---------------------------------------------------------
// CMmsOwnNumber::StartL
// ---------------------------------------------------------
//
TInt CMmsOwnNumber::StartL( const CDesCArray& aNumberList )
    {
    iIndex = KErrNotFound;
    delete iOperation;
    iOperation = NULL;
    
    if ( aNumberList.MdcaCount() <= 0)
        {
        return iIndex; // no contacts, not found
        }
    iNumberList = &aNumberList;
    iNumberToMatch = 0;
    iContactToMatch = 0;
        
    iContactList->OpenAllL( *this );
        
    // We must set ourselves to pending because we don't give our status to anybody
    // When search is complete, the functions call our callbacks, and we complete ourselves
    iState = EMmsOwnNumberOpening;
    iStatus = KRequestPending;
	SetActive();
	iActiveSchedulerWait.Start();	//Re-enter the active scheduler--execution halts here until RunL is called
	
	// after iActiveSchedulerWait has been stopped execution continues here
	
	return iIndex; // return the possible index of caller's number
    }
    
// ---------------------------------------------------------
// CMmsOwnNumber::SearchL
// ---------------------------------------------------------
//
void CMmsOwnNumber::SearchL()
    {
#ifdef _DEBUG
    TMmsGenUtils::Log( _L("OwnNumber::SearchL") );
#endif
  	// Try checking number of open stores to see if anything was opened
    if ( iNumberOfOpenStores > 0 )
        {
        iOperation = iContactManager->MatchPhoneNumberL(
            iNumberList->MdcaPoint( iNumberToMatch ), iDigitsToMatch, *this );
    
        // We must set ourselves to pending because we don't give our status to anybody
        // When search is complete, the functions call our callbacks, and we complete ourselves
        iStatus = KRequestPending;
	    SetActive();
        }
    // If we do not become active, RunL stops ActiveSchedulerWait
    }
    
// ---------------------------------------------------------
// CMmsOwnNumber::CheckDuplicatesL
// ---------------------------------------------------------
//
void CMmsOwnNumber::CheckDuplicatesL()
    {
    // If more than one contact found or length of number is less than the 
    // match number length, check for exact match
    
    // The match array is in iResultArray
    
#ifdef _DEBUG
    TMmsGenUtils::Log( _L("OwnNumber::CheckDuplicatesL") );
#endif
    delete iContact;
    iContact = NULL;
    
    TInt count = 0;
    if ( iResultArray )
        {
        count = iResultArray->Count();
        }
    
    if (  iContactToMatch < count )
        {
        iMatch = EFalse;
        iState = EMmsOwnNumberSearching;
		// Coverty fix, Forward NULL
        if(iResultArray)
        	{
        	iOperation = iContactManager->RetrieveContactL(
                iResultArray->At( iContactToMatch ),
                *this);
            }
        iStatus = KRequestPending;
        SetActive();
        }
    else
        {
        delete iResultArray;
        iResultArray = NULL;
        iStatus = KRequestPending;
        SetActive();
 	    TRequestStatus* s=&iStatus;
        User::RequestComplete( s, KErrNone );
        }
    
    }
    
    
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
