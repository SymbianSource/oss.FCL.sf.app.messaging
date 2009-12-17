/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Tests MsgEditorDocument.h
*
*/


#ifndef CMSGEDITORENGINE_H_
#define CMSGEDITORENGINE_H_
#include <MSVSTD.H>
#include <MSVAPI.H>
#include <MTCLBASE.H>
#include <MTMUIBAS.H>
#include <MTUDCBAS.H>
#include <eikapp.h>                 // CEikApplication
#include <eikdoc.h>                 // CEikDocument
#include <aknappui.h>               // CAknAppUi
#include <coecntrl.h>               // CCoeControl

#include <msvstd.h>                 // for TMsvId
#include <msvapi.h>                 // MTM server API

#include <CPbkMmsAddressSelect.h>           // CPbkMmsAddressSelect
#include <RPbkViewResourceFile.h>

#include <MsgEditorAppUi.h>
#include <MsgEditorObserver.h>      // MMsgEditorObserver
#include <MsgEditorDocument.h>      // CMsgEditorDocument

#include <MsgAttachmentModelObserver.h>


#ifdef USE_SETTINGS
    #include "testsettings.h"
#endif

// ========== CONSTANTS ====================================

// Note that the UID values are recorded in three places:
// - CPP source file( as pragma data_seg )
// - header file( below, as a constant for AppDllUid() method )
// - MMP file( i.e. also in the makefile )

const TUid KUidEngineApp =
    {
    0x10005982
    };

enum TMsgTestEditorPanics
    {
    EMsgEditorModeFlagsNotSet = 0,
    EMsgInfoFieldCannotBeCreated,
    EMsgIncorrectComponentIndex,
    EMsgMailNullPointer
    };

GLDEF_C void Panic( TMsgTestEditorPanics aPanic );

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgEditorView;
class CMsvSession;
class CMmsData;
class CStifLogger;

// ========== CLASS DECLARATION ============================

class TTestObjectObserver;

/**
* CDerApplication
*
*/
class CDerApplication : public CEikApplication
    {
    private:     // From CEikApplication
        CApaDocument* CreateDocumentL();         // abstract in CEikApplication

    private:     // From CApaApplication( via CEikApplication )
        TUid AppDllUid() const;                     // abstract in CApaApplication
    };

/**
* CDerEditorDocument
*
*/
class CDerEditorDocument : public CMsgEditorDocument
    {
    public:      // New functions
        static CDerEditorDocument* NewL( CEikApplication& aApp, CStifLogger* aLog );

        CDerEditorDocument( CEikApplication& aApp, CStifLogger* aLog );

        ~CDerEditorDocument();

    public: // from CMsgEditorDocument
        TMsvId CreateNewL( TMsvId aService, TMsvId aDestination );
        TMsvId DefaultMsgFolder() const;
        TMsvId DefaultMsgService() const;
        void EntryChangedL();
        void NotifyChanges( TMsgAttachmentCommand aCommand, CMsgAttachmentInfo* aAttachmentInfo );
		CEikAppUi* CreateAppUiL();               // abstract in CEikDocument
		
    protected:
        void ConstructL();

    private:     // From CEikDocument

    private:
        TMsvId CreateNewL( TMsvId aService, TMsvId aDestination, TInt aTypeList );

    private:

        // from CMsgEditorDocument
        CMsgAttachmentModel* CreateNewAttachmentModelL( TBool aReadOnly );

    private:
        // private data members, the model

        TTestObjectObserver* iObjectObserver;
        CMmsData* iMmsData;
        CStifLogger* iLog;
    };

/**
*
*/
class TTestObjectObserver : public MMsgAttachmentModelObserver
    {
    public:

        TTestObjectObserver( CDerEditorDocument& aDocument );

        // from MMsgAttachmentModelObserver
        void NotifyChanges( TMsgAttachmentCommand aCommand, CMsgAttachmentInfo* aAttachmentInfo );

        RFile GetAttachmentFileL( TMsvAttachmentId aId );

    private:

        CDerEditorDocument& iDocument;

    };

/**
* CMsgDerAppUi
*
*/
class CMsgDerAppUi : public CMsgEditorAppUi
    {
    public:  // New functions
        void ConstructL();           // Two-phase construction

        CMsgDerAppUi( CStifLogger* aLog );

        ~CMsgDerAppUi();

    public:  // from MMsgEditorObserver

        CMsgBaseControl* CreateCustomControlL( TInt /*aControlType*/ );

        void EditorObserver( TMsgEditorObserverFunc aFunc, TAny* aArg1, TAny* aArg2, TAny* aArg3 );

        //   void HandleFocusChangedL()     {;    }
        void CheckProtcdCMsgEditorAppUiFunsL(void);

    protected:

        // from CCoeAppUi
        void HandleForegroundEventL( TBool aForeground );

    private:
        enum TRecipient
            {
            EFirst,
            ENext,
            ECurrent
            };

    private:     // from CEikAppUi
        void HandleCommandL( TInt aCommand );
        void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );

    private:    // from CMsgEditorAppUi
        void LaunchViewL();
        void DoMsgSaveExitL();

    private:
        CDerEditorDocument* Document() const;

    private:     // Data
        CStifLogger*        iLog;

    };




class CStifLogger;
/**
 * CMsgEditorEngine
 */

class CMsgEditorEngine : public CBase
{
public:
    CMsgEditorEngine( CStifLogger* aLog );
    static CMsgEditorEngine* NewL( CStifLogger* aLog);
    virtual ~CMsgEditorEngine();
    CDerEditorDocument* GetDocument();
    TInt Init();
    //Set given entry to be current context and notifies after change.
    TBool SetnGetMTMEntryL();
    const TMsvEntry& Entry() const;
    CMsvSession& Session() const;
    TBool CurrentEntry();
    TBool Mtm();
    TBool MtmUi();
    CBaseMtmUiData& MtmUiData() const;
    CBaseMtmUi& MtmUiL() const;
    CBaseMtmUiData& MtmUiDataL() const;
    void PrepareMtmL(const TUid aMtmType);
    void RestoreL();
    CMsvEntry* GetEntry();
    TBool HasModel();
    TBool IsLaunched();
    TBool MediaAvailable();
    void SetEditorModelObserver(MMsgEditorModelObserver* aObs);    
    
private:

    void ConstructL();
    
private:
    //data
    
    CDerApplication*    iApp;
    
    CDerEditorDocument* iDocument;
    
    CMsvEntry*          iCmEntry;
    
    TBool               iInitialized;
    
    CStifLogger*        iLog;
    
    TMsvEntry           iTmEntry;    
    
};

#endif /*CMSGEDITORENGINE_H_*/

// End of File
