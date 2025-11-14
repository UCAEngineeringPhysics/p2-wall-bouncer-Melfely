#include "GPIO.h"

#pragma region PIN

/// @brief Creats and Inits a GPIO Pin.
/// @param pin The GPIOPin number to use 
/// @param output The mode. true for output, false for input.
GPIO::PIN::PIN(uint pin, bool output = true)
: pinID(pin)
{
    //Init the pin for Led Control
    gpio_init(pinID);
    gpio_set_dir(pinID, output);
}

/// @brief Constructor only for PWM GPIO Inits
/// @param pin the pin for the PWM GPIO Init
GPIO::PIN::PIN(uint pin) : pinID(pin)
{

}

/// @brief Toggles the state of the pin, in output mode.
void GPIO::PIN::Toggle() { 
    gpio_put(pinID, !(gpio_get(pinID)));
}

/// @brief Sets the pin output to given state
/// @param state the state to set the pin too
void GPIO::PIN::SetState(bool state) {
    gpio_put(pinID, state);
}

/// @brief Returns the state of GPIOPIN
/// @return the boolean state of the pin
bool GPIO::PIN::GetState() {
    return gpio_get(pinID);
}

/// @brief Returns the cached pin value
/// @return the cached uint value
uint GPIO::PIN::GetPin() {
    return pinID;
}

/// @brief Swaps between on and off, with the given delay as the MINIMUM, they actual delay will be based on loop execution speed
/// @param seconds the intended minimum delay between blinks, a fast code should blink this fast.
void GPIO::PIN::ToggleEvery(float seconds) {

    this->timePassed += (float)(time_us_64() - this->timeAtLastCall_us) / 1000000.0f;
    
    timeAtLastCall_us = time_us_64();
    if (this->timePassed >= seconds) {
        this->Toggle();
        this->timePassed = 0;
    }
}

/// @brief Sets the pull directions, you can set both to true or false, or any other combination.
/// @param PullUp is this pull up?
/// @param PullDown is this pull down?
void GPIO::PIN::SetPulls(bool PullUp, bool PullDown){
    gpio_set_pulls(pinID, PullUp, PullDown);
}

/// @brief Assigns a raw handler to this method, you will need to ackonlwedge the IRQ in your handler. YOU MAY ONLY DECLARE ONE
/// @param eventMask the eventmask that will be used to call the handler
/// @param handler the handler to call when the events trigger, remember to acknowledge the IRQ, and make sure its only being called by the correct event with a check.
void GPIO::PIN::SetIRQ(uint32_t eventMask, irq_handler_t handler) {
    gpio_add_raw_irq_handler(pinID, handler);
    gpio_set_irq_enabled(pinID, eventMask, true);
    this->handler = handler;
    this->eventMask = eventMask;
}

/// @brief deletes all information related to the IRQ handler, and disables all forms of IRQ for that pin, just in case.
void GPIO::PIN::DisableIRQ() {
    gpio_remove_raw_irq_handler(pinID, handler);
    gpio_set_irq_enabled(pinID, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE | GPIO_IRQ_LEVEL_HIGH | GPIO_IRQ_LEVEL_LOW, false);
    this->handler = nullptr;
    this->eventMask = -1;
}

#pragma endregion

#pragma region LED

/// @brief Creates a LED using non-default constructor for GPIOPIN
/// @param pin the GPIO pin to use for LED
GPIO::LED::LED(uint pin) : PIN(pin, true) {

}

#pragma endregion

#pragma region BUTTON 

/// @brief Creates a button using non-default constructor for GPIOPIN
/// @param pin the GPIO pin to use for BUTTON
/// @param IsPullUp if this is a PULL UP button, if not then it will be a PULL DOWN
GPIO::BUTTON::BUTTON(uint pin, bool IsPullUp) : PIN(pin, false) 
{
    if (IsPullUp) {
        gpio_pull_up(pinID);
    } else {
        gpio_pull_down(pinID);
    }
}

/// @brief Checks if the pin is in pull up mode
/// @return this will return true if in pull up mode. False if in pull down.
bool GPIO::BUTTON::IsPullUp() {
    return gpio_is_pulled_up(pinID);
}

/// @brief This method will return if button is pressed. Does properly handle PULL UP or PULL DOWN differences
/// @return 
bool GPIO::BUTTON::IsPressed() {
    if (IsPullUp()) {
        return !gpio_get(pinID);
    } else {
        return gpio_get(pinID);
    }
}

#pragma endregion