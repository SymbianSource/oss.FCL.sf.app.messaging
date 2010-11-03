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
 * Description: default sim-dialog plugin
 *
 */

#ifndef _DEFAULT_SIM_DIALOG_H_
#define _DEFAULT_SIM_DIALOG_H_

// INCLUDE FILES

// SYSTEM INCLUDES

// USER INCLUDES
#include "simdlgplugin.h"


// FORWARD DECLARATION
class CSmsMtmUi;
class CMsvSession;

/**
 *  CDefaultSimDlg
 *  default sim dialog implementation
 */
class CDefaultSimDlg : public CSimDlgPlugin
    {
public: 

    /**
     * Two phase construction
     * @param aParams TSimDlgParams
     */
    static CDefaultSimDlg* NewL( TSimDlgParams* aParams);

    /**
     * Two phase construction
     * @param aParams TSimDlgParams
     */
    static CDefaultSimDlg* NewLC( TSimDlgParams* aParams);

    /**
     * Destructor
     */
    virtual ~CDefaultSimDlg();


public: // From base class CSimDlgPlugin
    /**
     *  LaunchL
     * Launch the sim dialog
     */        
    void LaunchL();


private:

    /**
     * Constructor
     */
    CDefaultSimDlg( TSimDlgParams* aParams);

    /**
     * 2nd phase construtor
     */
    void ConstructL();
    
private: //Data

    CSmsMtmUi&         iSmsMtmUi;
    CMsvSession&        iSession;       
    };

#endif // _DEFAULT_SIM_DIALOG_H_
