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
 * Description:Implements QT functionality to get message pdu details.
 *
 */

#include "msgcharcounter.h"
#include "msgcharcounter_p.h"


	enum MsgCharSupport
	{
		msgCharSupportFull = 0,
		msgCharSupportReduced
	};
	
// ---------------------------------------------------------------------------
// Default Constructor.
// ---------------------------------------------------------------------------
//
MsgCharCounter::MsgCharCounter( QObject *parent ):
	QObject( parent )
	{
	d_ptr = MsgCharCounterPrivate::newL( this );
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
MsgCharCounter::~MsgCharCounter()
{
	if (d_ptr) {
		delete d_ptr;
		d_ptr = NULL;
	}
}

// ---------------------------------------------------------------------------
// Sets the encoding settings.
// ---------------------------------------------------------------------------
//
void MsgCharCounter::setEncodingSettings()
{
	// Default values.
	bool unicodeMode = false;
	int alternativeEncodingType = 0;
	int charSupportType = msgCharSupportFull;
	
	d_ptr->setEncodingSettings(unicodeMode, alternativeEncodingType,
			charSupportType);
}

// ---------------------------------------------------------------------------
// Gets the PDU details.
// ---------------------------------------------------------------------------
//
void MsgCharCounter::getNumPDUs(const QString &buf,
		int &numOfRemainingChars,
		int &numOfPDUs,
		bool &unicodeMode,
		int &alternativeEncodingType)
{
	d_ptr->getNumPDUs(buf, numOfRemainingChars, numOfPDUs, unicodeMode,
			alternativeEncodingType);
}

// EOF
