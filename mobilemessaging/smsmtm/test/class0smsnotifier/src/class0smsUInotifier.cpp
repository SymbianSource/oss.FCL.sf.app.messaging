// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ecom/ecom.h>
#include <tmsvpackednotifierrequest.h>
#include <ecom/implementationproxy.h>
#include <gsmubuf.h>
#include <s32mem.h>
#include <e32base.h>
#include "class0smsUInotifier.h"

/* UID of Class 0 SMS test notifier */
const TUid KClass0SmsNotifierPluginUid  = {0x2000C382};
const TUid KClass0SmsNotifierOutput 	= {0x2000C382};

//class 0 SMS data will be writing to file
_LIT(KTestClas0Sms, "\\logs\\testexecute\\class0sms.txt");

EXPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
	{
	CArrayPtrFlat<MEikSrvNotifierBase2>* subjects=NULL;
	TRAPD( err, subjects=new (ELeave)CArrayPtrFlat<MEikSrvNotifierBase2>(1) );
	if( err == KErrNone )
		{
		TRAP( err, subjects->AppendL( CClass0SmsUINotifier::NewL() ) );
		return(subjects);
		}
	else
		{
		return NULL;
		}
	}

CClass0SmsUINotifier* CClass0SmsUINotifier::NewL()
	{
	CClass0SmsUINotifier* self=new (ELeave) CClass0SmsUINotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CClass0SmsUINotifier::ConstructL()
	{
	}

CClass0SmsUINotifier::~CClass0SmsUINotifier()
	{
	REComSession::FinalClose();
	}

CClass0SmsUINotifier::CClass0SmsUINotifier()
	{
	iInfo.iUid      = KClass0SmsNotifierPluginUid;
	iInfo.iChannel  = KClass0SmsNotifierOutput;
	iInfo.iPriority = ENotifierPriorityHigh;
	}

void CClass0SmsUINotifier::Release()
	{
	delete this;
	}

/**
Called when a notifier is first loaded to allow any initial construction that is required.
 */
CClass0SmsUINotifier::TNotifierInfo CClass0SmsUINotifier::RegisterL()
	{
	return iInfo;
	}

CClass0SmsUINotifier::TNotifierInfo CClass0SmsUINotifier::Info() const
	{
	return iInfo;
	}
	
/**
The notifier has been deactivated so resources can be freed and outstanding messages completed.
 */
void CClass0SmsUINotifier::Cancel()
	{
	}

/**
Start the Notifier with data aBuffer. 

Not used for confirm notifiers
*/
TPtrC8 CClass0SmsUINotifier::StartL(const TDesC8& aBuffer)
	{
	CreateFile(aBuffer);
	return KNullDesC8();
	}
	
/**
Start the notifier with data aBuffer. aMessage should be completed when the notifier is deactivated.

May be called multiple times if more than one client starts the notifier. The notifier is immediately 
responsible for completing aMessage.
*/
void CClass0SmsUINotifier::StartL(const TDesC8& aBuffer, TInt /*aReplySlot*/, const RMessagePtr2&  aMessage)
	{
	CreateFile(aBuffer);
	aMessage.Complete(KErrNone);
	}
	
/**
Update a currently active notifier with data aBuffer.

Not used for confirm notifiers
*/
TPtrC8 CClass0SmsUINotifier::UpdateL(const TDesC8& /*aBuffer*/)
	{
	User::Leave(KErrNotSupported);
	return KNullDesC8();
	}


//Adding ECOM SUPPORT
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(0x2000C612, NotifierArray)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

//write received class0sms data to file
void CClass0SmsUINotifier::CreateFile(const TDesC8& aBuff)
{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TInt startPos, endPos;
	TBool isLastMessage = EFalse;
	
	RDesReadStream readStream(aBuff);
	CleanupClosePushL (readStream);
		
	startPos = readStream.ReadInt32L();
	endPos = readStream.ReadInt32L();
	isLastMessage = readStream.ReadInt32L();
	
	TBuf<1000> smsMsg1;
	readStream >> smsMsg1;
		
	RFile file;

	TPtrC8 smsMsg(REINTERPRET_CAST(const TUint8*, smsMsg1.Ptr()), smsMsg1.Size()); 
	User::LeaveIfError(file.Replace(fs, KTestClas0Sms, EFileShareAny | EFileWrite));
	file.Write(smsMsg);
	
	file.Close();
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&fs);
}
