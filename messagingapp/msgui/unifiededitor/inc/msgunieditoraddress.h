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
 * Description:Header for entering To field (phone number, contact name).
 *
 */

#ifndef UNIFIED_EDITOR_ADDRESS_H
#define UNIFIED_EDITOR_ADDRESS_H

#include <hbwidget.h>

#include <convergedmessageaddress.h>
#include <QMap>

class HbTextItem;
class HbPushButton;
class HbLineEdit;
class MsgUnifiedEditorLineEdit;

class MsgUnifiedEditorAddress : public HbWidget
    {
    Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgUnifiedEditorAddress(const QString& label,
                            const QString& pluginPath,
                            QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgUnifiedEditorAddress();

    /**
     * Seeker method to return back data to editor's view
     * Removes duplicates.
     */
    ConvergedMessageAddressList addresses();

    /**
     * Get total number of recipient's (including duplicates)
     */
    int addressCount();

    /**
     * setter method to set address
     */
    void setAddresses(ConvergedMessageAddressList addrlist);
    
    /**
     * Get amount of digits to be used in contact matching
     */
    static int contactMatchDigits();
        
signals:
    /**
     * Emitted when send button from virtual ITUT is pressed.
     */
    void sendMessage();

    /**
     * Emitted when content is added or removed
     */
    void contentChanged();

private slots:
   
    /**
     * called after selection from pbk.
     */
    void fetchContacts();
    /**
     * Slot for handling valid returns from the phonebook contacts fetched.
     */
    void handleOk(const QVariant& result);
    
    /**
     * Slot for handling errors. Error ids are provided as 
     */
    void handleError(int errorCode, const QString& errorMessage);

    /**
     * Called when contentsChanged signal is emitted by the line edit
     */
    void onContentsAdded(const QString&);

    /**
     * Called when contentsChanged signal is emitted by the line edit
     * Checks for empty text content
     */
    void onContentsRemoved(const QString& text);

private:
    /**
     * Remove edit-field's user-deleted addresses from Map
     */
    void syncDeletionsToMap();

    /**
     * Add edit-field's user-added addresses to Map
     */
    void syncAdditionsToMap();
    
    /**
     * Removes duplicate addresses and gives unique address list
     */
    QStringList uniqueAddressList();
private:
    
    /**
     * Push button to launch phone book.
     */
    HbPushButton* mLaunchBtn;

    /**
     * line edit field.
     */
    MsgUnifiedEditorLineEdit* mAddressEdit;

    /**
     * string to hold plugin path.
     */
    QString mPluginPath;

    /**
     * address map.
     */
    QMap<QString, QString> mAddressMap;
   
    };

#endif //UNIFIED_EDITOR_ADDRESS_H
