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
* Description: smilanchor  declaration
*
*/



#ifndef SMILANCHOR_H
#define SMILANCHOR_H

//  INCLUDES
#include <e32base.h>

#include <smiltextbuf.h>

// CLASS DECLARATION
class CSmilAnchor : public CObject
    {
    public:  // Constructors and destructor

	enum TPlayState
		{
		EPlay,
		EPause,
		EStop
		};

	enum TShow
		{
		ENew,
		EDeprecatedPause,
		EReplace
		};

	enum TActuate
		{
		EOnLoad,
		EOnRequest
		};


	enum {
		KTabUnspecified = -314159
		};

		/**
        * C++ default constructor.
        */
        CSmilAnchor();

		/**
        * Destructor
        */
		virtual ~CSmilAnchor();

    public: // New functions
		/**
        * Returns the anchor's href.
        * @return TPtrC - the value for the href
        */
		IMPORT_C TPtrC Href() const;

		/**
        * Returns the anchor's target.
        * @return TPtrC - the value for the target
        */
		TPtrC Target() const;

		/**
        * Returns the anchor's alt value.
        * @return TPtrC - the value for the alt
        */
		TPtrC Alt() const;
	
		/**
        * Sets the anchor's href.
        * @param aStringValue - the value the href have to be set
        */
		void SetHrefL( const TDesC& aStringValue );
		
		/**
        * Sets the anchor's alt.
        * @param aStringValue - the value the alt have to be set
        */
		void SetAltL( const TDesC& aStringValue );
		
		/**
        * Sets the anchor's target.
        * @param aStringValue - the value the target have to be set
        */
		void SetTargetL( const TDesC& aStringValue );
		

    public:
        RSmilTextBuf						iHref;
		RSmilTextBuf						iAlt;
		RSmilTextBuf						iTarget;
		TInt								iSourceLevel;
		TInt								iDestinationLevel;
		TInt								iTabIndex;
		TText								iAccessKey;
		TBool								iExternal;
		TPlayState							iSourcePlaystate;
		TPlayState							iDestinationPlaystate;
		TShow								iShow;
		TActuate							iActuate;
	};

#endif      // ?INCLUDE_H   
            
// End of File
