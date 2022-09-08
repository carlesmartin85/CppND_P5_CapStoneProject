
#include "trafficobject.h"

// init static variable
int TrafficObject::_idCnt = 0;

std::mutex TrafficObject::_mtx;

TrafficObject::TrafficObject() : _id(_idCnt++){}

std::string TrafficObject::getName(){return _name;}

unsigned int TrafficObject::getHdng(){ return _heading; }

std::tuple<double, double> TrafficObject::getLoc(){ return std::make_tuple(_lon, _lat);}

void TrafficObject::setLoc(std::tuple<double, double> loc){
    auto[_lon, _lat] = loc;
}

void TrafficObject::setUp(std::shared_ptr<Image> im_ptr, std::string name, double lon, double lat){
    _im_ptr = im_ptr;
    _name = name;
    _lon = lon;
    _lat = lat;
}

unsigned int TrafficObject::getFl() { return _fl; }

double TrafficObject::getIas() { return _ias; }

void TrafficObject::setLoc(double lon, double lat){
    _lon = lon;
    _lat = lat;
}

void TrafficObject::setName(std::string name){
    _name = name;
}

void TrafficObject::simulate(std::vector<std::shared_ptr<TrafficObject>> trafficObjects){}