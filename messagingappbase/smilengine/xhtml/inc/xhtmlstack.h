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
* Description:  xhtmlstack  declaration
*
*/



#ifndef XHTMLSTACK_H
#define XHTMLSTACK_H

#include <e32base.h>
#include <txtfrmat.h>

/**
 *  Class defining a style info. 
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 v3.2
 */ 
NONSHARABLE_CLASS( TXhtmlStyleInfo )
	{
public:
	
	/**
     * Constructor.
     *
     * @since S60 v3.2
     * @param aStyleStart Start position of the style
     * @param aPrevAlign Previous alignment
     * @return ?description
     */	
	TXhtmlStyleInfo( TInt aStyleStart, CParaFormat::TAlignment aPrevAlign );
	
	/**
     * Start position of the style
     */
	TInt  iStyleStart;
	
	/**
     * True if current style has been changed.
     * This is set to true in CXhtmlparser::ChangeCurrentStyleL
     */
	TBool iStyleChanged;
	
	/**
     * True if alignment has been changed.
     */
	TBool iAlignmentChanged;
	
	/**
     * Stores previous alignment.
     */
	CParaFormat::TAlignment iPrevAlign;
	};

/**
 *  Class defining a paragraph info.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 v3.2
 */ 
NONSHARABLE_CLASS( TXhtmlParaInfo )
	{
public:

    /**
     * Constructor.
     *
     * @since S60 v3.2
     * @param aParaStart Start position of the paragraph
     * @param aAlignment Alignment
     * @return ?description
     */	
    TXhtmlParaInfo( TInt aParaStart, CParaFormat::TAlignment aAlignment );
    
    /**
     * Start position of the paragraph
     */
    TInt iParaStart;
    
    /**
     * Alignment
     */
    CParaFormat::TAlignment iAlignment;
    };

/**
 *  Class defining a list info.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 v3.2
 */ 
NONSHARABLE_CLASS( TXhtmlListInfo )
	{
public:
    
    /**
     * Constructor.
     *
     * @since S60 v3.2
     * @param aListContext List context, ordered or unordered
     * @return ?description
     */	
    TXhtmlListInfo( TInt aListContext );
    
    /**
     * List index, tells the number of list items in current list
     */
    TInt iListIndex;
    
    /**
     * List context, ordered or unordered
     */
    TInt iListContext;
    };


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
template <typename T>
NONSHARABLE_CLASS( CXhtmlStack ) : public CBase
    {	
public:

    /**
     * CXhtmlStack phase one constructor
     *
     * @since S60 v3.2
     * @return Created CXhtmlStack instance
     */
    static inline CXhtmlStack* NewL();
    
    /**
    * Destructor.
    */
    virtual inline ~CXhtmlStack();
    
    /**
     * Removes item from top of the stack.
     *
     * @since S60 v3.2
     */ 
    inline void Pop();
	
	/**
     * Adds item to top of the stack.
     *
     * @since S60 v3.2
     * @param aInfo Pointer to item
     */ 
	inline void PushL( T* aInfo );
	
	/**
     * Returns item on top of the stack.
     *
     * @since S60 v3.2
     * @return Pointer to item
     */ 
	inline T* Top();
	
	/**
     * Returns number of items on the stack.
     *
     * @since S60 v3.2
     * @return Number of items
     */ 
	inline TInt Count() { return iStack->Count(); };	
	
protected:
	void ConstructL();
	CXhtmlStack();
		
private:
    /**
     * Array of pointers to stack items.
     * Owns
     */
    RPointerArray<T>* iStack;		
    };


#include "xhtmlstack.inl"

#endif

