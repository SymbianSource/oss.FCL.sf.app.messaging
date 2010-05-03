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

#include "univieweraddresswidget.h"
#include "univiewerfeeder_p.h"
// SYSTEM INCLUDES
#include <HbApplication>
#include <QString>
#include <QTextCursor>
#include <HbMenu>

// USER INCLUDES
#include "unitexteditor.h"

// LOCAL CONSTANTS
const QString ADDRESS_SEPARATOR("; ");
const QString ADDRESS_OPEN(" (");
const QString ADDRESS_CLOSE(")");

//----------------------------------------------------------------------------
// UniViewerAddressWidget::UniViewerAddressWidget
// @see header file
//----------------------------------------------------------------------------
UniViewerAddressWidget::UniViewerAddressWidget(QGraphicsItem *parent) :
    HbWidget(parent), mAddress(NULL)
{
    mAddress = new UniTextEditor(this);
    HbStyle::setItemName(mAddress, "addressField");

    connect(mAddress, SIGNAL(aboutToShowContextMenu(HbMenu *,QPointF)), this,
        SLOT(handleAboutToShowContextMenu(HbMenu *,QPointF)));

    // TODO: Wrapping fix breaking normal scenarios.
    // this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::~UniViewerAddressWidget
// @see header file
//----------------------------------------------------------------------------
UniViewerAddressWidget::~UniViewerAddressWidget()
{
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::populate
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::populate(const QString &label,
                                      const QString &address,
                                      const QString &alias)
{

    QTextCursor cursor(mAddress->document());

    QTextCharFormat addressFormat = cursor.charFormat();
    addressFormat.setFontWeight(QFont::Bold);
    addressFormat.setForeground(QApplication::palette().link());
    addressFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

    // Insert the label then the addresses
    cursor.insertText(label);
    QString address1 = QString();
    if (!(alias.isEmpty()))
    {
        address1.append(alias);
        QString alias1 = QString();
        if (UniViewerFeederPrivate::GetNameFromContacts(address, alias1) > 1)
        {
            address1.append(ADDRESS_OPEN);
            address1.append(address);
            address1.append(ADDRESS_CLOSE);
        }
    }
    else
    {
        address1.append(address);
    }
    addressFormat.setAnchorHref(address);
    cursor.insertText(address1, addressFormat);
    repolish();
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::populate
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::populate(const QString &label,
                                      ConvergedMessageAddressList addressList)
{
    QTextCursor cursor(mAddress->document());

    QTextCharFormat defaultFormat = cursor.charFormat();
    defaultFormat.setFontWeight(QFont::Bold);
    defaultFormat.setForeground(QApplication::palette().link());

    QTextCharFormat addressFormat = cursor.charFormat();
    addressFormat.setFontWeight(QFont::Bold);
    addressFormat.setForeground(QApplication::palette().link());
    addressFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

    // Insert the label then the addresses
    cursor.insertText(label);

    int addressCount = addressList.count();

    for (int i = 0; i < addressCount; ++i)
    {

        QString address = QString();
        if (! (addressList[i]->alias().isEmpty()))
        {
            address.append(addressList[i]->alias());
            QString alias = QString();
            if (UniViewerFeederPrivate::GetNameFromContacts(addressList[i]->address(),
                                                            alias) > 1)
            {
                address.append(ADDRESS_OPEN);
                address.append(addressList[i]->address());
                address.append(ADDRESS_CLOSE);
            }
        }
        else
        {
            address.append(addressList[i]->address());
        }

        addressFormat.setAnchorHref(addressList[i]->address());
        cursor.insertText(address, addressFormat);

        if (addressCount - 1 != i)
        {
            cursor.insertText(ADDRESS_SEPARATOR, defaultFormat);
        }
        
    }
    repolish();
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::clearContent
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::clearContent()
{
    mAddress->document()->clear();
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::handleAboutToShowContextMenu
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::handleAboutToShowContextMenu(HbMenu *contextMenu, const QPointF &pos)
{
    Q_UNUSED(pos)
    contextMenu->clearActions();
}

// EOF
