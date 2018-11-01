#include "OpenGLView.h"

using namespace ModelViewController;

/******************************************************************************/
/*                                Defines                                     */
/******************************************************************************/

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, -5.0f, 0.0f }; 
const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f }; 

bool OpenGLView::m_isBones = false;
bool OpenGLView::m_isFullHand = false;
bool OpenGLView::m_stop = false;

Tree<PointData>* OpenGLView::m_skeletonTree = NULL;

std::vector<PXCPoint3DF32> OpenGLView::m_cursorPoints[2] = {};

bool OpenGLView::m_hasLeftHand = false;
bool OpenGLView::m_hasRightHand = false;

std::mutex OpenGLView::m_mutex;

bool OpenGLView::m_pause = false;

pxcBYTE* OpenGLView::m_image =  0;
Texture* OpenGLView::DepthTex = 0;

int OpenGLView::m_fps = 0;

unsigned int OpenGLView::m_cursorPointsCount = 30;

float m_cursorColorFactor = 0.05f;

bool m_internalStop = false;

Camera GameCamera;

//===========================================================================//

bool OpenGLView::pause()
{
	return m_pause;
}

//===========================================================================//

void OpenGLView::pauseView()
{
	m_pause = true;
	GameCamera.UserControl = false;
}

//===========================================================================//

bool OpenGLView::OnKeyboard(int Key)
{
	bool Ret = false;

	switch (Key) 
	{
	// Pause
	case GLUT_KEY_F1:
		{		
			m_pause = !m_pause;
			GameCamera.UserControl = false;
			Ret = true;
			break;
		} 
	// Remove balls
	case GLUT_KEY_F3:
		{		
			if(m_cursorPointsCount != 30)
			{
				m_cursorPointsCount -= 30;
				m_cursorColorFactor += 0.01f;
			}
			Ret = true;
			break;
		} 
	// Add balls
	case GLUT_KEY_F4:
		{	
			if(m_cursorColorFactor >= 0.01)
			{
				m_cursorPointsCount += 30;
				m_cursorColorFactor -= 0.01f;
			}

			Ret = true;
			break;
		}
		// Auto Rotate Camera
	case GLUT_KEY_F5:
		{	
			GameCamera.UserControl = false;
			Ret = true;
			break;
		} 
	}

	return Ret;
}

//===========================================================================//

void OpenGLView::SpecialKeyboardCB(int Key, int x, int y)
{
	OnKeyboard(Key);
}


//===========================================================================//

static void mouseDownCB(int Key, int state, int x, int y)
{
	GameCamera.OnMouseDown(Key, state, x, y);
}

//===========================================================================//

static void mouseMoveCB(int x, int y)
{
	GameCamera.OnMouseMove(x,y);
}

//===========================================================================//

PXCPoint4DF32 quat(PXCPoint4DF32 v, bool flip = false) {
	if(flip)
	{
		v.x *= -1.f;
		v.w *= -1.f;
	}

	return v;
}

//===========================================================================//

PXCPoint3DF32 vec(PXCPoint3DF32 v) {
	return v;
}

//===========================================================================//

inline PXCPoint3DF32 transform(PXCPoint3DF32 v) {
	float z = v.z;
	z = 1 - z;
	v.z = z;
	return v;
}

//===========================================================================//
void quaternionMat(PXCPoint4DF32 q,float mat4x4[4][4])
{

	mat4x4[0][0] = 1 - 2 * q.y * q.y - 2 * q.z * q.z;
	mat4x4[0][1] = 2 * q.x * q.y + 2 * q.w * q.z;
	mat4x4[0][2] = 2 * q.x * q.z - 2 * q.w * q.y;
	mat4x4[0][3] = 0;

	mat4x4[1][0] = 2 * q.x * q.y - 2 * q.w * q.z;
	mat4x4[1][1] = 1 - 2 * q.x * q.x - 2 * q.z * q.z;
	mat4x4[1][2] = 2 * q.y * q.z + 2 * q.w * q.x;
	mat4x4[1][3] = 0;

	mat4x4[2][0] = 2 * q.x * q.z + 2 * q.w * q.y;
	mat4x4[2][1] = 2 * q.y * q.z - 2 * q.w * q.x;
	mat4x4[2][2] = 1 - 2 * q.x * q.x - 2 * q.y * q.y;
	mat4x4[2][3] = 0;

	mat4x4[3][0] = 0;
	mat4x4[3][1] = 0;
	mat4x4[3][2] = 0;
	mat4x4[3][3] = 1;
}

//===========================================================================//

void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions,GLUquadricObj *quadric,PXCPoint4DF32 rotation)
{
	glColor3f(0.8f, 0.8f , 0.8f );

	float vx = x2-x1;
	float vy = y2-y1;
	float vz = z2-z1;

	//handle the degenerate case of z1 == z2 with an approximation
	if(vz == 0)
		vz = .00000001f;

	float distance = sqrt( vx*vx + vy*vy + vz*vz );

	PXCPoint3DF32 p;
	p.x = x1;
	p.y = y1;
	p.z = z1;

	const PXCPoint3DF32 p0 = vec(p);

	glPushMatrix();
	{
		PXCPoint3DF32 v1 = transform(p0);
		//draw the cylinder body
		glTranslatef(v1.x, v1.y, v1.z);
		PXCPoint4DF32 q = quat(rotation, true);

		float mat4x4[4][4];
		quaternionMat(q,mat4x4);

		glMultMatrixf(&mat4x4[0][0]);
		glRotatef(90, 1, 0, 0);

		float fixDistanceFactor = 0.f;//0.009f;

		gluQuadricOrientation(quadric,GLU_OUTSIDE);
		gluCylinder(quadric, radius, 1.5f * radius, distance - fixDistanceFactor, 16, 2);

	}
	glPopMatrix();
}

//===========================================================================//

void renderCylinder_convenient(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions,PXCPoint4DF32 rotation)
{
	//the same quadric can be re-used for drawing many cylinders
	GLUquadricObj *quadric=gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);
	renderCylinder(x1,y1,z1,x2,y2,z2,radius,subdivisions,quadric,rotation);
	gluDeleteQuadric(quadric);
}

//===========================================================================//

void displayCylinder(const PXCPoint3DF32 firstVec,const PXCPoint3DF32 secondVec,PXCPoint4DF32 rotation)
{
	float x1 = firstVec.x;
	float y1 = firstVec.y;
	float z1 = firstVec.z;
	float x2 = secondVec.x;
	float y2 = secondVec.y;
	float z2 = secondVec.z;

	float radius = 0.005f;

	//render the cylinder
	renderCylinder_convenient(x1,y1,z1,x2,y2,z2,radius,32,rotation);
}

//===========================================================================//

static void resize(int width, int height)
{
	const float ar = (float) width / (float) height; 

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(70.f, ar, 0.1f, 1000.f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity() ;
}
//===========================================================================//

static void CreateScenario()
{
	glClearColor(0.3f, 0.5f, 0.9f,1.f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS); 

	glEnable(GL_LIGHT0);

	glEnable(GL_NORMALIZE);

	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING); 

	glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);

	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);

	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

//===========================================================================//

void timer(int value)
{
	glutTimerFunc(25,timer,value++);
	if(!m_internalStop)
		glutPostRedisplay();
}

/******************************************************************************/
/*                          Class Implementation                              */
/******************************************************************************/

void OpenGLView::RenderSceneCB()
{
	if(m_stop)
		return;

	m_mutex.lock();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	// Display Mirror
	glTranslatef(0.f,0.f,-0.9f);
	glRotatef(180.f,0.f,1.f,0.f);

	// Hands rendering preparation
	float mat4x4[16];

	float rightHandWristX = m_skeletonTree[0].getRoot().getNodeValue().positionWorld.x;
	float rightHandWristY = m_skeletonTree[0].getRoot().getNodeValue().positionWorld.y;
	float rightHandWristZ = m_skeletonTree[0].getRoot().getNodeValue().positionWorld.z;

	float leftHandWristX = m_skeletonTree[1].getRoot().getNodeValue().positionWorld.x;
	float leftHandWristY = m_skeletonTree[1].getRoot().getNodeValue().positionWorld.y;
	float leftHandWristZ = m_skeletonTree[1].getRoot().getNodeValue().positionWorld.z;

	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	if(m_isFullHand)
	{
		if(m_hasLeftHand && m_hasRightHand)
		{
			x = (rightHandWristX + leftHandWristX) / 2;
			y = (rightHandWristY + leftHandWristY) / 2;
			z = (rightHandWristZ + leftHandWristZ) / 2;
		}
		else if(m_hasRightHand)
		{
			x = rightHandWristX; 
			y = rightHandWristY;
			z = rightHandWristZ;
		}
		else if(m_hasLeftHand)
		{
			x = leftHandWristX;
			y = leftHandWristY;
			z = leftHandWristZ;
		}
	}

	glLightfv(GL_LIGHT0, GL_POSITION, light_position); 
	GameCamera.computeGlobalTransformation(0,0,0);
	GameCamera.getGlobalTransformation(mat4x4);

	// Draw Hands or Cursor
	glPushMatrix();
	{
		if ( false == (m_hasRightHand && m_hasLeftHand) )
		{
			glTranslatef(x, y, -z);
		}
		else
		{
			glTranslatef(0, 0, -z);
		}

		glMultMatrixf(mat4x4);

		if ( true == (m_hasRightHand && m_hasLeftHand) )
		{
			glTranslatef(x, y, z);
		}

		if(m_hasRightHand) 
			draw3DSkeleton(0, m_hasRightHand && m_hasLeftHand);
		else
			m_cursorPoints[0].clear();
		if(m_hasLeftHand)
			draw3DSkeleton(1, m_hasRightHand && m_hasLeftHand);
		else
			m_cursorPoints[1].clear();
	}
	glPopMatrix();

	// Draw Axis
	glPushMatrix();
	{
		glDisable(GL_LIGHTING);
		glTranslatef(0.6f, -0.4f, 0);
		glMultMatrixf(mat4x4);
		glLineWidth(1.3f);
		glBegin(GL_LINES);
		{
			glColor3f(1.0f,0.0f,0.0f);
			glVertex3f(0,0,0);
			glVertex3f(0.1f,0,0);

			glColor3f(0.0f,0.0f,1.0f);
			glVertex3f(0,0,0);
			glVertex3f(0,0.1f,0);

			glColor3f(0.0f,1.0f,0.0f);
			glVertex3f(0,0,0);
			glVertex3f(0,0,0.1f);
		}
		glEnd();
		

		// Drawing Floor
		float factor = 0.25f;
		for(int i = -10 ; i <= 10 ; ++i)
		{
			glColor3f(0.0f,0.0f,1.0f);
			glBegin(GL_LINES);
			{
				glVertex3f(i*factor,-2*factor,-10*factor);
				glVertex3f(i*factor,-2*factor,10*factor);					
				glVertex3f(-10*factor,-2*factor,i*factor);
				glVertex3f(10*factor,-2*factor,i*factor);
			}
			glEnd();
		}
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.75f,0.75f,0.75f,0.5f);
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();

	// Draw 2D Image
	if(m_image)
	{
		DepthTex->Update(m_image);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, DepthTex->id());
		glColor3ub(255, 255, 255);

		glPushMatrix();
		{
			const float s = 1.f;
			const float r = 640.f / 480.f;
			glScalef(s * r, s, s);
			glTranslatef(-2.0f,-1.5f,3.f);

			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);glVertex2f(-1.0,1.0);
			glTexCoord2f(1.0, 0.0);glVertex2f(1.0, 1.0);
			glTexCoord2f(1.0, 1.0);glVertex2f(1.0, -1.0);
			glTexCoord2f(0.0, 1.0);glVertex2f(-1.0, -1.0);
			glEnd();

			glLineWidth(6.f * r * s);
		}
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
	}


	printInstructions();
	drawFps();
	if(m_pause)
	{
		static int blink = 0;
		static bool blinking = false;
		static int blinkCounter = 0;

		if(blink % 50 == 0 || blinking)
		{
			drawPause();
			blink = 1;
			blinking = true;
		}

		if(blinking)
		{
			if(++blinkCounter == 20)
			{
				blinking = false;
				blinkCounter = 0;
			}
		}
		else
		{
			blink++;	
		}

		if(!GameCamera.UserControl)
		{
			GameCamera.OnMouseMove(GameCamera.autoRotateX++,0);
		}
	}

	glutSwapBuffers();

	m_mutex.unlock();
}


//===========================================================================//

void OpenGLView::drawPause()
{
	glDisable(GL_LIGHTING);
	std::string line = "--- PAUSE ---";
	glColor3f( 1.f, 0.f, 0.f );
	renderBitmapString(0.1f, 0.5f,GLUT_BITMAP_TIMES_ROMAN_24,line.c_str());
	glEnable(GL_LIGHTING);
}

//===========================================================================//
void OpenGLView::displayFps(pxcI32 fps)
{
	m_fps = fps;
}

//===========================================================================//

void OpenGLView::drawFps()
{
	std::string line = "FPS = ";
	line.append(std::to_string(m_fps));
	glPushAttrib(GL_CURRENT_BIT);
	glDisable(GL_LIGHTING);
	glColor3f(1.0,0.f,0.f);
	renderBitmapString(-0.7f, 0.49f,GLUT_BITMAP_8_BY_13,line.c_str());
	glEnable(GL_LIGHTING);
}


//===========================================================================//

void OpenGLView::printInstructions()
{
	glColor3f( 0.f, 0.f, 0.f );
	renderBitmapString(0.9f, 0.49f,GLUT_BITMAP_8_BY_13,"Instructions:");
	renderBitmapString(0.9f, 0.46f,GLUT_BITMAP_8_BY_13,"-------------");
	renderBitmapString(0.9f, 0.43f,GLUT_BITMAP_8_BY_13,"Full Hands Mode: Via Command Line - Run \"FF_Hands3DViewer.exe -full\"");
	renderBitmapString(0.9f, 0.40f,GLUT_BITMAP_8_BY_13,"Left Mouse Button - Hold to rotate view");
	renderBitmapString(0.9f, 0.37f,GLUT_BITMAP_8_BY_13,"Right Mouse Button - Click to reset view");
	renderBitmapString(0.9f, 0.34f,GLUT_BITMAP_8_BY_13,"Mouse Wheel Up/Down - Zoom in/out");	
	renderBitmapString(0.9f, 0.31f,GLUT_BITMAP_8_BY_13,"F1 - Pause/Play");
	renderBitmapString(0.9f, 0.28f,GLUT_BITMAP_8_BY_13,"F3 - <Cursor Only> Shorten trail");
	renderBitmapString(0.9f, 0.25f,GLUT_BITMAP_8_BY_13,"F4 - <Cursor Only> Extend trail");
}

//===========================================================================//

void OpenGLView::renderBitmapString(float x, float y, void *font,const char *string){
	const char *c;
	glRasterPos2f(x, y);
	for (c=string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

//===========================================================================//

void OpenGLView::InitializeGlutCallbacks()
{
	glutDisplayFunc(this->RenderSceneCB);
	glutSpecialFunc(SpecialKeyboardCB);
	glutMouseFunc(mouseDownCB);
	glutMotionFunc(mouseMoveCB);
}

//===========================================================================//

void OpenGLView::switchTrackingMode()
{
	m_isFullHand = !m_isFullHand;
}

//===========================================================================//

OpenGLView::OpenGLView(bool isFullHand)
{
	m_isFullHand = isFullHand;
}

//===========================================================================//

void OpenGLView::renderScene()
{
	glutMainLoop();
	m_stop = true;
}

//===========================================================================//

bool OpenGLView::stop()
{
	return m_stop;
}

//===========================================================================//

void::OpenGLView::close()
{
	m_stop = true;
	m_internalStop = true;
}

//===========================================================================//

void OpenGLView::init()
{
	char *myargv [1];
	int myargc=1;
	myargv [0]="3dHandsViewer";
	glutInit(&myargc, myargv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 
	glutInitWindowSize(1000, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("3D Hands Viewer");

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutCloseFunc(close);  

	m_stop = false;

	glutTimerFunc(25,timer,0);
	glutReshapeFunc(resize);
	InitializeGlutCallbacks();
	glutLeaveMainLoop();


	// Must be done after glut is initialized!
	GLenum res = glewInit();
	if (res != GLEW_OK) 
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
	}

	// 2d texture of depth image
	DepthTex = (new Texture(640, 480, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE))->Create(0);
	pxcBYTE* emptyDepth = new pxcBYTE[DepthTex->width * DepthTex->height * 4]();
	std::memset(emptyDepth, 0, DepthTex->width * DepthTex->height * 4);
	DepthTex->Update(emptyDepth);
	delete[] emptyDepth;

	CreateScenario();
}

//===========================================================================//

void OpenGLView::recursiveDrawBones(Node<PointData> node, PXCPoint3DF32 pGlobal)
{
	if(node.getChildNodes().size() == 0)
		return;

	glPushMatrix();
	{
		//TODO - hack
		if(!m_isBones)
		{
			PXCPoint3DF32 v0 = transform(pGlobal);
			glTranslatef(-v0.x, -v0.y, -v0.z);
			m_isBones = true;
		}


		PXCPoint3DF32 p0 = vec(node.getNodeValue().positionWorld);
		for(int i = 0 ; i < node.getChildNodes().size() ; ++i)
		{
			PXCPoint3DF32 p1 = vec(node.getChildNodes().at(i).getNodeValue().positionWorld);
			displayCylinder(p0,p1,node.getChildNodes().at(i).getNodeValue().globalOrientation);
			recursiveDrawBones(node.getChildNodes().at(i),pGlobal);
		}		
	}
	glPopMatrix();
}

//===========================================================================//

void OpenGLView::drawBones(int index, bool applyTransformFlag)
{
	PXCPoint3DF32 pGlobal;
	if ( false == applyTransformFlag )
	{
		pGlobal = vec(m_skeletonTree[index].getRoot().getNodeValue().positionWorld);
	}
	else
	{
		pGlobal.x = 0.0f;
		pGlobal.y = 0.0f;
		pGlobal.z = 0.0f;
	}

	recursiveDrawBones(m_skeletonTree[index].getRoot(),pGlobal);

	m_isBones = false;

}

void OpenGLView::recursiveDrawJoints(Node<PointData> node,PXCPoint3DF32 pGlobal)
{
	const PXCPoint3DF32 p0 = vec(node.getNodeValue().positionWorld);

	glPushMatrix();
	{
		PXCPoint3DF32 v0 = transform(pGlobal);
		glTranslatef(-v0.x, -v0.y, -v0.z);

		PXCPoint3DF32 v1 = transform(p0);
		glTranslatef(v1.x, v1.y, v1.z);

		glutSolidSphere(0.008,50,50);
	}
	glPopMatrix(); 

	if(node.getChildNodes().size() == 0)
		return;

	for(int i = 0 ; i < node.getChildNodes().size() ; ++i)
	{
		recursiveDrawJoints(node.getChildNodes().at(i),pGlobal);
	}

}

//===========================================================================//

void OpenGLView::drawJoints(int index, bool applyTransformFlag)
{
	PXCPoint3DF32 pGlobal;
	if ( false == applyTransformFlag )
	{
		pGlobal = vec(m_skeletonTree[index].getRoot().getNodeValue().positionWorld);
	}
	else
	{
		pGlobal.x = 0.0f;
		pGlobal.y = 0.0f;
		pGlobal.z = 0.0f;
	}

	if(m_isFullHand)
	{
		recursiveDrawJoints(m_skeletonTree[index].getRoot(),pGlobal);
	}
	else
	{
		if(!m_pause)
		{
			if (m_cursorPoints[index].size() > m_cursorPointsCount)
				m_cursorPoints[index].erase(m_cursorPoints[index].begin());
			m_cursorPoints[index].push_back(m_skeletonTree[index].getRoot().getNodeValue().positionWorld);
		}
		drawCursorPoints(index);
	}
}

//===========================================================================//

void OpenGLView::drawCursorPoints(int index)
{
	PXCPoint3DF32 pGlobal = {0.f,0.f,0.f};

	for(int j = 0 ; j < m_cursorPoints[index].size() ; ++j)
	{
		const PXCPoint3DF32 p0 = vec(m_cursorPoints[index][j]);

		glPushMatrix();
		{
			PXCPoint3DF32 v0 = transform(pGlobal);
			glTranslatef(-v0.x, -v0.y, -v0.z);

			PXCPoint3DF32 v1 = transform(p0);
			glTranslatef(v1.x, v1.y, v1.z);

			glutSolidSphere(0.008,50,50);

			if(index == 0)
			{
				//Here
				glColor3f( 1.0, j * m_cursorColorFactor, 0.0 );
			}
			else
			{			
				glColor3f( 0.0, j * m_cursorColorFactor, 1.0 );
			}
		}
		glPopMatrix(); 
	}
}


//===========================================================================//

void OpenGLView::draw3DSkeleton(int index, bool applyTransformFlag)
{
	if(index == 0)
	{
		glColor3f( 1.0, 0.0, 0.0 );
	}
	else
	{			
		glColor3f( 0.0, 0.0, 1.0 );
	}

	drawJoints(index, applyTransformFlag);
	if(!m_isFullHand) return;
	drawBones(index, applyTransformFlag);
}

//===========================================================================//

void OpenGLView::display3DSkeleton(Tree<PointData>* skeletonTree,bool hasLeftHand, bool hasRightHand)
{
	m_hasLeftHand = hasLeftHand;
	m_hasRightHand = hasRightHand;
	m_mutex.lock();
	m_skeletonTree = skeletonTree;
	m_mutex.unlock();
}


//===========================================================================//

void OpenGLView::display2DImage(pxcBYTE* image,pxcI32 width,pxcI32 height)
{
	if(!image)
		return;

	int bufferSize = width * height * 4;

	if(!m_image)
		m_image = new pxcBYTE[bufferSize];

	memcpy_s(m_image, bufferSize, image, bufferSize);
}

//===========================================================================//

void OpenGLView::display3DSpace() {}

//===========================================================================//

OpenGLView::~OpenGLView() 
{

}


