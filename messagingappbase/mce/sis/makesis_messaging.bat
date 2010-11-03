@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:  makesis_messaging.bat

@ECHO OFF

ECHO ************************************

ECHO To update conversation application/server create conversation app/server installer
ECHO before creating  messaging installer. Compile conversation from /ext/app/messagingext/group.
ECHO and then run /ext/app/messagingext/messagingconversation/docs/makesis-iad.bat to export
ECHO latest conversation installer to \epoc32\data\z\system\install\ path. Before run this
ECHO BAT file all language variant related resource need to be added conversation PKG file, and
ECHO sign certificate need to be modify based on prd/rnd image. If certificate is not up to date, 
ECHO update will be failed for certain component.

ECHO If there are no changes in conversation then conversation update can be prevented by removing
ECHO conversation embedded sisx from messaging pkg file.

ECHO ***********************************

makesis Messaging.pkg
