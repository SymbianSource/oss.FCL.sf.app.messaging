/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <QtTest/QtTest>
#include <QObject>
#include <qtestcase.h>
#include <t_s60qconversions.h>


void TestS60QConversions::initTestCase()
{

    mS60QConversions=0;
}


void TestS60QConversions::Tests60DescToQString()
{
    HBufC* temp = HBufC::NewLC(10);
    temp->Des().Copy(_L("Sometext"));
        
    QString text= S60QConversions::s60DescToQString(temp->Des());
    QVERIFY(text==QString("Sometext"));
    CleanupStack::PopAndDestroy(temp);
       
}

void TestS60QConversions::TestqStringToS60Desc()
{
    QString str ="ExampleText"; 
    HBufC* s60text = S60QConversions::qStringToS60Desc(str);
    QVERIFY(str==QString("ExampleText"));

}

void TestS60QConversions::Tests60Desc8ToQString()
{
    HBufC8* temp = HBufC8::NewLC(10);
    temp->Des().Copy(_L("Sometext"));
        
    QString text= S60QConversions::s60Desc8ToQString(temp->Des() );
    QVERIFY(text==QString("Sometext"));
    
    CleanupStack::PopAndDestroy(temp);
}

void TestS60QConversions::TestqStringToS60Desc8()
{
    QString str ="ExampleText"; 
    _LIT8(qStr8,"ExampleText");
    HBufC8* s60text = S60QConversions::qStringToS60Desc8(str);
    QVERIFY(s60text->Des() == qStr8);
    
}

void TestS60QConversions::cleanupTestCase()
{
    delete mS60QConversions;
}




