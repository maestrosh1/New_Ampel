#pragma once
#include <Arduino.h>
#include <array>
enum class TimerState
{
    IDLE,
    RUNNING,
    PAUSED,
    STOPPING
};

struct RGBColor
{
    uint8_t r, g, b;

    bool operator!=(const RGBColor &other) const
    {
        return r != other.r || g != other.g || b != other.b;
    }
};

class Timer
{
public:
    Timer(uint32_t);
    void start();
    void pause(RGBColor);
    void resume(RGBColor);
    void tick();
    void add(uint32_t, RGBColor);
    void stop();

    // gpt
    void updateDuration(uint32_t new_duration);
    // --

    RGBColor getLastColor() const;
    uint32_t getRemainingTime() const;
    TimerState getState();


private:
    TimerState state = TimerState::IDLE;
    uint32_t endMillis = 0;
    uint32_t remainingMilllis = 0;
    RGBColor lastLedColor = {0, 0, 0};
    uint32_t max_duration;
    RGBColor calcColor(uint32_t);
    void before_stop();

};

bool kill_timer();


extern Timer timer; 
