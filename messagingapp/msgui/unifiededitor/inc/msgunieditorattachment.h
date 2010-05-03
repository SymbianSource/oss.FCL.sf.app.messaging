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
 * Description:attachment field to show attachment details.
 *
 */

#ifndef UNIFIED_EDITOR_ATTACHMENT_H
#define UNIFIED_EDITOR_ATTACHMENT_H

#include <HbWidget>
#include <QList>

class HbTextItem;
class HbIconItem;
class HbGestureSceneFilter;

class MsgUnifiedEditorAttachment : public HbWidget
    {
    Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgUnifiedEditorAttachment( const QString& pluginPath,
                                const QString& attachmentpath,
                                const int filesize,
                                QGraphicsItem *parent = 0 );

    /**
     * Destructor
     */
    ~MsgUnifiedEditorAttachment();

    /**
     * Seeker method to get the path of the attachment
     * @return path of the attachment
     */
    const QString& path();

    /**
     * Seeker method to get the size of the attachment
     * @return size in bytes
     */
    qreal size();

    /**
     * Seeker method to get mimetype of the attachment
     * @return mimetype
     */
    const QString& mimeType();

    /**
     * Check if the attachment content is MM content
     * @return true/false
     */
    bool isMultimediaContent();
    
    HbFeedback::InstantEffect overrideFeedback(Hb::InstantInteraction interaction) const;

protected:
    /**
     * reimplemented from base class.
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
    
private:
    /**
     * Helper method to initialize gesture.
     */
    void initGesture();


signals:
    /**
	 * emit when short-tap happens on the attachment
	 */
    void clicked();

	/**
	 * emit when 'remove' option is selected from longpress menu
	 */
    void deleteMe(MsgUnifiedEditorAttachment* attachment);

private slots:
    /**
	 * show longpress menu for attachment object
	 */
    void longPressed(QPointF position);

	/**
	 * slot to remove attachment from msg editor
	 */
    void removeAttachment();

	/**
	 * slot to open attachment
	 */
    void openAttachment();

	/**
	 * slot to view details of the attachment file
	 */
    void viewDetails();

private:
    /**
	 * style plugin path
	 */
    QString mPluginPath;

    /**
	 * attachment file's path
	 */
    QString mPath;

    /**
	 * attachment's size in bytes
	 */
    qreal mSize;

    /**
	 * attachment file's mimetype
	 */
    QString mMimeType;

    /**
	 * layout icon to indicate attachment
	 */
    HbIconItem* mAttachmentIcon;

    /**
	 * layout item to hold attachment's name
	 */
    HbTextItem* mAttachmentName;

    /**
	 * layout item to hold attachment's details e.g. size
	 */
    HbTextItem* mAttachmentDetails;

    /**
	 * gesture filter for long press.
	 */    
    HbGestureSceneFilter* mGestureFilter;
	
	/**
     * Max limit on sms size. Store at class level for optimization
     */
    int mMaxSmsSize;

    };

typedef QList<MsgUnifiedEditorAttachment*> MsgUnifiedEditorAttachmentList;

#endif //UNIFIED_EDITOR_ATTACHMENT_H
