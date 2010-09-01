/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*     Sim Dialog plugin interface
*
*/

#ifndef __SIMDLG_PLUGIN_INTERFACE_H__
#define __SIMDLG_PLUGIN_INTERFACE_H__

//INCLUDES
#include <simdlgplugin.h>

// CLASS DECLARATION
/**
 * Class is used to instantiate the required plugin.
 * Acts as interface to all Plugins.
 */

class CSimDlgPluginInterface : public CBase 
    {
public:

    /** 
     * NewL
     * Symbian OS 2 phased constructor
     * 
     * @param aSmsMtmUi CSmsMtmUi
     * @param aSession CMsvSession 
     *
     * @return	A pointer to the created instance of CSimDlgPluginInterface.
     */
    static CSimDlgPluginInterface* NewL(CSmsMtmUi&  aSmsMtmUi, CMsvSession& aSession);

    /** 
     * NewLC
     * Symbian OS 2 phased constructor
     * 
     * @param aSmsMtmUi CSmsMtmUi
     * @param aSession CMsvSession 
     *
     * @return	A pointer to the created instance of CSimDlgPluginInterface.
     */
    static CSimDlgPluginInterface* NewLC(CSmsMtmUi&  aSmsMtmUi, CMsvSession& aSession);

    /**
     * Destructor.
     */      
    virtual ~CSimDlgPluginInterface();

    /**
     * UnloadPlugIn     
     */
    void UnloadPlugIn(); 

    /**
     * LaunchL
     * Forwards the request to the plugin instance ,
     * Launches the sim dialog
     */
    void  LaunchL();

private: 

    /** 
     * Performs the first phase of two phase construction.
     */
    CSimDlgPluginInterface();  

    /** 
     * Symbian OS 2 phased constructor.
     * 
     * @param aSmsMtmUi CSmsMtmUi
     * @param aSession CMsvSession 
     *
     */
    void ConstructL(CSmsMtmUi&  aSmsMtmUi, CMsvSession& aSession);

    /**
     * InstantiatePluginL
     * Instantiates the required plug-ins known by the ecom framework.
     *
     * @param aSmsMtmUi CSmsMtmUi
     * @param aSession CMsvSession 
     */
    void InstantiatePluginL(CSmsMtmUi&  aSmsMtmUi, CMsvSession& aSession);

    /**
     * ListAllImplementationsL
     * Lists all implementations which satisfy this ecom interface
     * 
     * @param aImplInfoArray On return, contains the list of available implementations
     */   
    inline void ListAllImplementationsL( 
            RImplInfoPtrArray& aImplInfoArray )
        {
        REComSession::ListImplementationsL( 
                KSimDialogPluginInterfaceUid, 
                aImplInfoArray );
        }


private: 

    /**
     * iDtor_ID_Key
     * Unique Key, handle by ECom plugin
     */
    TUid iDtor_ID_Key;

    /**
     * iSimDlgPlugin
     * plugin instance
     */
    CSimDlgPlugin* iSimDlgPlugin; 

    };

#endif // __SIMDLG_PLUGIN_INTERFACE_H__

//End of File
