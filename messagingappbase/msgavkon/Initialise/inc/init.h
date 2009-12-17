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
* Description:  init  declaration
*
*/




#ifndef __INIT_H_
#define __INIT_H__

#include <eikenv.h>
#include <eikappui.h>

#ifdef _DEBUG
_LIT(KInitLoggerBanner, "MailInit -> %S");
#define INITLOGGER(a)                   {_LIT(temp, a); RDebug::Print(KInitLoggerBanner, &temp);}
#define INITLOGGER_NUMBER(a)            {TBuf<20> num; num.Num(a); RDebug::Print(KInitLoggerBanner, &num);}
#define INITLOGGER_WRITE_FORMAT(a,b)    {_LIT(temp, a); TBuf<256> buf; buf.Format(temp, b); RDebug::Print(KInitLoggerBanner, &buf);}
#define INITLOGGER_WRITE_FORMAT2(a,b,c)    {_LIT(temp, a); TBuf<256> buf; buf.Format(temp, b, c); RDebug::Print(KInitLoggerBanner, &buf);}
#else
#define INITLOGGER(a)       
#define INITLOGGER_NUMBER(a)    
#define INITLOGGER_WRITE_FORMAT(a,b)
#define INITLOGGER_WRITE_FORMAT2(a,b,c) 
#endif

/**
* CMailInitAppUi class
*/
class CMailInitAppUi : public CEikAppUi
    {
    public:
        /**
        * Destructor
        */
        ~CMailInitAppUi();

        /**
        * Symbian constructor
        */
        void ConstructL();
    };

/**
* CMailInitServerEnv
*/ 
class CMailInitServerEnv : public CEikonEnv
    {
    public:
        /**
        * From CEikonEnv
        */
        void DestroyEnvironment();

        /**
        * Symbian constructor
        */
        void ConstructL();

    };



enum TMailInitPanic{EMailInitPanicStartAllocEnv,
                    EMailInitPanicStartAllocAppUi,
                    EMailInitPanicStartConstructL,
                    EMailInitPanicStartAllocCleanupStack,
                    EMailInitPanicWinsLoadLibrary,
                    EMailInitisationFailed};

GLREF_C void Panic(TMailInitPanic);

#endif
// End of file
