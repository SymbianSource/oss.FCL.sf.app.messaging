/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Function info definition.
*
*/



#ifndef __MSGFUNCTIONINFO_H__
#define __MSGFUNCTIONINFO_H__

//  INCLUDES
#include <e32def.h>
#include <msvreg.h>
#include <mtudcbas.h>


// CLASS DECLARATION

/**
* TMsgFunctionInfo
* Class for mtm functions.
*/
class TMsgFunctionInfo
    {
    public:
        /**
        *
        */
        inline TMsgFunctionInfo(
            const TDesC& aCaption,
            TInt aKeyCode,
            TInt aKeyModifiers, 
            TInt aFuncId, 
            TInt aFlags, 
            TUid aMtmUid );

    public:
        TBuf<CBaseMtmUiData::TMtmUiFunction::ECaptionMaxLen> iCaption;
        TInt iKeyCode;
        TInt iKeyModifiers;
        TInt iFuncId;
        TInt iFlags;
        TUid iMtmUid;
    };

/**
* CMsgFuncArray
* Array for mtm functions.
*/
typedef CArrayFixFlat<TMsgFunctionInfo> CMsgFuncArray;

// INLINES

inline TMsgFunctionInfo::TMsgFunctionInfo( 
                                            const TDesC& aCaption, 
                                            TInt aKeyCode, 
                                            TInt aKeyModifiers, 
                                            TInt aFuncId, 
                                            TInt aFlags, 
                                            TUid aMtmUid)
                                            :iCaption( aCaption ), 
                                            iKeyCode( aKeyCode ), 
                                            iKeyModifiers( aKeyModifiers ), 
                                            iFuncId( aFuncId ), 
                                            iFlags( aFlags ), 
                                            iMtmUid( aMtmUid ) 
    {
    }

#endif // __MSGFUNCTIONINFO_H__

// End of file
