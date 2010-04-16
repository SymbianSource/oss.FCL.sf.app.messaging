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
 * Description: Container class for msg attachments
 *
 */
#ifndef MSGATTACHMENTCONTAINER_H
#define MSGATTACHMENTCONTAINER_H

#include <HbWidget>
#include "msgunieditorattachment.h"

class QGraphicsLinearLayout;
class MmsInsertCheckOperation;

class MsgAttachmentContainer : public HbWidget
    {
    Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgAttachmentContainer(const QString& pluginPath, QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgAttachmentContainer();

    /**
     * add attachment to the container
	 * @param filepath of the attachment
     */
    void addAttachment(const QString& filepath);

    /**
     * count of attachments in the container
	 * @return count
     */
    int count();
    
    /**
     * List of attachments in the container
     * @return attachments list
     */
    MsgUnifiedEditorAttachmentList attachmentList();

signals:
    /**
     * emit when container holds MM content
     */
    void mmContentAdded(bool isAdded);

    /**
     * emit to indicate view that container is now empty
	 * View deletes the container if it becomes empty
     */
    void emptyAttachmentContainer();
    
    /**
     * Emitted when msg's attachment container size changes
     */
    void sizeChanged(int aSize);

private slots:
    /**
     * delete attachment from the container
     */
    void deleteAttachment(MsgUnifiedEditorAttachment* attachment);
    
private:
    /**
     * Size of the attachment container
     * @return size
     */
    int containerSize();

    /**
     * size of the msg
     */
    int messageSize();
    
private:
    /**
     * style plugin's path
     */
    QString mPluginPath;

    /**
     * container's layout
     */
    QGraphicsLinearLayout* mLayout;
    
    /**
     * MMs insert check utility class
     */
    MmsInsertCheckOperation* mMmsInsertCheckOp;

    /**
     * attachment list
     */
    MsgUnifiedEditorAttachmentList mAttachmentList;

    /**
     * flag to tell, if container is holding MM content
     */
    bool mIsMMContent;
    };

#endif // MSGATTACHMENTCONTAINER_H
