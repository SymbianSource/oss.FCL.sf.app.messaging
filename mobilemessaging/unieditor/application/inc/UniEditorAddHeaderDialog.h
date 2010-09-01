/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This is used when user wants to change the additional header fields.    
*
*/



#ifndef UNIEDITORADDHEADERDIALOG_H
#define UNIEDITORADDHEADERDIALOG_H

//  INCLUDES
#include <aknlistquerydialog.h> 

// CLASS DECLARATION

/**
* CUniEditorAddHeaderDialog is when user wants to change the setting
* of each additional header field.
* 
* @since 3.2
*/
class CUniEditorAddHeaderDialog : public CAknListQueryDialog
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        */
        CUniEditorAddHeaderDialog( CListBoxView::CSelectionIndexArray* aSelectionIndexArray );
        
        /**
        * Destructor.
        */
        virtual ~CUniEditorAddHeaderDialog();

    public: // Functions from base classes

        /**
        * Loaded version to enable Send key(EKeyYes)  and Ok key(EKeyOK) handling.
        * 
        * @see CAknListQueryDialog::OfferKeyEventL.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From CAknListQueryDialog 
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * From MEikCommandObserver
        */
        void ProcessCommandL( TInt aCommandId );

    private:
        
        /**
        * Updates MSK label
        */
        void UpdateMskL();
        
        /**
        * Performs toggle
        */
        void ToggleL();
        
        /**
        * Returns whether current item is selected
        */
        TBool IsCurrentSelected();
        
        /**
        * Returns pointer to listbox view.
        */
        CListBoxView* View();
    };

#endif      // UNIEDITORADDHEADERDIALOG_H 
            
// End of File
