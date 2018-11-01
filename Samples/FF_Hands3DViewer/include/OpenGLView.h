/*******************************************************************************
 
INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2016 Intel Corporation. All Rights Reserved.
 
*******************************************************************************/
#ifndef OPENGLVIEW_H
#define	OPENGLVIEW_H

#pragma once

#include "pxchandmodule.h"
#include "pxchanddata.h"
#include "pxchandconfiguration.h"

#include <iostream>
#include <mutex>
#include <string>

#include <glew.h>
#include <freeglut.h>

#include "IView.h"
#include "camera.h"
#include "Texture.hpp"

namespace ModelViewController
{
	class OpenGLView : public IView
	{
	public:
		OpenGLView(bool isFullHand = false);
		~OpenGLView();
		virtual void renderScene();
		virtual void display3DSkeleton(Tree<PointData>* skeletonTree,bool hasLeftHand, bool hasRightHand);
		virtual void display3DSpace();
		virtual void display2DImage(pxcBYTE* image,pxcI32 width,pxcI32 height);
		virtual void displayFps(pxcI32 fps);
		virtual void pauseView();
		virtual bool pause();
		virtual bool stop();
		virtual void switchTrackingMode();
		virtual void init();
		

	private:
		static void RenderSceneCB();
		void InitializeGlutCallbacks();
		static void draw3DSkeleton(int index, bool applyTransformFlag );
		static void drawBones(int index, bool applyTransformFlag);
		static void drawJoints(int index, bool applyTransformFlag);
		static void drawFps();
		static void drawPause();

		static void drawCursorPoints(int index);
		static void recursiveDrawJoints(Node<PointData> node, PXCPoint3DF32 pGlobal);
		static void recursiveDrawBones(Node<PointData> node, PXCPoint3DF32 pGlobal);

		static void renderBitmapString(float x, float y, void *font,const char *string);
		static void printInstructions();
		
		static bool OnKeyboard(int Key);
		static void SpecialKeyboardCB(int Key, int x, int y);
		static void setVSync(bool sync);

		static void close();

		static PointData m_skeletonsData[2][22];

		static pxcBYTE* m_image;
		static Texture* DepthTex;

		static bool m_hasLeftHand;
		static bool m_hasRightHand;

		static bool m_pause;
		static bool m_switchTrackingMode;

		static Tree<PointData>* m_skeletonTree;

		static std::vector<PXCPoint3DF32> m_cursorPoints[2];

		static std::mutex m_mutex;

		static bool m_isBones;

		static int m_fps;

		static bool m_stop;

		static bool m_isFullHand;

		static unsigned int m_cursorPointsCount;
	};
}

#endif	/* OPENGLVIEW_H */