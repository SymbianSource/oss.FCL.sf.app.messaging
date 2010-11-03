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
* Description:  Character set handler
*
*/



// INCLUDE FILES
#include    "Msgmailcharactersethandler.h"
#include    <featmgr.h>
#include    <bldvariant.hrh>
#include    <charconv.h>
#include    <eikmenup.h>
#include    <coemain.h>

#include    <StringLoader.h>
#include    <MsgMailViewer.rsg>
#include    <msgmailviewer.hrh>

// CONSTANTS
enum TVariationFlag 
    {
    EDefault,
    EJapaneseVariant,
    EChineseVariant
    };
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgMailCharacterSetHandler::CMsgMailCharacterSetHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgMailCharacterSetHandler::CMsgMailCharacterSetHandler()
    {
    }

// -----------------------------------------------------------------------------
// CMsgMailCharacterSetHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgMailCharacterSetHandler::ConstructL()
    {
    if( FeatureManager::FeatureSupported( KFeatureIdJapanese ) )
        {
        iVariationFlag = EJapaneseVariant;
        }
    else if( FeatureManager::FeatureSupported( KFeatureIdChinese ) )
        {
        iVariationFlag = EChineseVariant;
        }
    else
        {
        iVariationFlag = EDefault;
        }

    iFirstFreeCommandId = 0;
    AppendItemsToItemArrayL();
    }

// -----------------------------------------------------------------------------
// CMsgMailCharacterSetHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgMailCharacterSetHandler* CMsgMailCharacterSetHandler::NewL( 
    TInt aFirstFreeCommandId )
    {
    CMsgMailCharacterSetHandler* self = new( 
        ELeave ) CMsgMailCharacterSetHandler;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    self->iFirstFreeCommandId = aFirstFreeCommandId;
    CleanupStack::Pop(); // self
    return self;
    }

    
// Destructor
CMsgMailCharacterSetHandler::~CMsgMailCharacterSetHandler()
    {
    iItems.Close();
    }


// -----------------------------------------------------------------------------
// CMsgMailCharacterSetHandler::SetCharacterSetSubMenuItemsL
// Set filtered list of available characterset to menupane
// -----------------------------------------------------------------------------
//
void CMsgMailCharacterSetHandler::SetCharacterSetSubMenuItemsL(
    CEikMenuPane& aMenuPane, TUint aActiveCharacterSet)
    {
    for (TInt i=0; i<iItems.Count(); i++)
        {
        // Hide active characterset from options
        if (iItems[i].iCharacterSetId != aActiveCharacterSet)
            {
            CEikMenuPaneItem::SData data;
            
            data.iCommandId = iFirstFreeCommandId + iItems[i].iCommandId;
            data.iCascadeId = NULL;
            data.iFlags = NULL;         
            data.iText = iItems[i].iText;           
            data.iExtraText = KNullDesC;           
            
            aMenuPane.AddMenuItemL(data);            
            }
        }  
    
    }
// -----------------------------------------------------------------------------
// CMsgMailCharacterSetHandler::GetCharacterSetId
// Get character set id
// -----------------------------------------------------------------------------
//
TBool CMsgMailCharacterSetHandler::GetCharacterSetId(
    TInt aCommandId, TUint& aCharacterSetId)
    {
    TBool retVal(EFalse);
    aCharacterSetId = 0;
    for (TInt i=0; i<iItems.Count(); i++)
        {
        if (aCommandId == iItems[i].iCommandId + iFirstFreeCommandId)
            {
            aCharacterSetId = iItems[i].iCharacterSetId;
            retVal = ETrue;
            }
        }
    return retVal;
    }
// -----------------------------------------------------------------------------
// CMsgMailCharacterSetHandler::CharacterSetCount
// -----------------------------------------------------------------------------
//
void CMsgMailCharacterSetHandler::SetCharacterSetMenuItemL(
    CEikMenuPane& aMenuPane)
    {
    if ( iItems.Count() )
        {        
        if ( iVariationFlag == EJapaneseVariant )
            {
            CEikMenuPaneItem::SData& charecterSetItem = 
                aMenuPane.ItemData(EMsgMailViewerCmdCharSet);
            
            HBufC* text = StringLoader::LoadLC(R_QTN_JPN_MAIL_OM_CHARSET);
            charecterSetItem.iText =  
                (*text).Left(charecterSetItem.ENominalTextLength);
            CleanupStack::PopAndDestroy(); // text
            }
        aMenuPane.SetItemDimmed(EMsgMailViewerCmdCharSet, EFalse);
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailCharacterSetHandler::IsAcceptedCharacterSet
// -----------------------------------------------------------------------------
//
TBool CMsgMailCharacterSetHandler::IsAcceptedCharacterSet(
    TUint aCharacterSetId)
    {    
    switch(aCharacterSetId)
        {
        // For China variant
        case KCharacterSetIdentifierBig5:   // FALLTHROUGH
        case KCharacterSetIdentifierGb2312:
            return (iVariationFlag == EChineseVariant);
        // For Japan variant
        case KCharacterSetIdentifierShiftJis:   // FALLTHROUGH
        case KCharacterSetIdentifierEucJpPacked:// FALLTHROUGH
        case KCharacterSetIdentifierIso2022Jp:
            return (iVariationFlag == EJapaneseVariant);

        default:
            return EFalse;
        }
    
    }

// -----------------------------------------------------------------------------
// CMsgMailCharacterSetHandler::AppendItemsToItemArrayL
// -----------------------------------------------------------------------------
//
void CMsgMailCharacterSetHandler::AppendItemsToItemArrayL()
    {
    RFs& fs = CCoeEnv::Static()->FsSession();

    CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* charSets = 
        CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC( fs );
    
    for (TInt i=0; i<charSets->Count(); i++)
        {
        const TUint identifier = (*charSets)[i].Identifier();
        if ( IsAcceptedCharacterSet( identifier ) )
            {
            TData data;            
            data.iCommandId = i;
            data.iCharacterSetId = identifier;
            
            HBufC* buf = NULL;
            // China
            if ( identifier == KCharacterSetIdentifierGb2312 )
                {
                buf = StringLoader::LoadL(R_QTN_CHI_MAIL_SM_GB2312);
                }
            else if ( identifier == KCharacterSetIdentifierBig5 )
                {
                buf = StringLoader::LoadL(R_QTN_CHI_MAIL_SM_BIG5);
                }
            // Japanice
            else if ( identifier == KCharacterSetIdentifierIso2022Jp )
                {
                buf = StringLoader::LoadL(R_QTN_JPN_MAIL_SM_CHARSET_ISO2022JP);
                }
            else if ( identifier == KCharacterSetIdentifierEucJpPacked )
                {
                buf = StringLoader::LoadL(R_QTN_JPN_MAIL_SM_CHARSET_EUCJP);
                }
            else if ( identifier == KCharacterSetIdentifierShiftJis )
                {
                buf = StringLoader::LoadL(R_QTN_JPN_MAIL_SM_CHARSET_SHIFTJIS);
                }
            else
                {
                // Ask name from charconv
                CCnvCharacterSetConverter* charConv = 
                    CCnvCharacterSetConverter::NewLC();
                HBufC8* name = charConv->
                    ConvertCharacterSetIdentifierToStandardNameL(
                    identifier, fs);
                if ( name )
                    {
                    CleanupStack::PushL( name );
                    // cleanupstack is not needed here since
                    // the code cannot leave before deleting this buffer
                    buf = HBufC::NewL( name->Length() ); // CSI: 35 # see comment above
                    buf->Des().Copy( *name );
                    CleanupStack::PopAndDestroy(); // name
                    }
                CleanupStack::PopAndDestroy(); // charConv
                }
            
            if (!buf)
                {
                buf = KNullDesC().AllocL();
                }
            data.iText.Copy(buf->Left(data.ENominalTextLength));
            delete buf;
            buf = NULL;
            
            User::LeaveIfError( iItems.Append(data) );
            }
        }
    CleanupStack::PopAndDestroy(); // charSets
    }

//  End of File  
