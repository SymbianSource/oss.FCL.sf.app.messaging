/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UniSmsUtils  declaration
*
*/



#ifndef __UNIMSGSMSEDITORUTILS_H
#define __UNIMSGSMSEDITORUTILS_H

// INCLUDES
#include <AknQueryDialog.h>
// CLASS DECLARATION

/**
*  CUniSmsEMultilineQueryDialog
*  CUniSmsEMultilineQueryDialog provides the possibility to launch 
*  multi-line queries with a focus on the 2nd line.
*/
class CUniSmsEMultilineQueryDialog : public CAknMultiLineDataQueryDialog
    {
    public: // Constructors

       /**
        * 2-phase constructor.
        */
        static CUniSmsEMultilineQueryDialog* NewL( 
        	TDes&  aText1, 
        	TDes&  aText2,
        	TBool aEmailDlg = EFalse, 
        	TTone aTone = ENoTone);

        /**
        * Execute dialog 
        */
        TInt ExecuteLD( TInt aResourceId ) ;
        
    protected: // Functions from base classes

        /**
        * From CAknMultiLineDataQueryDialog 
        */
        virtual void SetInitialCurrentLine();

        /**
        * From CEikDialog 
        */
        TBool OkToExitL(TInt aButtonId);

    private: // new functions
 
        /**
        * A method to change focus to line two
        */
        void TryToChangeLineL();

    private:
        /**
        * C++ constructor.
        */
        CUniSmsEMultilineQueryDialog( TBool aEmailDlg, TTone aTone );
        
        /**
        * Destructor.
        */
        ~CUniSmsEMultilineQueryDialog();
        
    private: // data members
        TBool 	iEmailDlg;
        TInt    iResourceFile;      // Offset of resource file
    };



#endif // __UNIMSGSMSEDITORUTILS_H
