/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file has definition to test functions Belonging to 
*                   Send UI Meeting Request API/MeetingRequestData.h
*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "SendUIMeetingRequestAPITest.h"
#include <MeetingRequestData.h> 
#include <CMessageData.h>
#include <S32MEM.H>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS
#define FIRST_ID    0x0ffff
#define LAST_ID     0xfffff
#define MAILBOX_ID  0x12345
#define FILE_ID     0x123456 
#define NEWFILE_ID  0x12233

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSendUIMeetingRequestAPITest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CSendUIMeetingRequestAPITest::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CSendUIMeetingRequestAPITest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CSendUIMeetingRequestAPITest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "MailBox-FileID", CSendUIMeetingRequestAPITest::TestMRqDataMailBoxnFileIDL ),
        ENTRY( "AppendEntryLocalId-Array", CSendUIMeetingRequestAPITest::TestMRqDataEntryLocalIDL ),
        ENTRY( "MailHeader", CSendUIMeetingRequestAPITest::TestMRqDataMailHeaderL ),
        ENTRY( "Write", CSendUIMeetingRequestAPITest::TestWriteToBufferLC ),
        ENTRY( "Read", CSendUIMeetingRequestAPITest::TestReadFromBufferL ),
        
        //ADD NEW ENTRY HERE ^
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CSendUIMeetingRequestAPITest::TestMRqDataMailBoxnFileIDL
// This function tests UseMailbox, Mailbox, SetDatabaseFileId, and DatabaseFileId
// of CMeetingRequestData
// -----------------------------------------------------------------------------
//
TInt CSendUIMeetingRequestAPITest::TestMRqDataMailBoxnFileIDL(
   CStifItemParser& aItem )
   {

   CMeetingRequestData* mrqData = CMeetingRequestData::NewL();
   TInt error = KErrNone;
   
   //UseMailBox(), MailBox
   TMsvId testMailBoxId(MAILBOX_ID);   
   mrqData->UseMailbox(testMailBoxId);   
   if ( mrqData->Mailbox() != testMailBoxId )
       {
       _LIT( KError, "CMeetingRequestData::UseMailBox/Mailbox failed");
       iLog->Log(KError);
       error = KErrCancel;
       }
   else
       {
       _LIT( KError, "CMeetingRequestData::UseMailBox,Mailbox Passed");
       iLog->Log(KError);
       }
   
   TUint64 testFileId(FILE_ID);
   mrqData->SetDatabaseFileId(testFileId);
   if( mrqData->DatabaseFileId() != testFileId )
       {
       _LIT( KError, "CMeetingRequestData::SetDatabaseFileId/DatabaseFileId failed");
       iLog->Log(KError);       
       error = KErrCancel;
       }
   else
       {
       _LIT( KError, "CMeetingRequestData::SetDatabaseFileId,DatabaseFileId Passed");
       iLog->Log(KError);
       }
   delete mrqData;
   return error;
   
   }

// -----------------------------------------------------------------------------
// CSendUIMeetingRequestAPITest::TestMRqDataEntryLocalIDL
// This function tests AppendEntryLocalId and EntryLocalIdArray of --
// -- CMeetingRequestData
// -----------------------------------------------------------------------------
//
TInt CSendUIMeetingRequestAPITest::TestMRqDataEntryLocalIDL( 
        CStifItemParser& aItem )
    {
    
    CMeetingRequestData* mrqData = CMeetingRequestData::NewL();
    TInt error = KErrNone;
    for ( TUint32 i(0); i < LAST_ID; i += FIRST_ID )
        {
        mrqData->AppendEntryLocalId( i );
        }
    RArray<TUint32> entryLocalIdArray;   
    entryLocalIdArray = mrqData->EntryLocalIdArray();
    
    TInt j(0);
    for ( TUint32 i(0); i < LAST_ID; i += FIRST_ID )
        {
        if( entryLocalIdArray[j++] != i )
            {
            error = KErrCancel;
            }
        else
            {
            ;
            }
        } 
    if( error != KErrNone )
        {
        _LIT( KError, "CMeetingRequestData::AppendEntryLocalId/EntryLocalIdArray failed");
        iLog->Log(KError);
        }
    else
        {
        _LIT( KError, "CMeetingRequestData::SAppendEntryLocalId,EntryLocalIdArray passed");
        iLog->Log(KError);
        }
    delete mrqData;
    return error;
    
    }


// -----------------------------------------------------------------------------
// CSendUIMeetingRequestAPITest::TestMRqDataMailHeaderL
// This function tests SetMailHeaderMethodL and MailHeaderMethod of --
// -- CMeetingRequestData
// -----------------------------------------------------------------------------
//
TInt CSendUIMeetingRequestAPITest::TestMRqDataMailHeaderL(
        CStifItemParser& aItem )
    {
    
    CMeetingRequestData* mrqData = CMeetingRequestData::NewLC();
    TInt error = KErrNone;
    _LIT8 (KMethodType, "Method1");  
    _LIT8 (KMethodType2, "Method2");  
    _LIT8 (KResultText, "Method1Method2");  
    
    TRAPD( err, mrqData->SetMailHeaderMethodL( KMethodType));
    if( err != KErrNone )
        {
        _LIT( KError, "CMeetingRequestData::SetMailHeaderMethodL leaves with %d code");
        iLog->Log( KError, err );
        error = err;
        }
    TRAP( err, mrqData->SetMailHeaderMethodL( KMethodType2));
    if( err != KErrNone )
        {
        _LIT( KError, "CMeetingRequestData::SetMailHeaderMethodL leaves with %d code");
        iLog->Log( KError, err );
        error = err;
        }
    else
        {
        _LIT( KError, "CMeetingRequestData::SetMailHeaderMethodL Passed");
        iLog->Log( KError );        
        }
    
    TBuf8<256> testMethod;
    testMethod = mrqData->MailHeaderMethod();
    if( testMethod != KResultText )
        {
        _LIT( KError, "CMeetingRequestData::MailHeaderMethod Failed");
        iLog->Log( KError );         
        error = KErrCancel;
        }  
    else
        {
        _LIT( KError, "CMeetingRequestData::MailHeaderMethod Passed");
        iLog->Log( KError );        
        }
    CleanupStack::PopAndDestroy();//mrqData
    return error;
    
    }

// -----------------------------------------------------------------------------
// CSendUIMeetingRequestAPITest::TestReadFromBufferL
// This function tests ReadFromBufferL of --
// -- CMeetingRequestData
// -----------------------------------------------------------------------------
//
TInt CSendUIMeetingRequestAPITest::TestReadFromBufferL(
        CStifItemParser& aItem )
    {
    iLog->Log( _L("TestReadFromBufferL-Entry"));
    //Creating CMeetingRequestData Object
    CMeetingRequestData* mrqData = CMeetingRequestData::NewLC();    
    mrqData->AppendEntryLocalId( FIRST_ID );
    
    //Creating a stream buffer for ReadFromBufferL
    TInt bufLength = 0;
    TMsvId testMailBoxId(MAILBOX_ID);
    bufLength += sizeof( testMailBoxId );
    TUint64 testFileId(FILE_ID);
    bufLength += sizeof( testFileId );
    bufLength += KMethodTypeMaxLength;
    bufLength += sizeof( TInt );
    RArray<TUint32> entryLocalIdArray;   
    entryLocalIdArray = mrqData->EntryLocalIdArray();     
    bufLength += entryLocalIdArray.Count() * sizeof( TUint32 );
    
    HBufC8* buf = HBufC8::NewLC( bufLength );
    TPtr8 des = buf->Des();
    RDesWriteStream stream( des );
    
    stream << testMailBoxId;
    stream.WriteReal64L( testFileId );
    
    _LIT8 (KMethodType, "Method1");
    stream << KMethodType;
  
    TInt count = entryLocalIdArray.Count();
    stream.WriteInt16L(count);
        
    for ( TInt i = 0; i < count; ++i )
        {
        stream.WriteUint32L(entryLocalIdArray[i]);
        }
    stream.Close();
    
    //Test ReadFromBufferL
    TRAPD( err, mrqData->ReadFromBufferL( des ) );
    if( err != KErrNone )
        {
        _LIT( KError, "ReadFromBufferL leaves with %d error ");
        iLog->Log( KError, err );
        }
    else
        {
        if( mrqData->Mailbox() == testMailBoxId )
            {
            _LIT( KError, "CMeetingRequestData::ReadFromBufferL Passed" );
            iLog->Log( KError );
            }
        }
    CleanupStack::PopAndDestroy( buf );   
    CleanupStack::PopAndDestroy( mrqData );
    iLog->Log( _L("TestReadFromBufferL-Exit"));
    return err;
    
    }

// -----------------------------------------------------------------------------
// CSendUIMeetingRequestAPITest::TestWriteToBufferLC
// This function tests WriteToBufferLC of --
// -- CMeetingRequestData
// -----------------------------------------------------------------------------
//
TInt CSendUIMeetingRequestAPITest::TestWriteToBufferLC(
        CStifItemParser& aItem )
    {
    iLog->Log( _L("TestWriteToBufferLC-Entry"));
    //Creating CMeetingRequestData Object
    CMeetingRequestData* mrqData = CMeetingRequestData::NewLC();
    mrqData->AppendEntryLocalId( FIRST_ID );
    
    //Test WriteBufferLC
    HBufC8* buffer = mrqData->WriteToBufferLC();
    iLog->Log( _L("CMeetingRequestData::WriteToBufferLC passed"));
    CleanupStack::PopAndDestroy( 2 );
    iLog->Log( _L("TestWriteToBufferLC-Exit"));
    return KErrNone;
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
