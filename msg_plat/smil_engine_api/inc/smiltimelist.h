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
* Description: smiltimelist  declaration
*
*/



#ifndef SMILTIMELIST_H
#define SMILTIMELIST_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSmilObject;
class CSmilInstanceTime;
class TSmilTime;

// CLASS DECLARATION

class TSmilTimeList 
    {
    public:  // Constructors and destructor

		/**
        * Destructor.
        */
		void RecalcSyncbase();
		
		/**
        * Constructor.
        */
		TSmilTimeList();
		
		/**
        * Destructor.
        */
		virtual TSmilTimeList::~TSmilTimeList();		

		void SetObject(CSmilObject* aObject);

		/**
        * Add an instance time to the list.
        */
		void Add( CSmilInstanceTime* aTime );
		
		/**
        * Remove an instance time from the list.
        */
		void Remove( CSmilInstanceTime* aRem, CSmilInstanceTime* aPrev );
		
		/**
        * Reset this time list.
        */
		void Reset();
		
		/**
        * Get next greater instance.
        */
		TSmilTime NextInstance( TSmilTime& aAfter, TBool aEq );
		
		/**
        * Check if the list is empty or not.
        */
		TBool IsEmpty();

		TBool HasFiniteOffsets();

    private:    // Data
		
		CSmilObject*					iObject;
		CSmilInstanceTime*				iFirst;
		CSmilInstanceTime*				iLast;

		TBool							iSyncbaseCalculated;
    };

#endif      // ?INCLUDE_H   
            
// End of File
