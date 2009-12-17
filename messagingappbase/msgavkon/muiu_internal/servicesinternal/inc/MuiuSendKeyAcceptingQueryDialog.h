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
* Description: MuiuSendKeyAcceptingQueryDialog  declaration
*
*/



#ifndef __MUIUSENDKEYACCEPTINGQUERYDIALOG_H__
#define __MUIUSENDKEYACCEPTINGQUERYDIALOG_H__


//  INCLUDES
#include <AknQueryDialog.h>
#include <ConeResLoader.h>


// CLASS DECLARATION

/**
* CMuiuSendKeyAcceptingQueryDialog
* Query  dialog when asking from user if calling to sender of the message.
*/
class CMuiuSendKeyAcceptingQueryDialog : public CAknQueryDialog
    {
    public:

        /**
         * Factory method that creates this object.
         * @param aNumber:	Number part of prompt
         * @param aName:	Name part of prompt
         * @param aTone
         * @return CMuiuSendKeyAcceptingQueryDialog pointer
         */
        static CMuiuSendKeyAcceptingQueryDialog* NewL( const TDesC& aNumber,
                                                       const TDesC& aName, 
                                                       const TTone& aTone = ENoTone );

        /**
         * Destructor.
         */
        ~CMuiuSendKeyAcceptingQueryDialog();

    protected: // from CAknQueryDialog

        /**
         * @param aKeyEvent key event
         * @param aType     event type
         * @return response
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
         * @param aButtonId
         * @return
         */
        TBool OkToExitL( TInt aButtonId );

        /**
         * @param aButtonId
         * @return
         */
        TInt MappedCommandId( TInt aButtonId );

    private:
        /**
         * Constructor.
         * @param aTone
         */
        CMuiuSendKeyAcceptingQueryDialog( const TTone aTone );

        /**
         * Symbian OS default constructor can leave.
         * @param aNumber:	Number part of prompt
         * @param aName:	Name part of prompt
         */
        void ConstructL( const TDesC& aNumber,const TDesC& aName );

    private:
        RConeResourceLoader	iResources;
    };

#endif  //__MUIUSENDKEYACCEPTINGQUERYDIALOG_H__

// End of File
