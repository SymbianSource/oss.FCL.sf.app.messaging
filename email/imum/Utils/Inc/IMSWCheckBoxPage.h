/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IMSWCheckBoxPage.h
*
*/



#ifndef IMSWCHECKBOXPAGE_H
#define IMSWCHECKBOXPAGE_H

#include <e32base.h>
#include <akncheckboxsettingpage.h>

/**
*  Check box settings dialog.
*  This class adds into CAknCheckBoxSettingPage an enforcement where
*  at least one box has to be checked when the dialog is exited by OK
*  softkey. Left softkey is set empty if all boxes are unchecked.
*
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CIMSSettingsWizardCheckBoxPage ): public CAknCheckBoxSettingPage
    {
    public: // Constructors and destructor

        CIMSSettingsWizardCheckBoxPage(TInt aResourceID,
            CSelectionItemList* aItemArray, TBool aMustFillOne );

        virtual ~CIMSSettingsWizardCheckBoxPage();


    protected:  // Functions from base classes

        // From CAknListBoxSettingPage
        virtual void UpdateSettingL();

    private:  // Functions from base classes

        // Make unavailable to set itemArray after construction as we have
        // a copy of item array pointer
        void SetSelectionItemArrayL( CSelectionItemList* aItemArray );

    private: // Data

        // This is private in CAknCheckBoxSettingPage. Has to keep own copy.
	    CSelectionItemList* iItemArray; // not owned

        TBool iMustFillOne; // true if at least one box must be filled
        TInt iLeftSkResource; // current left softkey resource ID
    };

#endif      // IMSWTEXTEDITORPAGE_H

// End of File
