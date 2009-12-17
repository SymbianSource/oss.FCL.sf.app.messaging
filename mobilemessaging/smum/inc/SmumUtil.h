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
*     A simple utility class for providing common functionality
*     and minimizing duplicate code.
*
*/



#ifndef SMUMUTIL_H
#define SMUMUTIL_H

// FORWARD DECLARATIONS
class CSmsSettings;

// CLASS DECLARATION

/**
*  A simple utility class for providing common functionality
*  and minimizing duplicate code.
*/
class SmumUtil
    {
    public:

        /**
        * Finds default name for Service Centre and adds ordinal
        * number in the end if it is needed.
        * @param aName              A reference parameter for transferring the default name. 
        *                           Ensure the descriptor is long enough to handle default name + ordinal.
        * @param aSCNamesList       A pointer to list of SC names (if not 
        *                           given, returns only default name)
        */
        IMPORT_C static void FindDefaultNameForSCL(
            TDes& aName,
            TBool aNameIsForSCentreFromSIM,
            CDesCArrayFlat* aSCNamesList = NULL);

		/**
        * Returns the current Email-over-Sms settings
        * @since 2.6
        * @param aSmsc          
        * @param aDestinationAddress          
        * @param aModifiable
        * @return                   Symbian OS standard error code
        */
        IMPORT_C static TInt ReadEmailOverSmsSettingsL( 
            TDes& aSmsc, 
            TDes& aDestinationAddress,
            TBool& aModifiable );

        /**
        * Writes the current Email-over-Sms settings to file
        * @since 2.6
        * @param aSmsc service centre used
        * for Email over Sms messages
        * @param aDestinationAddress service number 
        * used for Email over Sms messages
        * @param aModifiable used to determine if user is allowed to 
        * modify setting or read only
        * @return                   Symbian OS standard error code
        */
        IMPORT_C static TInt WriteEmailOverSmsSettingsL( 
            const TDes& aSmsc, 
            const TDes& aDestinationAddress,
            const TBool& aModifiable );

        /**
        * Checks if Email Over Sms feature is supported 
        * according to local variation
        * @since 2.6
        * @return ETrue if feature supported
        */
        IMPORT_C static TBool CheckEmailOverSmsSupportL();

		/**
        * Shows an information note
        * @since 2.6
        * @param aResource, resourse id used in note text
        */
        static void ShowInformationNoteL( TInt aResource );
        
        /** 
        * Reads the MCE variation flags value from CentralRepository
        * @since 2.8
        * @param aKey, the key of interest
        * @return TInt, value of MCE variation flags
        */
        static TInt CheckVariationFlagsL( const TUid aUid, const TUint32 aKey );
        
    private:

        /**
        * Checks if Email Over Sms feature is supported 
        * according to Value Added Services (U)SIM, bit KCspSMMOEmail
        * @since 2.6
        * @return ETrue if feature supported
        */
        static TBool ReadEmailSupportFromVASL();

        // Prohibit copy constructor
        SmumUtil( const SmumUtil& );
        // Prohibit assigment operator
        SmumUtil& operator= ( const SmumUtil& );
    
    };

#endif      // SMUMUTIL.H   
            
// End of File
