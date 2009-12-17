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
*     Template class for CMsvSession*&
*
*/




#ifndef MCETEMPLATE_H
#define MCETEMPLATE_H

class CMsvSession;


template<class T>
class TPointer
    {
    public:
        TPointer( T*& aObj );
        TPointer& operator=( const TPointer& aObj );
        T*& operator->() const;
        T& operator*() const;

    private:
        T*& iObj;
    };

//typedef to make things easier

typedef TPointer<CMsvSession> CMsvSessionPtr;


template<class T>
TPointer<T>::TPointer( T*& aObj ) : iObj( aObj )
     {
     }

template<class T>
T*& TPointer<T>::operator->() const
    {
    return iObj;
    }

template<class T>
T& TPointer<T>::operator*() const
    {
    return *iObj;
    }

#endif

// End of file
