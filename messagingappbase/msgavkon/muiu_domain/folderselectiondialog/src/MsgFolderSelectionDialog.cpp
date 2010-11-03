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
*    Dialog for selecting message folder
*
*/



// INCLUDE FILES
#include <AknsUtils.h> // CreateIconL
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include <eiktxlbx.h>  // CEikTextListBox
#include <eiktxlbm.h>  // CTextListBoxModel
#include <avkon.hrh>
#include <msvuids.h>
#include <msvapi.h>
#include <bautils.h>    // BaflUtils
#include <gulicon.h>    // CGulIcon
#include <aknlists.h>
#include <eikclbd.h>
#include <AknQueryDialog.h>
#include <aknPopupHeadingPane.h>
#include <AknIconArray.h> // CAknIconArray
#include <muiu.rsg>       // r_muiu_folder_selection_list_query
#include <muiu.mbg>
#include <avkon.mbg>
#include <AknIconUtils.h> // AknIconUtils::AvkonIconFileName()
#include "MsgFolderSelectionDialog.h"
#include "MsgFolderSelectionListArray.h"



// CONSTANTS
_LIT(KDirAndResFileName,"muiu.rsc");
_LIT(KDirAndMuiuMbmFile,"muiu.mbm");
const TInt KMuiuArrayGranularity    = 4;



// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CMsgFolderSelectionDialog::CMsgFolderSelectionDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgFolderSelectionDialog::CMsgFolderSelectionDialog( TMsvId& aSelectedFolder, TDesC& aTitle )
:
iSelectedFolder( aSelectedFolder ),
iTitle( aTitle ),
iResources( *CCoeEnv::Static() )
    {
    }


// -----------------------------------------------------------------------------
// CMsgFolderSelectionDialog::ConstructL
// Symbian OS default constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgFolderSelectionDialog::ConstructL()
    {
    TParse parse;
    parse.Set(KDirAndMuiuMbmFile, &KDC_APP_BITMAP_DIR, NULL); 
    TFileName fileName(parse.FullName());

    iFolderArray = CMsgFolderSelectionListArray::NewL( iSelectedFolder );
    iIcons = new( ELeave ) CAknIconArray( KMuiuArrayGranularity );

    CFbsBitmap* bitmap;
    CFbsBitmap* bitmapMask;
    MAknsSkinInstance* skins = AknsUtils::SkinInstance();

    //EMbmMuiuQgn_prop_mce_inbox_small
    AknsUtils::CreateIconL( skins, KAknsIIDQgnPropMceInboxSmall, bitmap,
           bitmapMask, fileName, EMbmMuiuQgn_prop_mce_inbox_small,
           EMbmMuiuQgn_prop_mce_inbox_small + 1 );
    iIcons->AppendL(CGulIcon::NewL( bitmap, bitmapMask ));

    //EMbmMuiuQgn_prop_mce_doc_small
    AknsUtils::CreateIconL( skins, KAknsIIDQgnPropMceDocSmall, bitmap,
           bitmapMask, fileName, EMbmMuiuQgn_prop_mce_doc_small,
           EMbmMuiuQgn_prop_mce_doc_small + 1 );
    iIcons->AppendL(CGulIcon::NewL( bitmap, bitmapMask ));

    //EMbmAvkonQgn_prop_folder_small
    fileName = AknIconUtils::AvkonIconFileName();  //avkon.mbm file name and path
    AknsUtils::CreateIconL( skins, KAknsIIDQgnPropFolderSmall , bitmap,
           bitmapMask, fileName, EMbmAvkonQgn_prop_folder_small,
           EMbmAvkonQgn_prop_folder_small + 1 );
    iIcons->AppendL(CGulIcon::NewL( bitmap, bitmapMask ));

    parse.Set(KDirAndResFileName, &KDC_RESOURCE_FILES_DIR, NULL);
    fileName.Copy(parse.FullName());
    iResources.OpenL( fileName );
    }



// -----------------------------------------------------------------------------
// CMsgFolderSelectionDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgFolderSelectionDialog* CMsgFolderSelectionDialog::NewL( TMsvId& aSelectedFolder, 
                                                            TDesC& aTitle )
    { // static
    CMsgFolderSelectionDialog* self = new( ELeave ) CMsgFolderSelectionDialog( aSelectedFolder, 
                                                                               aTitle );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    return self;
    }


// -----------------------------------------------------------------------------
// CMsgFolderSelectionDialog::~CMsgFolderSelectionDialog
// Destructor.
// -----------------------------------------------------------------------------
//
CMsgFolderSelectionDialog::~CMsgFolderSelectionDialog()
    {
    delete iIcons;
    delete iFolderArray;
    iResources.Close();
    }


// -----------------------------------------------------------------------------
// CMsgFolderSelectionDialog::ExecutePopupListL
// 
// -----------------------------------------------------------------------------
//
TBool CMsgFolderSelectionDialog::ExecutePopupListL()
    {
    TInt index = 0;
    CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &index );
    dlg->PrepareLC( R_MUIU_FOLDER_SELECTION_LIST_QUERY );
    dlg->SetItemTextArray( iFolderArray );
    dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );
    dlg->SetIconArrayL( iIcons ); // takes ownership so must not be deleted
    iIcons = NULL;
    CAknPopupHeadingPane* heading = dlg->Heading();
    heading->SetTextL( iTitle );
    if( dlg->RunLD( ) )
        {
        iSelectedFolder = iFolderArray->ItemId( index );
        return ETrue;
        }
    return EFalse;

    }


// ---------------------------------------------------------
// CMsgFolderSelectionDialog::SelectFolderL
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgFolderSelectionDialog::SelectFolderL( TMsvId& aSelectedFolder, TDesC& aTitle )
    {
    CMsgFolderSelectionDialog* dlg = CMsgFolderSelectionDialog::NewL( aSelectedFolder, aTitle );
    CleanupStack::PushL( dlg );
    TBool button = dlg->ExecutePopupListL();
    CleanupStack::PopAndDestroy( dlg );
    return button;
    }

//  End of File

