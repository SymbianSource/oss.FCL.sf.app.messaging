/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Message view plug-in interface
*
*/



#ifndef MMAILMESSAGEVIEW_H
#define MMAILMESSAGEVIEW_H

#include <e32def.h>
#include <coedef.h>
#include <w32std.h>


// FORWARD DECLARATIONS
class MMailAppUiInterface;
class CEikMenuPane;
class CMailMessage;
class RFileReadStream;

// CLASS DECLARATION

/**
*  MMailMessageView interface.
*
*  @since Series 60 3.0
*/
class MMailMessageView
    {
    public:  // destructor

        /**
        * Destructor.
        */
        virtual ~MMailMessageView(){};
        
    public: // New functions

        /**
        * DynInitOptionsMenuL.
        * @since Series 60 3.0
        * @param aResourceId menu resource
        * @param aMenuPane menu pane
        */
        virtual void DynInitMenuPaneL(
            TInt aResourceId,CEikMenuPane* aMenuPane) = 0;

        /**
        * HandleCommandL.
        * @since Series 60 3.0
        * @param aCommand command id
        */
        virtual TBool HandleCommandL(TInt aCommand) = 0;

        /**
        * HandleCommandL.
        * @since Series 60 3.0
        * @param aKeyEvent key event
        * @param aType event type
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType) = 0;

        /**
        * MessageLoadL
        * @since Series 60 3.0
        * Called after each messag loading step so
        * that plug-in can examine message step by step.
        * Plug-in should not add any UI controls to app ui at this state.
        * @param aState message loading state. See TLoadState <CMailMessage.h>
        * @param aMessage Mail message.
        * If this function leaves, plug-in will be delete and
        * removed from the plug-in array.
        */
        virtual void MessageLoadL(TInt aState, CMailMessage& aMessage) = 0;

        /**
		* ViewMessageL.
		* @since Series 60 3.0
		* This function is called from the UI only the selected Plug-in that is
        * used for viewin the message.
		* @param aReadStream stream
		* @param aUICallBack mail viewer UI call back.
		*/
		virtual void ViewMessageL(
			RFileReadStream& aReadStream,
			MMailAppUiInterface& aUICallBack) = 0;

        /**
        * ViewMessageL
        * @since Series 60 3.0
        * This function is called from the UI only the selected Plug-in that is
        * used for viewin the message.
        * @param aMessage Fully loaded mail message.
        * @param aUICallBack mail viewer UI call back.
		* @param aParams params from plug-in to mail UI.        
        */
        virtual void ViewMessageL(
            CMailMessage& aMessage,
            MMailAppUiInterface& aUICallBack,
            TUint& aParams) = 0;

		/**
	    * ResourceFile.
	    * @since Series 60 3.0
	    * @param aResourceFile name of the resource file
	    * Gives the caller a resource file to load
	    */
        virtual const TDesC& ResourceFile() = 0;
    };

#endif      // MMAILMESSAGEVIEW_H

// End of File
