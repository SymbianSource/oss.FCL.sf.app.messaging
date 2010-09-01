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
* Description: smiltransitionfactoryinterface  declaration
*
*/



#ifndef SMILTRANSITIONFACTORY_H
#define SMILTRANSITIONFACTORY_H


#include <e32base.h>

#include <smiltransition.h>
#include <smiltransitionfilter.h>

class MSmilMedia;

/**
 * Interface to a factory that constructs transition filters.
 */
class MSmilTransitionFactory
	{
	public:		
		/**
		 * Construct a filter best implementing the transition description
		 * where CSmilTransition struct consists of fields corresponding to the attributes of SMIL <transition> element (id, type, subtype, dur, fadeColor, reverse, startProgress, endProgress)
		 */
		virtual CSmilTransitionFilter* CreateFilterL(CSmilTransition* aTransition, MSmilMedia* aMedia) = 0;	

		/**
		 * True if the named transition type is supported by this factory.
		 */
		virtual TBool IsSupportedType(const TDesC& aTransitionName) const = 0;

		/**
		 * True if the named subtype is supported for given transition type by this factory.
		 */
		virtual TBool IsSupportedSubtype(const TDesC& aTransitionName, const TDesC& aSubtypeName) const = 0; 

	};


#endif
