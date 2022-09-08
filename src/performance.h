#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <string>
#include <unordered_map>

enum FlightSegment{
    TakeOff,
    InitialClimb_toFL050,
    InitialClimb_toFL150,
    InitialClimb_toFL240,
    Climb,
    Cruise,
    InitialDescent,
    Descent_toFL100,
    Descent_toFL050,
    Approach,
};

enum Action{
    TurnLeft, // Decrement current heading by 30dg
    StayOnTrack, // Hold current heading
    TurnRight, // Increment current heading by 30dg
};

namespace A320{
static std::unordered_map<FlightSegment, unsigned int> IAS({
                                                  { TakeOff, 145 },
                                                  { InitialClimb_toFL050, 175 },
                                                  { InitialClimb_toFL150, 290 },
                                                  { InitialClimb_toFL240, 290 },
                                                  { Climb, 447 },
                                                  { Cruise, 450 },
                                                  { InitialDescent, 447 },
                                                  { Descent_toFL100, 290 },
                                                  { Descent_toFL050, 210 },
                                                  { Approach, 137 }
                                                                 });
static std::unordered_map<FlightSegment, unsigned int> ROC({
                                                  { TakeOff, 0 },
                                                  { InitialClimb_toFL050, 2500 },
                                                  { InitialClimb_toFL150, 2000 },
                                                  { InitialClimb_toFL240, 1400 },
                                                  { Climb, 1000 },
                                                  { Cruise, 1000 },
                                                  { InitialDescent, 1000 },
                                                  { Descent_toFL100, 3500 },
                                                  { Descent_toFL050, 3500 },
                                                  { Approach, 0 }
                                                                 });
static std::unordered_map<FlightSegment, unsigned int> distance({
                                                  { TakeOff, 2190 },
                                                  { Approach, 1440 }
                                                                 });
static unsigned int ceiling = 410;
}
#endif