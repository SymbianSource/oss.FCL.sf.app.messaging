/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*       Implementation for CDMA Smum creator class.
*
*/



// Includes
#include "SmsuCDMA.h"
#include "SmsiCDMA.h"

// ---------------------------------------------------------
// NewSmsMtmUiL
// ---------------------------------------------------------
EXPORT_C CBaseMtmUi* NewSmsMtmUiL( CBaseMtm& aMtm, CRegisteredMtmDll& aRegisteredDll )
    {
    return CSmsMtmUiCDMA::NewL( aMtm, aRegisteredDll );
    }

// ---------------------------------------------------------
// NewSmsMtmUiDataL
// ---------------------------------------------------------
EXPORT_C CBaseMtmUiData* NewSmsMtmUiDataL( CRegisteredMtmDll& aRegisteredDll )
	{
	return CSmsMtmUiDataCDMA::NewL( aRegisteredDll );
	}

