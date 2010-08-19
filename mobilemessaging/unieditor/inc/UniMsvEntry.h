/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: UniMsvEntry  declaration
*
*/


 
#ifndef __UNIMSVENTRY_H
#define __UNIMSVENTRY_H

//  INCLUDES

#include <msvstd.h>
#include <mmsconst.h>
#include <unieditorconsts.h>

// CONSTANTS

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

class TUniMsvEntry
    {
    public:
    
        /**
        * Returns the current message type of the entry.
        * @return Current message type.
        */
        static inline TUniMessageCurrentType CurrentMessageType( const TMsvEntry& aEntry );

        /**
        * Mutator for current message type.
        * @aParam aParam Current message type.
        */
        static inline void SetCurrentMessageType( TMsvEntry& aEntry, TUniMessageCurrentType aParam );

        /**
        * Returns the charset support of the entry.
        * @return Charset support.
        */
        static inline TUniMessageCharSetSupport CharSetSupport( const TMsvEntry& aEntry );

        /**
        * Mutator for charset support.
        * @aParam aParam Charset support.
        */
        static inline void SetCharSetSupport( TMsvEntry& aEntry, TUniMessageCharSetSupport aParam );

        /**
        * If the message type of the entry is locked.
        * @return ETrue if the message type of the entry is locked.
        */
        static inline TBool IsMessageTypeLocked( const TMsvEntry& aEntry );
        
        /**
        * Mutator for setting the message type of the entry locked.
        * @aParam aParam ETrue if the message type of the entry is locked.
        */
        static inline void SetMessageTypeLocked( TMsvEntry& aEntry, TBool aParam );

        /**
        * If the message entry is a forwarded message.
        * @return ETrue if the entry is a forwarded message.
        */
        static inline TBool IsForwardedMessage( const TMsvEntry& aEntry );
        
        /**
        * Mutator for forwarded message flag.
        * @aParam aParam ETrue if the entry is a forwarded message.
        */
        static inline void SetForwardedMessage( TMsvEntry& aEntry, TBool aParam );

        /**
        * If the message entry is an editor-oriented message.
        * @return ETrue if the entry is an editor-oriented message.
        */
        static inline TBool IsEditorOriented( const TMsvEntry& aEntry );
        
        /**
        * Mutator for editor-oriented message flag.
        * @aParam aParam ETrue if the entry is an editor-oriented message.
        */
        static inline void SetEditorOriented( TMsvEntry& aEntry, TBool aParam );

        /**
        * If the message entry is a "MMS Upload" message.
        * @return ETrue if the entry is a "MMS Upload" message.
        */
        static inline TBool IsMmsUpload( const TMsvEntry& aEntry );
        
        /**
        * Mutator for "MMS Upload" identifier.
        * @aParam aParam ETrue if the entry is a "MMS Upload" message.
        */
        static inline void SetMmsUpload( TMsvEntry& aEntry, TBool aParam );

    };

#include <unimsventry.inl>

#endif      // __UNIMSVENTRY_H
            
// End of File
