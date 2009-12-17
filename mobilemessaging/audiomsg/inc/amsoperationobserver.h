/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Observer class for send,insert,save and launch functions
*
*/



#ifndef __AMSOPERATIONOBSERVER_H
#define __AMSOPERATIONOBSERVER_H

#include "audiomessage.hrh"

// CLASS DEFINITION

/**
 *  This class defines a generic observer interface.
 *  
 *
 *  @lib audiomessage.exe
 *  @since S60 v3.1
 */
class MAmsOperationObserver
    {
    public:
        virtual void EditorOperationEvent(
            TAmsOperationType aOperation,
            TAmsOperationEvent aEvent ) = 0;
    };

#endif  // __AMSOPERATIONOBSERVER_H
