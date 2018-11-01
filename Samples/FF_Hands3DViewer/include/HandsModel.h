/*******************************************************************************
 
INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2016 Intel Corporation. All Rights Reserved.
 
*******************************************************************************/
#pragma once

#ifndef HANDSMODEL_H
#define	HANDSMODEL_H

#include <thread>
#include <mutex> 
#include <iostream>

#include "pxcsession.h"
#include "pxcsensemanager.h"
#include "PXCHandConfiguration.h"
#include "pxchandcursormodule.h"

#include "IModel.h"

namespace ModelViewController
{
	class HandsModel : public IModel
	{
	public:
		HandsModel();
		HandsModel(const HandsModel& src);
		HandsModel& operator=(const HandsModel& src);
		~HandsModel();
		virtual pxcStatus Init(PXCSenseManager* senseManager);
		virtual void initSkeletonTree(Tree<PointData>* tree);
		virtual Tree<PointData>* getSkeletonTree();
		virtual void setSkeleton(Tree<PointData>* skeletonTree);
		virtual bool updateModel();
		virtual bool hasRightHand();
		virtual bool hasLeftHand();
		virtual bool get2DImage(pxcBYTE* depthmap);
		virtual pxcI32 get2DImageHeight();
		virtual pxcI32 get2DImageWidth();
		virtual bool isModelPaused();
		virtual void pause(bool isPause, bool isModel);

	private:
		void updateSkeleton();
		Tree<PointData>* m_skeletonTree;
		void updateskeletonTree();
		void update2DImage();
		PXCHandModule* m_handModule;
		PXCHandCursorModule* m_handCursorModule;
		PXCHandData* m_handData;
		PXCCursorData* m_cursorData;
		bool m_rightHandExist;
		bool m_leftHandExist;
		pxcBYTE* m_depthmap;
		pxcI32 m_imageHeight;
		pxcI32 m_imageWidth;
		bool m_fullHandMode;
		bool m_isPaused;
		bool m_gestureFired;
		PXCSenseManager* m_senseManager;
		void copyJointToPoint(PointData & dst,const PXCHandData::JointData & src);
	};

}

#endif	/* HANDSMODEL_H */