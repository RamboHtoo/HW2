#include "TimeCode.h"
#include <stdexcept>
#include <string>

using namespace std;

long long unsigned int TimeCode::ComponentsToSeconds(
    unsigned int hr,
    unsigned int min,
    unsigned long long int sec)
{
    // Store TimeCode as one total number of seconds so rollover
    // happens naturally for large minute and second values.
    return hr * 3600ULL + min * 60ULL + sec;
}

void TimeCode::GetComponents(
    unsigned int& hr,
    unsigned int& min,
    unsigned int& sec) const
{
    // Output references let one conversion return all three components
    // without storing hours, minutes, or seconds as instance variables.
    hr = t / 3600;

    // Quotient and remainder keep minutes and seconds normalized to 0-59.
    long long unsigned int remaining = t % 3600;

    min = remaining / 60;
    sec = remaining % 60;
}

TimeCode::TimeCode(
    unsigned int hr,
    unsigned int min,
    long long unsigned int sec)
{
    // The constructor allows rollover, so converting everything directly
    // to total seconds handles oversized minute and second values.
    t = ComponentsToSeconds(hr, min, sec);
}

TimeCode::TimeCode(const TimeCode& tc)
{
    t = tc.t;
}

unsigned int TimeCode::GetHours() const
{
    unsigned int hr;
    unsigned int min;
    unsigned int sec;

    GetComponents(hr, min, sec);

    return hr;
}

unsigned int TimeCode::GetMinutes() const
{
    unsigned int hr;
    unsigned int min;
    unsigned int sec;

    GetComponents(hr, min, sec);

    return min;
}

unsigned int TimeCode::GetSeconds() const
{
    unsigned int hr;
    unsigned int min;
    unsigned int sec;

    GetComponents(hr, min, sec);

    return sec;
}

string TimeCode::ToString() const
{
    unsigned int hr;
    unsigned int min;
    unsigned int sec;

    GetComponents(hr, min, sec);

    return to_string(hr) + ":" +
           to_string(min) + ":" +
           to_string(sec);
}

void TimeCode::reset()
{
    t = 0;
}

void TimeCode::SetHours(unsigned int hours)
{
    unsigned int hr;
    unsigned int min;
    unsigned int sec;

    // Preserve the current minutes and seconds while replacing only the hours.
    GetComponents(hr, min, sec);

    t = ComponentsToSeconds(hours, min, sec);
}

void TimeCode::SetMinutes(unsigned int minutes)
{
    // SetMinutes changes only the minute component.
    // Unlike the constructor, setters do not allow rollover.
    if (minutes > 59)
    {
        throw invalid_argument("Minutes must be between 0 and 59");
    }

    unsigned int hr;
    unsigned int min;
    unsigned int sec;

    // Break the current total seconds into components so
    // the hours and seconds can remain unchanged.
    GetComponents(hr, min, sec);

    t = ComponentsToSeconds(hr, minutes, sec);
}

void TimeCode::SetSeconds(unsigned int seconds)
{
    // Seconds must stay between 0 and 59 because setters
    // do not allow rollover.
    if (seconds > 59)
    {
        throw invalid_argument("Seconds must be between 0 and 59");
    }

    unsigned int hr;
    unsigned int min;
    unsigned int sec;

    // Preserve the current hours and minutes while replacing only seconds.
    GetComponents(hr, min, sec);

    t = ComponentsToSeconds(hr, min, seconds);
}

TimeCode TimeCode::operator+(const TimeCode& other) const
{
    TimeCode result;

    // Because both TimeCodes are stored as total seconds,
    // addition automatically handles minute and hour rollover.
    result.t = t + other.t;
    return result;
}

TimeCode TimeCode::operator-(const TimeCode& other) const
{
    // TimeCodes cannot be negative, so prevent subtracting
    // a larger duration from a smaller one.
    if (other.t > t)
    {
        throw invalid_argument("TimeCode cannot be negative");
    }

    TimeCode result;
    result.t = t - other.t;

    return result;
}

TimeCode TimeCode::operator*(double a) const
{
    // A negative multiplier would create a negative TimeCode,
    // which is not allowed for this assignment.
    if (a < 0)
    {
        throw invalid_argument("Negative arguments not allowed");
    }

    TimeCode result;
    result.t = t * a;

    return result;
}

TimeCode TimeCode::operator/(double a) const
{
    // Negative TimeCodes are not allowed.
    if (a < 0)
    {
        throw invalid_argument("Negative arguments not allowed");
    }

    // Division by zero is invalid, so stop before performing the operation.
    if (a == 0)
    {
        throw invalid_argument("Cannot divide by zero");
    }

    TimeCode result;
    result.t = t / a;

    return result;
}

// Since t stores the entire duration in seconds,
// comparing TimeCodes is the same as comparing their t values.
bool TimeCode::operator==(const TimeCode& other) const
{
    return t == other.t;
}

bool TimeCode::operator!=(const TimeCode& other) const
{
    return t != other.t;
}

bool TimeCode::operator<(const TimeCode& other) const
{
    return t < other.t;
}

bool TimeCode::operator<=(const TimeCode& other) const
{
    return t <= other.t;
}

bool TimeCode::operator>(const TimeCode& other) const
{
    return t > other.t;
}

bool TimeCode::operator>=(const TimeCode& other) const
{
    return t >= other.t;
}

// I used ChatGPT to look up discuss C++ concepts, test-case ideas, and code review.