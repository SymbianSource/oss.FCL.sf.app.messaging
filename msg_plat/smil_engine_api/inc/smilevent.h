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
* Description: smilevent  declaration
*
*/



#ifndef SMILEVENT_H
#define SMILEVENT_H

//  INCLUDES
#include <e32base.h>

#include <smiltime.h>
#include <smiltextbuf.h>

/**
* Information on one Event
*/
class TSmilEvent
    {
    public:  // Constructor

		enum TEventClass 
			{ 
			EOther, EActivate, EBegin, EEnd, ERepeat, EFocusIn, EFocusOut, EInBounds, EOutOfBounds
			};
        
		TSmilEvent( const TSmilTime& aTime, const TDesC& aClass );
		TSmilEvent( const TSmilTime& aTime, const TDesC& aClass, const CSmilObject* aSource );
		TSmilEvent( const TSmilTime& aTime, TEventClass aClass, const CSmilObject* aSource );

    public: // New functions

		TPtrC ClassName() const;
		TPtrC SourceName() const;
		void SetSourceNameL(const TDesC& aSource);
		TBool IsInteractive() const;

		static TEventClass ClassForName(const TDesC& aName);
		static TPtrC NameForClass(TEventClass aClass);
		static TBool IsInteractive(TEventClass aClass);

    private:    
		
		RSmilTextBuf					iSourceName;	
		
	public:		
		TEventClass						iClass;

		TSmilTime						iTime;
		const CSmilObject*				iSourceObject;
		
	};

#endif      // ?INCLUDE_H   
            
// End of File
