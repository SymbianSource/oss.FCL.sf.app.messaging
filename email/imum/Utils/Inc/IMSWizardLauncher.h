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
* Description:  Class declaration file for CExtWizardClass
*
*/


#ifndef __IMSWIZARDLAUNCHER_H__
#define __IMSWIZARDLAUNCHER_H__

#include "IMSSettingsWizard.h"

// FORWARD DECLARATIONS
class CIMSSettingsWizard;

// CLASS DECLARATION

/**
*  Launches wizard when info screen is not needed in beginning
*
*  @lib IMUMUTILS.DLL
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CIMSWizardLauncher ): public CActive
	{
public:

    /**
     * Two-phased contructor
     *
     * @since S60 3.2
     * @param aWizard for a reference which to call to launch email wizard
     * @return new object
     */
    static CIMSWizardLauncher* NewL(
		        CIMSSettingsWizard& aWizard,
		        CIMSSettingsUi& aSettingsUi );
    
	virtual ~CIMSWizardLauncher();
    
    /**
     * Sets the active object active and finalizes it right
     * away to launch
     *
     * @since S60 3.2
     */
    void StartLauncher();
    
protected:

private:

    /**
     * Creates new object
     *
     * @since S60 3.2
     * @param aWizard for a reference which to call to launch email wizard
     * @param aPriority to send CActive class
     */
	CIMSWizardLauncher( 
				CIMSSettingsWizard& aWizard,
				CIMSSettingsUi& aSettingsUi,
				TInt aPriority = EPriorityStandard );

    /**
    * From CActive
    *
    * @since S60 3.2
    */
    virtual void DoCancel();


    /**
    * From CActive
    * Starts email wizard
    *
    * @since S60 3.2
    */
    virtual void RunL();

private: //data members
    CIMSSettingsWizard&     iWizard;
    CIMSSettingsUi& 		iSettingsUi;
	};


#endif      // IMUMMAILSETTINGSWIZARD_H