#pragma once
#include <Application/iApplication.h>

class cMyApplication : public iApplication
{
public:
	cMyApplication();

private:
	void MainLoopFunc() override;
	void CustomInitialization() override;
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;

	void ChangeBackground(double i_deltaTime);

private:
	double m_lastBackgroundChangeTime = 0;
};

