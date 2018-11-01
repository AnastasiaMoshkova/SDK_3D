#include "RssdkHandler.h"
#include "pxcmetadata.h"
#include "service/pxcsessionservice.h"

RssdkHandler::RssdkHandler(HandsController* handsController, IModel* model, IView* view)
{
	m_handsController = handsController;
	m_model = model;
	m_view = view;
}

pxcStatus RssdkHandler::Init(bool isFullHand, const pxcCHAR* sequencePath)
{
	// Error checking Status
	pxcStatus status = PXC_STATUS_INIT_FAILED;

	// Create the PXCSession
	m_session = PXCSession::CreateInstance();
	if(!m_session)
	{
		return status;
	}

	// Create the PXCSenseManager
	m_senseManager = m_session->CreateSenseManager();
	if(!m_senseManager)
	{
		return status;
	}

	// Load sequence from file
	if(sequencePath)
	{
		m_senseManager->QueryCaptureManager()->SetRealtime(false);
		status = m_senseManager->QueryCaptureManager()->SetFileName(sequencePath,false);
		if(status != PXC_STATUS_NO_ERROR)
		{
			return status;
		}
	}

	status = m_model->Init(m_senseManager);
	if(status != PXC_STATUS_NO_ERROR)
	{
		return status;
	}

	// initialize the PXCSenseManager
	status = m_senseManager->Init();
	if(status != PXC_STATUS_NO_ERROR)
	{
		return status;		
	}

	m_view->init();
	
	//If we got to this stage return success
	return PXC_STATUS_NO_ERROR;
}

//===========================================================================//

void RssdkHandler::Start()
{
	// Run camera frames and update information thread
	std::thread cameraThread(std::bind(&RssdkHandler::getFrame,this));

	//start rendering
	m_view->renderScene();
	
	cameraThread.join();

}

//===========================================================================//

bool RssdkHandler::getFrame()
{
	//Start working on current frame
	while(m_senseManager->AcquireFrame(true) == PXC_STATUS_NO_ERROR && !m_view->stop())
	{
		m_model->updateModel();

		m_handsController->updateView();

		//Finish Work on frame
		m_senseManager->ReleaseFrame();
	}
	return true;
}

//===========================================================================//

void RssdkHandler::release()
{
	if(m_senseManager)
	{
		m_senseManager->Close();
		m_senseManager->Release();
		m_senseManager = NULL;
	}

	if(m_session)
	{
		m_session->Release();
		m_session = NULL;
	}
}


//===========================================================================//

RssdkHandler::~RssdkHandler()
{
	release();
}