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
* Description:  xhtmlparser  declaration
*
*/



#ifndef XHTMLPARSER_H
#define XHTMLPARSER_H

#include <e32base.h>
#include <s32strm.h>
#include <txtrich.h>
#include <gmxmlparser.h>

class CXhtmlStackManager;
class CXhtmlEntityConverter;
class CMDXMLNode;
class CXhtmlFontSpecs;
class CXhtmlHyperLink;


/**
 *  Abstract observer interface for notification when XHTML parsing is complete.
 *  It should be implemented by users of CXhtmlParser. 
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( MXhtmlParserObserver )
	{
public:

    /**
     * Call back function used to inform a client of the Parser
     * when a parsing operation completes.
     *
     * @since S60 v3.2
     */
	virtual void ParseCompleteL() = 0;
	
	/**
     * Call back function used to inform a client of the Parser
     * about error.
     *
     * @since S60 v3.2
     */
	virtual void ParseError( TInt aError ) = 0;
	};

/**
 *	This class forms the basis for the parser.
 *  It has the call back methods that the parser uses to notify
 *  parser events. These methods then adds the apppropriate text &
 *  formatting to the richtext object.
 *  The xhtml renderer owns this richtext object.  
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 v3.2
 */ 
NONSHARABLE_CLASS( CXhtmlParser ) : public CBase, public MMDXMLParserObserver
	{	
public:

    /**
     * Parser phase one constructor
     *
     * @since S60 v3.2
     * @param aParserObserver Observer for notification when XHTML parsing is complete
     * @return Created CXhtmlParser instance
     */	
	IMPORT_C static CXhtmlParser* NewL( MXhtmlParserObserver* aParserObserver );	

    /**
    * Destructor.
    */
    virtual ~CXhtmlParser();
	
    /**
     * Creates a DOM structure from a given XHTML file.
     *
     * @since S60 v3.2
     * @param aRFs -
     * @param aFileToParse -
     */	
	IMPORT_C void CreateDomL( RFs &aRFs, const TDesC& aFileToParse );
				
    /**
     * Creates a DOM structure from a given XHTML file.
     *
     * @since S60 v3.2
     * @param aFileHandleToParse File handle to XHTML file to parse
     */	
	IMPORT_C void CreateDomL( RFile& aFileHandleToParse );
	
	/**
     * Constructs the richtext object from DOM structure.
     *
     * @since S60 v3.2
     * @param aRichText Richtext object where text, style and format is added based on parsed result
     */	
	IMPORT_C void ParseL( CRichText& aRichText );

	/**
     * Cancels the operation.
     *
     * @since S60 v3.2
     */	
    IMPORT_C void Cancel();


    /**
     * Sets fonts used when constructing the richtext object.
     * 
     * @since S60 v3.2
     * @param aBigFont Big font
     * @param aDefaultFont Default font
     * @param aSmallFont Small font
     * @param aCourierFont Courier font
     */	
     
	IMPORT_C void SetFonts( const CFont* aBigFont,
	                        const CFont* aDefaultFont,
	                        const CFont* aSmallFont,
	                        const CFont* aCourierFont );

    /**
     * Sets text color of default font.
     *
     * @since S60 v3.2
     * @param aColor Color
     */	
	IMPORT_C void SetDefaultTextColor( TRgb aColor );
	
	/**
     * Sets parsing mode.
     *
     * @since S60 v3.2
     * @param aPlainText If true, plain text mode is used (no style and format added) 
     * @param aShowUrls 
     */	
	IMPORT_C void SetMode( TBool aPlainText, TBool aShowUrls );

    /**
     * Gets information (start position, end position and target URL)
     * of hyperlink by hyperlink index. Index starts from 0. 
     *
     * @since S60 v3.2
     * @param aIndex Index of requested hyperlink
     * @param aStartPos Start position of hyperlink text. Filled by this function
     * @param aEndPos End position of hyperlink text. Filled by this function
     * @param aAddress target URL of hyperlink. Filled by this function
     * @return KErrNone if completed successfully, KErrArgument if index is out of bounds.
     */	
	IMPORT_C TInt HyperLink( TInt aIndex, TInt& aStartPos, TInt& aEndPos, TPtrC& aAddress );
     
    /**
     * Returns a number of found hyperlinks. 
     *
     * @since S60 v3.2
     * @return Number of found hyperlinks
     */	
	IMPORT_C TInt HyperLinkCount();
	
	
public: // inlines

    /**
     * Sets a new limit for nested lists. Default value is 5. 
     *
     * @param Limit value
     */
    inline void SetLimitForNestedLists( TInt aLimit );    

    /**
     * Sets a new width for indent. Default width is width of 4 space characters. 
     *
     * @param Number of space characters used to calculate width of indent
     */
    inline void SetIndentWidth( TInt aCharacters );    


    
protected:

// from base class MMDXMLParserObserver


    /**
     * From MMDXMLParserObserver.
     * XML parser calls this when parsing is completed (DOM is filled).
     * XHTML parser uses this DOM to perform its own parsing.
     *
     * @since S60 v3.2
     */
    void ParseFileCompleteL();
    
				
private:

	CXhtmlParser( MXhtmlParserObserver* aParserObserver );
	void ConstructL( );
	
    /**
     * Performs XHTML parsing using XML document filled by XML parser.
     *
     * @since S60 v3.2
     * @param aXmlDocument Given XML document
     */	
	void ParseDomL( CMDXMLDocument* aXmlDocument );

    /**
     * Handles a (begin) node in DOM document.
     * Calls corresponding handler based on node type (Element, Text, Comment etc.) 
     *
     * @since S60 v3.2
     * @param aNode Node to be handled.
     * @return True if completed successfully
     */	
	void OpenNodeL( CMDXMLNode* aNode );

    /**
     * 
     * Handles a (end) node in DOM document.
     *
     * @since S60 v3.2
     * @param aNode Node to be handled.
     */	
	void CloseNodeL( CMDXMLNode* aNode );

    /**
     * Performs some preparations before handling of begin tag.
     *
     * @since S60 v3.2
     */	
	void PrepareForBeginElementL( const TDesC& aName );

    /**
     * Performs needed operations for each begin tag. (e.g. for <div>, <p> etc.).
     * Contains main funtionality of parsing logic. (possible attributes are not
     * handled here, but in AttributevalueL function)
     *
     * @since S60 v3.2
     * @param aName Name of the tag (without angle brackets)
     */	
	void BeginElementL( const TDesC& aName );

    /**
     * Performs needed operations for each end tag. (e.g. for  </p>, </div> etc.).
     * Contains main funtionality of parsing logic.
     *
     * @since S60 v3.2
     * @param aName Name of the tag (without angle brackets)
     */	
	void EndElementL( const TDesC& aName );

   /**
     * Begins a new style. 
     * 
     * Creates a new StyleInfo item and adds it to the stack. This info is used later when
     * a new style is applied to richtext and it contains e.g. the start position of text to
     * be changed by a new style.
     *
     * @since S60 v3.2
     */	
	void BeginStyleL();

    /**
     * Ends a style
     *
     * Applies a new style to richtext object and removes the StyleInfo item from stack.
     * Note that a new style is applied only if style is changed by function ChangeCurrentStyleL.
     * In this case also current CharFormatLayer (created in ChangeCurrentStyleL) is removed by
     * calling CleanCharLayer.
     *
     * @since S60 v3.2
     */	
	void EndStyleL();

    /**
     * Changes current style.
     *
     * Applies "old" style to the richtext object (from iStylePos to current position).
     * Creates a new CharFormatLayer with new style (based on old layer in iCurrentCharLayer)
     * and marks the previous StyleInfo item in stack to be changed.
     * So, because item is marked to be changed, this new style is recognized in EndStyleL and
     * applied to the richtext object.  
     *
     * @since S60 v3.2
     * @param charFormat ?description
     * @param charFormatMask ?description
     */	
	void ChangeCurrentStyleL( TCharFormat charFormat, TCharFormatMask charFormatMask );

    /**
     * Applies current style to richtext object (from iStylePos to end of document)
     *
     * @since S60 v3.2
     */	
	void ApplyStyleL();

    /**
     * Begins a new paragraph. 
     *
     * @since S60 v3.2
     * @param aAlignment Alignment
     */	
	void BeginParagraphL( CParaFormat::TAlignment aAlignment );

    /**
     * Ends a paragraph.
     *
     * @since S60 v3.2
     */	
	void EndParagraphL();

    /**
     * Applies a paragraph format to richtext object.
     *
     * @since S60 v3.2
     * @param aParaStart Start position of the paragraph
     * @param aAlignment Alignment
     */	
	void ApplyParagraphL( TInt aParaStart, CParaFormat::TAlignment aAlignment );

    /**
     * Begins a new list (with <ol> or <ul> tag.)
     * Begins a new paragraphs and creates a new ListInfo item to stack to store list information.
     * 
     * @since S60 v3.2
     * @param aListContext Type of the current list, ordered or unordered
     */	
	void BeginListL( TInt aListContext );

    /**
     * Ends a list.
     * Ends paragraphs and removes ListInfo item from stack.
     *
     * NOTE: TELL HERE MORE ABOUT FUNCTIONALITY!!!!!!!!!!!!!!
     *
     * @since S60 v3.2
     */	
	void EndListL();

    /**
     * NOTE: TELL HERE MORE ABOUT FUNCTIONALITY!!!!!!!!!!!!!!
     *
     * @since S60 v3.2
     * @return ?description
     */	
	void BeginListItemL();

    /**
     * Stores the start position of hyperlink to be used later.
     * This function is called when <a> tag is found.
     *
     * @since S60 v3.2
     * @param aBeginPosition Start position of the hyperlink
     */	
	void BeginHyperLink( TInt aBeginPosition );

    /**
     * Creates a new hyperlink item and adds it to hyperlink array.
     * This item contains begin and end position of hyperlink text and
     * target URL of the link.
     * User is later able to get these hyperlinks by function HyperLink.
     *
     * This function is called when </a> tag is found.
     *
     * @since S60 v3.2
     * @param aEndPosition End position of the hyperlink
     */	
	void EndHyperLinkL( TInt aEndPosition );

    /**
     * This function adds the actual text to richtext object.
     * Text can be added as such (if preformatting is in use) or
     * so that extra whitespaces are removed (preformatting not in use)
     *
     * @since S60 v3.2
     * @param aData Text to be added to richtext object
     */	
	void ContentL( const TDesC& aData );

    /**
     * Removes extra whitespace characters from a text. 
     *
     * @since S60 v3.2
     * @param aData Text to be collapsed down 
     */	
	void CollapseWhiteSpaces( TDes& aData );

    /**
     * Handles attributes in begin tags.
     * Only a few attributes are supported. 
     *
     * @since S60 v3.2
     * @param aName Name of the attribute
     * @param aValue Value of the attribute
     */	
	void AttributeValueL( const TDesC& aName, const TDesC& aValue );

    /**
     * Sets target URL of current hyperlink.
     *
     * @since S60 v3.2
     * @param aUrl Target URL of hyperlink
     */	
	void SetReferenceL( const TDesC& aUrl );

    /**
     * Removes all space characters from the string. 
     *
     * @since S60 v3.2
     * @param aString Text to be handled
     */	
	void RemoveAllSpace( TDes& aString );

    /**
     * Splits passed attribute string to name and value.
     *
     * @since S60 v3.2
     * @param aString Attribute string
     * @param aAttributeName Attribute name
     * @param aAttributeValue Attribute value
     */	
	void SplitAttribute( TPtrC& aString, TPtrC& aAttributeName, 
		                    TPtrC& aAttributeValue );
      
    /**
     * Parses color value from given string and creates TRgb object based on it.
     * Supports both numberic format and name format (e.g. red or #ff0000) 
     *
     * @since S60 v3.2
     * @param aString String that contains the color value
     * @param aTransparent The function sets this true if color is transparent
     * @return TRgb object based on given color
     */	
	TRgb ParseColor( const TDesC& aString, TBool& aTransparent );
	
    /**
     * CDataL
     *
     * @since S60 v3.2
     * @param aData Data
     */	
	void CdataL( const TDesC& aData );

    /**
     * Cleans current CharFormatLayer and sets previous layer to active.
     * This is used to return to old style at the end of current style handling.
     *
     * @since S60 v3.2
     * @return Previous CharFormatLayer
     */	
	CCharFormatLayer* CleanCharLayer();

    /**
     * Converts between arabic-indic digits and european digits based on existing language setting.
     * So it'll convert any digit from the string
     * to use either european digits or arabic-indic digits based on current settings.
     *
     * @since S60 v3.2
     * @param aFieldString: Data buffer used in conversion.
     * @param @param aFieldData: Return converted data in this parameter.
     */	
	void DoNumberConversion( HBufC* aFieldString, TPtrC& aFieldData ) const;

    /**
     * Sets all internal variables to initial state. 
     *
     * @since S60 v3.2
     */	
	void ResetValues();
	
	/**
     * Inserts text to the RichText object. 
     *
     * @since S60 v3.2
     * @param aData Text to be written
     */	
	void InsertTextL( const TDesC& aText );
	
	/**
     * Inserts character to the RichText object. 
     *
     * @since S60 v3.2
     * @param aData Character to be written
     */	
    void InsertCharacterL( const TChar aText );
    
    /**
     * Appends a new paragraph to the RichText object. 
     *
     * @since S60 v3.2
     * @param aPlainText tells if plain text is used or not
     */	
    void AppendParagraphL( TBool aPlainText );
    
    /**
     * Inserts line break to the RichText object. 
     *
     * @since S60 v3.2
     */	
    void InsertLineBreak();
    
    /**
     * Inserts "forced" line break to the RichText object. 
     * This is called when <br> tag is found.
     *
     * @since S60 v3.2
     */	
    void InsertForcedLineBreak();
  
	                        
                           
private: // data
	
	/**
     * Tells if parsing is allowed or not. If not, no text is applied to RichText.
     */
	TBool iParsingAllowed;
	
	/**
     * If this is true, XHTML is parsed as plain text. 
     * In this case styles and paragraphs are not applied to rendered text.
     */
	TBool iPlainText;
	
	/**
     * If this is true, URL addresses are shown after links.  
     */
	TBool iShowUrls;
	
	/**
     * Tells if preformatted parsing is in use. (true between tags <pre> and </pre>).
     * When preformatted parsing is in use, text is added to RichText in same format
     * as it is written between tags <pre> and </pre>.
     * For example all whitespaces are added etc.
     * However, if there is another tags inside <pre> and </pre> tags, they are parsed
     * normally. (For example styles are changed normally: <p style="color:red">
     * changes the text color to red also in preformatted text.)
     *  
     */
	TInt iPreformatted;
	
	/**
     * Current text alignment (left, center or right). Used in paragraphs.
     * Changed by tag with corresponding style atribute, for example:
     * <p style="text-align:right">
     *
     */
	CParaFormat::TAlignment iAlignment;
	
	/**
     * Start position of the current style. Every time when a new style is added to
     * richtext object, it is applied from iStylePos to the end of document. After that,
     * iStylePos is set to point to the end of document. 
     */
	TInt iStylePos;
	
	/**
     * Tells if a new paragraph has already been added to richtext object AND
     * no characters are added after that. Used to avoid adding of empty paragraphs
     * For example. if there are several end tags without text
     * between them and each end tag creates a new paragraph, this generates unnecessary
     * paragraphs (also shown as extra lines in text) without handling of the situations with this variable.
     * So, if a new paragraph has already added and no text after that (=this variable is true),
     * new paragraphs are not added.  
     */
	TBool iNewParagraph;
	
	/**
     * iNewLines
     */
	TInt iNewLines;
	
	/**
     * iForcedNewLines
     */
	TInt iForcedNewLines;
	
	/**
     * iSkipWhiteSpaces 
     */
	TBool iSkipWhiteSpaces;
	
	/**
     * Tells is current alignment has changed by style attribute. (e.g. <p style="text-align:right">) 
     */
	TBool iAlignmentChanged;
	
    /**
     * Tells if tag in question is <img>. Alt attributes are handled only in this case. 
     */
	TBool iImageFound;
	
	/**
     * iFirstTextLineAdded 
     */
	TBool iFirstTextLineAdded;

	/**
     * Current list context, ordered or unordered list. Used to define how list items are rendered.
     * (in orderd list with item number and in unordered list with bullets)
     */
	TInt iCurrentListContext;

	/**
     * Index of current level of definition list, tells in which level we are now.
     * (There might be several nested definition lists, this is used to support for this). 
     * Used to define margins widths (with other same kind of variables) when a new
     * paragraph is created.
     */
	TInt iDefListLevel;

	/**
     * Index of current level of block quote, tells in which level we are now.
     * 
     * (In correct XHTML there may NOT be nested block quotes, but this is needed
     * also for one-level blocquote. Support also nested block-quotes although they are not allowed
     * in XHTML).
     * Used to define margins widths (with other same kind of variables) when a new
     * paragraph is created.
     */
	TInt iBlockQuoteLevel;

	/**
     * Stores target URL address of current hyperlink.
     */
	TInt iHyperLinkPos;
	
	/**
     * Default text color. Set by user by calling SetTextColor.
     */
	TRgb iTextColor;

	/**
     * Default margin width of ordered and unordered lists.
     * Set by user by calling SetMargins.
     */
	TInt iListMargin;

	/**
     * Default margin width of definition lists.
     * Set by user by calling SetMargins.
     */
	TInt iDefListMargin;

	/**
     * Default margin width of block quote.
     * Set by user by calling SetMargins.
     */
	TInt iBlockQuoteMargin;

	/**
     * Observer instance for notification when XHTML parsing is complete. 
     * Not own.
     */
	MXhtmlParserObserver* iObserver;
	
	/**
     * XML parser used to parse from XML file to DOM document.
     * Own.
     */
	CMDXMLParser* iXmlParser;
	
	/**
     * RichText object that is modified by XHTML parser.
     * XHTML parser creates a formatted text based on parsed XHTML and adds
     * the text to RichText object (with styles and paragraphs).
     * RichText is passed to parser in both ParseL functions.
     * The xhtml renderer owns this richtext object. 
     * Not own.
     */
    CRichText* iRichText;
    
    
    
    CMDXMLDocument* iXmlDocument;
	
	/**
     * Stackmanager for handling of stacks.
     * Own.
     */
	CXhtmlStackManager* iStackManager;	
	
	/**
     * Stores font-related information
     * Own.
     */
	CXhtmlFontSpecs* iXhtmlFontSpecs;
	
	/**
     * current CharFormatLayer that is based on GlobalCharFormatLayer
     * of Richtext object. New layers are added in function ChangeCurrentStyle
     * and removed in function CleanCharlayer. This variable tells the "style" that
     * is applied to text (e.g. color, fontsize etc.) 
     * Own.
     */
	CCharFormatLayer* iCurrentCharLayer;
	
	/**
     * Array of hyperlink information. Contains information of all found hyperlinks.
     * Own.
     */
	CArrayPtrFlat<CXhtmlHyperLink>* iHyperLinkArray;
	
	/**
     * Stores the value of current "href" attribute of <a> tag.
     * In practice the value contains target URL of the link.
     * Not own.
     */
	HBufC* iHyperLinkAddress;
	
	/**
     * The number of space characters used to calculate the width of indent.
     * Default value is 4 (space characters).
     */
	TInt iIndentWidth;
	
	/**
     * Maximum number of nested list levels.
     * Default value is 5 
     */
	TInt iMaxListLevels;
	};
	
#include <xhtmlparser.inl>

#endif
