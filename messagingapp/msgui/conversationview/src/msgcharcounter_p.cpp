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
#include "msgcharcounter_p.h"

#include <smuthdr.h>
#include <gsmuset.h>
#include <gsmumsg.h>
#include <txtetext.h>
#include <txtrich.h>

#include "msgcharcounter.h"
#include "s60qconversions.h"

enum MsgCharSupport
{
    msgCharSupportFull = 0, msgCharSupportReduced
};

// ---------------------------------------------------------------------------
// Default Constructor ( 1st phase )
// ---------------------------------------------------------------------------
//
MsgCharCounterPrivate::MsgCharCounterPrivate(MsgCharCounter *msgCharCounter) :
    q_ptr(msgCharCounter)
{
    // No implementation required
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
MsgCharCounterPrivate::~MsgCharCounterPrivate()
{
    q_ptr = NULL;

    if (mParaFormatLayer)
    {
        delete mParaFormatLayer;
        mParaFormatLayer = NULL;
    }

    if (mCharFormatLayer)
    {
        delete mCharFormatLayer;
        mCharFormatLayer = NULL;
    }

    if (mRichText)
    {
        delete mRichText;
        mRichText = NULL;
    }

    if (mSmsHeader)
    {
        delete mSmsHeader;
        mSmsHeader = NULL;
    }
}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
MsgCharCounterPrivate* MsgCharCounterPrivate::newL(
    MsgCharCounter *msgCharCounter)
{
    MsgCharCounterPrivate* self = new (ELeave) MsgCharCounterPrivate(
        msgCharCounter);
    CleanupStack::PushL(self);
    self->constructL();
    CleanupStack::Pop(); // self;
    return self;
}

// ---------------------------------------------------------------------------
// EPOC default constructor for performing 2nd stage construction
// ---------------------------------------------------------------------------
//
void MsgCharCounterPrivate::constructL()
{
    mParaFormatLayer = CParaFormatLayer::NewL();
    mCharFormatLayer = CCharFormatLayer::NewL();
    mRichText = CRichText::NewL(mParaFormatLayer, mCharFormatLayer);

    mSmsHeader = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *mRichText);
}

// ---------------------------------------------------------------------------
// To Set encoding settings like encoding type, character support and
// alternative encoding if any.
// ---------------------------------------------------------------------------
//
void MsgCharCounterPrivate::setEncodingSettings(bool unicodeMode,
    int alternativeEncodingType, int charSupportType)
{
    TSmsUserDataSettings smsSettings;
    CSmsMessage& smsMsg = mSmsHeader->Message();

    mUnicodeMode = unicodeMode;
    mCharSupportType = charSupportType;
    mAlternativeEncodingType = (TSmsEncoding) alternativeEncodingType;

    if (mUnicodeMode)
    {
        smsSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabetUCS2);
    }
    else
    {
        smsSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
    }
    smsSettings.SetTextCompressed(EFalse);
    smsMsg.SetUserDataSettingsL(smsSettings);
    //First try without any alternate encoding
    smsMsg.SetAlternative7bitEncoding(ESmsEncodingNone);
}

// ---------------------------------------------------------------------------
// To get PDU Info: extracts details of number of PDUs, number of remaining
// chars in last PDU and encoding types used.
// ---------------------------------------------------------------------------
//
void MsgCharCounterPrivate::getNumPDUs(const QString &buf,
    int &numOfRemainingChars, int &numOfPDUs, bool &unicodeMode,
    int &alternativeEncodingType)
{
    TInt numOfUnconvChars;
    TInt numOfDowngradedChars;
    TInt isAltEncSupported;
    TSmsEncoding currentAlternativeEncodingType;

    CSmsMessage& smsMsg = mSmsHeader->Message();

    // need to set the input buffer to SMS buffer through iRichText
    // (which is reference to SMS Buffer object)
    HBufC* buffer = S60QConversions::qStringToS60Desc(buf);
    CleanupStack::PushL(buffer);
    mRichText->Reset();
    mRichText->InsertL(0, *buffer);

    //call SMS stack API to get PDU info
    smsMsg.GetEncodingInfoL(numOfPDUs, numOfUnconvChars, numOfDowngradedChars,
        numOfRemainingChars);

    //Algo to switch to Unicode if required
    while ( (numOfUnconvChars || numOfDowngradedChars) && !mUnicodeMode)
    {
        currentAlternativeEncodingType = smsMsg.Alternative7bitEncoding();
        if (currentAlternativeEncodingType != mAlternativeEncodingType)
        {
            //try with this new alternative encoding type
            isAltEncSupported = smsMsg.SetAlternative7bitEncoding(
                mAlternativeEncodingType);
            if (isAltEncSupported == KErrNotSupported)
            {
                // if required alternative encoding plugin is not supported,
                // retain the existing encoding mechanism.
                mAlternativeEncodingType = currentAlternativeEncodingType;
                continue;
            }
        }
        else if (numOfUnconvChars || (MsgCharSupport) mCharSupportType
                == msgCharSupportFull)
        {
            //switch to Unicode
            //mUnicodeMode = ETrue;
            setEncodingSettings(ETrue, ESmsEncodingNone, mCharSupportType);
        }
        else
        {
            //Get out of while loop and return the results
            break;
        }
        //get the PDU info with new settings
        mRichText->Reset();
        mRichText->InsertL(0, *buffer);
        smsMsg.GetEncodingInfoL(numOfPDUs, numOfUnconvChars,
            numOfDowngradedChars, numOfRemainingChars);
    } // end of while

    CleanupStack::PopAndDestroy(buffer);
    unicodeMode = mUnicodeMode;
    alternativeEncodingType = mAlternativeEncodingType;
    if (mUnicodeMode)
    {
        // In case of Unicode mode, SMS Stack returns number of available free
        // User Data units. Need to convert them w.r.t characters.
        // Each char takse 2 UD units.
        numOfRemainingChars = numOfRemainingChars / 2;
    }
}

//EOF
