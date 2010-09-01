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
* Description:  Control class for mailbox wizard help screen
*
*/


#ifndef __CIMSWizardControl_H__
#define __CIMSWizardControl_H__

// FORWARD DECLARATIONS

class CAknsBasicBackgroundControlContext;
class CEikRichTextEditor;

// CLASS DECLARATION

/**
*  Control for mailbox wizard first page rich text editor help screen
*
*  @lib IMUMUTILS.DLL
*  @since S60 3.2
*/

NONSHARABLE_CLASS( CIMSWizardControl ) : public CEikBorderedControl
    {
    public: // Constructors and destructors

        /**
        * Constructor.
        */
        CIMSWizardControl();

        /**
        * Destructor.
        * @since S60 3.2
        */
        ~CIMSWizardControl();

    public: // Functions from base classes

        /**
        * From CCoeControl
        * @since S60 3.2
        */
        void ConstructFromResourceL( TResourceReader& aReader );

        /**
        * From CCoeControl
        * @since S60 3.2
        */
        void SizeChanged();

        /**
        * From CCoeControl.
        * @since S60 3.2
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl
        * @since S60 3.2
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        * @since S60 3.2
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl
        * @since S60 3.2
        */
        void HandleResourceChange( TInt aType );

        /**
        * Sets background control context
        * @since S60 3.2
        */
        void SetBackgroundContext( MAknsControlContext* aContext );

    private: // data
        
        // Rich text editor.
        CEikRichTextEditor* iEditor;

        // Background context.
        MAknsControlContext* iBgContext;
        
        
    };

#endif // __CIMSWizardControl_H__