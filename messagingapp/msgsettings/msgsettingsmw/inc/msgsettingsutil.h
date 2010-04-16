/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  This provides some utility functions to msgsettings m/w
 *
 */

#ifndef MSGSETTINGS_UTIL_H
#define MSGSETTINGS_UTIL_H

#include <e32base.h>

class MsgSettingsUtil
{
    
public:
    
    /**
     * Function to read email settings from CRE file
     * @param aSmsC - sms center number
     * @param aDestinationAddress - gateway address
     * @return int error status
     */
   static TInt ReadEmailOverSmsSettingsL(
                          TDes& aSmsc, 
                          TDes& aDestinationAddress); 
   
   /**
    * function writing gateway address into CRE file
    * @param aDestinationAddress - gateway address
    * @param aModifiable - writing status
    */
   static void WriteEmailGatewayOverSmsSettingsL(
                             const TDes& aDestinationAddress,
                             const TBool& aModifiable);
   
   /**
    * function writing email service number
    * @param aSmsc - service center number
    * @param aModifiable - writing status
    */
   static void WriteEmailServiceNumberOverSmsSettingsL(
                             const TDes& aSmsc,
                             const TBool& aModifiable);   
};

#endif   //MSGSETTINGS_UTIL_H
