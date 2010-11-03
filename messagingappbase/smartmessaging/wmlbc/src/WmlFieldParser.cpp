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
*     This class is used to parse fields to wml control from array, 
*     which is created by Wap Parser (wapp.dll).
*
*/



// INCLUDE FILES

#include "WmlFieldParser.h"             // CWmlFieldParser
#include "WmlBMSubItem30.h"             // for CWmlBMSubItem
#include "WmlBioControl.pan"

#include <wappdef.h>                    // Labels for use in array of ParsedFields
#include <bsp.h>                        // for CParsedField
#include <featmgr.h>
#include <bldvariant.hrh>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor
// ---------------------------------------------------------
//
CWmlFieldParser::CWmlFieldParser() : iCurrentCharacteristicType(ENotSpecifyed)
    {
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CWmlFieldParser::~CWmlFieldParser()
    {
    delete iBm;
    }

// ---------------------------------------------------------
// CWmlFieldParser::ParseFieldsL()
// Function goes through parsedfieldarray and creates subitems, 
// important thing is that field array has name value pairs and 
// meaning of these depends on which is the current character tag.
// ---------------------------------------------------------
//
void CWmlFieldParser::ParseFieldsL(
    const CArrayPtrSeg<CParsedField>& aParsedFieldArray,   
    CArrayPtrSeg<CWmlSubItemBase>& aBMArray )
    {
    iBMArray = &aBMArray;

    const TInt fieldCount = aParsedFieldArray.Count();
    
    TPtrC fieldName;
    TPtrC fieldValue;
    TInt fieldEnum;

    for ( TInt i = 0; i < fieldCount; i++ )
        {
        fieldName.Set( aParsedFieldArray.At(i)->FieldName() );  
        fieldValue.Set( aParsedFieldArray.At(i)->FieldValue() );
		RDebug::Print(_L("FieldParser Name: %S Value: %S"), &fieldName, &fieldValue);

        // Checks if the field indicates a new characteristic type.
        // Sets the iCurrentCharacteristicType attribute!
        if( !IsCharacteristicTypeL( fieldName, fieldValue, fieldEnum ) )
            {
            
            // Have to check what is the current characteric type, 
            // because fields in same name need to be added 
            // differently in different types.  
            switch ( iCurrentCharacteristicType )
                {				
                case EWappCharacteristicBookmark:
                    {
                    // iBm is always created when arrived here
                    iBm->AddFieldL( fieldValue, fieldEnum);
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            }
        }

    AddCurrentBMToArrayL();

	// If bookmarks are not supported clear the array
	if ( !FeatureManager::FeatureSupported( KFeatureIdSmartmsgSMSBookmarkReceiving ) )
		{
		iBMArray->ResetAndDestroy();
		}
    }

// ---------------------------------------------------------
// CWmlFieldParser :: CompareField
// This is used to map a fieldname to program enumerations.
// Done only because it is much faster to compare enums later
// ---------------------------------------------------------
//
TWappParserFields CWmlFieldParser::CompareField( TPtrC aFieldName ) 
    {
    if ( !aFieldName.Compare( KWappCharAddress ) )  
        return EWappCharacteristicAddress;
    if ( !aFieldName.Compare( KWappCharBookmark ) ) 
        return EWappCharacteristicBookmark;
    if ( !aFieldName.Compare( KWappCharName ) )     
        return EWappCharacteristicName;
    if ( !aFieldName.Compare( KWappCharURL ) )      
        return EWappCharacteristicURL;
    if ( !aFieldName.Compare( KWappCharMMSURL ) )      
        return EWappCharacteristicMMSURL;
    if ( !aFieldName.Compare( KWappCharID ) )     
        return EWappCharacteristicID; 
    if ( !aFieldName.Compare( KWappISP ) )         
        return EWappISP;
    if ( !aFieldName.Compare( KWappName ) )        
        return EWappName;
    if ( !aFieldName.Compare( KWappBearer ) )      
        return EWappBearer;
    if ( !aFieldName.Compare( KWappSMSCAddress ) ) 
        return EWappSMSCAddress;
    if ( !aFieldName.Compare( KWappProxy ) )       
        return EWappProxy;
    if ( !aFieldName.Compare( KWappPort ) )        
        return EWappPort;
    if ( !aFieldName.Compare( KWappProxyType ) )       
        return EWappProxyType;
    if ( !aFieldName.Compare( KWappURL ) )         
        return EWappURL;
    if ( !aFieldName.Compare( KWappProxyAuthName ) )   
        return EWappProxyAuthName;
    if ( !aFieldName.Compare( KWappProxyAuthSecret ) ) 
        return EWappProxyAuthSecret;
    if ( !aFieldName.Compare( KWappProxyLoginType ) ) 
        return EWappProxyLoginType;
    if ( !aFieldName.Compare( KWappUSSDCode ) )        
        return EWappUSSDCode;
    if ( !aFieldName.Compare( KWappAccessPointName ) ) 
        return EWappGPRSAccessPointName;
    if ( !aFieldName.Compare( KWappCsdDial ) )       
        return EWappCsdDial;
    if ( !aFieldName.Compare( KWappCsdCallType ) )   
        return EWappCsdCallType;
    if ( !aFieldName.Compare( KWappCsdCallSpeed ) )  
        return EWappCsdCallSpeed;
    if ( !aFieldName.Compare( KWappPPPAuthType ) )   
        return EWappPPPAuthType;
    if ( !aFieldName.Compare( KWappPPPAuthName ) )   
        return EWappPPPAuthName;
    if ( !aFieldName.Compare( KWappPPPLoginType ) )  
        return EWappPPPLoginType;
    if ( !aFieldName.Compare( KWappPPPAuthSecret ) ) 
        return EWappPPPAuthSecret;

    return ENotSpecifyed;
    }

// ---------------------------------------------------------
// CWmlFieldParser :: IsCharacteristicTypeL
// ---------------------------------------------------------
//
TBool CWmlFieldParser::IsCharacteristicTypeL(
    TPtrC aFieldName, 
    TPtrC /*aFieldValue*/, 
    TInt& aEnumField )
    {
    aEnumField = CompareField( aFieldName );
	TBool retValue(EFalse);
	
    switch( aEnumField )
        {
        case EWappCharacteristicBookmark:
            {            
            AddCurrentBMToArrayL();            
            iBm = CWmlBMSubItem::NewL();
            iCurrentCharacteristicType = EWappCharacteristicBookmark;
            retValue = ETrue;
            break;
            }
        default:
            {
            retValue = EFalse;
            break;
            }
        }   
	return retValue;        
    }

// ---------------------------------------------------------
// CWmlFieldParser :: AddCurrentBMToArrayL
// ---------------------------------------------------------
//
void CWmlFieldParser::AddCurrentBMToArrayL()
    {
    if( iBm && iBm->IsValidL() )
        { 
        iBMArray->AppendL( iBm ); 
        iBm = NULL;
        }
    else // item is not valid and it must be deleted.
        {
        delete iBm;
        iBm = NULL;
        }
    }

//  End of File  
