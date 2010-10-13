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
* Description:   Central repository keys and repository identifiers related to
*                the S60 messaging components visible in the domain APIs
*
*/



#ifndef MESSAGINGDOMAINCRKEYS_H
#define MESSAGINGDOMAINCRKEYS_H

//
// Messaging Configuration API
const TUid KCRUidMuiuMessagingConfiguration = {0x102824A0};

//
// The field controlling the usage of the soft notifications
// and constants defining the usage of the bits of the field
const TUint32 KMuiuSoftNotificationConfiguration = 0x00000001;
const TUint32 KMuiuMissedCallSNFlag     = 1;
const TUint32 KMuiuNewMessageSNFlag     = 2;
const TUint32 KMuiuNewVoiceMailSNFlag   = 4;
const TUint32 KMuiuNewEmailSNFlag       = 8;
const TUint32 KMuiuInstantMessageSNFlag = 16;
const TUint32 KMuiuAudioMessageSNFlag   = 32;

//
// The field instructing the clients of Send UI service
// and constants defining the usage of the bits of the field
const TUint32  KMuiuSendUiConfiguration          = 0x00000002;
const TUint32 KMuiuEmailDisabledInMultimedia = 1;

// Selectable Default email related settings
const TUid KCRUidSelectableDefaultEmailSettings = {0x101F8F28};
const TUint32 KSelectableDefaultMailAccount = 0x00000001;
const TUint32 KIntegratedEmailAppMtmPluginId = 0x00000002; 
//
// The field controlling the Display Lights of the soft notifications
// and constants defining the usage of the bits of the field
const TUint32 KMuiuDisplayLightsConfiguration = 0x00000003;
const TUint32 KMuiuDLMissedCallFlag     = 1;
const TUint32 KMuiuDLNewMessageFlag     = 2;
const TUint32 KMuiuDLNewVoiceMailFlag   = 4;
const TUint32 KMuiuDLNewEmailFlag       = 8;
const TUint32 KMuiuDLInstantMessageFlag = 16;
const TUint32 KMuiuDLAudioMessageFlag   = 32;
#endif // MESSAGINGDOMAINCRKEYS_H