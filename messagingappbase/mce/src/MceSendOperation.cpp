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
*     Sends messages from Outbox when user selects "Start" in Outbox.
*
*/




// INCLUDE FILES
#include "MceSendOperation.h"   // header

#include <SenduiMtmUids.h>	// mtm uids
#include <MTMStore.h>		// CMtmStore
#include <mtmuibas.h>		// CBaseMtmUi
#include <muiumsvuiserviceutilitiesinternal.h>
#include <msvstd.hrh>


// CONSTANTS
const TInt KSendPriority=1000;
const TInt KSendSelectionGranularity=4;
const TInt KSendServicesGranularity=4;

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CMceSendOperation* CMceSendOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    CMtmStore& aMtmStore,
    CMsvEntrySelection* aSel)
    {
    CMceSendOperation* self = new ( ELeave ) CMceSendOperation(aMsvSession, aObserverRequestStatus, aMtmStore);
    CleanupStack::PushL( self );
    self->ConstructL( aSel );
    CleanupStack::Pop( self );
    return self;
    }

// C++ default constructor can NOT contain any code that
// might leave.
//
CMceSendOperation::CMceSendOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    CMtmStore& aMtmStore)
    :
    CMsvOperation(aMsvSession, KSendPriority, aObserverRequestStatus),
    iMtmStore( aMtmStore ),
    iSelections( KSendSelectionGranularity ),
    iServices( KSendServicesGranularity ),
    iBlank(_L8(""))
    {
    CActiveScheduler::Add(this);
    }

//destructor
CMceSendOperation::~CMceSendOperation()
    {
    Cancel();
    delete iOperation;
    delete iCEntry;
    iSelections.ResetAndDestroy();
    }

// ----------------------------------------------------
// CMceSendOperation::ConstructL
// ----------------------------------------------------
void CMceSendOperation::ConstructL(CMsvEntrySelection* aSelection)
    {
    //@what if service is defined, but has been deleted?
    //@assert aSelection->Count()>=1
    // split the selection up into separate selections for particular services (grouped by MTM)
    CleanupStack::PushL(aSelection);
    TUid   mtm;
    TMsvId service;
    TMsvId actualMessageService;
    
    CMsvEntry* rootEntry=iMsvSession.GetEntryL(KMsvRootIndexEntryIdValue);
    CleanupStack::PushL(rootEntry);
    rootEntry->SetSortTypeL(TMsvSelectionOrdering(KMsvNoGrouping, EMsvSortByNone, ETrue));

    TInt index;
    CMsvEntrySelection* sel;

    iCEntry=iMsvSession.GetEntryL(aSelection->At(0));

    // nextIndexStart is used to keep the services of a particular MTM together
    TInt nextIndexStart = KErrNotFound;
    TMsvId id;
    TBool isDefaultForMTM = EFalse;
    TBool sendfirst = EFalse;
    while (aSelection->Count()>0)
        {
        index = (nextIndexStart>KErrNotFound) ? nextIndexStart:0;
        iCEntry->SetEntryL( aSelection->At(index) );
        mtm=iCEntry->Entry().iMtm;
        sendfirst = ( mtm==KSenduiMtmSmsUid );
        actualMessageService=iCEntry->Entry().iServiceId;
        service=actualMessageService;
        TInt serviceFoundErr = KErrNone;
        TRAP(serviceFoundErr, rootEntry->ChildDataL(service));
        
        // Use the default service if the actual message service is not available
        if ( service == KMsvUnkownServiceIndexEntryIdValue
            || service == KMsvLocalServiceIndexEntryIdValue
            || serviceFoundErr )
            {
            isDefaultForMTM = ETrue;
            service = MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
                iMsvSession,
                mtm, 
                ETrue );
            const TInt getDefaultErr = ( service != KMsvUnkownServiceIndexEntryIdValue &&
                service != KMsvLocalServiceIndexEntryIdValue );

            TInt foundDefaultServiceErr = KErrNone;
            if ( getDefaultErr == KErrNone )
                {
                TRAP(foundDefaultServiceErr, rootEntry->ChildDataL(service));
                }
            if (getDefaultErr || foundDefaultServiceErr)
                {
                CMsvEntrySelection* servs=rootEntry->ChildrenWithMtmL(mtm);
                if (servs!=0 && servs->Count()>0)
                    {
                    service=servs->At(0);
                    }
                delete servs;
                }
            }
        else
            {
            isDefaultForMTM=EFalse;
            }
        sel=new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL(sel);
        if (sendfirst)
            {
            iServices.InsertL(0,service);
            iSelections.InsertL(0,sel);
            }
        else
            {
            iServices.AppendL(service);
            iSelections.AppendL(sel);
            }
        CleanupStack::Pop( sel );
        nextIndexStart = KErrNotFound;
        
        // Entry IDs for this service
        while (index<aSelection->Count())
            {
            id=aSelection->At(index);
            iCEntry->SetEntryL(id);
            TBool entryIsForThisMTM = (iCEntry->Entry().iMtm==mtm );
            TMsvId thisServiceId=iCEntry->Entry().iServiceId;
            if (entryIsForThisMTM && (thisServiceId==service  || (actualMessageService==thisServiceId && serviceFoundErr) || ((thisServiceId==KMsvUnkownServiceIndexEntryIdValue || thisServiceId==KMsvLocalServiceIndexEntryIdValue) && isDefaultForMTM)))
                {
                sel->AppendL(id);
                aSelection->Delete(index);
                }
            else
                {
                if (nextIndexStart == KErrNotFound && entryIsForThisMTM)
                    {
                    nextIndexStart=index;
                    }

                ++index;
                }
            }
        }

    CleanupStack::PopAndDestroy(2, aSelection); // aSelection, rootEntry

    StartNextOperation();
    }


// ----------------------------------------------------
// CMceSendOperation::StartNextOperation
// ----------------------------------------------------
void CMceSendOperation::StartNextOperation()
    {
    if(iOperation)
        {
        delete iOperation;
        iOperation = NULL;
        }
    TRAPD(err, MakeNewOperationL());

    // whether error or not, remove the data for this operation
    iServices.Delete(0);
    delete iSelections[0];
    iSelections.Delete(0);
    // and set active
    iStatus=KRequestPending;
    SetActive();
    // if error, then complete this pass with the error code
    if (err)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, err);
        }
    }

// ----------------------------------------------------
// CMceSendOperation::MakeNewOperationL
// ----------------------------------------------------
void CMceSendOperation::MakeNewOperationL()
    {
    // get the service entry
    iCEntry->SetEntryL(iServices[0]);
    // get the UI setting context to the service
    CBaseMtmUi& mtmui=iMtmStore.GetMtmUiAndSetContextLC(iCEntry->Entry());
    // set the new current selection
    iOperation=mtmui.CopyToL(*iSelections[0], iStatus);
    // set the current mtm
    iMtm=iCEntry->Entry().iMtm;
    CleanupStack::PopAndDestroy(); // release mtmUi
    }

// ----------------------------------------------------
// CMceSendOperation::DoCancel
// ----------------------------------------------------
void CMceSendOperation::DoCancel()
    {
    if (iOperation)
        {
        iOperation->Cancel();
        }
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, iStatus.Int());
    }

// ----------------------------------------------------
// CMceSendOperation::RunL
// ----------------------------------------------------
void CMceSendOperation::RunL()
    {
    if (iSelections.Count()==0)
        {
        // nothing left to process, so complete the observer
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete(status, iStatus.Int());
        }
    else
        {
        const TDesC8& progress=ProgressL();
        if (progress.Length()>0)
            {
            iMtmStore.GetMtmUiLC(iCEntry->Entry().iMtm).DisplayProgressSummary(progress);
            CleanupStack::PopAndDestroy(); // release mtmUi
            }
        // start the next pass
        StartNextOperation();
        }
    }

// ----------------------------------------------------
// CMceSendOperation::ProgressL
// ----------------------------------------------------
const TDesC8& CMceSendOperation::ProgressL()
    {
    if (iOperation)
        {
        return iOperation->ProgressL();
        }
    return iBlank;
    }

// End of file
