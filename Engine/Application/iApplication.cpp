#include "iApplication.h"

iApplication* iApplication::instance = nullptr;

iApplication::iApplication()
{
}

void iApplication::_DisplayFunc()
{
	instance->DisplayFunc();
}

void iApplication::_IdleFunc()
{
	instance->IdleFunc();
}

void iApplication::_HandleKeyboardFunc(unsigned char key, int x, int y)
{
	instance->HandleKeyboardFunc(key, x, y);
}

void iApplication::Initialize(int argc, char** argv)
{
	instance = this;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(m_windowWidth, m_windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(m_windowTitle.c_str());
	glutDisplayFunc(_DisplayFunc);
	glutIdleFunc(_IdleFunc);
	glutKeyboardFunc(_HandleKeyboardFunc);

	CustomInitialization();
}

void iApplication::Run()
{
	m_running = true;
	glutMainLoop();
}

void iApplication::DisplayFunc()
{
}

void iApplication::IdleFunc()
{
}

void iApplication::HandleKeyboardFunc(unsigned char key, int x, int y)
{
}

void iApplication::CustomInitialization()
{
}

void iApplication::ExitApplication()
{
	m_running = false;
	glutLeaveMainLoop();
	delete this;
}

iApplication::~iApplication()
{
}
