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
* Description:  This file contains mtm panics and panic strings
*
*
*/

#ifndef __IMUMPANIC_H__
#define __IMUMPANIC_H__

// Internal API
_LIT( KImumInSettingsDataImpl, "ImumU: SetDataImpl - " );
_LIT( KImumInSettingsDataCollectionImpl, "ImumU: SetDataColImpl - " );
_LIT( KImumInternalApi, "ImumU: InternalApi - " );
_LIT( KImumInMailboxServices, "ImumU: MboxServ - " );
_LIT( KImumMboxSymbianDataConverter, "ImumU: SymDataConv - " );
_LIT( KImumInSettingsDataArray, "ImumU: SetDataArray - " );

// Imum Utils
_LIT( KImumMboxDefaultDataCtrl, "ImumU: DefDataCtrl - " );
_LIT( KImumMboxDefaultData, "ImumU: DefData - " );

_LIT( KImumCommsDbUtilPanic, "ImumU::CommsDb Utils - " );
_LIT( KIMASAccountControlPanic, "ImumU::AccCtrl - " );
_LIT( KIMASCenRepCtrlPanic, "ImumU::CenRepCtrl - " );
_LIT( KIMSStorerParamsPanic, "ImumU::StorerParams - " );
_LIT( KIMSSettingsDialogPanic, "ImumU::Settings - " );
_LIT( KIMSSettingsApproverPanic, "ImumU::Approver - " );
_LIT( KIMSWizardPanic, "ImumU: Wizard -" );
_LIT( KIMCSoftkeyControllerPanic, "ImumU: Softkey Control - " );
_LIT( KIMSSettingsNoteUiPanic, "ImumU: Note Ui - " );

// Imum MTM
_LIT( KImumMtmUiPanic, "Imum Mtm UI - " );

enum TImumPanics
    {
    EPanicRcvServiceMustBePOP3OrIMAP4,
    EPanicSendServiceMustBeSMTP,
    EPanicUnknownMtmType,
    EPanicUnknownNifState,
    EPanicIAPsDoNotMatch,
    EPanicOpNoMtmUi,
    EPanicOpBadFetchOp,
    EPanicAlreadySetFetchOp,
    EPanicSelectionEmpty,
    EPanicOpAlreadySet,
    EPanicSelAlreadyCreated,  // 10
    EPanicAlreadyHaveErrProg,
    EPanicNoOperation,
    EPanicWrongStateForOpErrorProgress,
    EIMAP4MtmUdFoldersNotSupported,
    EIMAP4MtmUdNoIconForAttachment,
    EIMAP4MtmUdNoIconForFolder,
    EIMAP4MtmUdRootEntryGiven,
    EIMAP4MtmUdUnknownOperationId,
    EIMAP4MtmUdWrongMtm,
    EIMAP4MtmUiSelectionIsEmpty,  // 20
    EIMAP4MtmUiSelectionOfMoreThanOneParent,
    EIMAP4MtmUiSelectionWithMessageOfWrongMtm,
    EIMAP4MtmUiOnlyWorksWithMessageEntries,
    EIMAP4MtmUiOnlyWorksWithServiceEntries,
    EIMAP4MtmUiOnlyWorksWithMessageAndServiceEntries,
    EIMAP4MtmUiCannotCopyOrMoveToRemote,
    EIMAP4MtmUiOnlyCreatesServiceEntries,
    EIMAP4MtmUiParentNotRoot,
    EIMAP4MtmUiOperationNull,
    EIMAP4MtmUiWrongMtm,  // 30
    EIMAP4MtmUiUnknownOperation,
    EIMAP4MtmUiNoProgress,
    EIMAP4MtmUiNullRelatedService,
    EIMAP4MtmUiOfflineNotSupported,
    EIMAP4MtmUiEditorFilenameDuplicate,
    EIMAP4MtmUiNoService,
    EIMAP4MtmUiInvalidProgress,
    EIMAP4MtmUiNullParameterInvalid,
    EIMAP4MtmUiEditorNoSmtpMtm,
    EIMAP4MtmUiEditorNoSmtpMtmUi, // 40
    EIMAP4MtmUiNotConnected,
    EIMAP4MtmUiRootIndexParameterInvalid,
    EIMAP4MtmUiNoBaseMTMContext,
    EIMAP4MtmUiOnlyWorksWithFolderEntries,
    EIMAP4MtmUiRepeatedCompleted,
    EIMAP4MtmUiNullOperation,
    EIMAP4MtmUiConnectFailedWithNoError,
    EIMAP4MtmUiBadStateInConnectionOp,
    EIMAP4MtmUiStartLCalledWhileStarted,
    EIMAP4MtmUiBadStateInSubscriptionOp,  // 50
    EIMAP4MtmUiNotAService,
    EIMAP4MtmUiNotObservedControl,
    EIMAP4MtmUiNoOriginatingEntry,
    EIMAP4MtmUiUnknownStateCheck,
    EIMAP4MtmUiBadStateInFolderListOp,
    EIMAP4MtmUiOnlyWorksWithAttachmentEntries,
    EIMAP4MtmUiMoreThanOneAttachmentFile,
    EIMAP4MtmUiIncorrectMessageStructure,
    EIMAP4MtmUiOnlyWorksWithFolderAndMessageEntries,
    EIMAP4MtmUiBadStateInFolderMessageDeleteOp, // 60
    EIMAP4MtmUiOperationStillActive,
    EIMAP4MtmUiDisplayConfNoteErr,
    EIMAP4MtmUiMoveToNotSupported,
    EIMAP4MtmUiCopyToNotSupported,
    EIMAP4MtmUiMoveFromNotSupported,
    EIMAP4MtmUiCopyFromNotSupported,
    EIMAP4MtmUiGetMailNotSupported,
    EIMAP4MtmUiPurgeNotSupported,
    EIMAP4MtmUiProgressMoveCopyWithinServiceNotSupported,
    EIMAP4MtmUiProgressUpdateFoldersNotSupported, // 70
    EIMAP4MtmUiProgressDeleteFolderNotSupported,
    EIMAP4MtmUiProgressAppendingNotSupported,
    EIMAP4MtmUiProgressMoveNotSupported,
    EIMAP4MtmUiSyncProgressFolder,
    EIMAP4MtmUiSyncProgressOther,
    EIMAP4MtmUiUiProgressSync,
    EIMAP4MtmUiUiProgressCreateFolder,
    EIMAP4MtmUiUiProgressSummaryCreateRenameFolder,
    EPop3MtmUdFoldersNotSupported,
    EPop3MtmUdNoIconAvailable,  // 80
    EPop3MtmUdNoIconForAttachment,
    EPop3MtmUdRootEntryGiven,
    EPop3MtmUdUnknownOperationId,
    EPop3MtmUdWrongMtm,
    EPop3MtmUiSelectionIsEmpty,
    EPop3MtmUiSelectionOfMoreThanOneParent,
    EPop3MtmUiSelectionWithMessageOfWrongMtm,
    EPop3MtmUiCannotCopyOrMoveToRemote,
    EPop3MtmUiParentNotRoot,
    EPop3MtmUiWrongMtm, // 90
    EPop3MtmUiUnknownOperation,
    EPop3MtmUiMoveNotAllowed,
    EPop3MtmUiUnknownProgressState,
    EPop3MtmUiNoProgress,
    EPop3MtmUiUnknownProgressType,
    EPop3MtmUiEditorEmptySelectionForFunction,
    EPop3MtmUiOpConnectNotAService,
    EPop3MtmUiOpUnknownState,
    EPop3MtmUiOpAlreadyHaveProgErr,
    EPop3MtmUiDisplayNoteErr, // 100
    ESmtpMtmUdWrongMtmType,
    ESmtpMtmUdNoIconForAttachment,
    ESmtpMtmUdNoIconAvailable,
    ESmtpMtmUdUnknownMsgStatus,
    ESmtpMtmUiEmptySelection,
    ESmtpMtmUiWrongMtm,
    ESmtpMtmUiFoldersNotSupported,
    ESmtpMtmUiNotAService,
    ESmtpMtmUiRootEntryGiven,
    ESmtpMtmUiServiceEntryIsNotSmtp,  // 110
    ESmtpMtmUiSmtpMessageNotLocal,
    ESmtpMtmUiSendOperationNoMessages,
    ESmtpMtmUiSendOperationAlreadySending,
    ESmtpMtmUiCannotCopyOrMoveFromSmtpServiceOrFolder,
    ESmtpMtmUiNullRelatedService,
    ESmtpMtmUiServiceParentNotRoot,
    ESmtpMtmUiEditorFilenameDuplicate,
    ESmtpMtmUiSvcDelOpNotAService,
    ESmtpMtmUiSvcDelOpNullRelatedServiceId,
    ESmtpMtmUiSvcDelOpParentIsNotRoot,  // 120
    ESmtpMtmUiNullNewMsgService,
    ESmtpMtmUiBadProgress,
    ESmtpMtmUiDisplayNoteErr,
    ESmtpMtmUiBadRFSLevel,
    EPanicOpStartLCalledWhileStarted,
    EPanicSelectionIsEmpty  // 126
    };

#endif // __IMUMPANIC_H__
