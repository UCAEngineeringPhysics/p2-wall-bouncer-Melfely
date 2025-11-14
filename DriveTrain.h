#ifndef DRIVETRAIN_H
#define DRIVETRAIN_H

#include "PWM.h"
#include "GPIO.h"

namespace Drivetrain
{
    class DualMotor {
        public:
            DualMotor(uint STBYPin, uint LeftMotorPWMPin, uint LeftMotorPin1, uint LeftMotorPin2, uint RightMotorPWMPin, uint RightMotorPin1, uint RightMotorPin2);

            virtual void Forward(float speed);
            virtual void Backward(float speed);

            virtual void SpinLeft(float speed);
            virtual void SpinRight(float speed);

            virtual void SetState(bool state);

            virtual void Stop();

            virtual float GetLeftDuty();
            virtual float GetRightDuty();

        protected:
            PWM::MOTOR LeftMotor;
            PWM::MOTOR RightMotor;
            GPIO::PIN StandbyPin;

        private:
            DualMotor() = delete;

    };

    
} // namespace DualMotor


#endif