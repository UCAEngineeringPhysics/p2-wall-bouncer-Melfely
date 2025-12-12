#ifndef PWM_H
#define PWM_H //AI help me notice the spelling error here. It broke alot but very unclearly, you should try it.

#include "GPIO.h"
#include <cassert>

namespace PWM
{

    class PIN : GPIO::PIN{

        public: 
            PIN(uint pin, int frequency, int wrapCounter);

            virtual void FadeUp(int msecs, float peak, volatile std::atomic<int>& DiffVar);
            virtual void FadeDown(int msecs, float peak, volatile std::atomic<int>& DiffVar);
            
            virtual void SetDuty(uint duty);
            virtual void SetDuty(float duty);

            virtual void Stop();

            virtual void Toggle();
            virtual void SetState(bool IsOn);
            virtual bool GetState();
            virtual float GetDuty();
            
            using GPIO::PIN::GetPin;
            using GPIO::PIN::ToggleEvery;
            using GPIO::PIN::SetIRQ;

        protected:
            PIN() = delete; //Remove Default Constructor
            float currentDuty;
            const int FREQUENCY;
            uint CHANNEL;
            uint SLICE;
            const int WRAPCOUNTER;

    };

    class LED : PIN{

        public:
            LED(uint pin);

            using PIN::Toggle;
            using PIN::SetState;
            using PIN::FadeDown;
            using PIN::FadeUp;
            using PIN::SetDuty;
            using PIN::Stop;
            using PIN::GetPin;
            using PIN::ToggleEvery;

        private:


    };

    class MOTOR : PIN {

        public:
            MOTOR(uint pwmPin, uint pin1, uint pin2);

            using PIN::GetDuty;
            using PIN::Stop;

            void Forward(float speed);
            void Backward(float speed);
        private:
            
            using PIN::SetDuty;

            GPIO::PIN Pin1;
            GPIO::PIN Pin2;


    };
} // namespace PWM
#endif