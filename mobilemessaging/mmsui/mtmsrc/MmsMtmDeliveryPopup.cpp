/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MmsMtmDeliveryPopup implementation
*
*/




// INCLUDE FILES
#include <aknmessagequerydialog.h>
#include <MmsUi.rsg>
#include <eikenv.h>

#include <commonphoneparser.h>
#include "CMsgDeliveryItem.h"
#include "MmsMtmDeliveryPopup.h"
#include "MmsMtmUi.h"           //  ConvertUtcToLocalTime()


// CONSTANTS
_LIT( KLineWrap, "\n" );
_LIT( KOpen, "<");
_LIT( KClose, ">");
_LIT( KComma, ",");
_LIT (KSpace, " ");

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================



// -----------------------------------------------------------------------------
// CMsgInfoDeliveryStatusPopup::CMsgInfoDeliveryStatusPopup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgInfoDeliveryStatusPopup::CMsgInfoDeliveryStatusPopup()
    {
    }

// -----------------------------------------------------------------------------
// CMsgInfoDeliveryStatusPopup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgInfoDeliveryStatusPopup::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CMsgInfoDeliveryStatusPopup::~CMsgInfoDeliveryStatusPopup
// Destructor
// -----------------------------------------------------------------------------
// 
CMsgInfoDeliveryStatusPopup::~CMsgInfoDeliveryStatusPopup()
    {
    if (iDataArray)
        {
        iDataArray->ResetAndDestroy();  
        delete iDataArray;
        }
        		
    delete iFailedText;
    delete iPendingText;
    delete iDeliveredText;
    delete iReadText;
    delete iUnavailableText;
    delete iText;
    }

// -----------------------------------------------------------------------------
// CMsgInfoDeliveryStatusPopup::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgInfoDeliveryStatusPopup* CMsgInfoDeliveryStatusPopup::NewL(/* CFileManagerItemProperties& aProperties */)
    {
    CMsgInfoDeliveryStatusPopup* self = new( ELeave ) CMsgInfoDeliveryStatusPopup( /*aProperties */);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
//  CMsgInfoDeliveryStatusPopup::ExecuteL
//  Gathers the data and lauches the dialog
// -----------------------------------------------------------------------------
// 
TInt CMsgInfoDeliveryStatusPopup::ExecuteL()
    {
    HBufC* headerText = CEikonEnv::Static()->AllocReadResourceLC(R_MMS_DELIVERY_STATUS_HEADER);
    ParseTextL();
    
    CAknMessageQueryDialog* dialog = CAknMessageQueryDialog::NewL(*iText); 
    dialog->SetHeaderTextL(*headerText);

    TInt answer = dialog->ExecuteLD(R_PROPERTIES_MESSAGE_QUERY);
    CleanupStack::PopAndDestroy();

    return answer;
}

// -----------------------------------------------------------------------------
//CMsgInfoDeliveryStatusPopup::SetDataArray
// Sets data array containing CMsgDeliveryItem's
// -----------------------------------------------------------------------------
// 
void CMsgInfoDeliveryStatusPopup::SetDataArray(CArrayPtrFlat< CMsgDeliveryItem >* aArray)
    {
    iDataArray = aArray;
    }

// -----------------------------------------------------------------------------
//CMsgInfoDeliveryStatusPopup::ParseText
// Loops through the msg items in data array and adds their contents to the strings
// -----------------------------------------------------------------------------
//
void CMsgInfoDeliveryStatusPopup::ParseTextL()
    {
    TInt itemCount(iDataArray->Count());
    TInt deliveredCount(0);
    TInt readCount(0);
    TInt failedCount(0);
    TInt pendingCount(0);
    TInt unavailableCount(0);
    
    for (TInt i = 0; i < itemCount; i++)
        {
        CMsgDeliveryItem* item = iDataArray->At(i);
        TInt status = item->GetStatus();

        // TODO This section can be used to test read status on the emulator 
        // until read reports are supported in the real world
/*        if (    item
            &&      item->GetNumber( ) == _L("04544187897") 
                ||  item->GetNumber( ) == _L("+3584544187897" ) )
            {
            status = CMsgDeliveryItem::ERead ;
            }
        if (    item
            &&      item->GetNumber( ) == _L("04544187898") 
                ||  item->GetNumber( ) == _L("+3584544187898" ) )
            {
            status = CMsgDeliveryItem::ERead ;
            }
        if (    item
            &&      item->GetNumber( ) == _L("0408200523") 
                ||  item->GetNumber( ) == _L("+358408200523" ) )
            {
            status = CMsgDeliveryItem::EDelivered ;
            }
        if (    item
            &&      item->GetNumber( ) == _L("0408200524") 
                ||  item->GetNumber( ) == _L("+358408200524" ) )
            {
            status = CMsgDeliveryItem::EDelivered ;
            }
*/        
        switch (status)
            {
            case CMsgDeliveryItem::EPending:
                iPendingSize += ParseStringL(item, NULL);
                pendingCount++;
                break;
            case CMsgDeliveryItem::EFailed:
                iFailedSize += ParseStringL(item, NULL);
                failedCount++;
                break;
            case CMsgDeliveryItem::EDelivered:
                iDeliveredSize += ParseStringL(item, NULL);
                deliveredCount++;
                break;
            case CMsgDeliveryItem::ERead:
                iReadSize += ParseStringL(item, NULL);
                readCount++;
                break;
            default: // for ENone (unavailable)
                iUnavailableSize += ParseStringL(item, NULL);
                unavailableCount++;
                break;
            }
        }
        
    CalculateLabelSizesL(failedCount, pendingCount,  deliveredCount, readCount, unavailableCount);    
    iMessageSize += iPendingSize;
    iMessageSize += iFailedSize;
    iMessageSize += iDeliveredSize;
    iMessageSize += iReadSize;
    iMessageSize += iUnavailableSize;
        
    TDesC linewrapDes(KLineWrap);
    iMessageSize +=  2* linewrapDes.Length(); // because there's two linewraps at the bottom    
    
    iFailedText = HBufC::NewL(iFailedSize);
    iPendingText = HBufC::NewL(iPendingSize);
    iDeliveredText = HBufC::NewL(iDeliveredSize);
    iReadText = HBufC::NewL(iReadSize);
    iUnavailableText = HBufC::NewL(iUnavailableSize);
        
    iText = HBufC::NewL(iMessageSize);    
    pendingCount = 0;
    failedCount = 0;
    deliveredCount = 0;
    readCount = 0;
    unavailableCount = 0;

    for (TInt i = 0; i < itemCount; i++)
        {
        CMsgDeliveryItem* item = iDataArray->At(i);
        TInt status = item->GetStatus();

        // TODO This section can be used to test read status on the emulator 
        // until read reports are supported in the real world
/*        if (    item
            &&      item->GetNumber( ) == _L("04544187897") 
                ||  item->GetNumber( ) == _L("+3584544187897" ) )
            {
            status = CMsgDeliveryItem::ERead ;
            }
        if (    item
            &&      item->GetNumber( ) == _L("04544187898") 
                ||  item->GetNumber( ) == _L("+3584544187898" ) )
            {
            status = CMsgDeliveryItem::ERead ;
            }
        if (    item
            &&      item->GetNumber( ) == _L("0408200523") 
                ||  item->GetNumber( ) == _L("+358408200523" ) )
            {
            status = CMsgDeliveryItem::EDelivered ;
            }
        if (    item
            &&      item->GetNumber( ) == _L("0408200524") 
                ||  item->GetNumber( ) == _L("+358408200524" ) )
            {
            status = CMsgDeliveryItem::EDelivered ;
            }
*/        

        switch (status)
            {
            case CMsgDeliveryItem::EPending:
                {
                TPtr str = iPendingText->Des();
                ParseStringL(item, &str);
                pendingCount++;
                break;
                }
            case CMsgDeliveryItem::EFailed:
                {
                TPtr str = iFailedText->Des();
                ParseStringL(item, &str );
                failedCount++;
                break;
                }
            case CMsgDeliveryItem::EDelivered:
                {
                TPtr str = iDeliveredText->Des();
                ParseStringL(item, &str );
                deliveredCount++;
                break;
                }
            case CMsgDeliveryItem::ERead:
                {
                TPtr str = iReadText->Des();
                ParseStringL(item, &str );
                readCount++;
                break;
                }
            default: // for ENone (unavailable)
                {
                TPtr str = iUnavailableText->Des();
                ParseStringL(item, &str );
                unavailableCount++;
                break;
                }
            }
        }

    // add the 
    AddLabelsL(failedCount, pendingCount,  deliveredCount, readCount, unavailableCount);	

    // add the parsed strings to their places and
    // insert line breaks in between if necessary
    if ( failedCount > 0 )
        {
         iText->Des().Append(*iFailedText); 
        }
    TInt countsSoFar = failedCount;
    if ( pendingCount > 0 )
        {
        if ( countsSoFar > 0 ) // if theres text before this, add a line break
            {
            iText->Des().Append(KLineWrap);
            iText->Des().Append(KLineWrap);
            }
        iText->Des().Append(*iPendingText);
        } 
    countsSoFar += pendingCount;
    if ( deliveredCount > 0 )
        {
        if ( countsSoFar > 0 ) // if theres text before this, add a line break
            {
            iText->Des().Append(KLineWrap);
            iText->Des().Append(KLineWrap);
            }
        iText->Des().Append(*iDeliveredText);
        }
    countsSoFar += deliveredCount;
    if ( readCount > 0 )
        {
        if ( countsSoFar > 0 ) // if theres text before this, add a line break
            {
            iText->Des().Append(KLineWrap);
            iText->Des().Append(KLineWrap);
            }
        iText->Des().Append(*iReadText);
        }
    countsSoFar += readCount;
    if ( unavailableCount > 0 )
        {
        if ( countsSoFar > 0 ) // if theres text before this, add a line break
            {
            iText->Des().Append(KLineWrap);
            iText->Des().Append(KLineWrap);
            }
        iText->Des().Append(*iUnavailableText);
        }
    }
// -----------------------------------------------------------------------------
// CMsgInfoDeliveryStatusPopup::AddLabels
// Adds the labels ("failed", "pending", "delivered", "read", "unavailable") 
// in front of the each string.
// -----------------------------------------------------------------------------
//	
void CMsgInfoDeliveryStatusPopup::AddLabelsL(   TInt aFailed, 
                                                TInt aPending,  
                                                TInt aDelivered, 
                                                TInt aRead, 
                                                TInt aUnavailable )
    {
    TInt resourceId( 0 );
    if (aFailed > 0)
        {   
        if ( aFailed > 1 )
            {
            resourceId = R_MMS_DELIVERY_STATUS_FAILED_PLURAL;
            }
        else
            {
            resourceId = R_MMS_DELIVERY_STATUS_FAILED;
            }
        HBufC* failedText = CEikonEnv::Static()->AllocReadResourceLC( resourceId );
        iFailedText->Des().Insert( 0, *failedText );
        CleanupStack::PopAndDestroy();
        }

    if (aPending > 0)
        {
        if ( aPending > 1 )
            {
            resourceId = R_MMS_DELIVERY_STATUS_PENDING_PLURAL;
            }
        else
            {
            resourceId = R_MMS_DELIVERY_STATUS_PENDING;
            }
        HBufC* pendingText = CEikonEnv::Static()->AllocReadResourceLC( resourceId );
        iPendingText->Des().Insert( 0, *pendingText );
        CleanupStack::PopAndDestroy();
        }

    if (aDelivered > 0)
        {
        if ( aDelivered > 1 )
            {
            resourceId = R_MMS_DELIVERY_STATUS_DELIVERED_PLURAL;
            }
        else
            {
            resourceId = R_MMS_DELIVERY_STATUS_DELIVERED;
            }
        HBufC* deliveredText = CEikonEnv::Static()->AllocReadResourceLC( resourceId );
        iDeliveredText->Des().Insert( 0, *deliveredText );
        CleanupStack::PopAndDestroy();
        }

    if (aRead > 0)
        {
        if ( aRead > 1 )
            {
            resourceId = R_MMS_DELIVERY_STATUS_READ_PLURAL;
            }
        else
            {
            resourceId = R_MMS_DELIVERY_STATUS_READ;
            }
        HBufC* readText = CEikonEnv::Static()->AllocReadResourceLC( resourceId );
        iReadText->Des().Insert( 0, *readText );
        CleanupStack::PopAndDestroy();
        }

    if (aUnavailable > 0)
        {
        if ( aUnavailable > 1 )
            {
            resourceId = R_MMS_DELIVERY_STATUS_UNAVAILABLE_PLURAL;
            }
        else
            {
            resourceId = R_MMS_DELIVERY_STATUS_UNAVAILABLE;
            }
        HBufC* unavailableText = CEikonEnv::Static()->AllocReadResourceLC( resourceId );
        iUnavailableText->Des().Insert( 0, *unavailableText );
        CleanupStack::PopAndDestroy();
        }
    }

// -----------------------------------------------------------------------------
// CMsgInfoDeliveryStatusPopup::CalculateLabelSizesL
// Calculate the labels' lengths and add them to string sizes for allocation
// -----------------------------------------------------------------------------
//
void CMsgInfoDeliveryStatusPopup::CalculateLabelSizesL(  TInt aFailed, 
                                                        TInt aPending,  
                                                        TInt aDelivered,
                                                        TInt aRead,
                                                        TInt aUnavailable )
    {
    TInt resourceId( 0 );
    if ( aFailed > 0 )
        {
        if ( aFailed > 1 )
            {
            resourceId = R_MMS_DELIVERY_STATUS_FAILED_PLURAL;
            }
        else
            {
            resourceId = R_MMS_DELIVERY_STATUS_FAILED;
            }
        HBufC* failedText = CEikonEnv::Static()->AllocReadResourceLC( resourceId );
        iFailedSize += failedText->Length();
        CleanupStack::PopAndDestroy();
        }

    if ( aPending > 0 )
        {
        if ( aPending > 1 )
            {
            resourceId = R_MMS_DELIVERY_STATUS_PENDING_PLURAL;
            }
        else
            {
            resourceId = R_MMS_DELIVERY_STATUS_PENDING;
            }
        HBufC* pendingText = CEikonEnv::Static()->AllocReadResourceLC( resourceId );
        iPendingSize += pendingText->Length();
        CleanupStack::PopAndDestroy();
        }

    if ( aDelivered > 0 )
        {
        if ( aDelivered > 1 )
            {
            resourceId = R_MMS_DELIVERY_STATUS_DELIVERED_PLURAL;
            }
        else
            {
            resourceId = R_MMS_DELIVERY_STATUS_DELIVERED;
            }
        HBufC* deliveredText = CEikonEnv::Static()->AllocReadResourceLC( resourceId );
        iDeliveredSize += deliveredText->Length();
        CleanupStack::PopAndDestroy();
        }

    if ( aRead > 0 )
        {
        if ( aRead > 1 )
            {
            resourceId = R_MMS_DELIVERY_STATUS_READ_PLURAL;
            }
        else
            {
            resourceId = R_MMS_DELIVERY_STATUS_READ;
            }
        HBufC* readText = CEikonEnv::Static()->AllocReadResourceLC( resourceId );
        iReadSize += readText->Length();
        CleanupStack::PopAndDestroy();
        }

    if ( aUnavailable > 0 )
        {
        if ( aUnavailable > 1 )
            {
            resourceId = R_MMS_DELIVERY_STATUS_UNAVAILABLE_PLURAL;
            }
        else
            {
            resourceId = R_MMS_DELIVERY_STATUS_UNAVAILABLE;
            }
        HBufC* unavailableText = CEikonEnv::Static()->AllocReadResourceLC( resourceId );
        iUnavailableSize += unavailableText->Length();
        CleanupStack::PopAndDestroy();
        }
    }
 
 // -----------------------------------------------------------------------------
// CMsgInfoDeliveryStatusPopup::ParseStringL
// Parse data from delivery item into the strings and return the length of
// the current items string. Give aDest as NULL and it only counts the length.
// -----------------------------------------------------------------------------
//   
TInt CMsgInfoDeliveryStatusPopup::ParseStringL(CMsgDeliveryItem* aItem, TPtr* aDest)
    {
	// Check for NULL. If NULL, return zero length
    if (!aItem)
    {
        return 0;
    }

    TInt size(0);

    TDesC commaDes(KComma);
    TDesC spaceDes(KSpace);
    TDesC openDes(KOpen);
    TDesC closeDes(KClose);
    TDesC linewrapDes(KLineWrap);
                
    // 1) linefeed
    if (aDest) // just calculating the length, aDest is NULL so DONT use it
        {
        aDest->Append(KLineWrap); 
        }
    size += linewrapDes.Length();
    
    // 2) time & date    
    if ( aItem->GetStatus() != CMsgDeliveryItem::ENone )
        {    
        TTime date = aItem->GetTime();  // message time
        CMmsMtmUi::ConvertUtcToLocalTime( date );
        TDateTime messageTime = date.DateTime();
    	
        if (MessageSentToday(messageTime)) // event happened today, use the time string
            {
            HBufC* timeFormat =
            CEikonEnv::Static()->AllocReadResourceLC(R_QTN_TIME_USUAL_WITH_ZERO);//from avkon resources
            TBuf<32> timeString;
            date.FormatL(timeString, *timeFormat);
            CleanupStack::PopAndDestroy();//timeFormat
            if (aDest) // just calculating the length, aDest is NULL so DONT use it
                {
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion( timeString );
                aDest->Append(timeString);    
                }
            size += timeString.Length();    
            }

        else // event was not today, so use the date string
            {
            HBufC* dateFormat =
            CEikonEnv::Static()->AllocReadResourceLC(R_QTN_DATE_USUAL_WITH_ZERO);//from avkonresources
            TBuf<32> dateString;
            date.FormatL(dateString, *dateFormat);
            CleanupStack::PopAndDestroy();//dateFormat
            if (aDest) // just calculating the length, aDest is NULL so DONT use it
                {
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion( dateString );
                aDest->Append(dateString);
                }
            size +=dateString.Length();      
            }
        if ( aDest ) // just calculating the length, aDest is NULL so DONT use it
            {
            aDest->Append(KComma);
            aDest->Append(KSpace);
            }
        size += commaDes.Length();
        size += spaceDes.Length();
        }
            
    // 3: ", <recipient alias>"
    TDesC &recip = aItem->GetRecipient();
    if ( aDest ) // just calculating the length, aDest is NULL so DONT use it
        {
        if ( CommonPhoneParser::IsValidPhoneNumber( recip, CommonPhoneParser::ESMSNumber ) )
            {
            HBufC* tempBuf = recip.AllocLC();
            TPtr number = tempBuf->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( number );
            aDest->Append(number);
            CleanupStack::PopAndDestroy( tempBuf );    
            }
        else
            {
            aDest->Append(recip);
            }
        }
    size += recip.Length(); 
        
    TDesC &temp = aItem->GetNumber();
    if ( temp.Length() > 0 )
        {
        HBufC* tempBuf = temp.AllocLC();
        TPtr number = tempBuf->Des();

        // 4: phone number. Value may be same as recipient for messages sent to
        // is not alias found ( set in the mms engine )
        if (    number.Length() > 0 
             && number.CompareF( recip ) )
            {
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( number );
            if (aDest) // just calculating the length, aDest is NULL so DONT use it
                {  
                aDest->Append(KLineWrap);
                aDest->Append(KOpen);
                aDest->Append(number); // FIXME , internationalisointi
                aDest->Append(KClose);
                }
                
            size += linewrapDes.Length(); 
            size += openDes.Length();
            size += number.Length();  
            size += closeDes.Length();     
            }
        CleanupStack::PopAndDestroy( tempBuf );    
        }
    // 5: No copy. Causes some extra characters to allocate, because
    // line feeds are appended only after paragraph change
    size += linewrapDes.Length(); 
        
    return size;    
    }

 // -----------------------------------------------------------------------------
// CMsgInfoDeliveryStatusPopup::MessageSentToday
// Check whether the msg is sent today.
// -----------------------------------------------------------------------------
//
TBool CMsgInfoDeliveryStatusPopup::MessageSentToday(const TDateTime& messageTime)
    {
    TBool sentToday(EFalse); // return value 
    // current date and time
    TTime time;
    time.UniversalTime();  // current time
    CMmsMtmUi::ConvertUtcToLocalTime( time );
    TDateTime currentTime = time.DateTime();


    TInt currentYear(currentTime.Year());
    TInt currentMonth(currentTime.Month());
    TInt currentDay(currentTime.Day());


    TInt messageYear(messageTime.Year());
    TInt messageMonth(messageTime.Month());
    TInt messageDay(messageTime.Day());

    if (messageYear == currentYear) // the year is current, check month
        {
        if (messageMonth == currentMonth) // the month is current, check day
            {
            if (messageDay == currentDay) // the year, month and the day are the same as current, so message was sent today
                {
                sentToday = ETrue;
                }
            }
        }

    return sentToday;
    }
// End of file
