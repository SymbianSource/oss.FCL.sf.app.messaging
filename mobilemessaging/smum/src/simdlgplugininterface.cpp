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


// INCLUDE FILES
#include "simdlgplugin.h"
#include "SMSU.H"
#include "simdlgplugininterface.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CSimDlgPluginInterface::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CSimDlgPluginInterface* CSimDlgPluginInterface::NewL(
        CSmsMtmUi&  aSmsMtmUi, 
        CMsvSession& aSession)
    {
    CSimDlgPluginInterface* self = CSimDlgPluginInterface::NewLC(aSmsMtmUi,aSession);
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CSimDlgPluginInterface::NewLC
// Two Phase Construction
// ----------------------------------------------------------------------------
CSimDlgPluginInterface* CSimDlgPluginInterface::NewLC(
        CSmsMtmUi&  aSmsMtmUi, 
        CMsvSession& aSession)
    {
    CSimDlgPluginInterface* self = new( ELeave ) CSimDlgPluginInterface();
    CleanupStack::PushL( self );
    self->ConstructL(aSmsMtmUi,aSession);

    return self;
    }

// ----------------------------------------------------------------------------
// CSimDlgPluginInterface::CSimDlgPluginInterface
// Constructor
// ----------------------------------------------------------------------------
CSimDlgPluginInterface::CSimDlgPluginInterface()
    {
    }

// ----------------------------------------------------------------------------
// CSimDlgPluginInterface::ConstructL
// 2nd phase constructor
// ----------------------------------------------------------------------------
void CSimDlgPluginInterface::ConstructL(
        CSmsMtmUi&  aSmsMtmUi, 
        CMsvSession& aSession)
    {
    InstantiatePluginL(aSmsMtmUi,aSession);
    }

// ----------------------------------------------------------------------------
// CSimDlgPluginInterface::~CSimDlgPluginInterface
// Constructor
// ----------------------------------------------------------------------------
CSimDlgPluginInterface::~CSimDlgPluginInterface( )
    {
    UnloadPlugIn();
    if(iSimDlgPlugin)
        {
        delete iSimDlgPlugin;
        iSimDlgPlugin = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CSimDlgPluginInterface::InstantiateAllPlugInsL
// Instantiates all plugins
// ----------------------------------------------------------------------------
void CSimDlgPluginInterface::InstantiatePluginL(
        CSmsMtmUi&  aSmsMtmUi, 
        CMsvSession& aSession)
    {
    RImplInfoPtrArray infoArray;

    // Get list of all implementations
    ListAllImplementationsL( infoArray );

    //other sim dialogs available, instantiate other than default
    for ( TInt iloop=0; iloop<infoArray.Count(); iloop++ )
        {
        // Get imp info
        CImplementationInformation& info( *infoArray[iloop] );
        // Get imp UID
        TUid impUid ( info.ImplementationUid() );

        if(infoArray.Count() > 1 && 
                !(info.DisplayName().Compare(_L("_default_"))))
            {
            continue;
            }
        else
            {
            //instantiate plugin for impUid
            iSimDlgPlugin = CSimDlgPlugin::NewL(impUid, aSmsMtmUi , aSession);
            break;
            }
        }
    infoArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSimDlgPluginInterface::UnloadPlugIn
// Unloads plugin
// -----------------------------------------------------------------------------
void CSimDlgPluginInterface::UnloadPlugIn()
    {
    REComSession::FinalClose();    
    }

// ----------------------------------------------------------------------------
// CSimDlgPluginInterface::LaunchL
// Forwards the request to the plugin instance ,
// Launch the sim dialog
// ----------------------------------------------------------------------------
void CSimDlgPluginInterface::LaunchL()
    {
    if( iSimDlgPlugin)
        {
        //launch default sim dialog
        iSimDlgPlugin->LaunchL();
        }
    }

//EOF
