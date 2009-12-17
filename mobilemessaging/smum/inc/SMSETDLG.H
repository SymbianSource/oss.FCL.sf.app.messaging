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
*     Help classes to handle Service Centres and settings.
*
*/



#ifndef __SMSETDLG_H__
#define __SMSETDLG_H__

//    uikon includes
#if !defined(__EIKDIALG_H__)
#include <eikdialg.h>                    // CEikDialog
#endif // __EIKDIALG_H__

#if !defined(__E32DEF_H__)
#include <e32def.h>
#endif // __E32DEF_H__

// INCLUDES
#include <muiusettingsarray.h>          // CMuiuSettingsArray 
#include "SMSU.H"                       // CSmsMtmUi

// FORWARD DECLARATIONS
class CEikColumnListBox;
class CSmsUiSettings;

//  CLASS DEFINITIONS

/**
*  TSmumSCNameInfo
*/
NONSHARABLE_CLASS( TSmumSCNameInfo )
    {
    public: // Constructors and destructor

        // Constructors
        TSmumSCNameInfo( const TDesC& aName, const TDesC& aAddress );
        TSmumSCNameInfo( const TDesC& aName );
        
        /**
        * Destructor.
        */
        ~TSmumSCNameInfo();
  
    public: // New functions

        /**
        * Returns SC name
        * @return descriptor pointer SC name
        */
        TPtrC Name() const;

        /**
        * Returns SC number
        * @return descriptor pointer SC number
        */
        TPtrC Address() const;

        /**
        * Returns SC name and number
        * @return descriptor pointer SC name and name
        */
        inline TPtrC NameAndAddress() const {return iNameAndAddress;};

    private:

        // Prohibit copy constructor
        TSmumSCNameInfo( const TSmumSCNameInfo& );
        // Prohibit assigment operator
        TSmumSCNameInfo& operator= ( const TSmumSCNameInfo& );
        
    private: // Data 
        TBuf<2*KHumanReadableNameLength> iNameAndAddress;
        TInt                             iNameLength;
    };

/**
*  CSmumSCArray
*  Inherited from MDesCArray, CArrayFixFlat
*/
NONSHARABLE_CLASS( CSmumSCArray ) : public CArrayFixFlat<TSmumSCNameInfo>, public MDesCArray
    {
    public: // Constructors and destructor

        // Constuctor
        static CSmumSCArray* NewL( TInt aGranularity );
        
        /**
        * Destructor.
        */
        ~CSmumSCArray();

    public: // New functions

        /**
        * Returns asked reference from array
        * @param aIndex referenced TSmumSCNameInfo in array
        * @return reference to TSmumSCNameInfo
        */        
        TSmumSCNameInfo& SCNameInfo( TInt aIndex );
        
    private: // Functions from base classes

        /**
        * From MDesCArray
        */
        TInt MdcaCount() const;
        
        /**
        * From MDesCArray
        */
        TPtrC MdcaPoint( TInt aIndex ) const;
        
    private:

        /**
        * By default Symbian OS constructor is private.
        */  
        void ConstructL();

        /**
        * By default Symbian OS constructor is private.
        */  
        CSmumSCArray( TInt aGranularity ); 


    private: // Data
        HBufC*                          iStringForSCName; 
    };

/**
*  CSmumSettingsArray
*  Inherited from CMuiuSettingsArray
*/
NONSHARABLE_CLASS( CSmumSettingsArray ) : public CMuiuSettingsArray
    {
    public: // Constructors and destructor
        
        // Constructor
        static CSmumSettingsArray* NewL( TInt aResourceId );

        /**
        * Destructor.
        */
        ~CSmumSettingsArray();
        
    public: // New functions

        /**
        * Sets replypath-variable
        * @param aReplyPathProvided boolean
        */
        void SetReplyPath( TBool aReplyPathProvided );
        
        /**
        * Returns replypath-variable
        * @return boolean
        */
        TBool ReplyPathProvided();

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
        TBool               iReplyPathProvided;
        HBufC*              iReplyPathString;
    };

#endif // __SMSETDLG_H__

//  End of File
