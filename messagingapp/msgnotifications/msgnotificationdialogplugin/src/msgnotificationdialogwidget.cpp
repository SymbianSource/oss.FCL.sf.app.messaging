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
 * Description: Widget class for Notificaiton Dialog Plugin
 *
 */
#include <QThreadPool>
#include <QRunnable>
#include "debugtraces.h"

#include <ccsdefs.h>
#include <QIcon>
#include <QVariant>
#include <QList>
#include <hbicon.h>
#include <hbpopup.h>
#include <xqservicerequest.h>
#include <xqaiwrequest.h>
#include <xqappmgr.h>

#include "convergedmessage.h"

#include "msgnotificationdialogpluginkeys.h"
#include "msgnotificationdialogwidget.h"

const int NoError = 0;
const int ParameterError = 10000;

const QString BT_ICON("qtg_large_bluetooth");
const QString SMS_ICON("qtg_large_new_message");
const QString MMS_ICON("qtg_large_new_message");
const QString RINGTONE_ICON("qtg_large_tone");
const QString VCARD_ICON("qtg_large_new_message");
// ----------------------------------------------------------------------------
// ServiceRequestSenderTask::ServiceRequestSenderTask
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------   
ServiceRequestSenderTask::ServiceRequestSenderTask(qint64 conversationId):
mConvId(conversationId)
     {     
     }

// ----------------------------------------------------------------------------
// ServiceRequestSenderTask::~ServiceRequestSenderTask
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------   
ServiceRequestSenderTask::~ServiceRequestSenderTask()
     {     
     }
     
// ----------------------------------------------------------------------------
// ServiceRequestSenderTask::run
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------   
void ServiceRequestSenderTask::run()
     {
     QList<QVariant> args;
     QString serviceName("com.nokia.services.hbserviceprovider");
     QString operation("open(qint64)");
     XQAiwRequest* request;
     XQApplicationManager appManager;
     request = appManager.create(serviceName, "conversationview", operation, false); // not embedded
     if ( request == NULL )
         {
         return;       
         }
     args << QVariant(mConvId);
     request->setArguments(args);
     request->send();
     delete request;
     }


// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::MsgNotificationDialogWidget
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
MsgNotificationDialogWidget::MsgNotificationDialogWidget(
                                                const QVariantMap &parameters)
: HbNotificationDialog(),
mLastError(NoError),
mShowEventReceived(false),
mConversationId(-1)
{
    constructDialog(parameters);
}

                                                
// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::setDeviceDialogParameters
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
bool MsgNotificationDialogWidget::setDeviceDialogParameters(
                                                const QVariantMap &parameters)
{
   return constructDialog(parameters);
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::constructDialog
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
bool MsgNotificationDialogWidget::constructDialog(
                                                const QVariantMap &parameters)
    {
    // Set parameters 
    mLastError = NoError;

    // if conversation id is not proper return false
    mConversationId = parameters.value(KConversationIdKey).toLongLong(); 
    if( mConversationId <= 0)
        {
        mLastError = ParameterError;
        return false;    
        }

    prepareDisplayName(parameters);

    int messageType = parameters.value(KMessageTypeKey).toInt();
    if( messageType == ECsSMS)
        {
        HbIcon icon(SMS_ICON);
        setIcon(icon);
        QString messageBody;
        messageBody = parameters.value(KMessageBodyKey).toString();
        messageBody.replace(QChar::ParagraphSeparator, QChar::LineSeparator);
        messageBody.replace('\r', QChar::LineSeparator);
        setText(messageBody);    
        }
    else if(messageType == ECsMMS)
        {
        HbIcon icon(MMS_ICON);
        setIcon(icon);
        setText(parameters.value(KMessageSubjectKey).toString());       
        }
    else if(messageType == ECsMmsNotification)
        {
        HbIcon icon(MMS_ICON);
        setIcon(icon);
        setText(parameters.value(KMessageSubjectKey).toString());       
        }
    else if(messageType == ECsRingingTone)
        {
        HbIcon icon(RINGTONE_ICON);
        setIcon(icon);
        setText(parameters.value(KMessageBodyKey).toString());      
        }
    else if(messageType == ECsBlueTooth)
        {
        HbIcon icon(BT_ICON); // show default for other message types
        setIcon(icon);
        setText(parameters.value(KMessageBodyKey).toString());    
        }
    else if(messageType == ECsBioMsg_VCard)
        {
        HbIcon icon(VCARD_ICON); // show default for other message types
        setIcon(icon);
        setText(parameters.value(KMessageBodyKey).toString());    
        }
    else
        {
        setText(parameters.value(KMessageBodyKey).toString());    
        }
    
    // enable touch activation and connect to slot
    enableTouchActivation(true);
    connect(this, SIGNAL(activated()), this, SLOT(widgetActivated()));

    // set the standard timeout value, that is used by default notificaitons dialogs
    setTimeout(HbPopup::StandardTimeout);

    return true;
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::deviceDialogError
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
int MsgNotificationDialogWidget::deviceDialogError() const
{    
    // Get error
    return mLastError;
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::closeDeviceDialog
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
void MsgNotificationDialogWidget::closeDeviceDialog(bool byClient)
{
    // Close device dialog
    Q_UNUSED(byClient);
    close();
    // If show event has been received, close is signalled from hide event. If not,
    // hide event does not come and close is signalled from here.
    if (!mShowEventReceived) {
        emit deviceDialogClosed();
    }
    
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::deviceDialogWidget
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
HbDialog *MsgNotificationDialogWidget::deviceDialogWidget() const
{    
    // Return display widget
    return const_cast<MsgNotificationDialogWidget*>(this);
}


// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::hideEvent
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
void MsgNotificationDialogWidget::hideEvent(QHideEvent *event)
{
    HbNotificationDialog::hideEvent(event);
    emit deviceDialogClosed();
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::showEvent
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
void MsgNotificationDialogWidget::showEvent(QShowEvent *event)
{
    HbNotificationDialog::showEvent(event);
    mShowEventReceived = true;
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::widgetActivated
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
void MsgNotificationDialogWidget::widgetActivated()
{
QThreadPool::globalInstance()->start(
        	new ServiceRequestSenderTask(mConversationId));  
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::prepareDisplayName
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
void MsgNotificationDialogWidget::prepareDisplayName(
                                                const QVariantMap &parameters)
{
    //Set the Contact Name/Number
    QString displayName = parameters.value(KDisplayNameKey).toString();
    QString contactAddress = parameters.value(KContactAddressKey).toString();
    
    if (displayName.isEmpty())
    {
        setTitle(contactAddress);
    }
    else
    {
        setTitle(displayName);
    }    
}
