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
#include <badesca.h>
#include <eikrted.h>

#include <mmsclient.h>
#include <mmssettings.h>
#include <mmsheaders.h>
#include <MsgMediaInfo.h>
#include <MsgMediaResolver.h>
#include <DRMHelper.h>


#include <QDateTime>

#include "convergedmessage.h"
#include "s60qconversions.h"
#include "convergedmessageaddress.h"
#include "unimmsdataplugin_p.h"
#include "UniDataModel.h"
#include "debugtraces.h"
#include "sessioneventhandler.h"


#define KSenduiMtmMmsUidValue 0x100058E1
const TUid KSenduiMtmMmsUid =
{KSenduiMtmMmsUidValue};
_LIT(KUnixEpoch, "19700000:000000.000000");

const TInt KMmsArrayAllocationNumber = 6;

UniMMSDataPluginPrivate::UniMMSDataPluginPrivate():
    mSession(NULL),
    mMtmRegistry(NULL),
    mUniDataModel(NULL),
    mMmsClient(NULL),
    mInsertingMedia(NULL),
    iEditor(NULL),
    iDom(NULL),
    iDRMHelper(NULL),
    iSessionHandler(NULL)
    {

    done = EFalse;
    iSessionHandler = new SessionEventHandler();
    mSession = CMsvSession::OpenSyncL(*iSessionHandler);
    iSessionHandler->start();
    }

void UniMMSDataPluginPrivate::initL()
{
    mMtmRegistry = CClientMtmRegistry::NewL(*mSession);
    mMmsClient
    = static_cast<CMmsClientMtm*> (mMtmRegistry->NewMtmL(KSenduiMtmMmsUid));

    User::LeaveIfError(mfsSession.Connect());
    iMessageId = 0;
}

UniMMSDataPluginPrivate::~UniMMSDataPluginPrivate()
{
    delete iEditor;
    delete iDom;
    if(mUniDataModel)
    {
        delete mUniDataModel;
    }
    mfsSession.Close();
    delete mMmsClient;
    delete mMtmRegistry;
    delete iSessionHandler;
    delete mSession;
}

int UniMMSDataPluginPrivate::setMessageId(int messageId)
{
    TInt error = KErrNone;
    TRAP(error, setMessageIdL(messageId));
    return error;
}

void UniMMSDataPluginPrivate::setMessageIdL(int messageId)
{
    if (done == EFalse)
    {
        initL();
    }

    if (iMessageId != messageId)
    {
        if (done)
        {
            reset();
        }
        done = ETrue;
        iMessageId = messageId;
        mMmsClient->SwitchCurrentEntryL(iMessageId);
        mMmsClient->LoadMessageL();
        mUniDataModel = CUniDataModel::NewL(mfsSession, *mMmsClient);
        mUniDataModel->RestoreL(*this, EFalse);
    }
}

void UniMMSDataPluginPrivate::reset()
{
    delete mUniDataModel;
    mUniDataModel = NULL;
}

void UniMMSDataPluginPrivate::body(QString& aBodyText)
{

    CRichText& textBody = mMmsClient->Body();
    TInt len = textBody.DocumentLength();
    HBufC* buf = HBufC::NewL(len);
    TPtr bufPtr = buf->Des();
    aBodyText = S60QConversions::s60DescToQString(bufPtr);
}

int UniMMSDataPluginPrivate::messageSize()
{
    return mMmsClient->MessageSize();
}


QString UniMMSDataPluginPrivate::subject()
{
   TPtrC sub =  mMmsClient->SubjectL();
   return S60QConversions::s60DescToQString(sub);
}




void UniMMSDataPluginPrivate::toRecipientList(
    ConvergedMessageAddressList& mAddressList)
{
    CMsvEntry* cEntry = mSession->GetEntryL(KMsvGlobalInBoxIndexEntryIdValue);
    CleanupStack::PushL(cEntry);
    TRAPD (error, cEntry->SetEntryL( iMessageId ));
    if (error != KErrNone)
    {
        CleanupStack::PopAndDestroy(); // cEntry
        return;
    }
    CMsvStore* store = cEntry->ReadStoreL();
    CleanupStack::PushL(store);
    CMmsSettings* settings = CMmsSettings::NewL();
    CleanupStack::PushL(settings);
    CMmsHeaders* mmsHeaders = CMmsHeaders::NewL(settings->MmsVersion());
    CleanupStack::PushL(mmsHeaders);
    mmsHeaders->RestoreL(*store);
    TPtrC name;
    TPtrC address;
    TPtrC to;
    for (TInt id = 0; id < mmsHeaders->ToRecipients().MdcaCount(); id++)
    {
        to.Set( (mmsHeaders->ToRecipients())[id]);
        extractNameAndAddress(to, name, address);
        // populate address
        ConvergedMessageAddress
        * messageAddress =
            new ConvergedMessageAddress(S60QConversions::s60DescToQString(address),
                S60QConversions::s60DescToQString(name));
        mAddressList.append(messageAddress);
    }

    CleanupStack::PopAndDestroy(3); //  mmsHeaders, settings, store

    CleanupStack::PopAndDestroy(); // cEntry
}

void UniMMSDataPluginPrivate::ccRecipientList(
    ConvergedMessageAddressList& mAddressList)
{
    CMsvEntry* cEntry = mSession->GetEntryL(KMsvGlobalInBoxIndexEntryIdValue);
    CleanupStack::PushL(cEntry);
    TRAPD (error, cEntry->SetEntryL( iMessageId ));
    if (error != KErrNone)
    {
        CleanupStack::PopAndDestroy(); // cEntry
        return;
    }
    CMsvStore* store = cEntry->ReadStoreL();
    CleanupStack::PushL(store);
    CMmsSettings* settings = CMmsSettings::NewL();
    CleanupStack::PushL(settings);
    CMmsHeaders* mmsHeaders = CMmsHeaders::NewL(settings->MmsVersion());
    CleanupStack::PushL(mmsHeaders);
    mmsHeaders->RestoreL(*store);
    //CDesCArray& ccList = mmsHeaders->CcRecipients();

    TPtrC cc;
    TPtrC name;
    TPtrC address;
    for (TInt id = 0; id < mmsHeaders->CcRecipients().MdcaCount(); id++)
    {
        cc.Set( (mmsHeaders->CcRecipients())[id]);
        extractNameAndAddress(cc, name, address);
        // populate address
        ConvergedMessageAddress
        * messageAddress =
            new ConvergedMessageAddress(S60QConversions::s60DescToQString(address),
                S60QConversions::s60DescToQString(name));
        mAddressList.append(messageAddress);
    }

    CleanupStack::PopAndDestroy(3); //  mmsHeaders, settings, store

    CleanupStack::PopAndDestroy(); // cEntry
}

void UniMMSDataPluginPrivate::bccRecipientList(
    ConvergedMessageAddressList& mAddressList)
{
    CMsvEntry* cEntry = mSession->GetEntryL(KMsvGlobalInBoxIndexEntryIdValue);
    CleanupStack::PushL(cEntry);
    TRAPD (error, cEntry->SetEntryL( iMessageId ));
    if (error != KErrNone)
    {
        CleanupStack::PopAndDestroy(); // cEntry
        return;
    }
    CMsvStore* store = cEntry->ReadStoreL();
    CleanupStack::PushL(store);
    CMmsSettings* settings = CMmsSettings::NewL();
    CleanupStack::PushL(settings);
    CMmsHeaders* mmsHeaders = CMmsHeaders::NewL(settings->MmsVersion());
    CleanupStack::PushL(mmsHeaders);
    mmsHeaders->RestoreL(*store);
    //CDesCArray& bccList = mmsHeaders->BccRecipients();

    TPtrC bcc;
    TPtrC name;
    TPtrC address;
    for (TInt id = 0; id < mmsHeaders->BccRecipients().MdcaCount(); id++)
    {
        bcc.Set( (mmsHeaders->BccRecipients())[id]);
        extractNameAndAddress(bcc, name, address);
        // populate address
        ConvergedMessageAddress
        * messageAddress =
            new ConvergedMessageAddress(S60QConversions::s60DescToQString(address),
                S60QConversions::s60DescToQString(name));
        mAddressList.append(messageAddress);
    }

    CleanupStack::PopAndDestroy(3); //  mmsHeaders, settings, store

    CleanupStack::PopAndDestroy(); // cEntry
}

void UniMMSDataPluginPrivate::fromAddress(QString& messageAddress)
{

    TPtrC sender = mMmsClient->Sender();
    messageAddress = S60QConversions::s60DescToQString(sender);
}

UniMessageInfoList UniMMSDataPluginPrivate::attachmentList()
{
    UniMessageInfoList attlist;
    int attcount = mUniDataModel->AttachmentList().Count();

    QString path;
    QString mimetype;
    int size;
    for (int i = 0; i < attcount; i++)
    {
        CUniObject *obj = mUniDataModel->AttachmentList().GetByIndex(i);

        mimetype = S60QConversions::s60Desc8ToQString(obj->MimeType());
        path
        = S60QConversions::s60DescToQString(obj->MediaInfo()->FullFilePath());
        size = obj->Size(EFalse);

        UniMessageInfo *msgobj = new UniMessageInfo(path, size, mimetype);
        attlist << msgobj;
    }

    return attlist;
}

QString UniMMSDataPluginPrivate::messageType()
{
    QString str("mms");
    return str;
}

MsgPriority UniMMSDataPluginPrivate::messagePriority()
{
    return Normal;
}

int UniMMSDataPluginPrivate::attachmentCount()
{
    return mUniDataModel->AttachmentList().Count();
}

bool UniMMSDataPluginPrivate::hasAttachment()
{
    int count = mUniDataModel->AttachmentList().Count();
    if (count > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int UniMMSDataPluginPrivate::objectCount()
{
    return mUniDataModel->ObjectList().Count();
}

UniMessageInfoList UniMMSDataPluginPrivate::objectList()
{
    UniMessageInfoList objlist;
    int objcount = mUniDataModel->ObjectList().Count();

    QString path;
    QString mimetype;
    int size;
    for (int i = 0; i < objcount; i++)
    {
        CUniObject *obj = mUniDataModel->ObjectList().GetByIndex(i);

        mimetype = S60QConversions::s60Desc8ToQString(obj->MimeType());
        path
        = S60QConversions::s60DescToQString(obj->MediaInfo()->FullFilePath());
        size = obj->Size(EFalse);

        UniMessageInfo *msgobj = new UniMessageInfo(path, size, mimetype);
        objlist << msgobj;
    }

    return objlist;
}

/*
 * For now not reqd
 */
int UniMMSDataPluginPrivate::slideCount()
{
    return mUniDataModel->SmilModel().SlideCount();
}

UniMessageInfoList UniMMSDataPluginPrivate::slideContent(TInt slidenum)
{
    UniMessageInfoList slidecontent;

    int slideobjcount = mUniDataModel->SmilModel().SlideObjectCount(slidenum);

    QString path;
    QString mimetype;
    int size;
    for (int i = 0; i < slideobjcount; i++)
    {
        CUniObject *obj = mUniDataModel->SmilModel().GetObjectByIndex(slidenum,
            i);

        mimetype = S60QConversions::s60Desc8ToQString(obj->MimeType());
        path
        = S60QConversions::s60DescToQString(obj->MediaInfo()->FullFilePath());
        size = obj->Size(EFalse);

        UniMessageInfo *msgobj = new UniMessageInfo(path, size, mimetype);
        slidecontent << msgobj;
    }

    return slidecontent;
}

int UniMMSDataPluginPrivate::timeStamp()
{
    QDateTime retTimeStamp;
    TTime timeStamp = mMmsClient->Entry().Entry().iDate;
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    return seconds.Int();
}

void UniMMSDataPluginPrivate::extractNameAndAddress(const TDesC& aMsvAddress,
    TPtrC& aName,
    TPtrC& aAddress)
{
    // For address information separation (start)
    const TUint KMsgMmsAddressStartChar('<');

    // For address information separation (end)
    const TUint KMsgMmsAddressEndChar('>');

    TInt addressStart = aMsvAddress.LocateReverse(KMsgMmsAddressStartChar);
    TInt addressEnd = aMsvAddress.LocateReverse(KMsgMmsAddressEndChar);

    if (addressStart != KErrNotFound && addressEnd != KErrNotFound
            && addressEnd > addressStart)
    {
        // verified address, will be used as selected from contacts manager
        aName.Set(aMsvAddress.Ptr(), addressStart);
        aAddress.Set(aMsvAddress.Mid(addressStart + 1).Ptr(), (addressEnd
                - addressStart) - 1);
        if (!aAddress.Length())
        {
            aAddress.Set(aName);
            aName.Set(KNullDesC); // empty string
        }
    }
    else
    {
        // unverified string, will be used as entered in the header field
        aName.Set(KNullDesC); // empty string
        aAddress.Set(aMsvAddress.Ptr(), aMsvAddress.Length()); // a whole string to address
    }

    if (aName.CompareF(aAddress) == 0)
    {
        aName.Set(KNullDesC); // empty string
    }
}

void UniMMSDataPluginPrivate::RestoreReady(TInt /*aParseResult*/, 
    TInt /*aError*/)
{
}

void UniMMSDataPluginPrivate::addObjectL( int aSlideNum,UniMessageInfo* aInfo )
{
    TInt error = KErrNone;

    HBufC* filePath = S60QConversions::qStringToS60Desc(aInfo->path());
    if(filePath)	
    {	
        CleanupStack::PushL(filePath);	

        RFile file = mUniDataModel->MediaResolver().FileHandleL( *filePath);
        CleanupClosePushL( file );

        mInsertingMedia = mUniDataModel->MediaResolver().CreateMediaInfoL( file );

        //TODO try using the commented code after testing on diff env or on hardware
        //mUniDataModel->MediaResolver().ParseInfoDetailsL( mInsertingMedia, file ); 
        iDRMHelper = CDRMHelper::NewL();   
        mInsertingMedia->ParseInfoDetailsL( file, *iDRMHelper, *this );

        mUniDataModel->SmilModel().AddObjectL( *this, aSlideNum, mInsertingMedia );

        //Since the ownership is transferred dont delete these
        mInsertingMedia = NULL;
        iDRMHelper = NULL;


        CleanupStack::PopAndDestroy( &file );
        CleanupStack::PopAndDestroy( filePath );	
    }	
}

void UniMMSDataPluginPrivate::ObjectSaveReady( TInt /*aError*/ )
{

}

void UniMMSDataPluginPrivate::MediaInfoParsed()
{

}			
void UniMMSDataPluginPrivate::addTextObjectL(int aSlideNum,QString aText )
{
    TInt error = KErrNone;	

    //Delete this always
    delete iEditor;
    iEditor = NULL;

    HBufC* textContent = S60QConversions::qStringToS60Desc(aText);
    if(textContent)
    {	
        CleanupStack::PushL(textContent);		 

        iEditor = new CEikRichTextEditor;   
        iEditor->ConstructL(NULL,0,0,CEikEdwin::ENoAutoSelection|
            CEikEdwin::EOwnsWindow,0,0);
        iEditor->RichText()->InsertL(0,*textContent);

        mUniDataModel->SmilModel().AddTextObjectL( aSlideNum,iEditor);
        CleanupStack::PopAndDestroy( textContent );	
    }	
}		

void UniMMSDataPluginPrivate::addSlideL( int aSlideNum )
{
    TInt error = KErrNone;	
    mUniDataModel->SmilModel().AddSlideL(aSlideNum);
}		

void UniMMSDataPluginPrivate::composeSmilL(CMsvStore& aEditStore)
{	
    TInt error = KErrNone;	

    delete iDom;
    iDom = NULL;

    iDom = mUniDataModel->SmilModel().ComposeL();

    mUniDataModel->SmilList().CreateSmilAttachmentL( *this,aEditStore, iDom );
}

void UniMMSDataPluginPrivate::saveObjects()
{
    mUniDataModel->ObjectList().SaveAll( *this, CMsvAttachment::EMsvFile );	
}

void UniMMSDataPluginPrivate::SmilComposeEvent( TInt /*aError*/ )
{

}

TMsvAttachmentId UniMMSDataPluginPrivate::getSmilAttachmentByIndex() 
{
    return mUniDataModel->SmilList().GetSmilAttachmentByIndex( 0 );
}

void UniMMSDataPluginPrivate::restoreL(CBaseMtm& mtm)
{
    initL();	
    mUniDataModel = CUniDataModel::NewL(mfsSession, mtm);
    mUniDataModel->RestoreL(*this, ETrue);
}

void UniMMSDataPluginPrivate::addAttachmentL( UniMessageInfo* aInfo )
{
    TInt error = KErrNone;

    HBufC* filePath = S60QConversions::qStringToS60Desc(aInfo->path());
    if(filePath)    
    {   
        CleanupStack::PushL(filePath);  

        RFile file = mUniDataModel->MediaResolver().FileHandleL( *filePath);
        CleanupClosePushL( file );

        mInsertingMedia = mUniDataModel->MediaResolver().CreateMediaInfoL( file );

        //TODO try using the commented code after testing on diff env or on hardware
        //mUniDataModel->MediaResolver().ParseInfoDetailsL( mInsertingMedia, file ); 
        iDRMHelper = CDRMHelper::NewL();   
        mInsertingMedia->ParseInfoDetailsL( file, *iDRMHelper, *this );

        mUniDataModel->AddAttachmentL( *this, mInsertingMedia,
            CMsvAttachment::EMsvFile );

        //Since the ownership is transferred dont delete these
        mInsertingMedia = NULL;
        iDRMHelper = NULL;


        CleanupStack::PopAndDestroy( &file );
        CleanupStack::PopAndDestroy( filePath );    
    }   
}

void UniMMSDataPluginPrivate::removeSlide(int aSlideNumber)
{
    mUniDataModel->SmilModel().RemoveSlide(aSlideNumber);
}


CMsvSession* UniMMSDataPluginPrivate::session()
{
    return mSession;
}

