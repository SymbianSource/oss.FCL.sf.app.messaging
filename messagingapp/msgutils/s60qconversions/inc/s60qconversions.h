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
#ifndef S60QCONVERSIONS
#define S60QCONVERSIONS

#include <QObject>
#include <e32base.h>
#include "s60qconversiondefines.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class S60QCONV_EXPORT S60QConversions
    {
    public:
        static QString s60DescToQString(const TDesC& desc);
        static HBufC* qStringToS60Desc(const QString& string);
        static QString s60Desc8ToQString(const TDesC8& desc);
        static HBufC8* qStringToS60Desc8(const QString& string);
    };

QT_END_NAMESPACE

QT_END_HEADER

#endif // S60QCONVERSIONS

// End of file

