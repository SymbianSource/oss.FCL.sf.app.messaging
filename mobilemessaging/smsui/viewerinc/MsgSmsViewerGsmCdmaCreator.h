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
*       Definition for Sms Viewer creator class.
*
*/



#ifndef MSGSMSVIEWERGSMCDMACREATOR_H
#define MSGSMSVIEWERGSMCDMACREATOR_H

class CMsgSmsViewerAppUi;

class SmsViewerCreator
    {
    public: 

    /**
    * Creates viewer AppUi-class. Either GSM / CMDA -version depending on the buildflags.
    * @return pointer to CMsgSmsViewerAppUi
    */
    static CMsgSmsViewerAppUi* CreateAppUiL();
    };

#endif      //  MSGSMSVIEWERGSMCDMACREATOR_H
