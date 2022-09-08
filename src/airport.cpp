
#include "airport.h"

void Airport::setUp(std::shared_ptr<Image> im_ptr, std::string name, double lon, double lat, unsigned int rwy){
    TrafficObject::setType(ObjectType::objectAirport);
    TrafficObject::setUp(im_ptr, name, lon, lat);
    _rwy = rwy;
}

void Airport::simulate(/*std::vector<std::shared_ptr<TrafficObject>> trafficObjects*/){
    //
}

unsigned int Airport::getRwy(){return _rwy;}