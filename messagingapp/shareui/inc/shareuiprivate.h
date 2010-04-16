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



#ifndef SHAREUIPRIVATE_H_
#define SHAREUIPRIVATE_H_

#include <qobject.h>
#include <qfile.h>
#include <qstandarditemmodel.h>
#include <QModelIndex>
#include <QStringList>

#include <hbaction.h>
#include <hbmenu.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hblistview.h>
#include <hblistviewitem.h>

#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <xqaiwinterfacedescriptor.h>


#define SERVICE_INTERFACE "imessage.send"
#define SHARE_OP "send(QVariant)"

/**
 * Private implementaion of shareui.
 * implements showing sending services dialog and sending files
 */
class ShareUiPrivate : public QObject
    {
    Q_OBJECT
    
public:
    
    /**
     * Constructor
     */
    ShareUiPrivate();
    
    /**
     * Destructor
     */
    virtual ~ShareUiPrivate();
    
    /**
     * method for passing file list to the sending services
     * @param fileList list of paths to files.eg: c:\images\sunset.jpg
     * @param embedded true if sendui dialog is embedded in the launched application
     * 
     * @return bool true if dialog initialization successfull otherwise false
     */
    bool init(QStringList& fileList, bool embedded);
    

    
public slots:

    /**
     * Handles setting th orguments for triggered service
     */
    void onTriggered(void); 
    
    /**
     * Slot for handling valid returns from the framework.
     * 
     * @param result const QVariant&
     */
    void handleOk(const QVariant& result);
    
    /**
     * Slot for handling errors. Error ids are provided as 
     * 32-bit integers.
     * @param errorCode qint32
     */
    void handleError(int errorCode, const QString& errorMessage);

private slots:

    /** 
     * When a list box item is pressed, this slot should be invoked. The action
     * corresponding to the clicked list item should be invoked with the stored lis
     * of files.
     * 
     * @param index QModelIndex
     * 
     */
    void itemActivated(QModelIndex index);
    
private:

    /**
     * fetchServiceDescriptors
     * 
     * Fetches the service descriptors using the Application Manager api. 
     * 
     * @return error boolean true if successfully fetched.
     */
    bool fetchServiceDescriptors(QList<XQAiwInterfaceDescriptor>& descriptorList);
    
    /**
     * fetchServiceAction fetches the action associated with a specified interface descriptor.
     * This is used after the fetchServiceDescriptors is called, and 
     * 
     * @param serviceDescriptor ServiceDescriptor fetched using fetchServiceDescriptors
     * @return error boolean true if successfully fetched.
     */
    HbAction* fetchServiceAction(XQAiwInterfaceDescriptor interfaceDescriptor);    
    
    /**
      * Creates the view for the sendui dialog.
      *  
      * @return bool true if initialization was successful, false otherwise.
      */
    void initializeUi(void);
    
    /**
     * Update the SendUi Dialog (and associated list items) once the required
     * information has been fetched.
     * 
     * @params action HbAction* The action that must be associated with the entry at that point in the list.
     */
    bool updateShareUiDialogList(HbAction* action);
    
    /**
     * Enable the UI and show it on the screen.
     */
    void enableUi(void);
    
    /**
     * Convert a qaction to hbaction.
     * 
     * @param action QAction*
     * @return HbAction* 
     */
    HbAction* convertAction(QAction *action);
    
private:
    /**
     * 
     * List of files to be sent.
     * 
     */
    QList<QVariant> mFileList;
    
    /**
     * Service List
     */
    QList<XQAiwRequest*> mAiwRequestList;
    /**
     * ShareUi dialog
     */
    HbDialog* mSharePopup;
    
    /**
     * 
     * item model for content list view.
     */
    
    QStandardItemModel* mContentItemModel;
    /**
     * content view
     */
    
    HbListView* mContentListView;
    
    /**
     * mapping qmodelindex to the action
     * 
     */
    
    QMap<QModelIndex, HbAction*> mIndexActionMap;
    
    /**
     * Application Manager
     */
    XQApplicationManager mAppManager;
    
    /**
     * Whether to launch the sending application as embedded or not.
     */
    bool mIsEmbedded;
    
    };

#endif /* SENDUIDIALOG_P_H_ */
