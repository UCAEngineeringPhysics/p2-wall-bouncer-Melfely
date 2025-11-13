//Helper methods to simplfy creating of many projects
#ifndef GPIO_H
#define GPIO_H


#include <stdio.h>
#include "pico/stdlib.h" 
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include <cassert>
#include <atomic>
#include <functional>

namespace GPIO
{
    class PIN {
        public:
            PIN(uint pin, bool output);

            virtual void Toggle();

            virtual void SetState(bool state);

            virtual bool GetState();

            virtual uint GetPin();

            virtual void ToggleEvery(float seconds);

            virtual void SetPulls(bool PullUp, bool PullDown);

            void SetIRQ(uint32_t eventMask, std::function<void(uint32_t)> callback);

            void DisableIRQ();

        protected:
            PIN(uint pin);
            PIN() = delete; //Remove default constructor
            const uint pinID;
            uint64_t timeAtLastCall_us= 0;
            float timePassed = 0;

            

        private:
            static std::function<void(uint32_t)> pinCallBack[NUM_BANK0_GPIOS];

            static void MasterCallback(uint, uint32_t);

    };

    class LED : PIN {

        public:
            LED(uint pin);

            using PIN::Toggle;
            using PIN::SetState;
            using PIN::GetState;
            using PIN::GetPin;
            using PIN::ToggleEvery;
            

        private:
            

    };

    class BUTTON : PIN {
        public:
            BUTTON(uint pin, bool IsPullUp);

            bool IsPullUp();

            bool IsPressed();

            using PIN::GetState;
            using PIN::GetPin;
        private:

    };
} //Namespace GPIO
#endif