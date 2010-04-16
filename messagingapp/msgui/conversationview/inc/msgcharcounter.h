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

#ifndef MSGCHARCOUNTER_H
#define MSGCHARCOUNTER_H

// INCLUDES
#include <QObject>

// FORWARD DECLARATIONS
class MsgCharCounterPrivate;

// CLASS DECLARATION

/**
 * MsgCharCounter
 * 
 * This class is a QT public class which provides methods to set encoding
 * settings and get the PDU info.
 */
class MsgCharCounter : public QObject
{
	Q_OBJECT

public:

	/**
	 * Default Constructor.
	 */
	MsgCharCounter(QObject *parent = 0);

	/**
	 * Destructor.
	 */
	~MsgCharCounter();
	
	/**
	 * To Set encoding settings like encoding type, character support and
	 * alternative encoding if any.
	 */
	void setEncodingSettings();
	
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
				int &numOfPDUs, bool &unicodeMode,
				int &alternativeEncodingType);

private:

	/**
	 * Pointer to S60 class MsgFetcherPrivate.
	 * Own.
	 */
	MsgCharCounterPrivate *d_ptr;
};

#endif /* MSGCHARCOUNTER_H */

// EOF
