rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description: This bat copies necessary setting files for STIF test
rem	framework
rem

echo off
@echo Imum API Tests (STIF)
@echo --

REM *** Change dir to SDK_DRIVE
REM %L_SDKDRIVE%

REM *** Copy ini-files
@echo Copy TestFramework.ini to \epoc32\winscw\c\TestFramework\
copy TestFramework.ini %L_SDKDRIVE%\epoc32\winscw\c\TestFramework\

REM *** Copy cfg-files
@echo Copy ui_ImumDomainApiTest.cfg to \epoc32\winscw\c\TestFramework\
copy ui_ImumDomainApiTest.cfg %L_SDKDRIVE%\epoc32\winscw\c\TestFramework\