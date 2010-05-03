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
 * Message body preview class.
 * has in built logic for finding phone number, email, url.
 *
 */

#ifndef UNIVIEWERTEXTITEM_H
#define UNIVIEWERTEXTITEM_H

#include <HbTextEdit>
#include <xqappmgr.h>

//forward declarations
class QRegExp;
class XQAiwRequest;

class UniViewerTextItem : public HbTextEdit
{
    Q_OBJECT
public:
    UniViewerTextItem(QGraphicsItem* parent=0);
    ~UniViewerTextItem();
    void setFindOn(bool on = true);
    void setText(const QString& text);

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:

    /**
     * Applies find rule and highlights text.
     */
    void applyRule();

    /**
      * Helper method to add menus.
      * @param contextMenu menu object.
      * @param data, anchor at current position.
      */
    void addNumberMenu(HbMenu* contextMenu,const QString& data);
    void addEmailMenu(HbMenu* contextMenu,const QString& data);
    void addUrlMenu(HbMenu* contextMenu,const QString& data);
    
    /**
     * short tap handler.
     * @param anchor anchor at cursor position.
     */
    void shortTapAction(QString anchor);    
    
    /**
     * Helper method to highlight find item on tap.
     * @param highlight, if true highlight else dont.
     */
    void highlightText(bool highlight);
	
    /** Helper method to get contact id against phone number or e-mail id.
     * @param value phone number or email id.
     * @param fieldName name of field to be matched.
     * @param fieldType type of field to be matched.
     */
    int resolveContactId(const QString& value,
                         const QString& fieldName, 
                         const QString& fieldType);

private slots:
    /**
      * called when aboutToShowContextMenu signal is emitted.
      */
    void aboutToShowContextMenu(HbMenu *contextMenu, const QPointF &pos);
    
    /**
     * Called when option menu is closed.
     */
    void menuClosed();


    //handlers for phone number specific menu items.
    void call();
    void sendMessage();    

    //handlers for e-mail specific menu items.
    void createEmail();

    //handlers for url specific menu items.
    void openLink();
    void addToBookmarks();

    //common handlers.
    void openContactInfo();
    void saveToContacts();
    void copyToClipboard();
    
    /**
     * Slot for handling valid returns from the framework.
     * 
     * @param result const QVariant&
     */
    void handleOk(const QVariant& result);
    
    /**
     * Slot for handling errors. Error ids are provided as 
     * 32-bit integers.
     * @param errorCode qint32
     */
    void handleError(int errorCode, const QString& errorMessage);

    //called after service request is completed.
    void onServiceRequestCompleted();
    
signals:
    /**
     * this signal is emitted when send message is triggered for a highlighted number.
     */
    void sendMessage(const QString& phoneNumber);
    

private:
    //map to hold rule and patterns.
    QMap<QString,QString> mRules;

    //property to on/off find patterns. default is on.
    bool mFindOn;

    //char formats for highlight.
    QTextCharFormat mFormatHighlight;
    QTextCharFormat mFormatNormal;

    //Current cursor position.
    int mCursorPos;

    XQApplicationManager mAppManager;
};

#endif // UNIVIEWERTEXTITEM_H
