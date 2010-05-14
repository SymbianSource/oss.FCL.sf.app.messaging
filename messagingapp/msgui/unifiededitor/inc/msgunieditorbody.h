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

#include <msgunifiededitorbasewidget.h>
#include <f32file.h>
#include "msgunieditorprocessimageoperation.h"

class HbTextEdit;
class HbTextItem;
class HbFrameItem;
class HbIconItem;
class HbPushButton;
//class HbGestureSceneFilter;
class CMsgMediaResolver;
class CMsgImageInfo;
class MmsConformanceCheck;
class UniEditorPluginInterface;
class UniEditorPluginLoader;


class MsgUnifiedEditorBody : public MsgUnifiedEditorBaseWidget,
                             public MUniEditorProcessImageOperationObserver
{
    Q_OBJECT

    Q_PROPERTY(bool hasImage READ hasImage WRITE setImage)
    Q_PROPERTY(bool hasAudio READ hasAudio WRITE setAudio)
public:

    /**
     * Constructor
     */
    MsgUnifiedEditorBody(QGraphicsItem *parent = 0);

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
    
    /**
     * get size of body content for MMS size calculations
     * @return size of body content
     */
    int bodySize();
    
    /*
     * Returns value of mUnicode 
     * @return mUnicode   
     */
     bool isUnicode();

     /*
      * Disables char counter
      */
     void disableCharCounter();
     
     /**
      * To set focus on editable field.
      */
     void setFocus();

     /**
      * Get to find body already contains an image
      * @return bool
      */
     bool hasImage();

     /**
      * Get to find body already contains an audio
      * @return bool
      */
     bool hasAudio();

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
     * Emitted when msg-body content changes
     */
    void contentChanged();

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

    /**
     * Service launch complete.
     */
    void handleOk(const QVariant& result);

    /**
     * Service launch errors.
     */
    void handleError(int errorCode, const QString& errorMessage);

private:

    /**
     * Get to find body already contains an image
     * @return bool
     */
    void setImage(bool image = false);

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
    //HbGestureSceneFilter* mGestureFilter;
	
    /**
     * MMs conformance check utility class
     */
    MmsConformanceCheck* mMmsConformanceCheck;
	
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
    
    /**
     * Instance of HbTextItem
     * Will be deleted automatically by parent.
     * Own.
     */
    HbTextItem *mCharCounter;

    /**
     * Instance of HbFrameItem
     * Will be deleted automatically by parent.
     * Own.
     */
    HbFrameItem* mBackgroundItem;
    
    /**
     * Holds the previous buffer inside msgeditor
     */
    QString mPrevBuffer;

    /**
     * Holds char type supported
     */
    int mCharSupportType;

    /**
     * Instance of UniEditorPluginInterface
     * Will be deleted automatically by parent.
     * Own.
     */
    UniEditorPluginInterface* mPluginInterface;

    /**
     * Instance of UniEditorPluginLoader
     * Will be deleted when UniEditorPluginInterface object is deleted
     * Not Own.
     */
    UniEditorPluginLoader* mPluginLoader;

    /*
     * Maintains information if any unicode character has been entered or not
     */
    bool mUnicode;
    
    /**
     * Content widget for processing animation.
     */
    HbWidget* mProcessingWidget;
};

#endif //UNIFIED_EDITOR_BODY_H
