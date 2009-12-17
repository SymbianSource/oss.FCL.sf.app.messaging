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
* Description: smilsequence  declaration
*
*/



#ifndef SMILSEQUENCE_H
#define SMILSEQUENCE_H

//  INCLUDES
#include <e32base.h>
#include "smilmedia.h"
#include "smilparallel.h"
#include "smilobject.h"

// CLASS DECLARATION

class CSmilSequence : public CSmilTimeContainer
    {
    public:  // Constructors and destructor

		virtual const TDesC* ClassName() const { _LIT(KN,"CSmilSequence"); return &KN; }       

        /**
        * Two-phased constructor.
        */
        static CSmilSequence* NewL(CSmilPresentation* aPresentation);
        
        /**
        * Destructor.
        */
        virtual ~CSmilSequence();

    public: // New functions

		void PropagateEventL( const TSmilEvent& aEvent, const TSmilTime& aTime );
		
		TSmilTime ImplicitDuration() const;

		void ProceedL( const TSmilTime& aTime );

		void RewindL(const TSmilTime& aTime, const TSmilTime& aTo);

    public: // Functions from base classes

		TBool IsSequence() const { return ETrue; }        

    protected:  // Functions from base classes
        
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(CSmilPresentation* aPresentation);
		
		/**
        * By default C++ constructor is private.
        */
        CSmilSequence();

    };

#endif      // ?INCLUDE_H   
            
// End of File
