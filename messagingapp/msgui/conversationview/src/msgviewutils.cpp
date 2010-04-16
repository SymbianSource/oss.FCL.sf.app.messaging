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
 * Description:
 *
 */

#include "msgviewutils.h"
#include "s60qconversions.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
MsgViewUtils::MsgViewUtils()
{
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
MsgViewUtils::~MsgViewUtils()
{
}

// ---------------------------------------------------------------------------
// To get the mime type of the file
//
// ---------------------------------------------------------------------------
//
QString MsgViewUtils::mimeTypeL(QString aFileName)
{
    RFs rfs;
    RFile file;

    HBufC* fileName = S60QConversions::qStringToS60Desc(aFileName);

    //TODO: The RFs and RApaLsSession has to be connected 1's, not for every mms

    if(rfs.Connect() != KErrNone)
        return QString();

    if(rfs.ShareProtected() != KErrNone )
        return QString();

    if(file.Open(rfs, *fileName, EFileShareReadersOrWriters | EFileRead) != KErrNone )
        return QString();

    TDataRecognitionResult dataType;
    RApaLsSession apaSession;

    if(apaSession.Connect() == KErrNone)
        {
        if (apaSession.RecognizeData(file, dataType) == KErrNone)
            {
            TPtrC8 fileType = dataType.iDataType.Des8();

            rfs.Close();
            apaSession.Close();

            return S60QConversions::s60Desc8ToQString(fileType);
            }
        }

    rfs.Close();
    return QString();
}

//EOF
