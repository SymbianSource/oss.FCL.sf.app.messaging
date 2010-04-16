/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: This is the domain header API for general message data model
 *              plugin behavior
 */
//----------------------------------------------------------------
// UniMessageInfo::UniMessageInfo
// @see header
//----------------------------------------------------------------
 UniMessageInfo::UniMessageInfo( const QString& filepath,
                                const int size,
                                const QString& mimetype ) : mPath (filepath), mSize(size), mMimeType(mimetype)
    {
    
    
    }

//----------------------------------------------------------------
// UniMessageInfo::~UniMessageInfo
// @see header
//----------------------------------------------------------------
 UniMessageInfo::~UniMessageInfo()
    {
    }

//----------------------------------------------------------------
// UniMessageInfo::setPath
// @see header
//----------------------------------------------------------------
inline void UniMessageInfo::setPath( const QString& filepath )
    {
    mPath = filepath;
    }

//----------------------------------------------------------------
// UniMessageInfo::setSize
// @see header
//----------------------------------------------------------------
/*inline void UniMessageInfo::setSize( const int filesize )
    {
    mSize = filesize;
    }
*/
//----------------------------------------------------------------
// UniMessageInfo::setMimeType
// @see header
//----------------------------------------------------------------
inline void UniMessageInfo::setMimeType( const QString& mimetype )
    {
    mMimeType = mimetype;
    }

//----------------------------------------------------------------
// UniMessageInfo::path
// @see header
//----------------------------------------------------------------
inline const QString& UniMessageInfo::path()
    {
    return mPath;
    }

//----------------------------------------------------------------
// UniMessageInfo::size
// @see header
//----------------------------------------------------------------
inline const int UniMessageInfo::size()
    {
    return mSize;
    }

//----------------------------------------------------------------
// UniMessageInfo::mimetype
// @see header
//----------------------------------------------------------------
inline const QString& UniMessageInfo::mimetype()
    {
    return mMimeType;
    }

//----------------------------------------------------------------
// UniMessageInfo::serialize
// @see header
//----------------------------------------------------------------
inline void UniMessageInfo::serialize(
		QDataStream &stream) const
    {
    stream << mPath;
    //stream << mSize;
    stream << mMimeType;
    }

//----------------------------------------------------------------
// UniMessageInfo::deserialize
// @see header
//----------------------------------------------------------------
inline void UniMessageInfo::deserialize(
		QDataStream &stream)
    {
    stream >> mPath;
    //TODO resolve the problem
    //stream >> mSize;
    stream >> mMimeType;
    }




