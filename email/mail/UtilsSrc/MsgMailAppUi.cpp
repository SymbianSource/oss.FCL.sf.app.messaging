/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A common base class for Mail application ui classes.
*
*/


// INCLUDE FILES
#include <akntitle.h>
#include <AknsUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <gulicon.h>

#include <msgmailutils.mbg>
#include <miuthdr.h>                    // CImHeader
#include <msvapi.h>                     // CMsvEntry
#include <miutmsg.h>                    // CImEmailMessage
#include <mmsvattachmentmanager.h>
#include <cmsvattachment.h>
#include <ConeResLoader.h>              // RConeResourceLoader
#include <CommonContentPolicy.h>        // ContentPolicy
#include <muiumsvuiserviceutilitiesinternal.h>  // MUIU MTM utils 




#include <AknWaitNoteWrapper.h>
#include <AknWaitDialog.h>
#include <featmgr.h>                    // FeatureManager
#include <bldvariant.hrh>
#include <coeutils.h>
#include <data_caging_path_literals.hrh> 
#include <messaginginternalcrkeys.h>
#include <messagingvariant.hrh>

// Editor base classes:
#include <MsgAttachmentInfo.h>
#include <MsgAttachmentModel.h>
#include <MsgEditorView.h>
#include <MsgBodyControl.h>
#include <MsgAddressControl.h>
#include <MsgRecipientItem.h>

#include <MuiuOperationWait.h>              // CMuiuOperationWait
#include <StringLoader.h>

#include <MsgMailUtils.rsg>
#include "MsgMailAppUi.h"
#include "MsgMailDocument.h"
#include "MsgMailPreferences.h"
#include "MailUtils.h"

// Mail Central Repository handler
#include "CMailCRHandler.h"

// LOCAL CONSTANTS AND MACROS

_LIT(KResourcesWithDir,"z:MsgMailUtils.rSC");
_LIT( KMsgMailMBMFileName, "z:msgmailutils.mbm" );
/// Message details item separator.
_LIT( KMsgDetailsSeparator, "; " );
/// Max length used for message details.
const TInt KMsgMaxDetailsLength    = 128;

// LOCAL FUNCTION PROTOTYPES
typedef TBool (*TParseFunc)( TLex& aLex );
LOCAL_C TBool DoOrReverse( TParseFunc aFunc, TLex& aLex );
LOCAL_C TBool Star( TParseFunc aFunc, TLex& aLex );
LOCAL_C TBool Plus( TParseFunc aFunc, TLex& aLex );
LOCAL_C TBool NoWSCtl( TLex& aLex );
LOCAL_C TBool WSP( TLex& aLex );
LOCAL_C TBool FWS( TLex& aLex );
LOCAL_C TBool CText( TLex& aLex );
LOCAL_C TBool QuotedPair( TLex& aLex );
LOCAL_C TBool FWSCContent( TLex& aLex );
LOCAL_C TBool CContent( TLex& aLex );
LOCAL_C TBool Comment( TLex& aLex );
LOCAL_C TBool CFWS( TLex& aLex );
LOCAL_C TBool AText( TLex& aLex );
LOCAL_C TBool Atom( TLex& aLex );
LOCAL_C TBool DotAtomText( TLex& aLex );
LOCAL_C TBool DotAtom( TLex& aLex );
LOCAL_C TBool DotPlusAText( TLex& aLex );
LOCAL_C TBool QText( TLex& aLex );
LOCAL_C TBool QContent( TLex& aLex );
LOCAL_C TBool FWSQContent( TLex& aLex );
LOCAL_C TBool QuotedString( TLex& aLex );
LOCAL_C TBool LocalPart( TLex& aLex );
LOCAL_C TBool DText( TLex& aLex );
LOCAL_C TBool DContent( TLex& aLex );
LOCAL_C TBool DomainLiteral( TLex& aLex );
LOCAL_C TBool Domain( TLex& aLex );
LOCAL_C TBool FWSDContent( TLex& aLex );
LOCAL_C TBool AddrSpec( TLex& aLex );
LOCAL_C TBool Word( TLex& aLex );
LOCAL_C TBool DisplayName( TLex& aLex );
LOCAL_C TBool CommaMailbox( TLex& aLex );
LOCAL_C TBool AngleAddr( TLex& aLex );
LOCAL_C TBool NameAddr( TLex& aLex );
LOCAL_C TBool Mailbox( TLex& aLex );
LOCAL_C TBool MailboxList( TLex& aLex );
LOCAL_C TBool Group( TLex& aLex );
LOCAL_C TBool Address( TLex& aLex );

// ==================== LOCAL FUNCTIONS ====================

// ----------------------------------------------------
// DoOrReverse()
// Calls aFunc. If it fails, the lexer is returned to 
// the state before calling DoOrReverse().
// ----------------------------------------------------
//
LOCAL_C TBool DoOrReverse( TParseFunc aFunc, TLex& aLex )
	{
	TLexMark mark;
	aLex.Mark( mark );
	if( (*aFunc)(aLex) )
		{
		return ETrue;
		}
	else
		{
		aLex.UnGetToMark( mark );
		return EFalse;
		}
	}

// ----------------------------------------------------
// Star()
// Calls aFunc repeatedly until aFunc returns EFalse.
// The lexer is left to the state where it was after
// the last succesful aFunc call. Returns ETrue if
// at least one call to aFunc succeeded.
// ----------------------------------------------------
//
LOCAL_C TBool Star( TParseFunc aFunc, TLex& aLex )
	{
	TBool hasMore( ETrue );
	TBool hasOne( EFalse );
	while( hasMore )
		{
		if( DoOrReverse( aFunc, aLex ) )
			{
			hasOne = ETrue;	
			}
		else
			{
			hasMore = EFalse;
			}
		}

	return hasOne;
	}

// ----------------------------------------------------
// Like Star() except enforces one succesful call
// to aFunc.
// ----------------------------------------------------
//
LOCAL_C TBool Plus( TParseFunc aFunc, TLex& aLex )
	{
	if( !DoOrReverse( aFunc, aLex ) )
		{
		return EFalse;
		}

	Star( aFunc, aLex );
	return ETrue;
	}

// ----------------------------------------------------
// NO-WS-CTL := %d1-8 / %d11 / %d12 / %d14-31 / %d127 ; US-ASCII 
// control characters that do not include the carriage return, 
// line feed, and white space characters
// ----------------------------------------------------
//
LOCAL_C TBool NoWSCtl( TLex& aLex )
	{
	TChar ch( aLex.Get() );
	return ( (ch >= 1 && ch <= 8)			// CSI: 47 # See a comment above.
		|| ch == 11							// CSI: 47 # See a comment above.
		|| ch == 12							// CSI: 47 # See a comment above.
		|| (ch >= 14 && ch <= 31)			// CSI: 47 # See a comment above.
		|| ch == 127 );						// CSI: 47 # See a comment above.
	}


// ----------------------------------------------------
// text := %d1-9 / %d11 / %d12 / %d14-127 / obs-text ; Characters excluding CR and LF
// ----------------------------------------------------
//
LOCAL_C TBool Text( TLex& aLex )
	{
	TChar ch( aLex.Get() );
	return ( (ch >= 1 && ch <= 9)			// CSI: 47 # See a comment above.
		|| ch == 11							// CSI: 47 # See a comment above.
		|| ch == 12							// CSI: 47 # See a comment above.
		|| (ch >= 14 && ch <= 127) );		// CSI: 47 # See a comment above.
	}

// ----------------------------------------------------
// quoted-pair := ("\" text) / obs-qp
// ----------------------------------------------------
//
LOCAL_C TBool QuotedPair( TLex& aLex )
	{
	if( aLex.Get() != '\\' )
		{
		return EFalse;
		}

	return Text( aLex );
	}

// ----------------------------------------------------
// WSP = SPACE / HTAB
// ----------------------------------------------------
//
LOCAL_C TBool WSP( TLex& aLex )
	{
	TChar ch( aLex.Get() );
	return ch == EKeySpace || ch == EKeyTab;
	}

// ----------------------------------------------------
// FWS := ([*WSP CRLF] 1*WSP) / obs-FWS ; Folding white space
// ----------------------------------------------------
//
LOCAL_C TBool FWS( TLex& aLex )
	{
	return Plus( WSP, aLex );
	}

// ----------------------------------------------------
// ctext := NO-WS-CTL / %d33-39 / %d42-91 / %d93-126 ; Non white space 
// controls and The rest of the US-ASCII characters not including "(", ")", or "\"
// ----------------------------------------------------
//
LOCAL_C TBool CText( TLex& aLex )
	{
	if( !DoOrReverse( NoWSCtl, aLex ) )
		{
		TChar ch( aLex.Get() );
		return( (ch >= 33 && ch <= 39)				// CSI: 47 # See a comment above.
			|| (ch >= 42 && ch <= 91)				// CSI: 47 # See a comment above.
			|| (ch >= 93 && ch <= 126) );			// CSI: 47 # See a comment above.
		}
	else
		{
		return ETrue;
		}
	}

// ----------------------------------------------------
// ccontent := ctext / quoted-pair / comment
// ----------------------------------------------------
//
LOCAL_C TBool CContent( TLex& aLex )
	{
	return DoOrReverse( CText, aLex ) || DoOrReverse( QuotedPair, aLex ) || Comment( aLex );
	}

// ----------------------------------------------------
// [FWS] ccontent
// ----------------------------------------------------
//
LOCAL_C TBool FWSCContent( TLex& aLex )
	{
	FWS( aLex );
	return CContent( aLex );
	}

// ----------------------------------------------------
// comment := "(" *([FWS] ccontent) [FWS] ")"
// ----------------------------------------------------
//
LOCAL_C TBool Comment( TLex& aLex )
	{
	if( aLex.Get() != '(' )
		{
		return EFalse;
		}

	Star( FWSCContent, aLex );
	FWS( aLex );
	return aLex.Get() == ')';
	}

// ----------------------------------------------------
// [FWS] comment
// ----------------------------------------------------
//
LOCAL_C TBool FWSComment( TLex& aLex )
	{
	FWS( aLex );
	return Comment( aLex );
	}

// ----------------------------------------------------
// CFWS := *([FWS] comment) (([FWS] comment) / FWS)
// ----------------------------------------------------
//
LOCAL_C TBool CFWS( TLex& aLex )
	{
	TBool hasOne( Star( FWSComment, aLex ) );

	return hasOne || FWS( aLex );
	}

// ----------------------------------------------------
// atext := ALPHA / DIGIT / "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / 
// "-" / "/" / "=" / "?" "^" / "_" / "`" / "{" / "|" / "}" / "~"; Any 
// character except controls, SP, and specials. Used for atoms
// ----------------------------------------------------
//
LOCAL_C TBool AText( TLex& aLex )
	{
	//ALPHA / DIGIT / "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "/" / "=" / "?" "^" / "_" / 
	//	"`" / "{" / "|" / "}" / "~"
	TChar ch( aLex.Get() );
	return( (ch >= 'a' && ch <= 'z' )
		|| (ch >= 'A' && ch <= 'Z' )
		|| (ch >= '0' && ch <= '9' )
		|| ch == '!'
		|| ch == '#'
		|| ch == '$'
		|| ch == '%'
		|| ch == '&'
		|| ch == '\''
		|| ch == '*'
		|| ch == '+'
		|| ch == '-'
		|| ch == '/'
		|| ch == '='
		|| ch == '?'
		|| ch == '^'
		|| ch == '_'
		|| ch == '`'
		|| ch == '{'
		|| ch == '|'
		|| ch == '}'
		|| ch == '~' );
	}

// ----------------------------------------------------
// atom := [CFWS] 1*atext [CFWS]
// ----------------------------------------------------
//
LOCAL_C TBool Atom( TLex& aLex )
	{
	DoOrReverse( CFWS, aLex );

	if( !Plus( AText, aLex ) )
		{
		return EFalse;
		}

	DoOrReverse( CFWS, aLex );
	return ETrue;
	}

// ----------------------------------------------------
// dot-atom := [CFWS] dot-atom-text [CFWS]
// ----------------------------------------------------
//
LOCAL_C TBool DotAtom( TLex& aLex )
	{
	DoOrReverse( CFWS, aLex );
	if( !DoOrReverse( DotAtomText, aLex ) )
		{
		return EFalse;
		}
	DoOrReverse( CFWS, aLex );

	return ETrue;
	}

// ----------------------------------------------------
// "." 1*atext
// ----------------------------------------------------
//
LOCAL_C TBool DotPlusAText( TLex& aLex )
	{
	return aLex.Get() == '.' && Plus( AText, aLex );
	}

// ----------------------------------------------------
// dot-atom-text := 1*atext *("." 1*atext)
// ----------------------------------------------------
//
LOCAL_C TBool DotAtomText( TLex& aLex )
	{
	if( !Plus( AText, aLex ) )
		{
		return EFalse;
		}

	Star( DotPlusAText, aLex );
	return ETrue;
	}

// ----------------------------------------------------
// qtext := NO-WS-CTL / %d33 / %d35-91 / %d93-126 ; Non white space 
// controls and The rest of the US-ASCII characters not including "\" 
// or the quote character
// ----------------------------------------------------
//
LOCAL_C TBool QText( TLex& aLex )
	{
	if( DoOrReverse( NoWSCtl, aLex ) )
		{
		return ETrue;
		}

	TChar ch( aLex.Get() );
	return ch == 33						// CSI: 47 # See a comment above.
		|| (ch >= 35 && ch <= 91)		// CSI: 47 # See a comment above.
		|| (ch >= 93 && ch <= 126);		// CSI: 47 # See a comment above.
	}

// ----------------------------------------------------
// qcontent := qtext / quoted-pair
// ----------------------------------------------------
//
LOCAL_C TBool QContent( TLex& aLex )
	{
	return DoOrReverse( QText, aLex ) || QuotedPair( aLex );
	}

// ----------------------------------------------------
// [FWS] qcontent
// ----------------------------------------------------
//
LOCAL_C TBool FWSQContent( TLex& aLex )
	{
	FWS( aLex );
	return QContent( aLex );
	}

// ----------------------------------------------------
// quoted-string := [CFWS] DQUOTE *([FWS] qcontent) [FWS] DQUOTE [CFWS]
// ----------------------------------------------------
//
LOCAL_C TBool QuotedString( TLex& aLex )
	{
	DoOrReverse( CFWS, aLex );

	if( aLex.Get() != '"' )
		{
		return EFalse;
		}

	Star( FWSQContent, aLex );

	DoOrReverse( FWS, aLex );

	if( aLex.Get() != '"' )
		{
		return EFalse;
		}

	DoOrReverse( CFWS, aLex );
	return ETrue;
	}

// ----------------------------------------------------
// word := atom / quoted-string
// ----------------------------------------------------
//
LOCAL_C TBool Word( TLex& aLex )
	{
	return DoOrReverse( Atom, aLex ) || QuotedString( aLex );
	}

// ----------------------------------------------------
// address := mailbox / group
// ----------------------------------------------------
//
LOCAL_C TBool Address( TLex& aLex )
	{
	return DoOrReverse( Mailbox, aLex ) || Group( aLex );
	}

// ----------------------------------------------------
// mailbox := name-addr / addr-spec
// ----------------------------------------------------
//
LOCAL_C TBool Mailbox( TLex& aLex )
	{
	return DoOrReverse( NameAddr, aLex ) || AddrSpec( aLex );
	}

// ----------------------------------------------------
// name-addr := [display-name] angle-addr
// ----------------------------------------------------
//
LOCAL_C TBool NameAddr( TLex& aLex )
	{
	DoOrReverse( DisplayName, aLex );
	return AngleAddr( aLex );
	}

// ----------------------------------------------------
// angle-addr := [CFWS] "<" addr-spec ">" [CFWS] / obs-angle-addr
// ----------------------------------------------------
//
LOCAL_C TBool AngleAddr( TLex& aLex )
	{
	DoOrReverse( CFWS, aLex );
	if( aLex.Get() != '<' )
		{
		return EFalse;
		}

	if( !DoOrReverse( AddrSpec, aLex ) )
		{
		return EFalse;
		}

	if( aLex.Get() != '>' )
		{
		return EFalse;
		}

	DoOrReverse( CFWS, aLex );
	return ETrue;
	}

// ----------------------------------------------------
// group := display-name ":" [mailbox-list / CFWS] ";" [CFWS]
// ----------------------------------------------------
//
LOCAL_C TBool Group( TLex& aLex )
	{
	if( !DisplayName( aLex ) )
		{
		return EFalse;
		}

	if( aLex.Get() != ':' )
		{
		return EFalse;
		}

	if( !DoOrReverse( MailboxList, aLex ) )
		{
		DoOrReverse( CFWS, aLex );
		}

	if( aLex.Get() != ';' )
		{
		return EFalse;
		}

	DoOrReverse( CFWS, aLex );
	return ETrue;
	}

// ----------------------------------------------------
// display-name := phrase
// phrase := 1*word / obs-phrase
// ----------------------------------------------------
//
LOCAL_C TBool DisplayName( TLex& aLex )
	{
	return Plus( Word, aLex );
	}

// ----------------------------------------------------
// "," mailbox
// ----------------------------------------------------
//
LOCAL_C TBool CommaMailbox( TLex& aLex )
	{
	return aLex.Get() == ',' && Mailbox( aLex );
	}

// ----------------------------------------------------
// mailbox-list := (mailbox *("," mailbox)) / obs-mbox-list
// ----------------------------------------------------
//
LOCAL_C TBool MailboxList( TLex& aLex )
	{
	if( !DoOrReverse( Mailbox, aLex ) )
		{
		return EFalse;
		}

	Star( CommaMailbox, aLex );

	return ETrue;
	}

// ----------------------------------------------------
// addr-spec := local-part "@" domain
// ----------------------------------------------------
//
LOCAL_C TBool AddrSpec( TLex& aLex )
	{
	if( !LocalPart( aLex ) )
		{
		return EFalse;
		}

	if( aLex.Get() != '@' )
		{
		return EFalse;
		}

	return Domain( aLex );
	}

// ----------------------------------------------------
// local-part := dot-atom / quoted-string / obs-local-part
// ----------------------------------------------------
//
LOCAL_C TBool LocalPart( TLex& aLex )
	{
	return DoOrReverse( DotAtom, aLex ) || QuotedString( aLex );
	}

// ----------------------------------------------------
// domain := dot-atom / domain-literal / obs-domain
// ----------------------------------------------------
//
LOCAL_C TBool Domain( TLex& aLex )
	{
	return DoOrReverse( DotAtom, aLex ) || DomainLiteral( aLex );
	}

// ----------------------------------------------------
// [FWS] dcontent
// ----------------------------------------------------
//
LOCAL_C TBool FWSDContent( TLex& aLex )
	{
	FWS( aLex );
	return DContent( aLex );
	}

// ----------------------------------------------------
// domain-literal := [CFWS] "[" *([FWS] dcontent) [FWS] "]" [CFWS]
// ----------------------------------------------------
//
LOCAL_C TBool DomainLiteral( TLex& aLex )
	{
	DoOrReverse( CFWS, aLex );
	if( aLex.Get() != '[' )
		{
		return EFalse;
		}

	Star( FWSDContent, aLex );

	DoOrReverse( FWS, aLex );
	if( aLex.Get() != ']' )
		{
		return EFalse;
		}

	DoOrReverse( CFWS, aLex );
	return ETrue;
	}

// ----------------------------------------------------
// dcontent := dtext / quoted-pair
// ----------------------------------------------------
//
LOCAL_C TBool DContent( TLex& aLex )
	{
	return DoOrReverse( DText, aLex ) || QuotedPair( aLex );
	}

// ----------------------------------------------------
// dtext := NO-WS-CTL / %d33-90 / %d94-126 ; Non white space controls and 
// the rest of the US-ASCII characters not including "[", "]", or "\"
// ----------------------------------------------------
//
LOCAL_C TBool DText( TLex& aLex )
	{
	if( DoOrReverse( NoWSCtl, aLex ) )
		{
		return ETrue;
		}

	TChar ch( aLex.Get() );
	return (ch >= 33 && ch <= 90)				// CSI: 47 # See a comment above.
		|| (ch >= 94 && ch <= 126);				// CSI: 47 # See a comment above.
	}

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor cannot contain any code that might leave
EXPORT_C CMsgMailAppUi::CMsgMailAppUi()
    : iResource(*iCoeEnv)
    {
    }

// Symbian OS default constructor can leave.
EXPORT_C void CMsgMailAppUi::ConstructL()
    {
    // the implementation of this virtual function must call 
    // CEikAppUi::ConstructL or CEikAppUi::BaseConstructL 
    // before calling any other leaving function    
    CMsgEditorAppUi::ConstructL();
    SetSmallIconL();
        
	TParse* fp = new(ELeave) TParse(); 
	fp->Set(KResourcesWithDir, &KDC_RESOURCE_FILES_DIR, NULL); 

	TFileName fileName(fp->FullName());
	delete fp;
	iResource.OpenL(fileName);
    
    // Used in derived AppUI classes
    if (FeatureManager::FeatureSupported( KFeatureIdHelp ))
    	{
    	iCommonFlags |= EHelpSupported;
    	}
    if (FeatureManager::FeatureSupported( KFeatureIdAvkonELaf ))
    	{
    	iCommonFlags |= EWestern;
    	}
    
    // Call PrepareToLaunchL if opened as standalone
    if( !iEikonEnv->StartedAsServerApp( ) )
    	{
       	Document()->PrepareToLaunchL( this );
       	}
  
    }

    
// Destructor
EXPORT_C CMsgMailAppUi::~CMsgMailAppUi()
    {
    delete iBannedChars;
    iResource.Close();
    }


// ----------------------------------------------------------------------------
// CMsgMailAppUi::MsgSaveL()
// Saves message header, body, attachments and send options
// in message store.
// Argument:
// TBool       aInPreparation
// In certain cases (e.g. printing) new message has to be saved temporarily,
// then aInPreparation flag is set to ETrue (default is EEalse) so automatic
// cleanup is made by the server if something goes wrong.
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CMsgMailAppUi::MsgSaveL(const TBool aInPreparation, 
                                      const TBool aReply /*= EFalse*/)
    {
    TBool saved(ETrue);
    if ( Document()->MediaAvailable() )
        {
        saved = DoSaveMessageL( aInPreparation, aReply );
        }
    return saved;    
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::CheckStoreSpaceL()
// -----------------------------------------------------------------------------
//
void CMsgMailAppUi::CheckStoreSpaceL( TMsvEmailEntry aMessage, TBool aReply )
    {
    // calculate message size
    // if forwarding HTML message, all attachments are copied.
    TBool countAttachments( ( aMessage.MHTMLEmail() && !aReply ) );
    TInt docSize( MessageSizeL( countAttachments ) );
    LOG1( "CMsgMailAppUi::CheckStoreSpaceL: %d", docSize );
    
    MailDocument()->DiskSpaceBelowCriticalLevelL( docSize );
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::SetServiceIdL()
// -----------------------------------------------------------------------------
//
TBool CMsgMailAppUi::SetServiceIdL( 
    TMsvEmailEntry& aEntry, CMsgMailDocument& aDocument )
	{
	TBool returnValue( ETrue );

	// set related service ID
	CMsvEntry* rootEntry = 
		aDocument.Session().GetEntryL(KMsvRootIndexEntryIdValue);
	CleanupStack::PushL(rootEntry);
	rootEntry->SetSortTypeL(TMsvSelectionOrdering(KMsvNoGrouping,
		EMsvSortByNone,ETrue));    
	
	TMsvEntry serviceEntry;
	TRAPD(err, serviceEntry = rootEntry->ChildDataL(aEntry.iServiceId));
	CMsgMailPreferences& pref = aDocument.SendOptions();
	if (err == KErrNotFound) // mailbox deleted, look for default
		{
		const TMsvId defaultId(
            MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
				aDocument.Session(), KUidMsgTypeSMTP, ETrue));
		if (defaultId == KMsvUnknownServiceIndexEntryId)
			{ // no mailboxes defined
			returnValue = EFalse;
			}
		else // set default mailbox's values to entry
			{
			serviceEntry = rootEntry->ChildDataL(defaultId);
			aEntry.iServiceId = defaultId;
			pref.SetServiceId(defaultId);
			}
		}
    aEntry.iRelatedId = serviceEntry.iRelatedId;        

	CleanupStack::PopAndDestroy(); // rootEntry
	return returnValue;
	}

// -----------------------------------------------------------------------------
// CMsgMailAppUi::SetEntryTimeL()
// -----------------------------------------------------------------------------
//
void CMsgMailAppUi::SetEntryTimeL( TMsvEntry& aEntry )
    {
    TInt muiuFlags(0);
	MailDocument()->MailCRHandler()->GetCRInt(
		KCRUidMuiuVariation, 
		KMuiuMceFeatures, muiuFlags);
	
	if ( (muiuFlags & KMceFeatureIdUseUtcTime) )
		{
		LOG( "CMsgMailAppUi::SetEntryTimeL UTC" );
		aEntry.iDate.UniversalTime();
		}
	else
		{
		aEntry.iDate.HomeTime(); 
		}    
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::SetEntryFlagsL()
// -----------------------------------------------------------------------------
//
void CMsgMailAppUi::SetEntryFlagsL( 
    TMsvEmailEntry& aEntry, TMsvId aOrigMessageId )
    {
    CMsgMailDocument* doc = MailDocument();
    // buf must be available when ChangeL() is called
    HBufC* buf = NULL;
    // update entry details
    CMsgAddressControl* control = AddressControl( EMsgComponentIdTo );    
    if( control )
        {
        buf = MakeDetailsLC( *control );
        aEntry.iDetails.Set( *buf );
        }
    else 
        {
        control = AddressControl( EMsgComponentIdFrom );
        if( control )
            {
            buf = MakeDetailsLC( *control );
            TPtr bufPtr( buf->Des() );
            StripIllegalCharsL( bufPtr );    
            aEntry.iDetails.Set( bufPtr );
            }
        }
    
    // calculate new message size 
    aEntry.iSize = MessageSizeL( ETrue );
    //aEntry.iSize += doc->Header().DataSize();
    LOG1( "CMsgMailAppUi::SetEntryFlagsL entry size: %d", aEntry.iSize );
    
    // changed flag
    doc->SetChanged(EFalse);

    if (aOrigMessageId != doc->Entry().Id())
        {
        doc->SetEntryL( aOrigMessageId );
        }
    doc->CurrentEntry().ChangeL( aEntry );
    //  LockEntryL();

    // buf must be available when ChangeL() is called
    if (buf)
        {
        CleanupStack::PopAndDestroy();
        }
    LOG1( "CMsgMailAppUi::SetEntryFlagsL entry size final: %d", aEntry.iSize );        
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::SetHeaderFieldsL()
// -----------------------------------------------------------------------------
//
void CMsgMailAppUi::SetHeaderFieldsL( TMsvEmailEntry& aMessage )
    {
    CMsgMailDocument* doc = MailDocument();
    CImHeader& header = doc->HeaderL();
    // set subject to header & entry description
    if (SubjectControl())
    	{
    	CRichText& subject = SubjectControl()->TextContent();
    	header.SetSubjectL(subject.Read(0, subject.DocumentLength()));
        
        // update entry description
    	aMessage.iDescription.Set(subject.Read(0, subject.DocumentLength()));
    	}
    else
        {
        header.SetSubjectL(KNullDesC);
        aMessage.iDescription.Set(KNullDesC);
        }

    iNeedRefresh = EFalse;
    
    TInt recipientCount(0); 
    CMsgAddressControl* toControl = AddressControl(EMsgComponentIdTo);
    AppendRecipientsL( toControl, header.ToRecipients() );
    recipientCount += header.ToRecipients().Count();
    
	CMsgAddressControl* ccControl = AddressControl(EMsgComponentIdCc);
	AppendRecipientsL( ccControl, header.CcRecipients() );
    recipientCount += header.CcRecipients().Count();
            
    CMsgAddressControl* bccControl = AddressControl(EMsgComponentIdBcc);
    AppendRecipientsL( bccControl, header.BccRecipients() );
    recipientCount += header.BccRecipients().Count();
    
    // Set multiblerecipient flag 
    aMessage.SetMultipleRecipients( ( recipientCount > 1 ) );
        
    CMsgAddressControl* fromControl = AddressControl(EMsgComponentIdFrom);
    if (fromControl)
        {
        CMsgRecipientArray* fromAddress = fromControl->GetRecipientsL();
        if (fromAddress->Count() > 0)
            {
            CMsgRecipientItem* recItem = fromAddress->At(0);
            HBufC* fullAddress = ConstructAddressStringLC(recItem);
            header.SetFromL(*fullAddress);
            CleanupStack::PopAndDestroy(); // fullAddress
            }
        if (iNeedRefresh)
            {
            fromControl->RefreshL(*fromAddress);
            iNeedRefresh = EFalse;
            }
        }

    // save CImHeader information to current entry's store
    doc->SaveHeaderToStoreL();
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::SetSmallIconL()
// -----------------------------------------------------------------------------
//
void CMsgMailAppUi::SetSmallIconL()
    {
    CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
        ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );    
    if( titlePane )
        {
        TParse fileParse;
        fileParse.Set( KMsgMailMBMFileName, &KDC_APP_BITMAP_DIR, NULL );     
        CGulIcon* image = AknsUtils::CreateGulIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDQgnPropMceEmailTitle,
            fileParse.FullName(),
            EMbmMsgmailutilsQgn_prop_mce_email_title,
            EMbmMsgmailutilsQgn_prop_mce_email_title_mask
            );
        TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ETitlePane, mainPane );
        TAknLayoutRect titleIconPaneLayoutRect;
        titleIconPaneLayoutRect.LayoutRect(
            mainPane,
            AknLayoutScalable_Avkon::title_pane_g2( 0 ).LayoutLine() );
    	TSize iconSize = titleIconPaneLayoutRect.Rect( ).Size( );
    	AknIconUtils::SetSize( image->Bitmap(), iconSize, EAspectRatioPreserved );

        image->SetBitmapsOwnedExternally( ETrue );	
    	// takes ownership of bitmaps. Be sure nothing leaves before this
    	titlePane->SetSmallPicture( image->Bitmap(), image->Mask(), ETrue );
        delete image;    	
        }
    }    
// -----------------------------------------------------------------------------
// CMsgMailAppUi::MailDocument()
// -----------------------------------------------------------------------------
//
CMsgMailDocument* CMsgMailAppUi::MailDocument()
    {
    return static_cast<CMsgMailDocument*> ( Document() );    
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::DoSaveMessageL()
// -----------------------------------------------------------------------------
//
TBool CMsgMailAppUi::DoSaveMessageL( TBool aInPreparation, TBool aReply )
    {
    CMsgMailDocument* doc = MailDocument();
    if ( doc->CurrentEntry().Entry().Id() != iMessageID )
        {
        doc->SetEntryL( iMessageID );
        }
    TMsvEmailEntry entry(doc->CurrentEntry().Entry());  
    LOG1( "CMsgMailAppUi::DoSaveMessageL entry size: %d", entry.iSize );
    entry.SetBodyTextComplete( ETrue );
    
    ASSERT( entry.Id() == iMessageID );
    CheckStoreSpaceL( entry, aReply );

    // set preparation flag
    if (!entry.MHTMLEmail() && !aReply)
        {
        entry.SetInPreparation(aInPreparation);    
        // set visible if not in preparation state
        entry.SetVisible(!aInPreparation);
        UnlockEntry();
        doc->CurrentEntry().ChangeL(entry);
        }
    //-------------------------------
    // 1. set variables
    //-------------------------------        
    TBool returnValue(ETrue);
        
    //-------------------------------------
    // 2. create new message id, if needed
    //-------------------------------------    
    const TMsvId origMessageId(entry.Id());

    entry.iType = KUidMsvMessageEntry;   
	CMsgMailPreferences& pref = doc->SendOptions();    
	
        
	// EMail type       
	entry.iMtm = KUidMsgTypeSMTP;
	// set related service ID
	returnValue = SetServiceIdL( entry, *doc );
        
    // set Send options
    pref.ImportSendOptionsL(entry);        
	// set entry time
    SetEntryTimeL( entry );

    //-------------------------------
    // 3. set CImHeader fields
    //-------------------------------    
    SetHeaderFieldsL( entry );

    // store message body
    CMsgBodyControl* bodyControl = BodyControl();
    if (bodyControl)
    	{
    	CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    	doc->MessageL().StoreBodyTextL( 
    	    origMessageId,
        	bodyControl->TextContent(), 
        	wait->iStatus);
        	
    	wait->Start();
    	CleanupStack::PopAndDestroy(); //wait
    	}

    //-------------------------------
    // 5. update the entry
    //-------------------------------
    SetEntryFlagsL( entry, origMessageId );

    return returnValue;
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::ConstructAddressStringLC()
// -----------------------------------------------------------------------------
//
HBufC* CMsgMailAppUi::ConstructAddressStringLC(CMsgRecipientItem* aRecItem)
    {   
    HBufC* fullAddress = NULL;    

    // remove illegal characters from user inputted addresses
    if (!aRecItem->Name()->Length())
        {
        TPtr address(aRecItem->Address()->Des());
        // won't overflow, only removing characters
        if (StripIllegalCharsL(address))
            {
            aRecItem->SetAddressL(address);
            }
        }
    
    // check that address item has alias and address and they are not the same
    if (aRecItem->Name()->Length() &&
        aRecItem->Name()->Compare(*(aRecItem->Address())) != 0)
        {
        // 2 Array for name and address
        CDesCArrayFlat* strings = new(ELeave) CDesCArrayFlat( 2 );		// CSI: 47 # See a comment above.
        CleanupStack::PushL( strings );
        
        strings->AppendL( *aRecItem->Name() );
        strings->AppendL( *aRecItem->Address() );

        fullAddress = StringLoader::LoadL(
            R_MAIL_ADDRESS_FORMAT_STRING, *strings, iCoeEnv);
        CleanupStack::PopAndDestroy(); // strings
        CleanupStack::PushL( fullAddress );
        }
    // otherwise return just address
    else
        {
        fullAddress = aRecItem->Address()->AllocLC();
        }
    return fullAddress;
    }
    
// -----------------------------------------------------------------------------
// CMsgMailAppUi::StripIllegalCharsL()
// -----------------------------------------------------------------------------
//
TBool CMsgMailAppUi::StripIllegalCharsL(TDes& aString)
    {
    TBool returnValue(EFalse);
    // initialise array when called for the 1st time
    if (!iBannedChars)
        {   
        iBannedChars = iEikonEnv->ReadDesC16ArrayResourceL(
            R_MAIL_BANNED_CHARS );
        iBannedChars->Compress();
        }

    // Check that inputted address does not contain illegal characters
    // These characters would mess up address inputting if they were saved
    const TInt count(iBannedChars->Count());
    for (TInt i = 0; i < count; i++)
        {    
        TInt pos(aString.Find((*iBannedChars)[i]));    
        while (pos != KErrNotFound)
            {
            aString.Delete(pos, 1);
            pos = aString.Find((*iBannedChars)[i]);
            iNeedRefresh = ETrue;
            returnValue = ETrue;
            }
        }

	aString.Trim();
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::AppendRecipientsL()
// -----------------------------------------------------------------------------
//
void CMsgMailAppUi::AppendRecipientsL( 
    CMsgAddressControl* aField, CDesCArray& aArray )
	{
    aArray.Reset();	
	
	if ( aField )
	    {
    	const CMsgRecipientArray* recipients = aField->GetRecipientsL();
    	const TInt count(recipients->Count());
    	for (TInt i=0; i < count; i++)
    		{
    		CMsgRecipientItem* recItem = recipients->At(i);
    		HBufC* fullAddress = ConstructAddressStringLC(recItem);
    		aArray.AppendL(*fullAddress);
    		CleanupStack::PopAndDestroy(); // fullAddress
    		}

    	if (iNeedRefresh)
    		{
    		aField->RefreshL(*recipients);
    		iNeedRefresh = EFalse;
    		}
	    }
	}

// -----------------------------------------------------------------------------
// CMsgMailAppUi::CheckFFSL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMsgMailAppUi::CheckFFSL(const TInt aSize)
    {
    return MailDocument()->DiskSpaceBelowCriticalLevelL( aSize );
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::IsBannedMimeL()
// -----------------------------------------------------------------------------
//    
EXPORT_C TBool CMsgMailAppUi::IsBannedMimeL(const TDesC& /*aBuf*/) const
    {
    // Use MailUtils::IsClosedFileL instead of this 
    User::Leave( KErrNotSupported );
    return EFalse;
    }
// -----------------------------------------------------------------------------
// CMsgMailAppUi::IsValidAddress()
// -----------------------------------------------------------------------------
// 
EXPORT_C TBool CMsgMailAppUi::IsValidAddress( const TDesC& aAddress ) const
	{
	TLex lex( aAddress );
	return Address( lex ) && lex.Eos();
	}

// -----------------------------------------------------------------------------
// CMsgMailAppUi::CheckIfBannedL()
// -----------------------------------------------------------------------------
// 
EXPORT_C TBool CMsgMailAppUi::CheckIfBannedL(const TDesC& /*aMimeType*/)
    {
    // Use MailUtils::IsClosedFileL instead of this
    User::Leave( KErrNotSupported );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::CheckIfBannedL()
// -----------------------------------------------------------------------------
// 
EXPORT_C TBool CMsgMailAppUi::CheckIfBannedL(RFile& /*aFile*/)
    {
    // Use MailUtils::IsClosedFileL instead of this
    User::Leave( KErrNotSupported );
    return EFalse;		
    }    

// -----------------------------------------------------------------------------
// CMsgMailAppUi::CleanupWaitNoteWrapper()
// -----------------------------------------------------------------------------
// 
EXPORT_C void CMsgMailAppUi::CleanupWaitNoteWrapper(TAny* aAny)
    {
    CAknWaitNoteWrapper** wrapper = STATIC_CAST(CAknWaitNoteWrapper**, aAny);
    if (wrapper && *wrapper)
        {
        delete *wrapper;
        *wrapper = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::CleanupWaitDialog()
// -----------------------------------------------------------------------------
// 
EXPORT_C void CMsgMailAppUi::CleanupWaitDialog(TAny* aAny)
    {
    CAknWaitDialog** dialog = STATIC_CAST(CAknWaitDialog**, aAny);
    if (dialog && *dialog)
        {
        delete *dialog;
        *dialog = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::MessageSizeL()
// -----------------------------------------------------------------------------
//     
EXPORT_C TInt CMsgMailAppUi::MessageSizeL( TBool aCountAttachments )	// CSI: 40 # We must return 
																		// the integer value although this 
																		// is a leaving method.
    {
    // calculate message size 
    const TInt multibleByTwoForUnicode(2);

    TInt fieldSize(0);
    TInt count;

    CMsgAddressControl* addressField;
    
    addressField = AddressControl( EMsgComponentIdTo );
    if (addressField)
        {
        addressField->GetSizeOfAddresses(count, fieldSize);
        }
    TInt docSize(fieldSize);
    addressField = AddressControl( EMsgComponentIdCc );
    if (addressField)
        {
        addressField->GetSizeOfAddresses(count, fieldSize);
        docSize += fieldSize;
        }

    addressField = AddressControl( EMsgComponentIdBcc );
    if (addressField)
        {
        addressField->GetSizeOfAddresses(count, fieldSize);
        docSize += fieldSize;
        }

    if( SubjectControl() )
        {
        docSize += SubjectControl()->TextContent().DocumentLength() 
            * multibleByTwoForUnicode;
        }    
    CRichText& rtf = BodyControl()->TextContent();
    docSize += rtf.DocumentLength() * multibleByTwoForUnicode;
    
    // Attachments
    if ( aCountAttachments )
        {        
        CMsgAttachmentModel& attModel = Document()->AttachmentModel();
        const TInt attCount( attModel.NumberOfItems() );    
        
        for (TInt cc=0; cc < attCount; cc++)
            {
            docSize += attModel.AttachmentInfoAt(cc).Size();
            }
        }    
    
    LOG1( "CMsgMailAppUi::MessageSizeL: %d", docSize );
    return docSize;     
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::SetDialerEnabled()
// -----------------------------------------------------------------------------
// 
EXPORT_C void CMsgMailAppUi::SetDialerEnabled( TBool aEnable )
    {
    if( aEnable )
        {
        // send key handling enabled
        SetKeyEventFlags( 0 );
        }
    else
        {
        // send key handling disabled
        SetKeyEventFlags( 
            CAknAppUiBase::EDisableSendKeyShort | 
            CAknAppUiBase::EDisableSendKeyLong );        
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailAppUi::MakeDetailsLC()
// -----------------------------------------------------------------------------
// 
HBufC* CMsgMailAppUi::MakeDetailsLC( CMsgAddressControl& aControl )
    {
    // Array is owned by the control
    CMsgRecipientArray* recipients = aControl.GetRecipientsL();
    TInt recipCount = recipients->Count();
    
    LOG1( "CMsgMailAppUi::MakeDetailsLC: %d Recipient(s)", recipCount );        
    if( recipCount == 0 )
        {
        // no recipients
        return KNullDesC().AllocLC();
        }
    
    // Append recipients to text.
    HBufC* detailsBuf = HBufC::NewLC( KMsgMaxDetailsLength );
    TPtr details( detailsBuf->Des() );
    TInt separatorLen = KMsgDetailsSeparator().Length();
    
    for( TInt i = 0; i < recipCount; ++i )
        {
        CMsgRecipientItem* item = recipients->At(i);

        // Select name or address (prefer name if available)
        HBufC* textToAdd = item->Name();
        if( !textToAdd )
            {
            textToAdd = item->Address();
            }
        else if( textToAdd->Length() == 0 )
            {
            textToAdd = item->Address();
            }
        
        if( textToAdd )
            {
            TInt textSpace = KMsgMaxDetailsLength - details.Length();
            
            // Append separator
            if( textSpace < separatorLen )
                {
                // no space for separator -> all done
                break;
                }
            if( details.Length() > 0 )
                {
                // not a first recipient -> add separator
                details.Append( KMsgDetailsSeparator );
                textSpace -= separatorLen;
                }
            
            // Append recipient's name or address
            details.Append( textToAdd->Left( textSpace ) );
            
            if( KMsgMaxDetailsLength - details.Length() <= 0 )
                {
                // No more space -> all done
                break;
                }
            }
        }
    
    return detailsBuf;
    }


//  End of File  
