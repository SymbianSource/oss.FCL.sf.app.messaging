/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Extracts needed attributes from received EMN message.
*		
*
*/



#ifndef EMNXMLCONTENTHANDLER_H
#define EMNXMLCONTENTHANDLER_H

#include <xml/contenthandler.h>
#include <xml/wbxmlextensionhandler.h>
#include <AlwaysOnlineManagerCommon.h>

#include "EMNHandler.h" // For common EMN error codes

using namespace Xml;

/*
* This class extracts needed attributes from received EMN message, which can
* be either in XML or in WBXML form.
*
*  @since S60 v3.1
*/
class CEMNXMLContentHandler : 
    public CBase, 
    public MWbxmlExtensionHandler, 
    public Xml::MContentHandler
	{

    public:
        /**
        * Creates object from CEMNXMLContentHandler
        * @since S60 v3.1
        * @return, Constructed object
        */
        static CEMNXMLContentHandler* NewL( 
            TEMNElement& aElement, TBool aIsAscii );
            
        /**
        * Creates object from CEMNXMLContentHandler and leaves it to 
        * cleanup stack
        * @since S60 v3.1
        * @return, Constructed object
        */
        static CEMNXMLContentHandler* NewLC( 
            TEMNElement& aElement, TBool aIsAscii );
        
        /**
        * Destructor
        * @since S60 v3.1
        */
        virtual ~CEMNXMLContentHandler();
        
        /**
        * Symbian 2-phase constructor
        * @since S60 v3.1
        */
        void ConstructL();
    
    public: // Following public functions are all from MContentHandler
    	
    	void OnStartDocumentL(
    	    const RDocumentParameters& aDocParam, TInt aErrorCode );
    	    
    	void OnEndDocumentL( TInt aErrorCode );
    	
    	void OnStartElementL(
    	    const RTagInfo& aElement, 
    	    const RAttributeArray& aAttributes,
    	    TInt aErrorCode );
    	    
    	void OnEndElementL( const RTagInfo& aElement, TInt aErrorCode );
    	
    	void OnContentL( const TDesC8& aBytes, TInt aErrorCode );
    	
    	void OnStartPrefixMappingL( 
    	    const RString& aPrefix, 
    	    const RString& aUri, 
    	    TInt aErrorCode );
    	    
    	void OnEndPrefixMappingL(
    	    const RString& aPrefix, 
    	    TInt aErrorCode );
    	    
    	void OnIgnorableWhiteSpaceL(
    	    const TDesC8& aBytes, 
    	    TInt aErrorCode );
    	    
    	void OnSkippedEntityL(
    	    const RString& aName, 
    	    TInt aErrorCode );
    	    
    	void OnProcessingInstructionL(
    	    const TDesC8& aTarget, 
    	    const TDesC8& aData, 
    	    TInt aErrorCode );
    	    
    	void OnError( TInt aErrorCode );
    	
    	TAny* GetExtendedInterface( const TInt32 aUid );

    public: // From MWbxmlExtensionHandler
	    
	    void OnExtensionL( 
	        const RString& aData, 
	        TInt aToken, 
	        TInt aErrorCode );
	    
    private:
        /**
        * Default constructor for class CEMNXMLContentHandler
        * @since S60 v3.1
        * @return, Constructed object
        */
    	CEMNXMLContentHandler( TEMNElement& aElement, TBool aIsAscii );
    	
    	/**
    	* Extracts given attribute from XML message.
    	* @param aAttributeName8 name of the attribute (8-bit)
    	* @param aAttributeValue8 value of the attribute (8-bit)
    	* @since S60 v3.1
    	*/
    	void HandleXMLAttributesL( 
    	    const TDesC8& aAttributeName8, const TDesC8& aAttributeValue8 );

    	/**
    	* Extracts given attribute from WBXML message.
    	* @param aAttributeName8 name of the attribute (8-bit)
    	* @param aAttributeValue8 value of the attribute (8-bit)
    	* @since S60 v3.1
    	*/
    	void HandleWBXMLAttributesL( 
    	    const TDesC8& aAttributeName8, const TDesC8& aAttributeValue8 );

    private:
        // Contains mailbox and timestamp
        TEMNElement&    iElement;
        // Is message XML or WBXML
        TBool           iIsAscii;
        // Mandatory attribute: mailbox
        TBool           iFoundMailboxAttribute;
    };
    
#endif  // EMNXMLCONTENTHANDLER_H
