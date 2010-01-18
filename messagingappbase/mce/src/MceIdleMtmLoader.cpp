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
*     Active object to load mtm's background after mce start.
*
*/



// INCLUDE FILES
#include <featmgr.h>  // CR : 401-1806
#include <MTMStore.h>
#include <mtmuids.h> // KUidMtmQueryCanSendMsg
#include <mtud.hrh>  // EMtudCommandSendAs
#include <mtudreg.h>
#include <MtmExtendedCapabilities.hrh> //KUidMsvMtmUiQueryExtendedGetMailProgress

#include <SenduiMtmUids.h>
#include "MceIdleMtmLoader.h"
#include "MceLogText.h"
#include "MsgFunctionInfo.h"
//cmail update
#define KUidMsgTypeFsMtmVal               0x2001F406
//cmail update

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CMceIdleMtmLoader::CMceIdleMtmLoader(
    CMtmStore&      aMtmStore,
    CUidNameArray&  aMsgTypesWritePopup,
    CUidNameArray&  aMsgTypesWriteSubmenu,
    CUidNameArray&  aMsgTypesSettings,
    CMsgFuncArray&  aMTMFunctionsArray,
    CMtmUiDataRegistry& aUiRegistry )
:   CActive( CActive::EPriorityIdle ),
    iUiRegistry( aUiRegistry ),
    iMtmStore(aMtmStore),
    iMsgTypesWritePopup( aMsgTypesWritePopup ),
    iMsgTypesWriteSubmenu( aMsgTypesWriteSubmenu ),
    iMsgTypesSettings( aMsgTypesSettings ),
    iMTMFunctionsArray( aMTMFunctionsArray ),
    iAudioMsgEnabled (EFalse), 
    iPostcardEnabled (EFalse)
    {
    CActiveScheduler::Add(this);
    }

// destructor
CMceIdleMtmLoader::~CMceIdleMtmLoader()
    {
    }

// ---------------------------------------------------------
// CMceIdleMtmLoader::StartL
// ---------------------------------------------------------
//
void CMceIdleMtmLoader::StartL(CMsvSession& /*aSession*/, TBool aAudioMsgEnabled, TBool aPostcardEnabled)
    {
    MCELOGGER_ENTERFN("CMceIdleMtmLoader::StartL()");
    iMsgTypesWriteSubmenu.Reset();
    iMsgTypesWritePopup.Reset();
    iMsgTypesSettings.Reset();
    iMTMFunctionsArray.Reset();
    iRegMtmIndex = 0;
    iAudioMsgEnabled = aAudioMsgEnabled ;  // CR : 401-1806
    iPostcardEnabled = aPostcardEnabled ;  // CR : 401-1806
    QueueLoad();
    MCELOGGER_LEAVEFN("CMceIdleMtmLoader::StartL()");
    }


// ---------------------------------------------------------
// CMceIdleMtmLoader::FinishL
// ---------------------------------------------------------
//
void CMceIdleMtmLoader::FinishL(TBool aAudioMsgEnabled, TBool aPostcardEnabled)
    {
    MCELOGGER_ENTERFN("CMceIdleMtmLoader::FinishL()");
    iAudioMsgEnabled = aAudioMsgEnabled ;  // CR : 401-1806
    iPostcardEnabled = aPostcardEnabled ;  // CR : 401-1806
    
    if(IsActive())
        {
        // First consume the outstanding completion.
        Cancel();
        // Now load the remaining MTMs synchronously.
        while(iRegMtmIndex < iUiRegistry.NumRegisteredMtmDlls())
            {
            LoadMtmL( iUiRegistry.MtmTypeUid(iRegMtmIndex) );
            iRegMtmIndex++;
            }

        MCELOGGER_WRITE_TIMESTAMP("Time: ");

        // And finally sort the array.
        SortAndChangeSmsMmsEmailFirstL();
        }
    MCELOGGER_LEAVEFN("CMceIdleMtmLoader::FinishL()");
    }


// ---------------------------------------------------------
// CMceIdleMtmLoader::DoCancel
// ---------------------------------------------------------
//
void CMceIdleMtmLoader::DoCancel()
    {
    }


// ---------------------------------------------------------
// CMceIdleMtmLoader::RunL
// ---------------------------------------------------------
//
void CMceIdleMtmLoader::RunL()
    {
    MCELOGGER_ENTERFN("CMceIdleMtmLoader::RunL()");
    LoadMtmL(iUiRegistry.MtmTypeUid(iRegMtmIndex) );
    iRegMtmIndex++;
    QueueLoad();
    if(!IsActive())
        {
        // Sort the array after all MTMs loaded.
        SortAndChangeSmsMmsEmailFirstL();

        MCELOGGER_WRITE("CMceIdleMtmLoader: Mtm loading finished");
        MCELOGGER_WRITE_TIMESTAMP("Time: ");


        }
    MCELOGGER_LEAVEFN("CMceIdleMtmLoader::RunL()");
    }


// ---------------------------------------------------------
// CMceIdleMtmLoader::QueueLoad
// ---------------------------------------------------------
//
void CMceIdleMtmLoader::QueueLoad()
    {
    MCELOGGER_ENTERFN("CMceIdleMtmLoader::QueueLoad()");
    const TInt mtmCount = iUiRegistry.NumRegisteredMtmDlls();
    if( iRegMtmIndex < mtmCount )
        {
        TRequestStatus *s = &iStatus;
        User::RequestComplete(s, KErrNone);
        SetActive();
        }
    MCELOGGER_LEAVEFN("CMceIdleMtmLoader::QueueLoad()");
    }


// ---------------------------------------------------------
// CMceIdleMtmLoader::LoadMtmL
// ---------------------------------------------------------
//
void CMceIdleMtmLoader::LoadMtmL( TUid aMtmType )
    {
    MCELOGGER_ENTERFN("CMceIdleMtmLoader::LoadMtmL()");
    MCELOGGER_WRITE_FORMAT("Loading %x", aMtmType.iUid );
    // CR : 401-1806
    FeatureManager::InitializeLibL();
    TBool newPostcardVal = EFalse;
    TBool newAudioMsgVal = EFalse;
    TBool cMailVal = EFalse;    
    newAudioMsgVal = FeatureManager::FeatureSupported( KFeatureIdAudioMessaging );
    newPostcardVal = FeatureManager::FeatureSupported( KFeatureIdMmsPostcard );
    cMailVal = FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework );
    FeatureManager::UnInitializeLib();	
    if ( aMtmType == KSenduiMtmAudioMessageUid && newAudioMsgVal != iAudioMsgEnabled )
        {
        iMtmStore.ReleaseMtmUiData(aMtmType);
        }
    if ( aMtmType == KSenduiMtmPostcardUid && newPostcardVal != iPostcardEnabled)
        {
        iMtmStore.ReleaseMtmUiData(aMtmType);
        }
    CBaseMtmUiData* uiData = NULL;
    TRAPD(err, ( uiData = &(iMtmStore.MtmUiDataL(aMtmType)) ) );
    if(err != KErrNone)
        {
        return;
        }

    TInt response;
    TUid canSend;
    canSend.iUid = KUidMsvMtmQuerySupportEditor ;

    TInt canSendResponse=uiData->QueryCapability(canSend, response);
    if ( canSendResponse != KErrNone )
        {
        canSend.iUid = KUidMtmQueryCanCreateNewMsgValue;
        canSendResponse = uiData->QueryCapability( canSend, response );      
        }
        
    if(cMailVal && aMtmType == KSenduiMtmSmtpUid)
       {
       canSendResponse = KErrNotSupported;
       }

    TBool foundPopup = EFalse;
    TBool foundSubmenu = EFalse;
    const CArrayFix<CBaseMtmUiData::TMtmUiFunction>& funcs =
        uiData->MtmSpecificFunctions();
    const TInt numFuncs = funcs.Count();
    
    for (TInt funcIndex = 0; funcIndex < numFuncs; funcIndex++ )
        {
        CBaseMtmUiData::TMtmUiFunction func = funcs.At(funcIndex);

        if ( (func.iFlags&EMtudCommandTransferSend) ||
             (func.iFlags&EMtudCommandTransferReceive) )
            {
            if ( canSendResponse == KErrNone &&
                 func.iFunctionId == KMtmUiMceWriteMessageSubmenu )
                {
                iMsgTypesWriteSubmenu.AppendL( TUidNameInfo(
                    aMtmType,
                    func.iCaption ) );
                foundSubmenu = ETrue;
                }
            else if ( canSendResponse == KErrNone &&
                func.iFunctionId == KMtmUiMceWriteMessagePopup )
                {
                iMsgTypesWritePopup.AppendL( TUidNameInfo(
                    aMtmType,
                    func.iCaption ) );
                foundPopup = ETrue;
                }
            else if ( func.iFunctionId == KMtmUiMceSettings )
                {
                iMsgTypesSettings.AppendL( TUidNameInfo(
                    aMtmType,
                    func.iCaption ) );
                }
            else
                {
                iMTMFunctionsArray.AppendL(
                    TMsgFunctionInfo(
                        func.iCaption,
                        func.iPreferredHotKeyKeyCode,
                        func.iPreferredHotKeyModifiers,
                        func.iFunctionId,
                        func.iFlags,
                        aMtmType) );
                }
            }
        } // end for
        
        
    if ( canSendResponse == KErrNone )
        {
        if ( !foundPopup )
            {
            iMsgTypesWritePopup.AppendL( TUidNameInfo(
                aMtmType,
                iUiRegistry.RegisteredMtmDllInfo(aMtmType).HumanReadableName()
            ));
            }

        if ( !foundSubmenu )
            {
            //cmail update
            if(!(cMailVal && (aMtmType.iUid == KUidMsgTypeFsMtmVal))) 
                {
                iMsgTypesWriteSubmenu.AppendL( TUidNameInfo(
                        aMtmType,
                        iUiRegistry.RegisteredMtmDllInfo(aMtmType).HumanReadableName()
                ));
                }
            //cmail update
            }
        }

    MCELOGGER_LEAVEFN("CMceIdleMtmLoader::LoadMtmL()");
    }

// ---------------------------------------------------------
// CMceIdleMtmLoader::SortAndChangeSmsMmsEmailFirstL
// ---------------------------------------------------------
//
void CMceIdleMtmLoader::SortAndChangeSmsMmsEmailFirstL()
    {
    iMsgTypesSettings.Sort(ECmpFolded);
    iMsgTypesWritePopup.Sort(ECmpFolded);
    iMsgTypesWriteSubmenu.Sort(ECmpFolded);

    ChangeMsgTypeTopL( iMsgTypesSettings, KSenduiMtmPostcardUid );
    ChangeMsgTypeTopL( iMsgTypesWritePopup, KSenduiMtmPostcardUid );
    ChangeMsgTypeTopL( iMsgTypesWriteSubmenu, KSenduiMtmPostcardUid );

    ChangeMsgTypeTopL( iMsgTypesSettings, KSenduiMtmSyncMLEmailUid );
    ChangeMsgTypeTopL( iMsgTypesWritePopup, KSenduiMtmSyncMLEmailUid );
    ChangeMsgTypeTopL( iMsgTypesWriteSubmenu, KSenduiMtmSyncMLEmailUid );

    ChangeMsgTypeTopL( iMsgTypesSettings, KSenduiMtmSmtpUid );
    ChangeMsgTypeTopL( iMsgTypesWritePopup, KSenduiMtmSmtpUid );
    ChangeMsgTypeTopL( iMsgTypesWriteSubmenu, KSenduiMtmSmtpUid );

    ChangeMsgTypeTopL( iMsgTypesSettings, KSenduiMtmAudioMessageUid );
    ChangeMsgTypeTopL( iMsgTypesWritePopup, KSenduiMtmAudioMessageUid );
    ChangeMsgTypeTopL( iMsgTypesWriteSubmenu, KSenduiMtmAudioMessageUid );

    // SMS & MMS still have own settings although the editor is common.
    ChangeMsgTypeTopL( iMsgTypesSettings, KSenduiMtmMmsUid );
    ChangeMsgTypeTopL( iMsgTypesSettings, KSenduiMtmSmsUid );
    ChangeMsgTypeTopL( iMsgTypesWritePopup, KSenduiMtmUniMessageUid );
    ChangeMsgTypeTopL( iMsgTypesWriteSubmenu, KSenduiMtmUniMessageUid );
    }
// ---------------------------------------------------------
// CMceIdleMtmLoader::ChangeMsgTypeTopL
// ---------------------------------------------------------
//
void CMceIdleMtmLoader::ChangeMsgTypeTopL( CUidNameArray& aArray, TUid aMsgType ) const
    {
    TInt loop = 0;
    // first change email to the top
    for (loop=1; loop < aArray.Count(); loop++)
        {
        if (aArray[loop].iUid == aMsgType)
            {
            TUidNameInfo info = aArray[loop];
            aArray.InsertL( 0, info );
            aArray.Delete( loop+1 );
            break;
            }
        }
    }

//  End of File
