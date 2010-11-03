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
* Description:  IMSWCheckBoxPage.cpp
*
*/


#include <ImumUtils.rsg>

#include "IMSWCheckBoxPage.h"
#include "ImumUtilsLogging.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::CIMSSettingsWizardEditTextPage
// ----------------------------------------------------------------------------
CIMSSettingsWizardCheckBoxPage::CIMSSettingsWizardCheckBoxPage(
    TInt aResourceID, CSelectionItemList* aItemArray, TBool aMustFillOne ):
    CAknCheckBoxSettingPage( aResourceID, aItemArray )
    {
    IMUM_CONTEXT( CIMSSettingsWizardCheckBoxPage::CIMSSettingsWizardCheckBoxPage, 0, KLogUi );

    iItemArray = aItemArray;
    iMustFillOne = aMustFillOne;
    iLeftSkResource = R_IMAS_SETTINGS_SK_BUTTON_OK;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::~CIMSSettingsWizardEditTextPage
// ----------------------------------------------------------------------------
CIMSSettingsWizardCheckBoxPage::~CIMSSettingsWizardCheckBoxPage()
    {
    IMUM_CONTEXT( CIMSSettingsWizardCheckBoxPage::~CIMSSettingsWizardCheckBoxPage, 0, KLogUi );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::UpdateSettingL
// ----------------------------------------------------------------------------
void CIMSSettingsWizardCheckBoxPage::UpdateSettingL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardCheckBoxPage::UpdateSettingL, 0, KLogUi );

    CAknCheckBoxSettingPage::UpdateSettingL();

    if ( iMustFillOne )
        {
        // Set left softkey empty if all boxes are unchecked
        TInt resourceId = R_IMAS_SETTINGS_SK_BUTTON_EMPTY;
        TInt nItems = iItemArray->Count();
        for( TInt i = 0; i < nItems; i++ )
            {
            if ( ( *iItemArray )[i]->SelectionStatus() )
                {
                resourceId = R_IMAS_SETTINGS_SK_BUTTON_OK;
                }
            }

        if ( resourceId != iLeftSkResource )
            {
            CEikButtonGroupContainer *bgc;
            MopGetObject(bgc);
            if (bgc)
                {
                CEikCba* cba(NULL);
		        cba = ( static_cast<CEikCba*>( bgc->ButtonGroup() ) ); // downcast from MEikButtonGroup
		        const TInt KSkPosition = 0; // left SK position
		        iLeftSkResource = resourceId;
		        cba->SetCommandL( KSkPosition, iLeftSkResource );
		        cba->DrawNow();
                }
            }
        }
    }

//  End of File
