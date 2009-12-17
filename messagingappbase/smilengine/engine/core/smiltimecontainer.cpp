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
* Description: smiltimecontainer implementation
*
*/



// INCLUDE FILES
#include "smiltimecontainer.h"
#include "smilobject.h"
#include "smilmedia.h"
#include "smilpresentation.h"
#include "smilregioninterface.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSmilTimeContainer::CSmilTimeContainer()
	{
	iFirstChild = 0;
	iLastChild = 0;
	}

// Destructor
CSmilTimeContainer::~CSmilTimeContainer()
	{
	for( CSmilObject* c = iFirstChild; c;)
		{
		CSmilObject* del = c;
		c = c->NextSibling();
		delete del;
		}
	
	}


void CSmilTimeContainer::ResolveL( const TSmilTime& aTps )
	{
	CSmilObject::ResolveL(aTps);	
	TSmilTime t = aTps - iCurrentInterval.iBegin;

	for( CSmilObject* c = iFirstChild; c ;c = c->NextSibling() )
		{
		c->ResolveL(t);
		}
	}



//
void CSmilTimeContainer::AddChild( CSmilObject* aObject )
	{
	aObject->SetParent( this );    

	if( iFirstChild == NULL )
		iFirstChild=iLastChild=aObject;
	else 
		{
		iLastChild->SetNextSibling( aObject );
		aObject->SetPreviousSibling( iLastChild );
		iLastChild = aObject;
		}
	}



void CSmilTimeContainer::EndL( const TSmilTime& aTime )
	{
	CSmilObject::EndL( aTime );
	for( CSmilObject* c = iFirstChild;c != NULL;c = c->NextSibling() )
		{
		if( c->IsPlaying() )
			c->EndL( aTime );
		
		if ( iFill==EFillRemove || iParentObject==0 )
			c->Unfreeze();		
		}
	}


void CSmilTimeContainer::BeginL( const TSmilTime& aTime )
	{
	CSmilObject::BeginL( aTime );
	}


TSmilTime CSmilTimeContainer::TimeContainerEnd() const
    {
	TSmilTime assumedResult;
	if( ( iEndsync == EEndsyncFirst ) || ( iEndsync == EEndsyncId ) )
		assumedResult = TSmilTime::KUnresolved;
	else
		assumedResult = 0;

	for( CSmilObject* c = iFirstChild; c != NULL; c = c->NextSibling() )
		{
		switch( iEndsync )
			{
			case EEndsyncFirst:
				if( c->EndTime() < assumedResult )
					assumedResult = c->EndTime();
				break;
			case EEndsyncId:
				// if we find the matching child, just return result
				if( c->Id() == iEndsyncId.Text() )
					return c->EndTime();
				break;
			case EEndsyncLast:
				if (c->HasMoreIntervals())
					assumedResult = TSmilTime::KUnresolved;
				else if( !c->BeginTime().IsUnresolved() && c->EndTime() > assumedResult )
					assumedResult = c->EndTime();
				break;
			case EEndsyncAll:
				if (c->HasMoreIntervals())
					assumedResult = TSmilTime::KUnresolved;
				else if( c->EndTime() > assumedResult )
					assumedResult = c->EndTime();
				break;
			}
		}
	return assumedResult;
	} 


void CSmilTimeContainer::NotifyDependents( const TSmilTime& aTime )
	{
	CSmilObject::NotifyDependents( aTime );
	for( CSmilObject* c = iFirstChild; c != NULL; c = c->NextSibling() )
		{
		// ### is this correct?
		c->RecalcInterval( aTime - iCurrentInterval.iBegin );        
		}
	}


void CSmilTimeContainer::Unfreeze()
	{	
	for( CSmilObject* a = iFirstChild;a != NULL;a = a->NextSibling() )
		a->Unfreeze();
	}


void CSmilTimeContainer::RepeatL( const TSmilTime& aTime )
	{
	// time container repeat...
	CSmilObject::RepeatL( aTime );
	//reset all children
	ResetChildrenL( aTime );
	iSimpleDuration = TSmilTime::KUnresolved;
	}


void CSmilTimeContainer::ResetChildrenL( const TSmilTime& aTime )
	{
	for( CSmilObject* c = iFirstChild;c != NULL;c = c->NextSibling() )
		{		
		if( c->IsPlaying() )
			c->EndL( aTime );
		if( c->Fill() != EFillRemove )
			c->Unfreeze();
		c->Reset();
		}
	}


void CSmilTimeContainer::Reset()
    {
	CSmilObject::Reset();
	for( CSmilObject* a = iFirstChild;a != NULL;a = a->NextSibling() )
		a->Reset();
	}


void CSmilTimeContainer::SetEndsyncL(TEndsync aEndsync, const TDesC& aId)
	{
	iEndsync = aEndsync;
	iEndsyncId.SetTextL(aId);
	}


CSmilObject* CSmilTimeContainer::FindNamedObject(const TDesC& aName)
	{

	CSmilObject* o = CSmilObject::FindNamedObject(aName);
	if (o) return o;

	for( CSmilObject* c = iFirstChild;c ;c = c->NextSibling() )
		{
		o = c->FindNamedObject(aName);	
		if (o) return o;
		}

	return 0;
	}


void CSmilTimeContainer::SetPaused(TBool aPaused)
	{
	for( CSmilObject* c = iFirstChild;c ;c = c->NextSibling() )
		{
		c->SetPaused(aPaused);
		}
	}


TSmilTime CSmilTimeContainer::NextActivation( const TSmilTime& aTime ) const
	{		

	TSmilTime res = CSmilObject::NextActivation(aTime);

	TSmilTime t = aTime - iRepeatBeginTime;

	for (CSmilObject* c=iFirstChild;c!=0;c=c->NextSibling())
		{
		TSmilTime a = c->NextActivation(t) + iRepeatBeginTime;
		if (a<res)
			res = a;
		}
	if (!res.IsFinite())
		res = TSmilTime::KUnresolved;
	return res;
	}


void CSmilTimeContainer::PrintTree(int d) const
	{
	CSmilObject::PrintTree(d);

	for (CSmilObject* c=iFirstChild;c!=0;c=c->NextSibling())
		c->PrintTree(d+2);

	}
// ================= OTHER EXPORTED FUNCTIONS ==============

//" Eagles may soar, but weasels don't get sucked into jet engines. "

//  End of File  

