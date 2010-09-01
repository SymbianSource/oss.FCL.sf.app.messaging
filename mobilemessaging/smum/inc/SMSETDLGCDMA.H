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
*     Help classes to handle CDMA settings settings.
*
*/



#ifndef __SMSETDLGCDMA_H__
#define __SMSETDLGCDMA_H__

//    uikon includes
#if !defined(__EIKDIALG_H__)
#include <eikdialg.h>                    // CEikDialog
#endif // __EIKDIALG_H__

#if !defined(__E32DEF_H__)
#include <e32def.h>
#endif // __E32DEF_H__

// INCLUDES
#include <MuiuSettingsArray.h>          // CMuiuSettingsArray 
#include "smsu.h"                       // CSmsMtmUi

// FORWARD DECLARATIONS
class CEikColumnListBox;
class CSmsUiSettings;

//  CLASS DEFINITIONS

/**
*  CSmumSettingsArray
*  Inherited from CMuiuSettingsArray
*/
NONSHARABLE_CLASS(CSmumSettingsArray) : public CMuiuSettingsArray
    {
    public: // Constructors and destructor
        
        // Constructor
        static CSmumSettingsArray* NewL( TInt aResourceId );

        /**
        * Destructor.
        */
        ~CSmumSettingsArray();
        
    public: // New functions

        void SetCallBackNumberL( const TDesC& aCallBackNumber);

        TPtrC CallBackNumber() const;

    private: // functions from base classes
        
             /**
             * From MDesCArray
        */
        TPtrC MdcaPoint( TInt aIndex ) const;
        
    private:
        
        /**
        * By default Symbian OS constructor is private.
        */  
        CSmumSettingsArray();

        /**
        * By default Symbian OS constructor is private.
        */  
        void ConstructL( TInt aResourceId );
                
    private: // Data
        HBufC*              iCallBackNumberString;
        TInt                iTypeOfSettingsId;
    };

#endif // __SMSETDLGCDMA_H__

//  End of File
