/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       MMmsMtmDeliveryDataContainer - Virtual interface class forcing the 
*     subclass to implement a certain method/methods    
*
*/




#ifndef MDATACONTAINER_H
#define MDATACONTAINER_H

#include <e32base.h>
#include "CMsgDeliveryItem.h"
// CLASS DECLARATION

/** 
*
*/
class MMmsMtmDeliveryDataContainer 
    {
    public: 

        /**
        * 
        *
        */
    inline virtual void SetDataArrayL(CArrayPtrFlat< CMsgDeliveryItem >* aDataArray) = 0 ;  

    };

#endif      
            
// End of File
