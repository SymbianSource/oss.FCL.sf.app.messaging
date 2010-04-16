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
* Description:  Offers message creation and sending services.
 *
*/


#include "shareuiprivate.h"
#include <QVariant>
#include <QDir>
#include <HbPushButton>
#include <HbListWidget>
#include <HbMainWindow>
#include <HbNotificationDialog>
#include <xqserviceglobal.h>

//Localized Constants
#define LOC_SEND_SELECTED_ITEM hbTrId("txt_messaging_title_send_selected_item")
#define LOC_BUTTON_CANCEL hbTrId("txt_common_button_cancel")

ShareUiPrivate::ShareUiPrivate()
    {
    
    }

/**
 * Destructor.
 */
ShareUiPrivate::~ShareUiPrivate()
    {
    if(mContentItemModel)
        {
        delete mContentItemModel;
        }
    if(mContentListView)
        {
        delete mContentListView;
        }
    if(mSharePopup)
        {
        delete mSharePopup;
        }
    mIndexActionMap.clear();
    mAiwRequestList.clear();
    mFileList.clear();
    }

/**
 * initialize and start the dialog
 * 
 * @param fileList list of tokens that would allow file manipulation
 * @return bool error false if dialog initialization failed, true otherwise.
 * 
 */
bool ShareUiPrivate::init(QStringList& fileList, bool embedded)
    {    
    bool retval = false;
    mFileList.clear();
    
    if(fileList.count())
        {
        for(int i = 0; i < fileList.count(); i++)
            {
            mFileList.append(QDir::toNativeSeparators(fileList[i]));
            }
        
        mIsEmbedded = embedded;
    
        initializeUi();
    
        QList<XQAiwInterfaceDescriptor> serviceDescriptorList;
        mIndexActionMap.clear();
        mAiwRequestList.clear();
        if(fetchServiceDescriptors(serviceDescriptorList))
            {
            
            HbAction* action;
            
            for(int i = 0; i < serviceDescriptorList.count() ; i++)
                {

                if((action = fetchServiceAction(serviceDescriptorList[i])) != NULL)
                    {
                    updateShareUiDialogList(action);
                    }
                }
            retval = true;
            }
        else
            {
            updateShareUiDialogList(NULL);
            }
    
        enableUi();
        }
    return retval;
    }

/**
  * Creates the view for the sendui dialog.
  *  
  * @return bool true if initialization was successful, false otherwise.
  */

void ShareUiPrivate::initializeUi()
    {
    mSharePopup = new HbDialog();
    mSharePopup->setHeadingWidget(new HbLabel(LOC_SEND_SELECTED_ITEM));
    mSharePopup->setTimeout(HbDialog::NoTimeout);  
    mContentListView = new HbListView(mSharePopup);

    mContentItemModel = new QStandardItemModel(mSharePopup); 
    mContentListView->setModel(mContentItemModel);
    mSharePopup->setContentWidget(mContentListView);
    mSharePopup->setFrameType(HbDialog::Strong);
    mSharePopup->setPrimaryAction(new HbAction(LOC_BUTTON_CANCEL));
    
    connect(mContentListView, SIGNAL(activated(QModelIndex)),this,SLOT(itemActivated(QModelIndex)));
    
    }

/**
 * fetchServiceDescriptors
 * 
 * Fetches the service descriptors using the Application Manager api. 
 * This function is in progress and will change.
 * 
 * @return error boolean true if successfully fetched.
 */

bool ShareUiPrivate::fetchServiceDescriptors(QList<XQAiwInterfaceDescriptor>& serviceDescriptorList)
    {
    bool retval = false;
    
    serviceDescriptorList = mAppManager.list(SERVICE_INTERFACE, SHARE_OP);
    
    if(serviceDescriptorList.size() > 0)
        {
        retval = true;
        }
    return retval;
    }


/**
 * fetchServiceAction fetches the action associated with a specified interface descriptor.
 * This is used after the fetchServiceDescriptors is called, and 
 * 
 * @param serviceDescriptor ServiceDescriptor fetched using fetchServiceDescriptors
 * @return error boolean true if successfully fetched.
 */

HbAction* ShareUiPrivate::fetchServiceAction(XQAiwInterfaceDescriptor interfaceDescriptor)
    {
    
    XQAiwRequest* request = mAppManager.create(interfaceDescriptor,SHARE_OP,mIsEmbedded);
    mAiwRequestList.append(request);
    HbAction* action = convertAction(request->createAction());
    if(action)
        {
        connect(request, SIGNAL(triggered()), this, SLOT(onTriggered()));
        connect(request, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleOk(const QVariant&)));
        connect(request, SIGNAL(requestError(int,const QString&)), this, SLOT(handleError(int,const QString&)));
        }
    return action;
    }


/**
 * Update the SendUi Dialog (and associated list items) once the required
 * information has been fetched.
 * 
 * @params action HbAction* The action that must be associated with the entry at that point in the list.
 */


bool ShareUiPrivate::updateShareUiDialogList(HbAction* action)
    {
    
    bool retval = false;
    QStandardItem* item = NULL;
    if(action != NULL)
        {
        item = new QStandardItem(action->icon().qicon(),action->text());
        mContentItemModel->appendRow(item);
        QModelIndex index = item->index();
        mIndexActionMap[index] = action;
        retval = true;
        }
    else
        {
        item = new QStandardItem(QString("No Services Found"));
        mContentItemModel->appendRow(item);
        }
    return retval;
    
    }


/**
 * 
 * Enable the UI and show it on the screen.
 * 
 * 
 */

void ShareUiPrivate::enableUi()
    {
    mSharePopup->exec();
    }

/**
 * Slot for handling send service selection on the UI
 * 
 */


void ShareUiPrivate::onTriggered(void)
    {
    
    XQAiwRequest* request = 0;
    request = qobject_cast<XQAiwRequest*>(sender());
    if(request)
        {
        request->setArguments(mFileList);
        }
    }

/**
 * Slot for handling valid returns from the framework.
 * 
 * @param result const QVariant&
 */

void ShareUiPrivate::handleOk(const QVariant& result)
    {
    Q_UNUSED(result)
    }

/**
 * Slot for handling errors. Error ids are provided as 
 * 32-bit integers. These are 
 * 
 * @param errorCode qint32
 * 
 * 
 */
void ShareUiPrivate::handleError(int errorCode, const QString& errorMessage)
    {
    Q_UNUSED(errorMessage)
    QString errText("IPC Error: ");
    HbNotificationDialog* dlg = new HbNotificationDialog();
    dlg->setFocusPolicy(Qt::NoFocus);
    dlg->setAttribute(Qt::WA_DeleteOnClose, true);

    
    switch(errorCode)
        {
        case XQService::EConnectionError:
            errText.append(QString("EConnectionError: "));
            break;
        case XQService::EConnectionClosed:
            errText.append(QString("EConnectionClosed: "));
            break;
        case XQService::EServerNotFound:
            errText.append(QString("EServerNotFound: "));
            break;
        case XQService::EIPCError:
            errText.append(QString("EIPCError: "));
            break;
        case XQService::EUnknownError:
            errText.append(QString("EUnknownError: "));
            break;
        case XQService::ERequestPending:
            errText.append(QString("ERequestPending: "));
            break;
        }
    errText.append(errorMessage);
    dlg->setText(errText);
    dlg->show();
    }



/** 
 * When a list box item is pressed, this slot should be invoked. The action
 * corresponding to the clicked list item should be invoked with the stored lis
 * of files.
 * 
 * @param index QModelIndex
 * 
 */

void ShareUiPrivate::itemActivated(QModelIndex index)
    {

    HbAction* action = mIndexActionMap[index];
    
    if(action)
        {
        action->setEnabled(true);
        action->activate(HbAction::Trigger);
        }    
    mSharePopup->close();

    }


/**
 * Convert a qaction to hbaction.
 * 
 * @param action QAction*
 * @return HbAction* 
 */

HbAction* ShareUiPrivate::convertAction(QAction *action)
    {
    HbAction *actionHb = 0;
    if (action) 
        {
        actionHb = new HbAction(HbIcon(action->icon()), action->text());
        actionHb->setVisible(action->isVisible());
        actionHb->setCheckable(action->isCheckable());
        actionHb->setEnabled(action->isEnabled());

        connect(actionHb, SIGNAL(triggered()), action, SIGNAL(triggered()));
        connect(actionHb, SIGNAL(changed()), action, SIGNAL(changed()));
        connect(actionHb, SIGNAL(hovered()), action, SIGNAL(hovered()));
        connect(actionHb, SIGNAL(toggled(bool)), action, SIGNAL(toggled(bool)));
        
        action->setParent(actionHb); 
        }
    
    return actionHb;
    }
