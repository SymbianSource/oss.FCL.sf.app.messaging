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
* Description:  ImumMboxDefaultData.h
*
*/


#ifndef CImumMboxDefaultData_H
#define CImumMboxDefaultData_H

// INCLUDES
#include <msvstd.h>

#include "ImumInSettingsDataImpl.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CIMASCenRepControl;
class CRepository;

// CLASS DECLARATION

/**
 *  Container class for default data.
 *
 *  This class implements functionality and stores all mailbox settings
 *  related default data. The data is read from the Central Repository
 *  and in case it fails, hardcoded values are used instead.
 *
 *  This class also contains functionality to create copy of default data
 *  settings and connection settings. This is used during the Settings
 *  data object creation phase, when specified protocol data is cloned from
 *  this object.
 *
 *  Each protocol can and should have own specific set of default settings.
 *  Currently supported protocols are Imap4, Pop3 and Smtp.
 *
 *  @lib ImumUtils.lib
 *  @since S60 3.0
 *  @see ImumInSettingsData.h
 *  @see ImumDaSettingsDataKeys.h
 *  @see ImumInSettingsDataKeys.h
 *  @see ImumConsts.h
 */
NONSHARABLE_CLASS( CImumMboxDefaultData ) : public CImumInSettingsDataImpl
    {
public:  // Constructors and destructor

    /**
     * Creates object from CImumMboxDefaultData and leaves it to cleanup stack
     *
     * @since S60 v3.2
     * @return, Constructed object
     */
    static CImumMboxDefaultData* NewLC( CImumInternalApiImpl& aMailboxApi );

    /**
     * Create object from CImumMboxDefaultData
     *
     * @since S60 v3.2
     * @return, Constructed object
     */
    static CImumMboxDefaultData* NewL( CImumInternalApiImpl& aMailboxApi );

    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CImumMboxDefaultData();

public: // New functions

    /**
     * Returns pointer to
     *
     * @since S60 v3.2
     * @param
     */
    const CImumInSettingsDataCollectionImpl* DefaultConnectionData(
        const TInt aIndex ) const;

    /**
     * Creates identical copy of general settings
     *
     * @since S60 v3.2
     * @return New settings object
     * @leave
     */
    CImumInSettingsDataArray* CloneSettingsL() const;

    /**
     * Creates identical copy of connection settings
     *
     * @since S60 v3.2
     * @param aProtocol Protocol to be cloned
     * @return New connection settings object
     * @leave
     */
    CImumInSettingsDataCollectionImpl* CloneConnectionSettingsL(
        const TUid& aProtocol ) const;

public: // Functions from base classes

    /**
     *
     * @since S60 3.2
     * @param
     * @return
     */
    virtual MImumDaSettingsDataCollection& AddSetL( const TUid& aProtocol );

    /**
     *
     * @since S60 3.2
     * @param
     * @return
     */
    virtual MImumInSettingsDataCollection& AddInSetL( const TUid& aProtocol );

    /**
     *
     * @since S60 v3.2
     * @param
     * @return
     */
    virtual TInt Validate();

    /**
     *
     * @since S60 v3.2
     * @return
     */
    virtual void Reset( const TUint aAttributeKey );

    /**
     *
     * @since S60 v3.2
     */
    virtual void ResetAll();

    /**
     *
     * @since S60 v3.2
     * @param
     * @return
     */
    virtual void Log() const;

protected:  // New virtual functions
protected:  // New functions
protected:  // Functions from base classes

private:  // Constructors

    /**
     * Default constructor for classCImumMboxDefaultData
     *
     * @since S60 v3.2
     * @return, Constructed object
     */
    CImumMboxDefaultData( CImumInternalApiImpl& aMailboxApi );

    /**
     * Symbian 2-phase constructor
     * @since S60 v3.2
     */
    void ConstructL();

private:  // New virtual functions
private:  // New functions


    /**
     *
     *
     * @since S60 v3.2
     */
    void ResetImap4Data();

    /**
     *
     *
     * @since S60 v3.2
     */
    void ResetPop3Data();

    /**
     *
     *
     * @since S60 v3.2
     */
    void ResetSmtpData();

    /**
     *
     *
     * @since S60 v3.2
     */
    void ResetGeneralData();

    /**
     * Update variated value from central repository
     *
     * @since S60 3.2
     * @param aDefaultData
     * @param aSettingsKey
     * @param aCrKey
     * @param aDefaultValue
     */
    template<class TDataType, class TSettingType>
    inline void UpdateData(
        TDataType& aDefaultData,
        const TUint aSettingsKey,
        const TUint32 aCRKey,
        const TSettingType& aDefaultValue );

    /**
     * Update variated value from central repository
     *
     * @since S60 3.2
     * @param aDefaultData
     * @param aSettingsKey
     * @param aCrKey
     * @param aDefaultValue
     */
    template<class TDataType>
    inline void UpdateData(
        TDataType& aDefaultData,
        const TUint aSettingsKey,
        const TUint32 aCRKeyLow,
        const TUint32 aCRKeyHigh,
        const TInt64& aDefaultValue );


private:  // Functions from base classes

public:     // Data
protected:  // Data
private:    // Data

    // Owned:
    CIMASCenRepControl*     iCenRepControl;
    // Owned:
    CRepository*            iImumRepository;
    };

#include "ImumMboxDefaultData.inl"

#endif //  CImumMboxDefaultData_H

// End of File
