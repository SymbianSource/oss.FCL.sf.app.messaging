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
* Description:  
*     Encapsulates the data which populates the fields in the message 
*     info dialogs (email, sms, bios, mms), also holds the text for the 
*     labels and fields in the message info dialog and displays a message 
*     info dialog for particular message types
*
*/



// INCLUDE FILES
#include <AknPhoneNumberGrouping.h>      // CAknPhoneNumberGrouping
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include <aknmessagequerydialog.h>       // CaknMessageQueryDialog
#include <eikenv.h>                      // CEikonEnv
#include <muiu.rsg>
#include "Muiumsginfo.h"
#include "Muiumsginfo.hrh"


// CONSTANTS
const TInt KExtraSpaceForLabelString = 2;  // Length of KLabelExtraString(":" + line feed)
const TInt KExtraSpaceForLineFeed = 1;     // Length of line feed
const TInt KNumberOfLineFeeds = 2;         // How many line feeds one item need
const TInt KNumOfReplaceChars = 3; 	// for subject field whitebox removal
const TInt KLineFeed = 0x0000000a; 	// unicode representation for linefeed.
const TInt KDateSize = 30;          // Size of Date and Time strings in Message info
 
_LIT( KDirAndMuiuResFileName,"muiu.rsc" );
_LIT( KNullString, "" );
_LIT( KLabelExtraString, ":\n" );
_LIT( Ks1, "<" );
_LIT( Ks2, ">" );


// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::~CMsgInfoMessageInfoDialog
// ---------------------------------------------------------
//
CMsgInfoMessageInfoDialog::~CMsgInfoMessageInfoDialog()
    {
    iResources.Close();
    }


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::CMsgInfoMessageInfoDialog
// ---------------------------------------------------------
//
CMsgInfoMessageInfoDialog::CMsgInfoMessageInfoDialog() : iResources( *CEikonEnv::Static() )
    {}


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::ConstructL
// ---------------------------------------------------------
//
void CMsgInfoMessageInfoDialog::ConstructL()
    {
    TParse parse;
    parse.Set( KDirAndMuiuResFileName, &KDC_RESOURCE_FILES_DIR, NULL ); 
    TFileName fileName( parse.FullName() );
    iResources.OpenL( fileName );
    }


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::NewL
// ---------------------------------------------------------
//
EXPORT_C CMsgInfoMessageInfoDialog* CMsgInfoMessageInfoDialog::NewL()
    {
    CMsgInfoMessageInfoDialog* self = new( ELeave ) CMsgInfoMessageInfoDialog;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self
    return self;
    }


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::ExecuteLD
// Show the message info dialog.
// ---------------------------------------------------------
//
EXPORT_C void CMsgInfoMessageInfoDialog::ExecuteLD( TMsgInfoMessageInfoData& aFieldData, 
                                                    TMessageInfoType aMessageType)
    {
    TInt popCount = 0;
    TMsgInfoMessageInfoData convertedFieldData;
    CleanupStack::PushL( this );
    iMessageType = aMessageType;


    // Note: Lanquage specific number conversion done here.
    // This way dialog seem to open faster. (Number conversion and memory allocation take time)

    //From ----------------------------------------------------------------------
    HBufC* tmpFromString = GroupPhoneNumberAndDoLangSpecificNumberConversionL( aFieldData.iFrom );
    CleanupStack::PushL( tmpFromString );
    popCount++;
    SetStringToPointerDescriptor( tmpFromString, convertedFieldData.iFrom );

    //Subject ----------------------------------------------------------------------
    HBufC* tmpSubjectString = aFieldData.iSubject.AllocLC();
    popCount++;
    TPtr tmpSubjecPtr = tmpSubjectString->Des();
    TBuf<KNumOfReplaceChars> replaceChars;
    replaceChars.Zero();
    replaceChars.Append( CEditableText::EParagraphDelimiter );
    replaceChars.Append( CEditableText::ETabCharacter );
    replaceChars.Append( KLineFeed );
    AknTextUtils::ReplaceCharacters(
        tmpSubjecPtr, 
        replaceChars, 
        CEditableText::ESpace );
    if ( tmpSubjecPtr.Length() > 0 )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tmpSubjecPtr );
        }
    convertedFieldData.iSubject.Set( tmpSubjecPtr );

    //Type ----------------------------------------------------------------------
    HBufC* tmpTypeString = aFieldData.iType.AllocLC();
    popCount++;
    DoNumberConversion( tmpTypeString, convertedFieldData.iType );

    //Size ----------------------------------------------------------------------
    HBufC* tmpSizeString = aFieldData.iSize.AllocLC();
    popCount++;
    DoNumberConversion( tmpSizeString, convertedFieldData.iSize );

    //Priority ------------------------------------------------------------------
    HBufC* tmpPriorityString = aFieldData.iPriority.AllocLC();
    popCount++;
    DoNumberConversion( tmpPriorityString, convertedFieldData.iPriority );

    //To----------------------------------------------------------------------
    HBufC* tmpToString = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( aFieldData.iTo );
    TPtr tmpToPtr = tmpToString->Des();
    AknTextUtils::ReplaceCharacters( tmpToPtr, replaceChars, CEditableText::ESpace );
    CleanupStack::PushL( tmpToString );
    popCount++;
    DoNumberConversion( tmpToString, convertedFieldData.iTo );

    //CC----------------------------------------------------------------------
    HBufC* tmpCCString = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( aFieldData.iCC );
    TPtr tmpCCPtr = tmpCCString->Des();
    AknTextUtils::ReplaceCharacters( tmpCCPtr, replaceChars, CEditableText::ESpace );
    CleanupStack::PushL( tmpCCString );
    popCount++;
    DoNumberConversion( tmpCCString, convertedFieldData.iCC );

    //BCC----------------------------------------------------------------------
    HBufC* tmpBCCString = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( aFieldData.iBCC );
    CleanupStack::PushL( tmpBCCString );
    popCount++;
    DoNumberConversion( tmpBCCString, convertedFieldData.iBCC );


    convertedFieldData.iDateTime = aFieldData.iDateTime;
    SetInfoArrayL( convertedFieldData );
    CleanupStack::PopAndDestroy( popCount, tmpFromString ); //tmpFromString, tmpSubjectString
    // tmpTypeString, tmpSizeString, tmpPriorityString, tmpToString, tmpCCString, tmpBCCString
    CleanupStack::Pop( this ); //this
    delete this;
    }


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::SetInfoArrayL
// 
// ---------------------------------------------------------
//
void CMsgInfoMessageInfoDialog::SetInfoArrayL( TMsgInfoMessageInfoData& aFieldData ) const
    {
    
    TInt neededHBufSpace(0);
    TInt cleanupStackIndex(0);

    CEikonEnv* env = CEikonEnv::Static();
    if ( !env )
        {
        User::Leave( KErrGeneral );
        }
    
    //Construct label strings and calculate how much space whole message info string need
    //From -----------------------------------------------------------
    HBufC* labelTextFrom;
    labelTextFrom = NULL;
    TBool fromAdded( EFalse );
    if( aFieldData.iFrom.Length() )
        {
        labelTextFrom = ConstructLabelStrAndCalculateNeededSpaceLC( 
                        R_MESSAGE_INFO_FROM_LABEL, 
                        aFieldData.iFrom, neededHBufSpace, env );
        fromAdded = ETrue;
        cleanupStackIndex++;
        }    

    //Subject --------------------------------------------------
    HBufC* labelTextSubject;
    labelTextSubject = NULL;
    TBool subjectAdded( EFalse );   
    if( aFieldData.iSubject.Length() )
        {
        labelTextSubject = ConstructLabelStrAndCalculateNeededSpaceLC( 
            R_MESSAGE_INFO_SUBJECT_LABEL, 
            aFieldData.iSubject, neededHBufSpace, env );
        subjectAdded = ETrue;
        cleanupStackIndex++;
        }
    
    
    //Date and Time -------------------------------------------------------------
    TBuf<KDateSize> dateString;
    TBuf<KDateSize> timeString;
    TPtrC dateStr;
    TPtrC timeStr;
    GetFormattedTimeAndDateStringsL( aFieldData.iDateTime, dateString, timeString, env );
    HBufC* tmpDateString = dateString.AllocLC();
    cleanupStackIndex++;
    DoNumberConversion( tmpDateString, dateStr );
    HBufC* tmpTimeString = timeString.AllocLC();
    cleanupStackIndex++;
    DoNumberConversion( tmpTimeString, timeStr );
    
    //Date ------------------------------------------------------------------
    HBufC* labelTextDate;
    labelTextDate = NULL;
    TBool dateAdded( EFalse );
    if( dateStr.Length() )
        {
        labelTextDate = ConstructLabelStrAndCalculateNeededSpaceLC( R_MESSAGE_INFO_DATE_LABEL, 
            dateStr, neededHBufSpace, env );
        dateAdded = ETrue;
        cleanupStackIndex++;
        }
    
    //Time ------------------------------------------------------------------
    HBufC* labelTextTime;
    labelTextTime = NULL;
    TBool timeAdded( EFalse );
    if( timeStr.Length() )
        {
        labelTextTime = ConstructLabelStrAndCalculateNeededSpaceLC( R_MESSAGE_INFO_TIME_LABEL, 
            timeStr, neededHBufSpace, env );
        timeAdded = ETrue;
        cleanupStackIndex++;
        }
    
    //Type ----------------------------------------------------------------------
    HBufC* labelTextType;
    labelTextType = NULL;
    TBool typeAdded( EFalse );
    if( aFieldData.iType.Length() )
        {
        labelTextType = ConstructLabelStrAndCalculateNeededSpaceLC( R_MESSAGE_INFO_TYPE_LABEL, 
            aFieldData.iType, neededHBufSpace, env );
        typeAdded = ETrue;
        cleanupStackIndex++;
        }
    
    
    //Size ----------------------------------------------------------------------
    HBufC* labelTextSize;
    labelTextSize = NULL;
    TBool sizeAdded( EFalse );
    if( aFieldData.iSize.Length() )
        {
        labelTextSize = ConstructLabelStrAndCalculateNeededSpaceLC( 
                    R_MESSAGE_INFO_SIZE_LABEL, 
                    aFieldData.iSize, neededHBufSpace, env );
        sizeAdded = ETrue;
        cleanupStackIndex++;
        }
    
    //Priority ------------------------------------------------------------------
    HBufC* labelTextPriority;
    labelTextPriority = NULL;
    TBool priorityAdded( EFalse );
    if( aFieldData.iPriority.Length() )
        {
        labelTextPriority = ConstructLabelStrAndCalculateNeededSpaceLC( 
                    R_MMS_INFO_PRIORITY_LABEL, 
                    aFieldData.iPriority, neededHBufSpace, env );
        priorityAdded = ETrue;
        cleanupStackIndex++;
        }

    
    //To ----------------------------------------------------------------------
    HBufC* labelTextTo;
    labelTextTo = NULL;
    TBool toAdded( EFalse );
    if( aFieldData.iTo.Length() )
        {
        labelTextTo = ConstructLabelStrAndCalculateNeededSpaceLC( 
                    R_MESSAGE_INFO_TO_LABEL, 
                    aFieldData.iTo, neededHBufSpace, env );
        toAdded = ETrue;
        cleanupStackIndex++;
        }
        
    
    //Cc ----------------------------------------------------------------------
    HBufC* labelTextCc;
    labelTextCc = NULL;
    TBool ccAdded( EFalse );
    if( aFieldData.iCC.Length() )
        {
        labelTextCc = ConstructLabelStrAndCalculateNeededSpaceLC( 
                    R_MESSAGE_INFO_CC_LABEL, 
                    aFieldData.iCC, neededHBufSpace, env );
        ccAdded = ETrue;
        cleanupStackIndex++;
        }

    
    //Bcc ----------------------------------------------------------------------
    HBufC* labelTextBcc;
    labelTextBcc = NULL;
    TBool bccAdded( EFalse );
    if( aFieldData.iBCC.Length() )
        {
        labelTextBcc = ConstructLabelStrAndCalculateNeededSpaceLC( 
                    R_MESSAGE_INFO_BCC_LABEL, 
                    aFieldData.iBCC, neededHBufSpace, env );
        bccAdded = ETrue;
        cleanupStackIndex++;
        }
    
    
    // Add all message info strings to queryDialogBuf and 
    // show queryDialogBuf in CAknMessageQueryDialog
    HBufC* queryDialogBuf;
    queryDialogBuf = NULL;
    
    queryDialogBuf = HBufC::NewLC( neededHBufSpace );
    cleanupStackIndex++;
    TBool firstItem(ETrue);
    //From -----------------------------------------------------------
    if( fromAdded && labelTextFrom )
        {
        AddToqueryDialogBuf( queryDialogBuf, *labelTextFrom, aFieldData.iFrom, firstItem );
        firstItem = EFalse;
        }
    //Subject --------------------------------------------------------
    if( subjectAdded && labelTextSubject )
        {
        AddToqueryDialogBuf( queryDialogBuf, *labelTextSubject, aFieldData.iSubject, firstItem );
        firstItem = EFalse;
        }
    
    //Date -----------------------------------------------------------
    if( dateAdded && labelTextDate )
        {
        AddToqueryDialogBuf( queryDialogBuf, *labelTextDate, dateStr, firstItem );
        firstItem = EFalse;
        }
    
    //Time -----------------------------------------------------------
    if( timeAdded && labelTextTime )
        {
        AddToqueryDialogBuf( queryDialogBuf, *labelTextTime, timeStr, firstItem  );
        firstItem = EFalse;
        }
    
    //Type -----------------------------------------------------------
    if( typeAdded && labelTextType )
        {
        AddToqueryDialogBuf( queryDialogBuf, *labelTextType, aFieldData.iType, firstItem );
        firstItem = EFalse;
        }
    
    //Size -----------------------------------------------------------
    if( sizeAdded && labelTextSize )
        {
        AddToqueryDialogBuf( queryDialogBuf, *labelTextSize, aFieldData.iSize, firstItem );
        firstItem = EFalse;
        }
    
    
    //Priority -------------------------------------------------------
    if( priorityAdded && labelTextPriority )
        {
        AddToqueryDialogBuf( queryDialogBuf, 
                             *labelTextPriority, aFieldData.iPriority, firstItem );
        firstItem = EFalse;
        }
    
    
    //To -------------------------------------------------------------
    if( toAdded && labelTextTo )
        {
        AddToqueryDialogBuf( queryDialogBuf, *labelTextTo, aFieldData.iTo, firstItem );
        firstItem = EFalse;
        }
    
    //Cc -------------------------------------------------------------
    if( ccAdded && labelTextCc )
        {
        AddToqueryDialogBuf( queryDialogBuf, *labelTextCc, aFieldData.iCC, firstItem );
        firstItem = EFalse;
        }
    
    //Bcc ------------------------------------------------------------
    if( bccAdded && labelTextBcc )
        {
        AddToqueryDialogBuf( queryDialogBuf, *labelTextBcc, aFieldData.iBCC, firstItem );
        firstItem = EFalse;
        }
    
    CAknMessageQueryDialog* dialog = CAknMessageQueryDialog::NewL( *queryDialogBuf );
    dialog->PrepareLC( R_MUIU_MESSAGE_QUERY );
    dialog->RunLD();
    CleanupStack::PopAndDestroy( cleanupStackIndex );
    }


// ---------------------------------------------------------
// Convert between arabic-indic digits and european digits based on existing language setting.
// So it'll convert any digit from the string
// to use either european digits or arabic-indic digits based on current settings.
// @param aFieldString: Data buffer used in conversion.
// @param aFieldData: Return converted data in this parameter.
// ---------------------------------------------------------
//
void CMsgInfoMessageInfoDialog::DoNumberConversion( HBufC* aFieldString, 
                                                    TPtrC& aFieldData ) const
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
// Creates formatted date and time strings and return these strings in parameters
// aDateBuf and aTimeBuf. Function get UTC time in parameter aUTCtime and
// constructs date and time strings with this UTC time value.
// @param aUTCtime: UTC time used in time conversion.
// @param aDateBuf: Return date string after time conversion.
// @param aTimeBuf: Return time string after time conversion. 
//
// ---------------------------------------------------------
//
void CMsgInfoMessageInfoDialog::GetFormattedTimeAndDateStringsL( TTime& aUTCtime, 
                                                                TDes& aDateBuf, 
                                                                TDes& aTimeBuf, 
                                                                const CEikonEnv* aEikonEnv ) const
    {
    TLocale locale;
    
    // Get Universal time offset
    TTimeIntervalSeconds universalTimeOffset( locale.UniversalTimeOffset() );
    
    // Add locale's universal time offset to universal time to get the local time
    aUTCtime+=universalTimeOffset;
    
    // If home daylight saving in effect, add one hour offset. 
    if ( locale.QueryHomeHasDaylightSavingOn() )
        {
        TTimeIntervalHours daylightSaving( 1 );
        aUTCtime+=daylightSaving;
        }
    
    if( aEikonEnv )
        {
        //Set date and time
        HBufC* dateFormat = aEikonEnv->AllocReadResourceLC( R_QTN_DATE_USUAL_WITH_ZERO );
        aUTCtime.FormatL( aDateBuf, *dateFormat );
        CleanupStack::PopAndDestroy( dateFormat );//dateFormat
        
        HBufC* timeFormat = aEikonEnv->AllocReadResourceLC( R_QTN_TIME_USUAL_WITH_ZERO );
        aUTCtime.FormatL( aTimeBuf, *timeFormat );
        CleanupStack::PopAndDestroy( timeFormat );//timeFormat
        }
    else
        {
        // Does nothing
        }
    }


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::ConstructLabelStrAndCalculateNeededSpaceLC
//
//
// ---------------------------------------------------------
//
HBufC* CMsgInfoMessageInfoDialog::ConstructLabelStrAndCalculateNeededSpaceLC( 
                                                              TInt aLabelResourceId,
                                                              const TPtrC& aFieldText,
                                                              TInt& aNeededSpace, 
                                                              const CEikonEnv* aEikonEnv ) const
    {
    if( aEikonEnv )
        {
        TInt spaceFieldNeed;
        HBufC* labelTextStr = aEikonEnv->AllocReadResourceLC( aLabelResourceId );
        spaceFieldNeed = labelTextStr->Des().Length() + KExtraSpaceForLabelString;
        spaceFieldNeed = spaceFieldNeed + 
            aFieldText.Length() + KNumberOfLineFeeds * KExtraSpaceForLineFeed;
        aNeededSpace = aNeededSpace + spaceFieldNeed;
        return labelTextStr;
        }
    else
        {
        return NULL;
        }
    }



// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::AddToqueryDialogBuf
//
//
// ---------------------------------------------------------
//
void CMsgInfoMessageInfoDialog::AddToqueryDialogBuf( HBufC* aQueryDialogBuf,
                                                     const TDesC& aLabelText,
                                                     const TDesC& aFieldText,
                                                     TBool aFirstItem ) const
    {
    if( aQueryDialogBuf )
        {
        if ( aFirstItem )
            {
            aQueryDialogBuf->Des().Append( aLabelText );
            aQueryDialogBuf->Des().Append( KLabelExtraString );
            aQueryDialogBuf->Des().Append( aFieldText );	
            }
        else
            {
            aQueryDialogBuf->Des().Append( KLineFeed );
            aQueryDialogBuf->Des().Append( KLineFeed );
            aQueryDialogBuf->Des().Append( aLabelText );
            aQueryDialogBuf->Des().Append( KLabelExtraString );
            aQueryDialogBuf->Des().Append( aFieldText );	
            }
        }
    }


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::GroupPhoneNumberAndDoLangSpecificNumberConversion
//
//
// ---------------------------------------------------------
// 
HBufC* CMsgInfoMessageInfoDialog::GroupPhoneNumberAndDoLangSpecificNumberConversionL( const TDesC& aFieldString )
    {
    HBufC* newString(NULL);
    if(aFieldString.Length())
        {	
        TPtrC16 name;
        TPtrC16 rightPartOfName;
        TPtrC16 number;
        TInt currentLength;
        TInt point1;
        TInt point2;
        TBool isEnd( EFalse );
		
        point1 = aFieldString.Find( Ks1 );
		
        if( point1 > KErrNotFound )
            {
            //There is phone number part so there is "<" )
            //For example string "Aa44 Bb55 <050 5899560>"
            currentLength = point1;
            name.Set( aFieldString.Left( currentLength ) );
            rightPartOfName.Set( aFieldString.Mid( currentLength ) );
            newString = HBufC::NewL( currentLength );
            TPtr newStringPtr = newString->Des();
            newStringPtr.Copy( name );
            }
        else
            {
            // There is no "<" char so there is only name part or 
            // phone number without "<" and ">" chars
            // For example "0505899560"
            HBufC* groupedNumberStr = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( aFieldString );
            TPtr groupedNumberPtr = groupedNumberStr->Des();
            currentLength = groupedNumberPtr.Length();
            newString = HBufC::NewL( currentLength );
            TPtr ptr = newString->Des();
            ptr.Copy( groupedNumberPtr );
            delete groupedNumberStr;
            groupedNumberStr = NULL;
            }
		
		
        while ( ( !isEnd ) && ( point1 > KErrNotFound ) )
            {
            point2 = rightPartOfName.Find( Ks2 );
            if( point2 > KErrNotFound )
                {
                number.Set( rightPartOfName.Left( point2 ) ); 
                rightPartOfName.Set( rightPartOfName.Mid( point2 ) );
                if ( number.Length() > 0 )
                    {
                    CleanupStack::PushL( newString );
                    HBufC* numberStr = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( number );
                    CleanupStack::Pop( newString );
                    TPtr numberPtr = numberStr->Des();
                    currentLength = currentLength + numberPtr.Length();
                    newString = newString->ReAlloc( currentLength );
                    TPtr ptr = newString->Des();
                    ptr.Append( numberPtr );
                    delete numberStr;
                    numberStr = NULL;
                    }
                else
                    {
                    currentLength = currentLength + rightPartOfName.Length();
                    newString = newString->ReAlloc( currentLength );
                    TPtr ptr = newString->Des();
                    ptr.Append( rightPartOfName );
                    }
                }
            else
                {
                isEnd = ETrue;
                currentLength = currentLength + rightPartOfName.Length();
                newString = newString->ReAlloc( currentLength );
                TPtr ptr = newString->Des();
                ptr.Append( rightPartOfName );
                }
			
            if( !isEnd )
			          {
                point1 = rightPartOfName.Find( Ks1 );
                if( point1 > KErrNotFound )
                    {
                    name.Set( rightPartOfName.Left( point1 ) );
                    rightPartOfName.Set( rightPartOfName.Mid( point1 ) );
                    currentLength = currentLength + point1;
                    newString = newString->ReAlloc( currentLength );
                    TPtr ptr = newString->Des();
                    ptr.Append( name );	
                    }
                else
                    {
                    currentLength = currentLength + rightPartOfName.Length();
                    newString = newString->ReAlloc( currentLength );
                    TPtr ptr = newString->Des();
                    ptr.Append( rightPartOfName );
                    }
			          }
            }; //While
        return newString;
        }
    else
        {
        return newString;	
        }
    }
    
    
    
// ---------------------------------------------------------
// Set HBufC string to pointer descriptor.
// 
// 
// ---------------------------------------------------------
//
void CMsgInfoMessageInfoDialog::SetStringToPointerDescriptor( HBufC* aFieldString, 
                                                        TPtrC& aFieldData ) const
    {
    if( aFieldString )
        {
    	  TPtr tmpPtr = aFieldString->Des();
        aFieldData.Set( tmpPtr );
        }
    else
        {
        aFieldData.Set( KNullString );
        }
    }


// End of file
