#include <stdio.h>
#include <string>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pico/multicore.h"
#include "hardware/watchdog.h"
#include "PWM.h"
#include "GPIO.h"
#include "Sensor.h"
#include "DriveTrain.h"
#include <atomic>

#pragma region 
volatile std::atomic<int> mode = 0;
static std::atomic<float> workTime = 0;

GPIO::BUTTON mainButton(22, false);

GPIO::LED redLed(28);
PWM::LED blueLed(26);
PWM::LED greenLed(27);


#pragma region Function Headers
void mainButton_callback(uint32_t eventMask);

int64_t alarmHoldRestart_callback(alarm_id_t event, void* USERDATA);

void core1_main(); 

#pragma endregion

#pragma region Main
int main()
{
    
    
    uint64_t workStart_us = 0;
    float idleFadeCycle = 0;
    float lowPowerWarnCycle = 0;

    //Init the default configurations
    //This turns on UART.
    stdio_init_all();
    
    //Launch core1
    multicore_launch_core1(core1_main);

    //Wait for core1 to fully init
    multicore_fifo_pop_blocking();

    

    while (true) {

        
        if (mode % 2 == 0) {
        #pragma region Pause Mode Core 1

            if(workTime < 45) {
                redLed.SetState(0);
                greenLed.FadeUp(500, 1, mode);
                if (mode % 2 != 0) continue;
                greenLed.FadeDown(500, 1, mode);
                blueLed.SetState(0);
            }else if (workTime >= 45 && workTime < 55){
                redLed.SetState(0);
                blueLed.FadeUp(500, 1, mode);
                if (mode % 2 != 0) continue;
                blueLed.FadeDown(500, 1, mode);
                greenLed.SetState(0);
            } else if (workTime >= 55 && workTime < 60) 
            {
                workStart_us = time_us_64(); //Start accuminlating workTime after 55 seconds in anymode, so it can restart after 60 (or 5 seconds of red light)
                if (idleFadeCycle <= .5) {
                    blueLed.SetDuty(idleFadeCycle * 2);
                } else if (idleFadeCycle < 1) {
                    blueLed.SetDuty(1.0f - ( (idleFadeCycle - 0.5f) * 2.0f));
                } else {
                    idleFadeCycle = 0;
                }

                redLed.ToggleEvery(.05);
                sleep_ms(10);
                workTime += (float)(time_us_64() - workStart_us) / 1000000.0f;
                idleFadeCycle += (float)(time_us_64() - workStart_us) / 1000000.0f;
                lowPowerWarnCycle += (float)(time_us_64() - workStart_us) / 1000000.0f;
                
            } else {
                break;
            }
        #pragma endregion
        } else {
        #pragma region Work Mode Core 1
           
            workStart_us = time_us_64();
            
            if (workTime < 45) {
                greenLed.SetState(1);
            } else if (workTime >= 45 && workTime < 55) {
                greenLed.SetState(0);
                blueLed.SetState(1);
            } else if (workTime >= 55 && workTime < 60) {
                greenLed.SetState(0);
                blueLed.SetState(1);
                redLed.ToggleEvery(.05);
            } else {
                break;
            }
            sleep_ms(10);
            workTime += (float)(time_us_64() - workStart_us) / 1000000.0f;
            lowPowerWarnCycle += (float)(time_us_64() - workStart_us) / 1000000.0f;
        }
        #pragma endregion
    }

    watchdog_reboot(0, 0, 100);
}
#pragma endregion

#pragma region Functions
void mainButton_callback(uint32_t eventMask) 
{
    static alarm_id_t alarmHoldID;
    static uint64_t buttonHoldStart_us; 
    if (mainButton.GetState()) {
        buttonHoldStart_us = time_us_64();
        alarm_pool_init_default();
        alarmHoldID = add_alarm_in_ms(3000, &alarmHoldRestart_callback, NULL, true);
    } else {
        mode++;
        cancel_alarm(alarmHoldID);
    }
}

int64_t alarmHoldRestart_callback(alarm_id_t event, void* USERDATA) {
    if (mainButton.IsPressed()) {
        watchdog_reboot(0,0,0);
    }
    return 1;
}

void core1_main() {

    Drivetrain::DualMotor Drive(12, 16, 17, 18, 15, 14, 13);
    Sensor::Distance DistanceSensor(9, 8);

    const float baseSpeed = 0.6;
    float speed = baseSpeed;

    int needsToTurn = 0;
    mainButton.SetIRQ(GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, &mainButton_callback);

    sleep_ms(500); //Give time for the distance sensor to react

    if (mainButton.GetState() == 0 && DistanceSensor.GetDistance() > 0 ) {
        for (int i = 0; i < 200; i++) {
            redLed.ToggleEvery(.1);
            blueLed.ToggleEvery(.1);
            greenLed.ToggleEvery(.1);

            sleep_ms(10);
        }
        redLed.SetState(0);
        blueLed.SetState(0);
        greenLed.SetState(0);
    } else {
        for(int i = 0; i < 100; i++) {
            redLed.ToggleEvery(.2);
            sleep_ms(10);
        }
        watchdog_reboot(0,0,0);
    }


    multicore_fifo_push_blocking(72);//Let Core0 know I am started

    while (true) {
        #pragma region Pause Mode Core 2
        if (mode % 2 == 0) {
            Drive.Stop();
            Drive.SetState(0);
            printf(" Distance: %.2f \n",DistanceSensor.GetDistance());
        #pragma endregion
        } else {
            #pragma region Work Mode Core 1
            if (workTime < 45) { speed = baseSpeed;}
            else { speed = baseSpeed / 2;}
            float distance = DistanceSensor.GetDistance();
            Drive.SetState(1);

           if ((distance > 0.55 || distance == -1) && needsToTurn == 0) {
                Drive.Forward(speed);
            } else if (needsToTurn <= 60){
                needsToTurn++;
                Drive.Backward(speed);
            } else {
                needsToTurn++;
                Drive.SpinLeft(speed);
                if (needsToTurn >= 100) {
                    needsToTurn = 0;
                }
            }
        }
        #pragma endregion
        sleep_ms(10);
    }
    Drive.Stop();
    Drive.SetState(0);
}
#pragma endregion