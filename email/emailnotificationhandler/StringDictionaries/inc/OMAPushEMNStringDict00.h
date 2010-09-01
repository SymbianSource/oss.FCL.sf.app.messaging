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
* Description: COMAPushEMNStringDict00
*		
*
*/



#ifndef __OMAPUSHEMNSTRINGDICT00_H__
#define __OMAPUSHEMNSTRINGDICT00_H__

#include <e32std.h>
#include <stringpool.h>
#include <xml/plugins/stringdictionary.h>


class COMAPushEMNStringDict00 : public CBase, public Xml::MStringDictionary
    {
    public:
        static Xml::MStringDictionary* NewL( TAny* aStringPool );
        virtual ~COMAPushEMNStringDict00();
        void ConstructL();

        void ElementL( TInt aToken, RString& aElement ) const;
        void AttributeL( TInt aToken, RString& aAttribute ) const;
        void AttributeValuePairL(
            TInt aToken, RString& aAttribute, RString& aValue ) const;
        void AttributeValueL( TInt aToken, RString& aValue ) const;

        TBool CompareThisDictionary( 
            const RString& aDictionaryDescription ) const;

        TInt SwitchCodePage( TInt aCodePage );
        void PublicIdentifier( RString& aPubId );
        void NamespaceUri( RString& aUri );

        void Release();

    public:

    // CodePage tokens.
    //
    // Note: There will be correlation between these token values and their
    // strings via the CDictionaryCodePage using the RStringPool.
    
    enum TTagToken {
        // CodePage 00
        // -//WAPFORUM//DTD EMN 1.0//EN
        ETagTokenEmn                        = 0x05,
      };
  
    enum TAttributeToken {
        // CodePage 00
        // -//WAPFORUM//DTD EMN 1.0//EN
        EAttributeTokenTimestamp            = 0x05,
        EAttributeTokenMailbox              = 0x06,
        EAttributeTokenMailboxMailat        = 0x07,
        EAttributeTokenMailboxImap          = 0x08,
        EAttributeTokenMailboxPop           = 0x09,
        EAttributeTokenMailboxHttp          = 0x0A,
        EAttributeTokenMailboxHttpWWW       = 0x0B,
        EAttributeTokenMailboxHttps         = 0x0C,
        EAttributeTokenMailboxHttpsWWW      = 0x0D,
    };
  
    enum TAttributeValueToken {
        // CodePage 00
        // -//WAPFORUM//DTD EMN 1.0//EN
        EAttributeValueTokenTimestamp       = 0x05,
        EAttributeValueTokenMailbox         = 0x06,
        EAttributeValueTokenMailat          = 0x07,
        EAttributeValueTokenPop             = 0x08,
        EAttributeValueTokenImap            = 0x09,
        EAttributeValueTokenHttp            = 0x0A,
        EAttributeValueTokenHttpWWW         = 0x0B,
        EAttributeValueTokenHttps           = 0x0C,
        EAttributeValueTokenHttpsWWW        = 0x0D,
        EAttributeValueTokenCom             = 0x85,
        EAttributeValueTokenEdu             = 0x86,
        EAttributeValueTokenNet             = 0x87,
        EAttributeValueTokenOrg             = 0x88,
    };



    private:

        COMAPushEMNStringDict00( RStringPool* aStringPool );

    private:

        /**
        The StringPool for this string dictionary.
        We don't own this.
        */
        RStringPool        iStringPool;

        Xml::CDictionaryCodePage*  iCodepage00Table;

        TInt          iCodePage;

    };


const TInt iTagCodePage00[] = 
    {
    COMAPushEMNStringDict00::ETagTokenEmn,
    0
    }; // ok to end with zero as this is used by a global token


const TInt iAttributeCodePage00[] = 
    {
    COMAPushEMNStringDict00::EAttributeTokenTimestamp,
    COMAPushEMNStringDict00::EAttributeTokenMailbox,
    COMAPushEMNStringDict00::EAttributeTokenMailboxMailat,
    COMAPushEMNStringDict00::EAttributeTokenMailboxImap,
    COMAPushEMNStringDict00::EAttributeTokenMailboxPop,
    COMAPushEMNStringDict00::EAttributeTokenMailboxHttp,
    COMAPushEMNStringDict00::EAttributeTokenMailboxHttpWWW,
    COMAPushEMNStringDict00::EAttributeTokenMailboxHttps,
    COMAPushEMNStringDict00::EAttributeTokenMailboxHttpsWWW,
    0
    }; // ok to end with zero as this is used by a global token



const TInt iAttributeValueCodePage00[] = 
    {
    COMAPushEMNStringDict00::EAttributeValueTokenTimestamp,
    COMAPushEMNStringDict00::EAttributeValueTokenMailbox,
    COMAPushEMNStringDict00::EAttributeValueTokenMailat,
    COMAPushEMNStringDict00::EAttributeValueTokenPop,
    COMAPushEMNStringDict00::EAttributeValueTokenImap,
    COMAPushEMNStringDict00::EAttributeValueTokenHttp,
    COMAPushEMNStringDict00::EAttributeValueTokenHttpWWW,
    COMAPushEMNStringDict00::EAttributeValueTokenHttps,
    COMAPushEMNStringDict00::EAttributeValueTokenHttpsWWW,
    COMAPushEMNStringDict00::EAttributeValueTokenCom,
    COMAPushEMNStringDict00::EAttributeValueTokenEdu,
    COMAPushEMNStringDict00::EAttributeValueTokenNet,
    COMAPushEMNStringDict00::EAttributeValueTokenOrg,
    0
    }; // ok to end with zero as this is used by a global token

#endif //__OMAPUSHEMNSTRINGDICT00_H__
