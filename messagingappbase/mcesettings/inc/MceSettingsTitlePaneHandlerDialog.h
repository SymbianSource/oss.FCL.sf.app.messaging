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
*     Handles title pane in mce's settings dialog
*
*/



#ifndef __MCESETTINGSTITLEPANEHANDLERDIALOG_H__
#define __MCESETTINGSTITLEPANEHANDLERDIALOG_H__

//  INCLUDES

#include <AknDialog.h>
#include <eiklbo.h>

// CLASS DECLARATION

/**
*/
class CMceSettingsTitlePaneHandlerDialog :
    public CAknDialog,
    public MEikListBoxObserver
    {
    public:

        /**
        * Constructor.
        */
        CMceSettingsTitlePaneHandlerDialog();

        /**
        * Destructor.
        */
        virtual ~CMceSettingsTitlePaneHandlerDialog();

    protected:

        /**
         * Stores current title pane text and replaces it with new one
         * @param aTitleText new title pane text
         */
        void StoreTitleTextAndSetNewL( TInt aResourceId );

        /**
         * Stores current title pane text and replaces it with new one
         * @param aTitleText new title pane text
         */
        void StoreTitleTextAndSetNewL( const TDesC& aTitleText );

        /**
         * Restores title pane text with the text stored in
         * StoreTitleTextAndSetNewL.
         */
        void RestoreTitleTextL();

    private:
        HBufC*      iOldTitleText;
    };

#endif // __MCESETTINGSTITLEPANEHANDLERDIALOG_H__
