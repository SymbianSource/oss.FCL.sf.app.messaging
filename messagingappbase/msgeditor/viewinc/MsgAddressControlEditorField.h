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
* Description:  MsgAddressControlEditorField  declaration
*
*/



#ifndef INC_MSGADDRESSCONTROLEDITORFIELD_H
#define INC_MSGADDRESSCONTROLEDITORFIELD_H

// ========== INCLUDE FILES ================================
#include <fldbase.h>            // for CTextField and MTextFieldFactory

// ========== CONSTANTS ====================================

const TUid KMsgAddressControlEditorFieldUid = { 0x100000 };

const TInt KMsgMaxStringLength = 1000;  // Maximum length of strings for iAddress and iName.
const TInt KMsgMaxFieldLength = 100;   // Maximum length of the field.

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================
class MVPbkContactLink;

// ========== CLASS DECLARATION ============================

/**
 * Defines a text field for the address control editor. This kind of field is used
 * to indicate that recipient's address has been verified i.e. found from the
 * contact database.
 *
 */
class CMsgAddressControlEditorField : public CTextField
{

public:

    /**
     * Constructor.
     */
    CMsgAddressControlEditorField();

    /**
     * Destructor.
     */
    ~CMsgAddressControlEditorField();

    /**
     *
     * @param aEntryString
     */
    void SetEntryStringL(const TDesC& aEntryString);

    /**
     *
     * @param aName
     */
    void SetNameL(const TDesC& aName);

    /**
     *
     * @param aAddress
     */
    void SetAddressL(const TDesC& aAddress);

    /**
     *
     * @param aVerified
     */
    void SetVerified(TBool aVerified);

    /**
     *
     * @param aValidated
     */
    void SetValidated(TBool aValidated);

    /**
     *
     * @param aId
     */
    void SetContactLink ( const MVPbkContactLink* aContactLink );
        /**
     *
     * @return
     */
    const MVPbkContactLink* ContactLink() const;

    /**
     *
     * @return
     */
    HBufC* Name() const;

    /**
     *
     * @return
     */
    HBufC* Address() const;

    /**
     *
     * @return
     */
    TBool IsVerified() const;

    /**
     *
     * @return
     */
    TBool IsValidated() const;
    

public:     // from TTextField

    /**
     *
     * @param aValueText
     * @return
     */
    TInt Value(TPtr& aValueText);

    /**
     *
     * @param aStream
     */
    void InternalizeL(RReadStream& aStream);

    /**
     *
     * @param aStream
     */
    void ExternalizeL(RWriteStream& aStream) const;

    /**
     *
     * @return
     */
    TUid Type() const;

protected:
    HBufC*         iEntryString;
    HBufC*         iName;
    HBufC*         iAddress;
    TBool          iVerified;
    TBool          iValidated;    
    const MVPbkContactLink* iContactLink;
};

// =========================================================

/**
 * Defines a text field factory for the address control editor.
 *
 */
class TMsgAddressControlEditorFieldFactory : public MTextFieldFactory
{

public:

    /**
     * Constructor.
     */
    TMsgAddressControlEditorFieldFactory();

    /**
     * Destructor.
     */
    virtual ~TMsgAddressControlEditorFieldFactory();

public:  // from MTextFieldFactory

    /**
     *
     * @param aFieldType
     * @return
     */
    CTextField* NewFieldL(TUid aFieldType);

private:

    /**
     * Copy constructor (not available).
     */
    TMsgAddressControlEditorFieldFactory(const TMsgAddressControlEditorFieldFactory& aSource);

    /**
     * Assignment operator (not available).
     */
    const TMsgAddressControlEditorFieldFactory& operator=(const TMsgAddressControlEditorFieldFactory& aSource);

};

#endif

// End of File
