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
*       Class for packaging a descriptor array for IPC.
*
*/



#ifndef __CNCNIPCDESCRIPTORARRAY_H__
#define __CNCNIPCDESCRIPTORARRAY_H__

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

// CLASS DECLARATION

/**
 * Class for packaging a descriptor array for sending
 * it via IPC message.
 */
class CNcnPackDescrArray : public CBase
    {
    public:
    
       /**
        * Two-phased constructor.
        * @return newly instantiated object
        */
        static CNcnPackDescrArray* NewL();
        
       /**
        * Two-phased constructor.
        * @return newly instantiated object
        */
        static CNcnPackDescrArray* NewLC();
        
       /**
        * Packages given descriptor array to a buffer and
        * and returns the buffer.
        * @param aSourceArray descriptor array to pack
        */
        static TDesC8* PackL( const MDesCArray& aSourceArray );
                    
       /**
        * Destructor.
        */                    
        ~CNcnPackDescrArray();

    private:
    
        CNcnPackDescrArray();

    private:  // Data
    
    };

#endif // __CNCNIPCDESCRIPTORARRAY_H__

// End of File
