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
* Description: smilprefetch  declaration
*
*/



#ifndef SMILPREFETCH_H
#define SMILPREFETCH_H

#include "smilobject.h"


class CSmilPrefetch : public CSmilObject
	{
public:
	virtual const TDesC* ClassName() const { _LIT(KN,"CSmilPrefetch"); return &KN; }

	static CSmilPrefetch* NewL(CSmilPresentation* aPresentation);
    
    virtual ~CSmilPrefetch();

	virtual TSmilTime ImplicitDuration() const;

	TBool IsPrefetch() const { return ETrue; }

protected:

	CSmilPrefetch();


	};


#endif
