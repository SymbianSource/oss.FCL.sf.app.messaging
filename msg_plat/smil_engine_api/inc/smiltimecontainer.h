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
* Description: smiltimecontainer  declaration
*
*/



#ifndef SMILTIMECONTAINER_H
#define SMILTIMECONTAINER_H

//  INCLUDES
#include <e32std.h>
#include "smilobject.h"

// CLASS DECLARATION

class CSmilTimeContainer : public CSmilObject
    {
    public:  // Constructors and destructor

		virtual const TDesC* ClassName() const { _LIT(KN,"CSmilTimeContainer"); return &KN; }
        
        //Cannot be instantiated => no NewL()
        
        /**
        * Destructor.
        */
        virtual ~CSmilTimeContainer();

    public: // New functions

		void ResolveL() { ResolveL(0); }
		virtual void ResolveL( const TSmilTime& aTps );

		void NotifyDependents( const TSmilTime& aTime );

		virtual TSmilTime NextActivation( const TSmilTime& aTime ) const;

		void EndL( const TSmilTime& aTime );
		void BeginL( const TSmilTime& aTime );		
		void Reset();
		
		void ResetChildrenL( const TSmilTime& aTime );


		TBool IsTimeContainer() const { return ETrue; }


		virtual void AddChild( CSmilObject* aObject );

		CSmilObject* FirstChild() const { return iFirstChild; }
		CSmilObject* LastChild() const { return iLastChild; }

		CSmilObject* FindNamedObject(const TDesC& aName);


		virtual void PrintTree(int d=0) const;

		virtual void SetPaused(TBool aPaused);

    protected:  
        	
		
        CSmilTimeContainer();

		virtual void Unfreeze();

		void RepeatL( const TSmilTime& aTime );

		TSmilTime TimeContainerEnd() const;

		void SetEndsyncL(TEndsync aEndsync, const TDesC& aId = TPtrC());		 		

	protected:
		CSmilObject*					iFirstChild;
		CSmilObject*					iLastChild;
		RSmilTextBuf					iEndsyncId;
		TEndsync						iEndsync;

    };

#endif      // ?INCLUDE_H   
            
// End of File
