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



#ifndef CWMLFIELDPARSER_H
#define CWMLFIELDPARSER_H

// INCLUDES

#include "WMLBC.hrh"
#include <e32base.h>

// FORWARD DECLARATIONS

class CParsedField;
class CWmlSubItemBase;
class CWmlBMSubItem;
enum TWappParserFields;

// CLASS DECLARATION

/**
* This class is used to parse fields to wml control from array, 
* which is created by Wap Parser (wapp.dll).
*/
NONSHARABLE_CLASS( CWmlFieldParser ) : public CBase
    {

    public:  // construction
        
        /**
        * Constuctor
        */
        CWmlFieldParser();

        /**
        * Destructor
        */        
        ~CWmlFieldParser();

    public: // new
       
        /**
        * Parses field from an array in WAP parser (wapp.dll) format into 
        * WML Bio Control's own internal format. Pushes the created array
        * into cleanup stack.
        * @param aParsedFieldArray reference to the parsed fields array, 
        * @param aWapArray reference to array where Wap AP items are pushed.
        * @param aMMSArray reference to array where MMS setting items are pushed.
        * @param aBMArray reference to array where Bookmark items are pushed.
        */  
        void ParseFieldsL( const CArrayPtrSeg<CParsedField>& aParsedFieldArray,
            CArrayPtrSeg<CWmlSubItemBase>& aBMArray);

    private: // internal member functions
        
        /**
        * Compare aFieldName with all enumerated parsed fields
        * @param aFieldName name from parser 
        * @return TWappParserFields correct enum
        */ 
        TWappParserFields CompareField( TPtrC aFieldName );

        /**
        * Checks if the field pair is characteric type. 
        * @param aFieldName name from parser 
        * @param aFieldValue value from parser 
        * @param aEnumField correct enum, which is updated.
        * @return ETrue if this is characteristic type, EFalse otherwise.
        */ 
        TBool IsCharacteristicTypeL( TPtrC aFieldName, 
            TPtrC aFieldValue, 
            TInt& aEnumField );

        /**
        * Add current bookmark item to the array.
        * The member iBm is added to the array and
        * validity checking is also performed for it.
        * Member iBm is nulled in this method.
        */
        void AddCurrentBMToArrayL();

    private: // data

        /// Bookmark item to be added in subItemArray.
        CWmlBMSubItem *iBm;

        /// Array containing WAP AP items. Not owned.
        CArrayPtrSeg<CWmlSubItemBase>* iBMArray;

        /// Indicates current character type when parsing fields.
        TInt iCurrentCharacteristicType;
    };

#endif // CWMLFIELDPARSER_H

// End of file
