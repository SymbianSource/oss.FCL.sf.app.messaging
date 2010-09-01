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
* Description:  xhtmlhyperlink  declaration
*
*/



#ifndef XHTMLHYPERLINK_H
#define XHTMLHYPERLINK_H

#include <e32base.h>

/**
 *  CXhtmlHyperLink class encapsulates hypertext-related information 
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CXhtmlHyperLink ) : public CBase
    {
public:

    /**
    * Constructor.
    */
    CXhtmlHyperLink(TInt aStartPosition, TInt aEndPosition);
    
    /**
    * Destructor.
    */
    virtual ~CXhtmlHyperLink();
    
    /**
     * Sets target URL of the link.
     *
     * @since S60 v3.2
     * @param aAddress target URL of the link
     */      
    inline void SetAddressL(TPtrC &aAddress) { iAddress = aAddress.AllocL(); };
    
    /**
     * Returns start position of the link text.
     *
     * @since S60 v3.2
     * @return Start position of the link
     */
    inline TInt StartPosition() { return iStartPosition; };
    
    /**
     * Returns end position of the link text.
     *
     * @since S60 v3.2
     * @return End position of the link
     */
    inline TInt EndPosition() { return iEndPosition; };
    
    /**
     * Returns target URL of the link. 
     *
     * @since S60 v3.2
     * @return Target URL of the link
     */
    inline HBufC* Address() { return iAddress; };
    
private: // data

    /**
     * Start position of the link text.
     */
	TInt iStartPosition;
    
    /**
     * End position of the link text.
     */
    TInt iEndPosition;
    
    /**
    * Target URL of the link.
    */
    HBufC* iAddress;
    };
	
#endif
