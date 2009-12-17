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
* Description:  ImumInMailboxServicesImpl.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <SendUiConsts.h>
#include <muiuemailtools.h>
#include <MuiuMsvProgressReporterOperation.h>
#include <avkon.mbg>
#include <featmgr.h>
#include <MessagingDomainCRKeys.h>

#include "ImumInternalApiImpl.h"
#include "ImumInMailboxServicesImpl.h"
#include "ImumInMailboxUtilitiesImpl.h"
#include "ImumInSettingsDataImpl.h"
#include "ImumPanic.h"
#include "ImumDaErrorCodes.h"
#include "ImumMboxDefaultData.h"
#include "ImumUtilsLogging.h"
#include "ImumMboxServiceDeletion.h"

// TEMP
#include "ImumInSettingsKeys.h"

#include "IMSSettingsNoteUi.h"
#include "EmailFeatureUtils.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================



// ================ CLASS CONSTRUCTION ================



// ----------------------------------------------------------------------------
// CImumInMailboxServicesImpl::CImumInMailboxServicesImpl()
// ----------------------------------------------------------------------------
//
CImumInMailboxServicesImpl::CImumInMailboxServicesImpl( CImumInternalApiImpl& aInternalApi )
    :
    iInternalApi( aInternalApi ),
    iDefaultData( NULL )
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::CImumInMailboxServicesImpl, 0, KLogInApi );
    IMUM_IN();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInMailboxServicesImpl::~CImumInMailboxServicesImpl()
// ----------------------------------------------------------------------------
//
CImumInMailboxServicesImpl::~CImumInMailboxServicesImpl()
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::~CImumInMailboxServicesImpl, 0, KLogInApi );
    IMUM_IN();

    delete iDefaultData;
    iDefaultData = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInMailboxServicesImpl::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumInMailboxServicesImpl::ConstructL()
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::ConstructL, 0, KLogInApi );
    IMUM_IN();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInMailboxServicesImpl::NewL()
// ----------------------------------------------------------------------------
//
CImumInMailboxServicesImpl* CImumInMailboxServicesImpl::NewL(
    CImumInternalApiImpl& aInternalApi )
    {
    IMUM_STATIC_CONTEXT( CImumInMailboxServicesImpl::NewL, 0, utils, KLogInApi );
    IMUM_IN();

    CImumInMailboxServicesImpl* self = NewLC( aInternalApi );
    CleanupStack::Pop( self );

    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumInMailboxServicesImpl::NewLC()
// ----------------------------------------------------------------------------
//
CImumInMailboxServicesImpl* CImumInMailboxServicesImpl::NewLC(
    CImumInternalApiImpl& aInternalApi )
    {
    IMUM_STATIC_CONTEXT( CImumInMailboxServicesImpl::NewLC, 0, utils, KLogInApi );
    IMUM_IN();

    CImumInMailboxServicesImpl* self =
        new ( ELeave ) CImumInMailboxServicesImpl( aInternalApi );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMUM_OUT();

    return self;
    }



// ================ INTERFACE IMPLEMENTATION ================



// ---------------------------------------------------------------------------
// From class MImumInMailboxServices.
// CImumInMailboxServicesImpl::CreateSettingsDataL()
// ---------------------------------------------------------------------------
//
CImumInSettingsData* CImumInMailboxServicesImpl::CreateSettingsDataL(
    const TUid& aProtocol ) const
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::CreateSettingsDataL, 0, KLogInApi );
    IMUM_IN();

    CImumInSettingsData* data = NULL;

    // Allow empty and valid mailbox mtm's
    if ( !aProtocol.iUid ||
         iInternalApi.MailboxUtilitiesL().IsMailMtm( aProtocol, ETrue ) )
        {
        CImumInMailboxServicesImpl* me =
            const_cast<CImumInMailboxServicesImpl*>( this );
        data = CImumInSettingsDataImpl::NewL(
            iInternalApi, aProtocol, me->DefaultDataObjectL() );
        }
    else
        {
        ServiceExceptionL( TImumDaErrorCode::EUnsupportedProtocol );
        }

    IMUM_OUT();

    return data;
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxServices.
// CImumInMailboxServicesImpl::CreateSettingsDataL()
// ---------------------------------------------------------------------------
//
CImumInSettingsData* CImumInMailboxServicesImpl::CreateSettingsDataL(
    const TUid& aProtocol,
    const TDesC& aEmailAddress,
    const TDesC& aIncomingServer,
    const TDesC& aOutgoingServer,
    const TInt aAccessPoint,
    const TDesC& aMailboxName ) const
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::CreateSettingsDataL, 0, KLogInApi );
    IMUM_IN();

    CImumInSettingsData* data = CreateSettingsDataL( aProtocol );
    CleanupStack::PushL( data );

    data->SetAttr( TImumDaSettings::EKeyMailboxName, aMailboxName );
    data->SetAttr( TImumDaSettings::EKeyEmailAddress, aEmailAddress );
    data->GetSetL( 0 ).SetAttr(
        TImumDaSettings::EKeyServer, aIncomingServer );
    data->GetSetL( 0 ).SetAttr(
        TImumDaSettings::EKeyAccessPoint, aAccessPoint );

    data->GetSetL( 1 ).SetAttr(
        TImumDaSettings::EKeyServer, aOutgoingServer );
    data->GetSetL( 1 ).SetAttr(
        TImumDaSettings::EKeyAccessPoint, aAccessPoint );

    CleanupStack::Pop( data );

    IMUM_OUT();

    return data;
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxServices.
// CImumInMailboxServicesImpl::LoadMailboxSettingsL()
// ---------------------------------------------------------------------------
//
CImumInSettingsData* CImumInMailboxServicesImpl::LoadMailboxSettingsL(
    const TMsvId aMailboxId ) const
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::LoadMailboxSettingsL, 0, KLogInApi );
    IMUM_IN();

    CImumInSettingsData* data = NULL;

    if ( iInternalApi.MailboxUtilitiesL().IsMailbox( aMailboxId ) )
        {
		// Load settings only in case of IMAP, POP and SMTP
		// otherwise a null pointer is returned
        TMsvEntry receiving = iInternalApi.MailboxUtilitiesL().GetMailboxEntryL(
        		aMailboxId, MImumInMailboxUtilities::ERequestReceiving );

        if( !( ( receiving.iMtm.iUid == KSenduiMtmSmtpUidValue )
        		|| ( receiving.iMtm.iUid == KSenduiMtmImap4UidValue )
        		|| ( receiving.iMtm.iUid == KSenduiMtmPop3UidValue ) ) )
        	{
        	return data;
        	}

        data = iInternalApi.SettingsManagerL().LoadEmailSettingsL(
            aMailboxId );
        }
    else
        {
        ServiceExceptionL( TImumDaErrorCode::EIdNotRecongnized );
        }

    IMUM_OUT();

    return data;
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxServices.
// CImumInMailboxServicesImpl::SaveMailboxSettingsL()
// ---------------------------------------------------------------------------
//
void CImumInMailboxServicesImpl::SaveMailboxSettingsL(
    CImumInSettingsData& aSettingsData ) const
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::SaveMailboxSettingsL, 0, KLogInApi );
    IMUM_IN();

    ValidateSettingsL( aSettingsData );

    iInternalApi.SettingsManagerL().SaveEmailSettingsL( aSettingsData );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxServices.
// CImumInMailboxServicesImpl::CreateMailboxL()
// ---------------------------------------------------------------------------
//
TMsvId CImumInMailboxServicesImpl::CreateMailboxL(
    CImumInSettingsData& aSettingsData ) const
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::CreateMailboxL, 0, KLogInApi );
    IMUM_IN();

    ValidateSettingsL( aSettingsData );

    IMUM_OUT();

    return iInternalApi.SettingsManagerL().CreateEmailAccountL( aSettingsData );
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxServices.
// CImumInMailboxServicesImpl::RemoveMailboxL()
// ---------------------------------------------------------------------------
//
CMsvOperation* CImumInMailboxServicesImpl::RemoveMailboxL( const TMsvId aMailboxId,
														   TRequestStatus& aStatus) const
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::RemoveMailboxL, 0, KLogInApi );
    IMUM_IN();

    CMsvOperation* op = NULL;
    CMsvSession& session = iInternalApi.MsvSession();

    //remove mailbox if email settings are not locked.
    if( !iInternalApi.IsEmailFeatureSupportedL(
        CImumInternalApi::EMailLockedSettings, R_QTN_SELEC_PROTECTED_SETTING ) )
        {
        if ( iInternalApi.MailboxUtilitiesL().IsMailbox( aMailboxId ) )
            {
            if ( iInternalApi.SettingsManagerL().CheckAccountDeletionConditionsL(
                aMailboxId ) )
                {
                // The delete will take more than 1 second, set progress note
                // visibility delay off.
			    CMsvProgressReporterOperation* reporter =
		            CMsvProgressReporterOperation::NewL( ETrue,
		                ETrue, session, aStatus, EMbmAvkonQgn_note_erased );

                // Once delete is started, a cancel option is not offered to the user.
                // Set right softkey to empty for a duration CMsvProgressReporterOperation
                // runs the progress dialog.
                CEikButtonGroupContainer::Current()->SetCommandSetL(
                    R_AVKON_SOFTKEYS_EMPTY );

			    CleanupStack::PushL( reporter );

		        op = CImumMBoxServiceDeletion::NewL( session,
		    	    reporter->RequestStatus(), *reporter, aMailboxId );

			    CleanupStack::PushL( op );

		        reporter->SetOperationL( op ); // Takes immediate ownership
		        CleanupStack::Pop( op );
		        CleanupStack::Pop( reporter );
        	    op = reporter;
                }
            }
        else
            {
            ServiceExceptionL( TImumDaErrorCode::EIdNotRecongnized );
            }
        }
    IMUM_OUT();
    return op?op:CMsvCompletedOperation::NewL( session,
                                               KUidMsvLocalServiceMtm,
                                               KNullDesC8,
                                               KMsvLocalServiceIndexEntryId,
                                               aStatus );
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxServices.
// CImumInMailboxServicesImpl::SetDefaultMailboxL()
// ---------------------------------------------------------------------------
//
void CImumInMailboxServicesImpl::SetDefaultMailboxL(
    const TMsvId aMailboxId ) const
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::SetDefaultMailboxL, 0, KLogInApi );
    IMUM1_IN( "aMailboxId = 0x%x", aMailboxId );

    // Make sure valid mailbox is set as default
    if ( iInternalApi.MailboxUtilitiesL().IsMailbox( aMailboxId ) )
        {
        IMUM0( 0, "Provided id belongs to mailbox, setting as default" );
        
        // When selectable email is in use, the default mailbox is stored also 
        // directly to CenRep.
        //
        FeatureManager::InitializeLibL();
        TBool selectableEmail = 
            FeatureManager::FeatureSupported( KFeatureIdSelectableEmail );
        FeatureManager::UnInitializeLib();
        if( selectableEmail )
            {
            CRepository* rep = 
                CRepository::NewLC( KCRUidSelectableDefaultEmailSettings );
            User::LeaveIfError( 
                rep->Set( KSelectableDefaultMailAccount, (TInt)aMailboxId ) );
            CleanupStack::PopAndDestroy( rep );
            }
        
        MuiuEmailTools::SetDefaultSendingMailboxL( aMailboxId );
        }
    // No valid mailboxes found, so find the first valid and set it as default
    else
        {
        IMUM0( 0, "Provided ID doesn't belong to mailbox, find first valid" );
        MImumInHealthServices::RMailboxIdArray mailboxes;
        iInternalApi.HealthServicesL().GetMailboxList(
            mailboxes,
            MImumInHealthServices::EFlagGetHealthy |
            MImumInHealthServices::EFlagIncludeSmtp );

        if ( mailboxes.Count() )
            {
            IMUM1( 0, "WARNING: Mailboxes in system without default. New default: 0x%x", mailboxes[0] );
            MuiuEmailTools::SetDefaultSendingMailboxL( mailboxes[0] );
            }

        mailboxes.Close();
        }

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxServices.
// CImumInMailboxServicesImpl::SetLastUpdateInfoL()
// ---------------------------------------------------------------------------
//
void CImumInMailboxServicesImpl::SetLastUpdateInfoL( TMsvId aMailboxId,
    TBool aSuccess, const TTime& aTime ) const
    {
    // This function is performance optimization as settings are not converted
    // to CImumInSettingsData format
    IMUM_CONTEXT( CImumInMailboxServicesImpl::SetLastUpdateInfoL, 0, KLogInApi );
    IMUM_IN();

    // Get account extension ID
    TMsvEntry mailbox =  iInternalApi.MailboxUtilitiesL().GetMailboxEntryL(
        aMailboxId, MImumInMailboxUtilities::ERequestReceiving );

    iInternalApi.SettingsStorerL().SetLastUpdateInfoL( mailbox.iMtmData2,
        mailbox.iMtm, aSuccess, aTime );

    IMUM_OUT();
    }

// ================ INTERNAL FUNCTIONALITY ================

// ---------------------------------------------------------------------------
// CImumInMailboxServicesImpl::FillCompulsorySettingsL()
// ---------------------------------------------------------------------------
//
void CImumInMailboxServicesImpl::FillCompulsorySettingsL(
    CImumInSettingsData& aSettingsData,
    const TUid& aProtocol,
    const TDesC& aEmailAddress,
    const TDesC& aIncomingServer,
    const TDesC& aOutgoingServer,
    const TInt aAccessPoint,
    const TDesC& aMailboxName ) const
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::FillCompulsorySettingsL, 0, KLogInApi );
    IMUM_IN();

    aSettingsData.SetAttr( TImumDaSettings::EKeyMailboxName, aMailboxName );
    aSettingsData.SetAttr( TImumDaSettings::EKeyEmailAddress, aEmailAddress );

    TRAPD( error, aSettingsData.GetSetL( 0 ).SetAttr(
        TImumDaSettings::EKeyProtocol, aProtocol.iUid ) );
    if ( error == TImumDaErrorCode::EConnectionIndexOverflow )
        {
        aSettingsData.AddSetL( aProtocol );
        }
    else
        {
        ServiceExceptionL( error );
        }

    aSettingsData.GetSetL( 0 ).SetAttr(
        TImumDaSettings::EKeyServer, aIncomingServer );
    aSettingsData.GetSetL( 0 ).SetAttr(
        TImumDaSettings::EKeyAccessPoint, aAccessPoint );

    TRAP( error, aSettingsData.GetSetL( 1 ).SetAttr(
        TImumDaSettings::EKeyProtocol,
        TImumDaSettings::EValueProtocolSmtp ) );
    if ( error == TImumDaErrorCode::EConnectionIndexOverflow )
        {
        aSettingsData.AddSetL( KSenduiMtmSmtpUid );
        }
    else
        {
        ServiceExceptionL( error );
        }

    aSettingsData.GetSetL( 1 ).SetAttr(
        TImumDaSettings::EKeyServer, aOutgoingServer );
    aSettingsData.GetSetL( 1 ).SetAttr(
        TImumDaSettings::EKeyAccessPoint, aAccessPoint );

    IMUM_OUT();
    }


// ---------------------------------------------------------------------------
// CImumInMailboxServicesImpl::ValidateSettingsL()
// ---------------------------------------------------------------------------
//
void CImumInMailboxServicesImpl::ValidateSettingsL(
    CImumInSettingsData& aSettingsData ) const
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::ValidateSettingsL, 0, KLogInApi );
    IMUM_IN();

    TInt result = aSettingsData.Validate();

    if ( result )
        {
        ServiceExceptionL( result );
        }

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInMailboxServicesImpl::ServiceExceptionL()
// ---------------------------------------------------------------------------
//
void CImumInMailboxServicesImpl::ServiceExceptionL(
    const TInt aErrorCode ) const
    {
    IMUM_CONTEXT( CImumInMailboxServicesImpl::ServiceExceptionL, 0, KLogInApi );
    IMUM_IN();

    IMUM1( 0, "Exception occurred. Error %d", aErrorCode );

    __ASSERT_DEBUG( EFalse,
        User::Panic( KImumInMailboxServices, aErrorCode ) );

    IMUM_OUT();
    User::Leave( aErrorCode );
    }

// ---------------------------------------------------------------------------
// CImumInMailboxServicesImpl::DefaultDataObjectL()
// ---------------------------------------------------------------------------
//
const CImumMboxDefaultData* CImumInMailboxServicesImpl::DefaultDataObjectL()
    {
    // Make sure the object exist
    if ( !iDefaultData )
        {
        iDefaultData = CImumMboxDefaultData::NewL( iInternalApi );
        }

    return iDefaultData;
    }

// End of File
