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
 * Description:Message send functionality handler.
 *
 */

#ifndef MSGUI_UTILS_MANAGER_H
#define MSGUI_UTILS_MANAGER_H

#include <QObject>

#ifdef BUILD_MSGUI_UTILS_DLL
#define MSGUI_UTILS_DLL_EXPORT Q_DECL_EXPORT
#else
#define MSGUI_UTILS_DLL_EXPORT Q_DECL_IMPORT
#endif

#include <msvstd.h>
#include <QGraphicsWidget>
#include "convergedmessageaddress.h"

class ConvergedMessage;
class MsgSendUtil;
class MsgImageFetcherUtil;
class MsgAudioFetcherUtil;
class MsgContactsUtil;

/**
 * This class manages the msgui services like send, image picker 
 * and contacts functionality.
 * 
 */
class MSGUI_UTILS_DLL_EXPORT MsgUiUtilsManager : public QObject
    {
    Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgUiUtilsManager(QObject* parent =0);

    /**
     * Destructor
     */
    ~MsgUiUtilsManager();
    
    
public slots:
    /**
     * Send message
     * @param msg Message to be sent 
     * @return KErrNone if send is successful, 
     *         KErrNotFound if settings are not available,
     *         KErrGeneral for other cases
     */
    int send(ConvergedMessage& msg);

    /**
     * Save message to drafts
     * @param msg Message to be sent 
     * @return true if message is saved to drafts
     */
    TMsvId saveToDrafts(ConvergedMessage& msg);

    /**
     * Fetch images 
     */
    void fetchImages();

    /**
     * Fectch conatcts
     */
    void fetchContacts();
    
    /**
     * Fectch videos
     */
    void fetchVideo();
    
    /**
     * Fectch audio
     */
    void fetchAudio();
    
    /**
     * Fectch others
     */
    void fetchOther();
    
    /*
     * Open Contact Details
     * @param contactId Conatct Id
     */
    void openContactDetails(qint32 contactId);

    /**
     * Activate Input Blocker
     * @param parent User should ensure that parent is of non-input
     * type e.g. view
     */
    void activateInputBlocker(QGraphicsWidget* parent);

    /**
     * Deactivate Input Blocker
     */
    void deactivateInputBlocker();

signals:

    /**
     * Signal emitted when images are fetched from gallary
     * @param images
     */

    void imagesFetched(const QVariant& images);

    /**
     * Signal emitted when audios are fetched from gallary
     * @param audio files
     */
    void audiosFetched(const QVariant& audios);

    /**
     * Signal emitted when contacts are fetched from phonebook.
     * @param contacts The list of contacts.
     */

    void contactsFetched(const QVariant& contacts);
    /**
     * Signal emitted when an error is generated.
     * @param error The error as a string to be displayed on the screen.
     */

    void serviceError(const QString& error);

private:

    /**
     * Send Utility
     * Own
     */
    MsgSendUtil* mSendUtil;
    
    /**
     * Image Fetcher
     * Own
     */
    MsgImageFetcherUtil* mImageUtil;

    /**
     * Image Fetcher
     * Own
     */
    MsgAudioFetcherUtil* mAudioUtil;

    /**
     * Conatcts Fetcher
     * Own
     */
    MsgContactsUtil* mContactsUtil;

    /**
     * Input Blocker
	 * Not owned
     */
    QGraphicsWidget* mInputBlocker;

    };

#endif // MSGUI_UTILS_MANAGER_H

// EOF
