#include "WPILib.h"

#define JoyDownButton 2
#define JoyUpButton 3
#define JoyLeftButton 4
#define JoyRightButton 5

#define Lvl1Button 6
#define Lvl2Button 7
#define Lvl3Button 8
#define Lvl4Button 9

#define ResetEncoderButton 11


class Robot: public IterativeRobot
{

	RobotDrive myRobot;			// robot drive system
	Joystick Xcont; 			// only joystick
	Joystick stick;
	Talon m_liftMotor;
	Talon m_grabMotor;
	LiveWindow *lw;
	Encoder lift_encoder;
	DigitalInput di;

	DigitalInput limitSwitch;
	BuiltInAccelerometer accel;


public:
	Robot() :
		myRobot(0, 1),			// these must be initialized in the same order
		Xcont(0),				// as they are declared above.
		stick(1),
		m_liftMotor(2),
		m_grabMotor(3),
		lw(NULL),
		lift_encoder(0,1),
		limitSwitch(2),
		accel(0)
	{
		myRobot.SetExpiration(0.1);
	}

private:
	void RobotInit()
	{
		lw = LiveWindow::GetInstance();

di.

		accel.SetRange(Accelerometer::Range::kRange_8G);	// Set accel range to 8G's

		SmartDashboard::init();
	}

	void AutonomousInit()
	{

		/*Starting behind tote

		 Grab tote, waiting until limit switch

		 Lift tote to level 2, detected by encoder

		 !!!Get to Auto zone!!! Difficult due to no encoders atm.
		 Will probably have to be timed - Means a lot of testing trial and error(Especially on the day)
		 Alternative:
		 Use

		 Stay in the auto zone
		 */
	}

	void AutonomousPeriodic()
	{

	}

	void TeleopInit()
	{
		SmartDashboard::init();
		// Controller Output
		SmartDashboard::PutNumber("Axis Left", Xcont.GetRawAxis(1));	// Print Left Axis
		SmartDashboard::PutNumber("Axis Right", Xcont.GetRawAxis(5));	// Print Right Axis
		// Motor output
		SmartDashboard::PutNumber("LeftOut", 0);						// Print leftOut
		SmartDashboard::PutNumber("RightOut", 0);						// Print rightOut
		// Encoder
		SmartDashboard::PutNumber("EncoderDirection", 0);				// Print encoder direction
		SmartDashboard::PutNumber("EncoderRotations", 0);				// Print encoder rotations
		// Accelerometer
		SmartDashboard::PutNumber("Get X", 0);
		SmartDashboard::PutNumber("Get Y", 0);
		SmartDashboard::PutNumber("Get Z", 0);

		SmartDashboard::PutBoolean("Switch", 0);
	}

	void TeleopPeriodic()
	{
		/* ---------- VARIABLES ---------- */
		double leftOut 	= 0;								// Left Output
		double rightOut = 0;								// Right Output
		double leftIn 	= -Xcont.GetRawAxis(1);				// leftIn (left Input) is set to XboxController (Axis 5 right thumbstick)
		double rightIn 	= -Xcont.GetRawAxis(5);				// rightIn (right Input) is set to XboxController (Axis 5 right thumbstick)
		float speed 	= 1-(Xcont.GetRawAxis(3)/2);		// Speed is inverted, then set to XboxController (Axis 3 right trigger), then divided by 2
		bool manualLift  = true;
		int setHeight = 0;
		double heights[] = {1000.0,2000.0,3000.0,4000.0};		//Change these for levels of totes
																//no math just magic numbers!


		/* ---------- DEADZONES ---------- */				// Input below 0.08 (8%) is set to 0.00
		if (leftIn 	< 0.08 && leftIn > -0.08){				// L Thumb-Stick
			leftIn 	= 0;
		}
		if (rightIn < 0.08 && rightIn > -0.08){				// R Thumb-Stick
			rightIn = 0;
		}

		/* ---------- DRIVE OUTPUT ---------- */
		leftOut 	= leftIn*speed;							// L Speed
		rightOut 	= rightIn*speed;						// R Speed

		myRobot.TankDrive(leftOut,rightOut); 				// Drive, accounts for speed.

		/* --------- ENCODER VALUES ---------- */
		bool direction = lift_encoder.GetDirection();
		int EncoderDir;
		double EncoderRot;

		EncoderRot = lift_encoder.GetDistance();

		/* ---- MANUAL GRABBING OUTPUT ---- */
		if (stick.GetRawButton(JoyDownButton)){			//Left Joystick Button
			m_grabMotor.Set(1);							//Grabs
		}
		else if (stick.GetRawButton(JoyRightButton)){	//Right Joystick Button
			m_grabMotor.Set(-1);						//Releases
		}
		else{
			m_grabMotor.Set(0);
		}

		/* ---- AUTO LIFT BUTTON CHECKING --- */

		if (stick.GetRawButton(Lvl1Button)){
			setHeight = 1;
			manualLift = false;
		}
		if (stick.GetRawButton(Lvl2Button)){
			setHeight = 2;
			manualLift = false;
		}
		if (stick.GetRawButton(Lvl3Button)){
			setHeight = 3;
			manualLift = false;
		}
		if (stick.GetRawButton(Lvl4Button)){
			setHeight = 4;
			manualLift = false;
		}
		if (stick.GetRawButton(ResetEncoderButton)){
			lift_encoder.Reset();
		}

		/* ------ MANUAL LIFT OUTPUT ------ */
		if (stick.GetRawButton(JoyUpButton)){			//Joystick Up Button
			m_liftMotor.Set(1);
			manualLift = true;
		}

		else if (stick.GetRawButton(JoyDownButton)){	//Joystick Down Button
			m_liftMotor.Set(-1);
			manualLift = true;
		} else {
			m_liftMotor.Set(0);
		}

		/* ---------- AUTO LIFT ---------- */
		if (!manualLift){
			if (EncoderRot < (heights[setHeight-1] * 0.99) ){
				SmartDashboard::PutNumber("EncoderTarget",heights[setHeight-1] * 0.99 ); //Going Up
				m_liftMotor.Set(1);
				EncoderDir = 1;
			}
			else if (EncoderRot > (heights[setHeight-1] * 1.01) ){
				SmartDashboard::PutNumber("EncoderTarget",heights[setHeight-1] * 1.01 ); //Going Down
				m_liftMotor.Set(-1);
				EncoderDir = -1;
			}
			else {
				m_liftMotor.Set(0);
				manualLift = true;				//*Ding* You have reached your destination!
				EncoderDir = 0;
			}
		}

		/* ---------- SMART DASHBOARD ---------- */
		SmartDashboard::PutNumber("LeftOut", leftOut);					// Print leftOut
		SmartDashboard::PutNumber("RightOut", rightOut);				// Print rightOut
		SmartDashboard::PutNumber("Axis Left", Xcont.GetRawAxis(1));	// Print Left Axis
		SmartDashboard::PutNumber("Axis Right", Xcont.GetRawAxis(5));	// Print Right Axis
		SmartDashboard::PutNumber("EncoderDirection", EncoderDir);		// Print encoder direction
		SmartDashboard::PutNumber("EncoderRotations", EncoderRot);		// Print encoder distance
		SmartDashboard::PutNumber("manualLift", manualLift);			// Print manualLift
		SmartDashboard::PutNumber("setHeight", setHeight);				// Print setHeight
		// Accelerometer Raw Input
		SmartDashboard::PutNumber("Get X", accel.GetX());
		SmartDashboard::PutNumber("Get Y", accel.GetY());
		SmartDashboard::PutNumber("Get Z", accel.GetZ());
		bool di = limitSwitch.CheckAnalogInput(2);
		SmartDashboard::PutBoolean("Switch", di);
	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
