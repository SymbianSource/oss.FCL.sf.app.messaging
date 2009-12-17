/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: smilinstancetime implementation
*
*/



// INCLUDE FILES
#include "smilinstancetime.h"
#include "smiltime.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSmilInstanceTime::CSmilInstanceTime( TInt aType, TSmilTime aTime )
:iType( aType ), iTime( aTime ), iOffset( aTime ), iSyncbase( NULL ),
 iSyncBegin( ETrue )
	{
     //this(type,time,null,true);
	}

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSmilInstanceTime::CSmilInstanceTime( TInt aType, TSmilTime aTime,
									  CSmilObject* aSyncbase, TBool aSyncToBegin )
									: iType ( aType ), iTime( aTime ), iOffset( aTime ),
									  iSyncbase( aSyncbase ), iSyncBegin( aSyncToBegin )
	{
	}

TBool CSmilInstanceTime::IsOffsetValue() const
	{
	if( iType == KOffsetValue )
		return ETrue;
	else
		return EFalse;
	}

TBool CSmilInstanceTime::IsEventValue() const
	{
	if( iType == KEventValue )
		return ETrue;
	else
		return EFalse;
	}

TBool CSmilInstanceTime::IsSyncBaseValue() const
	{
	if( iType == KSyncBaseValue )
		return ETrue;
	else
		return EFalse;
	}

TBool CSmilInstanceTime::IsBeginSync() const
	{
	return iSyncBegin;
	}

TBool CSmilInstanceTime::IsEndSync() const
	{
	return !iSyncBegin;
	}

CSmilInstanceTime* CSmilInstanceTime::Next() const
	{
	return iNext;
	}

CSmilObject* CSmilInstanceTime::Syncbase() const
	{
	return iSyncbase;
	}

TSmilTime CSmilInstanceTime::Time() const
	{
	return iTime;
	}

void CSmilInstanceTime::SetTime( TSmilTime aTime )
	{
	iTime = aTime;
	}

void CSmilInstanceTime::SetSyncbase( CSmilObject* aObject )
	{
	iSyncbase = aObject;
	}

void CSmilInstanceTime::SetNext( CSmilInstanceTime* aTime )
	{
	iNext = aTime;
	}

void CSmilInstanceTime::SetSyncBegin( TBool aBool )
	{
	iSyncBegin = aBool;
	}

TSmilTime CSmilInstanceTime::Offset() const
	{
	return iOffset;
	}

//  End of File  
