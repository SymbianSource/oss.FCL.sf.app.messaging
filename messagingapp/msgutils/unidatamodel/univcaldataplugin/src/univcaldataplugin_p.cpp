/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 *
 */

//SYSTEM INCLUDES
#include <e32cmn.h>
#include <mtclreg.h>
#include <mtclbase.h>
#include <msvids.h>
#include <txtetext.h>
#include <txtrich.h>
#include <txtfmlyr.h>
#include <smsclnt.h>

#include <bioscmds.h>
#include <biocmtm.h>
#include "debugtraces.h"

#include <QDateTime>
#include <s60qconversions.h>
#include <mmsvattachmentmanager.h>

#include "convergedmessage.h"
#include "convergedmessageaddress.h"
#include "univcaldataplugin_p.h"
#include "sessioneventhandler.h"

#define KBioUidValue  0x10001262
const TUid KBioUidValueUid =
{KBioUidValue};


_LIT(KUnixEpoch, "19700000:000000.000000");

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::~UniVCalDataPluginPrivate
// @see header
//---------------------------------------------------------------
UniVCalDataPluginPrivate::~UniVCalDataPluginPrivate()
{
    q_ptr = NULL;
    if (iMsvEntry)
    {
        delete iMsvEntry;
    }

    delete iBioClientMtm;
    delete iMtmReg;
    delete iSessionHandler;
    delete iMSession;

}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::UniVCalDataPluginPrivate
// @see header
//---------------------------------------------------------------
UniVCalDataPluginPrivate::UniVCalDataPluginPrivate(UniVCalDataPlugin* plugin) :
    q_ptr(plugin), iMSession(NULL), iMtmReg(NULL), iBioClientMtm(NULL),
            iMsvEntry(NULL), iSessionHandler(NULL)
{
    iSessionHandler = new SessionEventHandler();
    // Create a new messaging server session..
    iMSession = CMsvSession::OpenSyncL(*iSessionHandler);
    iSessionHandler->start();
    done = EFalse;
}

void UniVCalDataPluginPrivate::initL()
{
    iMtmReg = CClientMtmRegistry::NewL(*iMSession);
    iBioClientMtm = (CBIOClientMtm*) iMtmReg->NewMtmL(KBioUidValueUid);

    attachmentProcessed = EFalse;
    done = ETrue;

}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::reset
// @see header
//---------------------------------------------------------------
void UniVCalDataPluginPrivate::reset()
{
}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::setMessageId
// @see header
//---------------------------------------------------------------
int UniVCalDataPluginPrivate::setMessageId(int mId)
{
    TInt error = KErrNone;
    TRAP(error, setMessageIdL(mId));
    return error;
}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::setMessageIdL
// @see header
//---------------------------------------------------------------
void UniVCalDataPluginPrivate::setMessageIdL(int mId)
{
    if (done == EFalse)
    {
        initL();
    }
    iMessageId = (TMsvId) mId;
    iBioClientMtm->SwitchCurrentEntryL(iMessageId);

    iMsvEntry = CMsvEntry::NewL(iBioClientMtm->Session(),
                                iMessageId,
                                TMsvSelectionOrdering());

    iBioClientMtm->LoadMessageL();
}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::body
// @see header
//---------------------------------------------------------------
void UniVCalDataPluginPrivate::body(QString& aBodyText)
{

    CRichText& textBody = iBioClientMtm->Body();
    TInt len = textBody.DocumentLength();
    HBufC* buf = HBufC::NewL(len);
    TPtr bufPtr = buf->Des();
    textBody.ExtractSelectively(bufPtr, 0, len, CPlainText::EExtractAll);
    aBodyText = S60QConversions::s60DescToQString(*buf);
    delete buf;
}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::messageSize
// @see header
//---------------------------------------------------------------
qint32 UniVCalDataPluginPrivate::messageSize()
{
    return iBioClientMtm->Entry().Entry().iSize;
}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::toRecipientList
// @see header
//---------------------------------------------------------------
void UniVCalDataPluginPrivate::toRecipientList(
                                               ConvergedMessageAddressList& mAddressList)
{
    TPtrC name;
    TPtrC address;

    CPlainText* pText = CPlainText::NewL();
    CleanupStack::PushL(pText);

    CSmsHeader* smsHeader = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *pText);
    CleanupStack::PushL(smsHeader);

    CMsvStore* store = iMsvEntry->ReadStoreL();
    CleanupStack::PushL(store);

    smsHeader->RestoreL(*store);

    for (TInt id = 0; id < smsHeader->Recipients().Count(); ++id)
    {
        CSmsNumber* rcpt = smsHeader->Recipients()[id];
        name.Set(rcpt->Name());
        address.Set(rcpt->Address());

        ConvergedMessageAddress
                * messageAddress =
                        new ConvergedMessageAddress(S60QConversions::s60DescToQString(address),
                                                    S60QConversions::s60DescToQString(name));
        mAddressList.append(messageAddress);
    }
    CleanupStack::PopAndDestroy(3, pText);

}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::fromAddress
// @see header
//---------------------------------------------------------------
void UniVCalDataPluginPrivate::fromAddress(QString& messageAddress)
{
    CPlainText* pText = CPlainText::NewL();
    CleanupStack::PushL(pText);

    CSmsHeader* smsHeader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *pText);
    CleanupStack::PushL(smsHeader);

    CMsvStore* store = iMsvEntry->ReadStoreL();
    CleanupStack::PushL(store);

    smsHeader->RestoreL(*store);

    messageAddress
            = S60QConversions::s60DescToQString(smsHeader->FromAddress());
    CleanupStack::PopAndDestroy(3, pText);
}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::timeStamp
// @see header
//---------------------------------------------------------------
int UniVCalDataPluginPrivate::timeStamp()
{
    QDateTime retTimeStamp;
    TTime timeStamp = iBioClientMtm->Entry().Entry().iDate;
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    return seconds.Int();
}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::attachmentList
// @see header
//---------------------------------------------------------------
RFile UniVCalDataPluginPrivate::attachmentL()
{
    if (attachmentProcessed)
    {

        CMsvStore* store1 = iMsvEntry->ReadStoreL();
        CleanupStack::PushL(store1);
        MMsvAttachmentManager& attachMan = store1->AttachmentManagerL();
        RFile file = attachMan.GetAttachmentFileL(0);
        CleanupStack::PopAndDestroy(store1);
        return file;
    }

    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL(selection);

    selection->AppendL(iMessageId);

    TBuf8<1> aParameter;
    CMsvOperationWait* wait = CMsvOperationWait::NewLC();
    wait->iStatus = KRequestPending;
    wait->Start();

    CMsvOperation* operation =
            iBioClientMtm->InvokeAsyncFunctionL(KBiosMtmParse,
                                                *selection,
                                                aParameter,
                                                wait->iStatus);

    CActiveScheduler::Start();

    CMsvStore* store = iMsvEntry->ReadStoreL();
    CleanupStack::PushL(store);
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    RFile file = attachMan.GetAttachmentFileL(0);

    delete operation;
    CleanupStack::PopAndDestroy(3,selection);
    attachmentProcessed = ETrue;
    return file;

}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::attachmentCount
// @see header
//---------------------------------------------------------------

int UniVCalDataPluginPrivate::attachmentCount()
{
    if (!attachmentProcessed)
    {
        RFile file = attachmentL();
        file.Close();
    }

    CMsvStore* store = iMsvEntry->ReadStoreL();
    CleanupStack::PushL(store);

    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    int count = attachMan.AttachmentCount();
    CleanupStack::PopAndDestroy(store);

    return count;
}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::messagePriority
// @see header
//---------------------------------------------------------------

MsgPriority UniVCalDataPluginPrivate::messagePriority()
{

    MsgPriority priority = (MsgPriority) iMsvEntry->Entry().Priority();
    return priority;
}

//---------------------------------------------------------------
// UniVCalDataPluginPrivate::session
// @see header
//---------------------------------------------------------------
CMsvSession* UniVCalDataPluginPrivate::session()
{
    return iMSession;
}

