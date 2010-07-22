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
 * Base view for all messaging views (CLV,CV etc).
 *
 */
 
#include <HbView>
#include <QVariant>

#ifndef MSG_BASE_VIEW_H_
#define MSG_BASE_VIEW_H_


class MsgBaseView : public HbView
{
    Q_OBJECT
    
    public:
    /**
     * constructor.
     */
    MsgBaseView(QGraphicsItem* parent=0):HbView(parent){}
    
    /**
     * enum defining view id(s).
     */
    enum viewId
        {
        DEFAULT = 0,
        CLV,
        CV,
        DLV,
        UNIEDITOR,
        UNIVIEWER,
        MSGSETTINGS,
        SERVICE,
        AUDIOFETCHER
        };
    
    /**
     * enum definition for differentiating switchview to unieditor command further
     */
    enum UniEditorOperation
        {
        ADD_PHOTO = 0x01,
        ADD_AUDIO = 0x02,
        ADD_VIDEO = 0x04,
        ADD_VCARD = 0x08,
        ADD_OTHERS = 0x10,
        ADD_RECIPIENTS = 0x20,
        ADD_SUBJECT = 0x40,
        FORWARD_MSG = 0x80
        };
    
    signals:
    /**
     * This signal is emitted whenever a view switch is needed.
     */
    void switchView(const QVariantList& data);
    
    /**
     * Forceful close of conversation view
     */
    void closeOpenConversationView();

};

#endif //MSG_BASE_VIEW_H_
