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
* Description:  
*     Item array for the message items to be displayed by mce.
*     Main view items are kept in memory (CArrayFixFlat<TMceListItem>) when created but
*     Message view list items are fetched from the msgs when needed in MdcaPoint function
*
*/



// INCLUDE FILES

#include <AknUtils.h>       // AknTextUtils
#include <mtudcbas.h>
#include <msvids.h>
#include <NpdApi.h>         // CNotepadApi
#include "MtmRegistryObserver.h" // MMtmUiDataRegistryObserver
#include <StringLoader.h>   // StringLoader
#include <mce.rsg>
#include <SenduiMtmUids.h>  // mtm uids
#include <eikenv.h>
#include <gulicon.h>

#include <MTMStore.h>
#include <commonphoneparser.h>

#include <aknappui.h>
#include <akncontext.h>
#include <aknconsts.h>
#include <e32math.h>

#include <bldvariant.hrh>
#include <featmgr.h>
#include <NumberGrouping.h>
#include <mmsconst.h>
#include <mmsclient.h>
#include <mtclreg.h>
#include <muiu.mbg>

#include <data_caging_path_literals.hrh>
#include <miutset.h>
#include <msvapi.h>

#include <centralrepository.h>
#include <messaginginternalcrkeys.h>

#include "MceUtils.h"
#include "MceOneRowMessageListArray.h"
#include "MceOneRowMessageListArrayLoader.h"
#include "MceListItem.h"
#include "MceIds.h"


// CONSTANTS
const TInt KMceArabicSemiColon      = {0x061B};
const TInt KMceSemiColon            = {0x003B};
const TInt KMceMaxFormatFactor      = 2;
const TText KMceRLM                 = 0x200F;
const TText KMcePDF                 = 0x202C;
// const TUid KUidMsvMessageListSortOrder = {0x10005359};

const TInt KMceSubtitleLength       = 50;
const TInt KListGranularity         = 22;

const TInt KMceTimeHours    = 23;
const TInt KMceTimeMinutes  = 59;
const TInt KMceTimeSeconds  = 59;
const TInt KMceTimeMicroSeconds  = 999999;

// LOCAL CONSTANTS AND MACROS
enum TListArrayDocAmounts
    {
    KMceListArrayDocAmountOneRow = 0x0,
    KMceListArrayDocAmountManyOneRow,
    KMceListArrayDocAmountTempOneRow,
    KMceListArrayDocAmountTempsOneRow,
    KMceListArrayDocAmountMailOneRow,
    KMceListArrayDocAmountMailsOneRow
    };

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CMceOneRowListItemArray::CMceOneRowListItemArray(
    CMsvSessionPtr aSession,
    TMsvId aFolderId )
    :
    iSession( aSession ),
    iFolderId( aFolderId )
    {
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::ConstructL
// Symbian OS default constructor can leave.
// ----------------------------------------------------
void CMceOneRowListItemArray::ConstructL()
    {
    iMceListItemArrayLoader = CMceOneRowListItemArrayLoader::NewL(
        iSession );
    iNumberOfMessagesStrings = new(ELeave)CDesCArrayFlat( 6 );
    TBuf<KMcePrintableTextLength> tempString;
    iOwnEikonEnv = CEikonEnv::Static();
    StringLoader::Load( tempString, R_MCE_DOC_AMOUNT_1ROW, iOwnEikonEnv );
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_DOC_AMOUNT_MANY_1ROW, iOwnEikonEnv );
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_DOC_AMOUNT_TEMP_1ROW, iOwnEikonEnv );
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_DOC_AMOUNT_TEMPS_1ROW, iOwnEikonEnv );
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_MAILBOX_AMOUNT_MAIL_1ROW, iOwnEikonEnv );
    iNumberOfMessagesStrings->AppendL( tempString );
    StringLoader::Load( tempString, R_MCE_MAILBOX_AMOUNT_MAILS_1ROW, iOwnEikonEnv );
    iNumberOfMessagesStrings->AppendL( tempString );


    // Tree list subtitles
    iSubtitleStrings = new(ELeave)CDesCArrayFlat( KListGranularity );
    iSubtitleStrings->Reset();
    TBuf<KMceSubtitleLength> tmpBuf;
    StringLoader::Load( tmpBuf, R_MSG_LIST_SUBTITLE_MONDAY, iOwnEikonEnv );
    iSubtitleStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_MSG_LIST_SUBTITLE_TUESDAY, iOwnEikonEnv );
    iSubtitleStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_MSG_LIST_SUBTITLE_WEDNESDAY, iOwnEikonEnv );
    iSubtitleStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_MSG_LIST_SUBTITLE_THURSDAY, iOwnEikonEnv );
    iSubtitleStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_MSG_LIST_SUBTITLE_FRIDAY, iOwnEikonEnv );
    iSubtitleStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_MSG_LIST_SUBTITLE_SATURDAY, iOwnEikonEnv );
    iSubtitleStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_MSG_LIST_SUBTITLE_SUNDAY, iOwnEikonEnv );
    iSubtitleStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_MSG_LIST_SUBTITLE_TODAY, iOwnEikonEnv );
    iSubtitleStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_MSG_LIST_SUBTITLE_YESTERDAY, iOwnEikonEnv );
    iSubtitleStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_MSG_LIST_SUBTITLE_LAST_WEEK, iOwnEikonEnv );
    iSubtitleStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_MSG_LIST_SUBTITLE_OLDER, iOwnEikonEnv );
    iSubtitleStrings->AppendL( tmpBuf );
    
    iDescriptionLength = MceUtils::DescriptionLengthL();
    iMceListItemArrayLoader->StartL();
    }


// ----------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------
CMceOneRowListItemArray* CMceOneRowListItemArray::NewL(
    CMsvSessionPtr aSession,
    TMsvId aFolderId )
    {
    CMceOneRowListItemArray* self = new (ELeave) CMceOneRowListItemArray(
        aSession, aFolderId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }


// ----------------------------------------------------
// Destructor
// ----------------------------------------------------
CMceOneRowListItemArray::~CMceOneRowListItemArray( )
    {
    delete iMceListItemArrayLoader;
    delete iNumberOfMessagesStrings;
    delete iSubtitleStrings;
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::SetFolderL
// ----------------------------------------------------
void CMceOneRowListItemArray::SetFolderL( TMsvId aFolderId )
    {
    iMceListItemArrayLoader->SetEntryL(aFolderId);
    iFolderId = aFolderId;
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::FolderId
// ----------------------------------------------------
TMsvId CMceOneRowListItemArray::FolderId()
    {
    return iMceListItemArrayLoader->CurrentEntryId();
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::FolderEntry
// ----------------------------------------------------
const TMsvEntry& CMceOneRowListItemArray::FolderEntry() const
    {
    return iMceListItemArrayLoader->FolderEntry();
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::ItemId
// ----------------------------------------------------
TMsvId CMceOneRowListItemArray::ItemId( TInt aIndex )
    {
    return iMceListItemArrayLoader->ItemId( aIndex );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::ItemIndex
// ----------------------------------------------------
TInt CMceOneRowListItemArray::ItemIndex(TMsvId aItemId) const
    {
    return iMceListItemArrayLoader->ItemIndex( aItemId );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::RefreshArrayL
// ----------------------------------------------------
void CMceOneRowListItemArray::RefreshArrayL()
    {
    iMceListItemArrayLoader->StartL();
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::MdcaCount
// ----------------------------------------------------
TInt CMceOneRowListItemArray::MdcaCount() const
    {
    return ( iMceListItemArrayLoader->Count() );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::GetSubtitleTextL
// ----------------------------------------------------
TInt CMceOneRowListItemArray::GetSubtitleTextL( TInt aIndex, TDes& aTitleText)
    {
    TMsvEntry child;
    TInt error = iMceListItemArrayLoader->GetEntry( aIndex, child );

    aTitleText.Zero();

    if ( error == EHCListTypeTime )
        {
        TInt subtitleType = iMceListItemArrayLoader->GetTimeBasedSubtitle( aIndex );
        aTitleText.Append( (*iSubtitleStrings)[subtitleType] );

        // Add time information to subtitle string if there is parameter in string 
        TPtrC searchString( aTitleText );
        _LIT(KSkipParam, "*%U*");
        TInt temp = searchString.Match( KSkipParam );
        if ( temp > KErrNotFound )
            {
            TBuf<30> dateBuffer;
            AppendTimeStringL( dateBuffer, child, ETrue );
            TTime entryTime = child.iDate;
            TTime currentTime;
            TTime dateNow;
            currentTime.HomeTime();
            GetTime( entryTime, dateNow );
            GetTime( currentTime, dateNow );
            TTimeIntervalDays dayDiff = currentTime.DaysFrom(entryTime);
            TTimeIntervalDays dayToComp(0);

            if(entryTime > currentTime || dayDiff < dayToComp) // MSPO-7HX9ST
                {
                CCoeEnv* iownEikonEnvmak = CEikonEnv::Static();
                HBufC* dateFormat = iownEikonEnvmak->AllocReadResourceLC( R_QTN_DATE_WITHOUT_YEAR_WITH_ZERO );
                currentTime.FormatL( dateBuffer, *dateFormat );
                CleanupStack::PopAndDestroy( dateFormat );
                }
            StringLoader::Format( aTitleText,
                (*iSubtitleStrings)[subtitleType],
                -1, // no index in the key string
                dateBuffer );                
            }
        }
    else if ( error == EHCListTypeSize )
        {
        _LIT( KHCListTestingFormatSize, "size %d");
        aTitleText.Format( KHCListTestingFormatSize, child.iSize );
        }
    else if ( error == EHCListTypeSender )
        {
        if ( child.iMtm == KSenduiMtmImap4Uid || child.iMtm == KSenduiMtmPop3Uid
            || child.iMtm == KSenduiMtmSmtpUid )
            {
            aTitleText.Append( child.iDetails.Left( KOneRowListColumnTextSize ) );
            }
        else
            {
            AppendRecipientStringL( aTitleText, child );
            }
        }
    else if ( error == EHCListTypeSubject )
        {
       StripAndAppendString( aTitleText, child.iDescription.Left( KOneRowListColumnTextSize ));//ADD EAWG-7EKCX2
        }

    return error;
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::GetColumnTextL
// ----------------------------------------------------
TInt CMceOneRowListItemArray::GetColumnTextL( TInt aIndex, TInt aColumn, TDes& aText )
    {
    // TODO, performance optimisation: one function call for all three text buffers...

    // aColumn: 1 = C1, 2 = C2, 3 = C3, others ignored at the moment...
    if ( aColumn < 1 || aColumn > 3 )
        {
        aText.Zero();
        return KErrNotSupported;
        }

    TMsvEntry child;
    TInt error = iMceListItemArrayLoader->GetEntry( aIndex, child );

    aText.Zero();
    
    if ( error == KErrNone )
        {
       //Check if the entry is requiered folder entry and fill one row specific string
       if ( child.iType == KUidMsvFolderEntry )
            {
            if( IsFolderTemplate(child, aText, aColumn) )
                {
                return error;
                }
            }
        
        THCListListType listType = iMceListItemArrayLoader->SortTypeHCList();
        // sort by:
        // time:    C1 = sender, C2 = title, C3 = time
        // sender:  C1 = title,  C2 = time,  C3 = empty
        // title:   C1 = sender, C2 = time,  C3 = empty
        switch( listType )
            {
            default:
            case EHCListTypeTime:
                {
                if ( aColumn == 1 )
                    {
                    if ( child.iMtm == KSenduiMtmImap4Uid || child.iMtm == KSenduiMtmPop3Uid
                        || child.iMtm == KSenduiMtmSmtpUid )
                        {
                        aText.Copy( child.iDetails.Left( KOneRowListColumnTextSize ) );
                        }
                    else
                        {
                        AppendRecipientStringL( aText, child );
                        }
                    }
                else if ( aColumn == 2 )
                    {
                    StripAndAppendString( aText, child.iDescription.Left( KOneRowListColumnTextSize ));
                    }
                else
                    {
                    if ( child.iType == KUidMsvFolderEntry )
                        {
                        AppendFolderString( aText, child );        
                        }                        
                    else if ( child.iType == KUidMsvMessageEntry )
                        {
                        TBool bShowTime = ETrue ;
                        TInt subTitleNum = iMceListItemArrayLoader->GetTimeBasedSubtitle (aIndex) ;
                        if ( subTitleNum > CMceOneRowListItemArrayLoader::EYesterdayAdded )
                           {
                           bShowTime = EFalse;
                           }
                        AppendTimeStringL( aText, child, EFalse, bShowTime);
                        bShowTime = ETrue ;
                        }
                    }                    
                }
            break;
            case EHCListTypeSender:
                {
                if ( child.iType == KUidMsvMessageEntry )
                    {
                    if ( aColumn == 1 )
                        {
                       StripAndAppendString( aText, child.iDescription.Left( KOneRowListColumnTextSize )); //ADD - EAWG-7EKCX2
                        }
                    else if ( aColumn == 2 )
                        {
                        TBool subTitleNum = IsTodaysEntry( child ) ;
                        AppendTimeStringL( aText, child, EFalse, subTitleNum );
                        }
                    }
                else if ( aColumn == 1 )
                    {
                    aText.Copy( child.iDetails.Left( KOneRowListColumnTextSize ) );
                    }
                }
            break;
            case EHCListTypeSubject:
                {
                if ( child.iType == KUidMsvMessageEntry )
                    {
                    if ( aColumn == 1 )
                        {
                         if ( child.iMtm == KSenduiMtmImap4Uid || child.iMtm == KSenduiMtmPop3Uid
                            || child.iMtm == KSenduiMtmSmtpUid )
                            {
                            aText.Copy( child.iDetails.Left( KOneRowListColumnTextSize ) );
                            }
                        else
                            {
                            AppendRecipientStringL( aText, child );
                            }
                        }
                    else if ( aColumn == 2 )
                        {
                        TBool subTitleNum = IsTodaysEntry( child ) ;
                        AppendTimeStringL( aText, child, EFalse, subTitleNum );
                        }
                    }
                else if ( aColumn == 1 )
                    {
                    aText.Copy( child.iDetails.Left( KOneRowListColumnTextSize ) );
                    }
                }
            break;
            }
        }
    return error;
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::GetTime
// ----------------------------------------------------
void CMceOneRowListItemArray::GetTime( TTime& aEntryTime, TTime& aCurrentTime ) const
    {
    aCurrentTime.HomeTime();

    TTime timeNow;
    timeNow.UniversalTime();

    TTimeIntervalMinutes timeZone;
    if ( aCurrentTime.MinutesFrom(timeNow, timeZone) == KErrNone )
        {
        aEntryTime = aEntryTime + timeZone;
        }

    TDateTime dateTime = aCurrentTime.DateTime();
    dateTime.SetHour( KMceTimeHours );
    dateTime.SetMinute( KMceTimeMinutes );
    dateTime.SetSecond( KMceTimeSeconds );
    dateTime.SetMicroSecond( KMceTimeMicroSeconds );

    aCurrentTime = dateTime;

    }


// ----------------------------------------------------
// CMceOneRowListItemArray::AppendTimeStringL
// ----------------------------------------------------
void CMceOneRowListItemArray::AppendTimeStringL( TDes& aText, const TMsvEntry& aEntry, TBool aSubtitle, TBool aShowTime ) const
    {
    TBuf<30> dateBuffer;

    TTime entryTime;
    entryTime = aEntry.iDate;
    TTime dateNow;
    GetTime( entryTime, dateNow );
   
    if ( aShowTime && !aSubtitle )
        {
        // show time
        HBufC* timeFormat = iOwnEikonEnv->
            AllocReadResourceLC( R_QTN_TIME_USUAL_WITH_ZERO );

        entryTime.FormatL( dateBuffer, *timeFormat );
        AknTextUtils::LanguageSpecificNumberConversion( dateBuffer );
        CleanupStack::PopAndDestroy( timeFormat );
        }
    else
        {
        // show date
        HBufC* dateFormat = iOwnEikonEnv->
            AllocReadResourceLC( R_QTN_DATE_WITHOUT_YEAR_WITH_ZERO );

        entryTime.FormatL( dateBuffer, *dateFormat );
        AknTextUtils::LanguageSpecificNumberConversion( dateBuffer );
        CleanupStack::PopAndDestroy( dateFormat );
        }
    aText.Append( dateBuffer );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::AppendFolderString
// ----------------------------------------------------
void CMceOneRowListItemArray::AppendFolderString( TDes& aBuffer, const TMsvEntry& aEntry ) const
    {
    if ( aEntry.Id() == KMceTemplatesEntryId )
        {
        TInt templatesCount = CNotepadApi::NumberOfTemplates();
        templatesCount = templatesCount < 0 ? 0 : templatesCount;
        if ( templatesCount == 1 )
            {
            TBuf<KMcePrintableTextLength> tempString;
            tempString.Append( (*iNumberOfMessagesStrings)[
                KMceListArrayDocAmountTempOneRow] );
            AknTextUtils::LanguageSpecificNumberConversion( tempString );
            aBuffer.Append( tempString );
            }
        else
            {
            TBuf<KMcePrintableTextLength> tempString;
            StringLoader::Format( tempString,
                (*iNumberOfMessagesStrings)[KMceListArrayDocAmountTempsOneRow],
                -1, // no index in the key string
                templatesCount );
            AknTextUtils::LanguageSpecificNumberConversion( tempString );
            aBuffer.Append( tempString );
            }
        }
    else
        {
        TRAP_IGNORE( HandleFolderMessagesStringL( aBuffer, aEntry ) );
        }
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::HandleSessionEventL
// ----------------------------------------------------
void CMceOneRowListItemArray::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    switch (aEvent)
        {
        case EMsvEntriesCreated:
        case EMsvEntriesChanged:
        case EMsvEntriesDeleted:
        case EMsvMediaChanged:
        case EMsvMediaUnavailable:
        case EMsvMediaAvailable:
        case EMsvCloseSession:
            break;

        default:
            break;
        }
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::StripAndAppendString
// ----------------------------------------------------
void CMceOneRowListItemArray::StripAndAppendString(TDes& aTargetString, const TDesC& aSourceString) const
    {
    TRAP_IGNORE( MceUtils::StripAndAppendStringL(
        aTargetString,
        aSourceString,
        iDescriptionLength ) );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::AppendRecipientStringL
// handles recipient string
//
// ----------------------------------------------------
void CMceOneRowListItemArray::AppendRecipientStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const
    {

    //
    // The following processes the recipient(s) for languages which require number
    // conversion and/or number grouping
    //
    if ( AknLayoutUtils::LayoutMirrored() || //iNumberGroupingEnabledFlag ||
           EDigitTypeWestern != AknTextUtils::NumericEditorDigitType() )
        {

        //
        // semicolon separator
        //
        TBuf<1> semiColon;
        semiColon.Append( KMceSemiColon );
        TInt posForSemiColon = KErrNotFound;

        //
        // arabic semicolon separator
        //
        TBuf<1> arabicSemiColon;
        arabicSemiColon.Append( KMceArabicSemiColon );
        TInt posForArabicSemiColon = KErrNotFound;

        //
        // The following are used for keeping track of which type of separator we're
        // currently processing
        //
        TBuf<1> separator;
        TInt posForSeparator = KErrNotFound;

        TInt len = aEntry.iDetails.Length();

        //
        // recipient buffer is made to be larger than the length of the entry details
        // to allow additional room needed for number grouping
        //
        HBufC* recipientBuf = HBufC::NewLC( KMceMaxFormatFactor*len );
        TPtr recipientPtr = recipientBuf->Des();

        //
        // remaining details buffer is initialized to the same as the entry details
        // and we will remove processed info until this buffer becomes empty
        //
        HBufC* remainingDetailsBuf = HBufC::NewLC( len );
        TPtr remainingDetailsPtr = remainingDetailsBuf->Des();
        remainingDetailsPtr.Copy( aEntry.iDetails );

        //
        // result buffer is made to be larger than the length of the entry details
        // to allow additional room needed for number grouping
        //
        HBufC* resultBuf = HBufC::NewLC( KMceMaxFormatFactor*len );
        TPtr resultPtr = resultBuf->Des();

        //
        // Construct the result buffer from the entry details. Each recipient is
        // extracted from the entry details and valid phone numbers are number
        // grouped and converted as needed before they are concatenated into a
        // string separated by semicolon/arabic semicolon
        //
        do
            {

            // find the first occurrence of semi colon in the remaining details buffer
            posForSemiColon = remainingDetailsPtr.Find( semiColon );

            // find the first occurrence of arabic semicolon in the remaining details
            // buffer
            posForArabicSemiColon = remainingDetailsPtr.Find( arabicSemiColon );

            //
            // 5 possible cases:
            //
            // 1) Neither semicolon nor arabic semicolon is found in the remaining
            //    details buffer:
            //
            //    we will take the first KPhCltNameBufferLength to the recipient buffer
            //    to proceed with our number grouping and/or conversion. At the end of
            //    this iteration, there will not be any in the remaining details buffer
            //    to be processed. This is either the single recipient case or we are
            //    processing the last of the recipients
            //
            if ( posForSemiColon == KErrNotFound && posForArabicSemiColon == KErrNotFound )
                {
                posForSeparator = KErrNotFound;
                }

            // 2) found arabic semicolon in the remaining details buffer:
            //
            //    we will take everything upto where we find the arabic semicolon to the
            //    recipient buffer and we strip this recipient from our remaining details
            //    buffer for the next round of processing. (this is the multiple
            //    recipients case where each recipient is separated by arabic semicolon)
            //
            else if ( posForSemiColon == KErrNotFound )
                {
                posForSeparator = posForArabicSemiColon;
                separator.Copy( arabicSemiColon );
                }

            // 3) found semicolon in the remaining details buffer:
            //
            //    similar to the above. We will take everything upto where we find the
            //    semicolon to the recipient buffer and we strip this recipient from our
            //    remaining details buffer. (this is also the multiple recipients case
            //    where each recipient is separated by semicolon)
            //
            else if ( posForArabicSemiColon == KErrNotFound )
                {
                posForSeparator = posForSemiColon;
                separator.Copy( semiColon );
                }

            // The following 2 blocks are categorized under case 4).
            //
            // 4) found both semicolon and arabic semicolon: (This is the multiple
            //    recipients case where each recipient is separated by mix of semicolon
            //    and arabic semicolon).
            //    This is further broken down into 2 cases:
            //
            //    a) semicolon found is before arabic semicolon: treat it the same way
            //       as 3)
            //
            else if ( posForSemiColon < posForArabicSemiColon )
                {
                posForSeparator = posForSemiColon;
                separator.Copy( semiColon );
                }

            //    b) semicolon found is after arabic semicolon: treat it the same way
            //       as 2)
            //
            else
                {
                posForSeparator = posForArabicSemiColon;
                separator.Copy( arabicSemiColon );
                }

            //
            // take the first KPhCltNameBufferLength to the recipient buffer or take
            // everything upto where we find the separator
            //
            recipientPtr.Copy(
                remainingDetailsPtr.Left(
                    ( posForSeparator == KErrNotFound ) ? KPhCltNameBufferLength : posForSeparator ) );

            //
            // remove the recipient we're processing from the remaining details buffer.
            // remove the separator as well (this is accomplished by advancing the position
            // by 1)
            //
            remainingDetailsPtr.Copy(
                remainingDetailsPtr.Mid(
                    ( posForSeparator == KErrNotFound ) ? remainingDetailsPtr.Length() : posForSeparator + 1 ) );

            //
            // process the recipient buffer
            //
            if ( recipientPtr.Length() > 0 )
                {

                TBool validNumber = CommonPhoneParser::IsValidPhoneNumber(
                    recipientPtr, CommonPhoneParser::ESMSNumber );

                //
                // if the recipient is specified as a valid number, we will
                // perform number grouping and/or conversion as needed
                //
                if ( validNumber )
                    {
                    // perform number grouping as needed
//                    DoNumberGroupingL( recipientPtr );
                    // perform Arabic & Hebrew conversion as needed
                    AknTextUtils::LanguageSpecificNumberConversion( recipientPtr );
                    }

                //
                // for Arabic/Hebrew, if the recipient isn't the last in the list, we need to
                // put KMceRLM marker before the recipient name and McePDF marker before the
                // arabic semicolon, e.g.
                //
                // <KMceRLM><recipient name><KMcePDF><arabic semicolon>
                //
                if ( !validNumber &&
                     ( posForSeparator != KErrNotFound ) &&
                     ( separator.Compare( arabicSemiColon ) == 0 ) )
                   {
                   resultPtr.Append( KMceRLM );
                   }

                //
                // we've finished processing this recipient, append it to the result buffer
                //
                resultPtr.Append( recipientPtr );

                //
                // append separator if there are subsequent recipients
                //
                if ( posForSeparator != KErrNotFound )
                    {

                    //
                    // for Arabic/Hebrew, if the recipient isn't the last in the list, we need to
                    // put KMceRLM marker before the recipient name and McePDF marker before the
                    // arabic semicolon, e.g.
                    //
                    // <KMceRLM><recipient name><KMcePDF><arabic semicolon>
                    //
                    if ( !validNumber &&
                         ( separator.Compare( arabicSemiColon ) == 0 ) )
                        {
                        resultPtr.Append( KMcePDF );
                        }

                    resultPtr.Append( separator );
                    }

                }

            } while ( remainingDetailsPtr.Length() > 0 );

        //
        // we've processed the entire entry details, append our result to aBuffer
        //
        if ( len )
            {
            StripAndAppendString( aBuffer, resultPtr );
            }
        else
            {
            HBufC* text = StringLoader::LoadLC( R_MCE_PUSH_INBOX_SENDER_UNKW );
            aBuffer.Append( *text );
            CleanupStack::PopAndDestroy(); // text
            }
        CleanupStack::PopAndDestroy( 3 ); // recipientBuf, remainingDetailsBuf, resultBuf

        }

    //
    // The following processes the recipient(s) for languages which do not require number
    // conversion or number grouping for performance consideration
    //
    else
        {
        if ( aEntry.iDetails.Length() )
            {
            StripAndAppendString( aBuffer, aEntry.iDetails );
            }
        else
            {
            HBufC* text = StringLoader::LoadLC( R_MCE_PUSH_INBOX_SENDER_UNKW );
            aBuffer.Append( *text );
            CleanupStack::PopAndDestroy(); // text
            }
        }

    }


// ----------------------------------------------------
// CMceOneRowListItemArray::HandleFolderMessagesStringL
// ----------------------------------------------------
void CMceOneRowListItemArray::HandleFolderMessagesStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const
    {
    CMsvEntry* folderCEntry = iSession->GetEntryL( aEntry.Id() );
    CleanupStack::PushL( folderCEntry );

    // Folder strings for mail folder
    if ( aEntry.iMtm == KSenduiMtmImap4Uid &&
        aEntry.iType == KUidMsvFolderEntry )
        {
        CMsvEntrySelection* folSel = folderCEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
        CleanupStack::PushL( folSel );
        TInt folderCount = folSel->Count();
        TInt messagesCount = 0;
        if (folderCount >  0)
            {
            TInt cnt;
            for(cnt=0;cnt<folderCount;cnt++)
                {
                CMsvEntry* folderSubEntry = iSession->GetEntryL( folSel->At( cnt ) );
                CleanupStack::PushL( folderSubEntry );
                CMsvEntrySelection* mesSel = folderSubEntry->ChildrenWithTypeL( KUidMsvMessageEntry );
                CleanupStack::PushL( mesSel );        
                messagesCount+=mesSel->Count();              
                CleanupStack::PopAndDestroy( mesSel );
                CleanupStack::PopAndDestroy( folderSubEntry );
                }
            }
        else
            {
            CMsvEntrySelection* mesSel = folderCEntry->ChildrenWithTypeL( KUidMsvMessageEntry );
            CleanupStack::PushL( mesSel );
            messagesCount = mesSel->Count();
            CleanupStack::PopAndDestroy( mesSel );
            }
               
        CleanupStack::PopAndDestroy( folSel ); 
        TBuf<KMcePrintableTextLength> tempString;


        if ( messagesCount == 1 )
            {
            tempString.Append( (*iNumberOfMessagesStrings)[
                KMceListArrayDocAmountMailOneRow] );
            AknTextUtils::LanguageSpecificNumberConversion( tempString );
            }
        else
            {
            StringLoader::Format( tempString,
                (*iNumberOfMessagesStrings)[KMceListArrayDocAmountMailsOneRow],
                -1, // no index in the key string
                messagesCount );
            AknTextUtils::LanguageSpecificNumberConversion( tempString );
            }
            aBuffer.Append( tempString );

        }
    // Folder strings for user own folders
    else
        {
        const TInt messagesCount = folderCEntry->Count();
        if ( messagesCount == 1 )
            {
            TBuf<KMcePrintableTextLength> tempString;
            tempString.Append( (*iNumberOfMessagesStrings)[
                KMceListArrayDocAmountOneRow] );
            AknTextUtils::LanguageSpecificNumberConversion( tempString );
            aBuffer.Append( tempString );
            }
        else
            {
            TBuf<KMcePrintableTextLength> tempString;
            StringLoader::Format( tempString,
                (*iNumberOfMessagesStrings)[KMceListArrayDocAmountManyOneRow],
                -1, // no index in the key string
                messagesCount );
            AknTextUtils::LanguageSpecificNumberConversion( tempString );
            aBuffer.Append( tempString );
            }
        }
    CleanupStack::PopAndDestroy( folderCEntry );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::AppendBtIrMessageStringL
// ----------------------------------------------------
void CMceOneRowListItemArray::AppendBtIrMessageStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const
    {
    HBufC *fileName = AknTextUtils::ConvertFileNameL( aEntry.iDescription );
    StripAndAppendString( aBuffer, *fileName ); // does not leave
    delete fileName;
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::SetListItemArrayObserver
// ----------------------------------------------------
void CMceOneRowListItemArray::SetListItemArrayObserver( MMceOneRowListItemArrayObserver* aObserver )
    {
    iMceListItemArrayLoader->SetListItemArrayObserver( aObserver );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::GetEntry
// ----------------------------------------------------
TInt CMceOneRowListItemArray::GetEntry( TMsvId aEntryId, TMsvEntry& aEntry )
    {
    return iMceListItemArrayLoader->GetEntry( aEntryId, aEntry );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::GetEntry
// ----------------------------------------------------
TInt CMceOneRowListItemArray::GetEntry( TInt aIndex, TMsvEntry& aEntry )
    {
    return iMceListItemArrayLoader->GetEntry( aIndex, aEntry );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::IsItemFolder
// ----------------------------------------------------
TMsvId CMceOneRowListItemArray::GetParentFolderId() const
    {
    return iMceListItemArrayLoader->GetParentFolderId();
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::IsItemFolder
// ----------------------------------------------------
TBool CMceOneRowListItemArray::IsItemFolder( TMsvId aItemId ) const
    {
    return iMceListItemArrayLoader->IsItemFolder( aItemId );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::TreeItemId
// ----------------------------------------------------
TAknTreeItemID CMceOneRowListItemArray::TreeItemId( TInt aIndex )
    {
    return iMceListItemArrayLoader->TreeItemId( aIndex );
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::MsvItemId
// ----------------------------------------------------
TMsvId CMceOneRowListItemArray::MsvItemId( TAknTreeItemID aTreeItemId )
    {
    return iMceListItemArrayLoader->MsvItemId( aTreeItemId );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::ParentItemId
// ----------------------------------------------------
TAknTreeItemID CMceOneRowListItemArray::ParentItemId( TInt aIndex )
    {
    return iMceListItemArrayLoader->ParentItemId( aIndex );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::ParentItemIndex
// ----------------------------------------------------
TInt CMceOneRowListItemArray::ParentItemIndex( TInt aIndex )
    {
    return iMceListItemArrayLoader->ParentItemIndex( aIndex );
    }

// ----------------------------------------------------
// CMceOneRowListItemArray::SetTreeItemId
// ----------------------------------------------------
void CMceOneRowListItemArray::SetTreeItemId( TInt aIndex, TAknTreeItemID aTreeItemId )
    {
    iMceListItemArrayLoader->SetTreeItemId( aIndex, aTreeItemId );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::ResetAllTreeItemId
// ----------------------------------------------------
void CMceOneRowListItemArray::ResetAllTreeItemId()
    {
    iMceListItemArrayLoader->ResetAllTreeItemId();
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::ResetTreeItemId
// ----------------------------------------------------
void CMceOneRowListItemArray::ResetTreeItemId( TAknTreeItemID aTreeItemId )
    {
    iMceListItemArrayLoader->ResetTreeItemId( aTreeItemId );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::Compare
// ----------------------------------------------------
TInt CMceOneRowListItemArray::Compare( TAknTreeItemID aFirst, TAknTreeItemID aSecond )
    {
    return iMceListItemArrayLoader->Compare( aFirst, aSecond );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::FindFirstUnreadMessageL
// ----------------------------------------------------
TBool CMceOneRowListItemArray::FindFirstUnreadMessageL( TMsvId& aEntryId ) const
    {
    return iMceListItemArrayLoader->FindFirstUnreadMessageL( aEntryId );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::FindUnreadMessageUnderSubtitleL
// ----------------------------------------------------
TBool CMceOneRowListItemArray::FindUnreadMessageUnderSubtitleL( TAknTreeItemID aSubtitleID )
    {
    return iMceListItemArrayLoader->FindUnreadMessageUnderSubtitleL( aSubtitleID );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::FolderCount
// ----------------------------------------------------
TInt CMceOneRowListItemArray::FolderCount()
    {
    return iMceListItemArrayLoader->FolderCount();
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::MessageCount
// ----------------------------------------------------
TInt CMceOneRowListItemArray::MessageCount()
    {
    return iMceListItemArrayLoader->MessageCount();
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::HCItemType
// ----------------------------------------------------
TInt CMceOneRowListItemArray::HCItemType( TInt aIndex )
    {
    return iMceListItemArrayLoader->HCItemType( aIndex );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::SetRegistryObserver
// ----------------------------------------------------
void CMceOneRowListItemArray::SetRegistryObserver( MMtmUiDataRegistryObserver* aRegistryObserver )
    {
    iRegistryObserver = aRegistryObserver;
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::IsFolderRemote
// return ETrue, if remote folder
//
// ----------------------------------------------------
TBool CMceOneRowListItemArray::IsFolderRemote() const
    {
    return IsItemRemote( iFolderId );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::IsItemRemote
// ----------------------------------------------------
TBool CMceOneRowListItemArray::IsItemRemote( TMsvId aItemId ) const
    {
    TMsvId service = KMsvLocalServiceIndexEntryId;
    TMsvEntry entry;
    if ( iSession->GetEntry(aItemId,service,entry) == KErrNone )
        {
        return KMsvLocalServiceIndexEntryId != service;
        }
    else
        {
        return EFalse;
        }
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::DeleteDiscard
// ----------------------------------------------------
TBool CMceOneRowListItemArray::DeleteDiscard( TMsvId aItemId ) const
    {
    return iMceListItemArrayLoader->DeleteDiscard( aItemId );
    }


// ----------------------------------------------------
// CMceMessageListItemArray::SortType
// returns folder's sort type
//
// ----------------------------------------------------
TMsvSelectionOrdering CMceOneRowListItemArray::SortType()
    {
    return iMceListItemArrayLoader->SortType();
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::SetOrderingL
// ----------------------------------------------------
void CMceOneRowListItemArray::SetOrderingL( TInt aType, TBool aOrdering /*THCListListType aType*/ )
    {
    iMceListItemArrayLoader->SetOrderingL( aType, aOrdering );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::ItemIndexTreeId
// ----------------------------------------------------
TInt CMceOneRowListItemArray::ItemIndexTreeId( TAknTreeItemID aTreeItemId )
    {
    return iMceListItemArrayLoader->ItemIndexTreeId( aTreeItemId );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::SetItemMarkedL
// ----------------------------------------------------
void CMceOneRowListItemArray::SetItemMarked( TAknTreeItemID aFocused, TBool aState )
    {
    iMceListItemArrayLoader->SetItemMarked( aFocused, aState );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::GetItemMarkStatusL
// ----------------------------------------------------
TBool CMceOneRowListItemArray::GetItemMarkStatusL( TMsvId aEntryId )
    {
    return iMceListItemArrayLoader->GetItemMarkStatusL( aEntryId );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::GetSubfolderItemCount
// ----------------------------------------------------
TInt CMceOneRowListItemArray::GetSubfolderItemCount( TAknTreeItemID aParent )
    {
    return iMceListItemArrayLoader->GetSubfolderItemCount( aParent );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::IsConnected
// return ETrue, if account is connected
//
// ----------------------------------------------------
TBool CMceOneRowListItemArray::IsConnected() const
    {
    return iMceListItemArrayLoader->IsConnected();
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::FindEntryIndex
// ----------------------------------------------------
TInt CMceOneRowListItemArray::FindEntryIndex( const TDesC& aMatchString ) const
    {
    return iMceListItemArrayLoader->FindEntryIndex( aMatchString );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::EntryCreatedL
// ----------------------------------------------------
TInt CMceOneRowListItemArray::EntryCreatedL( TMsvId aEntryId )
    {
    return iMceListItemArrayLoader->EntryCreatedL( aEntryId );
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::EntryDeletedL
// ----------------------------------------------------
TBool CMceOneRowListItemArray::EntryDeletedL(
    TMsvId aEntryId,
    TAknTreeItemID& aTreeItemId,
    TAknTreeItemID& aParentTreeItemId )
    {
    return iMceListItemArrayLoader->EntryDeletedL( aEntryId, aTreeItemId, aParentTreeItemId );
    }

// ----------------------------------------------------
// CMceOneRowListItemArray::SaveMarkingL
// ----------------------------------------------------
void CMceOneRowListItemArray::SaveMarkingL( RArray<TAknTreeItemID>& aMarked )
    {
    iMceListItemArrayLoader->SaveMarkingL( aMarked );
    }

// ----------------------------------------------------
// CMceOneRowListItemArray::IsFolderTemplate
// ----------------------------------------------------
TBool CMceOneRowListItemArray::IsFolderTemplate( const TMsvEntry& child,TDes& aText,TInt aColumn )
    {
    TBool stringModified = EFalse;
    if( aColumn == 1 )
        {
        switch ( child.Id() )
            {
            case KMceTemplatesEntryId:
                {
                StringLoader::Load( aText, R_MCE_DOC_TEMP_FOLDER_1ROW, iOwnEikonEnv );
                stringModified = ETrue;
                }
                break;
            default:
                break;
            }
        }
    if( aColumn == 3 )
        {
        AppendFolderString( aText, child );
        stringModified = ETrue;
        }
    return stringModified;
    }

// ----------------------------------------------------
// CMceOneRowListItemArray::IsTodaysEntry
// ----------------------------------------------------
TBool CMceOneRowListItemArray::IsTodaysEntry(const TMsvEntry& aEntry) const
    {
    TTime entryTime;
    entryTime = aEntry.iDate;
    TTime dateNow;
    GetTime( entryTime, dateNow );
    
    TBool ifToday = EFalse ;
    
    if ( entryTime >= iMceListItemArrayLoader->TodayTime() )
        {
        ifToday = ETrue;
        }
    
    return ifToday ;
    }

//  End of File
