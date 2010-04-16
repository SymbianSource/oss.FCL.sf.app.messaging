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
* Description:  This holds all the attachments inside convergedmessage
*
*/

#ifndef UNIVIEWERATTACHMENTCONTAINER_H
#define UNIVIEWERATTACHMENTCONTAINER_H

// SYSTEM INCLUDES
#include <HbWidget>
#include <QList>

// FORWARD DECLARATIONS
class QGraphicsLinearLayout;
class UniViewerMediaWidget;

//const int MAX_ATTACHMENT = 10;

/**
 * Container widget for all attachment inside message
 */

class UniViewerAttachmentContainer: public HbWidget
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit UniViewerAttachmentContainer(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    virtual ~UniViewerAttachmentContainer();

    /**
     * Add one more attachment into the widget
     */
    void addAttachmentWidget(QString type, QString fileName);

    /**
     * Clears the content of the widget
     */
    void clearContent();

private:

    /**
     * This contains total number of attacmnets added
     */
    int mTotalAttachment;

    /**
     * Main layout reference.
     * Own
     */
    QGraphicsLinearLayout *mMainLayout;

    /**
     * From widget reference.
     * Own
     */
    QList<UniViewerMediaWidget *> mUniViewerMediaWidgetList;

};

#endif // UNIVIEWERATTACHMENTCONTAINER_H
//eof
