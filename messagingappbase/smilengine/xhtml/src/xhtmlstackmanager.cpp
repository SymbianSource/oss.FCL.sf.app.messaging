/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides class methods of classes that are stored to CXhtmlStack.
*
*/



#include "xhtmlstack.h"
#include "xhtmlstackmanager.h"


// ---------------------------------------------------------
// CXhtmlStackManager::NewL
// ---------------------------------------------------------
//
CXhtmlStackManager* CXhtmlStackManager::NewL()
	{
	CXhtmlStackManager* self = new(ELeave) CXhtmlStackManager();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self ); 
	return self;
	};

// ---------------------------------------------------------
// CXhtmlStackManager::~CXhtmlStackManager 
// ---------------------------------------------------------
//
CXhtmlStackManager::~CXhtmlStackManager()
	{
	delete iStyleStack;
	delete iParaStack;
	delete iListStack;
	}

// ---------------------------------------------------------
// CXhtmlStackManager::CXhtmlStackManager
// ---------------------------------------------------------
//
CXhtmlStackManager::CXhtmlStackManager( )
	{
	}

// ---------------------------------------------------------
// CXhtmlStackManager::ConstructL
// ---------------------------------------------------------
//
void CXhtmlStackManager::ConstructL()
	{
	iStyleStack = CXhtmlStack<TXhtmlStyleInfo>::NewL();
	iParaStack = CXhtmlStack<TXhtmlParaInfo>::NewL();
	iListStack = CXhtmlStack<TXhtmlListInfo>::NewL();
	}

