#pragma once
#include <Application/iApplication.h>

class cMyApplication : public iApplication
{
public:
	cMyApplication();

private:
	void MainLoopFunc() override;
	void CustomInitialization() override;

	void ChangeBackground(double i_deltaTime);

private:
	double m_lastBackgroundChangeTime = 0;
};

