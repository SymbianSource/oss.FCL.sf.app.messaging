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
* Description: smila implementation
*
*/



// INCLUDE FILES
#include "smillength.h"
#include "smila.h"
#include "smilmedia.h"
#include "smilrootregion.h"
#include "smilpresentation.h"
#include "smillinkedlist.h"



// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//

CSmilA::CSmilA()
: CSmilObject()
	{	
	}


// EPOC default constructor can leave.
void CSmilA::ConstructL(CSmilPresentation* aPresentation)
	{
	CSmilObject::ConstructL(aPresentation);
	}

// Two-phased constructor.
CSmilA* CSmilA::NewL(CSmilPresentation* aPresentation)
	{
	CSmilA* ptr = new ( ELeave ) CSmilA;
	CleanupStack::PushL( ptr );
	ptr->ConstructL(aPresentation);
	CleanupStack::Pop(); //ptr
	return ptr;
	}

CSmilA::~CSmilA()
	{
	}



TSmilTime CSmilA::ImplicitDuration() const
	{
	return 0;
	}


// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  



