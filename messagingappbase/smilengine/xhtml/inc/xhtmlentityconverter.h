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
* Description:  xhtmlentityconverter  declaration
*
*/



#ifndef XHTML_ENTITY_CONVERTER_H
#define XHTML_ENTITY_CONVERTER_H

#include <gmxmldocument.h>
#include <gmxmlentityconverter.h>


/**
 *  Specific Converter for XHTML entities
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CXhtmlEntityConverter ) : public CMDXMLEntityConverter
{

public:
   
     
// from base class CMDXMLEntityConverter

    /**
     * From CMDXMLEntityConverter.
     *
     * OutputComposedTextL
     *
     * @since S60 v3.2
     * @param aComposer Composer
     * @param aTextToConvert Text to convert
     */
	TInt OutputComposedTextL( CMDXMLComposer* aComposer, const TDesC& aTextToConvert );


protected:

    /**
     * From CMDXMLEntityConverter.
     *
     * DTDEntityToText
     *
     * @since S60 v3.2
     * @param aTextToConvert Text to convert
     */
	virtual TInt DTDEntityToText( TDes& aTextToConvert );

private:

    /**
     * 
     *
     * @since S60 v3.2
     * @param aData ?description
     * @param aStart ?description
     * @param aLength ?description
     * @return ?description
     */	
	TBool HandleEntity( TDes& aData, TInt aStart, TInt aLength );
	
	/**
     * 
     *
     * @since S60 v3.2
     * @param aEntity ?description
     * @param aEntityCode ?description
     * @return ?description
     */	
	TBool MatchEntityCode( const TDesC& aEntity, TDes& aEntityCode );
	
	/**
     * 
     *
     * @since S60 v3.2
     * @param aEntityCode ?description
     * @return ?description
     */	
	TPtrC MatchEntityString( const TDesC& aEntityCode ) const;
};


#endif

