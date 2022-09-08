#ifndef AIRPORT_H
#define AIRPORT_H

#include <string>
#include <memory>

#include "trafficobject.h"
#include "auxtools.h"

class Airport: public TrafficObject, public std::enable_shared_from_this<Airport> {

  public:
    void setUp(std::shared_ptr<Image> im_ptr, std::string name, double lon, double lat, unsigned int rwy);
    unsigned int getRwy();
    void simulate(/*std::vector<std::shared_ptr<TrafficObject>> trafficObjects*/);

    std::shared_ptr<Airport> getptr() {
        return shared_from_this();
    }

  private:
    unsigned int _rwy;
};

#endif