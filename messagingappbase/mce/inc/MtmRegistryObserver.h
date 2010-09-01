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
* Description: MtmRegistryObserver  declaration
*
*/




#if !defined(__MTMREGOB_H__)
#define __MTMREGOB_H__


// FORWARD DECLARATIONS
class CBaseMtmUiData;
class CBaseMtmUi;


// CLASS DECLARATION

/**
* MMtmUiDataRegistryObserver
*/
class MMtmUiDataRegistryObserver
    {
public:
    /**
    * GetMtmUiDataL
    */
    virtual CBaseMtmUiData* GetMtmUiDataL(TUid aEntryType) = 0;
    };


/**
* MMtmUiRegistryObserver
*/
class MMtmUiRegistryObserver
    {
public:
    /**
    * GetMtmUiL
    */
    virtual CBaseMtmUi* GetMtmUiL(TUid aEntryType) = 0;
    };

#endif // __MTMREGOB_H__

// End of File
