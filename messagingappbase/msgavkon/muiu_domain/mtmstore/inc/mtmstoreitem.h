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
* Description: mtmstoreitem  declaration
*
*/



#if !defined(__MTMSTOREITEM_H__)
#define __MTMSTOREITEM_H__

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class CBaseMtm;
class CBaseMtmUi;
class CMtmUsageTimer;


// CLASS DECLARATION

/**
* CMtmUiInfoItem
*/
class CMtmUiInfoItem: public CBase
    {
    public:
        CMtmUiInfoItem( CBaseMtm* aMtm, CBaseMtmUi* aMtmUi, CMtmUsageTimer* aTimer );
        ~CMtmUiInfoItem();
        
        CBaseMtm&   Mtm() const;
        CBaseMtmUi& MtmUi() const;
        
        void IncreaseUsage();
        void DecreaseUsage();
    private:
        CBaseMtm*		iMtm;
        CBaseMtmUi*		iMtmUi;
        CMtmUsageTimer*		iTimer;
        TInt			iUsageCount;
    };

#endif // __MTMSTOREITEM_H__

// End of File
