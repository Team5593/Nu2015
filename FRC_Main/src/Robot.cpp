#include "WPILib.h"
#include <cmath>

#define PI 3.141592653589793
// Co-Driver Joy Stick Global Buttons
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
	Timer timer;					// Time
	RobotDrive myRobot;				// Robot drive system
	Joystick Xcont; 				// Main Driver control system
	Joystick stick;					// Co-Driver controller system
	Jaguar m_liftMotor;				// Motor Controller
	Jaguar m_grabMotor;				// ...
	LiveWindow *lw;					// Live Window
	Encoder lift_encoder;			// Encoder for Lifting
	DigitalInput limitSwitch;		// Limit switch for lifting
	Gyro gyro;
	BuiltInAccelerometer accel;		// Built In Accelerometer

	/* ---------- Global Variables ---------- */
	// Timer
	double timeStart = 0;
	double secCount = 0;
	// Accelerometer Values and Outputs
	double speed;
	double distance;
	double acc;
	//Gyroscope Output
	float gyroAngle;
	// Coordinates for Auto-Mode
	double xGoTo[2] = {0.0,5.0};		// The X coordinate that the robot is aiming for.
	double yGoTo[2] = {0.0,5.0};		// The Y...
	double xSquared;					// Difference Squared
	double ySquared;					// ...
	double autoDistance;				// Distance needed to travel
	double autoAngle;


public:
	Robot() :
		myRobot(0, 1),				// these must be initialized in the same order
		Xcont(0),					// as they are declared above.
		stick(1),
		m_liftMotor(2),
		m_grabMotor(3),
		lw(NULL),
		lift_encoder(0,1),
		limitSwitch(2),
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
		/* ---------- ACCELEROMETER ---------- */
		double timeNow = timer.GetFPGATimestamp();
		double timeElapsed = timeNow - timeStart;
		gyroAngle = round(gyro.GetAngle());

		if(timeElapsed > 0.002) {								// If it has been one tick
			if (accel.GetY() < 0.02 or accel.GetY() < -0.02){ 	// Deadzone/Error Rate removal
				acc = 0;
			}
			else{
				acc		 = accel.GetY();					// Get Acceleration
				speed	 = acc*timeElapsed;					// Calculate Speed
				distance = distance+(speed*timeElapsed);	// distance = distance + speed x time
			}

			// x² + y² = D², Using pythagoras theorem to find the distance.
			xSquared = pow(xGoTo[1]-xGoTo[0], 2);				// Difference Squared
			ySquared = pow(yGoTo[1]-yGoTo[0], 2);				// ...
			autoDistance = sqrt((xSquared + ySquared));			// Distance from robot to new x and y

			// Calculating Angle from point "A" to point "B"
			autoAngle = (atan((xGoTo[1]-xGoTo[0])/(yGoTo[1]-yGoTo[0]))*180)/PI; 	//Useing trig the robot finds the angle it need to head in

			while(autoDistance != distance){
				break;
			}

			timeStart = timeNow;
			secCount++;
		}

		/* ---------- COORDINATES ---------- */

		//Smart Dashboard
		SmartDashboard::PutNumber("Auto Angle", autoAngle);
		SmartDashboard::PutNumber("Auto Distance", autoDistance);
		SmartDashboard::PutNumber("Gyro", gyroAngle);
		SmartDashboard::PutNumber("Distance Traveled", distance);
		SmartDashboard::PutNumber("Acceleration", acc);
		SmartDashboard::PutNumber("Seccond Counter", secCount);
		SmartDashboard::PutNumber("Speed", speed);
	}

	void TeleopInit()
	{
		/* ---------- SMARTDASHBOARD INIT ---------- */
		// Controller Output
		SmartDashboard::PutNumber("Xbox Left", Xcont.GetRawAxis(1));	// Print Left Axis
		SmartDashboard::PutNumber("Xbox Right", Xcont.GetRawAxis(5));	// Print Right Axis
		// Motor output
		SmartDashboard::PutNumber("LeftOut", 0);						// Print leftOut
		SmartDashboard::PutNumber("RightOut", 0);						// Print rightOut
		// Encoder
		SmartDashboard::PutNumber("EncoderDirection", 0);				// Print encoder direction
		SmartDashboard::PutNumber("EncoderRotations", 0);				// Print encoder rotations
		// Accelerometer
		SmartDashboard::PutNumber("Get X", 0);							// X Forward
		SmartDashboard::PutNumber("Get Y", 0);							// Y	Sideward

		SmartDashboard::PutBoolean("Switch", 0);
	}

	void TeleopPeriodic()
	{
		/* ---------- LIMIT SWITCH ----------*/
		SmartDashboard::PutBoolean("Switch", limitSwitch.Get());


		/* ---------- VARIABLES ---------- */
		double leftOut 	= 0;									// Left Output
		double rightOut = 0;									// Right Output
		double leftIn 	= -Xcont.GetRawAxis(1);					// leftIn (left Input) is set to XboxController (Axis 5 right thumbstick)
		double rightIn 	= -Xcont.GetRawAxis(5);					// rightIn (right Input) is set to XboxController (Axis 5 right thumbstick)
		float speed 	= 1-(Xcont.GetRawAxis(3)/2);			// Speed is inverted, then set to XboxController (Axis 3 right trigger), then divided by 2
		bool manualLift  = true;
		int setHeight = 0;
		double heights[] = {1000.0,2000.0,3000.0,4000.0};		// Change these for levels of totes
																// no math just magic numbers! Do you beleive in magic?


		/* ---------- DEADZONES ---------- */					// Input below 0.08 (8%) is set to 0.00
		if (leftIn 	< 0.08 && leftIn > -0.08){					// L Thumb-Stick
			leftIn 	= 0;
		}
		if (rightIn < 0.08 && rightIn > -0.08){					// R Thumb-Stick
			rightIn = 0;
		}

		/* ---------- DRIVE OUTPUT ---------- */
		leftOut 	= leftIn*speed;								// L with speed
		rightOut 	= rightIn*speed;							// R with speed

		myRobot.TankDrive(leftOut,rightOut); 					// Drive

		/* --------- ENCODER VALUES ---------- */
		int EncoderDir;
		double EncoderRot;

		EncoderRot = lift_encoder.GetDistance();

		/* ---- MANUAL GRABBING OUTPUT ---- */
		if (stick.GetRawButton(JoyLeftButton)){			// Left Joystick Button
			m_grabMotor.Set(1);							// Grabs
		}
		else if (stick.GetRawButton(JoyRightButton)){	// Right Joystick Button
			m_grabMotor.Set(-1);						// Releases
		}
		else{
			m_grabMotor.Set(0);
		}

		/* ---- AUTO LIFT BUTTON CHECKING --- */
		if (stick.GetRawButton(Lvl1Button)){			// Set height to 1 box
			setHeight = 1;
			manualLift = false;
		}
		if (stick.GetRawButton(Lvl2Button)){			// Set height to 2 box
			setHeight = 2;
			manualLift = false;
		}
		if (stick.GetRawButton(Lvl3Button)){			// Set height to 3 box
			setHeight = 3;
			manualLift = false;
		}
		if (stick.GetRawButton(Lvl4Button)){			// Set height to 4 box
			setHeight = 4;
			manualLift = false;
		}
		if (stick.GetRawButton(ResetEncoderButton)){	// Reset Height
			lift_encoder.Reset();
		}

		/* ------ MANUAL LIFT OUTPUT ------ */
		if (stick.GetRawButton(JoyUpButton)){			// Joystick Up Button "Going Up"
			m_liftMotor.Set(1);
			manualLift = true;
		}

		else if (stick.GetRawButton(JoyDownButton)){	// Joystick Down Button "Going Down"
			m_liftMotor.Set(-1);
			manualLift = true;
		} else{
			m_liftMotor.Set(0);
		}

		/* ---------- AUTO LIFT ---------- */
		if (!manualLift){
			if (EncoderRot < (heights[setHeight-1] * 0.99)){
				SmartDashboard::PutNumber("EncoderTarget",heights[setHeight-1] * 0.99 ); 	// Going Up. *Elevator Music*
				m_liftMotor.Set(1);
				EncoderDir = 1;
			}
			else if (EncoderRot > (heights[setHeight-1] * 1.01)){
				SmartDashboard::PutNumber("EncoderTarget",heights[setHeight-1] * 1.01 ); 	// Going Down
				m_liftMotor.Set(-1);
				EncoderDir = -1;
			}
			else {
				m_liftMotor.Set(0);
				manualLift = true;				// *Ding* You have reached your destination!
				EncoderDir = 0;
			}
		}


		/* ---------- SMART DASHBOARD ---------- */
		//Motor and Controller Outputs


		SmartDashboard::PutNumber("LeftOut", leftOut);					// Print leftOut
		SmartDashboard::PutNumber("RightOut", rightOut);				// Print rightOut
		SmartDashboard::PutNumber("Axis Left", Xcont.GetRawAxis(1));	// Print Left Axis
		SmartDashboard::PutNumber("Axis Right", Xcont.GetRawAxis(5));	// Print Right Axis
		// Lift Encoder
		SmartDashboard::PutNumber("EncoderDirection", EncoderDir);		// Print encoder direction
		SmartDashboard::PutNumber("EncoderRotations", EncoderRot);		// Print encoder distance
		SmartDashboard::PutNumber("manualLift", manualLift);			// Print manualLift
		SmartDashboard::PutNumber("setHeight", setHeight);				// Print setHeight
		// Accelerometer Raw Input
		SmartDashboard::PutNumber("Get Raw X", accel.GetX());			// Print X
		SmartDashboard::PutNumber("Get Raw Y", accel.GetY());			// Print Y
	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
