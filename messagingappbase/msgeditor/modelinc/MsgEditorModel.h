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
* Description:  MsgEditorModel  declaration
*
*/



#ifndef INC_MSGEDITORMODEL_H
#define INC_MSGEDITORMODEL_H

// ========== INCLUDE FILES ================================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CBaseMtm;
class CClientMtmRegistry;
class CMtmUiRegistry;
class CMtmUiDataRegistry;
class CBaseMtmUi;
class CBaseMtmUiData;
class MMsgEditorModelObserver;

// ========== CLASS DECLARATION ============================

/**
 * Interface for message editor/viewer model module.
 *
 */
class CMsgEditorModel : public CBase, public MMsvSessionObserver
{

public:      // new functions

    /**
     * Constructor.
     */
    CMsgEditorModel();

    /**
     * Destructor.
     */
    ~CMsgEditorModel();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Set up current message entry, creates a real connection between current
     * editor/viewer session and an message entry to be displayed or edited.
     * @param aEntryId
     */
    void SetEntryL(TMsvId aEntryId);

    /**
     * Loads client and ui mtm's according to given message type.
     */
    void PrepareMtmL(const TUid aMtmType);

    /**
     * Return reference to current session.
     * @return
     */
    inline CMsvSession& Session() const;

    /**
     * Return reference to mtm.
     * @return
     */
    inline CBaseMtm& Mtm() const;

    /**
     * Return reference to mtm ui.
     * @return
     */
    CBaseMtmUi& MtmUiL();

    /**
     * Return reference to mtm ui data.
     * @return
     */
    CBaseMtmUiData& MtmUiDataL();
    
    /**
     * Return current message's entry.
     * @return
     */
    inline const TMsvEntry& Entry() const;

    /**
     * Set the editor model observer.
     * @param aObserver
     */
    inline void SetModelObserver(MMsgEditorModelObserver* aObserver);

    /**
     * Returns the current (the most recently known) status of the media.
     * @return
     */
    inline TBool MediaAvailable() const;

    /**
     * Allows changing the media availability flag stored in model.
     * @param aMediaAvailable
     */
    inline void SetMediaAvailable(TBool aMediaAvailable);

    /**
     *
     */
    void Wait();

public:      // new public data member

    TInt iLaunchWait;

private:     // from MMsvSessionObserver

    /**
     * Handles session event observer, and call event handling functions in
     * observer. Note that if additional session event handlers are defined in
     * the session, they are called before this function (as this is the
     * main session observer).

     * Note that the same event (especially EMsvEntriesChanged) may happen
     * several times successively when the message is saved. This is probably
     * because the editor may modify the same entry several times when saving
     * message.
     *
     * @param aEvent
     * @param aArg1
     * @param aArg2
     * @param aArg3
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

    /**
     *
     */
    void CompleteConstructL();

private:     // data

    CMsvSession*             iSession;
    CBaseMtm*                iMtm;
    CClientMtmRegistry*      iMtmReg;
    CMtmUiRegistry*          iMtmUiReg;
    CMtmUiDataRegistry*      iMtmUiDataReg;
    CBaseMtmUi*              iMtmUi;
    MMsgEditorModelObserver* iModelObserver;
    TBool                    iMediaAvailable;
    TBool                    iWait;
    CBaseMtmUiData*          iMtmUiData;

};

#include "MsgEditorModel.inl"

#endif

// End of File
