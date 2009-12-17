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
*     Panic codes for bio control
*
*/



#ifndef WMLBIOCONTROL_PAN
#define WMLBIOCONTROL_PAN

enum TWMLBCPanicCodes
    {
	EParseFieldsCalledTwice = 1000,
    EItemArrayNotNull,
	EItemArrayNull,
	EItemArraysNotNull,
	EItemArraysNull,
	EItemArraysNotEmpty,
	EIllegalInternalState,
	EAsyncItemSaverNotNull,
	EAsyncItemSaverNull,
	ESelectedAPNULL,
	EAddressArrayCountShouldBeZero,
    EBioControlUnknownSubItem,
    EAPSubItemGetFieldInvalidIndex,
    EUnknownBearerType,
	EUnknownSpeed,
    EIndexOutOfBounds,
	ENoCurrentItemSet,
    EDetailWindowAlreadyExists,
	ENoDetailWindowCreated,
	ERichBioNull,
	ERichBioNotNull,
	EIllegalCommandInCurrentState,
	EItemWithGivenNameDoesNotExist,
	EBookmarkNameTruncated,
	EBookmarkUrlTruncated,
	EOriginalNameNotNull,
	ENameNotNull,
	EWmlBCStateObjectAlreadyCreated,
	EWmlBCStateObjectNotCreated,
	EIllegalStateTransition,
	EStateCantBeDeterminde,
	EDeprecated,
	EMMSAccesspointNotSetAsDefined,
	EIllegalMethodCallOrder,
	EPreConditionFail,
	EPostConditionFail,
	ENullPtr
    };

GLDEF_C void Panic( TInt aPanic );

#endif