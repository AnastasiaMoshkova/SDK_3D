/*******************************************************************************
 
INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2016 Intel Corporation. All Rights Reserved.
 
*******************************************************************************/
#pragma once

#ifndef IMODEL_H
#define	IMODEL_H

#include "Tree.h"

#define MAX_NUMBER_OF_HANDS 2

namespace ModelViewController
{
	class IModel
	{
	public:
		virtual pxcStatus Init(PXCSenseManager* senseManager) = 0;
		virtual void initSkeletonTree(Tree<PointData>* tree) = 0;
		virtual Tree<PointData>* getSkeletonTree() = 0;
		virtual void setSkeleton(Tree<PointData>* skeletonTree) = 0;
		virtual bool updateModel() = 0;
		virtual bool hasRightHand() = 0;
		virtual bool hasLeftHand() = 0;
		virtual bool get2DImage(pxcBYTE* depthmap) = 0;
		virtual pxcI32 get2DImageHeight() = 0;
		virtual pxcI32 get2DImageWidth() = 0;
		virtual bool isModelPaused() = 0;
		virtual void pause(bool isPause, bool isModel) = 0;
		virtual ~IModel() {}
	};

}

#endif	/* IMODEL_H */