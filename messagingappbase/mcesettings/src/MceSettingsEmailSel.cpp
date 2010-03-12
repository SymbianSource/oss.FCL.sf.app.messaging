/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Dialog showing all email MTMs
*
*/



// INCLUDE FILES
#include <coemain.h> 
#include <aknnotewrappers.h> 
#include <eikenv.h>
#include "MceSettingsEmailSel.h"
#include <MceSettingsEmailSel.rsg>
#include <MceSettings.rsg>
#include <ConeResLoader.h>
#include <data_caging_path_literals.hrh>
#include <msvstd.h> 
#include <msvapi.h>
#include <msvids.h>
#include <StringLoader.h>
#include <centralrepository.h>
#include <MessagingDomainCRKeys.h>
#include <mtmuids.h>
#include <mtudcbas.h>
#include <mtmuibas.h>			
#include <SendUiConsts.h>
#include <bautils.h>
#include <MtmExtendedCapabilities.hrh>
#include <MuiuMsvUiServiceUtilities.h>
#include <mtudreg.h>
#include <miutset.h>
#include <ImumInternalApi.h>
#include <ImumInHealthServices.h>
#include <muiuflags.h>
#include <muiuemailtools.h>
#include <featmgr.h>

const TInt KArrayGranularity = 3;
const TInt KCapabilitiesUnset = -1;

_LIT( KMceEmailSelResourceFileName,
	  "\\resource\\MceSettingsEmailSel.rsc" );
#define KUidMsgTypeFsMtmVal               0x2001F406

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CSampleApplication::CreateDocumentL
// Create a Sample document, and return a pointer to it.
// ---------------------------------------------------------
//
CMceSettingsEmailSel* CMceSettingsEmailSel::NewL()
    {
    // Create an object, and return a pointer to it
    CMceSettingsEmailSel* self = new (ELeave) CMceSettingsEmailSel;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CMceSettingsEmailSel::ConstructL
// 2nd phase contructor that can leave.
// ---------------------------------------------------------
//
void CMceSettingsEmailSel::ConstructL()
    {
    iMsvSessionPtr = CMsvSession::OpenSyncL( *this );   
    iMtmStore = CMtmStore::NewL( *iMsvSessionPtr );   
    LoadResourceL();      
    iRequiredCapabilities.iFlags = KCapabilitiesUnset;    
    
    if ( FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework ) )
    {
      iEmailFramework = ETrue;
    } 
    }
    
// ---------------------------------------------------------
// CMceSettingsEmailSel::CMceSettingsEmailSel
// contructor
// ---------------------------------------------------------
//
CMceSettingsEmailSel::CMceSettingsEmailSel()
		:iEmailFramework(EFalse)
    {
    }        

// ---------------------------------------------------------
// CMceSettingsEmailSel::~CMceSettingsEmailSel
// Destructor
// ---------------------------------------------------------
//
CMceSettingsEmailSel::~CMceSettingsEmailSel()
    {
    if( iMtmStore )
    	{
    	delete iMtmStore;
    	}     
    if( iMsvSessionPtr )
    	{
    	delete iMsvSessionPtr;
    	}  	  
    UnLoadResource();    	 	
    }    

// ---------------------------------------------------------
// CMceSettingsEmailSel::LoadResourceL
// Load the resource for the dll module
// ---------------------------------------------------------
//
void CMceSettingsEmailSel::LoadResourceL()
    {
    TFileName resourceFileNameBuf( KMceEmailSelResourceFileName );

    CEikonEnv* eikEnv = CEikonEnv::Static();
    BaflUtils::NearestLanguageFile( eikEnv->FsSession(),
    		                        resourceFileNameBuf );
    iResourceOffset = eikEnv->AddResourceFileL( resourceFileNameBuf );
    }    
    
// ---------------------------------------------------------
// CMceSettingsEmailSel::UnLoadResource
// Unload the resource for the dll module
// ---------------------------------------------------------
//
void CMceSettingsEmailSel::UnLoadResource()
    {
    CEikonEnv* eikEnv = CEikonEnv::Static();
    eikEnv->DeleteResourceFile( iResourceOffset );
    }

// ---------------------------------------------------------
// CMceSettingsEmailSel::HandleSessionEventL
// From MMsvSessionObserver
// ---------------------------------------------------------
//    
void CMceSettingsEmailSel::HandleSessionEventL(
    TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/,
    TAny* /*aArg2*/, TAny* /*aArg3*/ )
	{	
	}

// ---------------------------------------------------------
// CMceSettingsEmailSel::ShowEmailSelectDlgL
// 
// ---------------------------------------------------------
//    
EXPORT_C TBool CMceSettingsEmailSel::ShowSelectEmailDlgL(
    TMsvId& aSelectedService, TUid& aMtmType,
    TSendingCapabilities aRequiredCapabilities )
	{		
	CMceSettingsEmailSel* dlg = CMceSettingsEmailSel::NewL();

	CleanupStack::PushL( dlg );
	TBool retVal = dlg->ShowSelectDlgL(
			             aSelectedService, aMtmType, aRequiredCapabilities );
	CleanupStack::PopAndDestroy( dlg );
	return retVal;	
	}
	
// ---------------------------------------------------------
// CMceSettingsEmailSel::ShowSelectDlgL
// 
// ---------------------------------------------------------
//    	
TBool CMceSettingsEmailSel::ShowSelectDlgL(
                                           TMsvId& aSelectedService, TUid& aMtmType, 
                                           TSendingCapabilities aRequiredCapabilities )
    {
    iRequiredCapabilities = aRequiredCapabilities;
    // List items for list query.
    CDesCArrayFlat* listItems = new (ELeave) CDesCArrayFlat(
                                KArrayGranularity );
    CleanupStack::PushL( listItems );

    // This array is used to map user selection to correct service.
    CArrayFix<TUid>* listItemUids = 
        new ( ELeave ) CArrayFixFlat<TUid>( KArrayGranularity );
    CleanupStack::PushL( listItemUids );

    // Get and filter available MTM services supporting attachments.
    FilterEmailServicesL( *listItems, *listItemUids );
    
    //Check if Cmail is enabled
    if(iEmailFramework)
         {
         return ShowCmailSelectDlgL(aSelectedService, aMtmType,listItems, listItemUids );
         }

    // no accounts defined, nothing to show
    if ( 0 == listItems->Count() )
        {
        CleanupStack::PopAndDestroy( listItemUids );
        CleanupStack::PopAndDestroy( listItems );
        aSelectedService = KMsvUnknownServiceIndexEntryId;
        return ETrue;
        }

    // only one account, selection dialog not shown
    if( listItems->Count() == 1 )
        {
        aSelectedService = (*listItemUids)[0].iUid;

        TMsvEntry entry;
        TMsvId serviceId;
        User::LeaveIfError( iMsvSessionPtr->GetEntry(
                (*listItemUids)[0].iUid, serviceId, entry ) );
        if ( entry.iMtm == KSenduiMtmPop3Uid ||
             entry.iMtm == KSenduiMtmImap4Uid )
            {
            aMtmType = KSenduiMtmSmtpUid;
            aSelectedService = entry.iRelatedId; // related = SMTP
            }
        else
            {
            aMtmType = entry.iMtm;
            }		

        CleanupStack::PopAndDestroy( listItemUids );
        CleanupStack::PopAndDestroy( listItems );
        return ETrue;		
        }

    // Show list query.
    TInt choice = KErrNotFound;
    CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog( &choice );
    dlg->PrepareLC( R_SEND_EMAIL_LIST_QUERY );
    dlg->SetItemTextArray( listItems );
    dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );

    if ( !dlg->RunLD() )
        {
        // Cancel        
        CleanupStack::PopAndDestroy( listItemUids );
        CleanupStack::PopAndDestroy( listItems );
        return EFalse;
        }

    aSelectedService = (*listItemUids)[choice].iUid;

    TMsvEntry entry;
    TMsvId serviceId;
    User::LeaveIfError( iMsvSessionPtr->GetEntry(
            aSelectedService, serviceId, entry ) );
    if ( entry.iMtm == KSenduiMtmPop3Uid ||
         entry.iMtm == KSenduiMtmImap4Uid )
        {
        aMtmType = KSenduiMtmSmtpUid;
        aSelectedService = entry.iRelatedId; // related = SMTP
        }
    else
        {
        aMtmType = entry.iMtm;
        }

    CleanupStack::PopAndDestroy( listItemUids );
    CleanupStack::PopAndDestroy( listItems );
    return ETrue;	 
    }

// ---------------------------------------------------------
// CMceSettingsEmailSel::ShowCmailSelectDlgL
// 
// ---------------------------------------------------------
//  
TBool CMceSettingsEmailSel::ShowCmailSelectDlgL(TMsvId& aSelectedService, 
                            TUid& aMtmType,
                            CDesCArrayFlat*         aListItems,
                            CArrayFix<TUid>*        aListItemUids)
    {
    TBool retval = EFalse;
    //Add the non cmail boxes at the end of cmail list 
    for(int cnt = 0; cnt < aListItems->Count(); cnt++  )
        {
        iCmaillistItems->AppendL(aListItems->MdcaPoint(cnt));
        iCmaillistItemUids->AppendL(aListItemUids->At(cnt));
        }
    
    // no accounts defined, nothing to show
    if ( 0 == iCmaillistItems->Count() )
        {
        aSelectedService = KMsvUnknownServiceIndexEntryId;
        retval =  ETrue;
        }

    // only one account, selection dialog not shown
    if( iCmaillistItems->Count() == 1 && !retval )
        {
        aSelectedService = (*iCmaillistItemUids)[0].iUid;

        TMsvEntry entry;
        TMsvId serviceId;
        User::LeaveIfError( iMsvSessionPtr->GetEntry(
                (*iCmaillistItemUids)[0].iUid, serviceId, entry ) );
        if ( entry.iMtm == KSenduiMtmPop3Uid ||
             entry.iMtm == KSenduiMtmImap4Uid )
            {
            aMtmType = KSenduiMtmSmtpUid;
            aSelectedService = entry.iRelatedId; // related = SMTP
            }
        else
            {
            aMtmType = entry.iMtm;
            }       
        retval = ETrue;       
        }
    TBool cancel = EFalse;
    if( !retval )
        {
        // Show list query.
        TInt choice = KErrNotFound;
        CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog( &choice );
        dlg->PrepareLC( R_SEND_EMAIL_LIST_QUERY );
        dlg->SetItemTextArray( iCmaillistItems );
        dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );
    
        if ( !dlg->RunLD() )
            {
            // Cancel      
            cancel = ETrue;
            //retval = EFalse;
            }
        if(!cancel)
            {
            aSelectedService = (*iCmaillistItemUids)[choice].iUid;
        
            TMsvEntry entry;
            TMsvId serviceId;
            User::LeaveIfError( iMsvSessionPtr->GetEntry(
                    aSelectedService, serviceId, entry ) );
            aMtmType = entry.iMtm;
            }
        }
    delete iCmaillistItemUids;
    iCmaillistItemUids  = NULL;
    delete iCmaillistItems;
    iCmaillistItems = NULL;;
    CleanupStack::PopAndDestroy( aListItemUids );
    CleanupStack::PopAndDestroy( aListItems );
    if(cancel)
        return EFalse;
    else
        return ETrue;   
    }
// ---------------------------------------------------------
// TPtrC8Ident
// Hash Table Function 
// ---------------------------------------------------------
//
TBool TPtrC8Ident(const TPtrC16& aL, const TPtrC16& aR)
     {
     return DefaultIdentity::Des16(aL, aR);
     }

// ---------------------------------------------------------
// TPtrC8Hash
// Hash Table Function 
// ---------------------------------------------------------
//
TUint32 TPtrC8Hash(const TPtrC16& aPtr)
     {
     return DefaultHash::Des16(aPtr);

     }
// ---------------------------------------------------------
// CMceSettingsEmailSel::FilterEmailServicesL
// 
// ---------------------------------------------------------
//    
void CMceSettingsEmailSel::FilterEmailServicesL(
    CDesCArrayFlat&         aListItems,
    CArrayFix<TUid>&        aListItemUids )
    {
    iDefaultIndex = -1;    
     
    TInt keyValue;
    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL( KCRUidSelectableDefaultEmailSettings   ) );

    if ( ret == KErrNone )
        {
        if ( repository->Get( KSelectableDefaultMailAccount  , keyValue ) != KErrNone )
            {
            iDefaultIndex = -1;
            }
        // Check if there is other than platform email application registered
        // to handle S60 mailboxes	
        TInt err = repository->Get( KIntegratedEmailAppMtmPluginId  ,
                                    iMtmPluginID );
        if ( err != KErrNone )
            {
            iMtmPluginID = 0;
            }
        }
    delete repository;
        
    // fetch email mtms
    CMsvEntry* entry = iMsvSessionPtr->GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( entry );

    TInt cnt = entry->Count();

    // Add default item first
    if(!iEmailFramework)
      {
      entry->SetSortTypeL( TMsvSelectionOrdering(
                              KMsvGroupByType | KMsvGroupByStandardFolders,
                              EMsvSortByDetailsReverse, ETrue ) );
      
      for ( TInt cc = entry->Count(); --cc >= 0; )
          {
          iTentryDef = (*entry)[cc];
            if(!IsMailBoxHealthyL(iTentryDef))
               {
               continue;
               }

            if ( iTentryDef.iType.iUid == KUidMsvServiceEntryValue &&
                    iTentryDef.Id() != KMsvLocalServiceIndexEntryIdValue )
                {
                const TBool noRelatedId =
                iTentryDef.iRelatedId == KMsvNullIndexEntryId ||
                iTentryDef.iRelatedId == iTentryDef.Id();

                if ( IsEmailEntryL(iTentryDef)  &&
                     ( ( iTentryDef.Id() == keyValue ) ||
                       ( iTentryDef.iRelatedId == keyValue) ) )
                    {
                    TBool append = ETrue;
                    // query capability (attachment)
                    TUid entryUid;
                    entryUid = iTentryDef.iMtm;

                    if ( iTentryDef.iMtm == KSenduiMtmPop3Uid ||
                            iTentryDef.iMtm == KSenduiMtmImap4Uid )
                        {
                        entryUid = KSenduiMtmSmtpUid;
                        }

                    TBool ret = EFalse;
                    TRAPD( err, ret = QueryCapabilitiesAndValidateAccountL(
                    		                                     entryUid ) )
                    if ( ret && ( err == KErrNone ) )
                        {
                        append = ETrue;
                        }
                    else
                        {
                        append = EFalse;
                        }

                    const TInt count = aListItemUids.Count();

                    // Check that related service is not already added to
                    // array
                    for ( TInt loop = 0; loop < count; loop ++ )
                        {
                        if ( iTentryDef.iRelatedId ==
                                                aListItemUids.At(loop).iUid )
                            {
                            append = EFalse;
                            break;
                            }
                        }

                    if( append )
                        {
                        // Add this service if:
                        // it is visible, or,there is no associated related
                        // service, or, we have been asked to list all
                        // services.
                        HBufC* stringholder;
                        stringholder = StringLoader::LoadLC(
                            R_MCE_POPUP_DEFAULT_EMAIL,
                            iTentryDef.iDetails ); //Pushes stringholder onto the
                                               //Cleanup Stack.

                        entryUid.iUid = iTentryDef.Id();
                        aListItems.AppendL( stringholder->Des() );
                        aListItemUids.AppendL( entryUid );
                        iDefaultIndex = cc;
                        CleanupStack::PopAndDestroy( stringholder );
                        iTentryDef = (*entry)[iDefaultIndex];
                        break;
                        }
                    }
                }
            }
        }
        
    //Add Other Email Boxes    
    AddOtherEmailBoxesL(aListItems, aListItemUids, entry);    
    
    if(iEmailFramework)
        {
        //Hash Table to store the information of non cmail mailboxes for sorting and arranging their uids
        RHashMap<TPtrC16, TInt32 >* nonCmailUid = new( ELeave )RHashMap< TPtrC16, TInt32 >
                              (&TPtrC8Hash , &TPtrC8Ident);
        CleanupStack::PushL( nonCmailUid);
        
        for(TInt loop = 0; loop < aListItems.Count() ; loop++ )
            {
            nonCmailUid->Insert(aListItems.MdcaPoint(loop), aListItemUids.At(loop).iUid );
            }
        
        //Sort the non cmail items alphabatically
        aListItems.Sort(ECmpFolded);
        
        //Reset the array to load the Uids in new sorted order
        aListItemUids.Reset();
        
        for(TInt loop = 0; loop < aListItems.Count() ; loop++ )
            {
            TInt32* uid = nonCmailUid->Find(aListItems.MdcaPoint(loop));
            aListItemUids.AppendL(TUid::Uid( *uid ));
            }
       
        CleanupStack::PopAndDestroy( nonCmailUid );
        }
        CleanupStack::PopAndDestroy( entry );
    
    }	
// ---------------------------------------------------------
// TPtrC8Hash
// Hash Table Function
// ---------------------------------------------------------
//
TUint32 TPtrC8Hash(const TInt64 & atime)
     {
     return DefaultHash::Integer(atime);
     }

// ---------------------------------------------------------
// TInt64Ident
// Hash Table Function 
// ---------------------------------------------------------
//
TBool TInt64Ident(const TInt64& aL, const TInt64& aR)
     {
     return DefaultIdentity::Integer(aL, aR);

     }

// ---------------------------------------------------------
// CMceSettingsEmailSel::IsEmailEntryL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CMceSettingsEmailSel::IsEmailEntryL(TMsvEntry tentry)
    {
    TBool appendEmail = EFalse;
    if ( tentry.Visible() )
        {
        appendEmail = ETrue;
        }
    else
        {
        CMtmUiDataRegistry* uiRegistry =
                         CMtmUiDataRegistry::NewL( *iMsvSessionPtr );
        CleanupStack::PushL(uiRegistry);
        if ( uiRegistry->IsPresent( tentry.iMtm ) &&
             uiRegistry->IsPresent( KUidMsgTypePOP3 ) )
            {
            TUid mailMTMTechType =
                    uiRegistry->TechnologyTypeUid( KUidMsgTypePOP3 );
            if( uiRegistry->TechnologyTypeUid( tentry.iMtm ) ==
                                                    mailMTMTechType )
                {
                appendEmail = ETrue;
                }
            }
        CleanupStack::PopAndDestroy( uiRegistry );
        }
    return appendEmail;
    }

// ---------------------------------------------------------
// CMceSettingsEmailSel::AppendCmailBoxesL(
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMceSettingsEmailSel::AppendCmailBoxesL(CArrayFix<TInt64 >* atimeItems, CMsvEntry* aEntry)
    {
    //Sort the cmail Entry items based on time
    TKeyArrayFix key( 0, ECmpTInt64 );
    atimeItems->Sort(key);
    TUid cmailEntryUid;
    
    //Array to get the list of Cmail Boxes
    iCmaillistItems = new (ELeave) CDesCArrayFlat(
                                KArrayGranularity );
    
    iCmaillistItemUids = new ( ELeave ) CArrayFixFlat<TUid>( KArrayGranularity );
    
    // Go one by one in Hash table
    if(atimeItems->Count() > 0)
        {    
        for(TInt cnt = (atimeItems->Count() - 1) ; cnt >= 0 ; cnt--)
            {
            //Take one by one the Time Entry and Find the correponding Position Key
            //from key get the correponding Entry
           TInt* entrypos = iCmailBoxes->Find(atimeItems->At(cnt));
           TMsvEntry cmailentry = (*aEntry)[*entrypos];
           
           iCmaillistItems->AppendL(cmailentry.iDetails);          
           cmailEntryUid.iUid = cmailentry.Id();
           iCmaillistItemUids->AppendL(cmailEntryUid); 
           }
         }
    }

// ---------------------------------------------------------
// CMceSettingsEmailSel::AddOtherEmailBoxesL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMceSettingsEmailSel::AddOtherEmailBoxesL(CDesCArrayFlat& aListItems, CArrayFix<TUid>& aListItemUids , CMsvEntry* aEntry)
    {
    CArrayFix<TInt64 >* timeItems = NULL;
    CDesCArrayFlat* cmaillist = NULL;
    TTime timeNow;
    
    if(iEmailFramework)
        {
        //Added for Cmail Details
        iCmailBoxes =
              new( ELeave )RHashMap< TInt64, TInt >
                      (&TPtrC8Hash , &TInt64Ident);    
        
        // This array is used to map user selection to correct service.
        timeItems = 
             new ( ELeave ) CArrayFixFlat<TInt64 >( 3 );
        CleanupStack::PushL( timeItems );
             
        cmaillist = new (ELeave) CDesCArrayFlat(
                                    KArrayGranularity );
        CleanupStack::PushL( cmaillist );
        
        aEntry->SetSortTypeL( TMsvSelectionOrdering(
                                    KMsvGroupByMtm, 
                                    EMsvSortByDetailsReverse, ETrue ) );
               
        timeNow.UniversalTime();
        
        }
           
      // take entry one by one
      for ( TInt cc = aEntry->Count(); --cc >= 0; )
          {
          TMsvEntry tentry2 = (*aEntry)[cc];
          
           if(!IsMailBoxHealthyL(tentry2))
             {
             continue;
             }

          if ( tentry2.iType.iUid == KUidMsvServiceEntryValue &&
               tentry2.Id() != KMsvLocalServiceIndexEntryIdValue )
              {
              if ( IsEmailEntryL(tentry2) && ( cc != iDefaultIndex ) )
                  {
                  TBool append = ETrue;
                  TUid entryUid;

                  append = IsMailBoxAppendable( tentry2, &entryUid );
                  // If related service is SyncML or if 3'rd party email
                  // plugin is registered but the mailbox uid is wrong,
                  // it should be skipped. (This avoids doubles)
                  if(!iEmailFramework)
                  {
                   if ( entryUid != KSenduiMtmSmtpUid ||
                           iMtmPluginID != 0 && tentry2.iMtm.iUid != iMtmPluginID )
                       {
                       append = EFalse;
                       }
                  }
                  if ( append )
                  	{
                     append = IfMailBoxUidAlreadyAdded( &aListItemUids, tentry2 );
                  
                     if(! IfMailBoxAlreadyAdded(aListItems, cmaillist, tentry2 ))
                       continue;
                    }
                  if ( tentry2.iDetails.Compare(iTentryDef.iDetails) == 0 )
                      {
                      append = EFalse;
                      }

                  if( append )
                      {
                      // Add this service if:
                      // it is visible, or,there is no associated related
                      // service, or, we have been asked to list all
                      // services.                    
                      entryUid.iUid = tentry2.Id();    
                      //If cmail box add to the hash table and cmaillist
                      if(tentry2.iMtm.iUid == KUidMsgTypeFsMtmVal )
                          {
                          TTimeIntervalMicroSeconds difference = timeNow.MicroSecondsFrom( tentry2.iDate.Int64() );
                          TInt ret = iCmailBoxes->Insert(difference.Int64() , cc );
                          timeItems->AppendL(difference.Int64());
                          cmaillist->AppendL(tentry2.iDetails);
                          }
                      //Add Non Cmail
                      else
                          {
                          aListItemUids.AppendL( entryUid );
                          aListItems.AppendL( tentry2.iDetails );
                          }
                      }
                  }
              }
          }
      
      if(iEmailFramework)
          {
          AppendCmailBoxesL(timeItems, aEntry);
          CleanupStack::PopAndDestroy( cmaillist );
          CleanupStack::PopAndDestroy( timeItems );
          delete iCmailBoxes;
          iCmailBoxes = NULL;
          }
      }

// ---------------------------------------------------------
// CMceSettingsEmailSel::IfMailBoxAlreadyAdded
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CMceSettingsEmailSel::IfMailBoxAlreadyAdded(CDesCArrayFlat& aListItems, CDesCArrayFlat* aCmaillist, TMsvEntry aTentry)
    {
    TBool append = ETrue;
    if(iEmailFramework)
        {
        //Non Cmail/ Third Parties shold not be added twice
        for ( TInt loop = 0; loop < aListItems.Count(); loop ++ )
            {
            if ( aTentry.iDetails.Compare(aListItems.MdcaPoint(loop)) == 0 )
                {
                append = EFalse;
                break;
                }
            }
            
            //Cmail should not be added doubles 
            for ( TInt loop = 0; loop < aCmaillist->Count(); loop ++ )
                {
                if ( aTentry.iDetails.Compare(aCmaillist->MdcaPoint(loop)) == 0)
                       {
                       append = EFalse;
                       break;
                       }
                }   
        }
    return append;
    }

// ---------------------------------------------------------
// CMceSettingsEmailSel::IsMailBoxAppendable
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CMceSettingsEmailSel::IsMailBoxAppendable(TMsvEntry tentry2, TUid* aUid)
    {
    TBool append = ETrue;

    *aUid = tentry2.iMtm;

    if(!iEmailFramework)
       {
       if ( tentry2.iMtm == KSenduiMtmPop3Uid ||
               tentry2.iMtm == KSenduiMtmImap4Uid ||
               tentry2.iMtm.iUid == iMtmPluginID )
            {
            *aUid = KSenduiMtmSmtpUid;
            }
       }
    else
       {
       if ( (tentry2.iMtm == KSenduiMtmPop3Uid ||
               tentry2.iMtm == KSenduiMtmImap4Uid) && (tentry2.iMtm.iUid != KUidMsgTypeFsMtmVal))
            {
            *aUid = KSenduiMtmSmtpUid;
            }
       }
           

    TBool ret = EFalse;
    TRAPD( err, ret =
           QueryCapabilitiesAndValidateAccountL( *aUid ) )
    if ( ret && ( err == KErrNone ) )
        {
        append = ETrue;
        }
    else
        {
        append = EFalse;
        }
    return append;
    }

// ---------------------------------------------------------
// CMceSettingsEmailSel::IsMailBoxHealthyL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CMceSettingsEmailSel::IsMailBoxHealthyL(TMsvEntry aTentry)
    {
    TBool retval = EFalse;
    // Get all the healthy mailmox list. It filters the unhealthy mailbox.
    CImumInternalApi* emailApi = CreateEmailApiLC( &*iMsvSessionPtr );
    const MImumInHealthServices& mailboxDoctor = emailApi->HealthServicesL();
    
    if ( ( !mailboxDoctor.IsMailboxHealthy( aTentry.Id() ) ) &&
          ( !( ( aTentry.iType == KUidMsvServiceEntry ) && 
               ( aTentry.Id() != KMsvLocalServiceIndexEntryId ) && 
          ( !MuiuEmailTools::IsMailMtm( aTentry.iMtm, ETrue ) ) ) ) )
        {
        retval = EFalse;
        }
    else
        retval = ETrue;
    
    CleanupStack::PopAndDestroy( emailApi );
    return retval;
    }

// ---------------------------------------------------------
// CMceSettingsEmailSel::IfMailBoxUidAlreadyAdded
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CMceSettingsEmailSel::IfMailBoxUidAlreadyAdded(CArrayFix<TUid>* aListItemsUids, TMsvEntry aTentry)
    {
    TBool append = ETrue;
    const TInt count = aListItemsUids->Count();
    // check that related service is not already added to
    // array
    for ( TInt loop = 0; loop < count; loop ++ )
        {
        if ( aTentry.iRelatedId ==
                            aListItemsUids->At( loop ).iUid )
            {
            append = EFalse;
            break;
            }
        
        }
    return append;
    }
	
// ---------------------------------------------------------
// CMceSettingsEmailSel::QueryCapabilitiesAndValidateAccountL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CMceSettingsEmailSel::QueryCapabilitiesAndValidateAccountL(
		TUid aMtmUid )
    {
    TInt response = 0;
    const TUid supportEditor = { KUidMsvMtmQuerySupportEditor };    
    const TUid canCreateNewMsg = { KUidMtmQueryCanCreateNewMsgValue };    
    
    // Check to see that accounts (if required) have been set up
    TMsvId id;
    TMsvEntry indexEntry;

    User::LeaveIfError( iMsvSessionPtr->GetEntry(
                        KMsvRootIndexEntryId, id, indexEntry ) );

    CBaseMtmUiData& mtmUiData = iMtmStore->MtmUiDataL( aMtmUid );

    TInt validAccounts = ETrue;
    TMsvEntry serviceEntry;
    serviceEntry.iType.iUid = KUidMsvServiceEntryValue;
    serviceEntry.iMtm = aMtmUid;

    if ( mtmUiData.CanCreateEntryL( indexEntry, serviceEntry, response ) )
        {
        CMsvEntrySelection* accounts =
        MsvUiServiceUtilities::GetListOfAccountsWithMTML(
                               *iMsvSessionPtr, aMtmUid, ETrue );

        if ( accounts->Count() == 0 )
            {
            validAccounts = EFalse;
            }
        
        delete accounts;
        accounts = NULL;
        }

    if ( !validAccounts )
        {
        iMtmStore->ReleaseMtmUiData( aMtmUid ); // Remove from cache
        return EFalse;
        }

    // Query capabilities
    if ( mtmUiData.QueryCapability( 
            canCreateNewMsg, response ) != KErrNone )
        {
        iMtmStore->ReleaseMtmUiData( aMtmUid ); // Remove from cache
        return EFalse;
        }

    mtmUiData.QueryCapability( KUidMtmQueryMaxBodySize, response );
    if ( response < iRequiredCapabilities.iBodySize )
        {
        iMtmStore->ReleaseMtmUiData( aMtmUid ); // Remove from cache        
        return EFalse;
        }

    mtmUiData.QueryCapability( KUidMtmQueryMaxTotalMsgSize, response );
    if ( response < iRequiredCapabilities.iMessageSize )
        {
        iMtmStore->ReleaseMtmUiData( aMtmUid ); // Remove from cache
        return EFalse;
        }

    if ( iRequiredCapabilities.iFlags == TSendingCapabilities::EAllServices )
        { // No capabilities required from service
        iMtmStore->ReleaseMtmUiData( aMtmUid ); // Remove from cache
        return ETrue;
        }

    // Check if support for attachments are required.
    if ( iRequiredCapabilities.iFlags & 
            TSendingCapabilities::ESupportsAttachments )
        {
        if ( mtmUiData.QueryCapability( 
                KUidMtmQuerySupportAttachments, response ) == KErrNone )
            {
            iMtmStore->ReleaseMtmUiData( aMtmUid ); // Remove from cache
            return ETrue;
            }
        }

    // Check if support for body text is required.
    if ( iRequiredCapabilities.iFlags &
            TSendingCapabilities::ESupportsBodyText )
        {
        if ( mtmUiData.QueryCapability( 
                KUidMtmQuerySupportedBody, response ) == KErrNone )
            {
            iMtmStore->ReleaseMtmUiData( aMtmUid ); // Remove from cache
            return ETrue;
            }
        }

    // Check if support for BIO sending is required.
    if ( iRequiredCapabilities.iFlags & 
            TSendingCapabilities::ESupportsBioSending )
        {
        if ( mtmUiData.QueryCapability(
                KUidMsvQuerySupportsBioMsg, response ) == KErrNone )
            {
            iMtmStore->ReleaseMtmUiData( aMtmUid ); // Remove from cache
            return ETrue;
            }
        }

    // Check if support for editor is required.
    if ( iRequiredCapabilities.iFlags &
            TSendingCapabilities::ESupportsEditor )
        {
        if ( mtmUiData.QueryCapability(
                supportEditor, response ) == KErrNone )
            {
            iMtmStore->ReleaseMtmUiData( aMtmUid ); // Remove from cache
            return ETrue;
            }
        }

    iMtmStore->ReleaseMtmUiData( aMtmUid ); // Remove from cache
    return EFalse;
    }
	
//  End of File

