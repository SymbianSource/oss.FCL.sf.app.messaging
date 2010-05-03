# #####################################################################
# Automatically generated by qmake (2.01a) Mon Aug 31 10:36:00 2009
# #####################################################################
TEMPLATE = lib
TARGET = settingsview

DEFINES += SETTINGSVIEW_DLL

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../../msgui/inc
INCLUDEPATH += ../../msgutils/s60qconversions/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

CONFIG += hb

# Platforms
SYMBIAN_PLATFORMS = WINSCW \
                    ARMV5
    
# Input
SOURCES +=  src/msgsettingengine.cpp \
            src/smssettingsprivate.cpp \
            src/mmssettingsprivate.cpp \
            src/msgsettingsdataformcustomitem.cpp \
            src/msgsettingsform.cpp \
	    src/msgsettingsview.cpp \
	    src/msgsmscentersettingsform.cpp \
	    src/msgsmscenterview.cpp
			
HEADERS +=  inc/msgsettingengine.h \
            inc/smssettingsprivate.h \
            inc/mmssettingprivate.h \
            inc/msgsettingsdataformcustomitem.h\
            inc/msgsettingsform.h \
	    inc/msgsettingsview.h \
	    inc/msgsmscentersettingsform.h \
	    inc/msgsmscenterview.h \
	    ../../msgui/inc/msgbaseview.h
		   
symbian: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x20026813    
}

# Build.inf rules
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/settingsview.iby CORE_APP_LAYER_IBY_EXPORT_PATH(settingsview.iby)" \
    "stub_sis/settingsview_stub.sis   /epoc32/data/z/system/install/settingsview_stub.sis"
     
TARGET.CAPABILITY = All -TCB

LIBS += -lSmcm \
    -lmsgs \	
    -ls60qconversions \
    -lcommdb \
    -lmmsserversettings \
    -lmmsgenericsettings
    