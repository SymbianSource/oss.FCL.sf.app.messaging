/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: smilinstancetime  declaration
*
*/



#ifndef SMILINSTANCETIME_H
#define SMILINSTANCETIME_H

//  INCLUDES
#include <e32base.h>
#include "smiltime.h"
//#include "smilobject.h"
// CONSTANTS
const TInt KOffsetValue = 1;
const TInt KEventValue = 2;
const TInt KSyncBaseValue = 3;

// FORWARD DECLARATIONS
class CSmilObject;
class TSmilTime;

// CLASS DECLARATION

class CSmilInstanceTime : public CBase
    {
    public:  // Constructors and destructor

		/**
        * C++ default constructor.
        */
        CSmilInstanceTime( TInt aType, TSmilTime aTime );

		/**
        * C++ default constructor.
        */
        CSmilInstanceTime( TInt aType, TSmilTime aTime,
						   CSmilObject* aSyncBase,TBool aSyncToBegin );

    public: // New functions

		TBool IsOffsetValue() const; 
		
		TBool IsEventValue() const; 

		CSmilInstanceTime* Next() const; 
		
		CSmilObject* Syncbase() const;

		TBool IsSyncBaseValue() const;
		
		TBool IsBeginSync() const;
		
		TBool IsEndSync() const;
		
		TSmilTime Time() const;
		
		void SetTime( TSmilTime aTime );

		void SetSyncbase( CSmilObject* aObject );

		void SetNext( CSmilInstanceTime* aTime );

		void SetSyncBegin( TBool aBool );

		TSmilTime Offset() const;
	
    private:    // Data
		TInt						iType;
        TSmilTime					iTime;
		TSmilTime					iOffset;
		CSmilInstanceTime*			iNext;
		CSmilObject*				iSyncbase;
		TBool						iSyncBegin;
	};

#endif      // ?INCLUDE_H   
            
// End of File
