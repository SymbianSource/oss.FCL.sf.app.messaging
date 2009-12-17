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
* Description:  xhtmlfontspecs  declaration
*
*/



#ifndef XHTMLFONTSPECS_H
#define XHTMLFONTSPECS_H

#include <gdi.h>

/**
 *  CXhtmlFontSpecs class is used to encapsulate font-related information 
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CXhtmlFontSpecs ) : public CBase
    {
public:
		
	/**
    * Constructor.
    */
    CXhtmlFontSpecs();
     
	/**
    * Destructor.
    */
    virtual ~CXhtmlFontSpecs();	
				
	/**
     * Sets fonts used when constructing the richtext object.
     *
     * @since S60 v3.2
     * @param aBigFont Big font
     * @param aDefaultFont Default font
     * @param aSmallFont Small font
     * @param aCourierFont Courier font
     */
    void SetFonts( const CFont* aBigFont,
	               const CFont* aDefaultFont,
	               const CFont* aSmallFont,
	               const CFont* aCourierFont );	
	/**
     * Returns default font.
     *
     * @since S60 v3.2
     * @return Default font
     */	
	inline const CFont* DefaultFont() { return iDefaultFont; };

    /**
     * Returns courier font.
     *
     * @since S60 v3.2
     * @return Courier font
     */
    inline const CFont* CourierFont() { return iCourierFont; };
	
    /**
     * Returns small font.
     *
     * @since S60 v3.2
     * @return Small font
     */
    inline const CFont* SmallFont() { return iSmallFont; };
    
    /**
     * Returns big font.
     *
     * @since S60 v3.2
     * @return Big font
     */
    inline const CFont* BigFont() { return iBigFont; };
	  
	 /**
     * Returns font used with <h1> tag.
     *
     * @since S60 v3.2
     * @return Font used with <h1> tag
     */
    inline const CFont* H1Font() { return iBigFont; };

    /**
     * Returns font used with <h2> tag.
     *
     * @since S60 v3.2
     * @return Font used with <h2> tag
     */
    inline const CFont* H2Font() { return iBigFont; };
    
    /**
     * Returns font used with <h3> tag.
     *
     * @since S60 v3.2
     * @return Font used with <h3> tag
     */
    inline const CFont* H3Font() { return iDefaultFont; };
    
    /**
     * Returns font used with <h4> tag.
     *
     * @since S60 v3.2
     * @return Font used with <h4> tag
     */
    inline const CFont* H4Font() { return iDefaultFont; };
    
    /**
     * Returns font used with <h5> tag.
     *
     * @since S60 v3.2
     * @return Font used with <h5> tag
     */
    inline const CFont* H5Font() { return iSmallFont; };
    
    /**
     * Returns font used with <h6> tag.
     *
     * @since S60 v3.2
     * @return Font used with <h6> tag
     */
    inline const CFont* H6Font() { return iSmallFont; };
	

private: // data
		
    /**
     * Small font
     * Not own.
     */
    const CFont* iSmallFont;
    
    /**
     * Default font
     * Not own.
     */
	const CFont* iDefaultFont;
	
	/**
     * Big font
     * Not own.
     */
	const CFont* iBigFont;
		
    /**
     * Courier font
     * Not own.
     */
	const CFont* iCourierFont;
	};

#endif



