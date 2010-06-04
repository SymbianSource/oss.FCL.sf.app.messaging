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

// INCLUDES
#include <QTimer>
#include <HbTextItem>
#include <HbPushButton>
#include <HbAction>
#include <hbinputeditorinterface.h>
#include <hbmessagebox.h>
#include <cntservicescontact.h>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <telconfigcrkeys.h>        // KCRUidTelephonyConfiguration
#include <centralrepository.h>
#include <HbNotificationDialog>
#include <commonphoneparser.h>      // Common phone number validity checker
#include <xqconversions.h>

// USER INCLUDES
#include "debugtraces.h"
#include "msgunieditoraddress.h"
#include "msgunifiededitorlineedit.h"
#include "msgmonitor.h"
#include "unieditorgenutils.h"

const QString PBK_ICON("qtg_mono_contacts");
const QString SEND_ICON("qtg_mono_send");
const QString replacementStr("; ");

// Constants
const int KDefaultGsmNumberMatchLength = 7;  //matching unique ph numbers
#define LOC_SMS_RECIPIENT_LIMIT_REACHED hbTrId("txt_messaging_dialog_number_of_recipients_exceeded")
#define LOC_MMS_RECIPIENT_LIMIT_REACHED hbTrId("txt_messaging_dpopinfo_unable_to_add_more_recipien")
#define LOC_DIALOG_OK hbTrId("txt_common_button_ok")
#define LOC_BUTTON_CANCEL hbTrId("txt_common_button_cancel")
#define LOC_INVALID_RECIPIENT hbTrId("txt_messaging_dialog_invalid_recipient_found")

MsgUnifiedEditorAddress::MsgUnifiedEditorAddress( const QString& label,
                                                  QGraphicsItem *parent ) :
MsgUnifiedEditorBaseWidget(parent),
mSkipMaxRecipientQuery(false),
mAboutToExceedMaxSmsRecipients(false),
mAboutToExceedMaxMmsRecipients(false),
mExceedsMaxMmsRecipientsBy(0)
{
    this->setContentsMargins(0,0,0,0);

    mLaunchBtn = new HbPushButton(this);
    HbStyle::setItemName(mLaunchBtn,"launchBtn");
    connect(mLaunchBtn,SIGNAL(clicked()),this,SLOT(fetchContacts()));

    mLaunchBtn->setIcon(HbIcon(PBK_ICON));

    mAddressEdit = new MsgUnifiedEditorLineEdit(label,this);
    HbStyle::setItemName(mAddressEdit,"addressField");

    mAddressEdit->setMaxRows(40);
    connect(mAddressEdit, SIGNAL(contentsChanged(const QString&)),
            this, SLOT(onContentsChanged(const QString&)));

    // add "Send" action in VKB
    HbEditorInterface editorInterface(mAddressEdit);
    mAddressEdit->setInputMethodHints(Qt::ImhPreferNumbers);
    HbAction *sendAction = new HbAction(HbIcon(SEND_ICON), QString(),this);
    connect(sendAction, SIGNAL(triggered()),this, SIGNAL(sendMessage()));
    editorInterface.addAction(sendAction);
    }

MsgUnifiedEditorAddress::~MsgUnifiedEditorAddress()
{
	//TODO: Should remove this code depending on orbit's reply whether it is needed
	//to unregister the same plugin registered on two different widgets twice.
    //style()->unregisterPlugin(mPluginPath);
}

void MsgUnifiedEditorAddress::fetchContacts()
{
    QList<QVariant> args;
    QString serviceName("com.nokia.services.phonebookservices");
    QString operation("fetch(QString,QString,QString)");
    XQAiwRequest* request;
    XQApplicationManager appManager;
    request = appManager.create(serviceName, "Fetch", operation, true); // embedded
    if ( request == NULL )
        {
        return;
        }

    // Result handlers
    connect (request, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleOk(const QVariant&)));
    connect (request, SIGNAL(requestError(int,const QString&)), this, SLOT(handleError(int,const QString&)));

    args << QString(tr("Phonebook"));
    args << KCntActionAll;
    args << KCntFilterDisplayAll;

    request->setArguments(args);
    request->send();
    delete request;
}

void MsgUnifiedEditorAddress::handleOk(const QVariant& value)
{
   CntServicesContactList contactList =
           qVariantValue<CntServicesContactList>(value);
    int count = contactList.count();

    ConvergedMessageAddressList addrlist;
    for(int i = 0; i < count; i++ )
    {
        ConvergedMessageAddress* address =
                new ConvergedMessageAddress();
        address->setAddress(contactList[i].mPhoneNumber);
        address->setAlias(contactList[i].mDisplayName);
        addrlist << address;
    }
    setAddresses(addrlist);
}

void MsgUnifiedEditorAddress::handleError(int errorCode, const QString& errorMessage)
{
    Q_UNUSED(errorMessage)
    Q_UNUSED(errorCode)
}

// ----------------------------------------------------------------------------
// MsgUnifiedEditorAddress::addresses
// @see header
// ----------------------------------------------------------------------------
ConvergedMessageAddressList MsgUnifiedEditorAddress::addresses(
        bool removeDuplicates)
{
#ifdef _DEBUG_TRACES_
    qCritical() << "MsgUnifiedEditorAddress::addresses start";
#endif
    // sync-up map to account for user-actions on edit-field
    syncDeletionsToMap();
    syncAdditionsToMap();

    ConvergedMessageAddressList addresses;
    if(removeDuplicates)
    {
        QStringList uniqueAddr;
        uniqueAddr = uniqueAddressList();
        foreach(QString addr, uniqueAddr)
        {
            ConvergedMessageAddress* address = new ConvergedMessageAddress;
            address->setAddress(addr);
            address->setAlias(mAddressMap.value(addr));
            addresses.append(address);
        }
    }
    else
    {
        QMap<QString, QString>::iterator i = mAddressMap.begin();
        while (i != mAddressMap.end())
        {
            ConvergedMessageAddress* address = new ConvergedMessageAddress;
            address->setAddress(i.key());
            address->setAlias(i.value());
            addresses.append(address);
            i++;
        }
    }
#ifdef _DEBUG_TRACES_
    qCritical() << "MsgUnifiedEditorAddress::addresses end";
#endif
    return addresses;
}

int MsgUnifiedEditorAddress::addressCount()
{
    return mAddressEdit->addresses().count();
}

void MsgUnifiedEditorAddress::setAddresses(ConvergedMessageAddressList addrlist)
{
    // ensure flags are reset before starting the addr addition
    mAboutToExceedMaxSmsRecipients = false;
    mAboutToExceedMaxMmsRecipients = false;
    mExceedsMaxMmsRecipientsBy = 0;

    // first, we check if MMS max-recipient count will exceed
    int count = addrlist.count();
	int futureCount = count + MsgMonitor::msgAddressCount();
	if(futureCount > MsgMonitor::maxMmsRecipients())
	{
	    mAboutToExceedMaxMmsRecipients = true;
	    mExceedsMaxMmsRecipientsBy =
	            futureCount - MsgMonitor::maxMmsRecipients();
	}
	// else, check if SMS max-recipient count will exceed
	else if(!mSkipMaxRecipientQuery)
	{
	    futureCount = count + addressCount();
	    if( (addressCount() <= MsgMonitor::maxSmsRecipients()) &&
	        (futureCount > MsgMonitor::maxSmsRecipients()) )
	    {
	        mAboutToExceedMaxSmsRecipients = true;
	    }
	}


    for(int i = 0; i < count; i++ )
    {
        mAddressMap.insertMulti(addrlist[i]->address(), addrlist[i]->alias());
        if(!addrlist[i]->alias().isEmpty())
        {
            mAddressEdit->setText(addrlist[i]->alias());
        }
        else
        {
            mAddressEdit->setText(addrlist[i]->address(), false);
        }
    }

    // addition operation complete, reset flags
    mAboutToExceedMaxSmsRecipients = false;
    mAboutToExceedMaxMmsRecipients = false;
    mExceedsMaxMmsRecipientsBy = 0;
}

int MsgUnifiedEditorAddress::contactMatchDigits()
{
    // Read the amount of digits to be used in contact matching
    // The key is owned by PhoneApp
    int matchDigitCount = 0;
    TRAP_IGNORE(
        CRepository* repository = CRepository::NewLC(KCRUidTelConfiguration);
        if ( repository->Get(KTelMatchDigits, matchDigitCount) == KErrNone )
        {
            // Min is 7
            matchDigitCount = Max(matchDigitCount, KDefaultGsmNumberMatchLength);
        }
        CleanupStack::PopAndDestroy(); // repository
    );
    return matchDigitCount;
}

void MsgUnifiedEditorAddress::onContentsChanged(const QString& text)
{
    // Max MMS recipient count check
    if( mAboutToExceedMaxMmsRecipients ||
        (MsgMonitor::msgAddressCount() >= MsgMonitor::maxMmsRecipients()) )
    {
        if(mAboutToExceedMaxMmsRecipients)
        {// show discreet note only once
            --mExceedsMaxMmsRecipientsBy;
            if(!mExceedsMaxMmsRecipientsBy)
            {
                HbNotificationDialog::launchDialog(
                        LOC_MMS_RECIPIENT_LIMIT_REACHED);
            }
            resetToPrevious();
        }
        else
        {
            // update monitor data
            emit contentChanged();
            if(MsgMonitor::msgAddressCount() > MsgMonitor::maxMmsRecipients())
            {
                HbNotificationDialog::launchDialog(
                        LOC_MMS_RECIPIENT_LIMIT_REACHED);
                resetToPrevious();
                // reset monitor data
                emit contentChanged();
            }
            else
            {
                mPrevBuffer = text;
            }
        }
        return;
    }

    // Max SMS recipient count check
    if( !mSkipMaxRecipientQuery &&
        (MsgMonitor::messageType() == ConvergedMessage::Sms) &&
        (mAddressEdit->addresses().count() > MsgMonitor::maxSmsRecipients()) )
    {
        // when we show this dialog, we don't want the intermediate states
        // to be signalled to us
        disconnect(mAddressEdit, SIGNAL(contentsChanged(const QString&)),
                this, SLOT(onContentsChanged(const QString&)));
        QTimer::singleShot(50, this, SLOT(handleRecipientLimitReached()));
    }
    else
    {
        if(!mAboutToExceedMaxSmsRecipients)
        {// remember addresses before the block insertion started
            mPrevBuffer = text;
        }
        emit contentChanged();
    }
}

void MsgUnifiedEditorAddress::handleRecipientLimitReached()
{
    HbMessageBox* dlg = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFocusPolicy(Qt::NoFocus);
    dlg->setTimeout(HbPopup::NoTimeout);

    dlg->setText(LOC_SMS_RECIPIENT_LIMIT_REACHED);

    HbAction* okAction = new HbAction(LOC_DIALOG_OK,dlg);
    dlg->addAction(okAction);

    HbAction* cancelAction = new HbAction(LOC_BUTTON_CANCEL,dlg);
    dlg->addAction(cancelAction);

    dlg->open(this,SLOT(onMaxRecipientsReached(HbAction*)));
    // reconnect to get back updates
    connect(mAddressEdit, SIGNAL(contentsChanged(const QString&)),
            this, SLOT(onContentsChanged(const QString&)));
    emit contentChanged();
}

void MsgUnifiedEditorAddress::syncDeletionsToMap()
{
    // get address list from edit-field
    QStringList addrList = mAddressEdit->addresses();

    QMap<QString, QString>::iterator i = mAddressMap.begin();
    while (i != mAddressMap.end())
    {
        // check if either key or value is present in the list
        if( !(addrList.contains(i.value(), Qt::CaseSensitive) ||
              addrList.contains(i.key(), Qt::CaseSensitive)) )
        {// if none are present, then delete entry from map
            i = mAddressMap.erase(i);
        }
        else
        {
            ++i;
        }
    }
}

void MsgUnifiedEditorAddress::syncAdditionsToMap()
{
    // remove already mapped addresses from edit-field
    QStringList userInputAddrList(mAddressEdit->addresses());
    QStringList mapAddrList = mAddressMap.values();
    mapAddrList << mAddressMap.keys();
    foreach(QString addr, mapAddrList)
    {
        userInputAddrList.removeOne(addr);
    }

    // add the unmapped addresses to address-map
    foreach(QString addr, userInputAddrList)
    {
        mAddressMap.insertMulti(addr, QString());
    }
}

QStringList MsgUnifiedEditorAddress::uniqueAddressList()
{
    int matchDigitCount = MsgUnifiedEditorAddress::contactMatchDigits();
    QStringList mapAddrList = mAddressMap.keys();
    for(int j = 0;j<mapAddrList.count()-1;j++)
    {
        for(int i =j+1;i<mapAddrList.count();i++)
        {
            if(0 == mapAddrList[j].right(matchDigitCount).compare(mapAddrList[i].right(matchDigitCount)))
            {
               mapAddrList.removeAt(i);
               i--;
            }
        }
    }
    return mapAddrList;
}

// ----------------------------------------------------------------------------
// MsgUnifiedEditorAddress::skipMaxRecipientQuery
// @see header
// ----------------------------------------------------------------------------
void MsgUnifiedEditorAddress::skipMaxRecipientQuery(bool skip)
{
    mSkipMaxRecipientQuery = skip;
}

void MsgUnifiedEditorAddress::setFocus()
{
    mAddressEdit->setFocus(Qt::MouseFocusReason);
}

// ----------------------------------------------------------------------------
// MsgUnifiedEditorAddress::resetToPrevious
// @see header
// ----------------------------------------------------------------------------
void MsgUnifiedEditorAddress::resetToPrevious()
{
    // when we do this reset operation, we don't want the intermediate states
    // to be signalled to us
    disconnect(mAddressEdit, SIGNAL(contentsChanged(const QString&)),
            this, SLOT(onContentsChanged(const QString&)));

    mAddressEdit->clearContent();
    QStringList list = mPrevBuffer.split(replacementStr,
            QString::SkipEmptyParts);
    int count = list.count();
    QStringList valList = mAddressMap.values();
    for(int i=0; i<count; i++)
    {
        QString addr = list.at(i);
        if(valList.contains(addr))
        {
            mAddressEdit->setText(addr);
        }
        else
        {
            mAddressEdit->setText(addr, false);
        }
    }
    syncDeletionsToMap();
    connect(mAddressEdit, SIGNAL(contentsChanged(const QString&)),
            this, SLOT(onContentsChanged(const QString&)));
}

// ----------------------------------------------------------------------------
// MsgUnifiedEditorAddress::onMaxRecipientsReached
// @see header
// ----------------------------------------------------------------------------
void MsgUnifiedEditorAddress::onMaxRecipientsReached(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        // accept new content, update prev-buffer
        mPrevBuffer = mAddressEdit->content();
    }
    else {
        // reject the new content, keep the old
        resetToPrevious();
    }
}

// ----------------------------------------------------------------------------
// MsgUnifiedEditorAddress::validateContacts
// @see header
// ----------------------------------------------------------------------------
bool MsgUnifiedEditorAddress::validateContacts()
{
    UniEditorGenUtils* genUtils = new UniEditorGenUtils;

    // sync-up map to account for user-actions on address-field
    syncDeletionsToMap();
    syncAdditionsToMap();

    // get the list of contacts in address-field
    QStringList fieldAddresses(mAddressEdit->addresses());

    bool allValid = true;
    foreach(QString addr, fieldAddresses)
    {
        // run address validation only if address is unmapped
        // (i.e. user-inserted)
        if(mAddressMap.contains(addr))
        {
            // 1. perform number validation
            allValid = CommonPhoneParser::IsValidPhoneNumber(
                    *XQConversions::qStringToS60Desc(addr),
                    CommonPhoneParser::ESMSNumber );

            // 2. if number validity fails, then perform email addr validation
            if( !allValid &&
                (MsgMonitor::messageType() == ConvergedMessage::Mms) )
            { // additional check for MMS only
                allValid = genUtils->IsValidEmailAddress(
                        *XQConversions::qStringToS60Desc(addr) );
            }

            if(!allValid)
            {
                mAddressEdit->highlightInvalidString(addr);
                QString invalidAddrStr =
                        QString(LOC_INVALID_RECIPIENT).arg(addr);
                HbMessageBox* dlg = new HbMessageBox(invalidAddrStr,
                        HbMessageBox::MessageTypeInformation);
                dlg->setDismissPolicy(HbPopup::TapInside);
                dlg->setTimeout(HbPopup::NoTimeout);
                dlg->setAttribute(Qt::WA_DeleteOnClose, true);
                dlg->open(this, SLOT(handleInvalidContactDialog(HbAction*)));
                break;
            }
        }
    }
    delete genUtils;
    return allValid;
}

void MsgUnifiedEditorAddress::handleInvalidContactDialog(
        HbAction* act)
{
    Q_UNUSED(act);
    QTimer::singleShot(250, this, SLOT(setFocus()));
}

Q_IMPLEMENT_USER_METATYPE(CntServicesContact)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(CntServicesContactList)

//EOF
