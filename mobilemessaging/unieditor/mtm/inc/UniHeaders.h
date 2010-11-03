/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: UniHeaders  declaration
*
*/


 
#ifndef __UNIHEADERS_H
#define __UNIHEADERS_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <mtclbase.h>       // TMsvPartList
#include <mmsgenutils.h>


// CONSTANTS
// Maximum size for unified message field string value.
const TInt KMaxHeaderStringLength = 1000;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class provides access to unified message specific header data.
*  The class can be used from both Client MTM and Server MTM.
*/
NONSHARABLE_CLASS( CUniHeaders ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUniHeaders* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CUniHeaders();

    public: // New functions
        
        /**
        * Reset.
        */
        void Reset();
        
        /**
        * Internalize the headers.
        * @param aStore CMsvStore
        */
        void RestoreL( CMsvStore& aStore );

        /**
        * Externalize the headers.
        * Caller must commit the store 
        * @param aStore CMsvStore in edit mode.
        */
        void StoreL( CMsvStore& aStore );

        /**
        * Subject mutator. Note that Client MTM should update explicitly 
        * TMsvEntry.iDescription too.
        * @param aSubject subject string,
        */
        void SetSubjectL( const TDesC& aSubject );

        /**
        * Subject accessor.
        * @return Subject string.
        */
        TPtrC Subject() const;

        /**
        * Returns recipients that are of specified type (To, Cc, Bcc).
        * @param aType specifies recipient type
        * @return array of recipient addresses.
        */
        const CDesCArray& TypedAddresseeList( TMsvRecipientType aType );

        /**
        * Adds recipient with specified type (To, Cc, Bcc).
        * This function also adds all recipients into the internal list 
        * that contains all addressees regardless of addressee type.
        * @param aRealAddress recipient address.
        * @param aType specifies recipient type
        */
        void AddTypedAddresseeL( const TDesC& aRealAddress, 
                                          TMsvRecipientType aType );

        /**
        * Remove a given address from the right typed list.
        * @param aRealAddress recipient address.
        * @return ETrue is address is found and deleted.
        */
        TBool RemoveAddressee( const TDesC& aRealAddress );

        /** 
        * Give const access to the "To" recipient list
        * @return recipient list
        */
        inline const CDesCArray& ToRecipients() const;

        /** 
        * Give const access to the "Cc" recipient list
        * @return recipient list
        */
        inline const CDesCArray& CcRecipients() const;

        /** 
        * Give const access to the "Bcc" recipient list
        * @return recipient list
        */
        inline const CDesCArray& BccRecipients() const;
        
        /**
        * Size of the uni headers object in memory. 
        * @return object size
        */
        TInt Size() const;

        /**
        * Accessor for message type setting
        * @return Message type setting
        */
        inline TInt32 MessageTypeSetting() const;

        /**
        * Mutator for message type setting
        * @param aSetting Message type setting
        */
        inline void SetMessageTypeSetting( TInt32 aSetting );

        /**
        * Accessor for message type locking
        * @return Message type locking
        */
        inline TInt32 MessageTypeLocking() const;

        /**
        * Mutator for message type locking
        * @param aLocking Message type locking
        */
        inline void SetMessageTypeLocking( TInt32 aLocking );
        
        /**
        * Accessor for message root
        * @return Message root
        */
        inline TMsvAttachmentId MessageRoot() const;

        /**
        * Mutator for message root
        * @param aRoot Message root
        */
        inline void SetMessageRoot( TMsvAttachmentId aRoot );

    public: // Functions from base classes

        
    protected:  // New functions
        

    protected:  // Functions from base classes
        

    private:

        /**
        * C++ default constructor.
        */
        CUniHeaders();
        
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CUniHeaders( const CUniHeaders& );
        // Prohibit assignment operator
        CUniHeaders& operator= ( const CUniHeaders& );

        /**
        * Internalize.
        * @param aStream read stream
        */
        void InternalizeL( RMsvReadStream& aStream );

        /**
        * Externalize.
        * @param aStream write stream
        */
        void ExternalizeL( RMsvWriteStream& aStream ) const;

        /**
        * Remove a addressee from a given addressee list.
        * @param aList addressee list
        * @param aAddress addressee to match
        * @return ETrue if matched and deleted.
        */
        TBool RemoveAddressee(
            CDesCArray& aList,
            const TDesC& aAddress );

        /**
        * Externalize Array. 
        * @param anArray the array to be externalized
        * @param aStream stream where to write.
        */
        void ExternalizeArrayL(
            CDesC16Array& anArray,
            RWriteStream& aStream ) const;

        /**
        * Internalize Array.
        * @param anArray the array where to write data
        * @param aStream a stream where to read data from
        */
        void InternalizeArrayL(
            CDesC16Array& anArray,
            RReadStream& aStream );

    public:     // Data
    
    protected:  // Data

    private:    // Data
        

        CDesCArray*     iToArray;       // TO headers
        CDesCArray*     iCcArray;       // Cc headers
        CDesCArray*     iBccArray;      // Bcc headers

        HBufC*          iSubject;       // Subject
        TInt32          iMessageTypeSetting;
        TInt32          iMessageTypeLocking;
        TMsvAttachmentId 	iMessageRoot;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes


    };

#include "UniHeaders.inl"

#endif      // __UNIHEADERS_H
            
// End of File
