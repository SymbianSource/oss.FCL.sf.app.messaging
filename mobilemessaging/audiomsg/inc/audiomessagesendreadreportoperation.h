/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       CAudioMessageSendReadReportOperation checks whether
*       read report can be sent and sends it if requested and permitted.
*
*/




#ifndef __AUDIOMESSAGESENDREADREPORTOPERATION_H
#define __AUDIOMESSAGESENDREADRREPORTOPERATION_H

// INCLUDES

#include <e32base.h>
#include <e32std.h>

#include "amsoperationobserver.h"
#include "audiomessageoperation.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CAudioMessageDocument;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CAudioMessageSendReadReportOperation checks whether
* read report can be sent and sends it if requested and permitted.
* 
* @since 3.2
*/
class CAudioMessageSendReadReportOperation :   public CAudioMessageOperation
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @since    3.2
        */
        CAudioMessageSendReadReportOperation(
            MAmsOperationObserver& aOperationObserver,
            CAudioMessageDocument& aDocument );

        /**
        * Destructor
        *
        * @since    3.2
        */
        virtual ~CAudioMessageSendReadReportOperation();

        /**
        * Send the read report if sender of the original message asked it and 
        * if it was permitted by mms settings
        *
        * @since    3.2
        */
        void SendReadReportL( );
        
    private:
        
        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();
    
        /**
        * From CActive
        */

    protected: 

    private:

    protected: // data

        enum TAmsReadReportState
            {
            EAmsReadReportEnd = 0
            };
        TAmsReadReportState iNextState;
        CMsvOperation*      iSendReadReportOperation;
    };


#endif // __AudioMessageSendReadReportOperation_H
