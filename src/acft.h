#ifndef ACFT_H
#define ACFT_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <tuple>
#include <cmath>
#include <bits/stdc++.h>

#include "trafficobject.h"
#include "airport.h"
#include "performance.h"

class ACFT: public TrafficObject, public std::enable_shared_from_this<ACFT> {

  public:
    void setUp(std::shared_ptr<Image> im_ptr, std::string name, std::string orig, std::string dest, std::vector<std::shared_ptr<Airport>> airports);
    std::tuple<double, double> getLoc(double t);
    unsigned int getFl();
    std::tuple<double, double> getDest();
    void setLoc(std::string airport_name, std::vector<std::shared_ptr<Airport>> airports);
    void setHdng(std::string airport_name, std::vector<std::shared_ptr<Airport>> airports);
    unsigned int getHdng();
    double getIas();
    void setDest(std::string airport_name, std::vector<std::shared_ptr<Airport>> airports);
    void simulate(std::vector<std::shared_ptr<TrafficObject>> trafficObjects);
    void command(std::vector<std::shared_ptr<TrafficObject>> trafficObjects);
    void printMat2D(std::vector<std::vector<double>> mat);

    std::shared_ptr<ACFT> getptr() {
        return shared_from_this();
    }

  protected:
    //

  private:
    std::string _orig; // Originating airport
    std::string _dest; // Destination airport
    double _lonD; // Destination airport longitud
    double _latD; // Destination airport latitude
    FlightSegment _segment; // Flight segment

  // Qlearning

  public:
    void ql_setUpRewardMatrixbyDest();
    void ql_setUpRewardMatrixbyTraffic(double t, std::vector<std::shared_ptr<TrafficObject>> trafficObjects);
    void ql_setUpTransitionProbabilityMatrix(double t);
    std::tuple<double, double> getNextLoc(double x0, double y0, size_t a);
    void ql_setNextLoc();
    void ql_algorithm(std::vector<std::shared_ptr<TrafficObject>> trafficObjects);

  protected:
    double _ql_delta_t{1.0};
    double _ql_destreward{10.};
    double _ql_conflictreward{1};
    double _ql_gamma{0.8};
    int _ql_separation{2};
    int _ql_forwarditerations{10};
    int _ql_statedim;
    int _ql_actiondim{3};
    std::vector<std::vector<std::vector<std::vector<std::vector<double>>>>> _ql_transition;
    std::vector<std::vector<std::vector<double>>> _ql_Q;
    std::vector<std::vector<double>> _ql_V;
    std::vector<std::vector<double>> _ql_pi_star;
    std::vector<std::vector<double>> _ql_reward;
    std::vector<std::vector<double>> _ql_valuefunction;
    std::vector<std::vector<double>> _ql_qfunction;

  private:
    //

};

#endif