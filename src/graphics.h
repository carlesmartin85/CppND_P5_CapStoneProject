#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <thread>
#include <cmath>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "trafficobject.h"

class Graphics{
  public:
    // constructor / destructor

    // getters / setters
    void setBgFilename(std::string filename) { _bgFilename = filename; }
    void setTrafficObjects(std::vector<std::shared_ptr<TrafficObject>> &trafficObjects) { _trafficObjects = trafficObjects; };

    // typical behaviour methods
    void simulate();

  protected:
    void acft_dwg(int opt, cv::Mat im, std::tuple<double, double> p0, unsigned int hdg, int w, cv::Scalar aircraftColor);

  private:
    // typical behaviour methods
    void loadBackgroundImg();
    void drawTrafficObjects();

    // member variables
    std::vector<std::shared_ptr<TrafficObject>> _trafficObjects;
    std::string _bgFilename;
    std::string _windowName;
    std::vector<cv::Mat> _images;
};

#endif