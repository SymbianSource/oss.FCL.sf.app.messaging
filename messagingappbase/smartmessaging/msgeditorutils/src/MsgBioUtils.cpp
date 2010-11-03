/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class offers helper methods for BIO controls
*
*/



// INCLUDE FILES

#include <AknQueryDialog.h>         // CAknTextQueryDialog
#include <aknnotewrappers.h>
#include <StringLoader.h>       // StringLoader

#include "MsgBioUtils.h"            // Own header
#include <uri16.h>                // TUriParser8
#include <MsgEditorUtils.rsg>   // resouce identifiers
#include "MsgEditorUtils.pan"   // for MsgEditorUtils panics

// IP min length comes from 0.0.0.0 --> 7
const TInt KIPMinLength = 7;
// IP max lenght comes from 255.255.255.255 --> 15
const TInt KIPMaxLength = 15;
const TInt KMinDelimitedValue = 0;
const TInt KMaxDelimitedValue = 255;
const TInt KDigitMaxLength = 3;

#ifdef _DEBUG
_LIT(KMsgBioUtils,"MsgBioUtils");
#endif

// ================= MEMBER FUNCTIONS =======================

// leaves if user cancels query
EXPORT_C void MsgBioUtils::TextQueryL(TInt aQueryResource, TDes& aText)
    {
    CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog(aText,CAknQueryDialog::ENoTone);
    if (dlg->ExecuteLD(aQueryResource)) //R_DATA_QUERY
        {
        // query approved
        return;
        }
    // query was cancelled
    User::Leave(KLeaveWithoutAlert);
    }

EXPORT_C void MsgBioUtils::ErrorNoteL(const TDesC& aText)
    {
    CAknErrorNote* note = new(ELeave)CAknErrorNote( ETrue );
    note->ExecuteLD(aText);
    }

EXPORT_C void MsgBioUtils::ErrorNoteL(TInt aStringResource)
    {
    HBufC* buf = StringLoader::LoadLC(aStringResource);
    ErrorNoteL(*buf);
    CleanupStack::PopAndDestroy(buf);
    }

EXPORT_C void MsgBioUtils::ConfirmationNoteL(const TDesC& aText)
    {
    CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote;
    note->ExecuteLD(aText);
    }

EXPORT_C void MsgBioUtils::ConfirmationNoteL(TInt aStringResource)
    {
    HBufC* buf = StringLoader::LoadLC(aStringResource);
    ConfirmationNoteL(*buf);
    CleanupStack::PopAndDestroy(buf);
    }

EXPORT_C void MsgBioUtils::InformationNoteL(const TDesC& aText)
    {
    CAknInformationNote* note = new (ELeave) CAknInformationNote;
    note->ExecuteLD(aText);
    }

EXPORT_C void MsgBioUtils::InformationNoteL(TInt aStringResource)
    {
    HBufC* buf = StringLoader::LoadLC(aStringResource);
    InformationNoteL(*buf);
    CleanupStack::PopAndDestroy(buf);
    }

  
EXPORT_C void MsgBioUtils::IncrementNameL(TDes& aName, TInt aMaxLength)
    {
    TInt length = aName.Length();

    // If this fails it means that at least default name should have been assigned
    // to item.
    __ASSERT_ALWAYS(length > 0, Panic(EMEUNameEmpty));
    TInt newOrdinal = 0;
    TInt index = length-1;
    TInt charValue = aName[index];

    // Check if name ends with ')'
    if (charValue == ')')
        {
        TBool cutBetweenIndexes = EFalse;
        index--;
        TInt multi = 1;
        while ( index >= 0)
            {
            charValue = aName[index];
            TChar ch(charValue);

            if (!ch.IsDigit())
                {
                // There was non-digits inside the brackets.
                if ( charValue == '(')
                    {
                    // We found the start
                    cutBetweenIndexes = ETrue;
                    index--;
                    break;
                    }
                else
                    {
                    // There was a non-digit inside the '(' and ')'
                    // We just append after the original name.
                    break;
                    }
                } // if
            else
                {
                TInt newNum = ch.GetNumericValue();
                if (multi <= 100000000)
                    {
                    // If there's a pattern (007) then this makes it to be (007)(01)
                    if (newNum == 0 && newOrdinal != 0 && multi > 10)
                        {
                        break;
                        }
                    newOrdinal += newNum * multi;
                    multi*=10;
                    }//0010
                else
                    {
                    newOrdinal = 0;
                    break;
                    }
                }
            index--;
            } // while

        // There was correct pattern of (0000) so now we delete it.
        if (cutBetweenIndexes)
            {
            aName.Delete(index+1, length-index);
            }
        else
            {
            // This case is for example 12345) so we get 12345)(01)
            newOrdinal = 0;
            }
        } // if

    // Add one to the ordinal read from the old name
    newOrdinal++;

    // Check that there's enough space to add the ordinal
    TInt maxLength = aMaxLength;
    TInt ordinalSpace(4); // Ordinal space comes from (00) so it's 4
    length = aName.Length();
    if (length + ordinalSpace > maxLength)
        {
        aName.Delete(maxLength-ordinalSpace, ordinalSpace);
        }

    // Append the ordinal at the end of the name
    HBufC* old = aName.AllocLC();

    _LIT(KFormatSpecLessTen, "%S(0%d)");
    _LIT(KFormatSpecOverTen, "%S(%d)");
    if (newOrdinal < 10)
        {
        aName.Format(KFormatSpecLessTen, old, newOrdinal);
        }
    else
        {
        aName.Format(KFormatSpecOverTen, old, newOrdinal);
        }
    CleanupStack::PopAndDestroy(old); // old
    }

EXPORT_C TBool MsgBioUtils::CheckIP( const TDesC& aIP )
    {
    TInt length( aIP.Length() );

    // Check that ip length is correct.
    if ( !(length >= KIPMinLength && length <= KIPMaxLength) )
        {
        return EFalse;
        }

	TChar token('.');
    const TInt KNumberOfTokenizedValues = 4;

    TInt startIndex( -1 ); // startIndex is advanced in loop to point to next char.
    for ( TInt i = 0; i < KNumberOfTokenizedValues; i++ )
        {
        TInt value(0);

        // Advance the startIndex to point to the next character.
        startIndex++;
        if ( startIndex < length )
            {
            TBool hasNumValue = GetTokenizedValue( aIP, token, startIndex, value );
            if ( !hasNumValue )
                {
                return EFalse;
                }
            // Set the value to the correct variable
            switch ( i )
                {
                case 0:
                    {
                    
                    break;
                    }
                case 1:
                    {
                    
                    break;
                    }
                case 2:
                    {
                    
                    break;
                    }
                case 3:
                    {
                    
                    break;
                    }
                default:
                    {
                    // Check that your KNumberOfTokenizedValues is correct
                    // and add cases to this switch clause.
                    User::Invariant(); 
                    break;
                    }
                }
            }
        else
            {
            // too short IP string
            return EFalse;
            }
        }

    // At this point we've extracted four Tokenized values from the
    // ip string. 
    if ( startIndex < length )
        {
        // too long IP string
        return EFalse;
        }

    /// do any consistency checks here.
    return ETrue;
    }

EXPORT_C TBool MsgBioUtils::CheckURL( const TDesC& aUrl )
    {
    TUriParser16 uriParser;
    TInt err = uriParser.Parse( aUrl );
    if ( err == KErrNone )
        {
        return ETrue;
        }
    else
        {
        return EFalse;        
        }
    }

EXPORT_C TBool MsgBioUtils::ConfirmationQueryOkCancelL(const TDesC& aText)
    {
    //This temporary descriptor is needed because the query wants a non const
    HBufC* text = aText.AllocL();
    CleanupStack::PushL(text);
    CAknQueryDialog* qDlg = new (ELeave) CAknQueryDialog(*text);
    TInt response =
        qDlg->ExecuteLD(R_MSGEDITORUTILS_GENERAL_CONFIRMATION_QUERY_OK_CANCEL);
    CleanupStack::PopAndDestroy( text );
    if (response)
        {
        return ETrue;
        }
    return EFalse;
    }

EXPORT_C TBool MsgBioUtils::ConfirmationQueryOkCancelL(TInt aStringResource)
    {
    HBufC* buf = StringLoader::LoadLC(aStringResource);
    TBool result = ConfirmationQueryOkCancelL(*buf);
    CleanupStack::PopAndDestroy(buf);
    return result;
    }

TBool MsgBioUtils::GetTokenizedValue( const TDesC& aIP, 
    const TChar atoken,
    TInt& aStartIndex,
    TInt& aValue )
    {
    TInt length(aIP.Length());
    __ASSERT_DEBUG( aStartIndex >= 0, User::Panic(KMsgBioUtils, KErrUnderflow ) );
    __ASSERT_DEBUG( aStartIndex < length, User::Panic(KMsgBioUtils, KErrOverflow ) );

    aValue = 0;
    TBool hasValue(EFalse);
    TInt rounds(0);
    while ( aStartIndex < length )
        {
        rounds++;
        if ( rounds > KDigitMaxLength + 1  ) // +1 comes from the dot (255. -> count is 4 )
            {
            return EFalse;
            }
        TChar character = aIP[aStartIndex];
        if ( character.IsDigit() )
            {
            hasValue = ETrue;
            aValue*=10;
            TInt lastDigit = character.GetNumericValue();
            aValue+=lastDigit;

            // Do the limit check for value.
            if ( aValue < KMinDelimitedValue || aValue > KMaxDelimitedValue )
                {
                return EFalse;
                }
            }
        else
            {
            // Check if it's dot
            if ( character == atoken )
                {
                return hasValue;
                }
            else // not valid
                {
                return EFalse;
                }
            }
        aStartIndex++;
        }
    return hasValue; // Check that we had something.
    }

// End of File
