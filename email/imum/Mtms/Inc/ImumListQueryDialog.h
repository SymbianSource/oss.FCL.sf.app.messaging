/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*       Overwrite AknListQueryDialog to provide Imum a working touch dialog.
*
*/

#ifndef IMUMLISTQUERYDIALOG_H
#define IMUMLISTQUERYDIALOG_H

#include <aknlistquerydialog.h>


NONSHARABLE_CLASS( CImumListQueryDialog ) : public CAknListQueryDialog
    {
    public:
        /**
         * C++ default constructor.
         *
         * @param aIndex After the query is dismissed, the index will
         *        hold the value of selected item.
         */
		CImumListQueryDialog( TInt* aIndex );


        /**
        * Destructor.
        */
        ~CImumListQueryDialog();


        /**
         * From @c MEikListBoxObserver.
         * Overwrite CAknListQueryDialog.
         *
         * Handles events from listbox.
         *
         * @param aListBox The listbox which caused the event.
         * @param aEventType Type of the event.
         */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                          TListBoxEvent aEventType );

    private:
        /**
         * Overwrite CAknListQueryDialog.
         *
         * Closes the dialog.
         * Gets called by iIdle.
         *
         * @param aObj Target dialog to be closed.
         * @return @c EFalse always.
         */
        static TInt ClosePopup( TAny *aObj );

        /**
         * Overwrite CAknListQueryDialog.
         *
         * Closes the dialog and accepts selected item.
         * Gets called by iIdle.
         *
         * @param aObj Target dialog to be closed.
         * @return @c EFalse always.
         */
        static TInt ClosePopupAcceptingChanges( TAny *aObj );

    private:
        // Will hold the index of the selected item
        TInt *iIndex;
        // Was the enter key pressed
	    TBool iEnterKeyPressed;
        // Idle object to close the query if it there's no items to show
	    CIdle *iIdle;
	    // Holds previous focus index
        TInt iPreviousIndex;
	};

#endif	// IMUMLISTQUERYDIALOG_H