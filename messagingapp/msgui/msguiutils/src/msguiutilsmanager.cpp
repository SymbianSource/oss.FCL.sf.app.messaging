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
 * Description: Services like send, image picker 
 * and contacts fetching functionality handler.
 *
 */

#include "msguiutilsmanager.h"
#include "msgsendutil.h"
#include "msgimagefetcherutil.h"
#include "msgaudiofetcherutil.h"
#include "msgcontactsutil.h"

// System includes
#include "debugtraces.h"


//---------------------------------------------------------------
// MsgUiUtilsManagerManager::MsgUiUtilsManager
// Constructor
//--------------------------------------------------------------
MsgUiUtilsManager::MsgUiUtilsManager(QObject* parent):
QObject(parent),
mSendUtil(NULL),
mImageUtil(NULL),
mAudioUtil(NULL),
mContactsUtil(NULL),
mInputBlocker(NULL)
    {
    }

//---------------------------------------------------------------
// MsgUiUtilsManager::~MsgUiUtilsManager
// Desctructor
//--------------------------------------------------------------
MsgUiUtilsManager::~MsgUiUtilsManager()
    {
    deactivateInputBlocker();
    }

//---------------------------------------------------------------
// MsgUiUtilsManager::send
// @see header file
//--------------------------------------------------------------
int MsgUiUtilsManager::send(ConvergedMessage& msg)
    {
    if(!mSendUtil)
        {
        mSendUtil = new MsgSendUtil(this);
        }
    return mSendUtil->send(msg);
    }

//---------------------------------------------------------------
// MsgUiUtilsManager::saveToDrafts
// @see header file
//--------------------------------------------------------------
TMsvId MsgUiUtilsManager::saveToDrafts(ConvergedMessage& msg)
    {
    if(!mSendUtil)
        {
        mSendUtil = new MsgSendUtil(this);
        }
    return mSendUtil->saveToDrafts(msg);
    }

//---------------------------------------------------------------
// MsgUiUtilsManager::send
// @see header file
//--------------------------------------------------------------
void MsgUiUtilsManager::fetchImages()
    {
    if(!mImageUtil)
        {
        mImageUtil = new MsgImageFetcherUtil(this);
        }
    mImageUtil->fetchImages();
    }


//---------------------------------------------------------------
// MsgUiUtilsManager::fetchContacts
// @see header file
//--------------------------------------------------------------
void MsgUiUtilsManager::fetchContacts()
    {
    if(!mContactsUtil)
        {
        mContactsUtil = new MsgContactsUtil(this);
        }
    mContactsUtil->fetchContacts();
    }

//---------------------------------------------------------------
// MsgUiUtilsManager::fetchVideo
// @see header file
//--------------------------------------------------------------
void MsgUiUtilsManager::fetchVideo()
    {
    
    }

//---------------------------------------------------------------
// MsgUiUtilsManager::fetchAudio
// @see header file
//--------------------------------------------------------------
void MsgUiUtilsManager::fetchAudio()
    {
    if(!mAudioUtil)
        {
        mAudioUtil = new MsgAudioFetcherUtil(this);
    }
    mAudioUtil->fetchAudio();
    }

//---------------------------------------------------------------
// MsgUiUtilsManager::fetchOther
// @see header file
//--------------------------------------------------------------
void MsgUiUtilsManager::fetchOther()
    {
    
    }

//---------------------------------------------------------------
// MsgUiUtilsManager::openContactDetail
// @see header file
//--------------------------------------------------------------
void MsgUiUtilsManager::openContactDetails(qint32 contactId)
    {
    if(!mContactsUtil)
        {
        mContactsUtil = new MsgContactsUtil(this);
        }
    mContactsUtil->openContactDetails(contactId);
    }

//---------------------------------------------------------------
// MsgUiUtilsManager::activateInputBlocker
// @see header file
//--------------------------------------------------------------
void MsgUiUtilsManager::activateInputBlocker(QGraphicsWidget* parent)
    {
    if(!mInputBlocker)
        {
        mInputBlocker = parent;
        mInputBlocker->grabMouse();
        mInputBlocker->grabKeyboard();
        }
    }

//---------------------------------------------------------------
// MsgUiUtilsManager::deactivateInputBlocker
// @see header file
//--------------------------------------------------------------
void MsgUiUtilsManager::deactivateInputBlocker()
    {
    if(mInputBlocker)
        {
        mInputBlocker->ungrabKeyboard();
		mInputBlocker->ungrabMouse();
		mInputBlocker = NULL;
        }
    }

// EOF
