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
*    CMsgErrorDisconnectDlg declaration file 
*
*/



#ifndef MSGERRORDISCONNECTDLG_H
#define MSGERRORDISCONNECTDLG_H

//  INCLUDES
#include <e32base.h>
#include <DisconnectDlgClient.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* CMsgErrorDisconnectDlg
*
* @lib msgerrorwatcher.dll
* @since 2.6
*/
class CMsgErrorDisconnectDlg : public CActive
    {
    public:     // construct / destruct

        /**
        * Two-phased constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        static CMsgErrorDisconnectDlg* NewL();

        /**
        * Destructor.
        */
        virtual ~CMsgErrorDisconnectDlg();

        /**
        * For starting the active object.
        */
        void Start();

    private:
    
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        CMsgErrorDisconnectDlg();

        /**
        * From CActive
        */
        void RunL() ;

        /**
        * From CActive
        */
        void DoCancel();

    private:    // data

        RDisconnectDlgServer iDialogServer;
    };

#endif

// End of File
