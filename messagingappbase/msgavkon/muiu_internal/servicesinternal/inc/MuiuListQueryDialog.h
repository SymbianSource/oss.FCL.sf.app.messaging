/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CMuiuListQueryDialog is used when SMS viewer or MMS viewer is open
*              : and user want to call back to sender or to phone number which is
*              : highlighted in message.
*
*/



#ifndef MUIULISTQUERYDIALOG_H
#define MUIULISTQUERYDIALOG_H


//  INCLUDES
#include <aknlistquerydialog.h> 



// CLASS DECLARATION

/**
* CMuiuListQueryDialog is used when SMS viewer or MMS viewer is open
* and user want to call back to sender or to phone number which is
* highlighted in message.
*
*/
class CMuiuListQueryDialog : public CAknListQueryDialog
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        * 
        * @param aIndex After the query is dismissed, the index will hold
        *               the value of selected item.
        */
        CMuiuListQueryDialog( TInt* aIndex );
        
        /**
        * Destructor.
        */
        ~CMuiuListQueryDialog();


    public: // Functions from base classes

        /**
        * Loaded version to enable Send key(EKeyYes)  and Ok key(EKeyOK) handling.
        * 
        * @see CAknListQueryDialog::OfferKeyEventL.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    };

#endif      // MUIULISTQUERYDIALOG_H 
            
// End of File
