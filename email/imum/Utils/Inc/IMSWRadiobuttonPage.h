/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IMSWRadioButtonPage.h
*
*/



#ifndef IMSWRADIOBUTTONPAGE_H
#define IMSWRADIOBUTTONPAGE_H

// INCLUDES
#include <e32base.h>
#include <aknsettingpage.h>
#include <aknradiobuttonsettingpage.h>
#include <aknnavilabel.h>


/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class CIMSSettingsWizardRadioButtonPage : public CAknRadioButtonSettingPage
    {
    public: // Constructors and destructor

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CIMSSettingsWizardRadioButtonPage();

        /**
        *
        * @since S60 3.0
        */
        static CIMSSettingsWizardRadioButtonPage* NewL(
            CIMSSettingsWizard& aWizard,
            TInt aResourceId,
            TInt& aCurrentIndex,
            const MDesCArray* aItemArray,
            TDesC& aPageText );

        /**
        *
        * @since S60 3.0
        */
        static CIMSSettingsWizardRadioButtonPage* NewLC(
            CIMSSettingsWizard& aWizard,
            TInt aResourceId,
            TInt& aCurrentIndex,
            const MDesCArray* aItemArray,
            TDesC& aPageText );

    public: // New functions

        /**
        *
        * @since S60 3.0
        */
        void KeyPressedNextL();

        /**
        *
        * @since S60 3.0
        */
        void KeyPressedBackL();

        /**
        *
        * @since S60 3.0
        */
        void KeyPressedSelectL();

        /**
        *
        * @since S60 3.0
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
        							 TEventCode aType );

        /**
        *
        * @since S60 5.0
        */
		void HandleListBoxEventL( CEikListBox* aListBox,
        						  MEikListBoxObserver::TListBoxEvent aEventType );

    protected: // Constructors

        /**
        *
        * @since S60 3.0
        */
        CIMSSettingsWizardRadioButtonPage(
            CIMSSettingsWizard& aWizard,
            TInt aResourceId,
            TInt& aCurrentIndex,
            const MDesCArray* aItemArray,
            TDesC& aPageText );

        /**
        *
        * @since S60 3.0
        */
        void ConstructL();

    protected:  // Functions from base classes

        virtual void ProcessCommandL( TInt aCommandId );
        virtual TBool PostDisplayCheckL();


#ifdef RD_SCALABLE_UI_V2
        /**
        * Function for handling pointer (stylus) events
        * @since S60 v3.2
        * @param aPointerEvent, type of pointer actions
        */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

#endif  //RD_SCALABLE_UI_V2

    private:  // New virtual functions
    private:  // New functions

        /**
        *
        * @since S60 3.0
        */
        void UpdateNaviPaneL();

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        CIMSSettingsWizard&             iWizard;
        //
        TDesC&                          iPageText;
        //
        CAknNavigationControlContainer* iNaviPane;
        //
        CAknNavigationDecorator*        iNaviDecorator;
    };

#endif      // IMSWRADIOBUTTONPAGE_H

// End of File
