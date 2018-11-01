/*******************************************************************************
 
INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2016 Intel Corporation. All Rights Reserved.
 
*******************************************************************************/
#ifndef RSSDKHANDLER_H
#define	RSSDKHANDLER_H

#include <thread>
#include <iostream>

#include "pxcsession.h"
#include "pxcsensemanager.h"

#include "HandsController.h"


using namespace ModelViewController;

class RssdkHandler
{
public:
	RssdkHandler(HandsController* handsController, IModel* model, IView* view);
	~RssdkHandler();
	pxcStatus Init(bool isFullHand,const pxcCHAR* sequencePath = 0);
	void Start();
	bool getFrame();
	void release();
private:
	PXCSession *m_session;
	PXCSenseManager *m_senseManager;

	HandsController* m_handsController;
	IModel* m_model;
	IView* m_view;
};

#endif	/* RSSDKHANDLER_H */