#ifndef TRAFFICOBJECT_H
#define TRAFFICOBJECT_H

#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#include "auxtools.h"

enum ObjectType{
    noObject,
    objectAirport,
    objectACFT,
};

class TrafficObject {
  public:
    TrafficObject();
    virtual void setUp(std::shared_ptr<Image> im_ptr, std::string name, double lon, double lat);
    virtual void setName(std::string name);
    virtual void setLoc(double lon, double lat);
    virtual std::string getName();
    virtual std::tuple<double, double> getLoc();
    virtual void setLoc(std::tuple<double, double> loc);
    ObjectType getType() { return _type; }
    int getId() { return _id; }
    virtual unsigned int getFl();
    virtual double getIas();
    virtual unsigned int getHdng();
    virtual void simulate(std::vector<std::shared_ptr<TrafficObject>> trafficObjects);

  protected:
    void setType(ObjectType objecttype) { _type = objecttype; }

  protected:
    ObjectType _type; // identifies the class type
    int _id; // every traffic object has its own unique id
    double _lon, _lat; // vehicle position in pixels
    std::vector<std::thread> threads; // holds all threads that have been launched within this object
    static std::mutex _mtx; // mutex shared by all traffic objects for protecting cout
    unsigned int _heading; // Azimuth bearing angle
    std::shared_ptr<Image> _im_ptr; // Image size conversion class 
    unsigned int _fl; // Image size conversion class
    double _ias; // Indicated Airspeed

  private:
    std::string _name;
    static int _idCnt;
};

#endif