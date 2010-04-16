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
 * Description:message body. to enter text and show media inserted.
 *
 */

#ifndef UNIFIED_EDITOR_BODY_H
#define UNIFIED_EDITOR_BODY_H

#include <hbwidget.h>
#include <f32file.h>
#include "msgunieditorprocessimageoperation.h"

class HbTextEdit;
class HbFrameItem;
class HbIconItem;
class HbPushButton;
class HbGestureSceneFilter;
class CMsgMediaResolver;
class CMsgImageInfo;
class MmsInsertCheckOperation;


class MsgUnifiedEditorBody : public HbWidget,public MUniEditorProcessImageOperationObserver
{
    Q_OBJECT

    Q_PROPERTY(bool hasImage READ hasImage WRITE setImage)
    Q_PROPERTY(bool hasAudio READ hasAudio WRITE setAudio)
public:

    /**
     * Constructor
     */
    MsgUnifiedEditorBody(const QString& pluginPath, QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgUnifiedEditorBody();

    /**
     * Seeker method to return back data to editor's view
     */
    QString text();

    /**
     * Seeker method to return back data to editor's view
     */
    const QStringList mediaContent();

public slots:
    /**
     * Called to insert image content in editor.
     * @param medialist list of absolute paths of media.
     */
    void setImage(QString& imagefile);

    /**
     * Called to insert audio content in editor.
     * @param medialist list of absolute paths of media.
     */
    void setAudio(QString& audiofile);

    /**
     * Called to insert video content in editor.
     * @param medialist list of absolute paths of media.
     */
    void setVideo(QString& videofile);

    /**
     * Called to insert body text in editor.
     * @param text body text.
     */
    void setText(QString& text);

signals:
    /**
     * Emitted when send button from virtual ITUT is pressed.
     */
    void sendMessage();

    /**
     * Emitted when MMS content is added or removed
     */
    void mmContentAdded(bool isAdded);

    /**
     * Emitted when msg body size changes
     */
    void sizeChanged(int aSize);

public: // from MUniEditorProcessImageOperationObserver
    
    /*
     * @see MUniEditorProcessImageOperationObserver
     */
    void EditorOperationEvent( TUniEditorProcessImageOperationEvent aEvent,
        TFileName aFileName );

protected: // from HbWidget
    
    /**
     * Reimplemented from base class to give proper size.
     * @see HbWidget
     */
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;

private slots:
    /**
     * called on long press on the media objects
     */
    void longPressed(QPointF position);

    /**
     * called from the media object's item specific menu
     */
    void removeMedia();

    /**
     * called from the media object's item specific menu
     */
    void openMedia();

    /**
     * called from the media object's item specific menu
     */
    void viewDetails();
    
    /**
     * handle text changes in body field
     */
    void onTextChanged();

private:
    /**
     * Get to find body already contains an image
     * @return bool
     */
    bool hasImage();

    /**
     * Get to find body already contains an image
     * @return bool
     */
    void setImage(bool image = false);

    /**
     * Get to find body already contains an audio
     * @return bool
     */
    bool hasAudio();

    /**
     * Set that body now contains an audio
     */
	void setAudio(bool audio = false);
    
    /**
     * Get the region (image/audio/video) where longpress happened
     * @return region
     */
    QString getHitRegion(QPointF position);

    /**
     * get size of body content for MMS size calculations
     * @return size of body content
     */
    int bodySize();

    /**
     * size of the msg
     */
    int messageSize();    
    
    /**
     * Handler for set image function 
     */
    void handleSetImage();    

    /**
     * Starts the resize animation 
     */
    void startResizeAnimation();

    /**
     * Stops the resize animation 
     */
    void stopResizeAnimation();

private:
    /**
     * Info if message body has image.
     * true if message body has image else false.
     */
    bool mHasImage;

    /**
     * Info if message body has audio.
     * true if message body has audio else false.
     */
    bool mHasAudio;

    /**
     * editor to in put text.
     */
    HbTextEdit* mTextEdit;

    /**
     * frame for editor.
     */
    HbFrameItem* mEditorFrame;

    /**
     * icon item to preview images.
     */
    HbIconItem* mIconItem;

    /**
     * inline audio item
     */
    HbPushButton* mAudioItem;

    /**
     * string to hold plug in path.
     */
    QString mPluginPath;

	/**
	 * Image file contained inside body
	 */
    QString mImageFile;

    /**
     * Saves the original file which has to be used if resize fails
     */
    QString mSavedImageFile;

	/**
	 * Audio file contained inside body
	 */
    QString mAudioFile;

	/**
	 * Video file contained inside body
	 */
    QString mVideoFile;

    /**
     * To setup longpress gesture on media objects
     */
    HbGestureSceneFilter* mGestureFilter;
	
    /**
     * MMs insert check utility class
     */
    MmsInsertCheckOperation* mMmsInsertCheckOp;
	
    /**
     * Size of image in body 
     */
    int mImageSize;
    
    /**
     * Size of audio in body
     */
    int mAudioSize;
    
    /**
     * Size of video in body
     */
    int mVideoSize;

    /**
     * Rfs object
     */
    RFs mfs;

    /**
     * CUniEditorProcessImageOperation object
     */
    CUniEditorProcessImageOperation *mProcessImageOperation;

    /**
     * CMsgMediaResolver object
     */
    CMsgMediaResolver* mMediaResolver;
    
    /**
     * CMsgImageInfo object
     */
    CMsgImageInfo *mImageInfo;    
};

#endif //UNIFIED_EDITOR_BODY_H
