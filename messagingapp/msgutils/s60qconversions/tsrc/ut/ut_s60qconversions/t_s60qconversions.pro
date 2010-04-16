#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#
QT += testlib

TEMPLATE = lib
TARGET = 


INCLUDEPATH += . 

INCLUDEPATH += ../../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../../inc



DEFINES += BUILD_TEST_DLL 
HEADERS += \
					t_s60qconversions.h   

SOURCES += \
					 t_s60qconversions.cpp      

SYMBIAN_PLATFORMS = WINSCW ARMV5
symbian {
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    TARGET.EPOCALLOWDLLDATA = 1
		}
LIBS+= -ls60qconversions

