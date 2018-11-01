/*******************************************************************************
 
INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2016 Intel Corporation. All Rights Reserved.
 
*******************************************************************************/
#pragma once

#ifndef HANDSCONTROLLER_H
#define	HANDSCONTROLLER_H

#include <thread>
#include <mutex>

#include "IModel.h"
#include "OpenGlView.h"

#include "timer.h"


namespace ModelViewController
{

	class HandsController
	{
	public:
		HandsController(IModel* model, IView* view);
		void updateView();
		~HandsController();
	private:
		IModel* m_model;
		IView *m_view;

		Tree<PointData>* m_skeletonTree;
		Tree<PointData>* m_skeletonTreeTmp;

		std::mutex m_mutex;

		FPSTimer m_timer;
	};

}

#endif	/* HANDSCONTROLLER_H */