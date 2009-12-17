/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  xhtmlstackmanager  declaration
*
*/



#ifndef XHTMLSTACKMANAGER_H
#define XHTMLSTACKMANAGER_H

#include <e32base.h>

class TXhtmlStyleInfo;
class TXhtmlParaInfo;
class TXhtmlListInfo;

/**
 *  Class defining a stack. Methods implemented in XhtmlStack.inl.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 v3.2
 */ 
NONSHARABLE_CLASS( CXhtmlStackManager ) : public CBase
    {	
public:

    /**
     * CXhtmlStackManager phase one constructor
     *
     * @since S60 v3.2
     * @return Created CXhtmlStack instance
     */
    static CXhtmlStackManager* NewL();
    
    /**
    * Destructor.
    */
    virtual ~CXhtmlStackManager();
    
    /**
     * Returns pointer to TXhtmlParaInfo stack.
     *
     * @since S60 v3.2
     */ 
    inline CXhtmlStack<TXhtmlParaInfo>* ParaStack() 
        {
        return iParaStack;
        };
    
    /**
     * Returns pointer to TXhtmlStyleInfo stack.
     *
     * @since S60 v3.2
     */ 
    inline CXhtmlStack<TXhtmlStyleInfo>* StyleStack()
        {
        return iStyleStack;
        }
    
    /**
     * Returns pointer to TXhtmlListInfo stack.
     *
     * @since S60 v3.2
     */ 
    inline CXhtmlStack<TXhtmlListInfo>* ListStack()
        {
        return iListStack;
        }
	
	
protected:
	void ConstructL();
	CXhtmlStackManager();
		
private:
    
    /**
     * Stack for paragraph information items.
     * Own.
     */
    CXhtmlStack<TXhtmlParaInfo>* iParaStack;
	
	/**
     * Stack for style information items.
     * Own.
     */
    CXhtmlStack<TXhtmlStyleInfo>* iStyleStack;
	
	/**
     * Stack for list information items.
     * Own.
     */
	CXhtmlStack<TXhtmlListInfo>* iListStack;
    };

#endif

