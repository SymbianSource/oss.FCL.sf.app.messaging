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

#ifndef T_MSGCONVERSATIONWIDGET_HEADERS_H
#define T_MSGCONVERSATIONWIDGET_HEADERS_H

#include <QGraphicsWidget>

#include "convergedmessage.h"
#include "debugtraces.h"

#define QCoreApplication QCoreApplication2

/*
    Mocked HbEvent class.
 */
class HbEvent : public QObject
{
    Q_OBJECT
public:
	  enum Flags{
	  	ThemeChanged
	  	};
	  	
    HbEvent(HbEvent::Flags flag)
    {
    }
    ~HbEvent()
    {        
    }
        
private:
    Q_DISABLE_COPY(HbEvent)   
};

/*
    Mocked QCoreApplication class.
 */
 class MSG;
 
class QCoreApplication2 : public QObject
{
    Q_OBJECT
public:
	enum EventFilter
	{
		CodecForTr, UnicodeUTF8, DefaultCodec 
	};
		
    QCoreApplication ( int & argc, char ** argv )  {}
    ~QCoreApplication ()  {}
    bool filterEvent ( void * message, long * result ) {}
    virtual bool notify ( QObject * receiver, QEvent * event ) {}
    EventFilter setEventFilter ( EventFilter filter ) {}
    virtual bool winEventFilter ( MSG * msg, long * result ) {}
    static void postEvent(QObject* ref, HbEvent* ob) {}

private:
    Q_DISABLE_COPY(QCoreApplication)   
};

/*
    Mocked Hb class.
 */
class Hb : public QObject
{
    Q_OBJECT
public:
	enum Flags{
		TextCursorHidden
		
	};
	
    Hb(QObject *parent = NULL)
    {
    }
    ~Hb()
    {        
    }
        
private:
    Q_DISABLE_COPY(Hb)   
};

/*
    Mocked HbIcon class.
 */
class HbIcon
{
  
public:
  
    HbIcon (const QString &iconName)  {}
    HbIcon (const QIcon &icon) {}
    HbIcon (const HbIcon &other) {}

    QSizeF size() const {}
private:
    //Q_DISABLE_COPY(HbIcon)
};

/*
    Mocked HbIconItem class.
 */

class HbIconItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    HbIconItem(QGraphicsWidget *parent = NULL)
    {
    }
    ~HbIconItem()
    {        
    }
        void setAlignment(Qt::Alignment alignment) {}
        void setIcon(const HbIcon &icon) {}
        void setPreferredSize(QSizeF size) {}
        void show(){}
        void setIconName(const QString& name) {}
private:
    Q_DISABLE_COPY(HbIconItem)
private:    
};

/*
    Mocked HbTextItem class.
 */
class HbTextItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    HbTextItem(QGraphicsWidget *parent = NULL)
    {
    }
    ~HbTextItem()
    {        
    }
    void setText(const QString &timeStamp){}
    void show(){}
private:
    Q_DISABLE_COPY(HbTextItem)
private:    
};

/*
    Mocked HbIconItem class.
 */
extern int hbtextedit_setsmileysenabled_callcount;
extern int hbtextedit_setreadonly_callcount;
 
class HbTextEdit : public QGraphicsWidget
{
    Q_OBJECT
public:
    HbTextEdit(QGraphicsWidget *parent = NULL)
    {
    }
    ~HbTextEdit()
    {        
    }
        void setReadOnly(bool status) {
        	hbtextedit_setreadonly_callcount++;
        	}
        void setBackgroundItem(int back) {}
        void setSmileysEnabled(bool status) {
        	hbtextedit_setsmileysenabled_callcount++;
        	}
        void setFlag(QGraphicsWidget::GraphicsItemFlag flag,bool status) {}
        void setPlainText(QString text) {}
        void show() {}
        void setCursorVisibility(Hb::Flags flag) {}
private:
    Q_DISABLE_COPY(HbTextEdit)
private:    
};

/*
    Mocked HbStyle class.
 */
extern int hbstyle_setitemname_callcount;

class HbStyle: public QObject
{
    Q_OBJECT
public:
	enum{
		P_None
	};
    HbStyle(QObject *parent = NULL)
    {
    }
    
    void static setItemName(QGraphicsWidget* bubbleFrameItem, QString type)
    {
    	hbstyle_setitemname_callcount++;
    }
    
private:
    Q_DISABLE_COPY(HbStyle)
};

/*
    Mocked HbFrameDrawer class.
 */
 extern int hbframedrawer_setframetype_callcount;
 
class HbFrameDrawer: public QObject
{
    Q_OBJECT
public:
		enum PieceSize{
		OnePieces,
		TwoPieces,
		ThreePiecesVertical,
		FourPieces,
		NinePieces
	};
    HbFrameDrawer(QObject *parent = NULL)
    {
    }
    void setFrameType(HbFrameDrawer::PieceSize piece)
    	{
    		hbframedrawer_setframetype_callcount++;
    	}
    	
    	void setFrameGraphicsName(const QString name) {}
    	
private:
    Q_DISABLE_COPY(HbFrameDrawer)
};

/*
    Mocked HbFrameItem class.
 */
extern int hbframeitem_framedrawer_callcount;
 
class HbFrameItem : public QGraphicsWidget
{
    Q_OBJECT
    
public:
	
    HbFrameItem(QGraphicsWidget *parent = NULL)
    {
    }
    ~HbFrameItem()
    {        
    }
    
HbFrameDrawer& frameDrawer  ( )  
{
	hbframeitem_framedrawer_callcount++;
	return frameDraw;
}
    
private:
    Q_DISABLE_COPY(HbFrameItem)
private:
    HbFrameDrawer frameDraw;    
};

/*
    Include the header file of the class under test.
 */
#include "../../../inc/msgconversationwidget.h"

#endif /* T_MSGCONVERSATIONWIDGET_HEADERS_H */
