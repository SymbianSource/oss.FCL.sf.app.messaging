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
*     This contains all the state specific logic of the bio-control.
*
*/



#ifndef WMLSTATE_H
#define WMLSTATE_H

// INCLUDES
#include "WMLBC.hrh"

#include <w32std.h>
#include <coedef.h>
#include <MsgEditor.hrh>

// FORWARD DECLARATIONS

class CCoeControl;
class CEikMenuPane;
class CWmlBioControl;
class CWmlSubItemBase;
class TCoeHelpContext;

// CLASS DECLARATION

/**
* Interface common for all the states.
*/
NONSHARABLE_CLASS( MWmlState )
    {
    public:
        virtual ~MWmlState() {};
    public: // These must be implemented for each different state.
        // See the CWmlBioControl interface for explanations when these
        // methods are called and what's their purpose.
        virtual void SetAndGetSizeL( TSize& aSize ) = 0;
        virtual TBool HandleBioCommandL( TInt aCommand ) = 0;
        virtual TRect CurrentLineRect() const = 0;
        virtual TBool IsFocusChangePossible( TMsgFocusDirection aDirection ) const = 0;
        virtual HBufC* HeaderTextLC() const = 0;
        virtual void OptionMenuPermissionsL( TUint32& aPermissions ) const = 0;   
        virtual void SetMenuCommandSetL( CEikMenuPane& aMenuPane ) = 0;        
        virtual TInt VirtualHeight() = 0;
        virtual TInt VirtualVisibleTop() = 0;
        virtual TBool IsCursorLocation( TMsgCursorLocation aLocation ) const = 0;
        virtual TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
            TEventCode aType ) = 0;
        virtual void SizeChanged() = 0;
        virtual void FocusChanged( TDrawNow aDrawNow ) = 0;
        virtual void CreateViewerL( const CCoeControl& aContainer ) = 0;
        virtual TBool CursorInFirstLine() const = 0;

        virtual void GetHelpContext(TCoeHelpContext& aHelpContext) const = 0;

    protected: // State transition logic.
        
        /**
        * Called when entered to this state.
        * @param aState enumeration for the state.
        * Enumeration is used to check if it's legal
        * to enter this state.
        */
        virtual void EnterL( TWmlState aState ) = 0;


    private:

        friend class CWmlStateFactory;
    };

/**
* CWmlStateFactory owns the states and it's used to access
* the current state.
*/
NONSHARABLE_CLASS( CWmlStateFactory ) : public CBase
    {
    public:
        
        /**
        * Creates the new CWmlStateFactory object.
        * @param aBioControl reference to the biocontrol.
        */
        static CWmlStateFactory* NewL( CWmlBioControl& aBioControl );
        
        /**
        * Destructor.
        */
        ~CWmlStateFactory();

    public:

        /**
        * Defines the initial state from the item count.
        * @param aAPCount count of Accesspoint items.
        * @param aMMSCount count of MMS setting items.
        * @param aBMCount count of Bookmark items.
        */
        void SetInitialStateL( 
            const TInt aBMCount );

        /**
        * Get the current state object.
        * @return reference to the current state object.
        */
        MWmlState& State() const;

    protected:

        /**
        * Do the construction.
        * @param aBioControl reference to the biocontrol.
        */
        void ConstructL( CWmlBioControl& aBioControl );

    private:
        
        /**
        * Constructor, hidden from outsiders.
        */
        CWmlStateFactory() {};

        /**
        * Change the state to the inquired one.
        * @param aState enumeration of the desired state.
        * If the state transisition is not possible keeps the
        * current stete.
        */
        void ChangeStateL( TWmlState aState );

    private: // Concrete state objects.

        //// Pointer to the single state object.
        MWmlState* iStateSingle;
                
        //// Pointer to the biocontrol
        CWmlBioControl* iBioControl;

        /// Pointer to the current state. NOT DESCRETE OBJECT.
        MWmlState* iState;

    private: // State enumerations

        /// Current state as enumeration.
        TWmlState iStateEnum;

    private:
        friend class CWmlState;
    };

/**
* Common base class for all the state objects.
*/
NONSHARABLE_CLASS( CWmlState ) : public CBase,
    public MWmlState
    {
    protected:
        /**
        * Constructor.
        * @param aStateFactory pointer to the state factory.
        * @param aBioControl reference to the biocontrol.
        */
        CWmlState( CWmlStateFactory* aStateFactory, 
            CWmlBioControl& aBioControl);

    protected: // State transition logic.
        
        /**
        * Accesses the factory ChangeStateL-method.
        * @param aNextState enumeration for the next state.
        */
        //void ChangeStateL( TWmlState aNextState );

    private:

        /**
        * Default constructor.
        */
        CWmlState() {};

    protected:

        /// Pointer to the state factory, not owned.
        CWmlStateFactory* iStateFactory;

        /// Pointer to the bio control.
        CWmlBioControl* iBioControl;

        /// Current state.
        TWmlState iState;
    };

/**
* Single state.
*/
NONSHARABLE_CLASS( CWmlStateSingle ) : public CWmlState
    {
    public:
        CWmlStateSingle( CWmlStateFactory* aStateFactory, 
            CWmlBioControl& aBioControl);
        ~CWmlStateSingle();
    public: // These must be implemented for each different state.
        void SetAndGetSizeL( TSize& aSize );
        TBool HandleBioCommandL( TInt aCommand );
        TRect CurrentLineRect() const;
        TBool IsFocusChangePossible( TMsgFocusDirection aDirection ) const;
        HBufC* HeaderTextLC() const;
        void OptionMenuPermissionsL( TUint32& aPermissions ) const;    
        void SetMenuCommandSetL( CEikMenuPane& aMenuPane );
        TInt VirtualHeight();
        TInt VirtualVisibleTop();
        TBool IsCursorLocation( TMsgCursorLocation aLocation ) const;
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
            TEventCode aType );
        void SizeChanged();
        void FocusChanged( TDrawNow aDrawNow );
        void CreateViewerL( const CCoeControl& aContainer );
        TBool CursorInFirstLine() const;

        void GetHelpContext(TCoeHelpContext& aHelpContext) const;
    protected: // State transition logic.
        void EnterL( TWmlState aState );
    };

#endif // WMLSTATE_H

// End of file
