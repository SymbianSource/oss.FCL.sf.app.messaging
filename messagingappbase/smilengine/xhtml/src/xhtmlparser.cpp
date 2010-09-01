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
* Description:   Provides CXhtmlParser class methods.
*
*/



#include <coemain.h>
#include <gulfont.h> 
#include <eikenv.h> 
#include <txtfrmat.h>
#include <txtetext.h>
#include <frmconst.h>
#include <e32math.h>
#include <gdi.h>
#include <AknUtils.h>
#include <fontids.hrh>
#include <gmxmldocument.h>
#include <gmxmlnode.h>
#include <gmxmlelement.h>
#include <gmxmltext.h>
#include <smilliterals.h>

#include "Xhtmldef.h"
#include "xhtmlentityconverter.h"
#include "xhtmlfontspecs.h"
#include "xhtmlstack.h"
#include "xhtmlstackmanager.h"
#include "xhtmlhyperlink.h"
#include "xhtmlparser.h"
#include "xhtmlparserlogging.h"

const TInt KNoListContext        = 0;
const TInt KUnorderedListContext = 1;
const TInt KOrderedListContext   = 2;
const TInt KNoStylePosition      = -1;

const TInt KListBullet   = 0x2022;
const TInt KListBullet2  = 0x25E6;
const TInt KPlainBullet  = 0x2d;  // '-'
const TInt KPlainBullet2 = 0x2a;  // '*'
const TInt KColon        = 0x3a;  // ':'
const TInt KSemicolon    = 0x3b;  // ';' 

const TInt KBlackColorCode = 0x000000;
const TInt KWhiteColorCode = 0xffffff;
const TInt KBlueColorCode  = 0xff0000;
const TInt KLimeColorCode  = 0x00ff00;
const TInt KRedColorCode   = 0x0000ff;

const TInt KDefaultIndentWidth  = 4; // 4 spaces
const TInt KDefaultMaxListLevels = 5;

_LIT( KWhiteSpace, " " );
_LIT( KHrSeparator, "__________" );
_LIT16(KListNumberFormat, "%d. ");
_LIT( KNullString, "" );

const TInt KHyperLinkArrayGranularity = 16;
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
const TText KZeroWidthSpace         = 0x200B;
#endif

// ---------------------------------------------------------
// CXhtmlParser::NewL
// ---------------------------------------------------------
//
EXPORT_C CXhtmlParser* CXhtmlParser::NewL( MXhtmlParserObserver* aParserObserver )
	{
	CXhtmlParser* self = new( ELeave ) CXhtmlParser( aParserObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self ); 
	return self;
	};

// ---------------------------------------------------------
// CXhtmlParser::~CXhtmlParser 
// ---------------------------------------------------------
//
CXhtmlParser::~CXhtmlParser()
	{
	if( iCurrentCharLayer )
	    {
		iCurrentCharLayer->Reset();
		delete iCurrentCharLayer;
	    }

	delete iXmlParser;    
	delete iXhtmlFontSpecs;
	delete iHyperLinkAddress;
	delete iStackManager;
	delete iXmlDocument;
	
	if ( iHyperLinkArray )
	    {
	    iHyperLinkArray->ResetAndDestroy();
	    delete iHyperLinkArray;
	    }
	}

// ---------------------------------------------------------
// CXhtmlParser::CreateDomL 
// ---------------------------------------------------------
//
EXPORT_C void CXhtmlParser::CreateDomL( RFs &aRFs, const TDesC& aFileToParse )
	{	
	XHTMLLOG_WRITE( "CXhtmlParser::CreateDomL in" );
	User::LeaveIfError( iXmlParser->ParseFile( aRFs, aFileToParse ) );
	XHTMLLOG_WRITE( "CXhtmlParser::CreateDomL out" );
	}
	
// ---------------------------------------------------------
// CXhtmlParser::CreateDomL 
// ---------------------------------------------------------
//
EXPORT_C void CXhtmlParser::CreateDomL( RFile& aFileHandleToParse )
	{
	XHTMLLOG_WRITE( "CXhtmlParser::CreateDomL in" );
	User::LeaveIfError( iXmlParser->ParseFile( aFileHandleToParse ) );
	XHTMLLOG_WRITE( "CXhtmlParser::CreateDomL out" );
	}
	
// ---------------------------------------------------------
// CXhtmlParser::ParseL 
// ---------------------------------------------------------
//
EXPORT_C void CXhtmlParser::ParseL( CRichText& aRichText )
	{
	XHTMLLOG_WRITE( "CXhtmlParser::ParseL in" );
	iRichText = &aRichText;
   	
   	ResetValues();
   	ParseDomL( iXmlDocument);
	XHTMLLOG_WRITE( "CXhtmlParser::ParseL out" );
	}
	
// ---------------------------------------------------------
// CXhtmlParser::Cancel 
// ---------------------------------------------------------
//	
EXPORT_C void CXhtmlParser::Cancel()
    {
    iXmlParser->Cancel();
    }

// ---------------------------------------------------------
// CXhtmlParser::SetFonts 
// ---------------------------------------------------------
//
EXPORT_C void CXhtmlParser::SetFonts( const CFont* aBigFont,
	                                  const CFont* aDefaultFont,
	                                  const CFont* aSmallFont,
	                                  const CFont* aCourierFont )
    {
    iXhtmlFontSpecs->SetFonts( aBigFont, aDefaultFont, aSmallFont, aCourierFont );
    }

// ---------------------------------------------------------
// CXhtmlParser::SetDefaultTextColor 
// ---------------------------------------------------------
//   
EXPORT_C void CXhtmlParser::SetDefaultTextColor( TRgb aColor )
    {
    iTextColor = aColor;
    }
    
// ---------------------------------------------------------
// CXhtmlParser::SetMode
// 
// ---------------------------------------------------------
// 
EXPORT_C void CXhtmlParser::SetMode( TBool aPlainText, TBool aShowUrls )
    {
    iPlainText = aPlainText;
    iShowUrls = aShowUrls;
    }

// ---------------------------------------------------------
// CXhtmlParser::HyperLink 
// ---------------------------------------------------------
// 
EXPORT_C TInt CXhtmlParser::HyperLink( TInt aIndex, 
                        TInt& aStartPos, TInt& aEndPos,
                        TPtrC& aAddress )
    {
    if ( aIndex >= iHyperLinkArray->Count() )
        {
        return KErrArgument;
        }
        
    CXhtmlHyperLink* hpLink = (*iHyperLinkArray)[aIndex];
    aStartPos = hpLink->StartPosition();
    aEndPos = hpLink->EndPosition();
    
    if ( hpLink->Address() )
        {
        TPtr tmpPtr = hpLink->Address()->Des();
        aAddress.Set( tmpPtr );
        }
    else
        {
        aAddress.Set(KNullString);
        }
    return KErrNone;
    }
                
// ---------------------------------------------------------
// CXhtmlParser::HyperLinkCount 
// ---------------------------------------------------------
// 
EXPORT_C TInt CXhtmlParser::HyperLinkCount()
    {
    if ( iHyperLinkArray )
        {
        return iHyperLinkArray->Count();
        }
    return 0;        
    }
    
// ---------------------------------------------------------
// CXhtmlParser::ParseFileCompleteL
// ---------------------------------------------------------
//
void CXhtmlParser::ParseFileCompleteL()
    {
	XHTMLLOG_WRITE( "CXhtmlParser::ParseFileCompleteL in" );
    delete iXmlDocument;
    iXmlDocument = iXmlParser->DetachXMLDoc();
    
    TRAPD( error, iObserver->ParseCompleteL() );
    
    if ( error )
        {
        iObserver->ParseError( error );
        }
	XHTMLLOG_WRITE( "CXhtmlParser::ParseFileCompleteL out" );
    }
    
// ---------------------------------------------------------
// CXhtmlParser::CXhtmlParser
// ---------------------------------------------------------
//
CXhtmlParser::CXhtmlParser(  MXhtmlParserObserver* aParserObserver ) :
   	iParsingAllowed( EFalse ),
	iPlainText( EFalse ),
	iShowUrls( EFalse ),
	iPreformatted( 0 ),
	iAlignment( CParaFormat::ELeftAlign ),
	iStylePos( KNoStylePosition ),
	iNewParagraph( EFalse ),
    iNewLines( 0 ),
	iForcedNewLines( 0 ),
	iSkipWhiteSpaces( ETrue ),
	iAlignmentChanged( EFalse ),
	iImageFound( EFalse ),
	iFirstTextLineAdded( EFalse ),
	iCurrentListContext( KNoListContext ), 
	iDefListLevel( 0 ),
	iBlockQuoteLevel( 0 ),
	iHyperLinkPos( 0 ),
	iTextColor( KBlackColorCode ),
    iObserver( aParserObserver ),
   	iIndentWidth( KDefaultIndentWidth ),
	iMaxListLevels( KDefaultMaxListLevels )
   	{
	}

// ---------------------------------------------------------
// CXhtmlParser::ConstructL
// ---------------------------------------------------------
//
void CXhtmlParser::ConstructL()
	{
	iStackManager = CXhtmlStackManager::NewL();
    iXmlParser = CMDXMLParser::NewL( this );
        
// This enables the correction of the following bug:
//
// ID: TPHU-72BFLV
// Title: MMS Viewer: Whitespace characters are consumed in XHTML text     
//
// Partial correction to the bug is implemented in Symbian XML parser code.
// This call sets XML parser to mode where all whitespaces are forwarded to
// XHTML parser inside DOM document. So, actual whitespace handling is 
// implemented inside XHTML parser.   
     
    iXmlParser->SetWhiteSpaceHandlingMode( ETrue );
        
    CXhtmlEntityConverter* converter =
        new ( ELeave ) CXhtmlEntityConverter();
	            
	//Ownership transferred to XML parser
   	iXmlParser->SetEntityConverter( converter ); 
       
    iXhtmlFontSpecs = new ( ELeave ) CXhtmlFontSpecs();
    iHyperLinkArray = new ( ELeave ) CArrayPtrFlat<CXhtmlHyperLink>( KHyperLinkArrayGranularity );
   	}

// ---------------------------------------------------------
// CXhtmlParser::ParseDomL 
// ---------------------------------------------------------
//
void CXhtmlParser::ParseDomL( CMDXMLDocument* aXmlDocument )
	{
	XHTMLLOG_WRITE( "CXhtmlParser::ParseDomL in" );
    TCharFormat charFormat;
	TCharFormatMask charFormatMask;

	charFormat.iFontSpec = iXhtmlFontSpecs->DefaultFont()->FontSpecInTwips();
	charFormat.iFontPresentation.iTextColor = iTextColor;
    
    charFormatMask.SetAttrib( EAttColor );
    charFormatMask.SetAttrib( EAttFontTypeface );
    charFormatMask.SetAttrib( EAttFontPosture );
    charFormatMask.SetAttrib( EAttFontHeight );
    charFormatMask.SetAttrib( EAttFontStrokeWeight );

    // delete old CharFormatLayer
    
    if( iCurrentCharLayer )
	    {
		iCurrentCharLayer->Reset();
		delete iCurrentCharLayer;
		iCurrentCharLayer = NULL;
	    }
	    
	// create new CharFormatLayer based on GlobalCharFormatLayer and
	// custom font/color
    
	CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL( charFormat,
	    charFormatMask );
	    	
	charFormatLayer->SetBase( iRichText->GlobalCharFormatLayer() );
	iCurrentCharLayer = charFormatLayer;
		    
	CMDXMLNode* node = aXmlDocument->DocumentElement();   
   
	while( node )
		{
		OpenNodeL( node );
		    
		if( node->FirstChild() )
		    {
		    node = node->FirstChild();
		    }
		else if( node->NextSibling() )
			{
			CloseNodeL( node );
			node = node->NextSibling();
			}
		else 
			{
			while( node )
				{
				CloseNodeL( node );
				    				    
				if( node->NextSibling() )
					{
					node = node->NextSibling();
					break;
					}				
				node = node->ParentNode();			
				}
			}
		}	
	XHTMLLOG_WRITE( "CXhtmlParser::ParseDomL out" );
	}
    	
// ---------------------------------------------------------
// CXhtmlParser::OpenNodeL
// ---------------------------------------------------------
//
void CXhtmlParser::OpenNodeL( CMDXMLNode* aNode )
	{
    switch( aNode->NodeType() )
        {
	    case CMDXMLNode::EElementNode:
	        {
	        CMDXMLElement* element = static_cast<CMDXMLElement*>( aNode );
	        
	                
	        // PrepareForBeginElementL does some operations needed before possible
	        // attributes are handled.
	        //
	        // In practice PrepareForBeginElementL adds a new style to stylestack
	        // If there is no attributes within this tag, this style is not used although
	        // it is on the stack
	        	        
	        PrepareForBeginElementL( element->NodeName() );
                
            // handle possible attributes related to begin tag
                
            for( TInt i = 0; i<element->NumAttributes(); ++i )
                {
                TPtrC name;
                TPtrC value;
                    
                if ( element->AttributeDetails( i, name, value ) == KErrNone )
                    {
                    AttributeValueL( name, value );
                    }
                }
    				
    		// Handles actual begin tag. If it doesn't cause changes to 
    		// style or paragraphs, doesn't do anything	
    				
    		BeginElementL( element->NodeName() );
            break;    		    
    		}
    		
	    case CMDXMLNode::ETextNode:
    	    {
    		CMDXMLText* txt = static_cast<CMDXMLText*>( aNode );
    		  	
    		// Adds actual text to CRichText object.
    		// Style and paragraph format are added later when
    		// end tags are handled   	
    		  		
    		ContentL( txt->Data() );
            break;    		    
        	}
        	
	    case CMDXMLNode::ECDATASectionNode:
    	    {
            // CDATA section skipped
    		break;
            }
            
        case CMDXMLNode::ECommentNode:
    	    {
    	    // comment skipped
       		break;
            }
            
        default:
            {
            break;
            }
	    }
	}

// ---------------------------------------------------------
// CXhtmlParser::CloseNode 
// ---------------------------------------------------------
//
void CXhtmlParser::CloseNodeL( CMDXMLNode* aNode )
	{
	if( aNode->NodeType()==CMDXMLNode::EElementNode )
		{
        EndElementL( aNode->NodeName() );
   		}	
	}
	
// ---------------------------------------------------------
// CXhtmlParser::PrepareForBeginElementL 
// ---------------------------------------------------------
//
void CXhtmlParser::PrepareForBeginElementL( const TDesC& aName )
	{
	iAlignmentChanged = EFalse;
	iImageFound = EFalse;
	
	if ( !aName.CompareF( KATag ) )
        {
        BeginHyperLink( iRichText->DocumentLength() );
        }
    else if ( !aName.CompareF( KImgTag ) )
        {
        iImageFound = ETrue;
        }
            
	BeginStyleL();
	}
   
// ---------------------------------------------------------
// CXhtmlParser::BeginElementL
// ---------------------------------------------------------
//
void CXhtmlParser::BeginElementL( const TDesC& aName )
	{
	iImageFound = EFalse;
	
	// If default alignment changed within this tag, begin new paragraph with new alignment.
	// Add information about changed alignment. This info is used later
	// to end paragraph
	
    if ( iAlignmentChanged )
	    {
	    iStackManager->StyleStack()->Top()->iAlignmentChanged = ETrue;
        iAlignmentChanged = EFalse;

        BeginParagraphL( iAlignment );
   		}
   		
	if( !aName.CompareF( KHtmlTag ) )
		{
		}
    else if( !aName.CompareF( KHrTag ) )
        {
 	    BeginParagraphL( CParaFormat::ECenterAlign );
 	    // default alignment is center
 	    
	    BeginStyleL();
	
	    TCharFormat charFormat;
	    TCharFormatMask charFormatMask;
		    
	    charFormat.iFontSpec = iXhtmlFontSpecs->DefaultFont()->FontSpecInTwips();
	 	    
	    charFormatMask.SetAttrib( EAttFontHeight );
	    charFormatMask.SetAttrib( EAttFontTypeface );
	    charFormatMask.SetAttrib( EAttFontPosture );
        charFormatMask.SetAttrib( EAttFontStrokeWeight );
	
	    ChangeCurrentStyleL(charFormat, charFormatMask);
	    InsertTextL( KHrSeparator);
        }
    else if (!aName.CompareF( KDdTag ) )
		{
		BeginParagraphL( iAlignment );
        iDefListLevel++;
        }
    else if (!aName.CompareF( KDtTag ) )          
	    {
		InsertLineBreak();
	    }        
    else if (!aName.CompareF( KDlTag ) )        
        {
        InsertLineBreak();
       	BeginParagraphL( iAlignment );
        }
   	else if( !aName.CompareF( KBodyTag ) )
		{
		BeginStyleL();
		iParsingAllowed = ETrue;
		}
	else if( !aName.CompareF( KAbbrTag ) )
		{
		}
	else if( !aName.CompareF( KAcronymTag ) )
		{
		}
	else if( !aName.CompareF( KSpanTag ) )
		{
		}
	else if( !aName.CompareF( KDivTag ) )
		{
		BeginParagraphL( iAlignment );
   		}
	else if( !aName.CompareF( KBrTag ) )
	    {
	    InsertForcedLineBreak();
	    }
	else if( !aName.CompareF( KPTag ) )
		{	
		InsertLineBreak();
		BeginParagraphL( iAlignment );
		}
	else if( !aName.CompareF( KSmallTag ) || !aName.CompareF( KBigTag ) )
		{
		TCharFormat charFormat;
		TCharFormatMask charFormatMask;
				
		charFormatMask.SetAttrib( EAttFontTypeface );
	    charFormatMask.SetAttrib( EAttFontPosture );
	    charFormatMask.SetAttrib( EAttFontHeight );
	    charFormatMask.SetAttrib( EAttFontStrokeWeight );	
		
		if ( !aName.CompareF( KSmallTag ) )
		    {
			charFormat.iFontSpec = iXhtmlFontSpecs->SmallFont()->FontSpecInTwips();
		    }
		else
		    {
			charFormat.iFontSpec = iXhtmlFontSpecs->BigFont()->FontSpecInTwips();
		    }
		BeginStyleL();
        ChangeCurrentStyleL(charFormat, charFormatMask);
	    }
		
	else if( !aName.CompareF( KFontTag ) )
		{
		}
	else if( !aName.CompareF( KBTag ) || !aName.CompareF( KStrongTag ) )
		{
		TCharFormat charFormat;
		TCharFormatMask charFormatMask;
		charFormatMask.SetAttrib( EAttFontStrokeWeight );
		charFormat.iFontSpec.iFontStyle.SetStrokeWeight( EStrokeWeightBold );

        BeginStyleL();
        ChangeCurrentStyleL(charFormat, charFormatMask);
        }
	else if( !aName.CompareF( KITag ) || !aName.CompareF( KEmTag ) || 
	    !aName.CompareF( KCiteTag )	|| !aName.CompareF( KDfnTag ) ||
	    !aName.CompareF( KVarTag ) )
		{
		TCharFormat charFormat;
		TCharFormatMask charFormatMask;
		
		charFormatMask.SetAttrib( EAttFontPosture );
		charFormat.iFontSpec.iFontStyle.SetPosture( EPostureItalic );
		
		BeginStyleL();
		ChangeCurrentStyleL( charFormat, charFormatMask);
		}
	else if( !aName.CompareF( KAddressTag ) )
		{
		TCharFormat charFormat;
		TCharFormatMask charFormatMask;
		
		charFormatMask.SetAttrib( EAttFontPosture );
		charFormat.iFontSpec.iFontStyle.SetPosture( EPostureItalic );
		
		BeginStyleL();
		ChangeCurrentStyleL(charFormat, charFormatMask);
		
		InsertLineBreak();
		}
	else if( !aName.CompareF( KKbdTag ) || !aName.CompareF( KCodeTag ) || 
	    !aName.CompareF( KSampTag ) )
		{
		TCharFormat charFormat;
		TCharFormatMask charFormatMask;
		
		charFormatMask.SetAttrib( EAttFontTypeface );
	    charFormatMask.SetAttrib( EAttFontPosture );
	    charFormatMask.SetAttrib( EAttFontHeight );
	    charFormatMask.SetAttrib( EAttFontStrokeWeight );

		charFormat.iFontSpec = iXhtmlFontSpecs->CourierFont()->FontSpecInTwips();
		
		BeginStyleL();
		ChangeCurrentStyleL( charFormat, charFormatMask );
		}
	else if( !aName.CompareF( KBlockquoteTag ) )
		{
		InsertLineBreak();
		BeginParagraphL( iAlignment );
	
        iBlockQuoteLevel++;
   		}
	else if( !aName.CompareF( KPreTag ) )
		{
		if ( iPreformatted == 0 )
		    {
		    InsertLineBreak();
		    InsertLineBreak();
		    }
		iPreformatted++;
		}
	else if( !aName.CompareF( KQTag ) )
		{
		}
	else if ( !aName.CompareF( KH1Tag ) || !aName.CompareF( KH2Tag ) || 
	    !aName.CompareF( KH3Tag )|| !aName.CompareF( KH4Tag ) || 
	    !aName.CompareF( KH5Tag )|| !aName.CompareF( KH6Tag ) )
		{
		InsertLineBreak();
		BeginParagraphL( iAlignment );
		BeginStyleL();
		
		TCharFormat charFormat;
		TCharFormatMask charFormatMask;
		 
		charFormatMask.SetAttrib( EAttFontTypeface );
	    charFormatMask.SetAttrib( EAttFontPosture );
	    charFormatMask.SetAttrib( EAttFontHeight );
	    charFormatMask.SetAttrib( EAttFontStrokeWeight );
		
		if ( !aName.CompareF( KH1Tag ) )
	        {
	    	charFormat.iFontSpec = iXhtmlFontSpecs->H1Font()->FontSpecInTwips();
	        }
	    else if ( !aName.CompareF( KH2Tag ) )
	        {
	        charFormat.iFontSpec = iXhtmlFontSpecs->H2Font()->FontSpecInTwips();
	        }
	    else if ( !aName.CompareF( KH3Tag ) )
	        {
	        charFormat.iFontSpec = iXhtmlFontSpecs->H3Font()->FontSpecInTwips();
	        }
	    else if ( !aName.CompareF( KH4Tag ) )
	        {
	        charFormat.iFontSpec = iXhtmlFontSpecs->H4Font()->FontSpecInTwips();
	        }
	    else if ( !aName.CompareF( KH5Tag ) )
	        {
	        charFormat.iFontSpec = iXhtmlFontSpecs->H5Font()->FontSpecInTwips();
	        }
		else
		    {
		    charFormat.iFontSpec = iXhtmlFontSpecs->H6Font()->FontSpecInTwips();
		    }
		
		charFormat.iFontSpec.iFontStyle.SetStrokeWeight( EStrokeWeightBold );
        ChangeCurrentStyleL(charFormat, charFormatMask);
   		}
	else if( !aName.CompareF( KUlTag ) )
		{
        BeginListL( KUnorderedListContext );
    	}
    else if( !aName.CompareF( KOlTag ) )
		{
        BeginListL( KOrderedListContext );
    	}
	else if ( !aName.CompareF( KLiTag ) )
		{
        BeginListItemL();
        BeginStyleL();
		}
   	}

// ---------------------------------------------------------
// CXhtmlParser::EndElementL 
// ---------------------------------------------------------
//
void CXhtmlParser::EndElementL( const TDesC& aName )
	{
	if ( !aName.CompareF( KHtmlTag ) )
	    {
	    ApplyStyleL();
		
		// last line break to the end of the document
        iRichText->InsertL( iRichText->DocumentLength(), 
		                    CRichText::ELineBreak );
	    }
	else if ( !aName.CompareF( KBodyTag ) )
	    {
		EndStyleL();
		iParsingAllowed = EFalse;
	    }
	else if ( !aName.CompareF( KATag ) )
        { 
        EndHyperLinkL( iRichText->DocumentLength() );
        }
    else if ( !aName.CompareF( KDdTag ) )
        {
   		EndParagraphL();
   		iDefListLevel--;
        }
    else if ( !aName.CompareF( KDtTag ) )        
        {
        }
    else if ( !aName.CompareF( KDlTag ) )        
        {
        InsertLineBreak();
        EndParagraphL();
        }
    else if( !aName.CompareF( KSmallTag ) || !aName.CompareF( KBigTag ) )
	    {
	    EndStyleL();
	    }
	else if ( !aName.CompareF( KBTag ) || !aName.CompareF( KStrongTag ) )
		{
		EndStyleL();
		}
	else if ( !aName.CompareF( KITag ) || !aName.CompareF( KEmTag ) || 
	        !aName.CompareF( KCiteTag )	|| !aName.CompareF( KDfnTag ) || 
	        !aName.CompareF( KVarTag ) )
		{
		EndStyleL();
		}
	else if ( !aName.CompareF( KAddressTag ) )
		{
		InsertLineBreak();
		EndStyleL();
		}
	else if ( !aName.CompareF( KKbdTag ) || !aName.CompareF( KCodeTag ) ||
	        !aName.CompareF( KSampTag ) ) 
		{
		EndStyleL();
		}		
	else if ( !aName.CompareF( KBlockquoteTag ) )
		{
		InsertLineBreak();
   		EndParagraphL();
		iBlockQuoteLevel--;
	    }
	else if ( !aName.CompareF( KPreTag ) )
	    {
		if ( iPreformatted > 0 )
		    {
		    iPreformatted--;
	
		    if ( iPreformatted == 0 )
		        {
		        InsertLineBreak();
	            InsertLineBreak();
		        }
		    }
	    }
	else if ( !aName.CompareF( KPTag ) )
        {
		InsertLineBreak();
		EndParagraphL();
        }
    else if ( !aName.CompareF( KDivTag ) )
        {
		EndParagraphL();
        }
    else if ( !aName.CompareF( KQTag ) )
        {
        }  
	else if ( !aName.CompareF( KH1Tag ) || !aName.CompareF( KH2Tag ) || 
	        !aName.CompareF( KH3Tag )|| !aName.CompareF( KH4Tag ) || 
	        !aName.CompareF( KH5Tag )|| !aName.CompareF( KH6Tag ) )
		{
		EndStyleL();
		InsertLineBreak();
		EndParagraphL();
	    }
	else if ( !aName.CompareF( KLiTag ) )
		{
		EndStyleL();
		}
	else if ( !aName.CompareF( KUlTag ) || !aName.CompareF( KOlTag ) )
		{
		EndListL();
   		}
   	else if( !aName.CompareF( KHrTag ) )
        {
        EndStyleL();
        InsertLineBreak();
        EndParagraphL();
        }
    else if( !aName.CompareF( KBrTag ) )
        {
        }
                		   
    // if alignment was changed and a new paragraph added, end this paragraph  
    
    if ( iStackManager->StyleStack()->Top() && iStackManager->StyleStack()->Top()->iAlignmentChanged )
    {
        EndParagraphL();
   	}
   	EndStyleL(); // BeginStyleL has been called in PrepareForBeginElementL
	}

// ---------------------------------------------------------
// CXhtmlParser::BeginStyleL
// ---------------------------------------------------------
//
void CXhtmlParser::BeginStyleL()
    {
    if ( iStackManager->StyleStack()->Top() )
	    {
        if ( iStylePos == KNoStylePosition )
		    {
		    iStylePos = iStackManager->StyleStack()->Top()->iStyleStart;    
		    }
   		}
    TXhtmlStyleInfo* style = new( ELeave )
        TXhtmlStyleInfo( iRichText->DocumentLength(), iAlignment );
        
    iStackManager->StyleStack()->PushL( style );
    }

// ---------------------------------------------------------
// CXhtmlParser::EndStyleL 
// ---------------------------------------------------------
//
void CXhtmlParser::EndStyleL()
    {
    if ( iStackManager->StyleStack()->Top() )
        {
        if ( iStylePos == KNoStylePosition )
		    {
		    iStylePos = iStackManager->StyleStack()->Top()->iStyleStart;  
		    }
		if ( iStackManager->StyleStack()->Top()->iStyleChanged )
		    {
		    ApplyStyleL();
		    iStylePos = KNoStylePosition;
		    iCurrentCharLayer = CleanCharLayer();
            }
        iAlignment = iStackManager->StyleStack()->Top()->iPrevAlign;    
        }

    iStackManager->StyleStack()->Pop();
    
    if ( iStackManager->StyleStack()->Top() )
        {
        iStackManager->StyleStack()->Top()->iStyleStart = iRichText->DocumentLength();
        }
    }

// ---------------------------------------------------------
// CXhtmlParser::ChangeCurrentStyleL 
// ---------------------------------------------------------
//
void CXhtmlParser::ChangeCurrentStyleL(TCharFormat charFormat,
    TCharFormatMask charFormatMask)
    {
    ApplyStyleL();
    iStylePos = KNoStylePosition;

    if ( iStackManager->StyleStack()->Top() )
        {
        iStackManager->StyleStack()->Top()->iStyleChanged = ETrue;
        }
        
	CCharFormatLayer* cfl = 
	    CCharFormatLayer::NewL( charFormat, charFormatMask );
	cfl->SetBase( iCurrentCharLayer );
	iCurrentCharLayer = cfl;
    }

// ---------------------------------------------------------
// CXhtmlParser::ApplyStyle 
// ---------------------------------------------------------
//
void CXhtmlParser::ApplyStyleL()
    {
    if ( iStylePos != KNoStylePosition && ( iRichText->DocumentLength() - iStylePos > 0 ) )
        {
        TCharFormat charFormat;
	    TCharFormatMask charFormatMask;

		iCurrentCharLayer->SenseEffective( charFormat );
		charFormatMask.SetAll();
    
        if ( !iPlainText )
            {
            iRichText->ApplyCharFormatL( charFormat, charFormatMask, 
		        iStylePos, iRichText->DocumentLength() - iStylePos );
            }

        }
    }

// ---------------------------------------------------------
// CXhtmlParser::BeginParagraphL 
// ---------------------------------------------------------
//
void CXhtmlParser::BeginParagraphL( CParaFormat::TAlignment aAlignment )
    {
    if ( iStackManager->ParaStack()->Top() )
        {
        ApplyParagraphL( iStackManager->ParaStack()->Top()->iParaStart,
            iStackManager->ParaStack()->Top()->iAlignment );
        }
    if ( iFirstTextLineAdded )
        {
        AppendParagraphL( iPlainText ); 
        }
    else
        {
        iNewParagraph = ETrue;    
        }
    TXhtmlParaInfo* info = new( ELeave ) TXhtmlParaInfo( iRichText->DocumentLength(), aAlignment );
	iStackManager->ParaStack()->PushL( info );
    }

// ---------------------------------------------------------
// CXhtmlParser::EndParagraphL 
// ---------------------------------------------------------
//
void CXhtmlParser::EndParagraphL()
    {
    if (iStackManager->ParaStack()->Top())
        {
        ApplyParagraphL( iStackManager->ParaStack()->Top()->iParaStart,
            iStackManager->ParaStack()->Top()->iAlignment );
    
        if ( iFirstTextLineAdded )
            {
            AppendParagraphL( iPlainText ); 
            }
        else
            {
            iNewParagraph = ETrue;    
            }
        }
        
    iStackManager->ParaStack()->Pop();    
        
    if ( iStackManager->ParaStack()->Top() )
        {
        iStackManager->ParaStack()->Top()->iParaStart = iRichText->DocumentLength();
        }
    }

// ---------------------------------------------------------
// CXhtmlParser::ApplyParagraphL 
// ---------------------------------------------------------
//
void CXhtmlParser::ApplyParagraphL( TInt aParaStart,
    CParaFormat::TAlignment aAlignment )
    {
    TInt docLength = iRichText->DocumentLength();
	TInt paraLength = docLength - aParaStart;

    if ( paraLength > 0 )
        {
        CParaFormat* paraFormat = CParaFormat::NewL();
	    CleanupStack::PushL( paraFormat );
	    TParaFormatMask paraFormatMask = TParaFormatMask();
	
        if ( aAlignment != CParaFormat::EUnspecifiedAlign )
            {
            paraFormatMask.SetAttrib( EAttAlignment );
            paraFormat->iHorizontalAlignment = aAlignment;
            }
    
        CGraphicsDevice* screenDevice = CCoeEnv::Static()->ScreenDevice();

        TInt indentWidthInTwips = iIndentWidth * screenDevice->HorizontalPixelsToTwips( 
            iXhtmlFontSpecs->DefaultFont()->CharWidthInPixels( TChar(' ') ) );
      
        // check limits
        
        TInt leftLevel =
            ( ( iStackManager->ListStack()->Count() + iDefListLevel + iBlockQuoteLevel ) < iMaxListLevels )
            ? ( iStackManager->ListStack()->Count() + iDefListLevel + iBlockQuoteLevel  ) : iMaxListLevels;
         
        TInt rightLevel = ( iBlockQuoteLevel  < iMaxListLevels ) ? iBlockQuoteLevel : iMaxListLevels;
     
        // set indents 
         
   	    paraFormat->iLeftMarginInTwips = leftLevel * indentWidthInTwips;
   	    paraFormat->iRightMarginInTwips = rightLevel * indentWidthInTwips;
   	    
   	    paraFormatMask.SetAttrib( EAttLeftMargin );
   	    paraFormatMask.SetAttrib( EAttRightMargin );

        if (!iPlainText)
            {
            iRichText->ApplyParaFormatL( paraFormat, paraFormatMask, aParaStart,
		        paraLength );

            }
		CleanupStack::PopAndDestroy( paraFormat );
		}
    }

// ---------------------------------------------------------
// CXhtmlParser::BeginListL 
// ---------------------------------------------------------
//
void CXhtmlParser::BeginListL(TInt aListContext)
    {
    if ( iStackManager->ListStack()->Count() == 0 && iDefListLevel == 0)
        {
        InsertLineBreak();
        }
    BeginParagraphL( iAlignment );
    
    TXhtmlListInfo* info = new( ELeave ) TXhtmlListInfo( iCurrentListContext );
    iStackManager->ListStack()->PushL( info );
	
	iCurrentListContext = aListContext;  
    }

// ---------------------------------------------------------
// CXhtmlParser::EndList 
// ---------------------------------------------------------
//
void CXhtmlParser::EndListL()
    {
    if ( ( iStackManager->ListStack()->Count() + iDefListLevel ) <= 1 )
        {
        InsertLineBreak();
        }
   	EndParagraphL();
   	
    if (iStackManager->ListStack()->Top())
        {
        iCurrentListContext = iStackManager->ListStack()->Top()->iListContext;
        }   	
    iStackManager->ListStack()->Pop();
    }

// ---------------------------------------------------------
// CXhtmlParser::BeginListItemL
// ---------------------------------------------------------
//
void CXhtmlParser::BeginListItemL()
    {
    TXhtmlListInfo *info = iStackManager->ListStack()->Top();
    if ( !info )
        {
        return;
        }
    
    if ( ( info->iListIndex > 0 ) && ( iNewLines == 0 ) && ( iForcedNewLines == 0 ) )
        {
        iRichText->InsertL( iRichText->DocumentLength(),
            CRichText::EParagraphDelimiter );
        }
        
    info->iListIndex++;

   	if ( iCurrentListContext == KOrderedListContext )
	    {
	    TBuf16<16> prefix;
        prefix.Format( KListNumberFormat, info->iListIndex );
        
        HBufC* tmpString = prefix.AllocLC();
        
        TPtrC result; 
        DoNumberConversion( tmpString, result );

        InsertTextL( result );
        CleanupStack::PopAndDestroy( tmpString );
		}
	else
		{
		if ( iPlainText )
            {
            TInt bullet = ( iStackManager->ListStack()->Count() % 2 ) ? KPlainBullet : KPlainBullet2 ;
            InsertCharacterL( TChar( bullet ) );
            }
        else
            {
            TInt bullet = ( iStackManager->ListStack()->Count() % 2 ) ? KListBullet : KListBullet2 ;
            InsertCharacterL( TChar( bullet) );
            }
        }
    }
    
// ---------------------------------------------------------
// CXhtmlParser::BeginHyperLinkL 
// ---------------------------------------------------------
// 
void CXhtmlParser::BeginHyperLink( TInt aBeginPosition )
    {
	XHTMLLOG_WRITE( "BeginHyperLinkL" );

    delete iHyperLinkAddress;
    iHyperLinkAddress = NULL;
        
    iHyperLinkPos = aBeginPosition;
    }
 
// ---------------------------------------------------------
// CXhtmlParser::EndHyperLinkL 
// ---------------------------------------------------------
// 
void CXhtmlParser::EndHyperLinkL( TInt aEndPosition )
    {
	XHTMLLOG_WRITE( "EndHyperLinkL");
    
    if ( iHyperLinkAddress )
        {
        CXhtmlHyperLink* hpLink = new (ELeave) CXhtmlHyperLink( iHyperLinkPos,
            aEndPosition - 1 );
    
        TPtrC addrPtr = iHyperLinkAddress->Des();
        hpLink->SetAddressL(addrPtr);
              
        delete iHyperLinkAddress;
        iHyperLinkAddress = NULL;
        
        iHyperLinkArray->InsertL( iHyperLinkArray->Count(), hpLink );
       
        if ( iShowUrls )
            {
            InsertCharacterL( TChar(' ') );
            InsertCharacterL( TChar('(') );
            InsertTextL( hpLink->Address()->Des() );
            InsertCharacterL( TChar(')') );
            }
        }
    }

// ---------------------------------------------------------
// CXhtmlParser::ContentL
// ---------------------------------------------------------
//
void CXhtmlParser::ContentL( const TDesC& aData )
	{	 
    if ( !iParsingAllowed )
        {
        return;
        } 
	
    HBufC16* buffer = aData.Alloc();
    CleanupStack::PushL( buffer );

    TPtr16 bufferPtr = buffer->Des();

    if( iPreformatted > 0 )
        {        
        // XHTML parsing, pre tag specified (preformatted text)
        
        TInt offset = 0;
        TInt poffset = 0;
        
        TChar LF( 0x0A );
        TChar CR( 0x0D );  

        if( bufferPtr.Locate( LF ) > 0 || bufferPtr.Locate( CR ) > 0 )
            {
            while( offset < bufferPtr.Length() )
                {
                TPtrC16 theRest = bufferPtr.Mid( offset );
                
                poffset = theRest.Locate( CR );
                    
                if( poffset != KErrNotFound )
                    {
                    TPtrC16 first = theRest.Mid( 0, poffset );
                    InsertTextL( first );
                    
                        iRichText->InsertL( iRichText->DocumentLength(), 
		                    CRichText::ELineBreak );

                        offset = offset + poffset + 1;
                        
                        // if next character is LF, add offset by one
                        
                        if ( theRest.Locate( LF ) == poffset + 1)
                            {
                            offset++;
                            }
                        }
                    else
                        {
                         poffset = theRest.Locate( LF );
                         
                         if( poffset != KErrNotFound ) 
                            {
                            TPtrC16 first = theRest.Mid( 0, poffset );
                            InsertTextL( first );
                 
                            iRichText->InsertL( iRichText->DocumentLength(), 
		                    CRichText::ELineBreak );

                            offset = offset + poffset + 1;
                            }
                        else
                            {
                            InsertTextL( theRest );
                            break;
                            }
                        }
                } // while
            }
        else
            {
            // no newline(s) found, insert text as such
            InsertTextL( bufferPtr );
            }
        }
    else     
        {
        // XHTML parsing, pre tag not specified (extra whitespaces removed)
        CollapseWhiteSpaces( bufferPtr );   

        if (bufferPtr.Length() > 0)
            {
            InsertTextL( bufferPtr );
            }
        }
    CleanupStack::PopAndDestroy( buffer );
    }

// ---------------------------------------------------------
// CXhtmlParser::CollapseWhiteSpaces
//
// Function processes the whole text and converts any
// found sequence of white spaces into single space.
// ---------------------------------------------------------
//
void CXhtmlParser::CollapseWhiteSpaces( TDes& aData )
    {
    TInt whiteSpaces( 0 );
    TInt currentPos( 0 );
    
    while( currentPos < aData.Length() )
        {
        if( aData[currentPos] == CEditableText::ESpace ||
            aData[currentPos] == KZeroWidthSpace ||
            ( aData[currentPos] >= 0x0009 && aData[currentPos] <= 0x000d ) )
            {
            whiteSpaces++;
            }
        else if( whiteSpaces > 0 )
            {
            currentPos = currentPos - whiteSpaces;

            if (iSkipWhiteSpaces)
                {
                aData.Delete( currentPos, whiteSpaces );
                iSkipWhiteSpaces = EFalse;
                }
            else
                {
                aData.Replace( currentPos, whiteSpaces, KWhiteSpace );
                }
            whiteSpaces = 0;
            }
        else
            {
            // no whitespaces at the beginning
            iSkipWhiteSpaces = EFalse;
            }
        currentPos++;
        }
       
    if( whiteSpaces > 0 )
        {
        if (iSkipWhiteSpaces)
            {
            aData.Delete( currentPos - whiteSpaces, whiteSpaces );
            }
        else
            {
            aData.Replace( currentPos - whiteSpaces, whiteSpaces, KWhiteSpace );
            }
        }    
    }

// ---------------------------------------------------------
// CXhtmlParser::AttributeValueL
// ---------------------------------------------------------
//
void CXhtmlParser::AttributeValueL( const TDesC& aName, const TDesC& aValue )
	{
	CParaFormat::TAlignment alignment = iAlignment;

    if( !aName.CompareF( KHrefAttr ) ) 
        {
        SetReferenceL( aValue ); 
        }
	if( !aName.CompareF( KStyleAttr ) )
	    {
		TRgb colorValue = 0;
		TBool colorAttr = EFalse;
		TBool underlineAttr = EFalse;
		
		TInt offset = 0;
		TInt poffset = 0;

		HBufC16* buffer = aValue.AllocLC();

		TPtr16 bufferPtr = buffer->Des();

		RemoveAllSpace( bufferPtr );

		TPtrC nameStyle;
		TPtrC valueStyle;
		TBool transparent = EFalse;
        
        while( offset < bufferPtr.Length() )
            {
            TPtrC16 theRest = bufferPtr.Mid( offset );
            poffset = theRest.Locate( TChar( KSemicolon ) );

			if( poffset != KErrNotFound )
                {
			    TPtrC16 first = theRest.Mid( 0, poffset );
				SplitAttribute( first, nameStyle, valueStyle );
				offset = offset + poffset + 1;
			    }
			else
			    {
				SplitAttribute( theRest, nameStyle, valueStyle );
				offset = bufferPtr.Length();
			    }
				
            if( !nameStyle.CompareF( KColorAttr ) )
			    {
				colorValue = ParseColor( valueStyle, transparent );
				colorAttr = ETrue;
				}
				
			if ( !nameStyle.CompareF( KTextDecorationAttr ) &&
			     !valueStyle.CompareF( KUnderlineAttr ) )
			    {
			    underlineAttr = ETrue;
			    }
				
            if( !nameStyle.CompareF( KTextAlignAttr ) )
                {
                //left
                if ( !valueStyle.CompareF( KLeftAttr ) )
                    {
                    //set the align-value and use it in the paragraph-tag if set
                    iAlignment = CParaFormat::ELeftAlign;
                    }
                //center
                else if( !valueStyle.CompareF( KCenterAttr ) )
                    {
                    iAlignment = CParaFormat::ECenterAlign;                        
                    }
                    
                //right
                else if( !valueStyle.CompareF( KRightAttr ) )
                    {
                    iAlignment = CParaFormat::ERightAlign;
                    }

                //justified
                else if( !valueStyle.CompareF( KJustifyAttr ) )
                    {
                    iAlignment = CParaFormat::EJustifiedAlign;
                    }
                }
   		    }

		CleanupStack::PopAndDestroy( buffer );  
				
		TCharFormat charFormat;
		TCharFormatMask charFormatMask;
		
		if( colorAttr )
		    {
			charFormatMask.SetAttrib( EAttColor );
			charFormat.iFontPresentation.iTextColor = colorValue;
		    }
		if ( underlineAttr )
		    {
			charFormatMask.SetAttrib( EAttFontUnderline );
			charFormat.iFontPresentation.iUnderline = EUnderlineOn;
		    }
	    ChangeCurrentStyleL(charFormat, charFormatMask);
	    }

	if( !aName.CompareF( KColorAttr ) )
		{
		TBool transparent = EFalse;
		TRgb color = ParseColor( aValue, transparent );
		
		TCharFormat charFormat;
		TCharFormatMask charFormatMask;

		charFormatMask.SetAttrib( EAttColor ); 
		charFormat.iFontPresentation.iTextColor = color;
		ChangeCurrentStyleL(charFormat, charFormatMask);
		}
	else if( !aName.CompareF( KAltAttr ) )
		{
		if( aValue.Length() > 0 && iImageFound)
		    {
			InsertTextL( aValue );
		    }
		}
	if ( alignment != iAlignment )
	    {
	    iAlignmentChanged = ETrue;
	    }
	}

// ---------------------------------------------------------
// CXhtmlParser::SetReferenceL() 
// ---------------------------------------------------------
// 
void CXhtmlParser::SetReferenceL( const TDesC& aUrl )
    {
    delete iHyperLinkAddress;
    iHyperLinkAddress = NULL;
        
    iHyperLinkAddress = HBufC::NewL( aUrl.Length() );
    TPtr ptr = iHyperLinkAddress->Des();
    ptr = aUrl;
    }

// ---------------------------------------------------------
// CXhtmlParser::RemoveAllSpace
// ---------------------------------------------------------
//
void CXhtmlParser::RemoveAllSpace( TDes& aString )
    {
	const TChar KSpace = TChar( ' ' );  
	for( TInt offset = aString.Locate( KSpace ); offset != KErrNotFound;
	     offset = aString.Locate( KSpace ) )
	    {
		aString.Delete( offset, 1 );
	    }
    }

// ---------------------------------------------------------
// CXhtmlParser::SplitAttribute
// ---------------------------------------------------------
//
void CXhtmlParser::SplitAttribute( TPtrC& aString, TPtrC& aAttributeName, 
                                    TPtrC& aAttributeValue )
    {
	TInt offset = 0;
    TInt poffset = 0;
    TInt start = 0;
	
    if( aString.Locate( TChar( KColon ) ) > 0 )
        {
        while( offset < aString.Length() )
            {
            TPtrC16 theRest = aString.Mid( offset );
            poffset = theRest.Locate( TChar ( KColon ) );			

			if( poffset != KErrNotFound )
                {
                aAttributeName.Set( theRest.Mid( start, poffset ) );
				aAttributeValue.Set( theRest.Mid( poffset + 1 ) );
				return;
				}
			}
	    }
    }

// ---------------------------------------------------------
// CXhtmlParser::ParseColor
// ---------------------------------------------------------
//
TRgb CXhtmlParser::ParseColor( const TDesC& aString, TBool& aTransparent )
	{
	aTransparent = ETrue;
	TRgb res = 0;
	if ( aString.Length()>1 )
		{
		TUint32 val = 0;
		if( aString[0] == '#' )
			{
			TLex lexer ( aString.Mid( 1 ) );
			if( lexer.Val( val, EHex, KWhiteColorCode )==KErrNone )
				{
				res = TRgb ((val&KBlueColorCode)>>16, 
				    (val&KLimeColorCode)>>8, (val&KRedColorCode));
				aTransparent = EFalse;
				}			
			}
		else if ( aString == KTransparentVal )
			{
			aTransparent = ETrue;
			}
		else
			{
			for( TInt n = 0; KColorNames[n]; n++ )
				{
				
				if ( !aString.CompareF( TPtrC(KColorNames[n]) ) )
					{					
					res = KColorValues[n];

					aTransparent = EFalse;
					break;
					}
				}
			}
		}
	return res;
	}

// ---------------------------------------------------------
// CXhtmlParser::CdataL
// ---------------------------------------------------------
//
void CXhtmlParser::CdataL( const TDesC& aData ) 
	{
   	if ( iParsingAllowed )
	    {
		InsertTextL( aData );
	    }
	}

// ---------------------------------------------------------
// CXhtmlParser::CleanCharLayer
// ---------------------------------------------------------
//
CCharFormatLayer* CXhtmlParser::CleanCharLayer()
    {
   	CCharFormatLayer* cfl = iCurrentCharLayer;
	const CCharFormatLayer* layer = static_cast<const CCharFormatLayer*>( cfl->SenseBase() );
	iCurrentCharLayer = const_cast<CCharFormatLayer*>( layer ); 
	       
	cfl->Reset();
	delete cfl;
	return iCurrentCharLayer;
    }
        	
// ---------------------------------------------------------
// Convert between arabic-indic digits and european digits based on existing language setting.
// So it'll convert any digit from the string
// to use either european digits or arabic-indic digits based on current settings.
// @param aFieldString: Data buffer used in conversion.
// @param aFieldData: Return converted data in this parameter.
// ---------------------------------------------------------
//
void CXhtmlParser::DoNumberConversion( HBufC* aFieldString, TPtrC& aFieldData ) const
    {
    if( aFieldString )
        {
    	TPtr tmpPtr = aFieldString->Des();
    	if ( tmpPtr.Length() > 0 )
            {
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tmpPtr );
            }
        aFieldData.Set( tmpPtr );
        }
    else
        {
        aFieldData.Set(KNullString);
        }
    }

// ---------------------------------------------------------
// CXhtmlParser::ResetValuesL 
// ---------------------------------------------------------
//
void CXhtmlParser::ResetValues()
	{
	iParsingAllowed = EFalse;
   	iPreformatted = 0;
   	iAlignment = CParaFormat::ELeftAlign;
   	iStylePos = KNoStylePosition;
	iNewParagraph = EFalse;
    iNewLines = 0;
	iForcedNewLines = 0;
	iSkipWhiteSpaces = ETrue;
    iAlignmentChanged = EFalse;
    iImageFound = EFalse;
    iFirstTextLineAdded = EFalse;
   	iCurrentListContext = KNoListContext; 
	iDefListLevel = 0;
	iBlockQuoteLevel = 0;
	iHyperLinkPos = 0;
	iHyperLinkArray->ResetAndDestroy();
	}	    

// ---------------------------------------------------------
// CXhtmlParser::InsertTextL 
// ---------------------------------------------------------
//
void CXhtmlParser::InsertTextL( const TDesC& aText )
    {
#ifdef USE_LOGGER
	XHTMLLOG_WRITE( "InsertTextL" );
	HBufC* buf = HBufC::NewL( aText.Length() +4 );
	buf->Des().Append(_L("["));
	buf->Des().Append(aText);
	buf->Des().Append(_L("]"));
  XHTMLLOG_WRITEF( _L("%S"), buf );
	delete buf;
#endif 

    TInt maxLines = ( iNewLines > iForcedNewLines ) ? iNewLines : iForcedNewLines;
    
    if ( !iFirstTextLineAdded )
        {
        maxLines = iForcedNewLines;
        }
    
    if ( iNewParagraph )
        {
        maxLines--;
        }
        
    for ( TInt i = 0; i < maxLines; i++ )
        {
        iRichText->InsertL( iRichText->DocumentLength(), 
		                    CRichText::ELineBreak );
        }
    iRichText->InsertL( iRichText->DocumentLength(), aText );

    iNewParagraph = EFalse;
    iNewLines = 0;
    iForcedNewLines = 0; 
    
    iFirstTextLineAdded = ETrue;
    }

// ---------------------------------------------------------
// CXhtmlParser::InsertCharacterL 
// ---------------------------------------------------------
//
void CXhtmlParser::InsertCharacterL( const TChar aText )
    {
#ifdef USE_LOGGER
	XHTMLLOG_WRITE( "InsertCharacterL" );
    
    TBuf<2048> b;
	b.Append(_L("["));
	b.Append(aText);
	b.Append(_L("]"));
	XHTMLLOG_WRITEF( b );
#endif    
    
    TInt maxLines = ( iNewLines > iForcedNewLines ) ? iNewLines : iForcedNewLines;
    
    if ( !iFirstTextLineAdded )
        {
        maxLines = iForcedNewLines;
        }
            
    if ( iNewParagraph )
        {
        maxLines--;
        }
        
    for ( TInt i = 0; i < maxLines; i++ )
        {
        iRichText->InsertL( iRichText->DocumentLength(), 
		                    CRichText::ELineBreak );
        }
    iRichText->InsertL( iRichText->DocumentLength(), aText );

    iNewParagraph = EFalse;
    iNewLines = 0;
    iForcedNewLines = 0; 
    
    iFirstTextLineAdded = ETrue;
    }

// ---------------------------------------------------------
// CXhtmlParser::AppendParagraphL 
// ---------------------------------------------------------
//
void CXhtmlParser::AppendParagraphL( TBool aPlainText )
    {
	XHTMLLOG_WRITE( "AppendParagraphL" );

    iSkipWhiteSpaces = ETrue;
    
    if ( !iNewParagraph )
        {
        
        if ( aPlainText )
            {
            iRichText->InsertL( iRichText->DocumentLength(), 
		                    CRichText::ELineBreak );
            }
        else
            {
            iRichText->AppendParagraphL();
            }
        
        iNewParagraph = ETrue; 
        
        if ( iNewLines < 2 )
            {
            iNewLines++;
            }
        }
    }

// ---------------------------------------------------------
// CXhtmlParser::InsertLineBreakL 
// ---------------------------------------------------------
//
void CXhtmlParser::InsertLineBreak()
    {
	XHTMLLOG_WRITE( "InsertLineBreakL" );

    iSkipWhiteSpaces = ETrue;
    if ( iNewLines < 2 )
        {
        iNewLines++;
        }
    }
    
// ---------------------------------------------------------
// CXhtmlParser::InsertForcedLineBreakL 
// ---------------------------------------------------------
//
void CXhtmlParser::InsertForcedLineBreak()
    {
	XHTMLLOG_WRITE( "InsertForcedLineBreakL"  );
    
    iSkipWhiteSpaces = ETrue;
    iForcedNewLines++;
    }
