// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cactivewaiter.h"
#include "ctestimapservermtmrenamefolder.h"


CTestImapServerMtmRenameFolder::CTestImapServerMtmRenameFolder( )
	{
	
	}
	
CTestImapServerMtmRenameFolder::~CTestImapServerMtmRenameFolder( )
	{
	
	}
	
void CTestImapServerMtmRenameFolder::TestRenameFolderL( )
	{
	
	}
	
CTestSuite* CTestImapServerMtmRenameFolder::CreateSuiteL( const TDesC& aName )
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP( TestRenameFolderL );
	END_SUITE;
	}
