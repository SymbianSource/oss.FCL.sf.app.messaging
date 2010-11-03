/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*    Recognizer for vCalendar and vCard.
*
*/




#ifndef _VREC_H_
#define _VREC_H_

// INCLUDES
#include <apmrec.h>

// CLASS DECLARATION
/**
 * Recogniser for vCards and vCalendars.
 * A confidence of ECertain is used because vrec.mdl has to override rversit.mdl.
 */
class CVRec : public CApaDataRecognizerType
    {
	public: // from CApaDataRecognizerType

		static CApaDataRecognizerType* CreateRecognizerL();

        CVRec();
        TUint PreferredBufSize();
        TDataType SupportedDataTypeL(TInt aIndex) const;
	private: // from CApaDataRecognizerType
        void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
    private:
        // For testing purposes only
        friend class T_CVRecTestAccessor;
    };

#endif //_VREC_H_

//end of file
