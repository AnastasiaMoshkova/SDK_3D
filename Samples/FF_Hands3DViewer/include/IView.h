/*******************************************************************************
 
INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2016 Intel Corporation. All Rights Reserved.
 
*******************************************************************************/
#pragma once

#ifndef IVIEW_H
#define	IVIEW_H

#include "pxcsensemanager.h"

#include "Tree.h"

namespace ModelViewController
{
	class IView
	{
	public:
		virtual void renderScene() = 0;
		virtual void display3DSkeleton(Tree<PointData>* skeletonTree,bool hasLeftHand, bool hasRightHand) = 0;
		virtual void display3DSpace() = 0;
		virtual void displayFps(pxcI32 fps) = 0;
		virtual void display2DImage(pxcBYTE* image,pxcI32 width,pxcI32 height) = 0;
		virtual bool pause() = 0;
		virtual bool stop() = 0;
		virtual void pauseView() = 0;
		virtual void switchTrackingMode() = 0;
		virtual void init() = 0;
		virtual ~IView() {}
	};

}

#endif	/* IVIEW_H */