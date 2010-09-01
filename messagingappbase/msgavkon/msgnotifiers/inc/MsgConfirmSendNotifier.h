/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     CMsgConfirmSendNotifier declaration file
*
*/



#ifndef __MSGCONFIRMSENDNOTIFIER_H__
#define __MSGCONFIRMSENDNOTIFIER_H__

#include <eiksrv.h>
#include <eiknotapi.h>
#include <msvapi.h>

class CMsvEntrySelection;
class CMsgConfirmSendQuery;

NONSHARABLE_CLASS( CMsgConfirmSendNotifier ) : public CBase,
                                               public MEikSrvNotifierBase2
	{
    public:
    
    	//	Construction
    	static CMsgConfirmSendNotifier* NewL( TBool aSendAs );
    	~CMsgConfirmSendNotifier();

    void HandleActiveEvent( const TInt aStatus );
    	
    private:
    
    	CMsgConfirmSendNotifier( TBool aSendAs );
    	void ConstructL();

    	//	From MNotifierBase2
    	virtual void Release();
    	virtual TNotifierInfo RegisterL();
    	virtual TNotifierInfo Info() const;
    	virtual TPtrC8 StartL( const TDesC8& aBuffer );
    	virtual void StartL( const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage );
    	virtual void Cancel();
    	virtual TPtrC8 UpdateL( const TDesC8& aBuffer );

    private:
    
    	TNotifierInfo   iInfo;
    	RMessagePtr2    iMessage;
    	TInt		    iReplySlot;
    	
    	CMsgConfirmSendQuery* iQuery;

	};
	
//**********************************
// CDummyObserver
//**********************************

NONSHARABLE_CLASS( CDummyObserver ) : public CBase, public MMsvSessionObserver
	{
public:
	void HandleSessionEventL( TMsvSessionEvent, TAny*, TAny*, TAny* ) {};
	};

#endif	// __MSGCONFIRMSENDNOTIFIER_H__


