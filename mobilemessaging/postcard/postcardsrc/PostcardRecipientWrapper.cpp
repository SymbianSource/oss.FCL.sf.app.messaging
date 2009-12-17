/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       Helper class for recipient/phonebook handling
*
*/



// ========== INCLUDE FILES ================================

#include <badesca.h>
#include <e32std.h>

#include <aknlists.h> // PopupQuery stuff
#include <aknPopup.h> 
#include <aknpopuplayout.h>

#include <StringLoader.h>

#include <Postcard.rsg>
#include "PostcardAppUi.h"
#include "PostcardContact.h"
#include "PostcardDocument.h"
#include "PostcardRecipientWrapper.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CPostcardRecipientWrapper::CPostcardRecipientWrapper
// ---------------------------------------------------------
CPostcardRecipientWrapper::CPostcardRecipientWrapper( 
    CPostcardDocument& aDocument, 
    CPostcardAppUi& aAppUi,
    CPostcardContact& aContact
    ) :
    iDocument( aDocument ),
    iAppUi( aAppUi ),
    iContact( aContact )

    {    
    }

// ---------------------------------------------------------
// CPostcardRecipientWrapper::~CPostcardRecipientWrapper
// ---------------------------------------------------------
CPostcardRecipientWrapper::~CPostcardRecipientWrapper( )
    {
    }

// ---------------------------------------------------------
// CPostcardRecipientWrapper::ConstructL
// ---------------------------------------------------------
void CPostcardRecipientWrapper::ConstructL( )
    {
    }

// ---------------------------------------------------------
// CPostcardRecipientWrapper::NewL
// ---------------------------------------------------------
CPostcardRecipientWrapper* CPostcardRecipientWrapper::NewL( 
    CPostcardDocument& aDocument,
    CPostcardAppUi& aAppUi,
    CPostcardContact& aContact
    )
    {
    CPostcardRecipientWrapper* self = 
        new ( ELeave ) CPostcardRecipientWrapper(
            aDocument,
            aAppUi,
            aContact
            );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CPostcardRecipientWrapper::AskLocationL
// ---------------------------------------------------------
TBool CPostcardRecipientWrapper::AskLocationL(
    CPostcardContact::TLocation& aLocation )
    {
    static const CPostcardContact::TLocation locations[] =
        {
        CPostcardContact::ELocationPreferred,
        CPostcardContact::ELocationHome,
        CPostcardContact::ELocationWork
        };
    const TInt KNumLocations = sizeof( locations ) / sizeof( locations[ 0 ] );

    CPostcardContact::TLocation foundLocations[ KNumLocations ];
    TInt numLocations = 0;

    // Check if contact has address for possible locations
    for( TInt i = 0; i < KNumLocations; i++ )
        {
        if ( iContact.HasLocationL(locations[i]) )
            {
            foundLocations[ numLocations++ ] = locations[ i ];
            }
        }

    CPostcardContact::TLocation location =
        CPostcardContact::ELocationPreferred;

    if ( numLocations == 0 )
        {
        // Import only the name fields
        }
    else if ( numLocations == 1 )
        { // Only one location used -> use that
        location = foundLocations[ 0 ];
        }
    else
        {
        CAknSingleHeadingPopupMenuStyleListBox* listBox =
            new(ELeave) CAknSingleHeadingPopupMenuStyleListBox();
        CleanupStack::PushL( listBox );
        CAknPopupList* popupList = CAknPopupList::NewL( listBox,
            R_POSTCARD_SOFTKEYS_OK_BACK__OK,
            AknPopupLayouts::EMenuGraphicHeadingWindow );
        CleanupStack::PushL( popupList );
        listBox->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
        listBox->CreateScrollBarFrameL( ETrue );
        listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

        TInt resId = R_POSTCARD_ADDRESS_SELECTION_TITLE;
        HBufC* titleText = iAppUi.ReadFromResourceLC( resId );
        popupList->SetTitleL( *titleText );
        CleanupStack::PopAndDestroy( titleText ); // titleText

        CDesCArrayFlat* strings = new (ELeave) CDesCArrayFlat( KNumLocations );
        CleanupStack::PushL( strings );
        
        for( TInt i = 0; i < numLocations; i++ )
            {
            HBufC* buffer = GetFormattedStringL( foundLocations[ i ] );
            CleanupStack::PushL( buffer );
            strings->AppendL( *buffer );
            CleanupStack::PopAndDestroy( buffer );
            }

        // For each list item replace the %N in the item format with the number 
        // of the icon, the %0N with the label of the address (Default, Home or
        // Business) and the %1N with the actual combined address string.        
        
        CTextListBoxModel* model = listBox->Model();
        model->SetItemTextArray( strings );
        model->SetOwnershipType( ELbmOwnsItemArray );
        CleanupStack::Pop( strings );
                
        // And execute the dialog
        CleanupStack::Pop( popupList );
        TInt popupOk = popupList->ExecuteLD();
        TInt itemIndex = listBox->CurrentItemIndex();
        CleanupStack::PopAndDestroy( listBox ); // listBox;

        if ( popupOk )
	        {
	        location = foundLocations[ itemIndex ];
	        }
        else
            {
            // Nothing was selected so just return..
            return EFalse;
            }
        }
    // Set location as out parameter
    aLocation = location;
    return ETrue;
    }

// ---------------------------------------------------------
// CPostcardRecipientWrapper::GetFormattedStringL
// ---------------------------------------------------------
HBufC* CPostcardRecipientWrapper::GetFormattedStringL(
    CPostcardContact::TLocation aAddressLocation )
    {
    _LIT(KPostcardSelectItemFormat, "%0U\t%1U");
    TPtrC itemFormat(KPostcardSelectItemFormat);

    TBuf<64> addressBuffer;
    addressBuffer.Zero();

    static const TInt resIds[] =
        {
        R_POSTCARD_ADDRESS_SELECTION_NONE,
        R_POSTCARD_ADDRESS_SELECTION_HOME,
        R_POSTCARD_ADDRESS_SELECTION_WORK
        };
    TInt resId = resIds[ aAddressLocation ];
    HBufC* itemText = iAppUi.ReadFromResourceLC( resId );
    iContact.GetContactStringL( addressBuffer, aAddressLocation );
    
    TInt formatBufferLength = itemFormat.Length() + itemText->Length() + addressBuffer.Length();
            
    HBufC* sourceBuffer = HBufC::NewLC(formatBufferLength);
    HBufC* destinationBuffer = HBufC::NewLC(formatBufferLength);
    TPtr sourceBufferPtr = sourceBuffer->Des();
    TPtr destinationBufferPtr = destinationBuffer->Des();
            
    sourceBufferPtr.Copy(itemFormat);

    // List item label -> location 0, value itemText
    StringLoader::Format(destinationBufferPtr, sourceBufferPtr, 0, itemText->Des( ) );
    sourceBufferPtr.Copy(destinationBufferPtr);

    // List item text (address string) -> location 1, value addressBuffer
    StringLoader::Format(destinationBufferPtr, sourceBufferPtr, 1, addressBuffer);

    CleanupStack::Pop( destinationBuffer ); 
    CleanupStack::PopAndDestroy( 2, itemText ); // itemText, sourceBuffer
    return destinationBuffer;
    }
    
// EOF
