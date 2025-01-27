#pragma once
#include <Application/iApplication.h>

class cMyApplication : public iApplication
{
public:
	cMyApplication();

private:
	void HandleKeyboardFunc(unsigned char key, int x, int y) override;
	void CustomInitialization() override;
	void DisplayFunc() override;

	static void ChangeBackground(int value);
};

