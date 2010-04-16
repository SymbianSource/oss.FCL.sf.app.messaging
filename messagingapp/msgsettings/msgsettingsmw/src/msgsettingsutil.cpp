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
 * Description:  This provides the messaging mw interface for sms 
 *
 */

#include <centralrepository.h>          // CRepository
#include <MessagingInternalCRKeys.h>    // Keys
#include "debugtraces.h"


#include "msgsettingsutil.h"

// read settings of email from CRE
TInt MsgSettingsUtil::ReadEmailOverSmsSettingsL(
                                                TDes& aSmsc, 
                                                TDes& aDestinationAddress)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingsUtil::ReadEmailOverSmsSettingsL";
#endif

    
    // Create storage
    CRepository* storage = CRepository::NewLC( KCRUidSmum );
    
    storage->Get( KSumEmailSC, aSmsc );
    storage->Get( KSumEmailGateway, aDestinationAddress );
    
    TBool modifiable = EFalse;
    storage->Get( KSumEmailModifiable, modifiable );
    CleanupStack::PopAndDestroy(); // storage

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgSettingsUtil::ReadEmailOverSmsSettingsL";
#endif

    
    return KErrNone;
}


//write email settings into CRE file
void MsgSettingsUtil::WriteEmailGatewayOverSmsSettingsL(
                             const TDes& aDestinationAddress,
                             const TBool& aModifiable)
{   
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingsUtil::WriteEmailGatewayOverSmsSettingsL";
#endif

        
    // Create storage
    CRepository* storage = CRepository::NewLC( KCRUidSmum );
    
    storage->Set( KSumEmailGateway, aDestinationAddress );
    storage->Set( KSumEmailModifiable, aModifiable );
    
    CleanupStack::PopAndDestroy(); // storage
    
#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgSettingsUtil::WriteEmailGatewayOverSmsSettingsL";
#endif

    return;
}

//write email settings into CRE file
void MsgSettingsUtil::WriteEmailServiceNumberOverSmsSettingsL(
                              const TDes& aSmsc,
                              const TBool& aModifiable)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingsUtil::WriteEmailServiceNumberOverSmsSettingsL";
#endif

    
    // Create storage
    CRepository* storage = CRepository::NewLC( KCRUidSmum );
    
    storage->Set( KSumEmailSC, aSmsc );
    storage->Set( KSumEmailModifiable, aModifiable );
    
    CleanupStack::PopAndDestroy(); // storage

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgSettingsUtil::WriteEmailServiceNumberOverSmsSettingsL";
#endif

    return;

}

//eof
