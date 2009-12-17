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
* Description:  IMSWTextEditorPage.h
*
*/



#ifndef IMSWTEXTEDITORPAGE_H
#define IMSWTEXTEDITORPAGE_H

// INCLUDES
#include <e32base.h>
#include <aknsettingpage.h>
#include <akntextsettingpage.h>
#include <aknnavilabel.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class CIMSSettingsWizardEditTextPage :
    public CAknTextSettingPage
    {
    public: // Constructors and destructor

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CIMSSettingsWizardEditTextPage();

        /**
        *
        * @since S60 3.0
        */
        static CIMSSettingsWizardEditTextPage* NewL(
            CIMSSettingsWizard& aWizard,
            TInt aResourceId,
            TDes& aText,
            TDesC& aPageText,
            TInt aTextSettingPageFlags = 0 );

        /**
        *
        * @since S60 3.0
        */
        static CIMSSettingsWizardEditTextPage* NewLC(
            CIMSSettingsWizard& aWizard,
            TInt aResourceId,
            TDes& aText,
            TDesC& aPageText,
            TInt aTextSettingPageFlags = 0 );

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
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& /*aKeyEvent*/,
            TEventCode /*aType*/ );


    public: // Functions from base classes

    protected: // Constructors

        /**
        *
        * @since S60 3.0
        */
        CIMSSettingsWizardEditTextPage(
            CIMSSettingsWizard& aWizard,
            TInt aResourceId,
            TDes& aText,
            TDesC& aPageText,
            TInt aTextSettingPageFlags = 0 );

        /**
        *
        * @since S60 3.0
        */
        void ConstructL();

    protected:  // New virtual functions
    protected:  // New functions
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

        /**
        * Function for handling commands from Stylus popup menu
        * @since S60 v3.2
        * @param aControl
        * @param aEventType, type of event
        */
        void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType );

#endif  //RD_SCALABLE_UI_V2

    private:  // New virtual functions
    private:  // New functions
    private:  // Functions from base classes

    public:     // Data
    protected:  // Data

        /**
        *
        * @since S60 3.0
        */
        void UpdateIndicatorL();

        /**
        *
        * @since S60 3.0
        */
        void UpdateTextEditorL();

    private:    // Data

        // Reference to owner
        CIMSSettingsWizard& iWizard;
        //
        TDes&               iText;
        //
        TDesC&              iPageText;
    };

#endif      // IMSWTEXTEDITORPAGE_H

// End of File
