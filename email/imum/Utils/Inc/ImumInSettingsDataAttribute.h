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
* Description:  ImumInSettingsDataAttribute.h
*
*/


#ifndef C_IMUMINSETTINGSDATAATTRIBUTE_H
#define C_IMUMINSETTINGSDATAATTRIBUTE_H

#include <e32base.h>

/**
 *  Object with unique key ID to store anydata possible.
 *
 *  @lib ImumUtils.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CImumInSettingsDataAttribute ) : public CBase
    {
public:  // Constructors and destructor

    /**
     * Creates the attribute object.
     *
     * @since S60 v3.2
     * @param aAttributeId Unique id of the attribute
     * @param aAttributeType Type of the attribute
     * @param aAttributeSize Size of the data
     * @param aAttributeValue The actual value of the attribute
     * @param aAttributeDefault The default value of the attribute
     * @return Constructed object
     * @see ImumInSettingsKeys.h
     * @see ImumDaSettingsKeys.h
     */
    static CImumInSettingsDataAttribute* NewLC(
        const TUint aAttributeId,
        const TInt aAttributeType,
        const TUint aAttributeSize,
        const TAny* aAttributeValue,
        const TAny* aAttributeDefault );

    /**
     * Creates the attribute object.
     *
     * @since S60 v3.2
     * @param aAttributeId Unique id of the attribute
     * @param aAttributeType Type of the attribute
     * @param aAttributeSize Size of the data
     * @param aAttributeValue The actual value of the attribute
     * @param aAttributeDefault The default value of the attribute
     * @return Constructed object
     * @see ImumInSettingsKeys.h
     * @see ImumDaSettingsKeys.h
     */
    static CImumInSettingsDataAttribute* NewL(
        const TUint aAttributeId,
        const TInt aAttributeType,
        const TUint aAttributeSize,
        const TAny* aAttributeValue,
        const TAny* aAttributeDefault );

    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CImumInSettingsDataAttribute();

public: // New functions

    /**
     * Retrieves the stored value of the attribute.
     *
     * @since S60 v3.2
     * @param aAttribute Return parameter to store the value.
     * @type T The type of the attribute
     * @return
     */
    template<class T>
    inline void Value( T& aAttribute ) const;

    /**
     * Creates identical copy of the object
     *
     * @since S60 v3.2
     * @return Identical copy of the object
     */
    CImumInSettingsDataAttribute* CloneL();

    /**
     * Copies content of the attribute.
     *
     * @since S60 v3.2
     * @param aAttribute Attribute to be copied
     */
    void Copy(
        const CImumInSettingsDataAttribute& aAttribute );

    /**
     * Resets the value to default.
     *
     * @since S60 v3.2
     */
    void Reset();

    /**
     * Compares objects.
     *
     * @since S60 v3.2
     * @param aAttribute The attribute to be compared
     * @return ETrue, when objects are matching
     */
    TBool operator==(
        const CImumInSettingsDataAttribute& aAttribute );

    /**
     * Compares objects.
     *
     * @since S60 v3.2
     * @param aAttribute The attribute to be compared
     * @return ETrue, when objects are not matching
     */
    TBool operator!=(
        const CImumInSettingsDataAttribute& aAttribute );

    /**
     * Sets the id of the attribute.
     *
     * @since S60 v3.2
     * @param aId New unique ID
     */
    void SetId( const TUint aId );

    /**
     * Returns the unique Id of the key.
     *
     * @since S60 v3.2
     * @return The id of the key
     */
    TUint Id() const;

    /**
     * Returns the type of the key.
     *
     * @since S60 v3.2
     * @return The type of the key
     */
    TInt Type() const;

    /**
     * Method to compare id's of the to attributes. For RArrayPointer class.
     *
     * @since S60 v3.2
     * @param aLeft Item for comparing
     * @param aLeft Another item for comparing
     * @return ETrue/EFalse depending if the ID matches
     */
    static TBool IdCompare(
        const CImumInSettingsDataAttribute& aLeft,
        const CImumInSettingsDataAttribute& aRight );

protected:  // Constructors

    /**
    * Default constructor for classCImumInSettingsDataAttribute
    * @since S60 v3.2
    * @return, Constructed object
    */
    CImumInSettingsDataAttribute();

    /**
     * Symbian 2-phase constructor
     *
     * @since S60 v3.2
     * @param aAttributeId Unique id of the attribute
     * @param aAttributeType Type of the attribute
     * @param aAttributeSize Size of the data
     * @param aAttributeValue The actual value of the attribute
     * @param aAttributeDefault The default value of the attribute
     * @see ImumInSettingsKeys.h
     * @see ImumDaSettingsKeys.h
     */
    void ConstructL(
        const TUint aAttributeId,
        const TInt aAttributeType,
        const TUint aAttributeSize,
        const TAny* aAttribute,
        const TAny* aAttributeDefault );

private:  // New functions

    /**
     * Deletes the text objects allocated by the class
     *
     * @since S60 v3.2
     */
    void DeleteText();

    /**
     * Deletes number objects allocated by the class
     *
     * @since S60 v3.2
     * @param
     */
    template<class T>
    inline void DeleteNumber();

    /**
     * Deletes any objects allocated by the attribute.
     *
     * @since S60 v3.2
     */
    void Cleanup();

    /**
     * Allocates memory from heap for the values
     *
     * @since S60 v3.2
     * @template T type of the parameter
     * @param aValue The actual value of the attribute
     * @param aDefault The default value of the attibute
     */
    template<class T>
    inline void DoAllocL(
        const TAny* aValue,
        const TAny* aDefault );

    /**
     * Allocates memory from heap for the values
     *
     * @since S60 v3.2
     * @param aValue The actual value of the attribute
     * @param aDefault The default value of the attibute
     */
    void AllocL(
        const TAny* aValue,
        const TAny* aDefault );

    /**
     * Sets all the values of the attribute
     *
     * @since S60 v3.2
     * @param aAttributeId Unique id of the attribute
     * @param aAttributeType Type of the attribute
     * @param aAttributeSize Size of the data
     * @param aAttributeValue The actual value of the attribute
     * @param aAttributeDefault The default value of the attribute
     * @see ImumInSettingsKeys.h
     * @see ImumDaSettingsKeys.h
     */
    void SetAttributeL(
        const TUint aAttributeId,
        const TInt aAttributeType,
        const TUint aAttributeSize,
        const TAny* aAttributeValue,
        const TAny* aAttributeDefault );

    /**
     * Logs the data in the attribute. Debug only.
     *
     * @since S60 v3.2
     */
    void Log() const;

private:    // Data

    // Unique ID
    TUint           iId;
    // Item type
    TInt            iType;
    // Length of the item
    TUint           iSize;
    // Owned: The value, of which type is defined by iType
    TAny*           iValue;
    // Owned: Default value of the attribute
    TAny*           iDefault;
    };

#include "ImumInSettingsDataAttribute.inl"

#endif //  C_IMUMINSETTINGSDATAATTRIBUTE_H

// End of File
