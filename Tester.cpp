////////////////////////////////////////
// Tester.cpp
////////////////////////////////////////

#include "Tester.h"

////////////////////////////////////////////////////////////////////////////////

static Tester *TESTER=0;

int main(int argc, char **argv) {
	glutInit(&argc, argv);

	TESTER=new Tester("Project 4",argc,argv);
	glutMainLoop();
	delete TESTER;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

// These are really HACKS to make glut call member functions instead of static functions
static void display()									{TESTER->Draw();}
static void idle()										{TESTER->Update();}
static void resize(int x,int y)							{TESTER->Resize(x,y);}
static void keyboard(unsigned char key,int x,int y)		{TESTER->Keyboard(key,x,y);}
static void mousebutton(int btn,int state,int x,int y)	{TESTER->MouseButton(btn,state,x,y);}
static void mousemotion(int x, int y)					{TESTER->MouseMotion(x,y);}
static void specialKeys(int key, int x, int y)			{ TESTER->SpecialKeys(key, x, y); }

////////////////////////////////////////////////////////////////////////////////

Tester::Tester(const char *windowTitle,int argc,char **argv) {
	WinX = 800;
	WinY = 600;
	LeftDown=MiddleDown=RightDown=false;
	MouseX=MouseY=0;

	// Create the window
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( WinX, WinY );
	glutInitWindowPosition( 10, 10 );
	WindowHandle = glutCreateWindow( windowTitle );
	glutSetWindowTitle( windowTitle );
	glutSetWindow( WindowHandle );

	// Background color
	glClearColor( 0.1f, 0.1f, 0.1f, 1.f );

	// Callbacks
	glutDisplayFunc( display );
	glutIdleFunc( idle );
	glutKeyboardFunc( keyboard );
	glutSpecialFunc(specialKeys);
	glutMouseFunc( mousebutton );
	glutMotionFunc( mousemotion );
	glutPassiveMotionFunc( mousemotion );
	glutReshapeFunc( resize );

	// Initialize GLEW
	glewInit();
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	// Initialize components
	shader = new ShaderProgram("Model.glsl", ShaderProgram::eRender);
	cam=new Camera;
	cam->SetAspect(float(WinX)/float(WinY));

	glm::mat4 toWorld = glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0));
	toWorld = glm::translate(toWorld, glm::vec3(-1.5, 1.5, 0));
	cloth = new Cloth(3, 3, 20, 20, 0.1f, 20, 0.01f, toWorld);
}

////////////////////////////////////////////////////////////////////////////////

Tester::~Tester() {
	delete shader;
	delete cam;

	glFinish();
	glutDestroyWindow(WindowHandle);
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Update() {
	// Update the components in the world
	cam->Update();
	cloth->Update();

	// Tell glut to re-display the scene
	glutSetWindow(WindowHandle);
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Reset() {
	cam->Reset();
	cam->SetAspect(float(WinX)/float(WinY));
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Draw() {
	// Begin drawing scene
	glViewport(0, 0, WinX, WinY);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cloth->Draw(cam->GetViewProjectMtx(), shader->GetProgramID());

	// Finish drawing scene
	glFinish();
	glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Quit() {
	glFinish();
	glutDestroyWindow(WindowHandle);
	exit(0);
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Resize(int x,int y) {
	WinX = x;
	WinY = y;
	cam->SetAspect(float(WinX)/float(WinY));
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Keyboard(unsigned char key,int x,int y) {
	switch(key) {
		case 0x1b:		// Escape
			Quit();
			break;
		case 'r':
			Reset();
			break;
		case '-':
			cloth->windSpeed -= 1.0f;
			cout << "wind speed: " << cloth->windSpeed << endl;
			break;
		case '=':
			cloth->windSpeed += 1.0f;
			cout << "wind speed: " << cloth->windSpeed << endl;
			break;
		case 'w':
			cloth->Move(glm::vec3(0, 0.01f, 0));
			break;
		case 's':
			cloth->Move(glm::vec3(0, -0.01f, 0));
			break;
	}
}


void Tester::SpecialKeys(unsigned char key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {
		cloth->Move(glm::vec3(0.01f, 0, 0));
	}
	if (key == GLUT_KEY_RIGHT) {
		cloth->Move(glm::vec3(-0.01f, 0, 0));
	}
	if (key == GLUT_KEY_UP) {
		cloth->Move(glm::vec3(0, 0, 0.01f));
	}
	if (key == GLUT_KEY_DOWN) {
		cloth->Move(glm::vec3(0, 0, -0.01f));
	}
}

////////////////////////////////////////////////////////////////////////////////

void Tester::MouseButton(int btn,int state,int x,int y) {
	if(btn==GLUT_LEFT_BUTTON) {
		LeftDown = (state==GLUT_DOWN);
	}
	else if(btn==GLUT_MIDDLE_BUTTON) {
		MiddleDown = (state==GLUT_DOWN);
	}
	else if(btn==GLUT_RIGHT_BUTTON) {
		RightDown = (state==GLUT_DOWN);
	}
}

////////////////////////////////////////////////////////////////////////////////

void Tester::MouseMotion(int nx,int ny) {
	int maxDelta=100;
	int dx = glm::clamp(nx - MouseX,-maxDelta,maxDelta);
	int dy = glm::clamp(-(ny - MouseY),-maxDelta,maxDelta);

	MouseX = nx;
	MouseY = ny;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if(LeftDown) {
		const float rate=1.0f;
		cam->SetAzimuth(cam->GetAzimuth()+dx*rate);
		cam->SetIncline(glm::clamp(cam->GetIncline()-dy*rate,-90.0f,90.0f));
	}
	if(RightDown) {
		const float rate=0.005f;
		float dist=glm::clamp(cam->GetDistance()*(1.0f-dx*rate),0.01f,1000.0f);
		cam->SetDistance(dist);
	}
}

////////////////////////////////////////////////////////////////////////////////
