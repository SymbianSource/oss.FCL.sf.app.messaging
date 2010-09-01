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
* Description: smilcondition  declaration
*
*/



#ifndef SMILCONDITION_H
#define SMILCONDITION_H

#include "smiltextbuf.h"

#include "smiltime.h"
#include "smilevent.h"

class TSmilCondition : public CBase
    {
    public:  // Constructors and destructor
        
		/**
        * C++ default constructor.
        */
        TSmilCondition();

	public:
		/**
        * Destructor.
        */
        virtual ~TSmilCondition();

    public: // New functions
        
		void SetEventClassL( const TDesC& aEvent );
		void SetEventSourceL( const TDesC& aSource );

		TPtrC EventClassName() const; 
		TPtrC EventSourceName() const; 

		TBool IsEventCondition() const;

		TBool Matches( const TSmilEvent& aEvent ) const;	


	private:  	

		TSmilCondition(const TSmilCondition& ) : CBase() {}

		RSmilTextBuf		iEventClassName;
		RSmilTextBuf		iEventSourceName;
	public:
		TSmilEvent::TEventClass		iEventClass;
		TSmilTime			iOffset;
		CSmilObject*		iEventSourceObject;
	};

#endif      // SMILCONDITION_H   
            
// End of File
