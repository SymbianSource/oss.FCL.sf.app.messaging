/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MuiuDynamicHeader.h
*
*/



#ifndef __MUIUDYNAMICHEADER_H__
#define __MUIUDYNAMICHEADER_H__

// INCLUDES
#include <e32base.h>

#include "MuiuDynamicSettingsArray.hrh"

// FORWARD DECLARATIONS
class CMuiuSettingBase;

// DATATYPES
typedef TBuf<KMuiuDynMaxSettingsLongTextLength> TMuiuSettingsText;
typedef TBuf<KMuiuDynMaxPasswordLength> TMuiuPasswordText;
typedef CArrayPtrFlat<CMuiuSettingBase> CMuiuDynSetItemArray;
_LIT( KMuiuDynStrSpace, " " );
_LIT( KMuiuBaseItemPanic, "MuiuDynItem" );
_LIT( KMuiuExtLinkItemPanic, "Dynamic Item: Extended link" );
_LIT( KMuiuEditValueItemPanic, "Dynamic Item: Edit Value" );
_LIT( KMuiuEditTextItemPanic, "Dynamic Item: Edit Text" );
// Muiu dynamic dialog panic text
_LIT( KMuiuDynSetDialogPanic, "MuiuDynDialog" );
// Muiu array panic
_LIT( KMuiuDynSettingsArrayPanic, "MuiuDynArray" );

// FUNCTION PROTOTYPES
// CLASS DECLARATION

#endif //__MUIUDYNAMICHEADER_H__

// End of file
