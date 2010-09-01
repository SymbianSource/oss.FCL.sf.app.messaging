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
*     Helper class definitions for messaging.
*
*/



#ifndef __MSGARRAYS_H__
#define __MSGARRAYS_H__

//  INCLUDES
#include <e32def.h>
#include <msvreg.h>
#include <mtudcbas.h>


// CLASS DECLARATION

/**
* TUidNameInfo
* Class for mtm info i.e. uid and human readable name
*/
class TUidNameInfo
    {
    public:
        /**
        * Constructor
        */
        IMPORT_C TUidNameInfo( TUid aUid, const TDesC& aName );

    public:
        TUid                            iUid;
        TBuf<KHumanReadableNameLength>  iName;
    };

/**
* TUidNameKey
*/
class TUidNameKey : public TKeyArrayFix
    {
    public:
        /**
        * Constructor
        */
        IMPORT_C TUidNameKey( TKeyCmpText aType );

    private:
        /**
        * From TKey
        */
        TInt Compare( TInt aLeft, TInt aRight ) const;
    };


/**
* CUidNameArray
* Array class for mtms
* Can be used for example with list boxes because it is inherited from class MDesCArray.
*/
class CUidNameArray : public CArrayFixFlat<TUidNameInfo>, public MDesCArray
    {
    public:
        /**
        * Constructor
        */
        IMPORT_C CUidNameArray( TInt aGranularity );

        /**
        * Destructor
        */
        IMPORT_C ~CUidNameArray();

        /**
        * Sort function
        */
        IMPORT_C virtual void Sort( TKeyCmpText aTextComparisonType );

    private:
        /**
        * From MDesCArray
        */
        IMPORT_C virtual TInt MdcaCount() const;

        /**
        * From MDesCArray
        */
        IMPORT_C virtual TPtrC MdcaPoint( TInt aIndex ) const;
    };

#endif // __MSGARRAYS_H__

// End of file
