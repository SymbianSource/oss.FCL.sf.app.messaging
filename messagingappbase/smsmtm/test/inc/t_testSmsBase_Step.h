// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __T_TESTSMSENTRYBASE_STEP_H__
#define __T_TESTSMSENTRYBASE_STEP_H__
 
#include <test/testexecutestepbase.h>
#include "t_smcs2_server.h"
#include "T_smutTE.h"

#include "smuthdr.h"	// sms header

class CTestSmsBaseStep : public CTestStep
	{
public:
	CTestSmsBaseStep();
	~CTestSmsBaseStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	CSmutTest* iSmcmTest;
private:
	CActiveScheduler* iScheduler;
	
	CSmsTestUtils* iSmsTestUtils;
	};
	
#endif  //__T_TESTSMSENTRYBASE_STEP_H__

