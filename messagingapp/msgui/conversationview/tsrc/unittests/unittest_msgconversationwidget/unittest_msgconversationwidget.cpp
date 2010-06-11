/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "msgconversationwidgetheaders.h"

/*
    Global variables that are used to control the test environment and/or
    observe the individual tests. The naming convention can be described with
    a ternary template
    
        <class>_<member>[_<suffix>],
    
    where <class> is the name of a mocked class, <member> is the name of a
    variable, a function, or a property under consideration, and <suffix> is
    an optional description. For example, if we wish to observe the number of
    A::B() calls, we declare a global integer a_b_callcount and increment it
    every time A::B() is called. The variable is also declared in the header
    file of the mocked class by using the extern keyword. Typically, these
    global variables are defined and reset in the unit-test class' helper
    functions.
    
    Mocked HbLabel class' global variables:
    
        • argument of setNumber() and/or setPlainText(),
        • number of setNumber() calls, and
        • number of setPlaintText() calls.
 */

int hbframeitem_framedrawer_callcount;
int hbstyle_setitemname_callcount;
int hbframedrawer_setframetype_callcount;
int hbtextedit_setreadonly_callcount;
int hbtextedit_setsmileysenabled_callcount;
const QString SUBJECT = "Subject";
const QString BODYTEXT = "Body Text";

QGraphicsWidget *hbwidget_parent;

/*
    The unit-test class. The class definition contains four helper functions
    and a number of test functions. The helper functions are responsible for
    the initialization and cleanup of the test environment, whereas the test
    functions implement the actual testing. Although the test functions are
    executed in the order they are defined, they should not depend on each
    other in any way. In other words, one should be able to scramble the test
    functions and still compile a working unit test.
 */
class TestMsgConversationWidget : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void testConstructor();
    void testSetSubject();
    void testSetBodyText();
};

/*
    Initializes the test environment. This function is automatically invoked
    before the first test function.
 */
void TestMsgConversationWidget::initTestCase()
{
}

/*
    Cleans up the test environment. This function is automatically invoked
    after the last test function.
 */
void TestMsgConversationWidget::cleanupTestCase()
{
}

/*
    Performs initialization for a single test function. This function is
    automatically invoked before each test function.
 */
void TestMsgConversationWidget::init()
{
    hbframeitem_framedrawer_callcount = 0;
    hbstyle_setitemname_callcount = 0;
    hbframedrawer_setframetype_callcount = 0;
    hbtextedit_setsmileysenabled_callcount = 0;
    hbtextedit_setreadonly_callcount = 0;
    hbwidget_parent = NULL;
}

/*
    Performs cleanup for a single test function. This function is auto-
    matically invoked after each test function.
 */
void TestMsgConversationWidget::cleanup()
{
}

/*
    Test the constructor with different parent arguments:
    
        1. null parent (i.e. default parent argument) and
        2. non-null parent.
 */
void TestMsgConversationWidget::testConstructor()
{
    // 1.
    MsgConversationWidget *widget = new MsgConversationWidget();
    QVERIFY(!hbwidget_parent);
    delete widget;
    
    // 2.
    MsgConversationWidget parent(NULL);
    widget = new MsgConversationWidget(&parent);
    QVERIFY(hbwidget_parent == &parent);
    
    QVERIFY(6 == hbframeitem_framedrawer_callcount);
    QVERIFY(6 == hbframedrawer_setframetype_callcount);
    QVERIFY(6 == hbstyle_setitemname_callcount);
    
    delete widget;
}

void TestMsgConversationWidget::testSetSubject()
{
    // 1.
    MsgConversationWidget* myWidget = new MsgConversationWidget();
    myWidget->setSubject(SUBJECT);
    QVERIFY(1 == hbtextedit_setreadonly_callcount);
    QVERIFY(1 == hbtextedit_setsmileysenabled_callcount);
    
    delete myWidget;
}

void TestMsgConversationWidget::testSetBodyText()
{
    // 1.
    MsgConversationWidget* myWidget = new MsgConversationWidget();
    myWidget->setBodyText(BODYTEXT);
    QVERIFY(1 == hbtextedit_setreadonly_callcount);
    QVERIFY(1 == hbtextedit_setsmileysenabled_callcount);
    
    delete myWidget;
}

/*
    Implement a main() function that initializes the test environment,
    executes all tests in the order they were defined, and finally cleans up
    the test environment.
 */
QTEST_APPLESS_MAIN(TestMsgConversationWidget)

/*
    Because both the declaration and the implementation of our test class are
    in a single .cpp file, we also need to include the generated moc file to
    make Qt's introspection work.
 */
#include "unittest_msgconversationwidget.moc"
