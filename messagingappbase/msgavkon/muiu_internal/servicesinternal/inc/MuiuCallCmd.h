/*
* Copyright (c) 2002 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*      Call command object.
*
*/



#ifndef __MuiuCallCmd_H__
#define __MuiuCallCmd_H__

// INCLUDES
#include <e32base.h>                             
#include <aiwdialdataext.h>      // KMaximumNameLength


// CLASS DECLARATION

/**
* Start dialing and show calling dialog.
* 
*/
class CMuiuCallCmd : public CBase
    {
    public: // Interface
        /**
        * Factory method that creates this object.
        * @return CMuiuCallCmd pointer
        */
        static CMuiuCallCmd* NewL();

        /**
        * Destructor.
        */
        ~CMuiuCallCmd();

    public:
        /**
        * Start dialing and show calling dialog
        * @param aNumber:   Number part of prompt
        * @param aName:	    Name part of prompt
        * @param aCallTypeVoIP: If this parameter is true the call type is VoIP 
        * otherwise call type is normal cellular call.
        */
        void ExecuteLD( const TDesC& aNumber, const TDesC& aName, TBool aCallTypeVoIP = EFalse );
    private:
        /**
        * Constructor.
        */
        CMuiuCallCmd ();

        /**
        * Symbian OS default constructor can leave.
        */
        void ConstructL();
    private:    // Data
        // Own: telephone number and namebuffer(member variables to save stack space)
        TBuf< AIWDialDataExt::KMaximumPhoneNumberLength > iTelNumber;
        TBuf< AIWDialDataExt::KMaximumNameLength > iNameBuffer;
    };
#endif // __MuiuCallCmd_H__

// End of File
