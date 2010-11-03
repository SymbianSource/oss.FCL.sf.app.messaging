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
*    Dialog for selecting message folder
*
*/



#ifndef MSGFOLDERSELECTIONDIALOG_H__
#define MSGFOLDERSELECTIONDIALOG_H__

//  INCLUDES
#include <ConeResLoader.h>


// FORWARD DECLARATIONS
class CMsgFolderSelectionListArray;


// CLASS DECLARATION

/**
* Dialog for selecting message folder
*/
class CMsgFolderSelectionDialog : public CBase
    {
    public:
        /**
        * Destructor.
        */
        virtual ~CMsgFolderSelectionDialog();

    public:  // New functions
        /**
        * Lauches dialog for user to select destination folder.
        * @param aSelectedFolder will include id of the folder user selected when dialog closed.
        * Fill id of the current folder so that it will not be shown.
        * @return ETrue if Ok, EFalse if Back pressed.
        */
        IMPORT_C static TBool SelectFolderL( TMsvId& aSelectedFolder, TDesC& aTitle );

    private:

        /**
        * C++ default constructor.
        */
        CMsgFolderSelectionDialog( TMsvId& aSelectedFolder, TDesC& aTitle );

        /**
        * Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Two-phased constructor.
        */
        static CMsgFolderSelectionDialog* NewL( TMsvId& aSelectedFolder, TDesC& aTitle );

    private:
        /**
        * Lauches popuplist for user to select destination folder.
        * @return ETrue if Ok, EFalse if Back pressed.
        */
        TBool ExecutePopupListL();

        /**
        * By default, prohibit copy constructor.
        */
        CMsgFolderSelectionDialog( const CMsgFolderSelectionDialog& );

        /**
        * Prohibit assigment operator.
        */
        CMsgFolderSelectionDialog& operator= ( const CMsgFolderSelectionDialog& );

    private:    // Data
        CMsgFolderSelectionListArray*   iFolderArray;
        CArrayPtrFlat<CGulIcon>*        iIcons;
        TMsvId&                         iSelectedFolder;
        TDesC&                          iTitle;
        RConeResourceLoader             iResources;

    };

#endif

// End of File
