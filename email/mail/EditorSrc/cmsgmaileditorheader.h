/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Mail editor header content handler.
*
*/

#ifndef C_CMSGMAILEDITORHEADER_H
#define C_CMSGMAILEDITORHEADER_H

#include <e32base.h> // CBase
#include <MsgAttachmentModelObserver.h> // MMsgAttachmentModelObserver

class CMsgMailEditorDocument;
class CMsgEditorView;
class TAdditionalHeaderStatus;

/**
 *  Mail Editor UI header field handler.
 *
 *  Manipulates content of mail editor header fields.
 *
 *  @lib none
 *  @since S60 v3.1
 */
class CMsgMailEditorHeader : public CBase, public MMsgAttachmentModelObserver
    {
public:

    /**
     * NewL
     * Creates mail editor header fields
     * @since S60 v3.1
     * @param aDocument reference to editor document.
     * @param aView reference to editor view.
     * @return self
     */
    static CMsgMailEditorHeader* NewL(
        CMsgMailEditorDocument& aDocument,
        CMsgEditorView& aView );

    ~CMsgMailEditorHeader();

    /**
     * Update visibility of mail editor address fields.
     *
     * @since S60 v3.1
     */    
    void UpdateHeaderVisibilityL(
        RPointerArray<TAdditionalHeaderStatus>& aHeaders );
    
    /**
     * Update visibility of mail editor Reply-to field.
     *
     * @since S60 v3.2
     */        
    void UpdateReplyToControlL();    

// from base class MMsgAttachmentModelObserver
    void NotifyChanges( 
        TMsgAttachmentCommand aCommand, 
        CMsgAttachmentInfo* aAttachmentInfo );
    RFile GetAttachmentFileL( TMsvAttachmentId aId );

private:

    CMsgMailEditorHeader(
        CMsgMailEditorDocument& aDocument,
        CMsgEditorView& aView);
    void ConstructL();

private: // implementation    
    
    void AddControlL( TInt aControlType );
    CMsgAttachmentControl* AttachmentControl() const;
    void CreateHeadersL();    
    void DoCreateAttachmentControlL();
    void DoRemoveAttachmentControlL();
    void UpdateAttachmentControlL( 
        TMsgAttachmentCommand aCommand,
        CMsgAttachmentInfo* aAttachmentInfo );

private: // data

    /**
     * Reference to mail editor document
     */
    CMsgMailEditorDocument& iDocument;
    
    /**
     * Reference to mail editor view
     */    
    CMsgEditorView& iView;
    };

#endif // C_CMSGMAILEDITORHEADER_H
