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
 * Description: Plugin Loader that loads all the data model plugins.
 *
 */

#ifndef UNI_DATA_MODEL_LOADER_H
#define UNI_DATA_MODEL_LOADER_H

#include <QMap>

//Forward declarations
class UniDataModelPluginInterface;



#ifdef BUILD_DLL_DATA_MODEL
#define UNI_DATA_MODEL_LOADER_EXPORT Q_DECL_EXPORT
#else
#define UNI_DATA_MODEL_LOADER_EXPORT Q_DECL_IMPORT
#endif





/**
 * Loads all the data model plugins.
 */
class UNI_DATA_MODEL_LOADER_EXPORT UniDataModelLoader
{
public:
    /**
     * Constructor.
     */
    UniDataModelLoader();

    /**
     * Destructor.
     */
    ~UniDataModelLoader();

    /**
     * Gets the  data model intended plugin.
     * @param messageType
     */
    UniDataModelPluginInterface* getDataModelPlugin(const QString& messageType);

    /**
     * Loads plugins
     */
    void loadPlugins();

private:
    /**
     * This shall store all the message types
     * mapped to plugin instance
     */
    QMap<QString, UniDataModelPluginInterface*> mDataModelPluginMap;
};

#endif //UNI_DATA_MODEL_LOADER_H
