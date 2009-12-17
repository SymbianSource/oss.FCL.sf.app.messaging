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
*       Helper class for recipient/phonebook handling
*
*/



#ifndef __POSTCARDRECIPIENTWRAPPER_H
#define __POSTCARDRECIPIENTWRAPPER_H

// INCLUDES

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CPostcardAppUi;
class CPostcardContact;
class CPostcardDocument;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CPostcardRecipientWrapper
*
* @lib postcard.exe
* @since 3.0
*/
class CPostcardRecipientWrapper : public CBase
    {
    public:  // New methods

    /**
    * Destructor.
    */
    ~CPostcardRecipientWrapper();

    /**
    * C++ constructor
    * @param 
    */
    CPostcardRecipientWrapper(
        CPostcardDocument& aDocument,
        CPostcardAppUi& aAppUi,
        CPostcardContact& aContact
        );

    /**
    * Factory function to create this object
    * @param aApp   
    */
    static CPostcardRecipientWrapper* NewL( 
        CPostcardDocument& aDocument,
        CPostcardAppUi& aAppUi,
        CPostcardContact& aContact
        );

    /**
    * The only real public function.
    * The function is given a contact item id as a parameter. 
    * If the contact item has only one location of addresses filled
    * (default, home, work), that is returned in aLocation.
    * If more than 1 are filled, user is shown a query dialog where user
    * can choose which address location (s)he wants to use. This location is 
    * returned in aLocation.
    * @param aId IN the id of the contact item in question
    * @param aLocation OUT the location user chose
    * @return TBool aLocation is valid only if function returns ETRue
    *               if user chose cancel in the dialog -> EFalse is returned
    */
    TBool AskLocationL( CPostcardContact::TLocation& aLocation );

    private: // Own methods
    
    /**
    * Formats the address strings suitable for CAknSingleHeadingPopupMenuStyleListBox.
    */
    HBufC* GetFormattedStringL( CPostcardContact::TLocation aAddressLocation );
    
    /**
    * 2nd phase Constructor.
    */
    void ConstructL( );
        
    private: // data

        CPostcardDocument& iDocument;
        CPostcardAppUi& iAppUi;
        CPostcardContact& iContact;
    };

#endif // __POSTCARDRECIPIENTWRAPPER_H
