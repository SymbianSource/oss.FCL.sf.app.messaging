/*
* Copyright (c) 2002 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MuiuMsvUiServiceUtilitiesInternal  declaration
*
*/




#ifndef __MSVUISERVICEUTILITIESINTERNAL_H__
#define __MSVUISERVICEUTILITIESINTERNAL_H__

//  INCLUDES
#include <msvipc.h>
#include <commonphoneparser.h>

// DATA TYPES
/// Address types used inside messaging application
enum TMuiuAddressType   
    {
    EMuiuAddressTypeNone,         // Address does not exist or type is not any of below values
    EMuiuAddressTypePhoneNumber,
    EMuiuAddressTypeEmail
    };


// FORWARD DECLARATIONS
class CMsvSession;
class RFs;
class CEikonEnv;
class CMsgVoIPExtension;
/**
* Static class to help service handling.
*/
class MsvUiServiceUtilitiesInternal
    {
    public:

        /**
        * Messaging application viewer or other application may call to sender or 
        * higlighted address when send key is pressed or in other situation. 
        * This function makes the decision whether voice or VoIP
        * call is done. For more information about usage see CallToSenderQueryL(),
        * CallServiceL(), InternetCallToSenderQueryL() and InternetCallServiceL(). 
        * @param aSenderAddress: Sender's address number. Calling voice call to email
        *           address is not supported. 
        *           Address must be pure address i.e. alias must not be part of it.
        * @param aSenderName: Sender name
        * @param aAddressInFocus: Address which is highlighted in message. 
        *           Calling voice call to email address is not supported.
        *           Address must be pure address i.e. alias must not be part of it.
        * @param aDialWithoutQueries: If this parameter is true and calling to sender 
        *           is chosen, then call immediately to sender of the message and 
        *           doesn't ask nothing from user before it.
        * @return Returns ETrue if the address was valid.
        */
        IMPORT_C static TBool InternetOrVoiceCallServiceL( 
            const CMsgVoIPExtension&    aVoIPExtension,
            const TDesC&                aSenderAddress,
            const TDesC&                aSenderName,
            const TDesC&                aAddressInFocus = KNullDesC,
            TBool                       aDialWithoutQueries = ETrue,
            CEikonEnv*                  aEnv = NULL );

        /**
        * Messaging application viewer or other application may call to sender or 
        * higlighted address when send key is pressed or in other situation. 
        * This function makes the decision whether voice or VoIP
        * call is done. For more information about usage see CallToSenderQueryL(),
        * CallServiceL(), InternetCallToSenderQueryL() and InternetCallServiceL(). 
        * @param aSenderAddress: Sender's address number. Calling voice call to email
        *           address is not supported. 
        *           Address must be pure address i.e. alias must not be part of it.
        * @param aSenderName: Sender name
        * @param aSenderAddressType address type of sender
        * @param aAddressInFocus: Address which is highlighted in message. 
        *           Calling voice call to email address is not supported.
        *           Address must be pure address i.e. alias must not be part of it.
        * @param aAddressInFocusType address type of focused address
        * @param aDialWithoutQueries: If this parameter is true and calling to sender 
        *           is chosen, then call immediately to sender of the message and 
        *           doesn't ask nothing from user before it.
        * @return Returns ETrue if the address was valid.
        */
        IMPORT_C static TBool InternetOrVoiceCallServiceL( 
            CEikonEnv&                  aEnv,
            const CMsgVoIPExtension&    aVoIPExtension,
            const TDesC&                aSenderAddress,
            const TDesC&                aSenderName,
            TMuiuAddressType            aSenderAddressType,
            const TDesC&                aAddressInFocus = KNullDesC,
            TMuiuAddressType            aAddressInFocusType = EMuiuAddressTypeNone,
            TBool                       aDialWithoutQueries = ETrue );

        /**
        * Used when asking from user if calling to sender of the message and dial to sender
        * if user select "Yes". Check immediately if phone number is valid. If phone number
        * is not valid then return EFalse and doesn't do nothing.If parameter "aDialWithoutQueries" 
        * is true then call immediately to sender of the message and doesn't ask nothing from user before it.
        * @param aNumber: Phone number.
        * @param aName: Name
        * @param aDialWithoutQueries: If this parameter is true then call immediately 
        * to sender of the message and doesn't ask nothing from user before it.
        * @return Returns ETrue if the phone number was valid.
        */
        IMPORT_C static TBool CallToSenderQueryL( 
            const TDesC& aNumber,
            const TDesC& aName, 
            TBool aDialWithoutQueries = ETrue );

        /**
        * When SMS viewer or MMS viewer is open it is possible to call
        * back to sender or phone number which is currently highlighted
        * in message. If user select Send-key then CAknListQueryDialog 
        * is opened with items "Back to sender" and currently highlighted
        * phone number and with soft keys "Call" and "Cancel". If item
        * "Back to sender" is selected and user select "Call" then function
        * call "CallToSenderQueryL" function with parameters "aSenderNumber"
        * and "aSenderName".
        * If currently highlighted item is selected and user select "Call"
        * then function call "CallToSenderQueryL" function with parameters "aSenderNumber"
        * and KNullDesC. If no phone number is highlighted in message and user
        * select Send-key then function launch the normal call query.
        * @param aSenderNumber: Sender phone number.
        * @param aSenderName: Sender name
        * @param aNumberInFocus: Phone number which is highlighted in message
        * @return Returns ETrue if the phone number was valid.
        */
        IMPORT_C static TBool CallServiceL( 
            const TDesC& aSenderNumber,
            const TDesC& aSenderName,
            const TDesC& aNumberInFocus = KNullDesC,
            CEikonEnv* aEnv = NULL );

        /**
        * Used when asking from user if calling to sender of the message and dial to sender
        * if user select "Yes". Assumes allways that Internet call number is valid and 
        * ETrue returned. If parameter "aDialWithoutQueries" 
        * is true then call immediately to sender of the message and doesn't ask nothing from user before it.
        * @since 3.0
        * @param aNumber: Phone number.
        * @param aName: Name
        * @param aDialWithoutQueries: If this parameter is true then call immediately 
        * to sender of the message and doesn't ask nothing from user before it.
        * @return Returns ETrue.
        */
        IMPORT_C static TBool InternetCallToSenderQueryL( 
            const TDesC& aNumber,
            const TDesC& aName, 
            TBool aDialWithoutQueries = ETrue );
            
        /**
        * When SMS viewer or MMS viewer is open it is possible to call
        * back to sender or phone number which is currently highlighted
        * in message. If user select Send-key then CAknListQueryDialog 
        * is opened with items "Back to sender" and currently highlighted
        * phone number and with soft keys "Call" and "Cancel". If item
        * "Back to sender" is selected and user select "Call" then function
        * call "CallToSenderQueryL" function with parameters "aSenderNumber"
        * and "aSenderName".
        * If currently highlighted item is selected and user select "Call"
        * then function call "CallToSenderQueryL" function with parameters "aSenderNumber"
        * and KNullDesC. If no phone number is highlighted in message and user
        * select Send-key then function launch the normal call query.
        * @since 3.0
        * @param aSenderNumber: Sender phone number.
        * @param aSenderName: Sender name
        * @param aNumberInFocus: Phone number which is highlighted in message
        * @return Returns ETrue if the phone number was valid.
        */
        IMPORT_C static TBool InternetCallServiceL( 
            const TDesC& aSenderNumber,
            const TDesC& aSenderName,
            const TDesC& aNumberInFocus = KNullDesC,
            CEikonEnv* aEnv = NULL );

        /**
        * Sets message store to phone's internal memory.
        * This function is possible to use for example in situation
        * where current drive is memory card and memory card is not in use.
        * @param aSession: reference to CMsvSession object
        *
        */
        IMPORT_C  static void ChangeMessageStoreToPhoneL( CMsvSession& aSession );

        /**
        * Check if iUnicueID of memory card is same than uniqueid of
        * memory card in message server.
        * @param aSession: reference to CMsvSession object
        * @return Returns ETrue if iUniqueID of memory card is same than
        * memory card unique id in message server.
        *
        */
        IMPORT_C static TBool MmcDriveInfoL( CMsvSession& aSession );

        /**
        * Returns id of the default service of certain mtm type.
        * @param aSession: reference to CMsvSession object
        * @param aMtm: uid of the mtm type to be searched.
        * @param aFindFirstServiceIfNoDefault: if true returns first service
        *    if no default set.
        * @return TMsvId of the default service.
        *    KMsvUnknownServiceIndexEntryId if no default service.
        */
        IMPORT_C static TMsvId DefaultServiceForMTML(
            CMsvSession& aSession,
            TUid aMtm,
            TBool aFindFirstServiceIfNoDefault );

        /**
        * Sets default service for the certain mtm type.
        * @param aSession: reference to CMsvSession object
        * @param aMtm: uid of the mtm type to be searched.
        * @param aServiceId: id of the new default service.
        */
        IMPORT_C static void SetDefaultServiceForMTML(
            CMsvSession& aSession,
            TUid aMtm,
            TMsvId aServiceId );
            
        /**
        * Checks if phone is in offline mode or not
        * Make sure that offline feature has been checked 
        * using feature manager
        *
        * @return ETrue if phone is in offline mode
        *         Otherwise EFalse.
        */
        IMPORT_C static TBool IsPhoneOfflineL(); 

        /**
        * Resolves type of given address
        * Phone number is checked by CommonPhoneParser. 
        * Email address is checked by MsvUiServiceUtilities
        * @param aAddress address to check
        * @param aDefaultPhoneNumberType subtype of phone number check
        * @return TMuiuAddressType
        */
        IMPORT_C static TMuiuAddressType ResolveAddressTypeL(    
                    const TDesC&                        aAddress,
                    CommonPhoneParser::TPhoneNumberType aDefaultPhoneNumberType = 
                       CommonPhoneParser::EPhoneClientNumber);
                     
        /**
        * Converts UTC time to local time
        *
        * @param aTime Utc time that is converted to local time on return. 
        */
        IMPORT_C static void ConvertUtcToLocalTime( TTime& aTime );
        
        /**
         * Compares two phone numbers.
         * @param aNumber1
         * @param aNumber2
         * @return ETrue if numbers are same, EFalse if numbers are different.
         */
        IMPORT_C static TBool ComparePhoneNumberL(
            const TDesC& aNumber1, const TDesC& aNumber2, TInt aCompareLength = -1);
        IMPORT_C static TBool OtherEmailMTMExistL( CMsvSession& aSession, TInt aMailbox = 0 );
                       
    private:
        /**
        * Launch CMuiuListQueryDialog
        * @param aGroupedNumberPtr Phone number 
        * which is groupped before it goes to dialog
        * @param aIndex tell which item is selected when dialog is closed
        * @return command id of dialog:
        * return 1 if user select some item in the list, 
        * return 0 if user cancel selection
        */
        static TInt LaunchDialogL( const TPtr aGroupedNumberPtr, TInt* aIndex );

    };

#endif //__MSVUISERVICEUTILITIES_H__

// End of file
