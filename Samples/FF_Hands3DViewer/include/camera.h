/*******************************************************************************
 
INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2015 Intel Corporation. All Rights Reserved.
 
*******************************************************************************/
#pragma once

#ifndef CAMERA_H
#define	CAMERA_H

#include "pxcsession.h"

class Camera
{
public:

    Camera();

	void OnMouseDown(int button, int state, int x, int y) ;
	void OnMouseMove(int x, int y);

	const float GetZoom() const
	{
		return m_zoom;
	}

	void getGlobalTransformation(float mat4x4[16]);
	void computeGlobalTransformation( float x,float y,float z );
	void resetGlobalTransformation();
	
	PXCPoint4DF32 rotation;
	PXCPoint3DF32 pos;
	PXCPointF32 lastMouse;

	static bool UserControl;
	static int autoRotateX;

private:
	int m_lastPosX;
	int m_lastPosY;

	float m_xMov;
	float m_yMov;
	float m_zMov;

	int m_scaleFactor,m_xTrans,m_yTrans;
	float m_zoomFactor;

	float m_zoom;
	int oldX, oldY;
	bool rotate;

	void indentity(float mat4x4[16]);
	void rotX(float angle,float mat4x4[16]);
	void rotY(float angle,float mat4x4[16]);
	void rotZ(float angle,float mat4x4[16]);

	void multMat(float lhsmat4x4[16],float rhsmat4x4[16],float outmat4x4[16]) const;

	float rad2deg(float rad);

	void setXRotation(const float angle);
	void setYRotation(const float angle);
	void setZRotation(const float angle);

	void scale(const float x, const float y, const float z,float mat4x4[16]);

	float m_rotMatrix[16];

	
	
	float m_globalTransformation[16];

	void translate(const float x, const float y, const float z,float mat4x4[16]);

	
	
};

#endif	/* CAMERA_H */

