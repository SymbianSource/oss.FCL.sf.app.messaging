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
* Description:  
*       UniEditor feature flags
*
*/



#ifndef __UNIEDITORFEATURES_H
#define __UNIEDITORFEATURES_H

// DATA TYPES

enum TUniFeatures
    {
    EUniFeatureHelp                 = 0x00000001,
    EUniFeatureSmilEditor           = 0x00000004,
    EUniFeatureOffline              = 0x00000008,
    EUniFeatureDrmFull              = 0x00000020,
    EUniFeatureSubject              = 0x00000040,
    EUniFeatureSVK                  = 0x00000100,
    EUniFeatureJapanese             = 0x00000200, 
    EUniFeatureUpload               = 0x00000400,
    EUniFeatureCc                   = 0x00000800,   
    EUniFeatureBcc                  = 0x00001000,   
	EUniFeatureCamcorder            = 0x00002000,
    EUniFeatureDeliveryStatus       = 0x00004000,
    EUniFeaturePriority             = 0x00008000,
    EUniFeatureAudioMessaging       = 0x00010000,
    EUniFeatureSubjectConfigurable  = 0x00020000
    };

#endif // __UNIEDITORFEATURES_H
