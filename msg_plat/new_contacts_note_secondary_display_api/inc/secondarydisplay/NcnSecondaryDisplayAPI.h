/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   The set of messages provided to Secondary Display software by
*              : Ncnlist.
*
*/


#ifndef NCNSECONDARYDISPLAYAPI_H
#define NCNSECONDARYDISPLAYAPI_H

// INCLUDES
#include <e32base.h>

namespace SecondaryDisplay
{

// The category UID for the messages in this header file.
//
const TUid KCatNcnList = {0x100058F1};


/*
* ==============================================================================
* Dialogs shown by Ncnlist subsystem. These messages are handled using the
* Secondary Display support in Avkon.
* ==============================================================================
*/
enum TSecondaryDisplayNcnListDialogs
    {

    /**
    * A command for not showing notification on secondary display.
    */
    ECmdNoNotification = 1,

    /**
    * A command for showing "new messages" note on secondary display.
    */
    ECmdMessagesNotification,

    /**
    * A command for showing class 0 sms on secondary display.
    */
    ECmdClass0MessageNotification,

    /**
    * A command for showing "new email" note on secondary display.
    */
    ECmdEmailNotification,

    /**
    * A command for showing "new voice mail" note on secondary display.
    */
    ECmdVoiceMailNotification,

    /**
    * A command for showing "new voice mail on line 1" note on secondary display.
    */
    ECmdVoiceMailOnLine1Notification,

    /**
    * A command for showing "new voice mail on line 2" note on secondary display.
    */
    ECmdVoiceMailOnLine2Notification,

    /**
    * A command for showing "missed calls" note on secondary display.
    */
    ECmdMissedCallsNotification,

    /**
    * A command for showing "new audio message" note on secondary display.
    */
	ECmdAudioMessagesNotification

    };

} // namespace

#endif      // NCNSECONDARYDISPLAYAPI_H

// End of File
