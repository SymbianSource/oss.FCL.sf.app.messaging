/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ImumDomainApiTestBlocks.cpp
*
*/



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "ImumDomainApiTest.h"

#include <e32math.h>

#include <ImumDomainApi.h>
#include <ImumDaMailboxServices.h>
#include <ImumDaMailboxUtilities.h>
#include <SendUiConsts.h>


#include <ImumDaSettingsData.h>
#include <ImumDaSettingsKeys.h>
#include <ImumDaSettingsDataCollection.h>

#include <cemailaccounts.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KImumMboxDefaultAccessPoint = TImumDaSettings::EValueApAlwaysAsk;
_LIT( KImumMboxDefaultEmailAddress, "" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImumDomainApiTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CImumDomainApiTest::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CImumDomainApiTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

        ENTRY( "CreateMailbox", CImumDomainApiTest::CreateMailboxLTestL ),
        ENTRY( "SetDefaultMailbox", CImumDomainApiTest::SetDefaultMailboxLTestL ),
        ENTRY( "IsMailbox", CImumDomainApiTest::IsMailBoxTestL ),
        ENTRY( "GetMailboxEntry", CImumDomainApiTest::GetMailboxEntryTestL ),
        ENTRY( "IsMailboxHealthy", CImumDomainApiTest::IsMailboxHealthyTestL ),

        // Non UI requiring tests
        ENTRY( "CreateSettingsWithValidUid", CImumDomainApiTest::CreateSettingsDataWithValidUidTestL ),
        ENTRY( "IsMailMtm", CImumDomainApiTest::IsMailMtmTestL ),
        ENTRY( "GetMailboxes", CImumDomainApiTest::GetMailboxesTestL ),
        ENTRY( "AddSet", CImumDomainApiTest::AddSetTestL ),
        ENTRY( "SetAttr", CImumDomainApiTest::SetAttrTestL ),
        ENTRY( "SetAttrToDataCollection", CImumDomainApiTest::SetAttrToDataCollectionTestL ),
        ENTRY( "SetDataException", CImumDomainApiTest::SetDataExceptionL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CImumDomainApiTest::SetupL
// -----------------------------------------------------------------------------
//
void CImumDomainApiTest::SetupL()
	{
	//RemoveOldMailboxesL();
	}

// -----------------------------------------------------------------------------
// CImumDomainApiTest::TeardownL
// -----------------------------------------------------------------------------
//
void CImumDomainApiTest::TeardownL()
	{
	//RemoveOldMailboxesL();
	}

// -----------------------------------------------------------------------------
// CImumDomainApiTest::RemoveOldMailboxesL
// Helper method. Removes all old mailboxes
// NOTE! Using this might cause complications. Presumable some Imum settings are
// left as ghosts if this is used.
// -----------------------------------------------------------------------------
//
void CImumDomainApiTest::RemoveOldMailboxesL()
	{
	CEmailAccounts* iAccounts = CEmailAccounts::NewL();
	CleanupStack::PushL( iAccounts );

	RArray<TImapAccount> imapAccountArray;
	iAccounts->GetImapAccountsL( imapAccountArray );
	TInt count = imapAccountArray.Count();
	for( TInt i = 0; i < count; i++)
		{
		iAccounts->DeleteImapAccountL( imapAccountArray[i] );
		}
	imapAccountArray.Close();

	count = 0;
	RArray<TPopAccount> popAccountArray;
	iAccounts->GetPopAccountsL( popAccountArray );
	count = popAccountArray.Count();
	for( TInt i = 0; i < count; i++)
		{
		iAccounts->DeletePopAccountL( popAccountArray[i] );
		}
	popAccountArray.Close();

	count = 0;
	RArray<TSmtpMobilityAccount> smtpMobilityAccountArray;
	iAccounts->GetSmtpMobilityAccountsL( smtpMobilityAccountArray );
	count = smtpMobilityAccountArray.Count();
	for( TInt i = 0; i < count; i++)
		{
		iAccounts->DeleteSmtpMobilityAccountL( smtpMobilityAccountArray[i] );
		}
	smtpMobilityAccountArray.Close();

	count = 0;
	RArray<TSmtpAccount> smtpAccountArray;
	iAccounts->GetSmtpAccountsL( smtpAccountArray );
	count = smtpAccountArray.Count();
	for( TInt i = 0; i < count; i++)
		{
		iAccounts->DeleteSmtpAccountL( smtpAccountArray[i] );
		}
	smtpAccountArray.Close();

	CleanupStack::PopAndDestroy( iAccounts );
	}

// -----------------------------------------------------------------------------
// CImumDomainApiTest::NextMailboxIndexL
// Helper method. Gets next mailbox index number.
// This is used when creating new mailbox so that there are no same index numbers.
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::NextMailboxIndexL()
	{
	CEmailAccounts* iAccounts = CEmailAccounts::NewL();
	CleanupStack::PushL( iAccounts );

	TInt count( 0 );
	RArray<TSmtpAccount> smtpAccountArray;
	iAccounts->GetSmtpAccountsL( smtpAccountArray );
	count = smtpAccountArray.Count();
	smtpAccountArray.Close();

	CleanupStack::PopAndDestroy( iAccounts );
	return count;
	}


// -----------------------------------------------------------------------------
// CImumDomainApiTest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KImumDomainApiTest, "ImumDomainApiTest" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KImumDomainApiTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KImumDomainApiTest,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CImumDomainApiTest::CreateMailboxLTestL
// Tests MImumDaMailboxServices::CreateMailboxL
// Needs UI support from test framework (CoeEnv)
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::CreateMailboxLTestL( CStifItemParser& aItem )
	{
	SetupL();

    TImumDaSettings::TTextServerAddress serverAddress;
    serverAddress.Append(_L("serveraddress.com"));
    TImumDaSettings::TTextEmailAddress emailAddress;
    emailAddress.Append(_L("testi8@armada.digia.com"));
    TImumDaSettings::TTextMailboxName mailboxName;
    mailboxName.Append(_L("Test mailbox "));

    // Calculate index number which is added to mailbox name
    // This done because there cannot be mailboxes with the same name
    TBuf<5> indexBuf;
    _LIT( string, "%x" );
    indexBuf.Format( string, NextMailboxIndexL() );
    mailboxName.Append( indexBuf );

    // Create Domain API
    CImumDomainApi* domainApi = CreateDomainApiLC();
    // For mailbox creation
    MImumDaMailboxServices* services = &domainApi->MailboxServicesL();

    // Create settings data
    CImumDaSettingsData* imumDaSettingsData( NULL );
    // Creates default settings for POP3 (but a server address must be set manually).
    imumDaSettingsData = services->CreateSettingsDataL( KNullUid );
    if( imumDaSettingsData == NULL )
    	{
        return KErrGeneral;
    	}
    
    TInt err = KErrNone;
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyEmailAddress,
            emailAddress );
	if( err )
		{
		return KErrGeneral;
		}
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyMailboxName,
            mailboxName );
	if( err )
		{
		return KErrGeneral;
		}

	// Add Pop3 set
    MImumDaSettingsDataCollection& settingsCollectionPop = imumDaSettingsData->AddSetL( KSenduiMtmPop3Uid );
    err = settingsCollectionPop.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		return KErrGeneral;
		}
    err = settingsCollectionPop.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		return KErrGeneral;
		}

	// Add smtp set
    MImumDaSettingsDataCollection& settingsCollectionSmtp = imumDaSettingsData->AddSetL( KSenduiMtmSmtpUid );
    err = settingsCollectionSmtp.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		return KErrGeneral;
		}
    err = settingsCollectionSmtp.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		return KErrGeneral;
		}
	TMsvId msvId( 0 );
    msvId = services->CreateMailboxL( *imumDaSettingsData );

    // Test passed
    CleanupStack::PopAndDestroy( 1 ); // domainApi
    TeardownL();
    return KErrNone;
	}


// -----------------------------------------------------------------------------
// CImumDomainApiTest::SetDefaultMailboxLTestL
// Tests MImumDaMailboxServices::SetDefaultMailboxL
// and MImumDaMailboxUtilities::DefaultMailboxId
// Needs UI support from test framework (CoeEnv)
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::SetDefaultMailboxLTestL( CStifItemParser& aItem )
	{
	SetupL();

    TImumDaSettings::TTextServerAddress serverAddress;
    serverAddress.Append(_L("serveraddress.com"));
    TImumDaSettings::TTextEmailAddress emailAddress;
    emailAddress.Append(_L("testi8@armada.digia.com"));
    TImumDaSettings::TTextMailboxName mailboxName;
    mailboxName.Append(_L("Mailbox"));
    TImumDaSettings::TTextMailboxName mailboxNameTwo;
    mailboxNameTwo.Append(_L("Mailbox"));

    // Calculate index number which is added to mailbox name
    // This done because there cannot be mailboxes with the same name
    TBuf<5> indexBuf;
    _LIT( string, "%x" );
    indexBuf.Format( string, NextMailboxIndexL() );
    mailboxName.Append( indexBuf );
    mailboxName.Append( indexBuf );

    TBuf<5> indexBuf2;
    _LIT( string2, "%x" );
    indexBuf2.Format( string2, NextMailboxIndexL()+1 );
    mailboxNameTwo.Append( indexBuf2 );
    mailboxNameTwo.Append( indexBuf2 );

    // Create Domain API
    // LC() not used because of testing purpose
    CImumDomainApi* domainApi = CreateDomainApiL();
    CleanupStack::PushL(domainApi);
    
    // For mailbox creation
    MImumDaMailboxServices* services = &domainApi->MailboxServicesL();
    MImumDaMailboxUtilities* utils = &domainApi->MailboxUtilitiesL();

    // Start creating mailbox 1
    // Create settings data
    CImumDaSettingsData* imumDaSettingsData( NULL );
    imumDaSettingsData = services->CreateSettingsDataL( KNullUid );
    if( imumDaSettingsData == NULL )
    	{
        return KErrGeneral;
    	}

    TInt err = KErrNone;
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyEmailAddress,
            emailAddress );
	if( err )
		{
		return KErrGeneral;
		}
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyMailboxName,
            mailboxName );
	if( err )
		{
		return KErrGeneral;
		}

	// Add Pop3 set
    MImumDaSettingsDataCollection& settingsCollectionPop = imumDaSettingsData->AddSetL( KSenduiMtmPop3Uid );
    err = settingsCollectionPop.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		return KErrGeneral;
		}
    err = settingsCollectionPop.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		return KErrGeneral;
		}

	// Add smtp set
    MImumDaSettingsDataCollection& settingsCollectionSmtp = imumDaSettingsData->AddSetL( KSenduiMtmSmtpUid );
    err = settingsCollectionSmtp.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		return KErrGeneral;
		}
    err = settingsCollectionSmtp.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		return KErrGeneral;
		}
	TMsvId msvId_mailbox1( 0 );
	msvId_mailbox1 = services->CreateMailboxL( *imumDaSettingsData );
	// End of mailbox creation 1

    // Start creating mailbox 2
    // Create settings data
    CImumDaSettingsData* imumDaSettingsDataTwo( NULL );
    imumDaSettingsDataTwo = services->CreateSettingsDataL( KNullUid );
    if( imumDaSettingsDataTwo == NULL )
    	{
        return KErrGeneral;
    	}

    err = KErrNone;
    err = imumDaSettingsDataTwo->SetAttr(
            TImumDaSettings::EKeyEmailAddress,
            emailAddress );
	if( err )
		{
		return KErrGeneral;
		}
    err = imumDaSettingsDataTwo->SetAttr(
            TImumDaSettings::EKeyMailboxName,
            mailboxNameTwo );
	if( err )
		{
		return KErrGeneral;
		}

	// Add Pop3 set
    MImumDaSettingsDataCollection& settingsCollectionPopTwo = imumDaSettingsDataTwo->AddSetL( KSenduiMtmPop3Uid );
    err = settingsCollectionPopTwo.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		return KErrGeneral;
		}
    err = settingsCollectionPopTwo.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		return KErrGeneral;
		}

	// Add smtp set
    MImumDaSettingsDataCollection& settingsCollectionSmtpTwo = imumDaSettingsDataTwo->AddSetL( KSenduiMtmSmtpUid );
    err = settingsCollectionSmtpTwo.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		return KErrGeneral;
		}
    err = settingsCollectionSmtpTwo.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		return KErrGeneral;
		}
	TMsvId msvId_mailbox2( 0 );
	msvId_mailbox2 = services->CreateMailboxL( *imumDaSettingsDataTwo );
	// End of mailbox creation 2

	// Set default Id and compare it to default mailbox Id
	TMsvId defaultMsvId( 0 );
	services->SetDefaultMailboxL( msvId_mailbox1 );
	defaultMsvId = utils->DefaultMailboxId();
	if( defaultMsvId != msvId_mailbox1 )
		{
		return KErrGeneral;
		}

	// Set default Id and compare it to default mailbox Id
	defaultMsvId = 0;
	services->SetDefaultMailboxL( msvId_mailbox2 );
	defaultMsvId = utils->DefaultMailboxId();
	if( defaultMsvId != msvId_mailbox2 )
		{
		return KErrGeneral;
		}

    //Test Passed
    CleanupStack::PopAndDestroy( 1 ); // domainApi
    TeardownL();
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CImumDomainApiTest::IsMailBoxTestL
// Tests MImumDaMailboxUtilities::IsMailBox
// Needs UI support from test framework (CoeEnv)
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::IsMailBoxTestL( CStifItemParser& aItem )
	{
	SetupL();

    TImumDaSettings::TTextServerAddress serverAddress;
    serverAddress.Append(_L("serveraddress.com"));
    TImumDaSettings::TTextEmailAddress emailAddress;
    emailAddress.Append(_L("testi8@armada.digia.com"));
    TImumDaSettings::TTextMailboxName mailboxName;
    mailboxName.Append(_L("Mailbox"));

    // Calculate index number which is added to mailbox name
    // This done because there cannot be mailboxes with the same name
    TBuf<5> indexBuf;
    _LIT( string, "%x" );
    indexBuf.Format( string, NextMailboxIndexL() );
    mailboxName.Append( indexBuf );
    mailboxName.Append( indexBuf );

    // Create Domain API
    CImumDomainApi* domainApi = CreateDomainApiLC();
    // For mailbox creation
    MImumDaMailboxServices* services = &domainApi->MailboxServicesL();
    MImumDaMailboxUtilities* utils = &domainApi->MailboxUtilitiesL();

    // Create settings data
    CImumDaSettingsData* imumDaSettingsData( NULL );
    imumDaSettingsData = services->CreateSettingsDataL( KNullUid );
    if( imumDaSettingsData == NULL )
    	{
        return KErrGeneral;
    	}

    TInt err = KErrNone;
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyEmailAddress,
            emailAddress );
	if( err )
		{
		return KErrGeneral;
		}
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyMailboxName,
            mailboxName );
	if( err )
		{
		return KErrGeneral;
		}

	// Add Pop3 set
    MImumDaSettingsDataCollection& settingsCollectionPop = imumDaSettingsData->AddSetL( KSenduiMtmPop3Uid );
    err = settingsCollectionPop.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		return KErrGeneral;
		}
    err = settingsCollectionPop.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		return KErrGeneral;
		}

	// Add Smtp set
    MImumDaSettingsDataCollection& settingsCollectionSmtp = imumDaSettingsData->AddSetL( KSenduiMtmSmtpUid );
    err = settingsCollectionSmtp.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		return KErrGeneral;
		}
    err = settingsCollectionSmtp.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		return KErrGeneral;
		}
	TMsvId msvId( 0 );
    msvId = services->CreateMailboxL( *imumDaSettingsData );

    
    // Test IsMailBox
    TBool isMailbox( EFalse );
    // Test created mailbox, should return ETrue
    isMailbox = utils->IsMailbox( msvId );
    if( isMailbox == EFalse )
    	{
        return KErrGeneral;
    	}

    // Test false mailbox, should return EFalse
    TMsvId tempMsvId( 2 );
    isMailbox = utils->IsMailbox( tempMsvId );
    if( isMailbox != EFalse )
    	{
        return KErrGeneral;
    	}
    
    // Test IsMailbox( const TMsvEntry& aMailbox )    
    // If not a mailbox
    if (!utils->IsMailbox(utils->GetMailboxEntryL(msvId)))
    	{
    	return KErrGeneral;
    	}
    
    //Test Passed
    CleanupStack::PopAndDestroy( 1 ); // domainApi
    TeardownL();
    return KErrNone;
	}


// -----------------------------------------------------------------------------
// CImumDomainApiTest::GetMailboxEntryTestL
// Tests MImumDaMailboxUtilities::GetMailboxEntryL
// Needs UI support from test framework (CoeEnv)
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::GetMailboxEntryTestL( CStifItemParser& aItem )
	{
	SetupL();

    TImumDaSettings::TTextServerAddress serverAddress;
    serverAddress.Append(_L("serveraddress.com"));
    TImumDaSettings::TTextEmailAddress emailAddress;
    emailAddress.Append(_L("testi8@armada.digia.com"));
    TImumDaSettings::TTextMailboxName mailboxName;
    mailboxName.Append(_L("Mailbox"));

    // Calculate index number which is added to mailbox name
    // This done because there cannot be mailboxes with the same name
    TBuf<5> indexBuf;
    _LIT( string, "%x" );
    indexBuf.Format( string, NextMailboxIndexL() );
    mailboxName.Append( indexBuf );
    mailboxName.Append( indexBuf );

    // Create Domain API
    CImumDomainApi* domainApi = CreateDomainApiLC();
    // For mailbox creation
    MImumDaMailboxServices* services = &domainApi->MailboxServicesL();
    MImumDaMailboxUtilities* utils = &domainApi->MailboxUtilitiesL();

    // Create settings data
    CImumDaSettingsData* imumDaSettingsData( NULL );
    imumDaSettingsData = services->CreateSettingsDataL( KNullUid );
    if( imumDaSettingsData == NULL )
    	{
    	CleanupStack::PopAndDestroy(); // domainApi
        return KErrGeneral;
    	}

    TInt err = KErrNone;
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyEmailAddress,
            emailAddress );
	if( err )
		{
		CleanupStack::PopAndDestroy(); // domainApi
		return KErrGeneral;
		}
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyMailboxName,
            mailboxName );
	if( err )
		{
		CleanupStack::PopAndDestroy(); // domainApi
		return KErrGeneral;
		}

	// Add Pop3 set
    MImumDaSettingsDataCollection& settingsCollectionPop = imumDaSettingsData->AddSetL( KSenduiMtmPop3Uid );
    err = settingsCollectionPop.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		CleanupStack::PopAndDestroy(); // domainApi
		return KErrGeneral;
		}
    err = settingsCollectionPop.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		CleanupStack::PopAndDestroy(); // domainApi
		return KErrGeneral;
		}

	// Add Smtp set
    MImumDaSettingsDataCollection& settingsCollectionSmtp = imumDaSettingsData->AddSetL( KSenduiMtmSmtpUid );
    err = settingsCollectionSmtp.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		CleanupStack::PopAndDestroy(); // domainApi
		return KErrGeneral;
		}
    err = settingsCollectionSmtp.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		CleanupStack::PopAndDestroy(); // domainApi
		return KErrGeneral;
		}
	TMsvId msvId( 0 );
    msvId = services->CreateMailboxL( *imumDaSettingsData );

    // Test GetMailboxEntryL()
    const MImumDaMailboxUtilities::TImumDaMboxRequestType imumDaMboxRequestType( 
    		MImumDaMailboxUtilities::EValueRequestCurrent );
    TMsvEntry msvEntry = utils->GetMailboxEntryL( msvId, imumDaMboxRequestType );
    // compare msvIds
    if( msvId != msvEntry.Id() )
    	{
    	CleanupStack::PopAndDestroy(); // domainApi
    	return KErrGeneral;
    	}

    // Test GetMailboxEntriesL()
    MImumDaMailboxUtilities::RMsvEntryArray entryArray;
    CleanupClosePushL(entryArray);
    
    utils->GetMailboxEntriesL( msvId, entryArray);
    
    // Test fails if the array mtms are not KSenduiMtmPop3Uid or KSenduiMtmSmtpUid
    if ( !((entryArray[0].iMtm == KSenduiMtmPop3Uid &&
    	 entryArray[1].iMtm == KSenduiMtmSmtpUid) ||
    	 (entryArray[0].iMtm == KSenduiMtmSmtpUid &&
    	 entryArray[1].iMtm == KSenduiMtmPop3Uid)))
    	{
    	CleanupStack::PopAndDestroy( 2 ); // entryArray, domainApi
    	return KErrGeneral;
    	}
    
    //Test Passed
	CleanupStack::PopAndDestroy( 2 ); // entryArray, domainApi
    TeardownL();
    return KErrNone;
	}


// -----------------------------------------------------------------------------
// CImumDomainApiTest::IsMailboxHealthyTestL
// Tests MImumDaMailboxUtilities::IsMailboxHealthy
// This test creates a mailbox and then calls IsMailboxHealthy with the msvId
// of the mailbox.
// Needs UI support from test framework (CoeEnv)
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::IsMailboxHealthyTestL( CStifItemParser& aItem )
	{
	SetupL();

    TImumDaSettings::TTextServerAddress serverAddress;
    serverAddress.Append(_L("serveraddress.com"));
    TImumDaSettings::TTextEmailAddress emailAddress;
    emailAddress.Append(_L("testi8@armada.digia.com"));
    TImumDaSettings::TTextMailboxName mailboxName;
    mailboxName.Append(_L("Mailbox"));

    // Calculate index number which is added to mailbox name
    // This done because there cannot be mailboxes with the same name
    TBuf<5> indexBuf;
    _LIT( string, "%x" );
    indexBuf.Format( string, NextMailboxIndexL() );
    mailboxName.Append( indexBuf );
    mailboxName.Append( indexBuf );

    // Create Domain API
    CImumDomainApi* domainApi = CreateDomainApiLC();
    // For mailbox creation
    MImumDaMailboxServices* services = &domainApi->MailboxServicesL();
    MImumDaMailboxUtilities* utils = &domainApi->MailboxUtilitiesL();

    // Create settings data
    CImumDaSettingsData* imumDaSettingsData( NULL );
    imumDaSettingsData = services->CreateSettingsDataL( KNullUid );
    if( imumDaSettingsData == NULL )
    	{
        return KErrGeneral;
    	}

    TInt err = KErrNone;
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyEmailAddress,
            emailAddress );
	if( err )
		{
		return KErrGeneral;
		}
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyMailboxName,
            mailboxName );
	if( err )
		{
		return KErrGeneral;
		}

	// Add Pop3 set
    MImumDaSettingsDataCollection& settingsCollectionPop = imumDaSettingsData->AddSetL( KSenduiMtmPop3Uid );
    err = settingsCollectionPop.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		return KErrGeneral;
		}
    err = settingsCollectionPop.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		return KErrGeneral;
		}

	// Add smtp set
    MImumDaSettingsDataCollection& settingsCollectionSmtp = imumDaSettingsData->AddSetL( KSenduiMtmSmtpUid );
    err = settingsCollectionSmtp.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		return KErrGeneral;
		}
    err = settingsCollectionSmtp.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		return KErrGeneral;
		}
	TMsvId msvId( 0 );
    msvId = services->CreateMailboxL( *imumDaSettingsData );

    // Test is mailbox is healthy (should be healthy)
    TBool isHealthy( EFalse );
    isHealthy = utils->IsMailboxHealthy( msvId );

    if( !isHealthy )
    	{
    	return KErrGeneral;
    	}

    //Test Passed
	CleanupStack::PopAndDestroy( 1 ); // domainApi
    TeardownL();
    return KErrNone;
	}


// -----------------------------------------------------------------------------
// CImumDomainApiTest::CreateSettingsDataWithValidUidTestL
// Tests MImumDaMailboxServices::CreateSettingsDataL method with valid
// protocol uids.
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::CreateSettingsDataWithValidUidTestL( CStifItemParser& aItem )
    {
    SetupL();

    // Create Domain API
    CImumDomainApi* domainApi = CreateDomainApiLC();
    // For mailbox creation
    MImumDaMailboxServices* services = &domainApi->MailboxServicesL();

    // Test CreateSettingsDataL with valid UID values
    // Note that according to API header, CreateSettingsDataL takes
    //   any receiving protocol (Imap4 or Pop3) as paramater
    // For example SMTP uid should cause leave.
    // Note that CreateSettingsDataL causes panic when invalid UID
    //   is used with debug build (ASSERT_DEBUG) -> not tested
    CImumDaSettingsData* imumDaSettingsDataZeroUid( NULL );
    imumDaSettingsDataZeroUid = services->CreateSettingsDataL( TUid::Uid( 0 ) );
    if( imumDaSettingsDataZeroUid == NULL )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    CImumDaSettingsData* imumDaSettingsDataPop( NULL );
    imumDaSettingsDataPop = services->CreateSettingsDataL( KSenduiMtmPop3Uid );
    if( imumDaSettingsDataPop == NULL )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    CImumDaSettingsData* imumDaSettingsDataImap( NULL );
    imumDaSettingsDataImap = services->CreateSettingsDataL( KSenduiMtmImap4Uid );
    if( imumDaSettingsDataImap == NULL )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    
    // Test != operator for MImumDaSettingsDataCollection
    MImumDaSettingsDataCollection& imumDaCollectionImap = (*imumDaSettingsDataImap)[0];
    
    if ( imumDaCollectionImap != imumDaCollectionImap )
    	{
        TeardownL();
        return KErrGeneral;
    	}
   
    CleanupStack::PopAndDestroy( 1 ); // domainApi
    TeardownL();
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CImumDomainApiTest::IsMailMtmTestL
// Tests MImumDaMailboxUtilities::IsMailMtm
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::IsMailMtmTestL( CStifItemParser& aItem )
	{
    TBool isMailMtm( EFalse );
    TBool allowExtended( EFalse );

    SetupL();

    // Create Domain API
    CImumDomainApi* domainApi = CreateDomainApiLC();
    // For mailbox creation
    //MImumDaMailboxServices* services = &domainApi->MailboxServicesL();
    MImumDaMailboxUtilities* utils = &domainApi->MailboxUtilitiesL();

    // Test IsMailMtm, aAllowExtended parameter is false
    // Pop3 mtm, should return true
    isMailMtm = utils->IsMailMtm( KSenduiMtmPop3Uid , EFalse );
    if( isMailMtm == EFalse )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    isMailMtm = EFalse;

    // Imap mtm, should return true
    isMailMtm = utils->IsMailMtm( KSenduiMtmImap4Uid , EFalse );
    if( isMailMtm == EFalse )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    isMailMtm = EFalse;

    // Smtp mtm, should return true
    isMailMtm = utils->IsMailMtm( KSenduiMtmSmtpUid , EFalse );
    if( isMailMtm == EFalse )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    isMailMtm = EFalse;

    // UniMessage mtm, should return false
    isMailMtm = utils->IsMailMtm( KSenduiMtmUniMessageUid , EFalse );
    if( isMailMtm )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    isMailMtm = EFalse;

    // 0 mtm, should return false
    isMailMtm = utils->IsMailMtm( TUid::Uid( 0 ) , EFalse );
    if( isMailMtm )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    isMailMtm = EFalse;


    // Test IsMailMtm, aAllowExtended parameter is true
    // Pop3 mtm, should return true
    isMailMtm = utils->IsMailMtm( KSenduiMtmPop3Uid , ETrue );
    if( isMailMtm == EFalse )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    isMailMtm = EFalse;

    // Imap mtm, should return true
    isMailMtm = utils->IsMailMtm( KSenduiMtmImap4Uid , ETrue );
    if( isMailMtm == EFalse )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    isMailMtm = EFalse;

    // Smtp mtm, should return true
    isMailMtm = utils->IsMailMtm( KSenduiMtmSmtpUid , ETrue );
    if( isMailMtm == EFalse )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    isMailMtm = EFalse;

    // UniMessage mtm, should return false
    isMailMtm = utils->IsMailMtm( KSenduiMtmUniMessageUid , ETrue );
    if( isMailMtm )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    isMailMtm = EFalse;

    // 0 mtm, should return false
    isMailMtm = utils->IsMailMtm( TUid::Uid( 0 ) , ETrue );
    if( isMailMtm )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    isMailMtm = EFalse;

    //Test Passed
    CleanupStack::PopAndDestroy( 1 ); // domainApi
    TeardownL();
    return KErrNone;
	}


// -----------------------------------------------------------------------------
// CImumDomainApiTest::GetMailboxesTestL
// Tests MImumDaMailboxUtilities::GetMailboxesL
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::GetMailboxesTestL( CStifItemParser& aItem )
	{
	SetupL();

    TImumDaSettings::TTextServerAddress serverAddress;
    serverAddress.Append(_L("serveraddress.com"));
    TImumDaSettings::TTextEmailAddress emailAddress;
    emailAddress.Append(_L("testi8@armada.digia.com"));
    TImumDaSettings::TTextMailboxName mailboxName;
    mailboxName.Append(_L("Test mailbox "));

    // Calculate index number which is added to mailbox name
    // This done because there cannot be mailboxes with the same name
    TBuf<5> indexBuf;
    _LIT( string, "%x" );
    indexBuf.Format( string, NextMailboxIndexL() );
    mailboxName.Append( indexBuf );

    // Create Domain API
    CImumDomainApi* domainApi = CreateDomainApiLC();
    // For mailbox creation
    MImumDaMailboxServices* services = &domainApi->MailboxServicesL();
    MImumDaMailboxUtilities* utils = &domainApi->MailboxUtilitiesL();

    // Create settings data
    CImumDaSettingsData* imumDaSettingsData( NULL );
    // Creates default settings for POP3 (but a server address must be set manually).
    imumDaSettingsData = services->CreateSettingsDataL( KNullUid );
    if( imumDaSettingsData == NULL )
    	{
        return KErrGeneral;
    	}

    TInt err = KErrNone;
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyEmailAddress,
            emailAddress );
	if( err )
		{
		return KErrGeneral;
		}
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyMailboxName,
            mailboxName );
	if( err )
		{
		return KErrGeneral;
		}

	// Add Pop3 set
    MImumDaSettingsDataCollection& settingsCollectionPop = imumDaSettingsData->AddSetL( KSenduiMtmPop3Uid );
    err = settingsCollectionPop.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		return KErrGeneral;
		}
    err = settingsCollectionPop.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		return KErrGeneral;
		}

	// Add smtp set
    MImumDaSettingsDataCollection& settingsCollectionSmtp = imumDaSettingsData->AddSetL( KSenduiMtmSmtpUid );
    err = settingsCollectionSmtp.SetAttr(
        TImumDaSettings::EKeyAccessPoint,
        KImumMboxDefaultAccessPoint );
	if( err )
		{
		return KErrGeneral;
		}
    err = settingsCollectionSmtp.SetAttr(
            TImumDaSettings::EKeyServer,
            serverAddress );
	if( err )
		{
		return KErrGeneral;
		}
	TMsvId msvId( 0 );
    msvId = services->CreateMailboxL( *imumDaSettingsData );


    MImumDaMailboxUtilities::RMailboxIdArray mailboxIdArray;
    TInt count( 0 );

    // Test with arbitrary Id
    mailboxIdArray = utils->GetMailboxesL( 0x10 );
    count = mailboxIdArray.Count();
    if( count != 0 )
    	{
    	return KErrGeneral;
    	}
    mailboxIdArray.Close();

    // Test with KSenduiMtmImap4UidValue
    mailboxIdArray = utils->GetMailboxesL( KSenduiMtmImap4UidValue );
    count = mailboxIdArray.Count();
    if( count <= 0 )
    	{
    	return KErrGeneral;
    	}
    mailboxIdArray.Close();


    //Test Passed
    CleanupStack::PopAndDestroy( 1 ); // domainApi
    TeardownL();
    return KErrNone;
   	}


// -----------------------------------------------------------------------------
// CImumDomainApiTest::AddSetTestL
// Tests CImumDaSettingsData::AddSetL , ::GetSet, ::DelSet, != and ==
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::AddSetTestL( CStifItemParser& aItem )
	{
    SetupL();

    // Create Domain API
    CImumDomainApi* domainApi = CreateDomainApiLC();
    // For mailbox creation
    MImumDaMailboxServices* services = &domainApi->MailboxServicesL();
    MImumDaMailboxUtilities* utils = &domainApi->MailboxUtilitiesL();

    // Create settings data
    CImumDaSettingsData* imumDaSettingsData( NULL );
    imumDaSettingsData = services->CreateSettingsDataL( KNullUid );
    if( imumDaSettingsData == NULL )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    // Add set POP3
    MImumDaSettingsDataCollection& popImumDaSettingsCollection = imumDaSettingsData->AddSetL( KSenduiMtmPop3Uid );
    MImumDaSettingsDataCollection& tempPopImumDaSettingsCollection = imumDaSettingsData->GetSetL( TUint( 0 ) );
    if( popImumDaSettingsCollection != tempPopImumDaSettingsCollection )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    
    // == operator test
    if( !(popImumDaSettingsCollection == tempPopImumDaSettingsCollection) )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    // Add set SMTP
    MImumDaSettingsDataCollection& smtpImumDaSettingsCollection = imumDaSettingsData->AddSetL( KSenduiMtmSmtpUid );
    MImumDaSettingsDataCollection& tempSmtpImumDaSettingsCollection = imumDaSettingsData->GetSetL( TUint( 1 ) );
    if( smtpImumDaSettingsCollection != tempSmtpImumDaSettingsCollection )
    	{
        TeardownL();
        return KErrGeneral;
    	}
 
    // Delete sets
    imumDaSettingsData->DelSetL( popImumDaSettingsCollection );
    MImumDaSettingsDataCollection& tempImumDaSettingsCollection = imumDaSettingsData->GetSetL( TUint( 0 ) );
    if( smtpImumDaSettingsCollection != tempImumDaSettingsCollection )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    imumDaSettingsData->DelSetL( TUint( 0 ) );


    //Test Passed
	CleanupStack::PopAndDestroy( 1 ); // domainApi
    TeardownL();
    return KErrNone;
	}


// -----------------------------------------------------------------------------
// CImumDomainApiTest::SetAttrTestL
// Tests CImumDaSettingsData::SetAttr , ::GetAttr, ::Reset, ::ResetAll and
// ::Validate
// SetAttr and GetAttr are tested all different parameter types
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::SetAttrTestL( CStifItemParser& aItem )
	{
	TInt err = KErrNone;
	SetupL();

    TImumDaSettings::TTextEmailAddress emailAddress;
    emailAddress.Append(_L("testi8@armada.digia.com"));

    // Create Domain API
    CImumDomainApi* domainApi = CreateDomainApiLC();
    // For mailbox creation
    MImumDaMailboxServices* services = &domainApi->MailboxServicesL();
    MImumDaMailboxUtilities* utils = &domainApi->MailboxUtilitiesL();

    // Create settings data
    CImumDaSettingsData* imumDaSettingsData( NULL );
    imumDaSettingsData = services->CreateSettingsDataL( KNullUid );
    if( imumDaSettingsData == NULL )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    // Add set POP3
    MImumDaSettingsDataCollection& popImumDaSettingsCollection =
    		imumDaSettingsData->AddSetL( KSenduiMtmPop3Uid );

    // Get email address in the beginning (to be compared after resets)
    TImumDaSettings::TTextEmailAddress beginEmailAddress;
    err = imumDaSettingsData->GetAttr(
            TImumDaSettings::EKeyEmailAddress,
            beginEmailAddress );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}


    // Set, get n check attribute
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyEmailAddress,
            emailAddress );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    TImumDaSettings::TTextEmailAddress tempEmailAddress;
    err = imumDaSettingsData->GetAttr(
            TImumDaSettings::EKeyEmailAddress,
            tempEmailAddress );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    if( emailAddress != tempEmailAddress )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    // Reset n check attribute
    imumDaSettingsData->Reset( TImumDaSettings::EKeyEmailAddress );
    err = imumDaSettingsData->GetAttr(
                TImumDaSettings::EKeyEmailAddress,
                tempEmailAddress );
    if( beginEmailAddress != tempEmailAddress )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    // Set n check, reset all n check email address attribute
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyEmailAddress,
            emailAddress );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    
    err = imumDaSettingsData->GetAttr(
            TImumDaSettings::EKeyEmailAddress,
            tempEmailAddress );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    if( emailAddress != tempEmailAddress )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    imumDaSettingsData->ResetAll( );
    err = imumDaSettingsData->GetAttr(
            TImumDaSettings::EKeyEmailAddress,
            tempEmailAddress );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    if( beginEmailAddress != tempEmailAddress )
    	{
        TeardownL();
        return KErrGeneral;
    	}


    // Test different Set/GetAttr parameters

    // Set/GetAttr TInt&
    TInt tintValue( 1 );
    TInt tempTintValue( 0 );
    imumDaSettingsData->Reset( TImumDaSettings::EKeySendDelay );
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeySendDelay,
            tintValue );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    err = imumDaSettingsData->GetAttr(
            TImumDaSettings::EKeySendDelay,
            tempTintValue );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    if( tintValue != tempTintValue )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    // Set/GetAttr TInt64
    TInt64 tint64Value( 1 );
    TInt64 tempTint64Value( 0 );
    imumDaSettingsData->Reset( TImumDaSettings::EKeySendDelay );
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeySendDelay,
            tint64Value );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    err = imumDaSettingsData->GetAttr(
            TImumDaSettings::EKeySendDelay,
            tempTint64Value );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    if( tint64Value != tempTint64Value )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    // Set/GetAttr TMsvId
    TMsvId msvIdValue( 1 );
    TMsvId tempMsvIdValue( 0 );
    imumDaSettingsData->Reset( TImumDaSettings::EKeyInboxUpdateLimit );
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyInboxUpdateLimit,
            msvIdValue );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    err = imumDaSettingsData->GetAttr(
            TImumDaSettings::EKeyInboxUpdateLimit,
            tempMsvIdValue );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    if( msvIdValue != tempMsvIdValue )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    // Set/GetAttr TDes8
    TImumDaSettings::TTextUserName username;
    username.Append( _L( "user@domain.com" ) );
    TImumDaSettings::TTextUserName tempUsername;
    err = imumDaSettingsData->SetAttr(
            TImumDaSettings::EKeyReplyToAddress,
            username );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    err = imumDaSettingsData->GetAttr(
            TImumDaSettings::EKeyReplyToAddress,
            tempUsername );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    if( username != tempUsername )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    // Validate test
    // At least email address is not currently set so should return an error
    err = imumDaSettingsData->Validate();
    
    if ( err == KErrNone )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    delete imumDaSettingsData;
    //Test Passed
	CleanupStack::PopAndDestroy( 1 ); // domainApi
    TeardownL();
    return KErrNone;
	}


// -----------------------------------------------------------------------------
// CImumDomainApiTest::SetAttrToDataCollectionTestL
// Tests MImumDaSettingsDataCollection::SetAttr, ::GetAttr and ::Reset
// SetAttr and GetAttr are tested all different parameter types
// -----------------------------------------------------------------------------
//
TInt CImumDomainApiTest::SetAttrToDataCollectionTestL( CStifItemParser& aItem )
	{
	TInt err = KErrNone;
    TImumDaSettings::TTextEmailAddress emailAddress;
    emailAddress.Append(_L("testi8@armada.digia.com"));
	SetupL();

    // Create Domain API
    CImumDomainApi* domainApi = CreateDomainApiLC();
    // For mailbox creation
    MImumDaMailboxServices* services = &domainApi->MailboxServicesL();
    MImumDaMailboxUtilities* utils = &domainApi->MailboxUtilitiesL();

    // Create settings data
    CImumDaSettingsData* imumDaSettingsData( NULL );
    imumDaSettingsData = services->CreateSettingsDataL( KNullUid );
    if( imumDaSettingsData == NULL )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    // Add set POP3
    MImumDaSettingsDataCollection& popImumDaSettingsCollection =
    		imumDaSettingsData->AddSetL( KSenduiMtmPop3Uid );

    // Set, get n check attribute
    popImumDaSettingsCollection.Reset( TImumDaSettings::EKeyEmailAddress );
    err = popImumDaSettingsCollection.SetAttr(
            TImumDaSettings::EKeyEmailAddress,
            emailAddress );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    TImumDaSettings::TTextEmailAddress tempEmailAddress;
    err = popImumDaSettingsCollection.GetAttr(
            TImumDaSettings::EKeyEmailAddress,
            tempEmailAddress );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    if( emailAddress != tempEmailAddress )
    	{
        TeardownL();
        return KErrGeneral;
    	}


    // Test different Set/GetAttr parameters

    // Set/GetAttr TInt&
    TInt tintValue( 1 );
    TInt tempTintValue( 0 );
    popImumDaSettingsCollection.Reset( TImumDaSettings::EKeySendDelay );
    err = popImumDaSettingsCollection.SetAttr(
            TImumDaSettings::EKeySendDelay,
            tintValue );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    err = popImumDaSettingsCollection.GetAttr(
            TImumDaSettings::EKeySendDelay,
            tempTintValue );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    if( tintValue != tempTintValue )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    // Set/GetAttr TInt64
    TInt64 tint64Value( 1 );
    TInt64 tempTint64Value( 0 );
    popImumDaSettingsCollection.Reset( TImumDaSettings::EKeySendDelay );
    err = popImumDaSettingsCollection.SetAttr(
            TImumDaSettings::EKeySendDelay,
            tint64Value );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    err = popImumDaSettingsCollection.GetAttr(
            TImumDaSettings::EKeySendDelay,
            tempTint64Value );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    if( tint64Value != tempTint64Value )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    // Set/GetAttr TMsvId
    TMsvId msvIdValue( 1 );
    TMsvId tempMsvIdValue( 0 );
    popImumDaSettingsCollection.Reset( TImumDaSettings::EKeyInboxUpdateLimit );
    err = popImumDaSettingsCollection.SetAttr(
            TImumDaSettings::EKeyInboxUpdateLimit,
            msvIdValue );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    err = popImumDaSettingsCollection.GetAttr(
            TImumDaSettings::EKeyInboxUpdateLimit,
            tempMsvIdValue );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    if( msvIdValue != tempMsvIdValue )
    	{
        TeardownL();
        return KErrGeneral;
    	}

    // Set/GetAttr TDes8
    TImumDaSettings::TTextUserName username;
    username.Append( _L( "user@domain.com" ) );
    TImumDaSettings::TTextUserName tempUsername;
    err = popImumDaSettingsCollection.SetAttr(
            TImumDaSettings::EKeyReplyToAddress,
            username );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    err = popImumDaSettingsCollection.GetAttr(
            TImumDaSettings::EKeyReplyToAddress,
            tempUsername );
    if( err )
    	{
        TeardownL();
        return KErrGeneral;
    	}
    if( username != tempUsername )
    	{
        TeardownL();
        return KErrGeneral;
    	}


    //Test Passed
	CleanupStack::PopAndDestroy( 1 ); // domainApi
    TeardownL();
    return KErrNone;
	}

TInt CImumDomainApiTest::SetDataExceptionL( CStifItemParser& aItem )
	{
	SetupL();

    TImumDaSettings::TTextServerAddress serverAddress;
    serverAddress.Append(_L("serveraddress.com"));
    TImumDaSettings::TTextEmailAddress emailAddress;
    emailAddress.Append(_L("testi8@armada.digia.com"));
    TImumDaSettings::TTextMailboxName mailboxName;
    mailboxName.Append(_L("Test mailbox "));

    // Calculate index number which is added to mailbox name
    // This done because there cannot be mailboxes with the same name
    TBuf<5> indexBuf;
    _LIT( string, "%x" );
    indexBuf.Format( string, NextMailboxIndexL() );
    mailboxName.Append( indexBuf );

    // Create Domain API
    CImumDomainApi* domainApi = CreateDomainApiLC();
    // For mailbox creation
    MImumDaMailboxServices* services = &domainApi->MailboxServicesL();

    // Create settings data
    CImumDaSettingsData* imumDaSettingsData( NULL );
    // Creates default settings for POP3 (but a server address must be set manually).
    imumDaSettingsData = services->CreateSettingsDataL( KNullUid );
    if( imumDaSettingsData == NULL )
    	{
    	CleanupStack::PopAndDestroy( 1 ); // domainApi
        return KErrGeneral;
    	}
    
    TInt err(0);
    TRAP(err,imumDaSettingsData->GetSetL(KMaxTUint));
    
    
    if (err == KErrNone)
    	{
    	CleanupStack::PopAndDestroy( 1 ); // domainApi
    	return KErrGeneral;
    	}
    
    // Test passed
    CleanupStack::PopAndDestroy( 1 ); // domainApi
	
	
	return KErrNone;
	}


// -----------------------------------------------------------------------------
// CImumDomainApiTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CImumDomainApiTest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
