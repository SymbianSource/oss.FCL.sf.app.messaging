/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 *
 */

#include "unieditormmsplugin_p.h"

// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <msvids.h> 
#include <mtclreg.h>
#include <commdb.h> 
#include <mmsconst.h>
#include <mmsclient.h>
#include <mmsheaders.h>
#include <mmssettings.h>
#include <mmsmsventry.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh> 

#include <MuiuOperationWait.h>
#include <QDir>

#include "UniSendingSettings.h"
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"
#include "s60qconversions.h"
#include "debugtraces.h"
#include "UniEditorGenUtils.h"
#include "sessioneventhandler.h"

// Possible values for mms validity period in seconds
const TInt32 KUniMmsValidityPeriod1h = 3600;
const TInt32 KUniMmsValidityPeriod6h = 21600;
const TInt32 KUniMmsValidityPeriod24h = 86400;
const TInt32 KUniMmsValidityPeriod3Days = 259200;
const TInt32 KUniMmsValidityPeriodWeek = 604800;
const TInt32 KUniMmsValidityPeriodMax = 0;

const TInt  KMaxDetailsLength = 64; // Copy max this many chars to TMsvEntry::iDetails
_LIT( KAddressSeparator, ";" );

#define KSenduiMtmMmsUidValue 0x100058E1
const TUid KSenduiMtmMmsUid = {KSenduiMtmMmsUidValue};

const QString MMSStr("mms");

// -----------------------------------------------------------------------------
// Two-phased constructor.
// @see Header
// -----------------------------------------------------------------------------
//
CUniEditorMmsPluginPrivate* CUniEditorMmsPluginPrivate::NewL()
{
    CUniEditorMmsPluginPrivate* self = new ( ELeave ) CUniEditorMmsPluginPrivate(); 
    return self;
}

// -----------------------------------------------------------------------------
// Destructor
// @see Header
// -----------------------------------------------------------------------------
//
CUniEditorMmsPluginPrivate::~CUniEditorMmsPluginPrivate()
{
    //In case there is no settings at all leave occurs and resource not freed
    delete iMmsHeader;
    delete iMmsMtm;
    delete iMtmRegistry;
    delete iDataModelPluginLoader;
    delete iSessionHandler;
    delete iSession;
}

// -----------------------------------------------------------------------------
// C++ default constructor
// @see Header
// -----------------------------------------------------------------------------
//
CUniEditorMmsPluginPrivate::CUniEditorMmsPluginPrivate( )
: iSessionHandler(0)
{
    iSessionHandler = new SessionEventHandler();
    iSessionHandler->start();
    iSession = CMsvSession::OpenSyncL(*iSessionHandler);
}

// -----------------------------------------------------------------------------
// convertFromL
// @see Header
// -----------------------------------------------------------------------------
//
ConvergedMessage* CUniEditorMmsPluginPrivate::convertFromL( TMsvId aId )
{
    QDEBUG_WRITE("Enter convertFromL");

    MmsMtmL()->SwitchCurrentEntryL( aId );
    MmsMtmL()->LoadMessageL();

    TMsvEntry entry =MmsMtmL()->Entry().Entry();

    ConvergedMessage* msg = new ConvergedMessage;
    
    CleanupStack::PushL(msg);
    if( entry.Parent() == KMsvDraftEntryIdValue )
    {
       convertFromDraftsL(*msg);
    }
    CleanupStack::Pop(msg);
    QDEBUG_WRITE("Exit convertFromL");
    return msg;
}

// -----------------------------------------------------------------------------
// convertFromDraftsL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::convertFromDraftsL(
    ConvergedMessage& aMessage )
{
    QDEBUG_WRITE("Enter convertFromDraftsL");
    
    TMsvEntry entry =MmsMtmL()->Entry().Entry();

    //Message id 
    ConvergedMessageId id(entry.Id());
    aMessage.setMessageId(id);

    // Set Message type
    aMessage.setMessageType(ConvergedMessage::Mms);

    //Populate recipients
    populateRecipientsL(aMessage);

    //populate convergedmessage with the subject
    aMessage.setSubject(S60QConversions::s60DescToQString(
        MmsMtmL()->SubjectL()));

    // Priority
    TMsvPriority priority = entry.Priority();
    if( EMsvHighPriority == priority )
    {
        aMessage.setPriority(ConvergedMessage::High);
    }
    else if( EMsvLowPriority == priority )
    {
        aMessage.setPriority(ConvergedMessage::Low);
    }
    else if( EMsvMediumPriority == priority )
    {
        aMessage.setPriority(ConvergedMessage::Normal);
    }

    // Set direction and location
    aMessage.setDirection(ConvergedMessage::Outgoing);
    aMessage.setLocation (ConvergedMessage::Draft);


    if(!iDataModelPluginLoader)
    {
        iDataModelPluginLoader = new UniDataModelLoader;  
        iDataModelPluginLoader->loadPlugins();  
        iMmsDataPlugin = iDataModelPluginLoader->getDataModelPlugin(MMSStr);
    }

    iMmsDataPlugin->restore(*MmsMtmL());

    int slideCount = iMmsDataPlugin->slideCount();

    int attachmentCount = iMmsDataPlugin->attachmentCount();

    ConvergedMessageAttachmentList attachmentList;

    if (slideCount > 0)
    {
        UniMessageInfoList slideContentList = 
            iMmsDataPlugin->slideContent(0);
        for (int i = 0; i < slideContentList.size(); ++i)
        {
            if( slideContentList.at(i)->mimetype().contains("text") )
            {
                QString textContent;
                QFile file(slideContentList.at(i)->path());
                file.open(QIODevice::ReadOnly);
                textContent = file.readAll();
                aMessage.setBodyText(textContent);
                file.close();
            }
            else
            {
            ConvergedMessageAttachment* attachment =
                new ConvergedMessageAttachment(
                    slideContentList.at(i)->path(),
                    ConvergedMessageAttachment::EInline);
            attachmentList << attachment;
            }
        }
        
        foreach(UniMessageInfo* slide,slideContentList)
        {
            delete slide;
        }
    }

    
    if(attachmentCount > 0)
    {
        UniMessageInfoList modelAttachmentList = 
            iMmsDataPlugin->attachmentList();

        for (int i = 0; i < modelAttachmentList.count(); ++i)
        {
            ConvergedMessageAttachment* attachment =
                new ConvergedMessageAttachment(
                    modelAttachmentList.at(i)->path(),
                    ConvergedMessageAttachment::EAttachment);
            attachmentList << attachment;
        }
        
        foreach(UniMessageInfo* attachment,modelAttachmentList)
        {
            delete attachment;
        }
    }

    if(attachmentList.count() > 0)
    {
        aMessage.addAttachments(attachmentList);
    }       

    //Delete the pluginloader instance
    delete iDataModelPluginLoader;
    iDataModelPluginLoader = NULL;
    iMmsDataPlugin = NULL;

    QDEBUG_WRITE("Exit convertFromDraftsL");
}

// -----------------------------------------------------------------------------
// convertToL
// @see Header
// -----------------------------------------------------------------------------
//
TMsvId CUniEditorMmsPluginPrivate::convertToL(ConvergedMessage *aMessage)
{
    QDEBUG_WRITE("Enter ConvertToL");

    if (!iDataModelPluginLoader)
    {
        iDataModelPluginLoader = new UniDataModelLoader;
        iDataModelPluginLoader->loadPlugins();
        iMmsDataPlugin = iDataModelPluginLoader->getDataModelPlugin(MMSStr);
    }

    // create message in drafts.
    CMsvEntry* cEntry = MmsMtmL()->Session().GetEntryL(KMsvDraftEntryId);

    CleanupStack::PushL(cEntry);
    MmsMtmL()->SwitchCurrentEntryL(cEntry->EntryId());

    TMsvEntry entry;
    entry.iMtm = MmsMtmL()->Type();
    entry.iType = KUidMsvMessageEntry;
    entry.iServiceId = MmsMtmL()->DefaultServiceL();
    entry.iDate.UniversalTime();

    cEntry->CreateL(entry);

    TMsvId entryId = entry.Id();

    //Since entry is created if any of the below functions leave 
    //the created entry has to be deleted
    TInt error = KErrNone;
    TRAP(error,DoConvertToL(aMessage,entry));
    
    CleanupStack::PopAndDestroy(cEntry);
    
    //Delete the pluginloader instance
    delete iDataModelPluginLoader;
    iDataModelPluginLoader = NULL;
    iMmsDataPlugin = NULL;

    if (error != KErrNone)
    {
        this->deleteDraftsEntryL(entryId);
        entryId = -1; //Set Invalid entry id
    }
    QDEBUG_WRITE_FORMAT("Exit ConvertToL the entryId= ",entryId);
    return entryId;
}

// -----------------------------------------------------------------------------
// DoConvertToL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::DoConvertToL(ConvergedMessage *aMessage,
    TMsvEntry& entry)
{
    TMsvId entryId = entry.Id();
 
     // switch to created entry
     MmsMtmL()->SwitchCurrentEntryL(entryId);

     //Add recipients
     addRecipientsL(aMessage);

     //There is no size check inside plugin as it assumes 
     //we get proper data from editor
     HBufC* sub = S60QConversions::qStringToS60Desc(aMessage->subject());
     if( sub )
     {
         CleanupStack::PushL(sub);
         MmsMtmL()->SetSubjectL( *sub );
         CleanupStack::PopAndDestroy(sub);
     }

     //Add attachments
     // fetch attachment list and populate the smil model
     ConvergedMessageAttachmentList attachmentlist = aMessage->attachments();
     int attachmentcount = attachmentlist.count();
     if ((attachmentcount> 0) || !(aMessage->bodyText().isEmpty()))
     {
         QDEBUG_WRITE("Before calling iMmsDataPlugin->restore");
         iMmsDataPlugin->restore(*MmsMtmL());

         QDEBUG_WRITE("Before calling iMmsDataPlugin->addSlide");

         bool slideContentAdded = false;

         //Adding first slide
         iMmsDataPlugin->addSlide(0);

         for(int i=0; i < attachmentcount; i++)
         {
             //only file path is needed to be passed 
             UniMessageInfo *info = new UniMessageInfo(
                 QDir::toNativeSeparators(attachmentlist[i]->filePath()),
                                          0,
                                          QString());

             if(attachmentlist[i]->attachmentType() ==
                 ConvergedMessageAttachment::EInline )
             {
                 slideContentAdded = true;
                 iMmsDataPlugin->addObject(0,info);
             }
             else if(attachmentlist[i]->attachmentType() ==
                 ConvergedMessageAttachment::EAttachment)
             {
                 //Add attachment
                 iMmsDataPlugin->addAttachment(info);
             }
             delete info;
         }

         if(!(aMessage->bodyText().isEmpty()))
         {
             slideContentAdded = true;
             iMmsDataPlugin->addTextObject(0,aMessage->bodyText());
         }

         if( slideContentAdded )
         {
             iMmsDataPlugin->saveObjects();

             // Open store even if we didn't need it here.
             // -> We don't need to check later whether the store 
             //is open or not.    
             CMsvStore* editStore = MmsMtmL()->Entry().EditStoreL();
             
             //Ensure that the store is deleted if anything 
             //leaves before deletion otherwise the entry store will be locked
             //and entry cant be deleted
             CleanupStack::PushL(editStore);
              
             //Compose the smil if slide is added
             iMmsDataPlugin->composeSmil(*editStore);

             //Commit the store before setting the root
             editStore->CommitL();

             //delete the store before setMessageRoot is called
             CleanupStack::PopAndDestroy(editStore);

             //Set message root after composing SMIL but before 
             //calling SaveMessageL and after the store has been deleted
             MmsMtmL()->SetMessageRootL( 
                 iMmsDataPlugin->getSmilAttachmentByIndex( ));
         }
         else
         {
             //Remove the slide   
             iMmsDataPlugin->removeSlide(0);

             // Open store even if we didn't need it here.
             // -> We don't need to check later whether the store is open or not.    
             CMsvStore* editStore = MmsMtmL()->Entry().EditStoreL();
             
             //Ensure that the store is deleted if anything 
             //leaves before deletion otherwise the entry store will be locked
             //and entry cant be deleted
             CleanupStack::PushL(editStore);
             
             //Commit the store before setting the root
             editStore->CommitL();

             //delete the store before setMessageRoot is called
             CleanupStack::PopAndDestroy(editStore);
         }
     }

     //Set the priority before calling save
     ConvergedMessage::Priority priority = aMessage->priority();
     if (ConvergedMessage::High == priority)
     {
         MmsMtmL()->SetMessagePriority(EMmsPriorityHigh);
     }
     else if (ConvergedMessage::Low == priority)
     {
         MmsMtmL()->SetMessagePriority(EMmsPriorityLow);
     }
     else if (ConvergedMessage::Normal == priority)
     {
         MmsMtmL()->SetMessagePriority(EMmsPriorityNormal);
     }

     //Saving the changes
     MmsMtmL()->SaveMessageL();

     entry = MmsMtmL()->Entry().Entry();

     TBuf<KMaxDetailsLength> detailsBuf;
     MakeDetailsL( detailsBuf );
     entry.iDetails.Set( detailsBuf );

     MmsMtmL()->Entry().ChangeL(entry); // commit changes      

}

// -----------------------------------------------------------------------------
// sendL
// @see Header
// -----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::sendL( TMsvId aId )
{
    QDEBUG_WRITE("Enter sendL");

    MmsMtmL()->SwitchCurrentEntryL( aId );
    MmsMtmL()->LoadMessageL();
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    CMsvOperation* oper = MmsMtmL()->SendL( wait->iStatus );
    CleanupStack::PushL( oper );
    wait->Start();
    CleanupStack::PopAndDestroy( oper );
    CleanupStack::PopAndDestroy( wait );

    QDEBUG_WRITE("Exit sendL");
}

// -----------------------------------------------------------------------------
// validateServiceL
// @see Header
// -----------------------------------------------------------------------------
//
TBool CUniEditorMmsPluginPrivate::validateServiceL( TBool /*aEmailOverSms*/ )
{
    QDEBUG_WRITE("Enter ValidateServiceL");

    //Check if the mms client mtm object is already created or not
    if( iMmsMtm )
    {
        // If mms client mtm object is already created restore the settings
        iMmsMtm->RestoreSettingsL();
    }

    TMsvId service = MmsMtmL()->DefaultServiceL();
    TBool valid( MmsMtmL()->ValidateService( service ) == KErrNone );

    QDEBUG_WRITE_FORMAT("Exit ValidateServiceL the return val= ",valid);
    return valid;
}

// -----------------------------------------------------------------------------
// isServiceValidL
// @see Header
// -----------------------------------------------------------------------------
//
TBool CUniEditorMmsPluginPrivate::isServiceValidL()
{
    return MmsMtmL()->ValidateService( MmsMtmL()->DefaultServiceL() ) == KErrNone;
}

// -----------------------------------------------------------------------------
// MmsMtmL
// @see Header
// -----------------------------------------------------------------------------
//
CMmsClientMtm* CUniEditorMmsPluginPrivate::MmsMtmL()
{
    if ( !iMmsMtm )
    {
        if ( !iMtmRegistry )
        {            
            iMtmRegistry = CClientMtmRegistry::NewL( *iSession );
        }
        iMmsMtm = static_cast<CMmsClientMtm*>( iMtmRegistry->NewMtmL( 
            KSenduiMtmMmsUid ) );
    }
    return iMmsMtm;
}

// ---------------------------------------------------------
// MakeDetailsL
// @see Header
// ---------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::MakeDetailsL( TDes& aDetails )
{
    // This very same code can be found in CUniAddressHandler. 
    // They should be put in common location some day...
    const CMsvRecipientList& addresses = MmsMtmL()->AddresseeList();
    TInt addrCnt = addresses.Count();

    TPtrC stringToAdd;
    for ( TInt i = 0; i < addrCnt; i++)
    {
        // Only address is converted to western. 
        // There may numbers in contact name - they must not be converted 
        TPtrC alias = TMmsGenUtils::Alias( addresses[i] );
        HBufC* addressBuf = NULL;

        if ( alias.Length() != 0 )
        {
            stringToAdd.Set( alias );
        }
        else
        {
            TPtrC address = TMmsGenUtils::PureAddress( addresses[i] );
            addressBuf = HBufC::NewLC( address.Length() );
            TPtr addressPtr = addressBuf->Des();
            addressPtr.Copy( address );
            stringToAdd.Set( addressPtr );

            // Internal data structures always holds the address data in western format.
            // UI is responsible of doing language specific conversions.    
            //MuiuTextUtils::ConvertDigitsTo( addressPtr, EDigitTypeWestern );
            UniEditorGenUtils* genUtils = new UniEditorGenUtils();
            genUtils->ConvertDigitsTo( addressPtr, EDigitTypeWestern );
            delete genUtils;
        }

        if ( ( aDetails.Length() != 0 ) &&   // Not a first address
                ( aDetails.Length() + KAddressSeparator().Length() < KMaxDetailsLength ) )
        {
            // Add separator
            aDetails.Append( KAddressSeparator() );
        }

        if ( aDetails.Length() + stringToAdd.Length() < KMaxDetailsLength ) 
        {
            // whole string fits. Add it.
            aDetails.Append( stringToAdd );
            if ( addressBuf )
            {
                CleanupStack::PopAndDestroy( addressBuf );
            }
        }
        else
        {
            // Only part of the string fits
            TInt charsToAdd = KMaxDetailsLength - aDetails.Length();

            if ( charsToAdd <= 0 )
            {
                // Cannot add any more chars 
                break;
            }

            if ( charsToAdd >= stringToAdd.Length() )
            {
                // Guarantee that charsToAdd is not larger that stringToAdd lenght 
                charsToAdd = stringToAdd.Length();
            }

            aDetails.Append( stringToAdd.Left( charsToAdd ) );
            if ( addressBuf )
            {
                CleanupStack::PopAndDestroy( addressBuf );
            }
            break;
        }
    }
}

//---------------------------------------------------------------
// CUniEditorMmsPluginPrivate::addRecipientsL
// @see header
//---------------------------------------------------------------
void CUniEditorMmsPluginPrivate::addRecipientsL(ConvergedMessage *message)
{
    //add To feilds
    ConvergedMessageAddressList toAddressArray = message->toAddressList();
    addRecipientsL(toAddressArray, EMsvRecipientTo);

    //add cc feilds
    ConvergedMessageAddressList ccAddressArray = message->ccAddressList();
    addRecipientsL(ccAddressArray, EMsvRecipientCc);

    //add bcc feilds
    ConvergedMessageAddressList bccAddressArray = message->bccAddressList();
    addRecipientsL(bccAddressArray, EMsvRecipientBcc);
}

//---------------------------------------------------------------
// CUniEditorMmsPluginPrivate::addRecipientsL
// @see header
//---------------------------------------------------------------
void CUniEditorMmsPluginPrivate::addRecipientsL(
    const ConvergedMessageAddressList &array,
    TMsvRecipientType recpType)
{    

    for (int i = 0; i < array.count(); ++i)
    {
        if(array[i]->address().isEmpty())
        {
            continue;
        }
        // convert from QString to HBufC
        HBufC* addr = S60QConversions::qStringToS60Desc(array[i]->address());

        CleanupStack::PushL(addr);

        if(TMmsGenUtils::Alias(*addr).Length() > 0)
        {    
            MmsMtmL()->AddAddresseeL(recpType, TMmsGenUtils::PureAddress(*addr),
                TMmsGenUtils::Alias(*addr));
        }
        else
        {
            HBufC* displayName = S60QConversions::qStringToS60Desc(array[i]->alias());
            if(displayName)
            {
                CleanupStack::PushL(displayName);

                MmsMtmL()->AddAddresseeL(recpType, TMmsGenUtils::PureAddress(*addr),
                    *displayName);

                CleanupStack::PopAndDestroy(displayName);
            }
            else
            {
                MmsMtmL()->AddAddresseeL(recpType, TMmsGenUtils::PureAddress(*addr));    
            }
        }
        CleanupStack::PopAndDestroy(addr);
    }
}

// ----------------------------------------------------------------------------
// CUniEditorMmsPluginPrivate::populateRecipientsL
// @see header
// ----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::populateRecipientsL(
    ConvergedMessage &aMessage)
    {
    QDEBUG_WRITE("Enter populateRecipientsL");


    // get recipient list
    const CMsvRecipientList& addresses = MmsMtmL()->AddresseeList();
    TInt count = addresses.Count();

    // extract each address and populate into ConvergedMessageAddress
    for (TInt i = 0; i < count; ++i)
        {
        //Address
        QString address = S60QConversions::s60DescToQString(
            TMmsGenUtils::PureAddress(addresses[i]));
        //Alias
        QString alias = S60QConversions::s60DescToQString(
            TMmsGenUtils::Alias(addresses[i]));

        //add recipient to convergedMessage
        ConvergedMessageAddress messageAddress(address, alias);
        if (addresses.Type(i) == EMsvRecipientTo)
            {
            aMessage.addToRecipient(messageAddress);
            }
        else if (addresses.Type(i) == EMsvRecipientCc)
            {
            aMessage.addCcRecipient(messageAddress);
            }
        else if (addresses.Type(i) == EMsvRecipientBcc)
            {
            aMessage.addBccRecipient(messageAddress);
            }
        }

    QDEBUG_WRITE("Exit populateRecipientsL");

    }

// ----------------------------------------------------------------------------
// CUniEditorMmsPluginPrivate::deleteDraftsEntryL
// @see header
// ----------------------------------------------------------------------------
//
void CUniEditorMmsPluginPrivate::deleteDraftsEntryL( TMsvId aId )
{
    CMsvEntry* pEntry = iSession->GetEntryL(KMsvDraftEntryIdValue);
    CleanupStack::PushL(pEntry);
    pEntry->DeleteL( aId );
    CleanupStack::PopAndDestroy(pEntry);
}

//  End of File
