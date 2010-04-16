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
 * Description:Implements Symbian functionality to get message pdu details.
 *
 */

#ifndef MSGCHARCOUNTER_P_H
#define MSGCHARCOUNTER_P_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <gsmuelem.h>

// FORWARD DECLARATIONS
class MsgCharCounter;
class CParaFormatLayer;
class CCharFormatLayer;
class CRichText;
class CSmsHeader;
class QString;

// CLASS DECLARATION

/**
 * MsgCharCounterPrivate
 *
 * This class is a Symbian private class which provides methods to set encoding
 * settings and get the PDU info.
 */
class MsgCharCounterPrivate
{
public:

    /**
     * Two-phased constructor.
     * @param msgCharCounter Ptr to MsgCharCounter.
     */
    static MsgCharCounterPrivate* newL(MsgCharCounter* msgCharCounter);

    /**
     * Destructor.
     */
    ~MsgCharCounterPrivate();

    /**
     * To Set encoding settings like encoding type, character support and
     * alternative encoding if any.
     * @param unicodeMode True if unicode.
     * @param alternativeEncodingType Alternative 7bit encoding combinations.
     * @param charSupportType Character support type, full or reduced.
     */
    void setEncodingSettings(bool unicodeMode, int alternativeEncodingType,
        int charSupportType);

    /**
     * Gets the PDU info like, number of PDUs, number of remaining chars in last
     * PDU and encoding types used.
     * @param buf PDU buffer as input.
     * @param numOfRemainingChars output
     * @param numOfPDUs output
     * @param unicodeMode output
     * @param alternativeEncodingType output
     */
    void getNumPDUs(const QString &buf, int &numOfRemainingChars,
        int &numOfPDUs, bool &unicodeMode, int &alternativeEncodingType);

private:

    /**
     * Constructor for performing 1st stage construction
     * @param .
     */
    MsgCharCounterPrivate(MsgCharCounter *msgCharCounter);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void constructL();

private:
    // Data

    /**
     * Ptr to QT class MsgCharCounter.
     * Not Own.
     */
    MsgCharCounter *q_ptr;

    /**
     * Paragraph format layer.
     * Own.
     */
    CParaFormatLayer* mParaFormatLayer;

    /**
     * Character format layer.
     * Own.
     */
    CCharFormatLayer* mCharFormatLayer;

    /**
     * Text with rich formatting.
     * Own.
     */
    CRichText* mRichText;

    /**
     * SMS MTM encapsulation of an SMS message.
     * Own.
     */
    CSmsHeader* mSmsHeader;

    /**
     * True if message is converted to unicode.
     */
    TBool mUnicodeMode;

    /**
     * Character support, full or reduced.
     */
    TInt mCharSupportType;

    /**
     * GSM encoding type.
     */
    TSmsEncoding mAlternativeEncodingType;

};

#endif // MSGCHARCOUNTER_P_H

//EOF
