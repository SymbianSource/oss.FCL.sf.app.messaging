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
 * Description: Plugin Loader class
 *
 */

// System Includes

#include <QDir>
#include "debugtraces.h"

#include <QLibraryInfo>
#include <QPluginLoader>
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"

//---------------------------------------------------------------
//UniDataModelLoader::UniDataModelLoader()
//@see header
//---------------------------------------------------------------
UniDataModelLoader::UniDataModelLoader()
{
}

//---------------------------------------------------------------
//UniDataModelLoader::~UniDataModelLoader()
//@see header
//---------------------------------------------------------------
UniDataModelLoader::~UniDataModelLoader()
{
    QMap<QString, UniDataModelPluginInterface*>::iterator mapItor;
    for (mapItor = mDataModelPluginMap.begin(); mapItor
            != mDataModelPluginMap.end(); ++mapItor)
    {
#ifdef _DEBUG_TRACES_
        qDebug() << "Deleting Handler id:" << mapItor.key();
#endif
        UniDataModelPluginInterface* handler = mapItor.value();
        if (handler)
            delete handler;
    }
    mDataModelPluginMap.clear();
}

//---------------------------------------------------------------
//UniDataModelLoader::loadPlugins()
//@see header
//---------------------------------------------------------------
void UniDataModelLoader::loadPlugins()
{
    // plugins directory setting for EMULATOR
#ifdef __WINS__
    QDir dir(QLibraryInfo::location(QLibraryInfo::PluginsPath));
    dir.cd("messaging\\datamodel");
#else
    // plugins directory setting for HARDWARE IMAGE
    QDir dir("Z:\\resource\\qt\\plugins\\messaging\\datamodel");
#endif
    QString pluginPath = dir.absolutePath();
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter LoadPlugin path = " << pluginPath;
#endif

    // load the plugins
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Readable);
    foreach (QFileInfo entry, entries) 
        {
            QPluginLoader loader(entry.absoluteFilePath());
            // Check if plugin is already loaded...
            if (!loader.isLoaded())
            {
                UniDataModelPluginInterface* datamodelPlugin = qobject_cast<
                        UniDataModelPluginInterface*> (loader.instance());
                if (datamodelPlugin)
                {
                	  UniDataModelPluginInterface* pluginInstance = qobject_cast<
                        UniDataModelPluginInterface*> (datamodelPlugin->createInstance());                        
                    QString mType = pluginInstance->messageType();
                    QDEBUG_WRITE_FORMAT("plugin added to map", mType);
                    mDataModelPluginMap.insert(mType, pluginInstance);
                }
            }

        }
}

//---------------------------------------------------------------
//UniDataModelLoader::getDataModelPlugin()
//@see header
//---------------------------------------------------------------
UniDataModelPluginInterface* UniDataModelLoader::getDataModelPlugin(
                                                                    const QString& messageType)
{
    if (mDataModelPluginMap.contains(messageType))
    {
		QDEBUG_WRITE_FORMAT("getDataModelPlugin messageType is present ", messageType);
        return mDataModelPluginMap[messageType];
    }
    return NULL;
}
