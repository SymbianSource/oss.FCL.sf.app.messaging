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
 * Description:
 *
 */

// INCLUDES
#include <HbTextEdit>
#include <HbTextItem>
#include <HbFrameItem>
#include <HbFrameDrawer>
#include <HbIconItem>
#include <HbPushButton>
#include <QFileInfo>
#include <HbAction>
#include <hbinputeditorinterface.h>
#include <QGraphicsLayout>
#include <HbGestureSceneFilter>
#include <HbGesture>
#include <HbMenu>
#include <HbMainWindow>
#include <hbinstance.h>
#include <HbDeviceProfile>
#include <QImageReader>
#include <QFileInfo>
#include <apmstd.h>
#include <MsgMediaResolver.h>
#include <MsgImageInfo.h>
#include <HbIconAnimationManager>
#include <HbIconAnimator>
#include <HbIcon>

#include <csmsaccount.h>
#include <smutset.h>
#include <xqaiwrequest.h>
#include <xqrequestinfo.h>
#include <xqsharablefile.h>
#include <xqappmgr.h>

// USER INCLUDES
#include "msgunieditorbody.h"
#include "UniEditorGenUtils.h"
#include "unisendingsettings.h"
#include "msgmonitor.h"
#include "s60qconversions.h"
#include "mmsconformancecheck.h"
#include "unieditorpluginloader.h"
#include "unieditorplugininterface.h"
#include "convergedmessage.h"
#include "msgmediautil.h"

// Constants
const QString EDITOR_FRAME("qtg_fr_lineedit_normal");
const QString BACKGROUND_FRAME("qtg_fr_btn_normal");
const QString AUDIO_REGION("AudioRegion");
const QString VIDEO_REGION("VideoRegion");
const QString IMAGE_REGION("ImageRegion");
const QString INVALID_REGION("InvalidRegion");
const QString SEND_ICON("qtg_mono_send");

//Localized Constants for item specific menu
#define LOC_OPEN    hbTrId("txt_common_menu_open")
#define LOC_REMOVE  hbTrId("txt_common_menu_remove")
#define LOC_DETAILS hbTrId("txt_common_menu_details")
#define LOC_TITLE   hbTrId("txt_messaging_title_messaging")

const QString AUDIO_ICON("qtg_mono_audio");
const QString ANIMATION_ICON(":/qtg_anim_longtap_2");

const TInt KShowCounterLimit = 10;

MsgUnifiedEditorBody::MsgUnifiedEditorBody( const QString& pluginPath, 
    QGraphicsItem *parent ) :
HbWidget(parent),
mHasImage(false),
mHasAudio(false),
mTextEdit(0),
mEditorFrame(0),
mIconItem(0),
mAudioItem(0),
mPluginPath(pluginPath),
mImageSize(0),
mAudioSize(0),
mVideoSize(0),
mProcessImageOperation(0),
mMediaResolver(0),
mImageInfo(0)
{
    setPluginBaseId(style()->registerPlugin(mPluginPath));

    mTextEdit = new HbTextEdit(this);
    HbStyle::setItemName(mTextEdit,"textEdit");

    HbFrameDrawer* frameDrawer = new HbFrameDrawer(EDITOR_FRAME, 
                                                   HbFrameDrawer::NinePieces);
    
    mEditorFrame = new HbFrameItem(frameDrawer,this);
    HbStyle::setItemName(mEditorFrame,"textEditFrame");
    mEditorFrame->setZValue(-1);
    
    // add "Send" action in VKB
    HbEditorInterface editorInterface(mTextEdit);
    HbAction *sendAction = new HbAction(HbIcon(SEND_ICON), QString(),this);
    connect(sendAction, SIGNAL(triggered()),this, SIGNAL(sendMessage()));
    editorInterface.addAction(sendAction);

    mGestureFilter = new HbGestureSceneFilter(Qt::LeftButton, this);
    mGestureFilter->setLongpressAnimation(true);
    HbGesture *gesture = new HbGesture(HbGesture::longpress, 5);
    mGestureFilter->addGesture(gesture);
    connect(gesture, SIGNAL(longPress(QPointF)), this, SLOT(longPressed(QPointF)));
    connect(mTextEdit, SIGNAL(contentsChanged()), this, SLOT(onTextChanged()));

    mfs.Connect();
    mfs.ShareProtected();

    mMmsConformanceCheck = new MmsConformanceCheck;
    
    mCharCounter = new HbTextItem(this);
    HbStyle::setItemName(mCharCounter, "charCounter");
    mCharCounter->setZValue(1.5);
    mCharCounter->setText("160(1)");
    
    mBackgroundItem = new HbFrameItem(this);
    HbStyle::setItemName(mBackgroundItem, "charCounterFrame");

    mBackgroundItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    mBackgroundItem->frameDrawer().setFillWholeRect(true);
    
    mBackgroundItem->frameDrawer().setFrameGraphicsName(
        BACKGROUND_FRAME);    
    
    mPluginLoader = new UniEditorPluginLoader(this);

    mPluginInterface =
                        mPluginLoader->getUniEditorPlugin(ConvergedMessage::Sms);    
    
    CSmsSettings* settings = CSmsSettings::NewLC();
    CSmsAccount* account = CSmsAccount::NewLC();
    account->LoadSettingsL( *settings );

    if( settings->CharacterSet() == TSmsDataCodingScheme::ESmsAlphabetUCS2)
    {
    mCharSupportType = TUniSendingSettings::EUniCharSupportFull;
    }
    else
    {
    mCharSupportType = TUniSendingSettings::EUniCharSupportReduced;
    }
    
    CleanupStack::PopAndDestroy( account );
    CleanupStack::PopAndDestroy( settings );                

    //Set the mPrevBuffer to NULL initially
    mPrevBuffer = QString();
    
    mCharCounter->setVisible(false);
    mBackgroundItem->setVisible(false);
}

MsgUnifiedEditorBody::~MsgUnifiedEditorBody()
{
    style()->unregisterPlugin(mPluginPath);
    delete mMmsConformanceCheck;
    delete mProcessImageOperation;
    delete mMediaResolver;
    delete mImageInfo;
    //Close has to be called after ProcessImageOperation object is deleted
    mfs.Close();
}

QString MsgUnifiedEditorBody::text()
{
    return mTextEdit->toPlainText();
}

void MsgUnifiedEditorBody::setImage(QString& imagefile)
{
    if (!mImageInfo)
     {
        setImage(true);
        
        mImageFile = imagefile;
        if (mIconItem)
        {
            delete mIconItem;
            mIconItem = NULL;
            mImageSize = 0;
        }

        int error = KErrNone;
        
        if( !mProcessImageOperation )
        {
        TRAP(error,mProcessImageOperation = 
            CUniEditorProcessImageOperation::NewL(*this, mfs));
        }
        if( !mMediaResolver && error == KErrNone )
        {
        TRAP(error,mMediaResolver = CMsgMediaResolver::NewL());
        }

        if( error == KErrNone)
        {
            mMediaResolver->SetCharacterSetRecognition(EFalse);

            HBufC *name = S60QConversions::qStringToS60Desc(imagefile);
            RFile file = mMediaResolver->FileHandleL(*name);
     
            TRAP(error,mImageInfo = static_cast<CMsgImageInfo*>
            (mMediaResolver->CreateMediaInfoL(file)));
            if (error == KErrNone)
            {
                TRAP(error, mMediaResolver->ParseInfoDetailsL(mImageInfo, file));
            }

            file.Close();
            delete name;
        }
        
        if (error == KErrNone)
        {
            mSavedImageFile = imagefile;
            startResizeAnimation();
            mProcessImageOperation->Process(mImageInfo);
        }
        else
        {
            delete mImageInfo;
            mImageInfo = NULL;
            mSavedImageFile.clear();
            handleSetImage();
        }
    }
}

void MsgUnifiedEditorBody::handleSetImage()
{   
    //check for insert conformance
    if(EInsertSuccess != mMmsConformanceCheck->checkModeForInsert(mImageFile))
        return;
    
    int msgSize = messageSize();
    QFileInfo fileinfo(mImageFile);
    int imageSize = fileinfo.size() + KEstimatedMimeHeaderSize;
    if ( (imageSize + msgSize) <= MsgMonitor::maxMmsSize())
    {
        mImageSize = imageSize;
    }
    else
    {
        mImageFile.clear();
        setImage(false);
        //Show appropriate note and leave
        return;
    }

    mIconItem = new HbIconItem(this);
    mIconItem->hide();
    //mIconItem->setIconName(mImageFile);
    QPixmap pixmap(mImageFile);
    mIconItem->setIcon(HbIcon(pixmap));

    HbStyle::setItemName(mIconItem, "pixmap");
    mIconItem->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    mIconItem->installSceneEventFilter(mGestureFilter);

    // repolish the body widget
    this->repolish();

    // emit signal to indicate addition of image
    emit contentChanged();
}

void MsgUnifiedEditorBody::setAudio(QString& audiofile)
{
    //check for insert conformance
    if(EInsertSuccess != mMmsConformanceCheck->checkModeForInsert(audiofile))
        return;

    setAudio(true);
    mAudioFile = audiofile;
    if(mAudioItem)
    {
        delete mAudioItem;
        mAudioItem = NULL;
        mAudioSize = 0;
    }

    //TODO: Add conformance checks before calculating the size    
    int msgSize = messageSize();
    QFileInfo fileinfo(mAudioFile);
    int audioSize = fileinfo.size() + KEstimatedMimeHeaderSize;
    if((audioSize + msgSize) <= MsgMonitor::maxMmsSize() )
    {
    	mAudioSize = audioSize;
    }
    else
    {
    	mAudioFile.clear();
    	setAudio(false);
    	//Show appropriate note and leave
    	return;
    }    

    HbIconItem* audioIcon = new HbIconItem(AUDIO_ICON);
    mAudioItem = new HbPushButton(this);
    mAudioItem->hide();
    HbStyle::setItemName(mAudioItem,"audioItem");
    mAudioItem->setIcon(audioIcon->icon());
    mAudioItem->setText(fileinfo.baseName());
    MsgMediaUtil mediaUtil;
    mAudioItem->setAdditionalText(mediaUtil.mediaDuration(mAudioFile));
    mAudioItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    connect(mAudioItem, SIGNAL(longPress(QPointF)), this, SLOT(longPressed(QPointF)));

    // repolish the body widget
    this->repolish();

    // emit signal to indicate addition of audio
    emit contentChanged();
}

void MsgUnifiedEditorBody::setVideo(QString& videofile)
{
    //check for insert conformance
    if(EInsertSuccess != mMmsConformanceCheck->checkModeForInsert(videofile))
        return;

    // update the media file-list
    mVideoFile = videofile;

    //TODO: Add conformance checks before calculating the size    
    int msgSize = messageSize();
    QFileInfo fileinfo(mVideoFile);
    int videoSize = fileinfo.size() + KEstimatedMimeHeaderSize;
    if((videoSize + msgSize) <= MsgMonitor::maxMmsSize() )    
    {
    	mVideoSize = videoSize;
    }
    else
    {
    	//Show appropriate note and leave
    	return;
    }    

    //TODO: create video item instance

    // repolish the body widget
    this->repolish();

    // emit signal to indicate addition of video
    emit contentChanged();
}

void MsgUnifiedEditorBody::setText(QString& text)
{
	mTextEdit->setPlainText(text);
}

const QStringList MsgUnifiedEditorBody::mediaContent()
{
    QStringList mediaFiles;
    // Pick mImageFile only if mSavedImageFile is empty 
    // as it means we are not in middle of resizing
    if (mHasImage && mSavedImageFile.isEmpty()) 
        mediaFiles << mImageFile;
    if(mHasAudio)
        mediaFiles << mAudioFile;

    //TODO: append video content

    return mediaFiles;
}

QSizeF MsgUnifiedEditorBody::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    QSizeF szHint = HbWidget::sizeHint(which,constraint);
    
    QList<HbMainWindow *> windowList = hbInstance->allMainWindows();
    HbMainWindow* mMainWindow = windowList[0];

    qreal leftMargin = 0.0;
    qreal rightMargin = 0.0;
    qreal chromeHeight = 0.0;
    qreal toolbarHeight = 0.0;
    qreal bodyItemSpacing = 0.0;
    
    style()->parameter("hb-param-margin-gene-left",leftMargin);
    style()->parameter("hb-param-margin-gene-right",rightMargin);
    style()->parameter("hb-param-widget-chrome-height",chromeHeight);
    style()->parameter("hb-param-widget-toolbar-height",toolbarHeight);
    style()->parameter("hb-param-margin-gene-middle-vertical",bodyItemSpacing);

    HbDeviceProfile prf = HbDeviceProfile::profile(mMainWindow);
    qreal unt = prf.unitValue();

    // Note: With NGA, the orientation change does not cause HW to switch orientation.
    // So, the HW resolution also remains unchanged. We need to swap values of width &
    // height to map portrait resolution to landscape resolution.
    qreal maxWidth = 0.0, maxHeight = 0.0;
    
    // Landscape
    if(mMainWindow->orientation() == Qt::Horizontal)
        {
        maxHeight = mMainWindow->width()-chromeHeight-toolbarHeight;
        maxWidth = (mMainWindow->height()-leftMargin-rightMargin-(2*unt))/2;
        }
    else
    {
        maxWidth = mMainWindow->width()-leftMargin-rightMargin;
        maxHeight = mMainWindow->height()-chromeHeight-toolbarHeight;
    }
    szHint.setHeight(maxHeight);
    
    // add space for audio
    if(mAudioItem)
    {
		if(mMainWindow->orientation() == Qt::Horizontal)
		{
			mAudioItem->setStretched(true);
		}
		else
		{
			mAudioItem->setStretched(false);
		}
        QSizeF audioSize = mAudioItem->effectiveSizeHint(which, constraint);
        szHint.rheight() += (audioSize.height() + bodyItemSpacing);
        mAudioItem->show();
    }
    
    if(mIconItem)
        {
        QSizeF imageSize(0.0,0.0);
        QSizeF defaultImageSize(QImageReader(mImageFile).size());

        if(!defaultImageSize.isEmpty())
            {
            imageSize.setWidth(maxWidth);
            if(mMainWindow->orientation() == Qt::Vertical)
            {
                szHint.rheight() += bodyItemSpacing;
            }

            qreal newHeight = 0.0;
            if(defaultImageSize.width() <= imageSize.width())
            {
                // resize not needed
                newHeight = qMin(defaultImageSize.height(), maxHeight);
                imageSize.setHeight(newHeight);
            }
            else
            {
                // keep aspect-ratio and resize
                newHeight = imageSize.width()*(defaultImageSize.height()/defaultImageSize.width());
                newHeight = qMin(newHeight,maxHeight);
                imageSize.setHeight(newHeight);
            }

            if(newHeight == maxHeight)
            {
                qreal newWidth = defaultImageSize.width()*newHeight/defaultImageSize.height();
                imageSize.setWidth(newWidth);
            }

            mIconItem->setPreferredSize(imageSize);
            mIconItem->setSize(imageSize);
            if(mMainWindow->orientation() == Qt::Horizontal)
            {
                QPointF currPos = mIconItem->pos();
                currPos.setX(leftMargin+((maxWidth-imageSize.width())/2));
                mIconItem->setPos(currPos);
            }
            mIconItem->show();
            szHint.rheight() += imageSize.height();
        }
    }

    mTextEdit->setMinimumHeight(maxHeight);
    szHint.rheight() += bodyItemSpacing;
    return szHint;
}

void MsgUnifiedEditorBody::longPressed(QPointF position)
{
    HbMenu* menu = new HbMenu;
    menu->addAction(LOC_OPEN, this, SLOT(openMedia()));
    menu->addAction(LOC_REMOVE, this, SLOT(removeMedia()));
    menu->addAction(LOC_DETAILS, this, SLOT(viewDetails()));

    menu->setDismissPolicy(HbPopup::TapAnywhere);
    menu->setAttribute(Qt::WA_DeleteOnClose, true);
    menu->setPreferredPos(position);

    menu->setObjectName(getHitRegion(position));
    menu->show();
}

void MsgUnifiedEditorBody::removeMedia()
{
    HbAction* action = qobject_cast<HbAction*>(sender());
    QString objName = action->parent()->objectName();
    if(objName == IMAGE_REGION)
    {
        mImageFile.clear();
        if(mIconItem)
        {
            mIconItem->removeSceneEventFilter(mGestureFilter);
            delete mIconItem;
            mIconItem = NULL;
        }
        setImage(false);
        mImageSize = 0;
    }
    else if(objName == AUDIO_REGION)
    {
        mAudioFile.clear();
        if(mAudioItem)
        {
            delete mAudioItem;
            mAudioItem = NULL;
        }
        setAudio(false);
        mAudioSize = 0;
    }
    else if(objName == VIDEO_REGION)
    {
        //TODO: complete this with video handling story
    }
    else
    {
        // return without doing anything
        return;
    }

    this->repolish();

	emit contentChanged();
}

//---------------------------------------------------------------
// MsgUnifiedEditorBody :: openMedia
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorBody::openMedia()
{
    HbAction* action = qobject_cast<HbAction*>(sender());
    QString objName = action->parent()->objectName();
    
    QString fileName;
    if ( objName == IMAGE_REGION )
    {
        fileName = mImageFile;
    }
    else if ( objName == AUDIO_REGION )
    {
        fileName = mAudioFile;
    }
    else
    {
        return;
    }
    
    XQSharableFile sf;
    XQAiwRequest* request = 0;

    if ( !sf.open(fileName) ) 
        {
        return;
        }

    // Get handlers
    XQApplicationManager appManager;
    QList<XQAiwInterfaceDescriptor> fileHandlers = appManager.list(sf);
    if (fileHandlers.count() > 0)
        {
        XQAiwInterfaceDescriptor d = fileHandlers.first();
        request = appManager.create(sf, d);
    
        if ( !request )
            {
            sf.close();
            return ;
            }
        }
    else
        {
        sf.close();
        return;
        }

    // Result handlers
    connect (request, SIGNAL(requestOk(const QVariant&)), 
            this, SLOT(handleOk(const QVariant&)));
    connect (request, SIGNAL(requestError(const QVariant&)), 
            this, SLOT(handleError(const QVariant&)));

    request->setEmbedded(true);
    request->setSynchronous(true);

    // Fill args
    QList<QVariant> args;
    args << qVariantFromValue(sf);
    request->setArguments(args);

    // Fill headers
    QString key("WindowTitle");
    QVariant value(QString(LOC_TITLE));
    XQRequestInfo info;
    info.setInfo(key, value);
    request->setInfo(info);
    
    request->send();
    
    // Cleanup
    sf.close();
    delete request;
}

void MsgUnifiedEditorBody::viewDetails()
{
    //open details view.
}

bool MsgUnifiedEditorBody::hasImage()
{
    return mHasImage;
}

void MsgUnifiedEditorBody::setImage(bool image)
{
    mHasImage = image;
}

bool MsgUnifiedEditorBody::hasAudio()
{
    return mHasAudio;
}

void MsgUnifiedEditorBody::setAudio(bool audio)
{
    mHasAudio = audio;
}

int MsgUnifiedEditorBody::bodySize()
{
	int bodysize = 0;
	
	if( mImageSize || mTextEdit->toPlainText().size() || 
	    mAudioSize || mVideoSize )
	{
	   UniEditorGenUtils* uniEditorGenUtils = new UniEditorGenUtils;
        bodysize +=  mImageSize + mAudioSize + mVideoSize +
            uniEditorGenUtils->UTF8Size(mTextEdit->toPlainText()) +
            KEstimatedMimeHeaderSize + KEstimatedMmsSmilHeaderSize;
        delete uniEditorGenUtils;
	}
	return bodysize;
}

int MsgUnifiedEditorBody::messageSize()
{
    int estimatedMediaSize = bodySize();
    if(!estimatedMediaSize)
    {
        // This is the first media content to be inserted
        estimatedMediaSize = KEstimatedMmsSmilHeaderSize;
    }
    
    return estimatedMediaSize + MsgMonitor::subjectSize() + MsgMonitor::containerSize();
}

QString MsgUnifiedEditorBody::getHitRegion(QPointF position)
{
    if(mIconItem)
    {
        QPolygonF imageHitRegion = mIconItem->mapToScene(mIconItem->boundingRect());
        if(imageHitRegion.containsPoint(position, Qt::OddEvenFill))
        {
            return IMAGE_REGION;
        }
    }

    if(mAudioItem)
    {
        QPolygonF audioHitRegion = mAudioItem->mapToScene(mAudioItem->boundingRect());
        if(audioHitRegion.containsPoint(position, Qt::OddEvenFill))
        {
            return AUDIO_REGION;
        }
    }

    //TODO : add hit test for video region with video userstory

    return INVALID_REGION;
}

void MsgUnifiedEditorBody::onTextChanged()
{   
    QString string = text();
    
    if(  !mPrevBuffer.isEmpty() &&
         string.size() > mPrevBuffer.size() &&
         MsgMonitor::messageType() == ConvergedMessage::Mms )
    {
        //Save the previous buffer
        mPrevBuffer = string;
        // emit signal to indicate change in content
        emit contentChanged();
                
        return;
    }

    //Check done for optimization
    //Only if content is deleted we need to call encodingsettings again
    if (mPrevBuffer.isEmpty() || string.size() <= mPrevBuffer.size())
    {
        mPluginInterface->setEncodingSettings(EFalse, ESmsEncodingNone,
            mCharSupportType);
    }

    TInt numOfRemainingChars;
    TInt numOfPDUs;
    TBool unicodeMode;
    TSmsEncoding alternativeEncodingType;
    mPluginInterface->getNumPDUs(string, numOfRemainingChars, numOfPDUs,
        unicodeMode, alternativeEncodingType);

    //Save the unicode value returned
    mUnicode = unicodeMode;
    //Save the old buffer
    mPrevBuffer = string;
    
    // emit signal to indicate change in content
    emit contentChanged();
    
    if(MsgMonitor::messageType() == ConvergedMessage::Sms)
    {
        //Set char counter value
        QString display = QString("%1(%2)").arg(numOfRemainingChars).arg(
            numOfPDUs);
        mCharCounter->setText(display);
        
        if (numOfPDUs > 1 || numOfRemainingChars <= KShowCounterLimit)
        {
            mCharCounter->setVisible(true);
            mBackgroundItem->setVisible(true);
        }
        else
        {
            mCharCounter->setVisible(false);
            mBackgroundItem->setVisible(false);
        }
    }
}

void MsgUnifiedEditorBody::EditorOperationEvent(
    TUniEditorProcessImageOperationEvent aEvent, TFileName aFileName)
{
    stopResizeAnimation();
    
    delete mImageInfo;
    mImageInfo = NULL;

    if (aEvent == EUniEditorProcessImageOperationComplete && 
            aFileName.Length() > 0)
    {
        mImageFile = S60QConversions::s60DescToQString(aFileName);
    }
    else
    {
        mImageFile = mSavedImageFile;
    }
    mSavedImageFile.clear();
    //handle the processed image from ProcessImage Operation
    handleSetImage();
}

void MsgUnifiedEditorBody::startResizeAnimation()
{
    HbIconAnimationManager *manager = HbIconAnimationManager::global();
    bool defined = manager->addDefinitionFile(":/animation.axml");

    HbIcon animIcon;
    animIcon.setIconName("frame_anim_looping");

    mIconItem = new HbIconItem(this);
    mIconItem->hide();
    HbStyle::setItemName(mIconItem,"pixmap");
    mIconItem->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    mIconItem->setIcon(animIcon);

    mImageFile = ANIMATION_ICON;
    HbIconAnimator animator;
    animator.setIcon(animIcon);

    animator.startAnimation();
    this->repolish();
}

void MsgUnifiedEditorBody::stopResizeAnimation()
{
    if (mIconItem)
    {
        delete mIconItem;
        mIconItem = NULL;     
    }   
}

// ---------------------------------------------------------
// MsgUnifiedEditorBody::isUnicode
// ---------------------------------------------------------
//
bool MsgUnifiedEditorBody::isUnicode()
{
    return mUnicode;
}

// ---------------------------------------------------------
// MsgUnifiedEditorBody::disableCharCounter
// ---------------------------------------------------------
//
void MsgUnifiedEditorBody::disableCharCounter()
{
    mCharCounter->setVisible(false);
    mBackgroundItem->setVisible(false);
}

//---------------------------------------------------------------
// MsgUnifiedEditorBody :: handleOk
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorBody::handleOk(const QVariant& result)
{
    Q_UNUSED(result)
}

//---------------------------------------------------------------
// MsgUnifiedEditorBody :: handleError
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorBody::handleError(int errorCode, 
    const QString& errorMessage)
{
    Q_UNUSED(errorMessage)
    Q_UNUSED(errorCode)
}

// EOF
