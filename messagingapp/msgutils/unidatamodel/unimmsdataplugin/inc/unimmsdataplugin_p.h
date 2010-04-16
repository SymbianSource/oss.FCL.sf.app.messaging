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
 * Description: MMS data model plugin.
 *
 */

#ifndef UNIMMSDATAPLUGIN_P_H_
#define UNIMMSDATAPLUGIN_P_H_

//SYSTEM INCLUDE
#include <f32file.h>  //for FileSession RFs
#include <msvapi.h>
#include <e32base.h>

#include "unidatamodelplugininterface.h"
#include "UniDataModel.h"

//Forward Declaration
class CMmsClientMtm;
class CUniDataModel;
class CMsvSession;
class CClientMtmRegistry;
class CMsgMediaInfo;
class CMDXMLDocument;
class CDRMHelper;
class MMediaInfoObserver;
class SessionEventHandler;

/**
 * Symbian specific implementation of the uni SMS plugin
 */
class UniMMSDataPluginPrivate : public MUniDataModelObserver,
                                public MUniObjectSaveObserver,
                                public MUniSmilComposeObserver,
                                public MMediaInfoObserver
{
public:
    /**
     * Constructor
     */
    UniMMSDataPluginPrivate();

    /**
     * Destructor
     */
    ~UniMMSDataPluginPrivate();

    /**
     * Sets the id of the message whose details needs to be extacted.
     * @param TMsvId of the message.
     */
    int setMessageId(int messageId);

    /** Reset the datamodel
     * Resets the data model to be used again
     */
    void reset();

    /**
     * Get the body of the message
     * @param aBodyText , a QString to hold the message body
     */
    void body(QString& aBodyText);

    /**
     * Size of the message
     * @return message size
     */
    int messageSize();

    /**
     * List of the message To recipients 
     * @param  mAddressList, list of all addresses in the message
     */
    void toRecipientList(ConvergedMessageAddressList& mAddressList);

    /**
     * List of the message CC recipients 
     * @param  mAddressList, list of all addresses in the message
     */
    void ccRecipientList(ConvergedMessageAddressList& mAddressList);

    /**
     * List of the message  bCC recipients 
     * @param  mAddressList, list of all addresses in the message
     */
    void bccRecipientList(ConvergedMessageAddressList& mAddressList);

    /**
     * The from address of a message
     * @param messageAddress
     */
    void fromAddress(QString& messageAddress);

    /**
     * List of attachments in the message.
     * @return the attachment list for the message.
     */
    UniMessageInfoList attachmentList();

    /**
     * Type of the message (SMS/MMS etc..)
     * @return message type
     */
    QString messageType();

    /**
     * Priority of the message (low/medium/high)
     * @return MsgPriority.
     */
    MsgPriority messagePriority();

    /**
     * Number of attachments with the message
     * @return count of attachment
     */
    int attachmentCount();

    /**
     * To check if the message has attachments
     * @return true/false
     */
    bool hasAttachment();

    /**
     * Number of objects asociated with a MMS message
     * @return object count
     */
    int objectCount();

    /**
     * The MMS object list asociated with a MMS message
     * @return object list 
     */
    UniMessageInfoList objectList();

    /**
     * Number of slides asociated with a MMS message
     * @return slide count
     */
    int slideCount();

    /**
     * The content of a Slide in case of a MMS message
     * @param list of messageinfo objects.
     */
    UniMessageInfoList slideContent(TInt slidenum);

    /**
     * Time stamp of the message when it was created/recieved.
     * @param mesage time
     */
    int timeStamp();

    /**
     * Add object into smil 
     * @param slide number
     * @param UniMessageInfo
     */    
    void addObjectL( int aslideNum,UniMessageInfo* aInfo );    

    /**
     * Add text object into smil 
     * @param slide number
     * @param text
     */    
    void addTextObjectL(int aSlideNum,QString aText );

    /**
     * Add slide into smil model 
     * @param slide number
     */
    void addSlideL( int aSlideNum );

    /**
     * Compose smil attachment 
     * @param CMsvStore
     */
    void composeSmilL(CMsvStore& aEditStore);

    /**
     * Save objects inside model 
     */
    void saveObjects();

    /**
     * get the smil attachment index 
     * @return TMsvAttachmentId
     */
    TMsvAttachmentId getSmilAttachmentByIndex();

    /**
     * Restore the model for editing only 
     * @param CBaseMtm
     */
    void restoreL(CBaseMtm& mtm);

    /**
     * Add attachment 
     * @param UniMessageInfo
     */
    void addAttachmentL( UniMessageInfo* aInfo );

    /**
     * Remove slide from smil model 
     * @param slide number
     */
    void removeSlide(int aSlideNumber);
	
	/**
     * Subject of the message
     * @QString subject
     */
	QString subject();
	 
	/**
	 * Session with the Messaging server
	 * @return CMsvSession*
	 */
    CMsvSession* session();

public:

    /**
     * @see MUniObjectSaveObserver
     */
    void ObjectSaveReady( TInt aError );

    /**
     * @see MUniSmilComposeObserver
     */
    void SmilComposeEvent( TInt aError );

    /**
     * @see MMediaInfoObserver
     */    
    void MediaInfoParsed();

    /**
     * @see MUniDataModelObserver
     */    
    void RestoreReady(TInt aParseResult, TInt aError);

private:
    /**
     *  Extracts the name and adress from the CMsvRecipientList
     */
    void extractNameAndAddress(const TDesC& aMsvAddress, TPtrC& aName,
        TPtrC& aAddress);
    void initL();

    void setMessageIdL(int messageId);
private:

    CMsvSession* mSession;
    CClientMtmRegistry* mMtmRegistry;

    CUniDataModel *mUniDataModel;

    RFs mfsSession;

    TRequestStatus iStatus;
    CMmsClientMtm* mMmsClient;
    CMsgMediaInfo*    mInsertingMedia;
    CEikRichTextEditor *iEditor;
    CMDXMLDocument* iDom;
    CDRMHelper *iDRMHelper;
    TMsvId iMessageId;
    TBool done;    
    SessionEventHandler *iSessionHandler;
};

#endif /* UNIMMSDATAPLUGIN_P_H_ */
