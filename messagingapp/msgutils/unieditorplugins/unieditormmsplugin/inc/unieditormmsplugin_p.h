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
 * Description:
 *
 */

#ifndef __UNIEDITORMMSPLUGINPRIVATE_H
#define __UNIEDITORMMSPLUGINPRIVATE_H

#include <msvapi.h>
#include <cmsvrecipientlist.h>
#include "convergedmessage.h"
#include "convergedmessageid.h"

// DATA TYPES

// FORWARD DECLARATIONS

class CClientMtmRegistry;
class CMmsClientMtm;
class CMmsHeaders;
class CMmsSettings;
class CCommsDatabase;
class TUniSendingSettings;
class UniDataModelLoader;
class UniDataModelPluginInterface;
class SessionEventHandler;

// CLASS DECLARATION

class CUniEditorMmsPluginPrivate : public CBase
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CUniEditorMmsPluginPrivate* NewL();

    /**
     * Destructor.
     */
    virtual ~CUniEditorMmsPluginPrivate();

public: 

    /**
     * Converts message from message store into ConvergedMessage
     * @param TMsvId id
     * @return ConvergedMessage object is returned in successfull cases and 
     *         NULL is returned in failure cases. The ownership of the object
     *         is transferred to the caller 
     */
    ConvergedMessage* convertFromL( TMsvId aId );

    /**
     * delete entry from drafts folder
     * @param aId TMsvId of the message entry to be deleted
     */
    void deleteDraftsEntryL( TMsvId aId );

    /**
     * Converts ConvergedMessage message into a message in store
     * @param ConvergedMessage obj
     * @return TMsvId id
     */             
    TMsvId convertToL( ConvergedMessage *aMessage );
    
    /**
     * Implementation for convertToL which converts ConvergedMessage message 
     * into a message in store
     * @param ConvergedMessage obj
     * @param TMsvEntry
     */             
    void DoConvertToL(ConvergedMessage *aMessage,TMsvEntry& entry);

    /**
     * Sends the message
     * @param TMsvId id
     */                 
    void sendL( TMsvId aId );

    /**
     * Validates the service for send
     * @param aEmailOverSms
     * @return true if the service is valid for send
     */                    
    TBool validateServiceL( TBool aEmailOverSms = EFalse );

    /**
     * isServiceValidL
     * @return TBool
     */              
    TBool isServiceValidL();

private: // Constructors

    /**
     * C++ default constructor.
     */
    CUniEditorMmsPluginPrivate();

    /**
     * Returns pointer to iMmsMtm member variable  
     */
    CMmsClientMtm* MmsMtmL();

    /**
     * Generates details for TMsvEntry object
     */
    void MakeDetailsL( TDes& aDetails );

    /**
     * Populates address feilds in message from ConvergedMessage
     */
    void addRecipientsL(ConvergedMessage *message);

    /**
     * Populates address feilds in message from ConvergedMessage
     */
    void addRecipientsL(const ConvergedMessageAddressList &array, 
        TMsvRecipientType recpType);

    /**
     * Populates converged message from the drafts entry
     */
    void convertFromDraftsL(ConvergedMessage& aMessage );

    /**
     * Populates address feilds in drafts entry to converged message
     */
    void populateRecipientsL(
        ConvergedMessage &aMessage);

private:  // Data

    CMsvSession* iSession;        
    CClientMtmRegistry* iMtmRegistry;
    CMmsClientMtm* iMmsMtm;
    CMmsHeaders* iMmsHeader;

    // owned
    UniDataModelLoader* iDataModelPluginLoader;        
    //Not owned
    UniDataModelPluginInterface* iMmsDataPlugin;
    SessionEventHandler *iSessionHandler;
};

#endif   // __UNIEDITORMMSPLUGINPRIVATE_H

// End of File
