/*******************************************************************************
 
INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2015 Intel Corporation. All Rights Reserved.
 
*******************************************************************************/

#ifndef TIMER_H
#define	TIMER_H

#pragma once
#include <windows.h>
#include "pxcimage.h"

class FPSTimer {
public:

	FPSTimer(void);
    int Tick();

protected:

    LARGE_INTEGER freq, last;
	int fps;
	int fpsOut;
};


#endif	/* TIMER_H */