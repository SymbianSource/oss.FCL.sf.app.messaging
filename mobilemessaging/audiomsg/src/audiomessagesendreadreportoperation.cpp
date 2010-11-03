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
*       CAudioMessageSendReadReportOperation, Storage for single attachment in presentation. 
*
*/



// ========== INCLUDE FILES ================================

#include <mmsconst.h>
#include <mmsclient.h>

#include "AudioMessageLogging.h"
#include "audiomessagesendreadreportoperation.h"

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
// CAudioMessageSendReadReportOperation
//
// Constructor.
// ---------------------------------------------------------
//
CAudioMessageSendReadReportOperation::CAudioMessageSendReadReportOperation(
        MAmsOperationObserver& aOperationObserver,
        CAudioMessageDocument& aDocument ) :
    CAudioMessageOperation( aOperationObserver, aDocument)
    {
    }

// ---------------------------------------------------------
// ~CAudioMessageSendReadReportOperation
//
// Destructor.
// ---------------------------------------------------------
//
CAudioMessageSendReadReportOperation::~CAudioMessageSendReadReportOperation()
    {
    Cancel();
    delete iSendReadReportOperation;
    }


// ---------------------------------------------------------
// SendReadReportL
// ---------------------------------------------------------
//
void CAudioMessageSendReadReportOperation::SendReadReportL( )
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
void CAudioMessageSendReadReportOperation::RunL()
    {    
    if (    iStatus.Int() != KErrNone 
        &&  iStatus.Int() != KErrGeneral )
        {
        AMSLOGGER_WRITEF( _L("CAudioMessageSendReadReportOperation::RunL iStatus=%d"), iStatus.Int() );
        SetError( iStatus.Int() );
        iObserver.EditorOperationEvent(
            EAmsOperationSendReadReport,
            EAmsOperationError );
        }
    else
        {
        switch ( iNextState )
            {
            case EAmsReadReportEnd:
                {
#ifdef USE_LOGGER
                if ( iStatus.Int() == KErrGeneral )
                    {
                    // Report was not permitted
                    AMSLOGGER_WRITE( "CAudioMessageSendReadReportOperation::RunL Report was not permitted" );
                    }
                else
                    {                        
                    AMSLOGGER_WRITE( "CAudioMessageSendReadReportOperation::RunL Report sent OK" );
                    }
#endif
                iObserver.EditorOperationEvent(
                    EAmsOperationSendReadReport,
                    EAmsOperationComplete );
                break;
                }
            default:
                iObserver.EditorOperationEvent(
                    EAmsOperationSendReadReport,
                    EAmsOperationError ); 
                break;
            }
        }
    }

// ---------------------------------------------------------
// DoCancel
//
// ---------------------------------------------------------
//
void CAudioMessageSendReadReportOperation::DoCancel()
    {
    AMSLOGGER_WRITE( "CAudioMessageSendReadReportOperation::DoCancel" );
    if ( iSendReadReportOperation )
    	{
        iSendReadReportOperation->Cancel();
    	}
    }


// EOF
