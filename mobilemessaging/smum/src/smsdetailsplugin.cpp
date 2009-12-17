/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*       SMS Details Ecom Plugin implementation.
*
*/




// INCLUDE FILES

#include <e32std.h>
#include <implementationproxy.h>
#include <data_caging_path_literals.hrh> 

#include <gsmumsg.h>
#include <gsmuieoperations.h>
#include <gsmubuf.h>

#include <txtetext.h>   // CEditableText

#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <contactmatcher.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkStoreContact.h>
#include <CVPbkPhoneNumberMatchStrategy.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CPbk2StoreConfiguration.h>   // Contact store configuration

#include <StringLoader.h>               // StringLoader
#include <stringresourcereader.h>
#include <centralrepository.h>
#include <telconfigcrkeys.h>    // KCRUidTelephonyConfiguration
#include <commonphoneparser.h>      // Common phone number validity checker

#include <smsdetailsplugindata.rsg>
#include "smsdetailsplugin.h"

//For logging
#include "SmumLogging.h"

// CONSTANTS

const TInt KSmsDefaultGsmNumberMatchLength = 7;

const TInt KErrMultipleMatchFound = KErrGeneral;

_LIT( KSmsDetailsPluginResourceFile, "smsdetailsplugindata.rsc" );


struct TCntMatchRequestData
    {
    TPtrC iFromAddress;
    TDes* iOutput;
    TInt iMaxLength;
    TInt iMatchDigitCount;
    };


class CSmsDetailsPluginOneShotOperation : public CActive
    {
public:
    CSmsDetailsPluginOneShotOperation( TCntMatchRequestData& aRequestData );
    ~CSmsDetailsPluginOneShotOperation();

public: // API
    inline TInt CompletionCode() const { return iStatus.Int(); }

private: // From CActive
    void RunL();
    void DoCancel();

private: // Internal functions
    void MatchContactL();
    HBufC* GetContactNameL(const MVPbkContactLink& aContactLink);
    HBufC* GetContactNameInLowerCaseL(const MVPbkContactLink& aContactLink );
    TBool  ShowContactNameL(CVPbkContactLinkArray* aLinkArray, TInt &nameIndex);
    TInt GetCurrentStoreIndexL( CVPbkContactLinkArray& aLinkArray );
    
private: // Data members
    TCntMatchRequestData& iRequestData;
    CContactMatcher* iContactMatcher;
    };


// ==================== LOCAL FUNCTIONS ====================

// -----------------------------------------------------------------------------
// Define the implementation table for Ecom
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x102828A8, CSmsDetailsPlugin::NewL)
	};

// -----------------------------------------------------------------------------
// Returns the implementation table
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSmsDetailsPlugin::NewL
// -----------------------------------------------------------------------------
//
CSmsDetailsPlugin* CSmsDetailsPlugin::NewL()
	{
	SMUMLOGGER_ENTERFN("CSmsDetailsPlugin::NewL");
	CSmsDetailsPlugin* self = new (ELeave) CSmsDetailsPlugin();	
	SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::NewL");
	return self;
	}

// -----------------------------------------------------------------------------
// CSmsDetailsPlugin::CreateResourceReaderL
// -----------------------------------------------------------------------------
//
void CSmsDetailsPlugin::CreateResourceReaderL( RFs* aFs )
	{
	SMUMLOGGER_ENTERFN("CSmsDetailsPlugin::CreateResourceReaderL");
	if ( !iResourceReader )
	    {
        TParse fileParse;
        fileParse.Set( KSmsDetailsPluginResourceFile, &KDC_RESOURCE_FILES_DIR, NULL );
        TFileName resourceFile( fileParse.FullName() );
        if ( aFs )
            {
            iResourceReader = CStringResourceReader::NewL( resourceFile, *aFs );
            }
        else
            {
            iResourceReader = CStringResourceReader::NewL( resourceFile );
            }
	    }
	SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::CreateResourceReaderL");
	}

// -----------------------------------------------------------------------------
// CSmsDetailsPlugin::CSmsDetailsPlugin
// -----------------------------------------------------------------------------
//
CSmsDetailsPlugin::CSmsDetailsPlugin()
	{
	}

// -----------------------------------------------------------------------------
// CSmsDetailsPlugin::~CSmsDetailsPlugin
// -----------------------------------------------------------------------------
//
CSmsDetailsPlugin::~CSmsDetailsPlugin()
	{
	SMUMLOGGER_ENTERFN("CSmsDetailsPlugin::~CSmsDetailsPlugin");
	delete iResourceReader;
    SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::~CSmsDetailsPlugin");
	}

// -----------------------------------------------------------------------------
// CSmsDetailsPlugin::GetDetails
// -----------------------------------------------------------------------------
//
TInt CSmsDetailsPlugin::GetDetails(RFs& aFs, const CSmsMessage& aMessage, TDes& aDetails, TInt aMaxLength)
	{
    SMUMLOGGER_ENTERFN("CSmsDetailsPlugin::GetDetails(aFs, aMessage, aDetails, aMaxLength");

	__ASSERT_DEBUG( aMaxLength <= aDetails.MaxLength(), User::Invariant() );

	if (aMaxLength > aDetails.MaxLength())
		{
		aMaxLength = aDetails.MaxLength();
		}

	aDetails.Zero();

	TPtrC fromAddress;

	switch (aMessage.SmsPDU().Type())
		{
		case CSmsPDU::ESmsSubmit:
		case CSmsPDU::ESmsDeliver:
		case CSmsPDU::ESmsStatusReport:
			fromAddress.Set(aMessage.SmsPDU().ToFromAddress());
			break;
		default:
			return KErrNotSupported;
		}

	SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::GetDetails(aFs, aMessage, aDetails, aMaxLength");
	return GetDetails(aFs, fromAddress, aDetails, aMaxLength);
	}

// -----------------------------------------------------------------------------
// CSmsDetailsPlugin::GetDetails
// -----------------------------------------------------------------------------
//
TInt CSmsDetailsPlugin::GetDetails(RFs& aFs, const TDesC& aFromAddress, TDes& aDetails, TInt aMaxLength)
	{
    SMUMLOGGER_ENTERFN("CSmsDetailsPlugin::GetDetails(aFs, aFromAddress, aDetails, aMaxLength");
	__ASSERT_DEBUG( aMaxLength <= aDetails.MaxLength(), User::Invariant() );

	if (aMaxLength > aDetails.MaxLength())
		{
		aMaxLength = aDetails.MaxLength();
		}

    TInt ret = KErrNone;
	TRAPD(err, DoGetDetailsL(aFs, aFromAddress, aDetails, aMaxLength));

	if ( (err != KErrNone) || (aDetails.Length() == 0) )
		{
		if (aFromAddress.Length() <= aMaxLength)
			{
			aDetails = aFromAddress;
			aDetails.Trim();
			}
		else
			{
			// Truncate aFromAddress so that it fits into aDetails.
			aDetails = aFromAddress.Left(aMaxLength);
			aDetails.Trim();
			}
        // Propagate KErrCancel forwards
        if ( err == KErrCancel )
            {
            ret = err;
            }
		}
    SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::GetDetails(aFs, aFromAddress, aDetails, aMaxLength");

	return ret;
	}

// -----------------------------------------------------------------------------
// CSmsDetailsPlugin::GetDescription
// -----------------------------------------------------------------------------
//
TInt CSmsDetailsPlugin::GetDescription(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength)
	{
    SMUMLOGGER_ENTERFN("CSmsDetailsPlugin::GetDescription(aMessage, aDescription, aMaxLength");

	__ASSERT_DEBUG( aMaxLength <= aDescription.MaxLength(), User::Invariant() );

	if (aMaxLength > aDescription.MaxLength())
		{
		aMaxLength = aDescription.MaxLength();
		}

	aDescription.Zero();

	TBool gotDescription = EFalse;
	TRAPD(err, gotDescription = DoGetDescriptionL(aMessage, aDescription, aMaxLength));
	if(err != KErrNone || !gotDescription)
	    {
		ExtractDescriptionFromMessage(aMessage, aDescription, aMaxLength);
	    }
    SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::GetDescription(aMessage, aDescription, aMaxLength");
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSmsDetailsPlugin::DoGetDetailsL
//
// For clarity the whole function is flagged
// -----------------------------------------------------------------------------
//

void CSmsDetailsPlugin::DoGetDetailsL(RFs& aFs, const TDesC& aFromAddress, TDes& aDetails, TInt aMaxLength)
	{
	__UHEAP_MARK;
    SMUMLOGGER_ENTERFN("CSmsDetailsPlugin::DoGetDetailsL(aFs, aFromAddress, aDetails, aMaxLength");
	// Check that aFromAddress is a valid GSM telephone number
    if ( !CommonPhoneParser::IsValidPhoneNumber( aFromAddress, CommonPhoneParser::ESMSNumber ) )
        {
		User::Leave( KErrArgument );
        }

	aDetails.Zero();

    TInt matchDigitCount = KSmsDefaultGsmNumberMatchLength;

    // Read the amount of digits to be used in contact matching
    // The key is owned by PhoneApp
    CRepository* repository = CRepository::NewLC(KCRUidTelConfiguration);
    if (KErrNone == repository->Get(KTelMatchDigits, matchDigitCount))
        {
        // Min is 7
        matchDigitCount =
            Max(matchDigitCount, KSmsDefaultGsmNumberMatchLength);
        }
    CleanupStack::PopAndDestroy(); // repository

    // Match contacts to the from address
    // Prepare worker thread arguments
    TCntMatchRequestData requestData;
    requestData.iFromAddress.Set( aFromAddress );
    requestData.iOutput = &aDetails;
    requestData.iMaxLength = aMaxLength;
    requestData.iMatchDigitCount = matchDigitCount;

    // Prepare thread name
    _LIT( KSmsDetailsPluginCntMatcherWorkerThreadName, "S60SmsDetailsPluginWorkerThread_%08x" );
    TName threadName;
    threadName.AppendFormat( KSmsDetailsPluginCntMatcherWorkerThreadName, &requestData );

    // Create worker thread 
    RThread workerThread;
    CleanupClosePushL( workerThread );
    /*
     * LSAN-7HUB6B:: Contact matcher thread was using watcher process stack and hence running
     * out of memory as the inbox was growing. At some point, when inbox has arnd 1700 msgs, it fails to allocate memory
     * and hence there was no matching. 
     * changed the thread creation to use seperate stack of 1MB max size.
     */
    const TInt threadCreationError = workerThread.Create( threadName, CntMatchThreadFunction, 8 * 1024, 0x1000, 0x100000, &requestData  );
    User::LeaveIfError( threadCreationError );

    // Wait for thread to finish work
    TRequestStatus workerThreadStatus;
    workerThread.Rendezvous( workerThreadStatus );
    workerThread.Resume();
    User::WaitForRequest( workerThreadStatus );

    // Propagate error
    User::LeaveIfError( workerThreadStatus.Int() );

    // Tidy up
    CleanupStack::PopAndDestroy( &workerThread );
    SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::DoGetDetailsL(aFs, aFromAddress, aDetails, aMaxLength");
       
    __UHEAP_MARKEND;
    }


// -----------------------------------------------------------------------------
// CSmsDetailsPlugin::DoGetDescriptionL
// -----------------------------------------------------------------------------
//
TBool CSmsDetailsPlugin::DoGetDescriptionL(const CSmsMessage& aMessage,
    TDes& aDescription, TInt aMaxLength)
// this function returns EFalse if aMessage has no special message indication data and is not an SMS_STATUS_REPORT,
// i.e. more needs to be done to extract the description from the message
// otherwise returns ETrue
	{
    SMUMLOGGER_ENTERFN("CSmsDetailsPlugin::DoGetDescriptionL(aMessage, aDescription, aMaxLength");

    TInt resourceId = 0;
	TBuf<KSmsDescriptionLength> format;
	TSmsMessageIndicationType messageIndicationType;
	TExtendedSmsIndicationType extendedType;
	TSmsMessageProfileType messageProfileType;
	TBool toStore=EFalse;
	TUint totalIndicationCount=0;
	TUint totalMessageCount=0;	
	
	//check if the messae contains an enhanced voice mail indication
	CSmsEnhancedVoiceMailOperations& enhancedVoiceMailOperations = STATIC_CAST(CSmsEnhancedVoiceMailOperations&,aMessage.GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));
	
	if(enhancedVoiceMailOperations.ContainsEnhancedVoiceMailIEL())
		{
		//get a copy of the indication
		CEnhancedVoiceMailBoxInformation* retrievedNotification=enhancedVoiceMailOperations.CopyEnhancedVoiceMailIEL();
		TVoiceMailInfoType typeInfo=retrievedNotification->Type();
		//check its type
		if(typeInfo==EGsmSmsVoiceMailNotification)
			{
			//increment the indication count
			++totalIndicationCount;
			resourceId = R_MSG_INDICATION_ENHANCED_VOICEMAIL_ONE;	
			}
		
		TUint8 messageCount=retrievedNotification->NumberOfVoiceMessages();
		//add the message count to the running total
		totalMessageCount+=messageCount;
		//if there is more that one message of this type then set the resouce id to 'many'
		if(messageCount!=1)
			{
			++resourceId;	
			}
		
		delete retrievedNotification;
		}  
		
	//check for special message waiting indications
	CSmsSpecialSMSMessageOperations& operations = STATIC_CAST(CSmsSpecialSMSMessageOperations&,aMessage.GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));
	TUint specialMessageIndicationCount=operations.GetCountOfSpecialMessageIndicationsL();	
	
	if(specialMessageIndicationCount!=0)
		{
		//add special message indications to out indication count
		totalIndicationCount+=specialMessageIndicationCount;	
		
		if(totalIndicationCount>1) 
			{
			//set the resource id to R_MSG_INDICATION_OTHER_ONE
			resourceId = R_MSG_INDICATION_OTHER_ONE;
			//get the total number of messages from the indicatations
			TUint8 messageCount=0;
			for(TInt loopCount=0;loopCount<specialMessageIndicationCount;loopCount++)
				{
				operations.GetMessageIndicationIEL(loopCount,toStore,messageIndicationType,extendedType,messageProfileType,messageCount);
				totalMessageCount+=messageCount;						
				}
			}
		else
			{
			//there is only one indication, get it's type and the number of messages it holds.
			TUint8 messageCount=0;
			operations.GetMessageIndicationIEL(0,toStore,messageIndicationType,
											extendedType,messageProfileType,messageCount);	
		
			//add the message count to the running total
			totalMessageCount+=messageCount;
		
			switch (messageIndicationType)
				{
				case EGsmSmsVoiceMessageWaiting:
					resourceId = R_MSG_INDICATION_VOICEMAIL_ONE;
					break;
				
				case EGsmSmsFaxMessageWaiting:
					resourceId = R_MSG_INDICATION_FAX_ONE;
					break;
				
				case EGsmSmsElectronicMailMessageWaiting:
					resourceId = R_MSG_INDICATION_EMAIL_ONE;
					break;
					
				case EGsmSmsExtendedMessageTypeWaiting:
					//get the extended indications type
					if(extendedType==EGsmSmsVideoMessageWaiting)
						{
						resourceId = R_MSG_INDICATION_VIDEOMESSAGE_ONE;	
						}
					else
						{
						resourceId = R_MSG_INDICATION_OTHER_ONE;	
						}
					break;
					
				default:
					resourceId = R_MSG_INDICATION_OTHER_ONE;
					break;
				}	
			}
		//if there is more that one message waiting append 'many' to the id.
		if(totalMessageCount!=1)
			{
			resourceId++;	
			}
		}

	const CSmsPDU& smsPDU= aMessage.SmsPDU();
	// If no Special Msg Indication found in the User Data,
	// then check the DataCodingScheme.
	if (totalIndicationCount==0 && smsPDU.DataCodingSchemePresent())
		{
		TInt bits7to4 = smsPDU.Bits7To4();

		switch (bits7to4)
			{
			case TSmsDataCodingScheme::ESmsDCSMessageWaitingIndication7Bit:
			case TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationUCS2:
				{
				if (smsPDU.IndicationState() == TSmsDataCodingScheme::ESmsIndicationActive)
					{
					totalIndicationCount = 1;

					switch (smsPDU.IndicationType())
						{
						case TSmsDataCodingScheme::ESmsVoicemailMessageWaiting:
							resourceId = R_MSG_INDICATION_VOICEMAIL_ONE;
							break;
						case TSmsDataCodingScheme::ESmsFaxMessageWaiting:
							resourceId = R_MSG_INDICATION_FAX_ONE;
							break;
						case TSmsDataCodingScheme::ESmsElectronicMailMessageWaiting:
							resourceId = R_MSG_INDICATION_EMAIL_ONE;
							break;
						case TSmsDataCodingScheme::ESmsFaxOtherMessageWaiting:
						default:
							resourceId = R_MSG_INDICATION_OTHER_ONE;
							break;
						} //end switch
					} //end if
				} //end case
			default:
				{
				break; //do nothing
				}
			}
		}
	
	if (totalIndicationCount!=0)
		{
		//Special message found.
		CreateResourceReaderL( NULL );
        format = iResourceReader->ReadResourceString( resourceId );
        
        if (totalMessageCount == 1)
            {
            if (format.Length() <= aMaxLength)
                {
                aDescription = format;
                }
            else
                {
                // Truncate format so that it fits into aDescription.
                aDescription = format.Left(aMaxLength);
                }
            }
        else if (format.Length() < aMaxLength)
            {
            StringLoader::Format( aDescription, format, -1, totalMessageCount );
            }
        
        SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::DoGetDescriptionL(aMessage, aDescription, aMaxLength) - 1- true");
        return ETrue;
		}
	else
		{
		if(aMessage.Type() == CSmsPDU::ESmsStatusReport)
			{
			// for SMS_STATUS_REPORT messages, if we cannot read the string in, then
			// we do not attempt to extract the description from the message: return EFalse
    		CreateResourceReaderL( NULL );
			aDescription.Copy( iResourceReader->ReadResourceString( R_MSG_TYPE_STATUS_REPORT ) );
	        SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::DoGetDescriptionL(aMessage, aDescription, aMaxLength) - 2- true");
			return ETrue;
			}
		else
			{
	        SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::DoGetDescriptionL(aMessage, aDescription, aMaxLength) - 3- false");
			return EFalse;
			}
		}
	}

// -----------------------------------------------------------------------------
// CSmsDetailsPlugin::ExtractDescriptionFromMessage
// -----------------------------------------------------------------------------
//
void CSmsDetailsPlugin::ExtractDescriptionFromMessage(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength)
	{
    SMUMLOGGER_ENTERFN("CSmsDetailsPlugin::ExtractDescriptionFromMessage(aMessage, aDescription, aMaxLength)");
	if(aMessage.Type() != CSmsPDU::ESmsStatusReport)
		{
		aMessage.Buffer().Extract(aDescription, 0, Min(aMaxLength, aMessage.Buffer().Length()));

		TInt length = aDescription.Length();

		//replace paragraphs with spaces.
		while(length--)
			{
			TText& text = aDescription[length];
			const TChar ch(text);
			if (ch.IsSpace() || text == CEditableText::EParagraphDelimiter)
				text = ' ';
			}

		aDescription.TrimAll(); //removes leading trailing and multiple internal whitespace (spaces, line feeds etc)
		}
    SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::ExtractDescriptionFromMessage(aMessage, aDescription, aMaxLength)");
	}

TInt CSmsDetailsPlugin::CntMatchThreadFunction( TAny* aRequestData )
    {
    SMUMLOGGER_ENTERFN("CSmsDetailsPlugin::CntMatchThreadFunction");
    TInt error = KErrNone;
    //
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( !cleanupStack )
        {
        error = KErrNoMemory;
        }
    else
        {
        // Carry out operation in this thread, but via an active scheduler
        TRAP( error, CntMatchThreadFunctionL( aRequestData ) );
        delete cleanupStack;
        }

    // Rendezvous with MsvServer thread to report completion
    RThread::Rendezvous( error );
    SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::CntMatchThreadFunction");

    return error;
    }


void CSmsDetailsPlugin::CntMatchThreadFunctionL( TAny* aRequestData )
    {
    SMUMLOGGER_ENTERFN("CSmsDetailsPlugin::CntMatchThreadFunctionL");
    TCntMatchRequestData* requestData = reinterpret_cast< TCntMatchRequestData* >( aRequestData );

    CActiveScheduler* scheduler = new(ELeave) CActiveScheduler();
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    // Create one-shot object to do the match operation
    CSmsDetailsPluginOneShotOperation* operation = new(ELeave) CSmsDetailsPluginOneShotOperation( *requestData );
    CleanupStack::PushL( operation );
    
    // Now start the scheduler to carry out the op
    CActiveScheduler::Start();

    // Control returns here after the operation is complete. Propagate error codes to
    // thread function.
    User::LeaveIfError( operation->CompletionCode() );
    
    CleanupStack::PopAndDestroy( 2, scheduler ); // operation, scheduler
    SMUMLOGGER_LEAVEFN("CSmsDetailsPlugin::CntMatchThreadFunctionL");
    }





CSmsDetailsPluginOneShotOperation::CSmsDetailsPluginOneShotOperation( TCntMatchRequestData& aRequestData )
: CActive( EPriorityNormal ), iRequestData( aRequestData )
    {
    SMUMLOGGER_ENTERFN("CSmsDetailsPluginOneShotOperation::CSmsDetailsPluginOneShotOperation");
    CActiveScheduler::Add( this );
    //
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    SMUMLOGGER_LEAVEFN("CSmsDetailsPluginOneShotOperation::CSmsDetailsPluginOneShotOperation");
    }


CSmsDetailsPluginOneShotOperation::~CSmsDetailsPluginOneShotOperation()
    {
    SMUMLOGGER_ENTERFN("CSmsDetailsPluginOneShotOperation::~CSmsDetailsPluginOneShotOperation");
    if( iContactMatcher )
        delete iContactMatcher;
    Cancel();
    SMUMLOGGER_LEAVEFN("CSmsDetailsPluginOneShotOperation::~CSmsDetailsPluginOneShotOperation");
    }


void CSmsDetailsPluginOneShotOperation::RunL()
    {
    SMUMLOGGER_ENTERFN("CSmsDetailsPluginOneShotOperation::RunL");
    TRAPD( err, MatchContactL() );

    // Preserver error code so that the thread function can report it to the main MsvServer thread.
    iStatus = err;

    // Stop scheduler which will return control to the thread function
    CActiveScheduler::Stop();
    SMUMLOGGER_LEAVEFN("CSmsDetailsPluginOneShotOperation::RunL");
    }


void CSmsDetailsPluginOneShotOperation::DoCancel()
    {
    // Nothing to do here - we already completed our request in the ctor
    }


void CSmsDetailsPluginOneShotOperation::MatchContactL()
    {
    SMUMLOGGER_ENTERFN("CSmsDetailsPluginOneShotOperation::MatchContactL");
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL( fsSession );

    if( !iContactMatcher )
        {
        iContactMatcher = CContactMatcher::NewL( &fsSession );
        iContactMatcher->OpenAllStoresL();
        }
 
    CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC();
    
    iContactMatcher->MatchPhoneNumberL(
        iRequestData.iFromAddress,
        iRequestData.iMatchDigitCount,
        CVPbkPhoneNumberMatchStrategy::EVPbkMatchFlagsNone,
        *linkArray );
    
    TInt nameIndex = 0; //correct index if only one match is found
    if( linkArray->Count() == 0 )
        {
        SMUMLOGGER_WRITE("No match found");
        User::Leave( KErrNotFound );        
        }
    else if( linkArray->Count() > 1 )
        {
        //Multiple matches found. Get the current store single match index if any.
        nameIndex = GetCurrentStoreIndexL( *linkArray );
        if( nameIndex == KErrMultipleMatchFound )
            {
            /* No unique match in current store, Hence show the name only if all the matches have 
             * identical names
             */
            if( ShowContactNameL( linkArray, nameIndex) == EFalse)
                {
                SMUMLOGGER_WRITE("No (Perfect) match found");
                User::Leave( KErrMultipleMatchFound );
                }
            }
        }

    // There should be only one match or multiple identical matches in this case.
    // Use the same and read contact from the store.

    HBufC* alias = GetContactNameL( linkArray->At(nameIndex) );
    if ( alias )
        {
        iRequestData.iOutput->Copy( alias->Left( iRequestData.iMaxLength ) );
        delete alias;
        alias = NULL;        
        }

    CleanupStack::PopAndDestroy( 2, &fsSession ); // linkArray, fsSession
    SMUMLOGGER_LEAVEFN("CSmsDetailsPluginOneShotOperation::MatchContactL");
    }


// -----------------------------------------------------------------------------
// CSmsDetailsPluginOneShotOperation::GetContactNameL
// -----------------------------------------------------------------------------
//
HBufC* CSmsDetailsPluginOneShotOperation::GetContactNameL(const MVPbkContactLink& aContactLink )
    {
    //SMUMLOGGER_ENTERFN("CSmsDetailsPluginOneShotOperation::GetContactNameL");
    MVPbkStoreContact* tempContact;
    iContactMatcher->GetStoreContactL(aContactLink, &tempContact);
    tempContact->PushL();
    
    MVPbkStoreContactFieldCollection& coll = tempContact->Fields();
    HBufC* nameBuff = iContactMatcher->GetNameL( coll );
    
    CleanupStack::PopAndDestroy(tempContact); // tempContact
    //SMUMLOGGER_LEAVEFN("CSmsDetailsPluginOneShotOperation::GetContactNameL");
    return nameBuff;
    }

// -----------------------------------------------------------------------------
// CSmsDetailsPluginOneShotOperation::GetContactNameInLowerCaseL
// -----------------------------------------------------------------------------
//
HBufC* CSmsDetailsPluginOneShotOperation::GetContactNameInLowerCaseL(const MVPbkContactLink& aContactLink )
    {
    //SMUMLOGGER_ENTERFN("CSmsDetailsPluginOneShotOperation::GetContactNameInLowerCaseL");
    //get the name 
    HBufC* nameBuff =  GetContactNameL(aContactLink);
    CleanupStack::PushL( nameBuff );
    
    //SMUMLOGGER_WRITE_FORMAT( "length: %d", nameBuff->Length() );
    //SMUMLOGGER_WRITE_FORMAT( "size: %d", nameBuff->Size() );
    
    //Convert to lower case , since this name buffer is used to compare names.    
    HBufC* nameInLowerCase = HBufC::NewL( nameBuff->Length() + 2 );
    nameInLowerCase->Des().CopyLC( *nameBuff );
    
    CleanupStack::PopAndDestroy( nameBuff ); // nameBuff
    //SMUMLOGGER_LEAVEFN("CSmsDetailsPluginOneShotOperation::GetContactNameInLowerCaseL");
    return nameInLowerCase;
    }

// -----------------------------------------------------------------------------
// CSmsDetailsPluginOneShotOperation::ShowContactNameL
// -----------------------------------------------------------------------------
//
TBool CSmsDetailsPluginOneShotOperation::ShowContactNameL(CVPbkContactLinkArray* aLinkArray, TInt &nameIndex)
    {
    SMUMLOGGER_ENTERFN("CSmsDetailsPluginOneShotOperation::ShowContactName");
    /* TODO:: see the NOTE: below
     * compare the names upto standard
     * 1. if all the names are same - display the name 
     *    eg: "abcdef xyz" && "abcdef xyz"
     * 2. find min name legth among all,
     *    if this length is > standard length and matches upto standard length - display the larger name.
     *    eg: abcdef xyz123,  abcdef xyz12, abcdef xyz and std length is 10,
     *        since match upto 10 chars is fine, display abcdef xyz123
     * 3. in any other case do not show name
     *    eg: abcdef xyz , abcde xyz
     *        abcdef xyz , abcdef xy
     *        abcdef xyz , abcde
     */
    TInt i, minLength = 999, maxLength = 0, length = 0, maxLengthIndex = 0, stdLength = 14;
    TBool retVal = ETrue ;
    SMUMLOGGER_WRITE( "Contact Match statistics to follow..." );
    SMUMLOGGER_WRITE_FORMAT( "CSmsDetailsPluginOneShotOperation::MatchContactL Match count: %d", aLinkArray->Count() );
    
    for( i = 0 ; i < aLinkArray->Count(); i++ )
        {
        HBufC* alias = GetContactNameL( aLinkArray->At(i) );
        SMUMLOGGER_WRITE_FORMAT( ":-> %s", alias->Des().PtrZ() );
        length = alias->Des().Length();
        if(minLength > length)
            {
            minLength = length;
            }
        if(maxLength < length)
            {
            maxLength = length;
            maxLengthIndex = i;
            }
        delete alias;
        alias = NULL;
        }
    
    SMUMLOGGER_WRITE_FORMAT( "Cotact Lengths: Std Length  : %d", stdLength);
    SMUMLOGGER_WRITE_FORMAT( "                MinLength   : %d", minLength);
    SMUMLOGGER_WRITE_FORMAT( "                MaxLength   : %d", maxLength); 
    SMUMLOGGER_WRITE_FORMAT( "                MaxLen index: %d", maxLengthIndex);
    
    if(minLength != maxLength)
        {
        //complete length match not possible
        retVal = EFalse;
        
        /* NOTE:
         * Uncomment below code if partial length(upto stdLength) match is sufficient, 
         * ensure stdLength is correct
         */
        /*
        if(minLength < stdLength)
            {
            SMUMLOGGER_WRITE( "minLength < stdLength" );
            retVal = EFalse;
            }
        */
        }
    
    if( retVal )
        {
        TInt ret;
        HBufC* longestName = GetContactNameInLowerCaseL( aLinkArray->At(maxLengthIndex) );        
        SMUMLOGGER_WRITE_FORMAT( "Longest name:-> %s", longestName->Des().PtrZ() );
        for ( i = 0; i < aLinkArray->Count() && retVal; i++ )
            {
            HBufC* nameI = GetContactNameInLowerCaseL( aLinkArray->At(i) );
            SMUMLOGGER_WRITE_FORMAT( "compared with -> %s", nameI->Des().PtrZ() );
            ret = longestName->Find(nameI->Des());
            if(ret == KErrNotFound || ret != 0)
                {
                SMUMLOGGER_WRITE_FORMAT( "Part/Full Match error/offset: %d", ret );
                retVal = EFalse;
                }
            delete nameI;
            nameI = NULL;
           }
        delete longestName;
        longestName = NULL;
        }

    //If no match OR more than one match, then to avoid ambiguity, display the number
    SMUMLOGGER_WRITE_FORMAT( "Final Match result : %d", retVal );
    SMUMLOGGER_WRITE_FORMAT( "Final Match index  : %d", maxLengthIndex );
    SMUMLOGGER_WRITE( "Contact Match statistics End here..." );

    SMUMLOGGER_LEAVEFN("CSmsDetailsPluginOneShotOperation::ShowContactName");
    nameIndex = maxLengthIndex;
    return retVal;
    }

// -----------------------------------------------------------------------------
// CSmsDetailsPluginOneShotOperation::GetCurrentStoreIndexL
// -----------------------------------------------------------------------------
//
TInt CSmsDetailsPluginOneShotOperation::GetCurrentStoreIndexL( CVPbkContactLinkArray& aLinkArray )
    {
    TInt curStoreIndex( KErrMultipleMatchFound );
    TInt curStoreMatchCount = 0;
    RArray<TInt> otherStoreMatchIndices;
    CleanupClosePushL( otherStoreMatchIndices );
    
    //Get the current configured contact store array(s)
    CPbk2StoreConfiguration* storeConfiguration = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL( storeConfiguration );
    CVPbkContactStoreUriArray* currStoreArray = storeConfiguration->CurrentConfigurationL();
    CleanupStack::PopAndDestroy(storeConfiguration);

    if ( currStoreArray )
        {
        /* Contact's store is compared against user selected stores.
         * If contact is from such store, found index is incremented
         * else, other store contact indices are populated into array for further use
         */
        for ( TInt i = 0; i < aLinkArray.Count(); i++ )
            {
            TVPbkContactStoreUriPtr uri = aLinkArray.At(i).ContactStore().StoreProperties().Uri();
            if ( currStoreArray->IsIncluded( uri ) )
                {
                // Set index to found contact and increment the count.
                curStoreIndex = i;
                curStoreMatchCount++;
                }
            else
                {
                otherStoreMatchIndices.AppendL(i);
                }
            }
        
        delete currStoreArray;    
        if ( curStoreMatchCount > 1)
            {
            /* Multiple matches found from current user selected store(s) 
             * Delete match from other stores in aLinkArray. New aLinkArray should only contain 
             * current store contact matches, so that next level pruning can be done(e.g, names can be 
             * compared and displayed if they are identical).
             */
            for(TInt i = otherStoreMatchIndices.Count() - 1; i >= 0; i--)
                {
                aLinkArray.Delete( otherStoreMatchIndices[i] );
                }
            curStoreIndex = KErrMultipleMatchFound;
            }
        }
    CleanupStack::PopAndDestroy( &otherStoreMatchIndices );
    return curStoreIndex;
    }

//  End of File  
