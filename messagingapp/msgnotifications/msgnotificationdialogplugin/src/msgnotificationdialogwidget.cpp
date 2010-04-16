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
#include "convergedmessage.h"

#include "msgnotificationdialogpluginkeys.h"
#include "msgnotificationdialogwidget.h"

const int NoError = 0;
const int ParameterError = 10000;


//----------------------------------------------------------------

class ServiceRequestSenderTask : public QRunnable
 {
public:
     ServiceRequestSenderTask(qint64 conversationId):mConvId(conversationId)  {}
     
     void run()
     {
     XQServiceRequest snd(
            "com.nokia.services.hbserviceprovider.conversationview",
                "open(qint64)",false);
		  snd << mConvId;
		  bool res=snd.send();    
     }
     
     ~ServiceRequestSenderTask()
     {     
     }
     
 private: 
 	qint64 mConvId;    
 };

// ------------------------------------------------------------------------



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
        HbIcon icon(":/sms.svg");
        setIcon(icon);
        setText(parameters.value(KMessageBodyKey).toString());    
        }
    else if(messageType == ECsMMS)
        {
        HbIcon icon(":/mms.svg");
        setIcon(icon);
        setText(parameters.value(KMessageSubjectKey).toString());       
        }
    else if(messageType == ECsRingingTone)
        {
        HbIcon icon(":/ringingtone.svg");
        setIcon(icon);
        setText(parameters.value(KMessageBodyKey).toString());      
        }
    else
        {
        HbIcon icon(":/sms.svg"); // show default for other message types
        setIcon(icon);
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
    QString firstName = parameters.value(KFirstNameKey).toString();
    QString lastName = parameters.value(KLastNameKey).toString();
    QString contactAddress = parameters.value(KContactAddressKey).toString();
    QString nickName = parameters.value(KNickNameKey).toString();
    
    QString displayName;
    
    if (!nickName.isEmpty())
    {
        displayName.append(nickName);
    }
    else if ( firstName.isEmpty() && lastName.isEmpty())
    {
        displayName.append(contactAddress);
    }
    else if (lastName.isEmpty() && !firstName.isEmpty())
    {
        displayName.append(firstName);
    }
    else if (firstName.isEmpty() && !lastName.isEmpty())
    {
        displayName.append(lastName);
    }
    else
    {
        // If both first Name and last name are present
        displayName.append(firstName);
        displayName.append(" ");
        displayName.append(lastName);
    }
    
    // set the display name
    setTitle(displayName);
}
