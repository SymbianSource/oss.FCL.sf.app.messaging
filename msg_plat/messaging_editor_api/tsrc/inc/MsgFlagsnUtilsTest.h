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
* Description:   This file covers tests to MsgEditorFlags and MsgAttachmentUtils
*
*/


#ifndef CMSGFLAGSNUTILSTEST_H_
#define CMSGFLAGSNUTILSTEST_H_

#include <badesca.h>
#include <MMGFetchVerifier.h>

class CFetchVerifier : public MMGFetchVerifier , public CBase
{
    
    public:
    
    CFetchVerifier(){}
    TBool VerifySelectionL( const MDesCArray* /*aSelectedFiles*/ ){ return ETrue;}
    
};


class CMsgFlagsnUtilsTest : public CBase
{
public:
	CMsgFlagsnUtilsTest( CStifLogger* aLog );
	
	virtual ~CMsgFlagsnUtilsTest();
	
	
	  /**
        * C++ default constructor.
    */
    TInt MsgFlagsDefCtor();
    
    /*
    // One Argument Constructor.
    */
    TInt MsgFlagsArgCtor();

		//
		// Copy Constructor.
		//
    TInt MsgFlagsCopyCtor();

    /*
    Sets the given flags 
    */ 
    TInt SetFlag();
    
    //
    // Clears given flags.
    //
    TInt Clear();
    
    
    //
    // Returns ETrue, if all flags defined in aFlag parameter are set.
    //  
    TInt IsSet();
    
    //
    // Assignment operator.
    //
    TInt EqualToOperator();
    
    //
    // Write and read flags into and from given stream.
    //      
    TInt ExternalizenInternalizeL();
    
    //
    // Fetches an attachment of given type from external application.
    //      
    TInt ThreePmtrsFetchL();
      
    //
    // Fetches an attachment of given type from external application .
    //      
    TInt FourPmtrsFetchL();

    //
    // Fetches an attachment of given type from external application.
    //          
    TInt SixPmtrsFetchL();

    //
    // Fetches an attachment of given type from external application.
    //          
    TInt SixPmtrsParamListFetchL();

    //
    // Fetches an any attachment of given type from external application.
    //            
    TInt FetchAnyFileL();

    //
    // Tests GetFileNameFromBuffer of MsgAttachmentUtils
    //
    TInt GetFileNameFromBuffer();
    
    //
    //  Returns temp path used by msg editors and optionally
    // appends file name to it.
    //            
    TInt GetMsgEditorTempPath();	
    
private:
    
    //Data    
    CStifLogger*    iLog;
    
};

#endif /*CMSGFLAGSNUTILSTEST_H_*/
