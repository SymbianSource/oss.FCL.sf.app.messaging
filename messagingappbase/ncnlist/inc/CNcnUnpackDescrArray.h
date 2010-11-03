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
*       Class for unpacking a descriptor array received via IPC.
*
*/



#ifndef __CNCNUNPACKDESCRARRAY_H__
#define __CNCNUNPACKDESCRARRAY_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

// CLASS DECLARATION

/**
 * Class for unpacking a descriptor array received via IPC.
 * This class is a counterpart for CNcnPackDescrArray.
 */
class CNcnUnpackDescrArray : public CBase, public MDesCArray
    {
    public:
    
       /**
        * Two-phased constructor.
        * @return newly instantiated object
        */
        static CNcnUnpackDescrArray* NewLC();
        
       /**
        * Unpacks a descriptor array from IPC message.
        * @param aMessage IPC message
        * @param aParam index of the param holding the buffer
        */     
        void UnpackL( const RMessagePtr2& aMessage, TInt aParam );
        
       /**
        * Destructor.
        */                            
        ~CNcnUnpackDescrArray();

    public:  // from MDesCArray
    
        TInt MdcaCount() const;
        
        TPtrC16 MdcaPoint( TInt aIndex ) const;

    private:
    
        CNcnUnpackDescrArray();

    private:  // Data

        // Descriptor array    
        CDesCArrayFlat iArray;
    };

#endif // __CNCNUNPACKDESCRARRAY_H__

// End of File

