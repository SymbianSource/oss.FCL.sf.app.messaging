/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declares UI class for BVA application.
*
*/



#ifndef _BVAAPPUI_H_
#define _BVAAPPUI_H_

// INCLUDES
#include <msgbiocontrolObserver.h>
#include <aknappui.h>
#include <bldvariant.hrh> //__SERIES60_HELP

// FORWARD DECLARATIONS
class CBvaContainer;

enum TBVAPanic
    {
    EMenuPaneNull
    };


// CLASS DECLARATION

/**
 * Application UI class of BVA.
 */
NONSHARABLE_CLASS( CBvaAppUi ) :
	public CAknAppUi, public MMsgBioControlObserver
    {
    public: // Construction & destruction

        /// Second phase constructor.
        void ConstructL();

        /// Destructor.
        ~CBvaAppUi();

    public: // new functions

        /**
         * Loads the bio control.
         * @param aFile Data file handle.
         */
        void LoadBioControlL( const RFile& aFile );

    private: // from MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    public: // from MMsgBioControlObserver

        /**
         * Gives the first free command ID.
         * @return Gives the first free command ID.
         */
        TInt FirstFreeCommand() const;

        /**
         * For requesting a command to be handled.
         * @param aCommand Command type.
         */
        TInt RequestHandleCommandL(TMsgCommands aCommand);

        /**
         * For querying if a command is supported or not.
         * @param aCommand Command type.
         */
        TBool IsCommandSupported(TMsgCommands aCommand) const;

    private: // from CEikAppUi
        /**
         * Command handler.
         * @param aCommand command to be handled
         */
        void HandleCommandL(TInt aCommand);

        /**
         * Handles key events.
         * @param aKeyEvent Event to handled.
         * @param aType Type of the key event.
         * @return Reponse code (EKeyWasConsumed, EKeyWasNotConsumed).
         */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

    private:

        /**
         * Whether a data file has been specified.
         * @return ETrue if there is no data file.
         */
        TBool NoDataFile();

        /**
        * Launches help application
        */
        void LaunchHelpL();

        /**
        * Returns help context
        * @return application specific help context
        */
        CArrayFix<TCoeHelpContext>* HelpContextL() const;

        /**
        * Create a help context array.
        * @return Help context array
        */
        CArrayFix<TCoeHelpContext>* CreateHelpContextArrayL(
        	const TCoeHelpContext& aHelpContext ) const;

        /**
		* Panic wrapper.
		* @param aCode panic code
		*/
    	void Panic(TBVAPanic aCode);

    private: //Data

        /// This is a CCoeControl window owning container.
        CBvaContainer* iContainer;

        /**
         * Is used to mark whether biocontrol was loaded using file handle fwk
         * instead of filename. -1 = file name based, 0 = invalid handle,
         * valid handle > 0.
         */
        TInt iFileHandleSize;
        
        // Variable used to hold the status of BIO control instantiation status
        TBool iBioControlLoadFailed;

    };

#endif //_BVAAPPUI_H_

// End of File
