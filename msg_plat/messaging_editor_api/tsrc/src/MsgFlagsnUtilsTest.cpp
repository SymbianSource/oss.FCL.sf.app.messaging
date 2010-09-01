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
* Description:   This file definitions of tests to MsgEditorFlags and MsgAttachmentUtils
*
*/


#include "MessagingEditorAPITest.h"
#include "MsgFlagsnUtilsTest.h"
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <f32file.h>
#include <e32cmn.h>
#include <e32std.h>
#include <badesca.h>
#include <MsgAttachmentUtils.h>
#include <coemain.h>
#include <MsgEditorAppUi.rsg>
#include <MsgEditorCommon.h>
#include <MsgEditorFlags.h>
#include <s32file.h>
#include <stringloader.h>
#include <e32des16.h>
#include <MSVSTORE.H>
#include <S32MEM.H>


//Global declarations 
_LIT( KSuccess, "This API's check is a Success" );
_LIT( KFailure, "This API's check is a Failure" );


CMsgFlagsnUtilsTest::CMsgFlagsnUtilsTest( CStifLogger* aLog ): iLog( aLog )
    {
    }

CMsgFlagsnUtilsTest::~CMsgFlagsnUtilsTest()
    {
    }

// -----------------------------------------------------------------------------
// CMsgFlagsnUtilsTest::MsgFlagsDefCtor
// Test to cover -- TMsgEditorFlags::TMsgEditorFlags
//
// Default Constructor.
// -----------------------------------------------------------------------------
//
TInt CMsgFlagsnUtilsTest::MsgFlagsDefCtor()
    {
    _LIT( KApi, "MsgFlagsDefCtor" );
    // Print to log file
    iLog->Log( KApi );    
    //Calling the default Ctor -- TMsgEditorFlags::TMsgEditorFlags
    
    TMsgEditorFlags editorFlags;
    
    //Checking the constructor's correctness
    if( editorFlags.IsSet(TUint32(4294967295LL)) )    //4294967295 is decimal value of FFFFFFFF ; expected result is FALSE
    {
        iLog->Log( KFailure );
        return KErrGeneral;
    }
    else
    {
        iLog->Log( KSuccess );
        return KErrNone;
    
    }
   
  }
  
// -----------------------------------------------------------------------------
// CMsgFlagsnUtilsTest::MsgFlagsArgCtor
// Test to cover -- TMsgEditorFlags::TMsgEditorFlags(TUint32 aFlags)
//
// One Argument Constructor.
// -----------------------------------------------------------------------------
//


TInt CMsgFlagsnUtilsTest::MsgFlagsArgCtor()
    {
    _LIT( KApi, "In MsgFlagsArgCtor" );
    // Print to log file
    iLog->Log( KApi );
    
    TUint32 ini=4;
    
    //Calling the One Argument Constructor -- TMsgEditorFlags::TMsgEditorFlags(TUint32 aFlags)
    TMsgEditorFlags editorFlags(ini);
    
    //Checking the constructor's correctness
    if( editorFlags.IsSet(4294967291LL) )    //4294967291 is decimal value of FFFFFFFB ; expected result is FALSE
        {
            iLog->Log( KFailure );
            return KErrGeneral;
        }
    else
        {
            iLog->Log( KSuccess );
            return KErrNone;
        }
   }

// -----------------------------------------------------------------------------
// CMsgFlagsnUtilsTest::MsgFlagsCopyCtor
// Test to cover -- TMsgEditorFlags::TMsgEditorFlags(const TMsgEditorFlags& aFlags)
//
// Copy Constructor.
// -----------------------------------------------------------------------------
//

  TInt CMsgFlagsnUtilsTest::MsgFlagsCopyCtor()
    {
    _LIT( KApi, "In MsgFlagsCopyCtor" );
    // Print to log file
    iLog->Log( KApi );
    
    TUint32 ini=4;
    
    //Calling the One Argument Constructor
    TMsgEditorFlags editorFlags(ini);
    
    //Calling the Copy Constructor TMsgEditorFlags::TMsgEditorFlags(const TMsgEditorFlags& aFlags)
    TMsgEditorFlags editorFlagsa(editorFlags);
    
    //Checking the constructor's correctness
    if( editorFlagsa.IsSet(4294967291LL) )    //4294967291 is decimal value of FFFFFFFB ; expected result is FALSE
        {
            iLog->Log( KFailure );
            return KErrGeneral;
        }
    else
        {
            iLog->Log( KSuccess );
            return KErrNone;
        }    
    
  }
  
  // -----------------------------------------------------------------------------
  // CMsgFlagsnUtilsTest::SetFlag
  // Test to cover -- TMsgEditorFlags::Set(TUint32 aFlag)
  //
  // Sets the given flags 
  // -----------------------------------------------------------------------------
  //
  
  TInt CMsgFlagsnUtilsTest::SetFlag()
    {
    _LIT( KApi, "In SetFlag" );
    // Print to log file
    iLog->Log( KApi );
    
    TUint32 ini=4;
    TUint32 flag=6;
    
    //Calling the One Argument Constructor
    TMsgEditorFlags editorFlags(ini);
    
    //Calling the TMsgEditorFlags::Set(TUint32 aFlag)
    editorFlags.Set(flag);
    
    //Checking the SetFlag's correctness
    if( editorFlags.IsSet(4294967289LL) )    //4294967289 is decimal value of FFFFFFF9 ; expected result is FALSE
        {
            iLog->Log( KFailure );
            return KErrGeneral;
        }
    else
        {
            iLog->Log( KSuccess );
            return KErrNone;
        }
  }
  
  // -----------------------------------------------------------------------------
  // CMsgFlagsnUtilsTest::Clear
  // Test to cover -- TMsgEditorFlags::Clear(TUint32 aFlag)
  //
  // Clears given flags.
  // -----------------------------------------------------------------------------
  
  TInt CMsgFlagsnUtilsTest::Clear()
    {
    _LIT( KApi, "In Clear" );
    // Print to log file
    iLog->Log( KApi );
    
    TUint32 ini=3;
    TUint32 flag=4;
    
    //Calling the One Argument Constructor
    TMsgEditorFlags editorflags(ini);
    
    //Calling the TMsgEditorFlags::Clear(TUint32 aFlag)
    editorflags.Clear(flag);
    
    //Checking the Clear's correctness
    if( editorflags.IsSet(4294967292LL) )    //4294967292 is decimal value of FFFFFFFC ; expected result is FALSE
        {
        iLog->Log( KFailure );
        return KErrGeneral;
        }
    else
        {
        iLog->Log( KSuccess );
        return KErrNone;        
        }
  }
  
  // -----------------------------------------------------------------------------
  // CMsgFlagsnUtilsTest::IsSet
  // Test to cover -- TMsgEditorFlags::IsSet
  //
  // Returns ETrue, if all flags defined in aFlag parameter are set.
  // -----------------------------------------------------------------------------
  
  
TInt CMsgFlagsnUtilsTest::IsSet()
    {
    _LIT( KApi, "In IsSet" );
    // Print to log file
    iLog->Log( KApi );
    
    TUint32 ini=4;
        
    //Calling the One Argument Constructor -- TMsgEditorFlags::TMsgEditorFlags(TUint32 aFlags)
        TMsgEditorFlags editorFlags(ini);
        
    //Checking the IsSet's correctness
    if( editorFlags.IsSet(4294967291LL) )    //4294967291 is decimal value of FFFFFFFB ; expected result is FALSE
       {
                iLog->Log( KFailure );
                return KErrGeneral;
       }
    else
       {
                iLog->Log( KSuccess );
                return KErrNone;
       }
    
  }
  
// -----------------------------------------------------------------------------
// CMsgFlagsnUtilsTest::EqualToOperator
// Test to cover -- TMsgEditorFlags::operator=
//
// Assignment operator.
// -----------------------------------------------------------------------------

TInt CMsgFlagsnUtilsTest::EqualToOperator()
    {
    _LIT( KApi, "In EqualToOperator" );
    // Print to log file
    iLog->Log( KApi );
    
    TUint32 ini=4;
    
    //Calling the One Argument Constructor
    TMsgEditorFlags editorFlags(ini);
    
    TMsgEditorFlags editorFlagsa;
    
    //Calling the TMsgEditorFlags::operator=
       
    editorFlags=editorFlags;
    
    editorFlagsa=editorFlags;
    
    if( editorFlagsa.IsSet(4294967291LL) )    //4294967291 is decimal value of FFFFFFFB ; expected result is FALSE
       {
                    iLog->Log( KFailure );
                    return KErrGeneral;
       }
    else
       {
                    iLog->Log( KSuccess );
                    return KErrNone;
       }
    
}
    
// -----------------------------------------------------------------------------
// CMsgFlagsnUtilsTest::ExternalizenInternalizeL
// Tests to cover -- TMsgEditorFlags::ExternalizeL(RWriteStream& aStream) and
//                -- TMsgEditorFlags::InternalizeL
// Write flags into given stream.
// -----------------------------------------------------------------------------

TInt CMsgFlagsnUtilsTest::ExternalizenInternalizeL()
    {
    _LIT( KApi, "In WriteStreamL, ExternalizeL" );
    // Print to log file
    iLog->Log( KApi );   
    
    TUint32 ini=4;    
    //Calling the One Argument Constructor
    TMsgEditorFlags editorFlags(ini);
    RBufWriteStream writeStream;

    CBufFlat* buffer = CBufFlat::NewL( 1024 );
    CleanupStack::PushL( buffer );
    //Open the Stream
    writeStream.Open( *buffer );
    TRAPD(err,editorFlags.ExternalizeL(writeStream));    
    //Comit and Close the Write Stream
    writeStream.CommitL();
    writeStream.Close();    
    if( err != KErrNone )
        {
        return err;
        }
    
    _LIT( KApi2, "In WriteStreamL, InternalizeL" );
    iLog->Log( KApi2 );
    RBufReadStream readStream;
    readStream.Open(*buffer);
    //Calling the TMsgEditorFlags::InternalizeL(RReadStream& aStream
    TRAP(err,editorFlags.InternalizeL(readStream));
    //Release and Close the Read Stream
    readStream.Release();
    readStream.Close();
    CleanupStack::PopAndDestroy();
    if( err != KErrNone )
        {
        return err;
        }
    
    return KErrNone;
    
  }
 
// -----------------------------------------------------------------------------
// CMsgFlagsnUtilsTest::ThreePmtrsFetch
// Test to cover -- MsgAttachmentUtils::FetchFileL(TMsgAttachmentFetchType aType,TFileName& aFileName,TBool aCheckDiskSpace)
//
// Fetches an attachment of given type from external application.
// -----------------------------------------------------------------------------


TInt CMsgFlagsnUtilsTest::ThreePmtrsFetchL()
    {
    _LIT( KApi, "In ThreePmtrsFetch" );
    // Print to log file
    iLog->Log( KApi );   
    
    TBool checkDiskSpace=EFalse;
    
    TFileName* fileName = new( ELeave ) TFileName;
    CleanupStack::PushL( fileName );
    
    //Calling the MsgAttachmentUtils::FetchFileL(TMsgAttachmentFetchType aType,TFileName& aFileName,TBool aCheckDiskSpace) 
    //MsgAttachmentUtils::TMsgAttachmentFetchType fetchType=EImage;
    TRAPD( err, MsgAttachmentUtils::FetchFileL(MsgAttachmentUtils::EImage,*fileName,checkDiskSpace));
    if( err != KErrNone )
        {
        CleanupStack::PopAndDestroy( fileName );
        return err;
        }
    
    CleanupStack::PopAndDestroy( fileName );    
    return KErrNone;
 }

// -----------------------------------------------------------------------------
// CMsgFlagsnUtilsTest::FourPmtrsFetchL
// Test to cover -- MsgAttachmentUtils::FetchFileL(TMsgAttachmentFetchType aType,TFileName& aFileName,const TUid& aAppUid,TBool aCheckDiskSpace = EFalse)
//
// Fetches an attachment of given type from external application.
// -----------------------------------------------------------------------------
  
  TInt CMsgFlagsnUtilsTest::FourPmtrsFetchL()
    {
    _LIT( KApi, "In FourPmtrsFetchL" );
    // Print to log file
    iLog->Log( KApi );   
    
    //MsgAttachmentUtils::TMsgAttachmentFetchType fetchType=EImage;
    TBool checkDiskSpace=EFalse;
    
    TFileName* fileName = new( ELeave ) TFileName;
    CleanupStack::PushL( fileName );
    
    
    //Calling the MsgAttachmentUtils::FetchFileL(TMsgAttachmentFetchType aType,TFileName& aFileName,const TUid& aAppUid,TBool aCheckDiskSpace = EFalse)     
    TRAPD (err, MsgAttachmentUtils::FetchFileL(MsgAttachmentUtils::EImage,*fileName,KNullUid,checkDiskSpace));
    
    if( err != KErrNone )
            {
            CleanupStack::PopAndDestroy( fileName );
            return err;
            }
    CleanupStack::PopAndDestroy( fileName );  
    return KErrNone;
    
  }
  
  // -----------------------------------------------------------------------------
  // CMsgFlagsnUtilsTest::SixPmtrsFetchL
  // Test to cover -- MsgAttachmentUtils::FetchFileL(TMsgAttachmentFetchType aType,TFileName& aFileName,CDesCArray& aSelectedFiles,TBool aCheckDiskSpace = EFalse,TBool aMultiSelect = EFalse,MMGFetchVerifier*  aVerifier = NULL )
  //
  // Fetches an attachment of given type from external application.
  // -----------------------------------------------------------------------------  
  
  
  TInt CMsgFlagsnUtilsTest::SixPmtrsFetchL()
    {
    _LIT( KApi, "In SixPmtrsFetchL" );
    // Print to log file
    iLog->Log( KApi );  
    //MsgAttachmentUtils::TMsgAttachmentFetchType fetchType=EImage;
    TBool checkDiskSpace=EFalse;
    
    TFileName* fileName = new( ELeave ) TFileName;
    CleanupStack::PushL( fileName );
    
    CDesCArrayFlat* files = new ( ELeave ) CDesC16ArrayFlat( 1 );
    CleanupStack::PushL( files );
            
    // calling the MsgAttachmentUtils::FetchFileL(TMsgAttachmentFetchType aType,TFileName& aFileName,CDesCArray& aSelectedFiles,TBool aCheckDiskSpace = EFalse,TBool aMultiSelect = EFalse,MMGFetchVerifier*  aVerifier = NULL )
    TRAPD (err, MsgAttachmentUtils::FetchFileL(MsgAttachmentUtils::EImage,*fileName,*files,checkDiskSpace,EFalse,NULL));
    
    if( err != KErrNone )
                {
                CleanupStack::PopAndDestroy(files );
                CleanupStack::PopAndDestroy( fileName );
                return err;
                }
    
    CleanupStack::PopAndDestroy(files); //files
    CleanupStack::PopAndDestroy(fileName); //filename
    
    
    return KErrNone;
        
  }
  // -----------------------------------------------------------------------------
  // CMsgFlagsnUtilsTest::SixPmtrsParamListFetchL
  // Test to cover -- MsgAttachmentUtils::FetchFileL(TMsgAttachmentFetchType aType,CAiwGenericParamList* aParams,CDesCArray& aSelectedFiles,TBool aCheckDiskSpace = EFalse,TBool aMultiSelect = EFalse,MMGFetchVerifier*  aVerifier = NULL )
  //
  // Fetches an attachment of given type from external application.
  // -----------------------------------------------------------------------------  
  
    TInt CMsgFlagsnUtilsTest::SixPmtrsParamListFetchL()
    {
    _LIT( KApi, "In SixPmtrsParamListFetchL" );
    // Print to log file
    iLog->Log( KApi );  
    //MsgAttachmentUtils::TMsgAttachmentFetchType fetchType=EImage;
    TBool checkDiskSpace=EFalse;
    
    CDesCArrayFlat* files = new ( ELeave ) CDesC16ArrayFlat( 1 );
    CleanupStack::PushL( files );
    
    CFetchVerifier* verifier=new ( ELeave ) CFetchVerifier();
            
    //calling the MsgAttachmentUtils::FetchFileL(TMsgAttachmentFetchType aType,CAiwGenericParamList* aParams,CDesCArray& aSelectedFiles,TBool aCheckDiskSpace = EFalse,TBool aMultiSelect = EFalse,MMGFetchVerifier*  aVerifier = NULL )
    TRAPD (err, MsgAttachmentUtils::FetchFileL(MsgAttachmentUtils::EImage,*files,NULL,checkDiskSpace,EFalse,verifier));
    
    if( err != KErrNone )
		{
		CleanupStack::PopAndDestroy(files );
		return err;
		}
    
    CleanupStack::PopAndDestroy(files );
    
    return KErrNone;
        
  }
    
    // -----------------------------------------------------------------------------
    // CMsgFlagsnUtilsTest::FetchAnyFileL
    // Test to cover -- MsgAttachmentUtils::FetchAnyFileL(TFileName& aFileName,CCoeEnv& aCoeEnv,MAknFileSelectionObserver* aVerifier = NULL,MAknFileFilter* aFilter = NULL 
    //
    // Fetches an any attachment of given type from external application.
    // -----------------------------------------------------------------------------  
  
 TInt CMsgFlagsnUtilsTest::FetchAnyFileL()
    {
    _LIT( KApi, "In FetchAnyFileL" );
    // Print to log file
    iLog->Log( KApi );  
    //MsgAttachmentUtils::TMsgAttachmentFetchType fetchType=EImage;
//    TBool checkDiskSpace=EFalse; //for ARM Warnings
    
    //CCoeEnv* environ=new ( ELeave ) CCoeEnv();
    
    TFileName* fileName = new( ELeave ) TFileName;
    CleanupStack::PushL( fileName );
    
    //calling the MsgAttachmentUtils::FetchAnyFileL(TFileName& aFileName,CCoeEnv& aCoeEnv,MAknFileSelectionObserver* aVerifier = NULL,MAknFileFilter* aFilter = NULL 
    TRAPD (err, MsgAttachmentUtils::FetchAnyFileL(*fileName,*( CCoeEnv::Static() ),NULL,NULL));
    
    if( err != KErrNone )
                    {
                    CleanupStack::PopAndDestroy( fileName );
                    return err;
                    }
    
    CleanupStack::PopAndDestroy( fileName );
    
    return KErrNone;
    
   }
 
 // -----------------------------------------------------------------------------
 // CMsgFlagsnUtilsTest::GetFileNameFromBuffer
 // Test to cover -- MsgAttachmentUtils::GetFileNameFromBuffer(TFileName& ,const TDesC&, TInt, const TDesC& )
 //
 // -----------------------------------------------------------------------------  
 TInt CMsgFlagsnUtilsTest::GetFileNameFromBuffer()
     {
     _LIT( KApi, "In GetFileNameFromBuffer" );
     // Print to log file
     iLog->Log( KApi );  
     _LIT( KFileName, "Terve! Mitä kuul.txt");
     TFileName fileNameOut;
     TFileName fileName(KFileName);
     TBuf<64> buffer = _L("  Terve...! Mitä kuuluu? T: <hemmo> :-)  ");
     TBuf<4>   ext = _L(".txt");
     TInt      max = 20;
     MsgAttachmentUtils::GetFileNameFromBuffer (fileNameOut, buffer, max, &ext);
     if( fileNameOut == fileName )
         {
         return KErrNone;    
         }
     else
         {
         return KErrCancel;
         }
     }
 
 // -----------------------------------------------------------------------------
 // CMsgFlagsnUtilsTest::GetMsgEditorTempPath
 // Test to cover -- MsgAttachmentUtils::GetMsgEditorTempPath(TFileName& aTempPath,const TDesC* aFileName = NULL)
 //
 // Returns temp path used by msg editors and optionally
 // appends file name to it.
 // -----------------------------------------------------------------------------  
   
   TInt CMsgFlagsnUtilsTest::GetMsgEditorTempPath()
   {
    _LIT( KApi, "In GetMsgEditorTempPath" );
    _LIT( KFileName, "abc.dat" );
    // Print to log file
    iLog->Log( KApi );  
    
    TFileName temppath;
    
    TFileName tempfilename(KFileName);
    
    //calling the MsgAttachmentUtils::GetMsgEditorTempPath(TFileName& aTempPath,const TDesC* aFileName = NULL)
    MsgAttachmentUtils::GetMsgEditorTempPath(temppath);
    
    return KErrNone;    
   }
   
