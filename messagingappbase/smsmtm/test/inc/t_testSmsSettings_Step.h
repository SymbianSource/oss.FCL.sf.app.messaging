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

#ifndef __T_TESTSMSSETTINGS_STEP_H__
#define __T_TESTSMSSETTINGS_STEP_H__
 
#include <test/testexecutestepbase.h>
#include <smstestutils.h>
#include "t_smcs2_server.h"
#include "t_testSmsBase_Step.h"
	
class CTestSmsSettingsStep : public CTestSmsBaseStep
	{
public:
	CTestSmsSettingsStep();
	~CTestSmsSettingsStep();
	virtual TVerdict doTestStepL();
private:
private:
	};
 	
_LIT(KTestSmsSettings,"TestSmsSettings");
	
#endif  //__T_TESTSMSSETTINGS_STEP_H__

