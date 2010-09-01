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
* Description:   TestSmilEngineBlocks implementation*
*/




#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <smiltime.h>
#include <smilobject.h>
#include "smilpresentation.h"
#include "TestSmilEngine.h"
#include <smilplayerinterface.h>
#include "smiltransitionfilter.h"
#include "smilparser.h"
#include <f32file.h>
#include <SmilSyncParse.h>
#include "ConformanceChecker.h"
#include <smilanchor.h>
/*START for SMIL Player Dialog Creation*/
#include <smilplayerdialog.h>
#include <MMediaFactoryFileInfo.h>
#include <COEAUI.H>
#include <aknappui.h>
#include <eikappui.h>
#include <eikbtgpc.h>
#include <eikdialg.h>

#define iEikonEnv   (CEikonEnv::Static())
class CDummyMediaFactoryFileInfo : public CBase, 
                       public MMediaFactoryFileInfo
   {
   virtual void GetFileHandleL( const TDesC& aUrl, RFile& aFile )
       {
           _LIT(KPath,"c:\\data\\Others\\");
           TBuf<96> path(KPath);
           path.Append(aUrl); 

           User::LeaveIfError(aFile.Open(fs, path, EFileShareAny | EFileRead));
       }
   virtual TPtrC8 GetMimeTypeL( const TDesC& aUrl )
       {
           TDataType iDataType = KNullDesC8();
           
           iDataType = _L8( "image/jpg" );
  
           return iDataType.Des8();
       }
   RFs fs;
 
   virtual TUint GetCharacterSetL( const TDesC& aUrl )
       {
        return 0;
       }
public:
  void ConstructL()
       {
           User::LeaveIfError(fs.Connect());
           User::LeaveIfError(fs.ShareProtected());
       }
   };
/*END for SMIL Player Dialog Creation*/

_LIT( KSmilEngine, "SmilEngine" );



void CTestSmilEngine::Delete() 
    {

    }

 

// -----------------------------------------------------------------------------
// CTestSmilEngine::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestSmilEngine::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "TSmilTime::TSmilTime", CTestSmilEngine::CtorTIntArg ),
        
        ENTRY( "TSmilTime::TSmilTime()", CTestSmilEngine::DefaultCtor ),
        
        ENTRY( "TSmilTime::TSmilTime(TDesC&)",CTestSmilEngine::CtorSmilTimeArg),
              
        ENTRY( "TSmilTime::Operator==", CTestSmilEngine::EqualityOptr ),
        
        ENTRY( "TSmilTime::Operator!=", CTestSmilEngine::NotEqualityOptr ),
        
        ENTRY( "TSmilTime::Operator*", CTestSmilEngine::StarOptr ),
        
        ENTRY( "TSmilTime::Operator*(TReal32)", CTestSmilEngine::StarRealOptr ),
        
        ENTRY( "TSmilTime::Operator-", CTestSmilEngine::MinusOptr ),
        
        ENTRY( "TSmilTime::Operator>", CTestSmilEngine::GreaterThanOptr ),
        
        ENTRY( "TSmilTime::Operator<", CTestSmilEngine::LessThanOptr ),
        
        ENTRY( "TSmilTime::Operator>=", CTestSmilEngine::GtrEqualThanOptr ),
        
        ENTRY( "TSmilTime::Operator<=", CTestSmilEngine::LessThanEqualOptr ),
        
        ENTRY( "TSmilTime::Operator++", CTestSmilEngine::PlusPlusOptr ),
        
        ENTRY( "TSmilTime::Operator=", CTestSmilEngine::EqualOptr ),
        
        ENTRY( "TSmilTime::Operator=(TInt)", CTestSmilEngine::TIntEqualOptr ),
        
        ENTRY( "TSmilTime::Operator+", CTestSmilEngine::PlusOptr ),
        
        ENTRY( "TSmilTime::ToMicroSeconds", CTestSmilEngine::ToMicroSeconds ),
        
        ENTRY( "TSmilTime::FromMicroSeconds", CTestSmilEngine::FromMicroSeconds ),
        
        ENTRY( "ConformanceCheck",CTestSmilEngine::ConformanceCheckL ), 
        ENTRY( "TSmilTime::IntFromMicroSeconds", CTestSmilEngine::IntFromMicroSeconds ),
        
		ENTRY( "CSmilTransitionFilter",CTestSmilEngine::CreateCSmilTransitionFilterL ),
		
        
		ENTRY( "SmilPresentation",CTestSmilEngine::SmilPresentationL ),
        
        ENTRY( "SmilParser",CTestSmilEngine::SmilParserL ), 
       
		ENTRY( "SmilUIPresentation",CTestSmilEngine::SmilUIPresentationL ),
		
    //    ENTRY( "SmilAnchor",CTestSmilEngine::SmilAnchorAPIs ),  
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }



// -----------------------------------------------------------------------------
// CTestSmilEngine::CtorTIntArg
// TestCase for TSmilTime::TSmilTime( TInt ) method
// -----------------------------------------------------------------------------
//


TInt CTestSmilEngine::CtorTIntArg( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KApiName, "In Ctor1ArgL" );
    _LIT( KSuccess, "This API is tested success" );
    _LIT( KFailure, "This API is tested failure" );
    _LIT( KFinished, "Going out of Ctor1ArgL" );
    TestModuleIf().Printf( 0, KSmilEngine, KApiName );
    // Print to log file
    iLog->Log( KApiName );
    TInt ini;
    aItem.GetNextInt(ini);
    
	// Creating TSmilTime object with 1 arguement
		TSmilTime time(ini) ;
		TSmilTime test(ini) ;
		
	// Verifying the TSmilTime::TSmilTime( TInt )
		
	if(time==test)
	    {
	        iLog->Log( KSuccess );
	        iLog->Log( KFinished );
	        
	        return KErrNone;
	    }
	else
	    {
	        iLog->Log( KFailure );
	        iLog->Log( KFinished );
	        
	        return KErrGeneral;
	    }
    }


// -----------------------------------------------------------------------------
// CTestSmilEngine::DefaultCtor
// TestCase for TSmilTime::TSmilTime() Constructor.
// -----------------------------------------------------------------------------
//


TInt CTestSmilEngine::DefaultCtor()
    {

    // Print to UI
    
    _LIT( KApiName, "In DefaultCtorL" );
    _LIT( KSuccess, "This API is tested success" );
    _LIT( KFailure, "This API is tested failure" );
    _LIT( KFinished, "Going out of DefaultCtorL" );
    TestModuleIf().Printf( 0, KSmilEngine, KApiName );
    // Print to log file
    iLog->Log( KApiName );
    
	// Creating TSmilTime object from default constructor
	TSmilTime time;
	
	TSmilTime test(0);
	
	//Verifying the TSmilTime::TSmilTime() method
	
	if(time==test)
	        {
	            iLog->Log( KSuccess );
	            iLog->Log( KFinished );
	            
	            return KErrNone;
	        }
	else
	        {
	            iLog->Log( KFailure );
	            iLog->Log( KFinished );
	            
	            return KErrGeneral;
	        }
    }

// -----------------------------------------------------------------------------
// CTestSmilEngine::DefaultCtorL
// TestCase for TSmilTime::TSmilTime(TDes&) Constructor
// -----------------------------------------------------------------------------
//

TInt CTestSmilEngine::CtorSmilTimeArg()
    {
    
    // Print to UI

        _LIT( KApiName, "In CtorSmilTime" );
        _LIT( KTime, "2s" );
        _LIT( KTimea, "-2s" );
        _LIT( KTimeh, "+2s" );
        _LIT( KTimeb, "2ms" );
        _LIT( KTimec, "2us" );
        _LIT( KTimed, "KIndef" );
        _LIT( KTimee, "KMedia" );
        _LIT( KTimef, "2min" );
        _LIT( KTimeg, "2h" );
        _LIT( KTimex, "2:30" );
        _LIT( KTimey, "1.30" );
        TestModuleIf().Printf( 0, KSmilEngine, KApiName );
        // Print to log file
        iLog->Log( KApiName );
        
        //Calling TSmilTime::TSmilTime(TDes&) for every scenario 
        
        TSmilTime time(KTime);
        
        TSmilTime timea(KTimea);
        
        TSmilTime timeb(KTimeb);
        
        TSmilTime timec(KTimec);
        
        TSmilTime timed(KTimed);
        
        TSmilTime timee(KTimee);
        
        TSmilTime timef(KTimef);

        TSmilTime timeh(KTimeh);

        TSmilTime timeg(KTimeg);

        TSmilTime timex(KTimex);

        TSmilTime timey(KTimey); 
        
        return KErrNone;
    
    }
    
    
   

// -----------------------------------------------------------------------------
// CTestSmilEngine::EqulityOptr
// TestCase for TSmilTime::operator== 
// -----------------------------------------------------------------------------
//    
TInt CTestSmilEngine::EqualityOptr()    
{
	// Print to UI
 
  _LIT( KApiName, "In EqulityOptr" );
  _LIT( KSuccess, "This API is tested success" );
  _LIT( KFailure, "This API is tested failure" );
  _LIT( KTimeA, "60s" );
  _LIT( KTimeB, "1min" );
  _LIT( KTimeC, "2min" );
  _LIT( KTimeD, "2h" );
  _LIT( KFinished, "Going out of EqualityOptr" );
  
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file 
  iLog->Log( KApiName );
    
	TSmilTime objectA(KTimeA);
	TSmilTime objectB(KTimeB);
    TSmilTime objectC(KTimeC);
    TSmilTime objectD(KTimeD);
	
	//Checking and covering the TSmilTime::Operator == method
	if (objectA==objectB)
	{
 
	    if (!(objectC==objectD))
	    {
	        iLog->Log( KSuccess );
	        iLog->Log( KFinished );
	        
	        return KErrNone;
	    }
	}
	
    else
    {
        iLog->Log( KFailure );
        iLog->Log( KFinished );
        
        return KErrGeneral;
    }	
	
}

// -----------------------------------------------------------------------------
// CTestSmilEngine::StarOptr
// TestCase for TSmilTime::Operator*.
// -----------------------------------------------------------------------------
//    
TInt CTestSmilEngine::StarOptr(CStifItemParser& aItem)    
{
	// Print to UI
    _LIT( KSuccess, "This API is tested success" );
    _LIT( KFailure, "This API is tested failure" );
    _LIT( KFinished, "Going out of Ctor1ArgL" );
  _LIT( KApiName, "In StarOptr" );
  
  
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
  
    TInt a,b;
    aItem.GetNextInt(a);
    aItem.GetNextInt(b);
    
    _LIT( KAvalue, "The value of a is %d" );
    _LIT( KBvalue, "The value of b is %d" );
    
    iLog->Log( KAvalue,a );
    
    iLog->Log( KBvalue,b );
    
	TSmilTime objectA(TSmilTime::KUnresolved);
	TSmilTime objectB(3);
	TSmilTime objectC;
	TSmilTime objectD;
	TSmilTime objectE(TSmilTime::KIndefinite);
	
	//Multiplying with KUnresolved operand
	objectC=objectA*objectB;
	
	//Multiplying with value 0 operand
	objectC=objectB*objectD;
	
	//Multiplying with KIndefinite operand
	objectC=objectB*objectE;
	
	//Multiplying with valid operand
	TSmilTime objectF(a);
	TSmilTime objectG(b);
  
	objectC=objectF*objectG;
	
	_LIT( KCvalue, "The value of c is %d" );
	    
	iLog->Log( KCvalue,objectC );
	
  //Verifying the TSmilTime::Operator*
	
	TSmilTime test(6);
	if(objectC==test)
      {
          iLog->Log( KSuccess );
          iLog->Log( KFinished );
          
          return KErrNone;
      }
    else
      {
          iLog->Log( KFailure );
          iLog->Log( KFinished );
          
          return KErrGeneral;
      }
 
}

// -----------------------------------------------------------------------------
// CTestSmilEngine::NequlityOptr
// TestCase for TSmilTime::operator!= 
// -----------------------------------------------------------------------------
// 
TInt CTestSmilEngine::NotEqualityOptr()    
{
	// Print to UI

  _LIT( KApiName, "In EqulityOptr" );
  _LIT( KSuccess, "This API is tested success" );
  _LIT( KFailure, "This API is tested failure" );
  _LIT( KFinished, "Going out of NequlityOptr" );
  _LIT( KTimeA, "3min" );
  _LIT( KTimeB, "120s" );
  _LIT( KTimeC, "3min" );
  _LIT( KTimeD, "180s" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
    
	TSmilTime objectA(KTimeA);
	TSmilTime objectB(KTimeB);
    TSmilTime objectC(KTimeC);
    TSmilTime objectD(KTimeD);	
	
	//  //Verifying the TSmilTime::Operator!=
	if (objectA!=objectB)
	{
	    if (!(objectC!=objectD))
	    {
	        iLog->Log( KSuccess );
	        iLog->Log( KFinished );
	        
	        return KErrNone;

	    }
	}
	

	else
	{
	    iLog->Log( KFailure );
	    iLog->Log( KFinished );
	    
	    return KErrGeneral;
	    
	}

}
// -----------------------------------------------------------------------------
// CTestSmilEngine::StarRealOptr
// TestCase for TSmilTime::operator* (TReal32) 
// -----------------------------------------------------------------------------
//  

TInt CTestSmilEngine::StarRealOptr()    
{
	// Print to UI

  _LIT( KApiName, "In StarRealOptr" );
  _LIT( KSuccess, "This API is tested success" );
      _LIT( KFailure, "This API is tested failure" );
      _LIT( KFinished, "Going out of DefaultCtorL" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
  
    
	TSmilTime objectA(2);
	TSmilTime objectC;
	TReal32 realB(-12346);
	TReal32 realC(-4);
	TReal32 realD(KMaxTReal32);
	TSmilTime objectB;
	
	//Multiplying with KUnresolved operand
	objectC=objectA*realB;
	
  //Multiplying with value 0 operand
	objectC=objectB*realC;
	
	//Multiplying with KIndefinite operand
	objectC=objectA*realD;
	
	//Multiplying with valid operands
	objectC=objectA*realC;
	
	TSmilTime test(-8);
	
  //Verifying the TSmilTime::Operator*(TReal32)
	
    if(objectC==test)
        {
            iLog->Log( KSuccess );
            iLog->Log( KFinished );
            
            return KErrNone;
        }
    else
        {
            iLog->Log( KFailure );
            iLog->Log( KFinished );
            
            return KErrGeneral;
        }

}

// -----------------------------------------------------------------------------
// CTestSmilEngine::MinusOptr
// TestCase for TSmilTime::operator- 
// -----------------------------------------------------------------------------
//  

TInt CTestSmilEngine::MinusOptr(CStifItemParser& aItem)    
{
	// Print to UI

  _LIT( KApiName, "In MinusOptr" );
  _LIT( KSuccess, "This API is tested success" );
      _LIT( KFailure, "This API is tested failure" );
      _LIT( KFinished, "Going out of DefaultCtorL" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
  
  TInt a,b;
  aItem.GetNextInt(a);
  aItem.GetNextInt(b);
    
	TSmilTime objectA(TSmilTime::KUnresolved);
	TSmilTime objectB(3);
	TSmilTime objectC;
	TSmilTime objectD(TSmilTime::KIndefinite);
	TSmilTime objectE(2);
	
	//Subtracting with KUnresolved operand
	objectC=objectA-objectB;
	
	//Subtracting with KIndefinite operand
	objectC=objectD-objectE;
	
	TSmilTime objectF(a);
    TSmilTime objectG(b);
  
    //Subtracting valid operands
    objectC=objectF-objectG;
  
    TSmilTime test(5);
  
     //Verifying the TSmilTime::Operator-
    if(objectC==test)
          {
              iLog->Log( KSuccess );
              iLog->Log( KFinished );
              
              return KErrNone;
          }
    else
          {
              iLog->Log( KFailure );
              iLog->Log( KFinished );
              
              return KErrGeneral;
          }
   
}

// -----------------------------------------------------------------------------
// CTestSmilEngine::EqulityOptr
// TestCase for TSmilTime::operator > 
// -----------------------------------------------------------------------------
//    
TInt CTestSmilEngine::GreaterThanOptr()    
{
	// Print to UI

  _LIT( KApiName, "In GreaterThanOptr" );
  _LIT( KSuccess, "This API is tested success" );
  _LIT( KFailure, "This API is tested failure" );
  _LIT( KFinished, "Going out of DefaultCtorL" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
    
	TSmilTime objectA(5);
	TSmilTime objectB(3);
    TSmilTime objectC(3);
    TSmilTime objectD(5);
    
    //Verifying the TSmilTime::Operator>
	if (objectA>objectB)
	{
        if (!(objectC>objectD))
            {
            iLog->Log( KSuccess );
            iLog->Log( KFinished );
            
            return KErrNone;
            }
		
	}
	
	else
	    {  
	        iLog->Log( KFailure );
	        iLog->Log( KFinished );
	        
	        return KErrNone;
	        
	    }

	}
// -----------------------------------------------------------------------------
// CTestSmilEngine::GtrEqualthanOptr
// TestCase for TSmilTime::operator >= 
// -----------------------------------------------------------------------------
//    
TInt CTestSmilEngine::GtrEqualThanOptr()    
{
	// Print to UI

  _LIT( KApiName, "In GtrEqualthanOptr" );
  _LIT( KGreater, "First object is greater or equal  than the second" );
  _LIT( KNGreater, "First object is not greater or equal than the second" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
    
	TSmilTime objectA(5);
	TSmilTime objectB(5);
	
  //Checking if the first object is greter than or equal to the second
  if (objectA>=objectB)
	{
		iLog->Log( KGreater );
	}
	
	TSmilTime objectC(3);
	TSmilTime objectD(5);

	  //Verifying the TSmilTime::Operator>=
	
	if (!(objectC>=objectD))
	{
		iLog->Log( KNGreater );
		return KErrNone;
	}
	else
	{
	    return KErrGeneral;
	}
	
}
// -----------------------------------------------------------------------------
// CTestSmilEngine::LessThanOptr
// TestCase for TSmilTime::operator<
// -----------------------------------------------------------------------------
// 
TInt CTestSmilEngine::LessThanOptr()    
{
	// Print to UI

  _LIT( KApiName, "In LessThanOptr" );
  _LIT( KLesser, "First object is lesser than the second" );
  _LIT( KNLesser, "First object is not lesser than the second" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
    
	TSmilTime objectA(3);
	TSmilTime objectB(5);
	
	//Checking if the first object is lesser or not
	if (objectA<objectB)
	{
		iLog->Log( KLesser );
	}
	
	TSmilTime objectC(5);
	TSmilTime objectD(3);
	
	  //Verifying the TSmilTime::Operator<
	if (!(objectC<objectD))
	{
		iLog->Log( KNLesser );
		return KErrNone;
	}
    else
    {
        return KErrGeneral;
    }
	
}
// -----------------------------------------------------------------------------
// CTestSmilEngine::LessThanEqualOptr
// TestCase for TSmilTime:: operator<=
// -----------------------------------------------------------------------------
//
TInt CTestSmilEngine::LessThanEqualOptr()    
{
	// Print to UI

  _LIT( KApiName, "In LessThanOptr" );
  _LIT( KLesser, "First object is lesser or equal to the second" );
  _LIT( KNLesser, "First object is not lesser or equal to the second" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
    
	TSmilTime objectA(3);
	TSmilTime objectB(3);
	
	  //Verifying the TSmilTime::Operator<=
	if (objectA<=objectB)
	{
		iLog->Log( KLesser );
	}
	
	TSmilTime objectC(5);
	TSmilTime objectD(3);
	
	//Checking if the first object is lesser or equal to the second or not
	if (!(objectC<=objectD))
	{
		iLog->Log( KNLesser );
		return KErrNone;
	}
    else
    {
        return KErrGeneral;
    }	
	
	
}
// -----------------------------------------------------------------------------
// CTestSmilEngine::PlusPlusOptr
// TestCase for TSmilTime::operator++
// -----------------------------------------------------------------------------
//

TInt CTestSmilEngine::PlusPlusOptr()    
{
	// Print to UI

  _LIT( KApiName, "In PlusPlusOptr" );
  _LIT( KSuccess, "This API is tested success" );
  _LIT( KFailure, "This API is tested failure" );
  _LIT( KFinished, "Going out of PlusPlusOptr" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
  
  TSmilTime objectA(2);
	
	//Incrementing the object using ++ operator
	objectA++;
	
	TSmilTime test(3);
	
	  //Verifying the TSmilTime::Operator++
	
    if(objectA==test)
            {
                iLog->Log( KSuccess );
                iLog->Log( KFinished );
                
                return KErrNone;
            }
    else
            {
                iLog->Log( KFailure );
                iLog->Log( KFinished );
                
                return KErrGeneral;
            }
        
        
    }	
	
	
			
	

// -----------------------------------------------------------------------------
// CTestSmilEngine::EqualOptr
// TestCase for TSmilTime::operator=.
// -----------------------------------------------------------------------------
//
TInt CTestSmilEngine::EqualOptr() {
	// Print to UI

  _LIT( KApiName, "In EqualOptr" );
  _LIT( KSuccess, "This API is tested success" );
  _LIT( KFailure, "This API is tested failure" );
  _LIT( KFinished, "Going out of EqualOptr" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
  
  TSmilTime objectA(2);
	TSmilTime objectB;
	
	  //Verifying the TSmilTime::Operator=
	
	objectB=objectA;
    if(objectB==objectA)
            {
                iLog->Log( KSuccess );
                iLog->Log( KFinished );
                
                return KErrNone;
            }
    else
            {
                iLog->Log( KFailure );
                iLog->Log( KFinished );
                
                return KErrGeneral;
            }
       
    }   
// -----------------------------------------------------------------------------
// CTestSmilEngine::IntEqualOptr
// TestCase for TSmilTime::operator=(TInt)
// -----------------------------------------------------------------------------
//

TInt CTestSmilEngine::TIntEqualOptr()    
{
	// Print to UI

  _LIT( KApiName, "In IntEqualOptr" );
  _LIT( KSuccess, "This API is tested success" );
   _LIT( KFailure, "This API is tested failure" );
   _LIT( KFinished, "Going out of IntEqualOptr" );

  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
  
  TSmilTime objectA;
	
  //Verifying the TSmilTime::Operator=(TInt)
	objectA=10;
	
	TSmilTime test(10);
    if(objectA==test)
            {
                iLog->Log( KSuccess );
                iLog->Log( KFinished );
                
                return KErrNone;
            }
    else
            {
                iLog->Log( KFailure );
                iLog->Log( KFinished );
                
                return KErrGeneral;
            }
        
  }   
// -----------------------------------------------------------------------------
// CTestSmilEngine::PlusOptr
// TestCase for TSmilTime::operator+
// -----------------------------------------------------------------------------
//
TInt CTestSmilEngine::PlusOptr(CStifItemParser& aItem)    
{
	// Print to UI

  _LIT( KApiName, "In MinusOptr" );
  _LIT( KSuccess, "This API is tested success" );
   _LIT( KFailure, "This API is tested failure" );
   _LIT( KFinished, "Going out of PlusOptr" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
  
  TInt a,b;
  aItem.GetNextInt(a);
  aItem.GetNextInt(b);
    
	TSmilTime objectA(TSmilTime::KUnresolved);
	TSmilTime objectB(3);
	TSmilTime objectC;
	TSmilTime objectD(TSmilTime::KIndefinite);
	TSmilTime objectE(2);
	
	//Adding with KUnresolved operand
	objectC=objectA+objectB;
	
	//Adding with KIndefinite operand
	objectC=objectD+objectE;
	
	TSmilTime objectF(a);
  TSmilTime objectG(b);
  
 	//Multiplying with valid operands
  objectC=objectF+objectG;
  TSmilTime test(15);
  
  //Verifying the TSmilTime::Operator+
  if(objectC==test)
          {
              iLog->Log( KSuccess );
              iLog->Log( KFinished );
              
              return KErrNone;
          }
  else
          {
              iLog->Log( KFailure );
              iLog->Log( KFinished );
              
              return KErrGeneral;
          }
      
      
  }   
// -----------------------------------------------------------------------------
// CTestSmilEngine::ToMicroSeconds
// TestCase for TSmilTime::ToMicroSeconds.
// -----------------------------------------------------------------------------
//

TInt CTestSmilEngine::ToMicroSeconds()    
{
	// Print to UI
  _LIT( KApiName, "In ToMicroSecondsL" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
  
  TTimeIntervalMicroSeconds time;
  TSmilTime objectB(10);
  
  //converting the TSmilTime's iValue to microseconds and equating this to TTimeIntervalMicroSeconds's time
  time=objectB.ToMicroSeconds();
		
	return KErrNone;
	
}
// -----------------------------------------------------------------------------
// CTestSmilEngine::FromMicroSeconds
// TestCase for TSmilTime::FromMicroSeconds.
// -----------------------------------------------------------------------------
//
TInt CTestSmilEngine::FromMicroSeconds()    
{
	// Print to UI

  _LIT( KApiName, "In ToMicroSeconds" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
  
  TTimeIntervalMicroSeconds time;
  TSmilTime objectB(10);
  TSmilTime objectC;
  
  //Creates time value from an integer value
  objectB=objectB.FromMicroSeconds(time);
		
	return KErrNone;
	
}
// -----------------------------------------------------------------------------
// CTestSmilEngine::IntFromMicroSeconds
// TestCase for TSmilTime::FromMicroSeconds.
// -----------------------------------------------------------------------------
//
TInt CTestSmilEngine::IntFromMicroSeconds()    
{
	// Print to UI

  _LIT( KApiName, "In ToMicroSecondsL" );
  TestModuleIf().Printf( 0, KSmilEngine, KApiName );
  // Print to log file
  iLog->Log( KApiName );
  
  TInt time;
  TSmilTime objectB(10);
  TSmilTime objectC;
  
  objectC=objectB.FromMicroSeconds(time);
	
	return KErrNone;
	
}


// For CSmilTransitionFilter's APIs
         



         
 // -----------------------------------------------------------------------------
 // CTestSmilEngine::CreateCSmilParserL
 // TestCase for CSmilParser's 6 Exported functions 
 // -----------------------------------------------------------------------------
 //

TInt CTestSmilEngine::SmilParserL() 
    {
    
        TFileName iFilePath;
	#ifdef __WINSCW__
	
	_LIT( KTestFile,"c:\\data\\Others\\pres.smil" );
	iFilePath = KTestFile;
	#else
	_LIT( KTestFile, "%c:\\testing\\data\\TestSmilEngine\\pres.smil" );
	iFilePath = KTestFile;
	iFilePath.Format(KTestFile,69);/*69 Repesents E*/	   
	#endif
    //E:\testing\data\MMC\TestSmilEngine\pres.smil 
    
    _LIT( KApiName, "In SmilParserL" );
    _LIT(KParseSmilDocStart,"Parsing of the Smil Document has started");
    _LIT(KParseSmilDocEnd,"Parsing of the Smil Document has ended");
    _LIT(KParseDomStart,"Parsing of the Dom has started");
    _LIT(KParseDomEnd,"Parsing of the Dom has ended");
    _LIT(KBaseUrl,"");
    _LIT(KSetMaxDownUpScaling,"In SetMaxDownUpScaling");
    _LIT(KColour,"color");
    
    TestModuleIf().Printf( 0, KSmilEngine, KApiName );
    // Print to log file
    iLog->Log( KApiName ); 
    
    
    RFs aFs;
    TRAPD(errRF,aFs.Connect());
     
    if( errRF != KErrNone )
     {
               return errRF;
     }
     
     
    RFile file;
    CMDXMLDocument* xmldoc;
      
    //Creating the CSmilSyncParse's object for smil doc parsing
    CSmilSyncParse* syncParse = CSmilSyncParse::NewLC();
    
    
    iLog->Log( KParseSmilDocStart );

    //Starting to parse the smil document
    xmldoc = syncParse->StartL( aFs, iFilePath);
    
    iLog->Log( KParseSmilDocEnd );
    
    CleanupStack::PopAndDestroy();  // syncParse
  
    //creating MSmilPlayer's object
    MSmilPlayer* player=SmilPlayer::NewLC();
        
    //Creating the CSmilParser's object for dom structure parsing and to create CSmilPresentation's object
    CSmilParser* parser = CSmilParser::NewL(player);
    
    CSmilPresentation* presentation;
    
    iLog->Log( KParseDomStart );
    
         
    //Starting to parse the dom and to create CSmilPresentation's object by CSmilParser::ParseL(CMDXMLDocument* aDoc,TDesC& aBaseUrl)
    
    TRAPD (errorparser,presentation = parser->ParseL(xmldoc,KBaseUrl));
    if( errorparser != KErrNone )
        {
              
              return errorparser;
                            
        }
   
    
    iLog->Log( KParseDomEnd );
    
    TReal32 down(3);
    
    TReal32 up(4);
    
    
    //calling CSmilParser::SetMaxDownUpScaling
    
    
    parser->SetMaxDownUpScaling(down,up);
    
    iLog->Log( KSetMaxDownUpScaling );
    
    TBool test=EFalse;
    
    //Calling ParseColor for every scenario
    
    _LIT(KColour1,"");
    _LIT(KColour2,"#a3b");
    _LIT(KColour3,"transparent");
    _LIT(KColour4,"rgb(");
    _LIT(KColour6,"foreground-color");
    _LIT(KColour7,"ForegroundColor");
    _LIT(KColour8,"silver");
    _LIT(KColour5,"cdsjhvcdkvksdjvbasdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdsdgrpogjgogjlbfdlbnflbnlfnknb");   
    _LIT(KColour9,"#a3bcde");
    _LIT(KColour10,"#silver");
    _LIT(KColour11,"abcdrgb(");
    
    parser->ParseColor(KColour,test);
    
    parser->ParseColor(KColour1,test);
    
    parser->ParseColor(KColour2,test);
    
    parser->ParseColor(KColour3,test);
 
    parser->ParseColor(KColour4,test);
    
    parser->ParseColor(KColour5,test);
    
    parser->ParseColor(KColour6,test);
    
    parser->ParseColor(KColour7,test);
    
    parser->ParseColor(KColour8,test);
    
    parser->ParseColor(KColour9,test);
    
    parser->ParseColor(KColour10,test);
    
    
    parser->ParseColor(KColour11,test);
    
    
    //Starting to parse the dom and to create CSmilPresentation's object by CSmilParser::ParseL(TDesC& aSmil,TDesC& aBaseUrl)
    TRAPD (error,presentation = parser->ParseL(iFilePath,KBaseUrl));
    
    if( error != KErrNone )
        {
              return error;
        }
        
    
    
    return KErrNone;
    
    }
      
// -----------------------------------------------------------------------------
// CTestSmilEngine::SmilUIPresentationL
// TestCase for SmilPresentation API
// -----------------------------------------------------------------------------
// 
TInt CTestSmilEngine::SmilUIPresentationL(CStifItemParser& aItem) 
    {
    
    TFileName iFilePath;
#ifdef __WINSCW__

    _LIT( KTestFile,"c:\\data\\Others\\UI_pres.smil" );
    iFilePath = KTestFile;
#else

    _LIT( KTestFile, "%c:\\testing\\Data\\TestSmilEngine\\UI_pres.smil" );
    iFilePath = KTestFile;
    iFilePath.Format(KTestFile,69);/*69 Repesents E*/
#endif

    _LIT( KApiName, "In SmilUIPresentationL" );
    _LIT(KSmilUIPresentationStart,"Starting the Smil UI presentation");
    _LIT(KSmilUIPresentationEnd,"Ending the Smil UI presentation");

    TestModuleIf().Printf( 0, KSmilEngine, KApiName );

    // Print to log file
    iLog->Log( KApiName ); 

    RFs aFs;
    TRAPD(errorRF,aFs.Connect());

    if( errorRF != KErrNone )
        {
        return errorRF;
        }

    CMDXMLDocument* xmldoc;

    //Creating the CSmilSyncParse's object for smil doc parsing
    CSmilSyncParse* syncParse = CSmilSyncParse::NewLC();

    iLog->Log( KSmilUIPresentationStart );

    //Starting to parse the smil document
    xmldoc = syncParse->StartL( aFs, iFilePath);

    CDummyMediaFactoryFileInfo *mfactoryInfo = new(ELeave) CDummyMediaFactoryFileInfo;
    mfactoryInfo->ConstructL();

    _LIT(KDesC,"");
	TPtrC string;   
	//If file is passed as input parameter
	if(aItem.GetNextString(string) == KErrNotFound)
		{
	    CSmilPlayerDialog *dlg = CSmilPlayerDialog::NewL(xmldoc,mfactoryInfo,KDesC,ETrue,EFalse);
	    iEikonEnv->AppUiFactory()->ReadAppInfoResourceL(0,iEikonEnv->EikAppUi());
	    
	    TRAPD(err1 ,dlg->ExecuteLD());
	    if(err1 == KErrNone)
	        {
	        iLog->Log( _L("CSmilPlayerDialog::NewL with parameter CMDXMLDocument is Passed") );
	        }
	    else
	        {
	        iLog->Log( _L("CSmilPlayerDialog::NewL with parameter CMDXMLDocument has left with error id = %d"),err1 );
			delete mfactoryInfo;
	    	CleanupStack::PopAndDestroy();  // syncParse
	    	return err1;
	        }
		}
	else
		{
		TBuf16<256>   fileName;
#ifdef __WINSCW__

	_LIT( KSmilPath,"c:\\data\\Others\\" );
	fileName.Format( KSmilPath);
#else
	_LIT( KSmilPath, "%c:\\testing\\Data\\TestSmilEngine\\" );
	fileName.Format(KSmilPath,69);/*69 Repesents E*/
#endif

		//Append the smil file name passed from outside
		fileName.Append( string);
		//Open the smil file 
		RFile smilfile;
		User::LeaveIfError(smilfile.Open(aFs, fileName, EFileRead));
		//Calling CSmilPlayerDialog::NewL with RFile parameters
		iLog->Log( _L("Calling CSmilPlayerDialog::NewL with RFile parameters"));
		CSmilPlayerDialog *dlgfile = CSmilPlayerDialog::NewL(smilfile,mfactoryInfo,KDesC,ETrue,EFalse);
		iEikonEnv->AppUiFactory()->ReadAppInfoResourceL(0,iEikonEnv->EikAppUi()); 

		TRAPD(errlrfile ,dlgfile->ExecuteLD());
		if(errlrfile == KErrNone)
			{
			iLog->Log( _L("CSmilPlayerDialog::NewL with parameter RFile is Passed") );
			 smilfile.Close();
			}
		else
			{
			iLog->Log( _L("CSmilPlayerDialog::NewL with parameter RFile has left with error id = %d"),errlrfile );
			delete mfactoryInfo;
			smilfile.Close();
			CleanupStack::PopAndDestroy();  // syncParse
			return errlrfile;
			} 
		}
    iLog->Log( KSmilUIPresentationEnd );
    delete mfactoryInfo;
    CleanupStack::PopAndDestroy();  // syncParse
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CTestSmilEngine::SmilPresentationL
// TestCase for SmilPresentation API
// -----------------------------------------------------------------------------
//
    
 
 TInt CTestSmilEngine::SmilPresentationL() 
     {
         TFileName iFilePath;
   	#ifdef __WINSCW__
	
	_LIT( KTestFile,"c:\\data\\Others\\pres.smil" );
	iFilePath = KTestFile;
	#else

	_LIT( KTestFile, "%c:\\testing\\data\\TestSmilEngine\\pres.smil" );
	iFilePath = KTestFile;
	iFilePath.Format(KTestFile,69);/*69 Repesents E*/	  
	
	#endif
     
     _LIT( KApiName, "In SmilPresentationL" );
     _LIT(KPlay,"Playing the presentation");
     _LIT(KPause,"Pausing the presentation");
     _LIT(KResume,"Resuming the presentation");
     _LIT(KStop,"Stoping the presentation");
     _LIT(KPlayError,"Error in playing");
     _LIT(KPauseError,"Error in Pausing the presentation");
     _LIT(KResumeError,"Error in Resuming the presentation");
     _LIT(KStopError,"Error in Stoping the presentation");
     _LIT(KParseSmilDocStart,"Parsing of the Smil Document has started");
     _LIT(KParseSmilDocEnd,"Parsing of the Smil Document has ended");
     _LIT(KParseDomStart,"Parsing of the Dom has started");
     _LIT(KParseDomEnd,"Parsing of the Dom has ended");
     _LIT(KVolumeError,"Error in Setting Volume");
     _LIT(KVolume,"Success in Setting Volume");
	 _LIT(KBaseNewUrl,"http://www.google.com");
     _LIT(KBaseUrl,"");
     _LIT(KNextSlide,"Moved to next slide");
     _LIT(KNextSlideError,"Error in moving to next slide");
     _LIT(KPrevious,"Moved to the previous slide");
     _LIT(KPreviousError,"Error in moving to the previous slide");
     _LIT(KCurrent,"Success in checking CurrentSlide");
     _LIT(KCurrentError,"Failure in checking CurrentSlide");
     _LIT(KBaseUrlSuccess,"Success  in setting BaseUrl");
     _LIT(KBaseUrlFailure,"Failure in setting BaseUrl");
      _LIT(KTransparent,"Success in Transparent method");
     _LIT(KTransparentError,"Failure  in Transparent method");
     _LIT(KSlideCountSuccess,"Success in SlideCount method");
     _LIT(KSlideCountFailure,"Failure in SlideCount method");
    
	 _LIT(KMouseClicked,"In MouseClickedL method");
     _LIT(KFocusedObject,"In FocusedObject method");
	 _LIT(KInTransparent,"In IsTransparent method");
     _LIT(KCurentTime,"In CurentTime()");
     _LIT(KState,"In State()");
     _LIT(KDuration,"In Duration()");
     _LIT(KRootRegionRect,"In RootRegionRect");
     _LIT(KGetScalingFactor,"In GetScalingFactor");
     _LIT(KName,"name");
     _LIT(KSource,"source");
     
     TestModuleIf().Printf( 0, KSmilEngine, KApiName );
     
     // Print to log file
     iLog->Log( KApiName ); 
     
     
     
     RFs aFs;
     TRAPD(errorRF,aFs.Connect());
     
     if( errorRF != KErrNone )
     {
               return errorRF;
     }
     
     CMDXMLDocument* xmldoc;
     
     
     //Creating the CSmilSyncParse's object for smil doc parsing
     CSmilSyncParse* syncParse = CSmilSyncParse::NewLC();
     
     
     iLog->Log( KParseSmilDocStart );

     //Starting to parse the smil document
     xmldoc = syncParse->StartL( aFs, iFilePath);
          
     iLog->Log( KParseSmilDocEnd );
     
     CleanupStack::PopAndDestroy();  // syncParse
     
     MSmilPlayer* player=SmilPlayer::NewLC();
     
     //Creating the CSmilParser's object for dom structure parsing and to create CSmilPresentation's object
     CSmilParser* parser = CSmilParser::NewL(player);
     
     CSmilPresentation* presentation;
      
     iLog->Log( KParseDomStart );
     
     //Starting to parse the dom and to create CSmilPresentation's object
     TRAPD (error,presentation = parser->ParseL(xmldoc,KBaseUrl));
      
     if( error != KErrNone )
         {
               return error;
         }
     
     iLog->Log( KParseDomEnd );
    
    
 
  
	//Playing the pres.smil presentation.This presentation contains 3 slides,one video and 2 images
     TRAPD (err,presentation->PlayL());
          
     if( err != KErrNone )
       {
          return err;
       }
     
     
     CSmilPresentation::TPresentationState state;
     
     //calling the State() to get the state the player is in
     state=presentation->State();
     
     if(state==CSmilPresentation::EPlaying)
         {
             iLog->Log( KPlay );
         }
     
     else
         {
             iLog->Log( KPlayError );
             return KErrGeneral;
            
         }
 
     //Setting the volume when the presentation is playing.
          
     TInt volume=5;
     
     presentation->SetVolume(volume);
     
     //Checking the SetVolume method 
     
     if(presentation->Volume()==5)
     	 {
             iLog->Log( KVolume );
         }
     
     else
         {
             iLog->Log( KVolumeError );
             return KErrGeneral;
          
         }
          
  	//Pausing the pres.smil presentation EStopped
    
	presentation->Pause();
	
	TPoint point(100,355);
	
	//Calling MouseClickedL 
	
	iLog->Log( KMouseClicked );
	
	presentation->MouseClickedL(point);
		
	state=presentation->State();
    
    //Checking the Pause method 
    
    if(state==CSmilPresentation::EPaused)
        {
            iLog->Log( KPause );
        }
    
    else
        {
            iLog->Log( KPauseError );
            return KErrGeneral;
        }
        
        
    presentation->Pause();
    	
     // Setting the iSlideModeEnabled true for Slide traversal
   	
	 presentation->SetSlideModeEnabled(ETrue);	
     
     //Knowing the slide we are in now using Exported CurrentSlide()
   
     TInt current;
     
     current=presentation->CurrentSlide();
     
     //Checking the CurrentSlide method
     
     if(current==1)
        {
            iLog->Log( KCurrent );
        }
    
     else
        {
            iLog->Log( KCurrentError );
            return KErrGeneral;
        }
        
     TInt previous;
     
     //Calling PreviousSlideL when there is no previous slide
        
     TRAPD(errorNoPreviousSlide,previous=presentation->PreviousSlideL());
     
     if(errorNoPreviousSlide!=KErrNone) 
     {
             return errorNoPreviousSlide;
     } 
     

      
    //Moving to the Next slide using Exported NextSlideL() 
     
   	// Setting the iSlideModeEnabled true
   	
	 presentation->SetSlideModeEnabled(ETrue);
     
     TBool next;
     
     next=presentation->NextSlideL();
     
     //Checking the NextSlideL method
     
     if(next)
        {
            iLog->Log( KNextSlide );
        }
    
     else
        {
            iLog->Log( KNextSlideError );
            return KErrGeneral;
        }
        
     //Moving to the third slide which is the last slide inthe presentation
     
     next=presentation->NextSlideL();
     
     //calling NextSlideLwhen there is no next slide
     
     next=presentation->NextSlideL();

     if(next)
        {
            iLog->Log( KNextSlideError );
                  
        }
    
     else
        {
            iLog->Log( KNextSlide );
        }
        
        
     //Moving to the previous slide
             
     TRAPD(errorPreviousSlide,previous=presentation->PreviousSlideL());
     
     if(errorPreviousSlide!=KErrNone) 
     {
     	return errorPreviousSlide;
     }
     
     //Checking the PreviousSlideL method
     
     if(previous)
        {
            iLog->Log( KPrevious );
        }
    
     else
        {
            iLog->Log( KPrevious );
            return KErrGeneral;
        }        
        
	// Resuming the Presentation
	
	presentation->Resume();
	

	state=presentation->State();
	
	//Checking the Resume method 
	
    if(state==CSmilPresentation::EPlaying)
        {
            iLog->Log( KResume );
        }
    
    else
        {
            iLog->Log( KResumeError );
            return KErrGeneral;
        }	
    
    //Calling Resume again to check
    
    presentation->Resume();

        
    presentation->SetStartAction(CSmilPresentation::EShowFirstFrame);
	
	//Stoping the pres.smil presentation
    TRAPD (eror,presentation->StopL());
              
    if( eror != KErrNone )
        {
            return eror;
        }
	state=presentation->State();
	
	//Checking the StopL method
	    
    if(state==CSmilPresentation::EStopped)
        {
            iLog->Log( KStop );
        }
    
    else
        {
            iLog->Log( KStopError );
            return KErrGeneral;
            
        }	
        
     //Calling PlayL   
     TRAPD (errP,presentation->PlayL());
          
     if( errP != KErrNone )
       {
          return errP;
       }
       
       
		
	//creating TSmilTime's object
	TSmilTime cur;
	
	//calling the CurrentTime
	cur = presentation->CurrentTime();
	
	iLog->Log( KCurentTime );
	
	//Changing the state and calling CurrentTime
	
	TRAPD (erorstop,presentation->StopL());
	
	cur = presentation->CurrentTime();
	
	
	
	//creating TSmilTime's object
	TSmilTime duration;
	
	
	iLog->Log( KDuration );
	//calling the Duration()
	duration = presentation->Duration();
	

	TInt count;
	
	//Calling SlideCount 
	
	count=presentation->SlideCount();
	
	//Checking the SlideCount method
    
	if(count==3)
    {
        iLog->Log( KSlideCountSuccess );
    }
    
    else
    {
        iLog->Log( KSlideCountFailure );
        
    }
	
	
	//Calling PreviousSlideL NextSlideL CurrentSlide SlideCount SlideCount  when SetSlideModeEnabled is disabled 
	
	presentation->SetSlideModeEnabled(EFalse);
	
	presentation->PreviousSlideL();
	
	presentation->NextSlideL();
	
	presentation->CurrentSlide();
	
	presentation->SlideCount();

	// Setting the new BaseUrl

	TRAPD(ErrorSetBase,presentation->SetBaseUrlL(KBaseNewUrl));
	
	if(ErrorSetBase!=KErrNone)
	{
		
		return ErrorSetBase;
	}
	
	//Calling BaseUrl to get the BaseUrl 
	
	TPtrC base=presentation->BaseUrl();

	//Checking SetBaseUrlL and BaseUrl mathods
	TPtrC ptr1(KBaseNewUrl); 

	if(base.Compare(ptr1))
	{
		iLog->Log( KBaseUrlSuccess );
	}
	
	else
	{
		iLog->Log( KBaseUrlFailure );
		
	}
	
	
	TReal32 scalingFactor;
	
	//calling GetScalingFactor();
	
	
	iLog->Log( KGetScalingFactor );
	
	scalingFactor=presentation->GetScalingFactor();
	
	
	TRect rect;
	//calling the RootRegionRect();
	
	iLog->Log( KRootRegionRect );
	
	rect=presentation->RootRegionRect();
	
	
	//	Calling the SubmitEventL method
	
	presentation->SubmitEventL(KName,KSource);
	
	//Calling the FocusedObject to get the focused object
	
	
     
     CSmilObject* smilObject;
     
     iLog->Log( KFocusedObject );
     
     //Calling the FocusedObject to get the current FocusedObject
     
     smilObject=presentation->FocusedObject();
     
     
     //Calling Cancel focusL
     
     presentation->CancelFocusL();
     
     //Calling the IsTransparent method 
     
     TBool transparent;
     
     iLog->Log( KInTransparent );
     
     transparent=presentation->IsTransparent();
     
     if(transparent)
     {
     	
     	iLog->Log( KTransparent );
     }
     
     else
     {
     	
     	iLog->Log( KTransparentError );
     }

   
    return KErrNone;
        
     
     }
     
     
 
 
 // -----------------------------------------------------------------------------
 // CTestSmilEngine::CreateCSmilTransitionFilterL
 // TestCase for SmilTranstion API
 // -----------------------------------------------------------------------------
 //

 TInt CTestSmilEngine::CreateCSmilTransitionFilterL()
 
 {
 
 
      _LIT( KApiName, "In CreateCSmilTransitionFilterL" );
      _LIT(KConstruct,"In ConstructL");
      _LIT(KActivate,"In ActivateL");
      _LIT(KMedia,"In MediaL");
      _LIT(KTransitions,"In TransitionsL");
         
      TestModuleIf().Printf( 0, KSmilEngine, KApiName );
      // Print to log file
      iLog->Log( KApiName );
      
      
     //Calling CSmilTransitionFilter::CSmilTransitionFilter to construct its object
     //CDerSmilTransitionFilter is derived from CSmilTransitionFilter
      
     CDerSmilTransitionFilter* filter=new(ELeave)CDerSmilTransitionFilter;
     
     //Creating MSmilMedia's object 
     MSmilMedia* media=new(ELeave) SmilMedia;
     
     TSmilTime time(10);
     
     //Calling the CSmilTransitionFilter::ConstructL
     filter->ConstructL( media );
     
     iLog->Log( KConstruct );
     
     //calling the CSmilTransitionFilter::TransitionStartingL and CSmilTransitionFilter::TransitionEnding
     
    filter->TransitionsL();
     
    iLog->Log( KTransitions );
     
     
     //CSmilTransitionFilter::Media
     
     media=filter->Media();
     
     iLog->Log( KMedia );
     
     
     //CSmilTransitionFilter::~CSmilTransitionFilter
      
      
     delete filter;
 
     return KErrNone;
     
     
 }
 
  // -----------------------------------------------------------------------------
 // CTestSmilEngine::ConformanceCheckL
 // TestCase for ConformanceCheck.
 // -----------------------------------------------------------------------------
 //
 
 TInt CTestSmilEngine::ConformanceCheckL()
     {
    TFileName iFilePath;
  
 
    _LIT( KApiName, "In ConformanceChecker" );
    _LIT( KConformanceStart, "ConformanceChecker Testing Started" );
    _LIT( KConformanceSuccess, "Conformance Checked with success" );
    _LIT( KConformanceFailure, "Conformance Checked with failure" );
    _LIT( KConformanceEnd, "ConformanceChecker Testing Ended" );
             
    TestModuleIf().Printf( 0, KSmilEngine, KApiName );
    
    // Print to log file
    iLog->Log( KApiName );
    
     
     RFs aFs;
     TRAPD(errorRF,aFs.Connect());
     
     if( errorRF != KErrNone )
     {
               return errorRF;
     }
     
     CMDXMLDocument* xmldoc1;
     CMDXMLDocument* xmldoc2;
     CMDXMLDocument* xmldoc3;
     CMDXMLDocument* xmldoc5;
     CMDXMLDocument* xmldoc6;
     CMDXMLDocument* xmldoc7;  
     CMDXMLDocument* xmldoc8;
     CMDXMLDocument* xmldoc9;    
     CMDXMLDocument* xmldoc10;    
     
     //Creating the CSmilSyncParse's object for smil doc parsing
     CSmilSyncParse* syncParse = CSmilSyncParse::NewLC();


     //Starting to parse the smil document
 #ifdef __WINSCW__
	_LIT( KTestFile1,"c:\\private\\1000484b\\Mail2\\00001001_S\\a\\0010002a_F\\3gpp\\3gpp002.smil" );
	xmldoc1 = syncParse->StartL( aFs, KTestFile1); 
#else 
	_LIT( KTestFile1, "%c:\\testing\\data\\TestSmilEngine\\3gpp002.smil" );
	iFilePath = KTestFile1;
	iFilePath.Format(KTestFile1,69);/*69 Repesents E*/	
	xmldoc1= syncParse->StartL( aFs, iFilePath);
#endif
     
#ifdef __WINSCW__
	_LIT( KTestFile2,"c:\\private\\1000484b\\Mail2\\00001001_S\\a\\0010002a_F\\EAllowAllAttributes_video.smil" );
	xmldoc2 = syncParse->StartL( aFs, KTestFile2);
 #else 
	_LIT( KTestFile2, "%c:\\testing\\data\\TestSmilEngine\\EAllowAllAttributes_video.smil" );
	iFilePath = KTestFile2;
	iFilePath.Format(KTestFile2,69);/*69 Repesents E*/	
	xmldoc2= syncParse->StartL( aFs, iFilePath);
 #endif
     
#ifdef __WINSCW__
	_LIT( KTestFile3,"c:\\data\\Others\\pres.smil" );
	xmldoc3 = syncParse->StartL( aFs, KTestFile3);	 
#else 
	_LIT( KTestFile3, "%c:\\testing\\data\\TestSmilEngine\\pres.smil" );
	iFilePath = KTestFile3;
	iFilePath.Format(KTestFile3,69);/*69 Repesents E*/	
	xmldoc3 = syncParse->StartL( aFs, iFilePath);	  
#endif
     
 #ifdef __WINSCW__
	_LIT( KTestFile4,"c:\\private\\1000484b\\Mail2\\00001001_S\\a\\0010002a_F\\3gpp002.smil" );
	xmldoc5 = syncParse->StartL( aFs, KTestFile4); 
#else 
	_LIT( KTestFile4, "%c:\\testing\\data\\TestSmilEngine\\3gpp003.smil" );
	iFilePath = KTestFile4;
	iFilePath.Format(KTestFile4,69);/*69 Repesents E*/	 
	xmldoc5 = syncParse->StartL( aFs, iFilePath);	  
#endif
     
#ifdef __WINSCW__
	_LIT( KTestFile5,"c:\\private\\1000484b\\Mail2\\00001001_S\\a\\0010002a_F\\EAllowAllAttributes1_video.smil" );
	xmldoc6 = syncParse->StartL( aFs, KTestFile5);
#else 
	_LIT( KTestFile5, "%c:\\testing\\data\\TestSmilEngine\\EAllowAllAttributes1_video.smil" );
	iFilePath = KTestFile5;
	iFilePath.Format(KTestFile5,69);/*69 Repesents E*/		 
	xmldoc6 = syncParse->StartL( aFs, iFilePath);	  
#endif
     
#ifdef __WINSCW__
	 
	 _LIT( KTestFile6,"c:\\private\\1000484b\\Mail2\\00001001_S\\a\\0010002a_F\\EAllowAllAttributes2_video.smil" );
	 xmldoc7 = syncParse->StartL( aFs, KTestFile6);
 #else 
	 _LIT( KTestFile6, "%c:\\testing\\data\\TestSmilEngine\\EAllowAllAttributes2_video.smil" );
	iFilePath = KTestFile6;
	iFilePath.Format(KTestFile6,69);/*69 Repesents E*/	 
	xmldoc7 = syncParse->StartL( aFs, iFilePath);
#endif
     
#ifdef __WINSCW__
	 
	 _LIT( KTestFile7,"c:\\private\\1000484b\\Mail2\\00001001_S\\a\\0010002a_F\\EAllowAllAttributes3_video.smil" );
        xmldoc8 = syncParse->StartL( aFs, KTestFile7);	 
#else 
	 _LIT( KTestFile7, "%c:\\testing\\data\\TestSmilEngine\\EAllowAllAttributes3_video.smil" );
	  iFilePath = KTestFile7;
	  iFilePath.Format(KTestFile7,69);/*69 Repesents E*/		 
	  xmldoc8 = syncParse->StartL( aFs, iFilePath);	 
#endif
     
#ifdef __WINSCW__
	 
	 _LIT( KTestFile8,"c:\\private\\1000484b\\Mail2\\00001001_S\\a\\0010002a_F\\EAllowAllAttributes4_video.smil" );
        xmldoc9 = syncParse->StartL( aFs, KTestFile8);
 
#else 
	 _LIT( KTestFile8, "%c:\\testing\\data\\TestSmilEngine\\EAllowAllAttributes4_video.smil" );
	  iFilePath = KTestFile8;
	  iFilePath.Format(KTestFile8,69);/*69 Repesents E*/		 
         xmldoc9 = syncParse->StartL( aFs, iFilePath);	  
#endif
#ifdef __WINSCW__
	 
	 _LIT( KTestFile9,"c:\\private\\1000484b\\Mail2\\00001001_S\\a\\0010002a_F\\EAllowAllAttributes5_video.smil" );
	 xmldoc10 = syncParse->StartL( aFs, KTestFile9);

	 
#else 
	 _LIT( KTestFile9, "%c:\\testing\\data\\TestSmilEngine\\EAllowAllAttributes5_video.smil" );
	  iFilePath = KTestFile9;
	  iFilePath.Format(KTestFile9,69);/*69 Repesents E*/		 
	  xmldoc10 = syncParse->StartL( aFs, iFilePath);	  
#endif
         
     CleanupStack::PopAndDestroy();  // syncParse
   
       
     CConformanceChecker* checker = CConformanceChecker::NewL();
    
     TBool result1,result2,result3,result4; 
     ///To Avoid WarCreated Error when building for ARMV5 
     TBool result5,result6,result7,result8,result9,result10,result11;
     
    
    
     //Checking for the correctness of the CConformanceChecker::Check API
     
     iLog->Log( KConformanceStart );
     
     result8=checker->Check(xmldoc7,EMmsSmil_v2_0,EAllowAllAttributes);
     
     if(!result8)
     {
     	iLog->Log( KApiName );
     	iLog->Log( KConformanceFailure );
     	iLog->Log( KConformanceEnd );	
     	
     	return KErrGeneral;
     	
     }
     
     else
     {
     
     	iLog->Log( KConformanceSuccess );
     	
     }
     
     //Covering all the decisions in the API
    
     result1=checker->Check(xmldoc1,EMmsSmil_v2_0,EAllowVideoTag | EAllowNonMilliseconds);
     
     result2=checker->Check(xmldoc2,EMmsSmil_v2_0,EAllowNonUsAscii );
     
     result3=checker->Check(xmldoc3,EMmsSmil_v2_0,EAllowSeqTag | EAllowMixedRegionDimensions);
     
     result4=checker->Check(NULL,EMmsSmil_v2_0,EAllowAnyRegionNames);
     
     result5=checker->Check(xmldoc3,EMmsSmil_v3_0,EAllowAllAttributes);
     
     result6=checker->Check(xmldoc5,EMmsSmil_v2_0,EAllowNonUsAscii);
     
     result7=checker->Check(xmldoc6,EMmsSmil_v2_0,EAllowMixedRegionDimensions | EAllowSeqTag );
     
     result9=checker->Check(xmldoc8,EMmsSmil_v2_0,EAllowVideoTag | EAllowSeqTag);
     
     result10=checker->Check(xmldoc9,EMmsSmil_v2_0,EAllowMixedRegionDimensions);
     
     result11=checker->Check(xmldoc10,EMmsSmil_v2_0,EAllowVideoTag);
     
     iLog->Log( KConformanceEnd );
     
     return KErrNone;
     
     }
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
