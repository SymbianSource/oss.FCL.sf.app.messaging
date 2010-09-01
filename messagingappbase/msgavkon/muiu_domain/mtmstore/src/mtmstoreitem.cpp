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
* Description: mtmstoreitem implementation
*
*/




// INCLUDE FILES
#include <mtclbase.h>
#include <mtmuibas.h>
#include "mtmstoreitem.h"
#include "muiudomainpan.h"
#include "mtmusagetimer.h"


// CONSTANTS
const TInt KUsageCount = 1;


// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------
// CMtmUiInfoItem::CMtmUiInfoItem
// ---------------------------------------------------------
//
CMtmUiInfoItem::CMtmUiInfoItem( CBaseMtm* aMtm, CBaseMtmUi* aMtmUi, CMtmUsageTimer* aTimer )
:CBase(), 
iMtm( aMtm ), 
iMtmUi( aMtmUi ), 
iTimer( aTimer ), 
iUsageCount( KUsageCount )
    {
    __ASSERT_DEBUG( aMtm, Panic( EMuiuMtmStoreNullMtmPtr ) );
    __ASSERT_DEBUG( aMtmUi, Panic( EMuiuMtmStoreNullMtmUiPtr ) );
    __ASSERT_DEBUG( aTimer, Panic( EMuiuMtmStoreNullTimerPtr ) );
    }

// ---------------------------------------------------------
// CMtmUiInfoItem::~CMtmUiInfoItem
// ---------------------------------------------------------
//
CMtmUiInfoItem::~CMtmUiInfoItem()
    {
    delete iTimer;
    delete iMtmUi;
    delete iMtm;
    }

// ---------------------------------------------------------
// CMtmUiInfoItem::Mtm
// ---------------------------------------------------------
//
CBaseMtm& CMtmUiInfoItem::Mtm() const
    {
    __ASSERT_DEBUG( iMtm, Panic( EMuiuMtmStoreNullMtmPtr ) );
    return *iMtm;
    }

// ---------------------------------------------------------
// CMtmUiInfoItem::MtmUi
// ---------------------------------------------------------
//
CBaseMtmUi& CMtmUiInfoItem::MtmUi() const
    {
    __ASSERT_DEBUG( iMtmUi, Panic( EMuiuMtmStoreNullMtmUiPtr ) );
    return *iMtmUi;
    }

// ---------------------------------------------------------
// CMtmUiInfoItem::IncreaseUsage
// ---------------------------------------------------------
//
void CMtmUiInfoItem::IncreaseUsage()
    {
    __ASSERT_DEBUG( iUsageCount >= 0, Panic( EMuiuMtmStoreNegativeCounterValue ) );
    iUsageCount++;
    iTimer->Cancel();
    }


// ---------------------------------------------------------
// CMtmUiInfoItem::DecreaseUsage
// ---------------------------------------------------------
//
void CMtmUiInfoItem::DecreaseUsage()
    {
    iUsageCount--;
    __ASSERT_DEBUG( iUsageCount >= 0, Panic( EMuiuMtmStoreNegativeCounterValue ) );
    if ( iUsageCount == 0 )
        {
        iTimer->Start();
        }
    }

// End of File
