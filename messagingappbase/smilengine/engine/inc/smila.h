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
* Description: smila  declaration
*
*/



#ifndef SMILA_H
#define SMILA_H

//  INCLUDES
#include <e32base.h>

#include "smilanchor.h"
#include "smiltimecontainer.h"

#include <smillength.h>
#include <smiltime.h>

#include <smilfocusinterface.h>
// FORWARD DECLARATIONS
class CGraphicsContext;

// CLASS DECLARATION


class CSmilA : public CSmilObject
    {
    public:  // Constructors and destructor

		virtual const TDesC* ClassName() const { _LIT(KN,"CSmilA"); return &KN; }

		/**
        * Destructor
        */
		virtual ~CSmilA();

		static CSmilA* NewL(CSmilPresentation* aPresentation);

    public: // New functions

		virtual TBool IsA() const { return ETrue; }


	protected:

		/**
        * C++ default constructor.
        */
        CSmilA();	
		void ConstructL(CSmilPresentation* aPresentation);

		
		TSmilTime ImplicitDuration() const;
		
	public:	
		
	};

#endif      // ?INCLUDE_H   
            
// End of File
