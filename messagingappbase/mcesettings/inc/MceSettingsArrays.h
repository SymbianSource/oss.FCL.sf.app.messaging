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
*     Helper class for dialogs. Overrides MdcaPoint to return correct string for
*     listboxes.
*
*/



#ifndef MCESETTINGSARRAYS_H
#define MCESETTINGSARRAYS_H

//  INCLUDES

#include <MsgArrays.h> // CUidNameArray

// FORWARD DECLARATIONS


// CLASS DECLARATION


/**
*  Helper class for dialogs. Overrides MdcaPoint to return correct string for
*  listboxes.
*/
class CMceUidNameArray :
    public CUidNameArray
    {
    public:

        enum TMceUidNameArrayType
            {
            EMtmList,
            EServicesList
            };

    public:

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMceUidNameArray* NewL( TMceUidNameArrayType aType = EMtmList );
        
        /**
         * Destructor
         */
        IMPORT_C ~CMceUidNameArray();

    private:

        /**
         * Default constructor
         */
        CMceUidNameArray( TMceUidNameArrayType aType );

        /**
         * Symbian OS constructor
         */
        void ConstructL();

        /**
         * From MDesCArray
         * @param aIndex id of the folder to be opened
         * Returns string for listbox for displaying.
         */
        virtual TPtrC MdcaPoint( TInt aIndex ) const;

    private:
        TMceUidNameArrayType iListType;
        HBufC*  iText;

    };


#endif // MCESETTINGSARRAYS_H

// End of file
