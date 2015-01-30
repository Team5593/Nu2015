#include "WPILib.h"

class Robot: public IterativeRobot
{

	RobotDrive myRobot;			// robot drive system
	Joystick Xcont; 			// only joystick
	Joystick stick;
	Talon m_liftMotor;
	Talon m_grabMotor;
	LiveWindow *lw;


public:
	Robot() :
		myRobot(0, 1),			// these must be initialized in the same order
		Xcont(0),				// as they are declared above.
		stick(1),
		m_liftMotor(2),
		m_grabMotor(3),
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
		double leftIn 	= -Xcont.GetRawAxis(1);				// leftIn (left Input) is set to XboxController (Axis 5 right thumbstick)
		double rightIn 	= -Xcont.GetRawAxis(5);				// rightIn (right Input) is set to XboxController (Axis 5 right thumbstick)
		float speed 	= 1-(Xcont.GetRawAxis(3)/2);		// Speed is inverted, then set to XboxController (Axis 3 right trigger), then divided by 2

		/* ---------- DEADZONES ---------- */				// Input below 0.08 (8%) is set to 0.00
		if (leftIn 	< 0.08 && leftIn > -0.08){				// L Thumb-Sti ck
			leftIn 	= 0;
		}
		if (rightIn < 0.08 && rightIn > -0.08){				// R Thumb-Stick
			rightIn = 0;
		}

		leftOut 	= leftIn*speed;							// L Speed
		rightOut 	= rightIn*speed;						// R Speed

		myRobot.TankDrive(leftOut,rightOut); 				// Drive, accounts for speed.

		if (stick.GetRawButton(3)){
			m_liftMotor.Set(1.0);
		}
		else{
			if (stick.GetRawButton(2)){
				m_liftMotor.Set(-1);
			}
			else{
				m_liftMotor.Set(0);
			};
		};

		if (stick.GetRawButton(4)){
			m_grabMotor.Set(1.0);
		}
		else{
			if (stick.GetRawButton(5)){
				m_grabMotor.Set(-1);
			}
			else{
				m_grabMotor.Set(0);
			};
		};


		// Smart dashboard
		SmartDashboard::PutNumber("LeftOut", leftOut);		// Print leftOut
		SmartDashboard::PutNumber("RightOut", rightOut);	// Print rightOut
	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
