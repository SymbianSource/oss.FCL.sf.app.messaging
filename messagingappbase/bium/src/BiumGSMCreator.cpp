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
* Description:   Factory methods for creating MtmUi & MtMUiData
*
*/



// Messaging inludes
#include <MTMStore.h>

#include "biou.h"
#include "bioi.h"

EXPORT_C CBaseMtmUiData* NewBioMtmUiDataL( CRegisteredMtmDll& aRegisteredDll )
	{
	return CBioMtmUiData::NewL( aRegisteredDll );
	}

EXPORT_C CBaseMtmUi* NewBioMtmUiL( CBaseMtm& aMtm, CRegisteredMtmDll& aRegisteredDll )
    {
    return CBioMtmUi::NewL( aMtm, aRegisteredDll );
    }


