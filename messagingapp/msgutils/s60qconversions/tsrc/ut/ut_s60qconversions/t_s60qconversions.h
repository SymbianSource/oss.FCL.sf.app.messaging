/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
#ifndef T_S60QCONVERSIONS_H
#define T_S60QCONVERSIONS_H


#ifdef BUILD_TEST_DLL
#define TEST_EXPORT Q_DECL_EXPORT
#else
#define TEST_EXPORT Q_DECL_IMPORT
#endif


#include <QObject>
#include "s60qconversions.h"


class TEST_EXPORT TestS60QConversions: public QObject
{
    Q_OBJECT

private slots:

   void initTestCase();

    void Tests60DescToQString();

	void TestqStringToS60Desc();

	void Tests60Desc8ToQString();

	void TestqStringToS60Desc8();

	void cleanupTestCase();


private:

    S60QConversions* mS60QConversions;
};

#endif //TEST_S60QCONVERSIONS_H
