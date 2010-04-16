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
 * Description: mms creation mode checks
 *
 */


#include "mmsinsertcheckoperation.h"

#include <MmsConformance.h>
#include <centralrepository.h>
#include <mmsconst.h>
#include <msgmediainfo.h>
#include <fileprotectionresolver.h>

#include <MsgMediaResolver.h>
#include <DRMHelper.h>
#include <MmsEngineInternalCRKeys.h>
#include <hbmessagebox.h>

#include "s60qconversions.h"
#include "debugtraces.h"

//DEFINES
#define RMODE_INSERT_ERROR hbTrId("Unable to insert. Object format not supported in restricted creation mode.")
#define INSERT_ERROR hbTrId("Unable to insert. Object format not supported.")
#define INSERT_QUERY_CONFRM hbTrId("The receiving phone may not support this object. Continue?")
// -----------------------------------------------------------------------------
// MmsInsertCheckOperation::MmsInsertCheckOperation
// -----------------------------------------------------------------------------
//
MmsInsertCheckOperation::MmsInsertCheckOperation()
{
    QDEBUG_WRITE("MmsInsertCheckOperation::MmsInsertCheckOperation start");

    CRepository* repository = CRepository::NewL(KCRUidMmsEngine);
    CleanupStack::PushL(repository);
    TInt creationMode = EMmsCreationModeRestricted;
    if (repository->Get(KMmsEngineCreationMode, creationMode) == KErrNone) {
        iCreationMode = creationMode;
    }
    CleanupStack::PopAndDestroy(repository);

    iMmsConformance = CMmsConformance::NewL();
    iMmsConformance->CheckCharacterSet(EFalse);
    iMediaResolver = CMsgMediaResolver::NewL();
    iMediaResolver->SetCharacterSetRecognition(EFalse);
    iDRMHelper = CDRMHelper::NewL();

    QDEBUG_WRITE("MmsInsertCheckOperation::MmsInsertCheckOperation end");
}

// -----------------------------------------------------------------------------
// MmsInsertCheckOperation::~MmsInsertCheckOperation
// -----------------------------------------------------------------------------
//
MmsInsertCheckOperation::~MmsInsertCheckOperation()
{
    delete iMmsConformance;
    delete iMediaResolver;
    delete iDRMHelper;
}

// ---------------------------------------------------------
// MmsInsertCheckOperation::CheckModeForInsertL
// ---------------------------------------------------------
//
int MmsInsertCheckOperation::checkModeForInsert(const QString& file)
{
    QDEBUG_WRITE("CheckModeForInsert::CheckModeForInsert start");
    HBufC* filePath = S60QConversions::qStringToS60Desc(file);
    if (filePath) {
        CleanupStack::PushL(filePath);

        RFile fileHandle = iMediaResolver->FileHandleL(*filePath);
        CleanupClosePushL(fileHandle);

        CMsgMediaInfo* info = iMediaResolver->CreateMediaInfoL(fileHandle);
        info->ParseInfoDetails(fileHandle, *iDRMHelper, *this);
        CleanupStack::PopAndDestroy(2);

        TMmsConformance conformance = iMmsConformance->MediaConformance(*info);
        TUint32 confStatus = conformance.iConfStatus;

        // In "free" mode user can insert images that are larger by dimensions than allowed by conformance
        if (iCreationMode != EMmsCreationModeRestricted) {
            TInt i = EMmsConfNokFreeModeOnly | EMmsConfNokScalingNeeded | EMmsConfNokTooBig;
            TInt j = ~(EMmsConfNokFreeModeOnly | EMmsConfNokScalingNeeded | EMmsConfNokTooBig);

            // If user answers yes to Guided mode confirmation query he/she moves to free mode
            if ((confStatus & i) && !(confStatus & j)) {
                if (iCreationMode == EMmsCreationModeFree || info->Protection()
                    & EFileProtSuperDistributable) {
                    // SuperDistribution not checked here
                    // Mask "FreeModeOnly" and "ScalingNeeded" away in free mode
                    confStatus &= ~EMmsConfNokFreeModeOnly;
                    confStatus &= ~EMmsConfNokScalingNeeded;
                }
                else if (launchEditorQuery()) {
                    // Query accepted.
                    // Mask "FreeModeOnly" and "ScalingNeeded" away in free mode
                    confStatus &= ~EMmsConfNokFreeModeOnly;
                    confStatus &= ~EMmsConfNokScalingNeeded;
                }
                else {
                    //query not accepted. Stop insertion.
                    return EInsertQueryAbort;
                }
            }
        }
        else if (confStatus & EMmsConfNokDRM || confStatus & EMmsConfNokNotEnoughInfo || confStatus
            & EMmsConfNokNotSupported || confStatus & EMmsConfNokFreeModeOnly || confStatus
            & EMmsConfNokCorrupt) {
            // Sanity check
            // "Not conformant" assumed if check fails.     
            if (iCreationMode == EMmsCreationModeRestricted)
            HbMessageBox::launchInformationMessageBox(RMODE_INSERT_ERROR);
            else
            HbMessageBox::launchInformationMessageBox(INSERT_ERROR);
            
            return EInsertNotSupported;
        }
        delete info;
    }
    QDEBUG_WRITE("CheckModeForInsert::CheckModeForInsert end");
    return EInsertSuccess;
}

// ---------------------------------------------------------
// MmsInsertCheckOperation::launchEditorQuery
// ---------------------------------------------------------
//
bool MmsInsertCheckOperation::launchEditorQuery()
{
    return HbMessageBox::launchQuestionMessageBox(INSERT_QUERY_CONFRM);
}

// -----------------------------------------------------------------------------
// MmsInsertCheckOperation::MediaInfoParsed
// -----------------------------------------------------------------------------
//
void MmsInsertCheckOperation::MediaInfoParsed()
{
}
