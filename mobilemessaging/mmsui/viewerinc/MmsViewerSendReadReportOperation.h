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
*       CMmsViewerSendReadReportOperation checks whether
*       read report can be sent and sends it if requested and permitted.
*
*/




#ifndef __MMSVIEWERSENDREADREPORTOPERATION_H
#define __MMSVIEWERSENDREADREPORTOPERATION_H

// INCLUDES

#include <e32base.h>
#include <e32std.h>

#include "MmsViewerOperation.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CMmsViewerDocument;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CMmsViewerSendReadReportOperation checks whether
* read report can be sent and sends it if requested and permitted.
* 
* @since 3.2
*/
class CMmsViewerSendReadReportOperation :   public CMmsViewerOperation
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @since    3.2
        */
        CMmsViewerSendReadReportOperation(
            MMmsViewerOperationObserver& aOperationObserver,
            CMmsViewerDocument& aDocument,
            RFs& aFs );

        /**
        * Destructor
        *
        * @since    3.2
        */
        virtual ~CMmsViewerSendReadReportOperation();

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
        TInt RunError( TInt aError );

    protected: 

    private:

    protected: // data

       CMsvOperation*      iSendReadReportOperation;
    };


#endif // __MmsViewerSendReadReportOperation_H
