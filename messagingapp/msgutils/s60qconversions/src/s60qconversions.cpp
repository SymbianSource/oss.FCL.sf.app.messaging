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

#include "s60qconversions.h"
#include "utf.h"

/*!
    \class S60QConversions
    \brief S60QConversions class offers functions for converting Symbian/Series60 data types to Qt data types and vice versa.
 */    


/*!
    Converts Symbian/Series60 descriptor (string) to QString

    \param desc descriptor to be converted
    \return QString containing converted string
 */
QString S60QConversions::s60DescToQString(const TDesC& desc)
    {
    return QString::fromUtf16(desc.Ptr(),desc.Length());
    }

/*!
    Converts QString to Symbian/Series60 descriptor (string).
    Note: Ownership of returned descriptor (string) is transferred to caller

    \param string QString to be converted
    \return pointer to Symbian/Series60 descriptor on success;
            otherwise returns NULL pointer
 */
HBufC* S60QConversions::qStringToS60Desc(const QString& string)
    {
    TPtrC16 str(reinterpret_cast<const TUint16*>(string.utf16()));
    return str.Alloc();
    }

/*!
    Converts Symbian/Series60 8 bit descriptor (UTF8 string) to QString

    \param desc 8 bit descriptor to be converted
    \return QString on success; otherwise returns null QString
 */
QString S60QConversions::s60Desc8ToQString(const TDesC8& desc)
    {
    QString qtString;
    HBufC* s60str = NULL;
    TRAPD(error, s60str = CnvUtfConverter::ConvertToUnicodeFromUtf8L(desc));
    if (error == KErrNone) {
    qtString = QString::fromUtf16(s60str->Ptr(),s60str->Length());
    delete s60str;
    }
    return qtString;
    }

/*!
    Converts QString to Symbian/Series60 8 bit descriptor (UTF8 string).
    Note: Ownership of returned descriptor (string) is transferred to caller

    \param string QString to be converted
    \return pointer to UTF8 string in Symbian/Series60 descriptor on success;
            otherwise returns NULL pointer
 */
HBufC8* S60QConversions::qStringToS60Desc8(const QString& string)
    {
    TPtrC16 str(reinterpret_cast<const TUint16*>(string.utf16()));
    HBufC8* s60str = NULL;
    TRAPD(error, s60str = CnvUtfConverter::ConvertFromUnicodeToUtf8L(str));
    if (error != KErrNone) {
    return NULL;
    }
    return s60str;
    }

// End of file


