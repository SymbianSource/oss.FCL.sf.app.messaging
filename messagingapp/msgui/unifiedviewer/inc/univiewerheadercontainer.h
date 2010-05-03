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
 * Description: This widget holds the message header.
 *
 */

#ifndef UNIVIEWER_HEADER_CONTAINER_H
#define UNIVIEWER_HEADER_CONTAINER_H

#include <HbWidget>

#include <convergedmessageaddress.h>

class UniViewerDetailsWidget;
class HbGroupBox;
class UniViewerAddressContainer;
class UniViewerAttachmentContainer;
class UniViewerFeeder;

class UniViewerHeaderContainer: public HbWidget
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit UniViewerHeaderContainer(UniViewerFeeder* feeder, QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~UniViewerHeaderContainer();

    /**
     * Populates the content on the widget
     */
    void populateContent();

    /**
     * Clears the content on the widget
     */
    void clearContent();

private:

    /**
     * Populates the subject field in Details widget.
     */
    void populateSubject();

    /**
     * Populates the attachment list in Attachment container.
     */
    void populateAttachments();

    /**
     * Populates the from, to & cc fields.
     */
    void populateAddressContainer();

    /**
     * Sets the heading text of the address group box.
     */
    void setAddrGroupBoxHeading();

    /**
     * Creates a comma seperated address list using converged message address list.
     * @param addressList Converged message address list.
     * @return Comma separated address list.
     */
    QString createAddressList(const ConvergedMessageAddressList &addressList);

private:

    /**
     * UniViewerFeeder object
     * Not Owned
     */
    UniViewerFeeder* mViewFeeder;

    /**
     * UniViewerDetailsWidget object
     * Owned
     */
    UniViewerDetailsWidget *mViewerDetails;

    /**
     * HbGroupBox object
     * owned.
     */
    HbGroupBox *mHeaderGroupBox;

    /**
     * UniViewerAddressContainer object
     * owned.
     */
    UniViewerAddressContainer *mAddressContainer;

    /**
     * UniViewerAttachmentContainer object
     * owned.
     */
    UniViewerAttachmentContainer *mAttachmentContainer;
};

#endif /* UNIVIEWER_HEADER_CONTAINER_H */

// EOF
