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
* Description:  MsgRecipientItem  declaration
*
*/



#ifndef INC_MSGRECIPIENTITEM_H
#define INC_MSGRECIPIENTITEM_H

// ========== INCLUDE FILES ================================
#include <e32base.h>
#include <cntdef.h>             // for TContactItemId

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgRecipientItem;
class MVPbkContactLink;

// ========== DATA TYPES ===================================

typedef CArrayPtrFlat<CMsgRecipientItem> CMsgRecipientArray;

// ========== CLASS DECLARATION ============================

/**
 * Recipient class for address control.
 */
class CMsgRecipientItem : public CBase
{

public:

    /**
     * Factory method that creates this object.
     *
     * @param aName
     * @param aAddress
     * @return
     */
    IMPORT_C static CMsgRecipientItem* NewLC (
        const TDesC& aName, const TDesC& aAddress);

    /**
     * Factory method that creates this object.
     *
     * @param aName
     * @param aAddress
     * @return
     */
    IMPORT_C static CMsgRecipientItem* NewL (
        const TDesC& aName, const TDesC& aAddress);

    /**
     * Destructor.
     */
    ~CMsgRecipientItem();

    /**
     * Sets name.
     * @param aName
     */
    IMPORT_C void    SetNameL(const TDesC& aName);

    /**
     * Returns name.
     * @return
     */
    IMPORT_C HBufC*  Name();

    /**
     * Sets address.
     * @param aAddress
     */
    IMPORT_C void    SetAddressL(const TDesC& aAddress);

    /**
     * Returns address.
     * @return
     */
    IMPORT_C HBufC*  Address();

// RD_MESSAGING_VIRTUAL_PHONEBOOK
    /**
     * Sets contact id.
     * @param aId
     */
    IMPORT_C void SetContactLink ( const MVPbkContactLink* aContactLink );
// RD_MESSAGING_VIRTUAL_PHONEBOOK
    /**
     * Returns contact id.
     * @return
     */
    IMPORT_C const MVPbkContactLink* ContactLink();

    /**
     * Sets contact id.
     * @param aId
     */
    IMPORT_C void    SetId (TContactItemId aId);

    /**
     * Returns contact id.
     * @return
     */
    IMPORT_C TContactItemId Id();


    /**
     * Sets verified flag (i.e. entry has been fetched from phonebook).
     * @param aVerified
     */
    IMPORT_C void    SetVerified (TBool aVerified);

    /**
     * Returns verified flag (i.e. entry has been fetched from phonebook).
     * @return
     */
    IMPORT_C TBool   IsVerified();

    /**
     * Sets validated flag (i.e. entry is in correct format).
     * @param aValidated
     */
    IMPORT_C void    SetValidated (TBool aValidated);

    /**
     * Returns validated flag (i.e. entry is in correct format).
     * @return
     */
    IMPORT_C TBool   IsValidated();

protected:

    /**
     * Constructor.
     */
    CMsgRecipientItem();

private:

    HBufC*         iName;
    HBufC*         iAddress;
    const MVPbkContactLink* iContactLink ;
    TBool          iVerified;
    TBool          iValidated;

};

/**
 * Recipient list class for address control.
 */
class CMsgRecipientList : public CBase
{
public:

    /**
     * Factory method that creates this object.
     *
     * @return
     */
    IMPORT_C static CMsgRecipientList* NewL();

    /**
     * Destructor.
     */
    ~CMsgRecipientList();

    /**
     * Append item to list.
     */
    IMPORT_C void AppendL (CMsgRecipientItem* aItem);

    /**
     * Insert item to list.
     */
    IMPORT_C void InsertL(TInt aIndex, CMsgRecipientItem* aItem);

    /**
     * Return count of items in the list.
     */
    IMPORT_C TInt Count() const;

    /**
     * Return item at position aIndex.
     */
    IMPORT_C CMsgRecipientItem* At(TInt aIndex) const;

    /**
     * Return item at position aIndex.
     */
    IMPORT_C CMsgRecipientItem* operator[](TInt aIndex) const;

private:

    /**
     * Constructor.
     */
    CMsgRecipientList();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

private:

    CMsgRecipientArray* iRecipientArray;

};

#endif

// End of File
