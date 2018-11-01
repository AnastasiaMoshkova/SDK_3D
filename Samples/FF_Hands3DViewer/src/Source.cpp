#include <stdio.h>
#include <iostream>

#include "RssdkHandler.h"
#include "HandsModel.h"
#include "CursorModel.h"

ModelViewController::IModel* model;
ModelViewController::IView* openGLView;
ModelViewController::HandsController* controller;
RssdkHandler* rssdkHandler;

void releaseAll();
bool start(bool isFullHand)
{
	pxcStatus status = PXC_STATUS_ALLOC_FAILED;

	if(isFullHand)
	{
		// Create hand model
		model = new ModelViewController::HandsModel();
	}
	else
	{
		// Create cursor model
		model = new ModelViewController::CursorModel();
	}

	if(!model)
	{
		return false;
	}

	// Create Openglview which implements IView (allows creations of different views)
	openGLView = new ModelViewController::OpenGLView(isFullHand);
	if(!openGLView)
	{
		return false;
	}

	// When using sequence, change useSequence to true and apply sequencePath with sequence path
	bool useSequence = false;
	pxcCHAR* sequencePath = L"Insert Sequence Path Here";

	// Bind controller with model and view and start playing
	controller = new ModelViewController::HandsController(model,openGLView);
	if(!controller)
	{
		return false;
	}

	rssdkHandler = new RssdkHandler(controller,model,openGLView);

	if(useSequence)
	{
		if(rssdkHandler->Init(isFullHand,sequencePath) == PXC_STATUS_NO_ERROR)
		{
			rssdkHandler->Start();
		}

		else
		{
			return false;
		}
	}

	// Live Camera
	else
	{
		status = rssdkHandler->Init(isFullHand);
		if(status == PXC_STATUS_NO_ERROR)
		{
			rssdkHandler->Start();
		}
		else
		{
			return false;
		}
	}

	return true;
}

int main(int argc, char** argv)
{
	bool isFullHand = false;
	if(argc == 2)
	{
		if (strcmp(argv[1],"-full")==0)
		{
			isFullHand = true;
		}
	}

	// Full hand mode
	if(isFullHand)
	{
		if(!start(isFullHand))
		{
			std::printf("Failed at Initialization\n");
			releaseAll();
		}
	}
	// Cursor hand
	else
	{
		if(!start(isFullHand))
		{
			releaseAll();
			if(!start(!isFullHand))
			{
				std::printf("Failed at Initialization\n");
				releaseAll();
			}			
		}
	}
	
	releaseAll();
    return 0;
}

void releaseAll()
{
	//delete all pointers
	if(model)
	{
		delete model;
		model = NULL;
	}
	if(openGLView)
	{
		delete openGLView;
		openGLView = NULL;
	}
	if(controller)
	{
		delete controller;
		controller = NULL;
	}
	if(rssdkHandler)
	{
		delete rssdkHandler;
		rssdkHandler = NULL;
	}
}


