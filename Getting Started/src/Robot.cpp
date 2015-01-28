#include "WPILib.h"

class Robot: public IterativeRobot
{

	RobotDrive myRobot; // robot drive system
	Joystick leftStick; // only joystick
	Joystick rightStick; //
	LiveWindow *lw;

public:
	Robot() :
		myRobot(0, 1),	// these must be initialized in the same order
		leftStick(0),		// as they are declared above.
		rightStick(1),
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
		/* ---------- SENSOR ---------- */

		/* ---------- LIFTING ---------- */

		/* ---------- DRIVE ---------- */

		// variables
		float power = 0;
		double leftOut = 0;
		double rightOut = 0;
		double leftIn = leftStick.GetY();
		double rightIn = rightStick.GetY();

		// Power
		if (leftStick.GetRawButton(2)){						// Set Power to 65% if button 2 is pressed on L stick
			power = 0.65;
		}
		else power = 1;

		// Output calculation
		leftOut = -leftIn*power;
		rightOut = -rightIn*power;

		// Deadzones
		if (leftOut < 0.08){								// L Stick
			leftOut = 0;
		}
		if (rightOut < 0.08){								// R Stick
			leftOut = 0;
		}

		// Smart dashboard
		SmartDashboard::PutNumber("LeftOut", leftOut);		// Print leftOut
		SmartDashboard::PutNumber("RightOut", rightOut);	// Print rightOut

		myRobot.TankDrive(leftOut,rightOut); 				// Drive with tank controls
	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
