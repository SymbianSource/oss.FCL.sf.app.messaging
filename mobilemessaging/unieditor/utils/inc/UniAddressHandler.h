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
* Description:   Unified Message Editor Address Handler.
*
*/



#ifndef C_UNIADDRESSHANDLER_H
#define C_UNIADDRESSHANDLER_H

// ========== INCLUDE FILES ================================

#include <e32base.h>
#include <MsgCheckNames.h>
#include <cmsvrecipientlist.h>
#include <ConeResLoader.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CCoeEnv;
class CBaseMtm;
class CMsgAddressControl;
class CMsgEditorAppUi;
class MObjectProvider;

// ========== DATA TYPES ===================================

// ========== CLASS DECLARATION ============================

/**
*  Unified Editor Address Handler class declaration.
*  Class provides services for address handling. 
*
*  @lib UniUtils.lib
*
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CUniAddressHandler ) : public CBase
    {
    public:

        /**
        * Allocates and constructs a CUniAddressHandler object.
        *
        * @param aMtm       Client MTM
        * @param aControl   Address control to which this handler is connected
        * @param aCoeEnv    Handle to control environment
        *
        * @return Pointer to the new CUniAddressHandler object
        */
        IMPORT_C static CUniAddressHandler* NewL( CBaseMtm& aMtm,
                                                  CMsgAddressControl& aControl,
                                                  CCoeEnv& aCoeEnv );
        /**
        * Destructor
        */
        virtual ~CUniAddressHandler();

        /**
        * Clears given type addresses from MTM.
        *
        * @since S60 3.2
        *
        * @param aRecipientType Determines the address type to be cleared
        */        
        IMPORT_C void RemoveAddressesFromMtmL( TMsvRecipientTypeValues aRecipientType );

        /**
        * Clears given type old addresses from MTM and copies new from address control to MTM.
        *
        * @since S60 3.2
        *
        * @param aRecipientType Determines the address type to be copied
        */        
        IMPORT_C void CopyAddressesToMtmL( TMsvRecipientTypeValues aRecipientType );

        /**
        * Appends given type addresses from address control to MTM.
        * Note! Old addresses are not cleared from MTM
        *
        * @since S60 3.2
        *
        * @param aRecipientType Determines the address type to be appended
        */        
        IMPORT_C void AppendAddressesToMtmL( TMsvRecipientTypeValues aRecipientType );

        /**
        * Copies addresses from MTM to address control.
        *
        * @since S60 3.2
        *
        * @param aRecipientType Determines the address type to be appended
        * @param aAddInvalid    Specifies whether invalid contacts are added to address control
        *                       or stored for later use when client calls ShowInvalidRecipientInfoNotesL
        * 
        */        
        IMPORT_C void CopyAddressesFromMtmL( TMsvRecipientTypeValues aRecipientType,
                                             TBool aAddInvalid );

        /**
        * Verifies addresses in address control.
        *
        * @since S60 3.2
        *
        * @param aModified 
        *
        * @return ETrue, if verify successful,
        *         EFalse otherwise
        */        
        IMPORT_C TBool VerifyAddressesL( TBool& aModified );

        /**
        * Removes duplicate addresses from recipients at send time.
        * Removes duplicates from every address control at a time.
        *
        * @since S60 3.2
        *
        * @param aAppUi Pointer to editor appui
        * @param aAddressControls Array of address controls
        *
        * @return ETrue if something was removed,
        *         EFalse otherwise
        */        
        IMPORT_C static TBool RemoveDuplicateAddressesL( 
                CArrayPtrFlat<CMsgAddressControl>& aAddressControls );

        /**
        * Add recipient.
        *
        * @since S60 3.2
        *
        * @param aInvalid               OUT ETrue if invalid addresses found
        *                                   EFalse otherwise
        * @return ETrue if something was added,
        *         EFalse otherwise
        */
        IMPORT_C TBool AddRecipientL( TBool& aInvalid );

        /** 
        * Checks address.
        *
        * @since S60 3.2
        *
        * @param aAddress, phonenumber or email
        * @return ETrue if address is valid, EFalse otherwise
        */        
        IMPORT_C TBool CheckSingleAddressL( const TDesC& aAddress );

        /**
        * Creates a string to be set as entry iDetails.
        * Places addresses one after other separated by ";".
        *
        * @since S60 3.2
        *
        * @param IN/OUT aDetails
        */
        IMPORT_C void MakeDetailsL( TDes& aDetails );
         
        /**
        * Shows address information notes for invalid contacts detected when
        * copying addresses from MTM.Allows UI to
        * determine correct timing for showing this information to user.
        *
        * @since S60 3.2
        *
        */
        IMPORT_C void ShowInvalidRecipientInfoNotesL();
        
        /**
        * Sets valid address type for the address handler.
        *
        * @since S60 3.2
        */
        inline void SetValidAddressType( CMsgCheckNames::TMsgAddressSelectType aValidAddressType );
        
        /**
        * Returns currently valid address type.
        *
        * @since S60 3.2
        */ 
        inline CMsgCheckNames::TMsgAddressSelectType ValidAddressType() const;
        
   private:

        /**
        * Default C++ constructor.
        */
        CUniAddressHandler();
        
        /**
        * Constructor.
        */
        CUniAddressHandler( CBaseMtm& aMtm,
                            CMsgAddressControl& aControl,
                            CCoeEnv& aCoeEnv );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Shows address info (error) note
        *
        * @param aAddress
        * @param aAlias
        */
        void ShowAddressInfoNoteL( const TDesC& aAddress, const TDesC& aAlias );

        /**
        * Removes characters from string that confuses address 
        * control or clientMtm. Note that function does not make diffrence 
        * whether string is phone number or email address.
        *
        * @param aString
        * @return ETrue, chars were removed, EFalse otherwise 
        */
        static TBool RemoveIllegalChars( TDes& aString );

        /**
        * Check that character is not something that confuses address
        * control or clientMtm.
        *
        * @param aChar
        * @return ETrue, char is legal, EFalse otherwise 
        */
        static TBool IsValidChar( const TChar& aChar ); 

        /**
        * Converts number-strings to/from arabic-indic <-> western 
        * NOTE : Leaves string to CleanupStack
        * @param aOrigNumber contains the number to be converted
        * @param aDirection EFalse -> western, ETrue -> arabic-indic if needed
        * @return HBufC* buffer contains converted string
        */
        static HBufC* NumberConversionLC( const TDesC& aOrigNumber, TBool aDirection );

        /**
        * Performs proper recipient array cleanup.
        *
        * @param aArray CMsgRecipientArray pointer.
        */
        static void CleanupRecipientArray( TAny* aArray );
        
    private:
        
        struct TRecipientsInfo
            {
            CMsgRecipientArray*  iRecipients;
            TInt                 iOriginalCount;
            TBool                iModified;
            };
        
        //
        CBaseMtm&               iMtm;
        
        //
        CMsgAddressControl&     iControl;
        
        //
        CCoeEnv&                iCoeEnv;

        CMsgCheckNames*         iCheckNames;
        
        // List for invalid recipients.
        CMsgRecipientList*      iInvalidRecipients;
        
        // Valid address type.
        CMsgCheckNames::TMsgAddressSelectType iValidAddressType;
        
        RConeResourceLoader iResourceLoader;
    };

#include <uniaddresshandler.inl>

#endif // C_UNIADDRESSHANDLER_H

// End of File
