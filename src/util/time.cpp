/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2016                                                               *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
 ****************************************************************************************/

#include <openspace/util/time.h>

#include <time.h>

#include "time_lua.inl"

#include <openspace/util/spicemanager.h>
#include <openspace/util/syncbuffer.h>

#include <ghoul/filesystem/filesystem.h>
#include <ghoul/misc/assert.h>




namespace openspace {

Time* Time::_instance = nullptr;

Time::Time(double secondsJ2000)
{
    local.time = secondsJ2000;
}


Time::Time(const Time& other)
    : local(other.local)
    , synced(other.synced)
{

}

void Time::initialize() {
    ghoul_assert(_instance == nullptr, "Static time must not have been ininitialized");
    _instance = new Time();
}

void Time::deinitialize() {
    ghoul_assert(_instance, "Static time must have been ininitialized");
    delete _instance;
    _instance = nullptr;
}

Time& Time::ref() {
    ghoul_assert(_instance, "Static time must have been ininitialized");
    return *_instance;
}

Time Time::now() {
    Time now;
    time_t secondsSince1970;
    secondsSince1970 = time(nullptr);
    time_t secondsInAYear = 365.25 * 24 * 60 * 60;
    double secondsSince2000 = (double)(secondsSince1970 - 30*secondsInAYear);
    now.setTime(secondsSince2000);
    return now;
}


bool Time::isInitialized() {
    return (_instance != nullptr);
}

void Time::setTime(double value, bool requireJump) {
    local.time = value;
    local.timeJumped = requireJump;
}

double Time::j2000Seconds() const {
    return local.time;
}

double Time::advanceTime(double tickTime) {
    if (_timePaused)
        return local.time;
    else
        return local.time += local.dt * tickTime;
}

void Time::setDeltaTime(double deltaT) {
    local.dt = deltaT;
}

double Time::deltaTime() const {
    return local.dt;
}

void Time::setPause(bool pause) {
    _timePaused = pause;    
}

bool Time::togglePause() {
    _timePaused = !_timePaused;
    return _timePaused;
}

void Time::setTime(std::string time, bool requireJump) {
    local.time = SpiceManager::ref().ephemerisTimeFromDate(std::move(time));
    local.timeJumped = requireJump;
}

std::string Time::UTC() const {
    return SpiceManager::ref().dateFromEphemerisTime(local.time);
}

std::string Time::ISO8601() const {
    std::string datetime = SpiceManager::ref().dateFromEphemerisTime(local.time);
    std::string month = datetime.substr(5, 3);

    std::string MM = "";
    if (month == "JAN") MM = "01";
    else if (month == "FEB") MM = "02";
    else if (month == "MAR") MM = "03";
    else if (month == "APR") MM = "04";
    else if (month == "MAY") MM = "05";
    else if (month == "JUN") MM = "06";
    else if (month == "JUL") MM = "07";
    else if (month == "AUG") MM = "08";
    else if (month == "SEP") MM = "09";
    else if (month == "OCT") MM = "10";
    else if (month == "NOV") MM = "11";
    else if (month == "DEC") MM = "12";
    else ghoul_assert(false, "Bad month");

    datetime.replace(4, 5, "-" + MM + "-");
    return datetime;
}

void Time::serialize(SyncBuffer* syncBuffer) {
    _syncMutex.lock();
    local.serialize(syncBuffer);
    _syncMutex.unlock();
}

void Time::deserialize(SyncBuffer* syncBuffer) {
    _syncMutex.lock();
    synced.deserialize(syncBuffer);
    _syncMutex.unlock();
}

bool Time::timeJumped() const {
    return local.timeJumped;
}

void Time::setTimeJumped(bool jumped) {
    local.timeJumped = jumped;
}
    
bool Time::paused() const {
    return _timePaused;
}
void Time::updateDoubleBuffer() {
    local = synced;
}


scripting::LuaLibrary Time::luaLibrary() {
    return {
        "time",
        {
            {
                "setDeltaTime",
                &luascriptfunctions::time_setDeltaTime,
                "number",
                "Sets the amount of simulation time that happens "
                "in one second of real time",
                true
            },
            {
                "deltaTime",
                &luascriptfunctions::time_deltaTime,
                "",
                "Returns the amount of simulated time that passes in one "
                "second of real time"
            },
            {
                "setPause",
                &luascriptfunctions::time_setPause,
                "bool",
                "Pauses the simulation time or restores the delta time",
                true
            },
            {
                "togglePause",
                &luascriptfunctions::time_togglePause,
                "",
                "Toggles the pause function, i.e. temporarily setting the delta time to 0"
                " and restoring it afterwards",
                true
            },
            {
                "setTime",
                &luascriptfunctions::time_setTime,
                "{number, string}",
                "Sets the current simulation time to the "
                "specified value. If the parameter is a number, the value is the number "
                "of seconds past the J2000 epoch. If it is a string, it has to be a "
                "valid ISO 8601 date string (YYYY-MM-DDTHH:MN:SS)",
                true
            },
            {
                "currentTime",
                &luascriptfunctions::time_currentTime,
                "",
                "Returns the current time as the number of seconds since "
                "the J2000 epoch"
            },
            {
                "UTC",
                &luascriptfunctions::time_currentTimeUTC,
                "",
                "Returns the current time as an ISO 8601 date string "
                "(YYYY-MM-DDTHH:MN:SS)"
            },
            {
                "currentWallTime",
                &luascriptfunctions::time_currentWallTime,
                "",
                "Returns the current wall time as an ISO 8601 date string "
                "(YYYY-MM-DDTHH-MN-SS) in the UTC timezone"
            }
        }
    };
}

} // namespace openspace
