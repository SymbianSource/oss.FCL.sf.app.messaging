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
* Description: smilparallel  declaration
*
*/



#ifndef SMILPARALLEL_H
#define SMILPARALLEL_H

//  INCLUDES
#include <e32base.h>
#include "smiltimecontainer.h"

// CLASS DECLARATION

class CSmilParallel : public CSmilTimeContainer
    {
    public:  // Constructors and destructor

		virtual const TDesC* ClassName() const { _LIT(KN,"CSmilParallel"); return &KN; }

        /**
        * Two-phased constructor.
        */
        static CSmilParallel* NewL(CSmilPresentation* aPresentation);
        
        /**
        * Destructor.
        */
        virtual ~CSmilParallel();

    public: // New functions
		
		void PropagateEventL( const TSmilEvent& aEvent, const TSmilTime& aTime );

		void ProceedL( const TSmilTime& aTime );

		void RewindL(const TSmilTime& aTime, const TSmilTime& aTo);
		
		TSmilTime ImplicitDuration() const;


		virtual TBool IsParallel() const { return ETrue; }

		
    protected:
		
		/**
        * By default C++ constructor is private.
        */
        CSmilParallel();
		
		/**
        * By default EPOC constructor is private.
        */
		void ConstructL(CSmilPresentation* aPresentation);

    };

#endif      // ?INCLUDE_H   
            
// End of File
