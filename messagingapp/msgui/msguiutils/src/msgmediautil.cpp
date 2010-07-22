/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:This class provides duration of media file by parsing it
 *
 */
#include <MsgMediaResolver.h>
#include <xqconversions.h>

#include "msgmediautil.h"

//---------------------------------------------------------------
// MsgMediaUtil::MsgMediaUtil
// Constructor
//---------------------------------------------------------------
MsgMediaUtil::MsgMediaUtil()
    {
    
    }

//---------------------------------------------------------------
// MsgMediaUtil::MsgMediaUtil
// Desctructor
//---------------------------------------------------------------
MsgMediaUtil::~MsgMediaUtil()
    {
    
    }

//---------------------------------------------------------------
// MsgMediaUtil::mediaDuration
// @see header file
//---------------------------------------------------------------
QString MsgMediaUtil::mediaDuration(const QString& mediaFile)
    {
    QString formattedDuration(" ");
    
    HBufC *name = XQConversions::qStringToS60Desc(mediaFile);
    TInt duration = 0;
    TRAPD(error, duration = mediaDurationL(*name));
    delete name;
    
    if ( error )
        return formattedDuration;
        
    int sec_duration = duration / 1000;
    int hour_component = sec_duration / (60*60);
    int min_component = (sec_duration - (hour_component*60*60))/60;
    int sec_component = (sec_duration - (hour_component*60*60) - (min_component*60));
    
    if( hour_component > 0)
        {
        formattedDuration = QString("%1:%2:%3").arg(hour_component, 2, 10, QChar('0')).
            arg(min_component, 2, 10, QChar('0')).arg(sec_component, 2, 10, QChar('0'));
        }
    else
        {
        formattedDuration = QString("%1:%2").arg(min_component, 2, 10, QChar('0')).
            arg(sec_component, 2, 10, QChar('0'));
        }

    return formattedDuration;
    }

//---------------------------------------------------------------
// MsgMediaUtil::mediaDurationL
// @see header file
//---------------------------------------------------------------
TInt MsgMediaUtil::mediaDurationL(const TDesC& mediaFile)
    {
    CMsgMediaResolver* resolver = CMsgMediaResolver::NewLC();
    resolver->SetCharacterSetRecognition(EFalse);
    
    RFile rFile = resolver->FileHandleL(mediaFile);
    CleanupClosePushL( rFile );

    CMsgMediaInfo* info = resolver->CreateMediaInfoL( rFile );
    resolver->ParseInfoDetailsL(info, rFile);
    CleanupStack::PushL( info );

    TInt duration = info->Duration();
    
    CleanupStack::PopAndDestroy(3, resolver); // info, rFile, resolver
    
    return duration;
    }


// EOF

