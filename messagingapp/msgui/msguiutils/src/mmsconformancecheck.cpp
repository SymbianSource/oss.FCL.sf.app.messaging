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


#include "mmsconformancecheck.h"

#include <MmsConformance.h>
#include <centralrepository.h>
#include <mmsconst.h>
#include <msgmediainfo.h>
#include <fileprotectionresolver.h>

#include <MsgMediaResolver.h>
#include <DRMHelper.h>
#include <MmsEngineInternalCRKeys.h>
#include <hbmessagebox.h>
#include <hbnotificationdialog>
#include <hbaction.h>

#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"
#include "unieditorgenutils.h" // This is needed for KDefaultMaxSize
#include "s60qconversions.h"
#include "debugtraces.h"

//DEFINES
#define RMODE_INSERT_ERROR hbTrId("Unable to insert. Object format not supported in restricted creation mode.")
#define INSERT_ERROR hbTrId("txt_messaging_dpopinfo_unable_to_attach_item_file")
#define INSERT_QUERY_CONFRM hbTrId("The receiving phone may not support this object. Continue?")
// -----------------------------------------------------------------------------
// MmsConformanceCheck::MmsConformanceCheck
// -----------------------------------------------------------------------------
//
MmsConformanceCheck::MmsConformanceCheck()
{
    QDEBUG_WRITE("MmsConformanceCheck::MmsConformanceCheck start");

    TRAP_IGNORE(        
        CRepository* repository = CRepository::NewL(KCRUidMmsEngine);
    CleanupStack::PushL(repository);

    //Fetch and set creation mode
    TInt creationMode = EMmsCreationModeRestricted;
    repository->Get(KMmsEngineCreationMode, creationMode);
    iCreationMode = creationMode;

    //Fetch and set max mms composition size
    TInt maxSize = KDefaultMaxSize;
    repository->Get( KMmsEngineMaximumSendSize, maxSize );
    iMaxMmsSize = maxSize;

    CleanupStack::PopAndDestroy(repository);        
    );

    QDEBUG_WRITE("MmsConformanceCheck::MmsConformanceCheck end");
}

// -----------------------------------------------------------------------------
// MmsConformanceCheck::~MmsConformanceCheck
// -----------------------------------------------------------------------------
//
MmsConformanceCheck::~MmsConformanceCheck()
{

}

// ---------------------------------------------------------
// MmsConformanceCheck::CheckModeForInsert
// ---------------------------------------------------------
//
int MmsConformanceCheck::checkModeForInsert(const QString& file,
    bool showNote)
{
    QDEBUG_WRITE("MmsConformanceCheck::CheckModeForInsert start");
    HBufC* filePath = S60QConversions::qStringToS60Desc(file);
    if (filePath)
    {
        CleanupStack::PushL(filePath);

        CMmsConformance* mmsConformance = CMmsConformance::NewL();
        mmsConformance->CheckCharacterSet(EFalse);
        
        CleanupStack::PushL(mmsConformance);
        
        CMsgMediaResolver* mediaResolver = CMsgMediaResolver::NewL();     
        mediaResolver->SetCharacterSetRecognition(EFalse);
        
        CleanupStack::PushL(mediaResolver);
        
        RFile fileHandle = mediaResolver->FileHandleL(*filePath);
        CleanupClosePushL(fileHandle);

        CMsgMediaInfo* info = mediaResolver->CreateMediaInfoL(fileHandle);
        mediaResolver->ParseInfoDetailsL(info,fileHandle);
        
        TMmsConformance conformance = mmsConformance->MediaConformance(*info);
        iConfStatus = conformance.iConfStatus;

        CleanupStack::PopAndDestroy(4);

        
        // In "free" mode user can insert images that are larger by dimensions than allowed by conformance
        if (iCreationMode != EMmsCreationModeRestricted)
        {
            TInt i = EMmsConfNokFreeModeOnly | EMmsConfNokScalingNeeded
                    | EMmsConfNokTooBig;
            TInt j = ~ (EMmsConfNokFreeModeOnly | EMmsConfNokScalingNeeded
                    | EMmsConfNokTooBig);

            // If user answers yes to Guided mode confirmation query he/she moves to free mode
            if ( (iConfStatus & i) && ! (iConfStatus & j))
            {
                if (iCreationMode == EMmsCreationModeFree || info->Protection()
                        & EFileProtSuperDistributable)
                {
                    // SuperDistribution not checked here
                    // Mask "FreeModeOnly" and "ScalingNeeded" away in free mode
                    iConfStatus &= ~EMmsConfNokFreeModeOnly;
                    iConfStatus &= ~EMmsConfNokScalingNeeded;
                }
                else if (showNote)
                {
                    HbMessageBox::question(INSERT_QUERY_CONFRM, this, SLOT(onDialogInsertMedia(HbAction*)));
                }
                else
                {
                    //query not accepted. Stop insertion.
                    return EInsertQueryAbort;
                }
            }
        }
        else if (iConfStatus & EMmsConfNokDRM || iConfStatus
                & EMmsConfNokNotEnoughInfo || iConfStatus
                & EMmsConfNokNotSupported || iConfStatus
                & EMmsConfNokFreeModeOnly || iConfStatus & EMmsConfNokCorrupt)
        {
            // Sanity check
            // "Not conformant" assumed if check fails.
            if(showNote)
            {
                showPopup(INSERT_ERROR);
            }

            return EInsertNotSupported;
        }
        delete info;
    }
    QDEBUG_WRITE("MmsConformanceCheck::CheckModeForInsert end");
    return EInsertSuccess;
}

// ---------------------------------------------------------
// MmsConformanceCheck::validateMsgForForward
// ---------------------------------------------------------
//
bool MmsConformanceCheck::validateMsgForForward(int messageId)
{
    UniDataModelLoader* pluginLoader = new UniDataModelLoader();
    UniDataModelPluginInterface* pluginInterface =
            pluginLoader->getDataModelPlugin(ConvergedMessage::Mms);
    pluginInterface->setMessageId(messageId);

    //Check if slide count is greater than 1
    if (pluginInterface->slideCount() > 1)
    {
        delete pluginLoader;
        return false;
    }

    //Check if message size is inside max mms composition limits 
    if (pluginInterface->messageSize() > iMaxMmsSize)
    {
        return false;
    }

    //If there is restricted content then return false
    UniMessageInfoList slideContentList = pluginInterface->slideContent(0);
    bool retValue = true;

    for (int i = 0; i < slideContentList.size(); ++i)
    {
        if (checkModeForInsert(slideContentList.at(i)->path(), false)
                != EInsertSuccess)
        {
            retValue = false;
            break;
        }
    }

    foreach(UniMessageInfo *slide,slideContentList)
        {
            delete slide;
        }

    if (!retValue)
    {
        delete pluginLoader;

        return false;
    }

    UniMessageInfoList modelAttachmentList = pluginInterface->attachmentList();

    for (int i = 0; i < modelAttachmentList.count(); ++i)
    {
        if (checkModeForInsert(modelAttachmentList.at(i)->path(), false)
                != EInsertSuccess)
        {
            retValue = false;
            break;
        }
    }

    foreach(UniMessageInfo *attachment,modelAttachmentList)
        {
            delete attachment;
        }

    delete pluginLoader;

    return retValue;
}

// -----------------------------------------------------------------------------
// MmsConformanceCheck::onDialogInsertMedia
// -----------------------------------------------------------------------------
//
void MmsConformanceCheck::onDialogInsertMedia(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        // Query accepted.
        // Mask "FreeModeOnly" and "ScalingNeeded" away in free mode
        iConfStatus &= ~EMmsConfNokFreeModeOnly;
        iConfStatus &= ~EMmsConfNokScalingNeeded;
    }

}

// -----------------------------------------------------------------------------
// MmsConformanceCheck::showPopup
// -----------------------------------------------------------------------------
//
void MmsConformanceCheck::showPopup(const QString& text)
{
    HbNotificationDialog* dlg = new HbNotificationDialog();
    dlg->setFocusPolicy(Qt::NoFocus);
    dlg->setAttribute(Qt::WA_DeleteOnClose, true);
    dlg->setText(text);
    dlg->show();
}

