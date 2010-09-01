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
* Description: 
*     CImumUsernameAndPasswordDialog declaration file.
*
*
*/


#ifndef IMUMUSERNAMEANDPASSWORDDIALOG_H
#define IMUMUSERNAMEANDPASSWORDDIALOG_H

//  INCLUDES
#include <AknQueryDialog.h>


// CLASS DECLARATION

/**
*  CImumUsernameAndPasswordDialog
*  Need to override this so that if username is filled then cursor should be
*  on password field when opening the dialog
*/
class CImumUsernameAndPasswordDialog : public CAknMultiLineDataQueryDialog
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CImumUsernameAndPasswordDialog* NewL(
            TDes& aUsername, TDes& aPassword );

        /**
        * Destructor
        */
        ~CImumUsernameAndPasswordDialog();

    protected:
        /**
        * C++ constructor.
        */
        CImumUsernameAndPasswordDialog( TTone aTone = ENoTone );


    protected:

        /**
        * SetInitialCurrentLine function, CEikDialog
        */
        virtual void SetInitialCurrentLine();

    private:
        // none
    };

#endif      // __IMUMUSERNAMEANDPASSWORDDIALOG_H

// End of File
