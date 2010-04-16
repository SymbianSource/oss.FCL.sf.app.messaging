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
 * Description:  This class provides custom list item for the
 * DataForm
 *
 */

#include <msgsettingsdataformcustomitem.h>
#include <hbdataformmodelitem.h>
#include <hbpushbutton.h>

MsgSettingsDataFormCustomItem::MsgSettingsDataFormCustomItem(QGraphicsItem *parent) :
HbDataFormViewItem(parent)
{
}

MsgSettingsDataFormCustomItem::~MsgSettingsDataFormCustomItem()
{
}

HbAbstractViewItem* MsgSettingsDataFormCustomItem::createItem()
{
    return new MsgSettingsDataFormCustomItem(*this);
}

HbWidget* MsgSettingsDataFormCustomItem::createCustomWidget()
{
    HbDataFormModelItem::DataItemType itemType =
    static_cast<HbDataFormModelItem::DataItemType> 
    ( modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt() );
    
    switch (itemType)
    {
      case HbDataFormModelItem::CustomItemBase + 1:
            { 
            //custom button
            QString str =
                    modelIndex().data(HbDataFormModelItem::KeyRole).toString();
            HbPushButton *button = new HbPushButton(str);
    
            return button;
            }
        default:
            return 0;
    }
}
