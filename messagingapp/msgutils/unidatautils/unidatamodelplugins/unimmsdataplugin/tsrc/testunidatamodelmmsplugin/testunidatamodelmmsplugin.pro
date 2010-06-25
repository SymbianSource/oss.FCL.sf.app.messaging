######################################################################
# Automatically generated by qmake (2.01a) Fri Aug 7 12:54:45 2009
######################################################################
QT += testlib
QT -= gui

TEMPLATE = lib
TARGET = test-unidatamodel-mms-plugin


INCLUDEPATH += .
INCLUDEPATH += ../../../../../internal/sf/app/messaging/mmsengine/mmsmessage/inc 
INCLUDEPATH += ../../../../../internal/sf/app/messaging/mmsengine/inc
INCLUDEPATH += ../../../../../../inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_TEST_DLL

SOURCES += \
	TestUniDataModelMmsPlugin.cpp

				 

# Input
HEADERS += \
	TestUniDataModelMmsPlugin.h
	
	   	   			 
   
SYMBIAN_PLATFORMS = WINSCW ARMV5
    symbian {
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    TARGET.EPOCALLOWDLLDATA = 1
    BLD_INF_RULES.prj_exports += "photo.jpg C:/pictures/photo.jpg"
		}
		
defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/test_unidatamodel_mms_plugin.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/test_unidatamodel_mms_plugin.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock
		
 LIBS += -leuser \
	-lconvergedmessageutils\
	-lxqutils \
	-lmsgs \
	-lsmcm \
	-lgsmu \
	-letext \
	-lmsgs \
	-lunidatamodelloader\
	-lQtCore \
	-lmmscli \
	-lefsrv \
	-lmmsmessage \
	-lmmsserversettings

