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
* Description: Derieved HbLineEdit class for custom address editor
*
*/

#ifndef MSGUNIFIEDEDITORLINEEDIT_H
#define MSGUNIFIEDEDITORLINEEDIT_H

#include <HbLineEdit>
#include<QBasicTimer>


class MsgUnifiedEditorLineEdit : public HbLineEdit
{
    Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgUnifiedEditorLineEdit(const QString& label, QGraphicsItem* parent=0);
    
    /**
     * Destructor
     */
    ~MsgUnifiedEditorLineEdit();
    
    /**
     * seeker method for getting address list
     */
    QStringList addresses();

    void setDefaultBehaviour(bool defaultBehaviour = false);
    QString content() const;
	
signals:
    void contentsChanged(const QString& text);

public slots:
    void setText(const QString &text);
    void cut();
    void selectAll();

private slots:
    void selectionChanged(const QTextCursor &oldCursor, const QTextCursor& newCursor);
    void onContentsChanged();
    
protected: // from HbLineEdit
    void inputMethodEvent(QInputMethodEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void focusInEvent(QFocusEvent* event);
    void timerEvent (QTimerEvent *event);


private:
    void setHighlight(int currentPos);
    QString text() const;

private:
    QRegExp mLabelExpr;
    QString mLabel;
    QBasicTimer mSelectionSnapTimer;

    int mSelectionStart;
    int mSelectionEnd;
    bool mDefaultBehaviour;
};

#endif // MSGUNIFIEDEDITORLINEEDIT_H
