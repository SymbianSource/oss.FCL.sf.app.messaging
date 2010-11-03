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
* Description:   Provides CXhtmlStack Class inline member functions.
*
*/



#ifndef XHTMLSTACK_INL
#define XHTMLSTACK_INL


// ---------------------------------------------------------
// CXhtmlStack::NewL
// ---------------------------------------------------------
//
template <typename T> inline CXhtmlStack<T>* CXhtmlStack<T>::NewL()
	{
	CXhtmlStack<T>* self = new( ELeave ) CXhtmlStack<T>;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(); 
	return self;
	}


// ---------------------------------------------------------
// CXhtmlStack::ConstructL
// ---------------------------------------------------------
//
template <typename T> inline void CXhtmlStack<T>::ConstructL()
	{
	iStack = new (ELeave) RPointerArray<T>( 16 );
	}

// ---------------------------------------------------------
// CXhtmlStack::~CXhtmlStack
// ---------------------------------------------------------
//
template <typename T> CXhtmlStack<T>::~CXhtmlStack()
	{
	if( iStack )
		{		
		iStack->ResetAndDestroy();
		}
	delete iStack;
	}

// ---------------------------------------------------------
// CXhtmlStack::CXhtmlStack
// ---------------------------------------------------------
//
template <typename T> CXhtmlStack<T>::CXhtmlStack() 
	{
	}

// ---------------------------------------------------------
// CXhtmlStack::Top
// ---------------------------------------------------------
//
template <typename T> inline T* CXhtmlStack<T>::Top()
	{
	TInt index = iStack->Count();
    if (index > 0)
        {
       	return (*iStack)[--index];
        }
    return 0; 
	}

// ---------------------------------------------------------
// CXhtmlStack::Pop
// ---------------------------------------------------------
//
template <typename T> inline void CXhtmlStack<T>::Pop()
	{
	TInt index = iStack->Count();
	if( index > 0 )
	    {
		T* ptr = (*iStack)[--index];
		iStack->Remove( index );
		delete ptr;
	    }
	}

// ---------------------------------------------------------
// CXhtmlStack::Push
// ---------------------------------------------------------
//
template <typename T> inline void CXhtmlStack<T>::PushL( T* aStyle )
	{
	iStack->InsertL( aStyle, iStack->Count());
	}


#endif


