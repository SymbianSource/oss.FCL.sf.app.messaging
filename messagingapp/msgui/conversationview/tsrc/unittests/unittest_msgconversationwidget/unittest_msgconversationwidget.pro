#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#

CONFIG            += qtestlib symbian_test
INCLUDEPATH       += .
MMP_RULES         += "USERINCLUDE . ../../../../../../inc"
MOC_DIR            = moc
TARGET             = unittest_msgconversationwidget
TARGET.CAPABILITY  = All -TCB
TEMPLATE           = app

HEADERS += hbwidget.h \
           msgconversationwidgetheaders.h \
           ../../../inc/msgconversationwidget.h

SOURCES += unittest_msgconversationwidget.cpp \
           ../../../src/msgconversationwidget.cpp
