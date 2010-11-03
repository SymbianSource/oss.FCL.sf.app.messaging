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
*     SMS UI panic type definition.
*
*/



#if !defined(__SMSUI_PAN__)
#define __SMSUI_PAN__

#if !defined(__E32BASE_H__)
#include <e32base.h>                     // GLREF_C
#endif

//  DATA TYPES
enum TSmsUiPanic
    {
    ESmsuEmptySelection,
    ESmsuWrongEntryType,
    ESmsuWrongMtmType,
    ESmsuFoldersNotSupported,
    ESmsuAttachmentsNotSupported,
    ESmsuNotAServiceEntry,
    ESmsuEditorFilenameDuplicate,
    ESmsuViewerFilenameDuplicate,
    ESmsuNotAMessage,
    ESmsuMessageParentNotLocalFolder,
    ESmsuMessageNotLocal,
    ESmsuNullPointer,

	ESmsiFoldersNotSupported,
    ESmsiNoIconForAttachment,
    ESmsiWrongMtm,
    ESmsiMtmUdUnknownMsgStatus,

	ESmsetdlgUnknownValidity,
    ESmsetdlgUnknownValidityChoice,
    ESmsetdlgUnknownConversion,
    ESmsetdlgUnknownConversionChoice,
    ESmsetdlgInvalidIndex,
   
	EBioViewNotAllowed,
	EBioOperationNotNull,
	EBioOperationDoesNotExist,
	
	ESmumUtilNullPointer
    };

GLREF_C void Panic(TSmsUiPanic aPanic);


#endif



//  End of File
