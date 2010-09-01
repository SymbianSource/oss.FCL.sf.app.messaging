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
*       CMmsViewerSendReadReportOperation, Storage for single attachment in presentation. 
*
*/



// ========== INCLUDE FILES ================================

#include <mmsconst.h>
#include "MmsMtmConst.h"
#include <mmsclient.h>

#include "MmsViewerSendReadReportOperation.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMmsViewerSendReadReportOperation
//
// Constructor.
// ---------------------------------------------------------
//
CMmsViewerSendReadReportOperation::CMmsViewerSendReadReportOperation(
        MMmsViewerOperationObserver& aOperationObserver,
        CMmsViewerDocument& aDocument,
        RFs& aFs ) :
    CMmsViewerOperation( aOperationObserver, aDocument, aFs )
    {
    }

// ---------------------------------------------------------
// ~CMmsViewerSendReadReportOperation
//
// Destructor.
// ---------------------------------------------------------
//
CMmsViewerSendReadReportOperation::~CMmsViewerSendReadReportOperation()
    {
    Cancel();
    delete iSendReadReportOperation;
    }


// ---------------------------------------------------------
// SendReadReportL
// ---------------------------------------------------------
//
void CMmsViewerSendReadReportOperation::SendReadReportL( )
    {
    iStatus = KRequestPending;
    SetActive();
    iSendReadReportOperation =  iDocument.Mtm( ).SendReadReportL( 
                                iDocument.Entry().Id( ),
                                iStatus );
    }


// ---------------------------------------------------------
// RunL
// ---------------------------------------------------------
//
void CMmsViewerSendReadReportOperation::RunL()
    {    
    if (    iStatus.Int() != KErrNone 
        &&  iStatus.Int() != KErrGeneral )
        {
        LOGTEXT2(_L("CMmsViewerSendReadReportOperation::RunL iStatus=%d"), iStatus.Int() );
        iObserver.ViewerOperationEvent(
            EMmsViewerOperationReadReport,
            EMmsViewerOperationError,
            iStatus.Int() );
        }
    else
        {
#ifdef USE_LOGGER
        if ( iStatus.Int() == KErrGeneral )
            {
            // Report was not permitted
            LOGTEXT(_L8("CMmsViewerSendReadReportOperation::RunL Report was not permitted") );
            }
        else
            {                        
            LOGTEXT(_L8("CMmsViewerSendReadReportOperation::RunL Report sent OK") );
            }
#endif
        iObserver.ViewerOperationEvent(
            EMmsViewerOperationReadReport,
            EMmsViewerOperationComplete,
            KErrNone ); 
        }
    }

// ---------------------------------------------------------
// DoCancel
//
// ---------------------------------------------------------
//
void CMmsViewerSendReadReportOperation::DoCancel()
    {
    LOGTEXT(_L8("CMmsViewerSendReadReportOperation::DoCancel") );
    // iSendReadReportOperation completes ourselves, when it cancels
    if ( iSendReadReportOperation )
        {            
        iSendReadReportOperation->Cancel();
        }
    iObserver.ViewerOperationEvent(
        EMmsViewerOperationReadReport,
        EMmsViewerOperationCancel,
        KErrNone );
    }


// ---------------------------------------------------------
// RunError
//
// ---------------------------------------------------------
//
TInt CMmsViewerSendReadReportOperation::RunError( TInt aError )
    {
    LOGTEXT2(_L("CMmsViewerSendReadReportOperation::RunError aError"), aError );
    iObserver.ViewerOperationEvent(
        EMmsViewerOperationReadReport,
        EMmsViewerOperationError,
        aError );
    return KErrNone;
    }

// EOF
