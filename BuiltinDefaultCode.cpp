#include "WPILib.h"

/**
 * This "BuiltinDefaultCode" provides the "default code" functionality as used in the "Benchtop Test."
 * 
 * The BuiltinDefaultCode extends the IterativeRobot base class to provide the "default code"
 * functionality to confirm the operation and usage of the core control system components, as 
 * used in the "Benchtop Test" described in Chapter 2 of the 2009 FRC Control System Manual.
 * 
 * This program provides features in the Disabled, Autonomous, and Teleop modes as described
 * in the benchtop test directions, including "once-a-second" debugging printouts when disabled, 
 * a "KITT light show" on the solenoid lights when in autonomous, and elementary driving
 * capabilities and "button mapping" of joysticks when teleoperated.  This demonstration
 * program also shows the use of the MotorSafety timer.
 * 
 * This demonstration is not intended to serve as a "starting template" for development of
 * robot code for a team, as there are better templates and examples created specifically
 * for that purpose.  However, teams may find the techniques used in this program to be
 * interesting possibilities for use in their own robot code.
 * 
 * The details of the behavior provided by this demonstration are summarized below:
 *  
 * Disabled Mode:
 * - Once per second, print (on the console) the number of seconds the robot has been disabled.
 * 
 * Autonomous Mode:
 * - Flash the solenoid lights like KITT in Knight Rider
 * - Example code (commented out by default) to drive forward at half-speed for 2 seconds
 * 
 * Teleop Mode:
 * - Select between two different drive options depending upon Z-location of Joystick1
 * - When "Z-Up" (on Joystick1) provide "arcade drive" on Joystick1
 * - When "Z-Down" (on Joystick1) provide "tank drive" on Joystick1 and Joystick2
 * - Use Joystick buttons (on Joystick1 or Joystick2) to display the button number in binary on
 *   the solenoid LEDs (Note that this feature can be used to easily "map out" the buttons on a
 *   Joystick.  Note also that if multiple buttons are pressed simultaneously, a "15" is displayed
 *   on the solenoid LEDs to indicate that multiple buttons are pressed.)
 *
 * This code assumes the following connections:
 * - Driver Station:
 *   - USB 1 - The "right" joystick.  Used for either "arcade drive" or "right" stick for tank drive
 *   - USB 2 - The "left" joystick.  Used as the "left" stick for tank drive
 * 
 * - Robot:
 *   - Digital Sidecar 1:
 *     - PWM 1/3 - Connected to "left" drive motor(s)
 *     - PWM 2/4 - Connected to "right" drive motor(s)
 */
class BuiltinDefaultCode: public IterativeRobot {
	// Declare variable for the robot drive system
	RobotDrive *m_robotDrive; // robot will use PWM 1-4 for drive motors
	Victor *frontLeft;
	Victor *frontRight;
	Victor *backLeft;
	Victor *backRight;
	Victor *elevator;
	Solenoid *armEngage;
	Solenoid *armDisengage;
	Solenoid *clawOpen;
	Solenoid *clawClose;
	
	Compressor *compressor;
	// Declare a variable to use to access the driver station object
	DriverStation *m_ds; // driver station object
	UINT32 m_priorPacketNumber; // keep track of the most recent packet number from the DS
	UINT8 m_dsPacketsReceivedInCurrentSecond; // keep track of the ds packets received in the current second

	// Declare variables for the two joysticks being used
	Joystick *driveController; // joystick 1 (arcade stick or right tank stick)			// joystick 2 (tank left stick


public:
	/**
	 * Constructor for this "BuiltinDefaultCode" Class.
	 * 
	 * The constructor creates all of the objects used for the different inputs and outputs of
	 * the robot.  Essentially, the constructor defines the input/output mapping for the robot,
	 * providing named objects for each of the robot interfaces. 
	 */
	BuiltinDefaultCode(void) {
		printf("BuiltinDefaultCode Constructor Started\n");

		// Create a robot using standard right/left robot drive on PWMS 1, 2, 3, and #4
		frontLeft = new Victor(3);
		frontRight = new Victor(4);
		backLeft = new Victor(2);
		backRight = new Victor(1);
		armEngage = new Solenoid(4);
		armDisengage = new Solenoid(5);
		clawOpen = new Solenoid(1);
		clawClose = new Solenoid(2);
		compressor = new Compressor(5, 6);
		elevator = new Victor(5);

		m_robotDrive = new RobotDrive(frontLeft, backLeft, frontRight,
				backRight);

		// Acquire the Driver Station object
		m_ds = DriverStation::GetInstance();
		m_priorPacketNumber = 0;
		m_dsPacketsReceivedInCurrentSecond = 0;
		// Define joysticks being used at USB port #1 and USB port #2 on the Drivers Station
		driveController = new Joystick(1);

		GetWatchdog().SetEnabled(false);

		printf("BuiltinDefaultCode Constructor Completed\n");
	}

	/********************************** Init Routines *************************************/

	void RobotInit(void) {
		// Actions which would be performed once (and only once) upon initialization of the
		// robot would be put here.

		printf("RobotInit() completed.\n");
	}

	void DisabledInit(void) {
		// Move the cursor down a few, since we'll move it back up in periodic.
		printf("\x1b[2B");
	}

	void AutonomousInit(void) {
	}

	void TeleopInit(void) {
		m_dsPacketsReceivedInCurrentSecond = 0;
	}

	/********************************** Periodic Routines *************************************/

	void DisabledPeriodic(void) {
		static INT32 printSec = (INT32) GetClock() + 1;
		static const INT32 startSec = (INT32) GetClock();
		// while disabled, printout the duration of current disabled mode in seconds
		if (GetClock() > printSec) {
			// Move the cursor back to the previous line and clear it.
			printf("\x1b[1A\x1b[2K");
			printf("Disabled seconds: %d\r\n", printSec - startSec);
			printSec++;
		}
	}
	void driveForward() {
		backRight->SetSpeed(0.5);
		backLeft->SetSpeed(0.5);
		frontLeft->SetSpeed(0.5);
		frontRight->SetSpeed(0.5);
	}
	void stop() {
		backRight->SetSpeed(0);
		backLeft->SetSpeed(0);
		frontRight->SetSpeed(0);
		frontLeft->SetSpeed(0);
	}
	void driveMethod(int action, int time) {
		if (action == 0) {
			driveForward();
			sleep(time);
			stop();
		} else if (action == 1) {
			frontRight->SetSpeed(0.25); // - is forward
			backRight->SetSpeed(0.25); // + is back
			frontLeft->SetSpeed(0.25);
			backLeft->SetSpeed(0.25);
			sleep(time);
			stop();
		} else if (action == 2) {
			frontRight->SetSpeed(-0.25); // - is forward
			backRight->SetSpeed(-0.25); // + is back
			frontLeft->SetSpeed(-0.25);
			backLeft->SetSpeed(-0.25);
			sleep(time);
			stop();
		}
	}
	void AutonomousPeriodic(void) {
		int action[] = { 0, 1, 2, 1, 0 }; // 0 go, 1 right, 2 left
		int time[] = { 100, 0, 0, 0, 100 };
		for (int i = 0; i < 5; i++) {
			driveMethod(action[i], time[i]);
		}
	}

	void TeleopPeriodic(void) {
		while (1) {
			m_dsPacketsReceivedInCurrentSecond++;
			if (driveController->GetRawButton(8)
					&& !driveController->GetRawButton(7)) { //turn right on right bumper press
				frontRight->SetSpeed(0.25); // - is forward
				backRight->SetSpeed(0.25); // + is back
				frontLeft->SetSpeed(0.25);//turn right on right trigger
				backLeft->SetSpeed(0.25);
			} else if (driveController->GetRawButton(7)
					&& !driveController->GetRawButton(8)) { //turn left on left bumper press
				frontRight->SetSpeed(-0.25); // turn left on right trigger
				backRight->SetSpeed(-0.25);
				frontLeft->SetSpeed(-0.25);
				backLeft->SetSpeed(-0.25);
			} else if (driveController->GetRawButton(1)) { //drops claw arm on X button press
				bool state = armEngage->Get();
				if(state == true){
				armEngage->Set(false);
				armDisengage->Set(true);
				}
				else{
					armDisengage->Set(false);
					armEngage->Set(true);
				}
			} else if (driveController->GetRawButton(3)) { //toggles claw state with B button press
				bool state = clawOpen->Get();
				if (state == true) {		//if claw is open close close it
					clawOpen->Set(false);
					clawClose->Set(true);
				} else {					//otherwise open the claw
					clawOpen->Set(true);
					clawClose->Set(false);
				}
			} else if (driveController->GetRawButton(4)) { //Raise arm on Y button press
				while (driveController->GetRawButton(4)) {
					elevator->SetSpeed(0.1);
				}
			} else if (driveController->GetRawButton(2)) { //Lower arm on A button press
				while (driveController->GetRawButton(2)) {
					elevator->SetSpeed(-0.1);
				}
			} else {
				if (driveController->GetRawAxis(4) >= 0.05
						|| driveController->GetRawAxis(4) <= -0.05
						|| driveController->GetRawAxis(2) >= 0.05
						|| driveController->GetRawAxis(2) <= -0.05) //tank drive on joystick
					m_robotDrive->TankDrive(
							-1 * driveController->GetRawAxis(2),
							-1 * driveController->GetRawAxis(4)); // drive with tank style
				else {									//if no input stop doing anything
					frontRight->SetSpeed(0);
					backRight->SetSpeed(0);
					frontLeft->SetSpeed(0);
					backLeft->SetSpeed(0);
				}
			}
		}
	}

	/********************************** Continuous Routines *************************************/

	/* 
	 * These routines are not used in this demonstration robot
	 *
	 * 
	 void DisabledContinuous(void) {

	 }
	 */
	void compressorControl() {
		compressor->Start();
		while (compressor->GetPressureSwitchValue() < 110) {
		}
		stop();
	}
	void AutonomousContinuous(void) {
		if (compressor->GetPressureSwitchValue() < 60)
			compressorControl();
	}
	void TeleopContinuous(void) {
		if (compressor->GetPressureSwitchValue() < 60)
			compressorControl();
	}

};

START_ROBOT_CLASS(BuiltinDefaultCode)
;
