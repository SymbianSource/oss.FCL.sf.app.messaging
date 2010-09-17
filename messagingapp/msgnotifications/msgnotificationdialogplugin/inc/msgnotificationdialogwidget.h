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
 * Description: Message notification plugin widget.
 *
 */

#ifndef MSGNOTIFICATIONDIALOGWIDGET_H
#define MSGNOTIFICATIONDIALOGWIDGET_H

#include <QObject>
#include <QVariantMap>
#include <QRunnable>
#include <hbdevicedialoginterface.h>
#include <hbdevicedialog.h>
#include <hbnotificationdialog.h>


/**
 * Class for sending service request
 */
class ServiceRequestSenderTask :public QObject,public QRunnable
{
    Q_OBJECT
    
public:
    /**
     * Constructor
     */
    ServiceRequestSenderTask(qint64 conversationId);
    
    /**
     * Destructor
     */
    ~ServiceRequestSenderTask();
     
    /**
     * create and send service request
     */
     void run();

private slots:

    /**
     * Slot invoked after Conversation view is launched.
     */
     void onRequestCompleted(const QVariant& value);
     
    /**
     * Slot invoked if error occurred during launch of CV.
     */
     void onRequestError(int errorCode, const QString& errorMessage);
          
signals:

	/**
	* Signal is emitted to indicate that Notification Dialog 
	* can now be closed.
	*/
     void serviceRequestCompleted();
     
private: 
     qint64 mConvId;
};

/**
 * Message notification widget class. 
 * Widget shown for the new message notifications. 
 * Part of messagenotificationplugin.
 */
class MsgNotificationDialogWidget : public HbNotificationDialog, 
                                    public HbDeviceDialogInterface
{
    Q_OBJECT
    
public:
    
    /** 
     * Constructor 
     * @param parameters variant map list
     */
    MsgNotificationDialogWidget(const QVariantMap &parameters);

    /**
     * @see HbDeviceDialogInterface
     */
    bool setDeviceDialogParameters(const QVariantMap &parameters);
    
    /**
     * @see HbDeviceDialogInterface
     */
    int deviceDialogError() const;

    /**
     * @see HbDeviceDialogInterface
     */
    void closeDeviceDialog(bool byClient);
    
    /**
     * @see HbDeviceDialogInterface
     */
    HbDialog *deviceDialogWidget() const;

public slots:
    /**
     * Called when the widget is tapped. 
     * This launches the conversation view.
     */
    void widgetActivated();

private:
    /**
     * Prepares the displaname to be shown in the notification.
     * This extracts firsname, lastname, nick name and address
     * and prepares the display name.
     * @param parameters variant map value list
     */
    void prepareDisplayName(const QVariantMap &parameters);
    
    /**
     * Hide event call back function
     * Called when widget is about to hide. Closing effect has ended.
     * @param event QHideEvent
     */
    void hideEvent(QHideEvent *event);
    
    /**
     * Show event call back function
     * Called when widget is about to show
     * @param event QShowEvent
     */
    void showEvent(QShowEvent *event);

     /** 
      * Extracts the information and constrcuts the widget
      * @param parameters QVariantMap values
      */
    bool constructDialog(const QVariantMap &parameters);
signals:
   /**
    * Signal emitted when dialog is closed
    */
    void deviceDialogClosed();
    
    /**
     * Device dialog data signal
     */
    void deviceDialogData(QVariantMap data);

private:
    Q_DISABLE_COPY(MsgNotificationDialogWidget)

     /**
      * Hold the last error 
      */
     int mLastError;
    
     /**
      * Show event has come or not
      */
     bool mShowEventReceived;
    
     /**
      * Current conversation id.
      */
     qint64 mConversationId;
};

#endif // MSGNOTIFICATIONDIALOGWIDGET_P_H
