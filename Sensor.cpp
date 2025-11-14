#include "Sensor.h"
#include <cmath>

#pragma region Distance

/// @brief Constructor for a distance Sensor
/// @param TriggerPin This is the pin that will be used for starting the cycle. Is PWM
/// @param EchoPin This is the pin that will be used to return the time it took. Is GPIO
Sensor::Distance::Distance(uint TriggerPin, uint EchoPin)
:
TriggerPin(TriggerPin, 12, 49999), EchoPin(EchoPin, false)
{
    this->TriggerPin.SetDuty((uint)(6));
    this->EchoPin.SetPulls(false, true);

    this->EchoPin.SetIRQ( GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, std::bind(&Distance::echoHandler, this, std::placeholders::_1 ));

    this->distance = std::nullopt;
    this->startTime = 0;
}

/// @brief Method used by the echoHandler_Callback method to handle the measuring calculating the distance
/// @param events Internal Stuff
void Sensor::Distance::echoHandler(uint32_t events) {
    if (this->EchoPin.GetState() ) {
        this->startTime = time_us_64();
        //printf("StartTime: %llu -> ", startTime); //Debug Print
    } else {
            uint64_t dT = time_us_64() - this->startTime;
            //printf(" dT: %llu \n", dT); //Debug Print
            if (dT < 100) {
                this->distance = 0;
            } else if (dT > 100 && dT < 38000) {
                this->distance = dT / 58.0f / 100.0f;
            } else {
                this->distance = std::nullopt;
            }
            
    }
}

/// @brief Returns the non-thread Safe Distance read by the sensor
/// @return this will return the distance in meters as a float, if a -1.0 then that is out of range
float Sensor::Distance::GetDistance() {
    if (this->distance != std::nullopt) {
            return *this->distance; 
    } else {
            return -1.0f;
    }

}

#pragma endregion
#pragma region MotorEncoder

Sensor::MotorEncoder::MotorEncoder(uint pinA, uint pinB)
: EncodPinA(pinA, false), EncodPinB(pinB, false)
{

    EncodPinA.SetPulls(false, false);
    EncodPinB.SetPulls(false, false);

    this->pinAVal = EncodPinA.GetState();
    this->pinBVal = EncodPinB.GetState();

    this->encoderCounts = 0.0f;
    this->previousCounts = 0.0f;

    this->wheelAngVelocity = 0;
    this->wheelLinVelocity = 0;

    this->EncodPinB.SetIRQ(GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, std::bind(&MotorEncoder::PinAHandler, this, std::placeholders::_1));
    this->EncodPinA.SetIRQ(GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, std::bind(&MotorEncoder::PinBHandler, this, std::placeholders::_1));
    

    //Make the timer negative, so the time is ALWAYS accurate regardless of callback execution time
    add_repeating_timer_ms(-1 * (1000 / timerFrequency), MeasureVelocity_Callback, this, &timer);
    
}

void Sensor::MotorEncoder::PinAHandler(uint32_t events) {
    this->pinAVal = EncodPinA.GetState();
    //This will subtract when pinA and pinB are equal, otherwise will add
    /*
    a = 0, b = 0 subtract
    a = 1, b = 0 add
    a = 0, b = 1 add
    a = 1, b = 1 subtract    
    */

    if (pinAVal != pinBVal) {
        encoderCounts += 1;
    } else {
        encoderCounts -= 1;
    }

    //printf("PinA\n"); //debug line

}
void Sensor::MotorEncoder::PinBHandler(uint32_t events){
    this->pinBVal = EncodPinB.GetState();
    //This will add when pinA and pinB are equal, otherwise will subtract
    /*
    a = 0, b = 0 add
    a = 1, b = 0 subtract
    a = 0, b = 1 subtract
    a = 1, b = 1 add
    */

    if (pinAVal != pinBVal) {
        encoderCounts -= 1;
    } else {
        encoderCounts += 1;
    }

    //printf("PinB\n" ); //debug line
}

void Sensor::MotorEncoder::MeasureVelocity(){
    
    
    int deltaCounts = this->encoderCounts - this->previousCounts;
    
    this->previousCounts = this->encoderCounts; //Update previous counts

    float countsPerSecond = deltaCounts * timerFrequency;
    float motorRPS = countsPerSecond / encoderCPR;
    float motorAngVelocity = motorRPS * 2 * M_PI;

    this->wheelAngVelocity = motorAngVelocity / gearRatio;
    this->wheelLinVelocity = this->wheelAngVelocity * wheelRadius;
    

}

bool Sensor::MotorEncoder::MeasureVelocity_Callback(struct repeating_timer *t){
    //Take the void pointer, cast it to an uint pointer, then dereference it, getting us a uint number
    MotorEncoder* self = (MotorEncoder*)t->user_data;
    self->MeasureVelocity();
    return true;

}

#pragma endregion