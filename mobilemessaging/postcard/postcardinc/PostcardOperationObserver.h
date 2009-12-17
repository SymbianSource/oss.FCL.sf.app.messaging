/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       CPostcardOperationObserver class
*
*/



#ifndef __POSTCARDOPERATIONOBSERVER_H
#define __POSTCARDOPERATIONOBSERVER_H

// INCLUDES

#include <e32base.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

// DATA TYPES

// Enumeration for the operation type
enum TPostcardOperationType
    {
    EPostcardOperationOpen,
    EPostcardOperationInsertImage,
    EPostcardOperationSave,
    EPostcardOperationSend
    };

// Enumeration for the operation event
enum TPostcardOperationEvent
	{
	EPostcardOperationComplete,
    EPostcardOperationError
	};

// Enumeration for the file type
enum TPostcardFileType
    {
    EPostcardFileImage,
    EPostcardFileText,
    EPostcardFileAddress,
    EPostcardFileSmil,
    EPostcardFileUnknown,
    };

// FUNCTION PROTOTYPES

// CLASS DECLARATION

// Interface class for operation events
class MPostcardOperationObserver
    {
    public:
        virtual void PostcardOperationEvent(
            TPostcardOperationType aOperation,
            TPostcardOperationEvent aEvent ) = 0;
    };

#endif // __POSTCARDOPERATIONOBSERVER_H
