#include "WPILib.h"

// Co-Driver Joystick Constants
#define JoyDownButton 2
#define JoyUpButton 3
#define JoyLeftButton 4
#define JoyRightButton 5

class Robot: public IterativeRobot
{
	Timer timer;					// Time
	RobotDrive myRobot;				// Robot drive system
	Joystick Xcont; 				// Main Driver control system
	Joystick stick;					// Co-Driver controller system
	Jaguar m_hook;					// Motor Controller
	Jaguar m_grab;					// ...
	LiveWindow *lw;					// Live Window
	DigitalInput LSL;				// Limit Switch Left
	DigitalInput LSR;				// Limit Switch Right
	Gyro gyro;						// Gyroscope
	BuiltInAccelerometer accel;		// Built In Accelerometer

	/* ---- Auto Variables (Global) ---- */
	double timeStart = 0;
	double timeDifference = 0;
	double timeCurrent = 0;

public:
	Robot() :
		myRobot(0, 1),		// These must be initialized in the same order
		Xcont(0),			// as they are declared above.
		stick(1),
		m_hook(2),
		m_grab(3),
		lw(NULL),
		LSL(2),
		LSR(3),
		gyro(0)
	{
		myRobot.SetExpiration(0.1);
	}

private:
	void RobotInit()
	{
		lw = LiveWindow::GetInstance();
		SmartDashboard::init();

	}

	void AutonomousInit()
	{
		timeStart = timer.GetFPGATimestamp();		// Timer Start
		SmartDashboard::PutNumber("Gyro", 0);		// Init Gyro SmartDashboard
		gyro.InitGyro();							// Init Gyro Sensor

	}

	void AutonomousPeriodic()
	{
		timeCurrent =  timer.GetFPGATimestamp();
		timeDifference= timeCurrent - timeStart;

		// drive for 10 seconds
		if(timeDifference < 10){
			myRobot.TankDrive(-0.8, -0.8);
		}
		else{
		// stop after 10 seconds
			myRobot.TankDrive(0.0, 0.0); // STOP!
		}

		gyro.GetAngle();

		SmartDashboard::PutNumber("Time Difference", timeDifference);
	}

	void TeleopInit()
	{
		/* ---- SMARTDASHBOARD INIT ---- */
		// Controller Output
		SmartDashboard::PutNumber("Xbox Left", Xcont.GetRawAxis(1));	// Print Left Axis
		SmartDashboard::PutNumber("Xbox Right", Xcont.GetRawAxis(5));	// Print Right Axis
		// Motor output
		SmartDashboard::PutNumber("LeftOut", 0);						// Print leftOut
		SmartDashboard::PutNumber("RightOut", 0);						// Print rightOut
	}

	void TeleopPeriodic()
	{
		/* ---- VARIABLES ---- */
		double leftOut 	= 0.0;									// Left Output
		double rightOut = 0.0;									// Right Output
		double leftIn 	= Xcont.GetRawAxis(1);					// leftIn (left Input) is set to XboxController (Axis 5 right thumbstick)
		double rightIn 	= Xcont.GetRawAxis(5);					// rightIn (right Input) is set to XboxController (Axis 5 right thumbstick)
		float speed 	= 1-(Xcont.GetRawAxis(3)/2);			// Speed calculations


		/* ---- CONTROLLER DEADZONES ---- */		// Input below 0.08 (8%) is set to nothing (0.0)
		if (leftIn 	< 0.08 && leftIn > -0.08){					// L Thumb-Stick
			leftIn 	= 0.0;
		}
		if (rightIn < 0.08 && rightIn > -0.08){					// R Thumb-Stick
			rightIn = 0.0;
		}

		/* ---- DRIVE ---- */
		leftOut 	= -leftIn*speed;							// L inverted x speed
		rightOut 	= -rightIn*speed;							// R inverted x speed

		myRobot.TankDrive(leftOut, rightOut); 					// Drive

		/* ---- HOOK ---- */
		if (stick.GetRawButton(JoyUpButton)){
			m_hook.Set(1.0);
		}

		if (stick.GetRawButton(JoyDownButton)){
			m_hook.Set(-1.0);
		}

		/* ---- GRABBING ---- */
		if (stick.GetRawButton(JoyLeftButton) &! LSL){			// Left Joystick Button
			m_grab.Set(1.0);			// Grabs
		}
		else if (stick.GetRawButton(JoyRightButton) &! LSR){	// Right Joystick Button
			m_grab.Set(-1.0);			// Releases
		}
		else{
			m_grab.Set(0.0);			// Stops any movement
		}

		/* ---- SMART DASHBOARD ---- */
		// Controller Output
		SmartDashboard::PutNumber("Left Out", leftOut);					// Print leftOut
		SmartDashboard::PutNumber("Right Out", rightOut);				// Print rightOut
		// Motor Output
		SmartDashboard::PutNumber("Xbox Left", Xcont.GetRawAxis(1));	// Print Left Axis
		SmartDashboard::PutNumber("Xbox Right", Xcont.GetRawAxis(5));	// Print Right Axis
	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
