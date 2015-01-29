#include "WPILib.h"

class Robot: public IterativeRobot
{

	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick
	LiveWindow *lw;

public:
	Robot() :
		myRobot(0, 1),	// these must be initialized in the same order
		stick(0),		// as they are declared above.
		lw(NULL)
	{
		myRobot.SetExpiration(0.1);
	}

private:
	void RobotInit()
	{
		lw = LiveWindow::GetInstance();

		SmartDashboard::init();
		SmartDashboard::PutString("init","init has run");
	}

	void AutonomousInit()
	{
	}

	void AutonomousPeriodic()
	{
	}

	void TeleopInit()
	{
	}

	void TeleopPeriodic()
	{
		/* ---------- VARIABLES ---------- */
		double leftOut 	= 0;
		double rightOut = 0;
		double leftIn 	= -stick.GetRawAxis(1);
		double rightIn 	= -stick.GetRawAxis(5);				// leftIn (left Input) is set to XboxController (Axis 5 right thumbstick)
		float speed 	= -(stick.GetRawAxis(3)/2);			// Speed is inverted, then set to XboxController (Axis 3 right trigger), then divided by 2

		/* ---------- DEADZONES ---------- */				// Input below 0.08 (8%) is set to 0.00
		if (leftIn 	< 0.08){								// L Thumb-Stick
			leftIn 	= 0;
		}
		if (rightIn < 0.08){								// R Thumb-Stick
			rightIn = 0;
		}

		leftOut 	= leftIn*speed;
		rightOut 	= rightIn*speed;

		myRobot.TankDrive(leftOut,rightOut); 		// Drive, accounts for speed.



		// Smart dashboard
		SmartDashboard::PutNumber("LeftOut", leftOut);	// Print leftOut
		SmartDashboard::PutNumber("RightOut", rightOut);	// Print rightOut
	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
