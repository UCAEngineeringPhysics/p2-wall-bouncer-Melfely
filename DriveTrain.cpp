#include "DriveTrain.h"

#pragma region DualMotorDrive

/// @brief Inits a two motor drive, using two PWM::MOTORS, assumes a motor drive that utilizes a STBYpin
/// @param STBYPin The Pin the STBY system will run on, this allows this class to enable and disable motor output
/// @param LeftMotorPWMPin //The pwm pin for the left motor
/// @param LeftMotorPin1 //Pin 1 for the left motor, this on means backwards
/// @param LeftMotorPin2  //Pin 2 for the left motor, this on means forwards
/// @param RightMotorPWMPin //the pwm pin for the right motor
/// @param RightMotorPin1 //Pin 1 for the right motor, this on means backwards
/// @param RightMotorPin2 //Pin 2 for the right motor, this on means forwards
Drivetrain::DualMotor::DualMotor(uint STBYPin, uint LeftMotorPWMPin, uint LeftMotorPin1, uint LeftMotorPin2, uint RightMotorPWMPin, uint RightMotorPin1, uint RightMotorPin2) 
:
StandbyPin(STBYPin, true),
LeftMotor(LeftMotorPWMPin, LeftMotorPin1, LeftMotorPin2),
RightMotor(RightMotorPWMPin, RightMotorPin1, RightMotorPin2)
{

}

/// @brief Stops the motors
void Drivetrain::DualMotor::Stop() {
    LeftMotor.Stop();
    RightMotor.Stop();
}

/// @brief sets both motors to the same forward duty speed
/// @param speed a number between 0 and 1 that is the wanted speed
void Drivetrain::DualMotor::Forward(float speed) {
    LeftMotor.Forward(speed);
    RightMotor.Forward(speed);
}

/// @brief sets both motors to the same forward duty speed
/// @param speed a number between 0 and 1 that is the wanted speed
void Drivetrain::DualMotor::Backward(float speed) {
    LeftMotor.Backward(speed);
    RightMotor.Backward(speed);
}

/// @brief Sets the right motor to spin forward at given speed, and left motor to spin backwards at given speed, will be a left spin
/// @param speed a number between 0 and 1 that is the wanted speed
void Drivetrain::DualMotor::SpinLeft(float speed) {
    LeftMotor.Backward(speed);
    RightMotor.Forward(speed);
}

/// @brief Sets the right motor to spin backward at given speed, and left motor to spin forward at given speed, will be a right spin
/// @param speed a number between 0 and 1 that is the wanted speed
void Drivetrain::DualMotor::SpinRight(float speed){ 
    LeftMotor.Forward(speed);
    RightMotor.Backward(speed);
}

/// @brief Sets the STBYpin handled by the drivetrain to given state
/// @param state state to STBYpin to
void Drivetrain::DualMotor::SetState(bool state) {
    StandbyPin.SetState(state);
}

/// @brief Will return the duty or speed of the left motor
/// @return returns a float from 0 to 1
float Drivetrain::DualMotor::GetLeftDuty() {
    return LeftMotor.GetDuty();
}

/// @brief Will return the duty or speed of the left motor
/// @return returns a float from 0 to 1
float Drivetrain::DualMotor::GetRightDuty() {
    return RightMotor.GetDuty();
}

#pragma endregion