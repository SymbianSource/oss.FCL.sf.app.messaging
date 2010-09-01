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
 * Description: sim-dialog plugin interface
 *
 */

#ifndef __SIMDLG_PLUGIN_H__
#define __SIMDLG_PLUGIN_H__

//  INCLUDES
#include <ecom/ecom.h>
// CONSTANTS
const TUid KSimDialogPluginInterfaceUid = { 0x20029F1E };

// FORWARD DECLARATIONS
class CSmsMtmUi;
class CMsvSession;

/**
* params for Sim Dialog construction
*/

struct TSimDlgParams
{
	TSimDlgParams(CSmsMtmUi&  aSmsMtmUi, CMsvSession& aSession)
	:iSmsMtmUi(aSmsMtmUi),iSession(aSession) { }
	
	//Data
	CSmsMtmUi&  iSmsMtmUi;
	CMsvSession& iSession;
};

/**
 * CSimDlgPlugin
 * Sim Dialog plug-ins (Ecom) interface definition.
 *
 */

class CSimDlgPlugin: public CBase
    {

public: 

    /**
     * NewL.
     * Ecom interface static factory method implementation.
     * 
     * @param aImpUid Ecom's implementation uid
     * @param aSmsMtmUi CSmsMtmUi
     * @param aSession CMsvSession 
     
     */
    static inline CSimDlgPlugin* NewL(             
            TUid aImpUid, 
            CSmsMtmUi&  aSmsMtmUi, 
            CMsvSession& aSession);

    /**
     * NewLC.
     * Ecom interface static factory method implementation.
     * 
     * @param aImpUid Ecom's implementation uid
     * @param aSmsMtmUi CSmsMtmUi
     * @param aSession CMsvSession 
     
     */
    static inline CSimDlgPlugin* NewLC( 
            TUid aImpUid, 
            CSmsMtmUi&  aSmsMtmUi, 
            CMsvSession& aSession);
    
    /**
     * Destructor
     */
    virtual ~CSimDlgPlugin();

public: 
   
    /**
     * LaunchL.
     * Launch Sim Dialog.
     * 
     */
    virtual void LaunchL()= 0;

private:    
   
    /**
     * iDtor_ID_Key
     * UID required on cleanup.
     */
    TUid iDtor_ID_Key;

    };

#include "simdlgplugin.inl"

#endif		// __SIMDLG_PLUGIN_H__

// End of File

