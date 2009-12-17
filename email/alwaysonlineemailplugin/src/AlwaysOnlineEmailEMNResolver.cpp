/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Handles OMA Email Notification messages.
*
*/


// INCLUDE FILES
#include <e32base.h>

#include <AlwaysOnlineManagerCommon.h>
#include <ImumInSettingsKeys.h>
#include <ImumInMailboxServices.h>


#include "AlwaysOnlineEmailEMNResolver.h"
#include "AlwaysOnlineEmailPluginLogging.h"
#include "AlwaysOnlineEmailLoggingTools.h"

// Literals used when trying to match a mailbox 
// to a received EMN message
_LIT(KMailatPattern, "*mailat:*@*");
_LIT(KImapUserPattern, "*imap://*@*");  // Same as KImapPattern, but includes also username
_LIT(KPopUserPattern, "*pop://*;*@*");      // Include username and auth
_LIT(KPopUserNoAuthPattern, "*pop://*@*");  // Same as KPopUserPattern, but does not include auth
// Literals used when trying to parse username, server
_LIT(KForwardSlashes, "//");
_LIT(KAtSign, "@");
_LIT(KSemicolon, ";");

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailEMNResolver::CAlwaysOnlineEmailEMNResolver()
// ----------------------------------------------------------------------------
//
CAlwaysOnlineEmailEMNResolver::CAlwaysOnlineEmailEMNResolver()
    {
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailEMNResolver::~CAlwaysOnlineEmailEMNResolver()
// ----------------------------------------------------------------------------
//
CAlwaysOnlineEmailEMNResolver::~CAlwaysOnlineEmailEMNResolver()
    {
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailEMNResolver::ConstructL()
// ----------------------------------------------------------------------------
//
void CAlwaysOnlineEmailEMNResolver::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailEMNResolver::NewL()
// ----------------------------------------------------------------------------
//
CAlwaysOnlineEmailEMNResolver* CAlwaysOnlineEmailEMNResolver::NewL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailEMNResolver::NewL" );
    CAlwaysOnlineEmailEMNResolver* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailEMNResolver::NewLC()
// ----------------------------------------------------------------------------
//
CAlwaysOnlineEmailEMNResolver* CAlwaysOnlineEmailEMNResolver::NewLC()
    {
    AOLOG_IN( "CAlwaysOnlineEmailEMNResolver::NewLC" );
    CAlwaysOnlineEmailEMNResolver* self = 
        new ( ELeave ) CAlwaysOnlineEmailEMNResolver();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

//-----------------------------------------------------------------------------
// CAlwaysOnlineEmailEMNResolver::FindEMNMailbox
//-----------------------------------------------------------------------------
CAlwaysOnlineEmailAgentBase* CAlwaysOnlineEmailEMNResolver::FindEMNMailbox( 
    TDesC& aMailboxURI, CAOEmailAgentArray& aMailAgentArray )
    {
    AOLOG_IN( "CAlwaysOnlineEmailEMNResolver::FindEMNMailbox" );
    CAlwaysOnlineEmailAgentBase* mailAgent = NULL;
    TImumDaSettings::TTextEmailAddress emailAddress;
    TImumDaSettings::TTextServerAddress incomingServer;
    TImumDaSettings::TTextUserName username;    
    
    const TInt count = aMailAgentArray.Count();

    for ( TInt loop = 0; loop < count && !mailAgent; loop++)
        {
        TBool isEmn = aMailAgentArray[loop]->IsEmn();
        
        // We are only interested of those mailboxes which have EMN on.
        if ( isEmn )
            {
            // Get needed settings
            aMailAgentArray[loop]->EmailAddress( emailAddress );
            aMailAgentArray[loop]->ServerAddress( incomingServer );
            aMailAgentArray[loop]->Username( username );
            TBool isImap = aMailAgentArray[loop]->IsImap4();
            
            KAOEMAIL_LOGGER_WRITE_FORMAT("aMailboxURI = %s", aMailboxURI.Ptr() );
            
            // <emn mailbox="mailat:username@mail.somehost.com"/>
            if ( aMailboxURI.Match( KMailatPattern ) == 0 && 
                 aMailboxURI.Find( emailAddress ) > 0 )
                {
                mailAgent = aMailAgentArray[loop];
                }
            // <emn mailbox="imap://username@mail.somehost.com"/>
            else if ( aMailboxURI.Match( KImapUserPattern ) == 0 && isImap )
                {
                if ( HandleEmnImapUserURI( aMailboxURI, username, incomingServer ) )
                    {
                    mailAgent = aMailAgentArray[loop];
                    }
                }
            // <emn mailbox="pop://userxyz;auth=3598302@mail.somehost.com"/>
            else if ( aMailboxURI.Match( KPopUserPattern ) == 0 && !isImap )
                {
                if ( HandleEmnPopUserURI( aMailboxURI, username, incomingServer ) )
                    {
                    mailAgent = aMailAgentArray[loop];
                    }
                }
            // <emn mailbox="pop://userxyz@mail.somehost.com"/>
            else if ( aMailboxURI.Match( KPopUserNoAuthPattern ) == 0 && !isImap )
                {
                if ( HandleEmnPopNoAuthURI( aMailboxURI, username, incomingServer ) )
                    {
                    mailAgent = aMailAgentArray[loop];
                    }
                }
            else
                {
                KAOEMAIL_LOGGER_WRITE_FORMAT("Mailbox 0x%x is EMN mailbox, but there was no match", aMailAgentArray[loop]->MailboxId() );
                }
            }
        else
            {
            KAOEMAIL_LOGGER_WRITE_FORMAT("Mailbox 0x%x is not EMN mailbox", aMailAgentArray[loop]->MailboxId() );
            }
        }
    
    return mailAgent;  
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailEMNResolver::ParameterDispatchTEMNElement
// ----------------------------------------------------------------------------
TInt CAlwaysOnlineEmailEMNResolver::ParameterDispatchTEMNElement( 
    const TDesC8& aParameters,
    TEMNElement& aElement ) const
    {
    AOLOG_IN( "CAlwaysOnlineEmailEMNResolver::ParameterDispatchTEMNElement" );
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailEMNResolver::ParameterDispatchTEMNElement(): Dispatch mailbox URI and timestamp from parameters");
    AOLOG_WRV( "Dispatch mailbox URI and timestamp from parameters", EAoLogSt3 );

    // Presume that wrong type of parameter is given
    TInt err = KErrNotSupported;

    // Unpack parameters to see to which mailbox EMN is for
    TPckgBuf<TEMNElement> paramPack;

    // Make sure that the parameter length matches TEMNElement length and
    // extract mailbox and timestamp values.
    if ( aParameters.Size() == sizeof( TEMNElement ) )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailEMNResolver::ParameterDispatchTEMNElement(): EMN parameters not corrupted, extracting... " );
        AOLOG_WRV( "EMN parameters not corrupted, extracting...", EAoLogSt3 );

        paramPack.Copy( aParameters );
        
        aElement.mailbox.Zero();
        aElement.mailbox.Append( paramPack().mailbox );
        
        aElement.timestamp = paramPack().timestamp;
                
        err = KErrNone;
        }
    else
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailEMNResolver::ParameterDispatchTEMNElement(): Corrupted EMN parameters?!?!");
        }

    return err;
    }
    
//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailEMNResolver::HandleEmnImapUserURI
// ----------------------------------------------------------------------------
TBool CAlwaysOnlineEmailEMNResolver::HandleEmnImapUserURI(
    const TDesC& aURI,
    const TDesC8& aUsername,
    const TDesC& aServer ) const    
    {
    AOLOG_IN( "CAlwaysOnlineEmailEMNResolver::HandleEmnImapUserURI" );
    // Try to match with following EMN message
    // <emn mailbox="imap://username@mail.somehost.com/folder;UIDVALIDITY=385759045/;UID=20"/>
    // <emn mailbox="imap://username@mail.somehost.com/"/>
    
    // endPart = username@mail.somehost.com/folder;UIDVALIDITY=385759045/;UID=20
    TInt pos = aURI.Find( KForwardSlashes );
    // Step over forward slashes
    pos += 2;   // CSI: 47 # see comment above
    // Strip "imap://" away
    TPtrC endPart( aURI.Right( aURI.Length() - ( pos ) ) );

    // Username is from beginning to @ character
    pos = endPart.Find( KAtSign );    
    TImumDaSettings::TTextUserName user;
    user.Copy( endPart.Left( pos ) ); 
    
    // Strip username and '@' character away
    endPart.Set( endPart.Right( endPart.Length() - pos - 1 ) );
    TPtrC server( endPart );
    
    // Do we have a winner?
    return ( aUsername.Compare( user ) == 0 &&
             server.Find( aServer ) == 0 );
    }
    
// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailEMNResolver::HandleEmnPopUserURI
// ----------------------------------------------------------------------------
TBool CAlwaysOnlineEmailEMNResolver::HandleEmnPopUserURI(
    const TDesC& aURI,
    const TDesC8& aUsername,
    const TDesC& aServer ) const
    {
    AOLOG_IN( "CAlwaysOnlineEmailEMNResolver::HandleEmnPopUserURI" );
    // Try to match with following EMN message
    // <emn mailbox="pop://userxyz;auth=3598302@mail.somehost.com"/>
    
    TInt pos = aURI.Find( KForwardSlashes );
    // Step over forward slashes
    pos += 2;   // CSI: 47 # see comment above
    // Strip "pop://" away
    TPtrC endPart( aURI.Right( aURI.Length() - ( pos ) ) );
    
    // Username is from beginning to ; character
    pos = endPart.Find( KSemicolon );    
    TImumDaSettings::TTextUserName user;
    user.Copy( endPart.Left( pos ) ); 

    // ";auth=3598302" omitted
    
    // server name
    pos = endPart.Find( KAtSign );
    // Step over at sign
    pos++;
    
    // server = mail.somehost.com
    TPtrC server( endPart.Right( endPart.Length() - pos ) );
    
    // Do we have a winner?
    return ( aUsername.Compare( user ) == 0 &&
             aServer.Compare( server ) == 0 );
    }
    
// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailEMNResolver::HandleEmnPopNoAuthURI
// ----------------------------------------------------------------------------
TBool CAlwaysOnlineEmailEMNResolver::HandleEmnPopNoAuthURI( 
    const TDesC& aURI,
    const TDesC8& aUsername,
    const TDesC& aServer ) const
    {
    AOLOG_IN( "CAlwaysOnlineEmailEMNResolver::HandleEmnPopNoAuthURI" );
    // Try to match with following EMN message
    // <emn mailbox="pop://userxyz@mail.somehost.com"/>

    TInt pos = aURI.Find( KForwardSlashes );
    // Step over forward slashes
    pos += 2;   // CSI: 47 # see comment above
    // Strip "pop://" away
    TPtrC endPart( aURI.Right( aURI.Length() - ( pos ) ) );
        
    // Username is from beginning to @ character
    pos = endPart.Find( KAtSign );
    TImumDaSettings::TTextUserName user;
    user.Copy( endPart.Left( pos ) ); 
    // Step over at sign
    pos++;
    // server = mail.somehost.com
    TPtrC server( endPart.Right( endPart.Length() - pos ) );    

    // Do we have a winner?
    return ( aUsername.Compare( user ) == 0 &&
             aServer.Compare( server ) == 0 );

    }

// End of File
