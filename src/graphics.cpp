
#include "graphics.h"
#include "airport.h"
#include "acft.h"

void Graphics::simulate(){
    this->loadBackgroundImg();
    while (true){
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // update graphics
        this->drawTrafficObjects();
    }
}

void Graphics::loadBackgroundImg(){
    // create window
    _windowName = "EnRoute";
    cv::namedWindow(_windowName, cv::WINDOW_NORMAL);

    // load image and create copy to be used for semi-transparent overlay
    cv::Mat background = cv::imread(_bgFilename);
    _images.push_back(background);         // first element is the original background
    _images.push_back(background.clone()); // second element will be the transparent overlay
    _images.push_back(background.clone()); // third element will be the result image for display
}

void Graphics::drawTrafficObjects()
{
    // reset images
    _images.at(1) = _images.at(0).clone();
    _images.at(2) = _images.at(0).clone();

    // create overlay from all traffic objects
    for (auto it : _trafficObjects){

        auto [posx, posy] = it->TrafficObject::getLoc();

        if (it->getType() == ObjectType::objectAirport)
        {
            // cast object type from TrafficObject to Airport
            std::shared_ptr<Airport> airport = std::dynamic_pointer_cast<Airport>(it);

            // draw airports at location as circles with text label
            cv::Scalar airportColor = cv::Scalar(51, 102, 0);
            cv::circle(_images.at(1), cv::Point2d(posx, posy), 5, airportColor, -1);
            cv::putText(_images.at(1), it->getName(), cv::Point(posx + 5, posy - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, airportColor, 0.25, cv::LINE_8, false);
        }
        else if (it->getType() == ObjectType::objectACFT)
        {
            // cast object type from TrafficObject to ACFT
            std::shared_ptr<ACFT> acft = std::dynamic_pointer_cast<ACFT>(it);

            //std::cout << "IAS = " << acft->getIas() << std::endl;

            // draw aircrafts at location as polygons with text label and orientation
            cv::Scalar aircraftColor = cv::Scalar(0, 0, 0);
            //cv::circle(_images.at(1), cv::Point2d(posx, posy), 5, aircraftColor, -1);
            Graphics::acft_dwg(0, _images.at(1), std::make_tuple(posx, posy), it->getHdng(), 1, aircraftColor);
            cv::putText(_images.at(1), it->getName(), cv::Point(posx + 5*(cos((double)it->getHdng()*M_PI/18.) + sin((double)it->getHdng()*M_PI/18.)), posy - 20 - 5*(sin((double)it->getHdng()*M_PI/18.) + cos((double)it->getHdng()*M_PI/18.))), cv::FONT_HERSHEY_SIMPLEX, 0.3, aircraftColor, 0.25, cv::LINE_8, false);
            cv::putText(_images.at(1), "FL" + std::to_string(it->getFl()), cv::Point(posx + 5*(cos((double)it->getHdng()*M_PI/18.) + sin((double)it->getHdng()*M_PI/18.)), posy - 10 - 5*(sin((double)it->getHdng()*M_PI/18.) + cos((double)it->getHdng()*M_PI/18.))), cv::FONT_HERSHEY_SIMPLEX, 0.3, aircraftColor, 0.25, cv::LINE_8, false);
            cv::putText(_images.at(1), std::to_string((int)(it->getIas())) + "kts", cv::Point(posx + 5*(cos((double)it->getHdng()*M_PI/18.) + sin((double)it->getHdng()*M_PI/18.)), posy + 0 - 5*(sin((double)it->getHdng()*M_PI/18.) + cos((double)it->getHdng()*M_PI/18.))), cv::FONT_HERSHEY_SIMPLEX, 0.3, aircraftColor, 0.25, cv::LINE_8, false);
        }
    }

    float opacity = 0.85;
    cv::addWeighted(_images.at(1), opacity, _images.at(0), 1.0 - opacity, 0, _images.at(2));

    // display background and overlay image
    cv::imshow(_windowName, _images.at(2));
    cv::waitKey(33);
}

void Graphics::acft_dwg(int opt, cv::Mat im, std::tuple<double, double> p0, unsigned int hdg, int w, cv::Scalar aircraftColor){
    double hdg_ = (double)hdg*M_PI/18.;
    double coef = w*0.06;
    int lineType = cv::LINE_8;
    auto [x0, y0] = p0;
    int n = 22;
    if(opt == 0){n = 42;coef = w*0.15;}
    cv::Point acft_pts[1][n];
    int npt[] = { n };
    if(opt == 0){
    acft_pts[0][0]  = cv::Point((int)(x0 + coef*((0.0)*std::cos(hdg_) - (0.0)*std::sin(hdg_))),(int)(y0 + coef*((0.0)*std::sin(hdg_) + (0.0)*std::cos(hdg_))));
    acft_pts[0][1]  = cv::Point((int)(x0 + coef*((-6.0)*std::cos(hdg_) - (2.0)*std::sin(hdg_))),(int)(y0 + coef*((-6.0)*std::sin(hdg_) + (2.0)*std::cos(hdg_))));
    acft_pts[0][2]  = cv::Point((int)(x0 + coef*((-8.0)*std::cos(hdg_) - (7.0)*std::sin(hdg_))),(int)(y0 + coef*((-8.0)*std::sin(hdg_) + (7.0)*std::cos(hdg_))));
    acft_pts[0][3]  = cv::Point((int)(x0 + coef*((-11.0)*std::cos(hdg_) - (13.0)*std::sin(hdg_))),(int)(y0 + coef*((-11.0)*std::sin(hdg_) + (13.0)*std::cos(hdg_))));
    acft_pts[0][4]  = cv::Point((int)(x0 + coef*((-13.0)*std::cos(hdg_) - (20.0)*std::sin(hdg_))),(int)(y0 + coef*((-13.0)*std::sin(hdg_) + (20.0)*std::cos(hdg_))));
    acft_pts[0][5]  = cv::Point((int)(x0 + coef*((-12.0)*std::cos(hdg_) - (25.0)*std::sin(hdg_))),(int)(y0 + coef*((-12.0)*std::sin(hdg_) + (25.0)*std::cos(hdg_))));
    acft_pts[0][6]  = cv::Point((int)(x0 + coef*((-13.0)*std::cos(hdg_) - (32.0)*std::sin(hdg_))),(int)(y0 + coef*((-13.0)*std::sin(hdg_) + (32.0)*std::cos(hdg_))));
    acft_pts[0][7]  = cv::Point((int)(x0 + coef*((-12.0)*std::cos(hdg_) - (38.0)*std::sin(hdg_))),(int)(y0 + coef*((-12.0)*std::sin(hdg_) + (38.0)*std::cos(hdg_))));
    acft_pts[0][8]  = cv::Point((int)(x0 + coef*((-13.0)*std::cos(hdg_) - (44.0)*std::sin(hdg_))),(int)(y0 + coef*((-13.0)*std::sin(hdg_) + (44.0)*std::cos(hdg_))));
    acft_pts[0][9]  = cv::Point((int)(x0 + coef*((-11.0)*std::cos(hdg_) - (53.0)*std::sin(hdg_))),(int)(y0 + coef*((-11.0)*std::sin(hdg_) + (53.0)*std::cos(hdg_))));
    acft_pts[0][10]  = cv::Point((int)(x0 + coef*((-73.0)*std::cos(hdg_) - (92.0)*std::sin(hdg_))),(int)(y0 + coef*((-73.0)*std::sin(hdg_) + (92.0)*std::cos(hdg_))));
    acft_pts[0][11]  = cv::Point((int)(x0 + coef*((-73.0)*std::cos(hdg_) - (109.0)*std::sin(hdg_))),(int)(y0 + coef*((-73.0)*std::sin(hdg_) + (109.0)*std::cos(hdg_))));
    acft_pts[0][12]  = cv::Point((int)(x0 + coef*((-12.0)*std::cos(hdg_) - (90.0)*std::sin(hdg_))),(int)(y0 + coef*((-12.0)*std::sin(hdg_) + (90.0)*std::cos(hdg_))));
    acft_pts[0][13]  = cv::Point((int)(x0 + coef*((-12.0)*std::cos(hdg_) - (126.0)*std::sin(hdg_))),(int)(y0 + coef*((-12.0)*std::sin(hdg_) + (126.0)*std::cos(hdg_))));
    acft_pts[0][14]  = cv::Point((int)(x0 + coef*((-12.0)*std::cos(hdg_) - (131.0)*std::sin(hdg_))),(int)(y0 + coef*((-12.0)*std::sin(hdg_) + (131.0)*std::cos(hdg_))));
    acft_pts[0][15]  = cv::Point((int)(x0 + coef*((-12.0)*std::cos(hdg_) - (90.0)*std::sin(hdg_))),(int)(y0 + coef*((-12.0)*std::sin(hdg_) + (90.0)*std::cos(hdg_))));
    acft_pts[0][16]  = cv::Point((int)(x0 + coef*((-12.0)*std::cos(hdg_) - (130.0)*std::sin(hdg_))),(int)(y0 + coef*((-12.0)*std::sin(hdg_) + (130.0)*std::cos(hdg_))));
    acft_pts[0][17]  = cv::Point((int)(x0 + coef*((-13.0)*std::cos(hdg_) - (132.0)*std::sin(hdg_))),(int)(y0 + coef*((-13.0)*std::sin(hdg_) + (132.0)*std::cos(hdg_))));
    acft_pts[0][18]  = cv::Point((int)(x0 + coef*((-27.0)*std::cos(hdg_) - (147.0)*std::sin(hdg_))),(int)(y0 + coef*((-27.0)*std::sin(hdg_) + (147.0)*std::cos(hdg_))));
    acft_pts[0][19]  = cv::Point((int)(x0 + coef*((-27.0)*std::cos(hdg_) - (154.0)*std::sin(hdg_))),(int)(y0 + coef*((-27.0)*std::sin(hdg_) + (154.0)*std::cos(hdg_))));
    acft_pts[0][20]  = cv::Point((int)(x0 + coef*((-8.0)*std::cos(hdg_) - (148.0)*std::sin(hdg_))),(int)(y0 + coef*((-8.0)*std::sin(hdg_) + (148.0)*std::cos(hdg_))));
    acft_pts[0][21]  = cv::Point((int)(x0 + coef*((0.0)*std::cos(hdg_) - (150.0)*std::sin(hdg_))),(int)(y0 + coef*((0.0)*std::sin(hdg_) + (150.0)*std::cos(hdg_))));
    acft_pts[0][22]  = cv::Point((int)(x0 + coef*((8.0)*std::cos(hdg_) - (148.0)*std::sin(hdg_))),(int)(y0 + coef*((8.0)*std::sin(hdg_) + (148.0)*std::cos(hdg_))));
    acft_pts[0][23]  = cv::Point((int)(x0 + coef*((27.0)*std::cos(hdg_) - (154.0)*std::sin(hdg_))),(int)(y0 + coef*((27.0)*std::sin(hdg_) + (154.0)*std::cos(hdg_))));
    acft_pts[0][24]  = cv::Point((int)(x0 + coef*((27.0)*std::cos(hdg_) - (147.0)*std::sin(hdg_))),(int)(y0 + coef*((27.0)*std::sin(hdg_) + (147.0)*std::cos(hdg_))));
    acft_pts[0][25]  = cv::Point((int)(x0 + coef*((13.0)*std::cos(hdg_) - (132.0)*std::sin(hdg_))),(int)(y0 + coef*((13.0)*std::sin(hdg_) + (132.0)*std::cos(hdg_))));
    acft_pts[0][26]  = cv::Point((int)(x0 + coef*((12.0)*std::cos(hdg_) - (130.0)*std::sin(hdg_))),(int)(y0 + coef*((12.0)*std::sin(hdg_) + (130.0)*std::cos(hdg_))));
    acft_pts[0][27]  = cv::Point((int)(x0 + coef*((12.0)*std::cos(hdg_) - (90.0)*std::sin(hdg_))),(int)(y0 + coef*((12.0)*std::sin(hdg_) + (90.0)*std::cos(hdg_))));
    acft_pts[0][28]  = cv::Point((int)(x0 + coef*((12.0)*std::cos(hdg_) - (131.0)*std::sin(hdg_))),(int)(y0 + coef*((12.0)*std::sin(hdg_) + (131.0)*std::cos(hdg_))));
    acft_pts[0][29]  = cv::Point((int)(x0 + coef*((12.0)*std::cos(hdg_) - (126.0)*std::sin(hdg_))),(int)(y0 + coef*((12.0)*std::sin(hdg_) + (126.0)*std::cos(hdg_))));
    acft_pts[0][30]  = cv::Point((int)(x0 + coef*((12.0)*std::cos(hdg_) - (90.0)*std::sin(hdg_))),(int)(y0 + coef*((12.0)*std::sin(hdg_) + (90.0)*std::cos(hdg_))));
    acft_pts[0][31]  = cv::Point((int)(x0 + coef*((73.0)*std::cos(hdg_) - (109.0)*std::sin(hdg_))),(int)(y0 + coef*((73.0)*std::sin(hdg_) + (109.0)*std::cos(hdg_))));
    acft_pts[0][32]  = cv::Point((int)(x0 + coef*((73.0)*std::cos(hdg_) - (92.0)*std::sin(hdg_))),(int)(y0 + coef*((73.0)*std::sin(hdg_) + (92.0)*std::cos(hdg_))));
    acft_pts[0][33]  = cv::Point((int)(x0 + coef*((11.0)*std::cos(hdg_) - (53.0)*std::sin(hdg_))),(int)(y0 + coef*((11.0)*std::sin(hdg_) + (53.0)*std::cos(hdg_))));
    acft_pts[0][34]  = cv::Point((int)(x0 + coef*((13.0)*std::cos(hdg_) - (44.0)*std::sin(hdg_))),(int)(y0 + coef*((13.0)*std::sin(hdg_) + (44.0)*std::cos(hdg_))));
    acft_pts[0][35]  = cv::Point((int)(x0 + coef*((12.0)*std::cos(hdg_) - (38.0)*std::sin(hdg_))),(int)(y0 + coef*((12.0)*std::sin(hdg_) + (38.0)*std::cos(hdg_))));
    acft_pts[0][36]  = cv::Point((int)(x0 + coef*((13.0)*std::cos(hdg_) - (32.0)*std::sin(hdg_))),(int)(y0 + coef*((13.0)*std::sin(hdg_) + (32.0)*std::cos(hdg_))));
    acft_pts[0][37]  = cv::Point((int)(x0 + coef*((12.0)*std::cos(hdg_) - (25.0)*std::sin(hdg_))),(int)(y0 + coef*((12.0)*std::sin(hdg_) + (25.0)*std::cos(hdg_))));
    acft_pts[0][38]  = cv::Point((int)(x0 + coef*((13.0)*std::cos(hdg_) - (20.0)*std::sin(hdg_))),(int)(y0 + coef*((13.0)*std::sin(hdg_) + (20.0)*std::cos(hdg_))));
    acft_pts[0][39]  = cv::Point((int)(x0 + coef*((11.0)*std::cos(hdg_) - (13.0)*std::sin(hdg_))),(int)(y0 + coef*((11.0)*std::sin(hdg_) + (13.0)*std::cos(hdg_))));
    acft_pts[0][40]  = cv::Point((int)(x0 + coef*((8.0)*std::cos(hdg_) - (7.0)*std::sin(hdg_))),(int)(y0 + coef*((8.0)*std::sin(hdg_) + (7.0)*std::cos(hdg_))));
    acft_pts[0][41]  = cv::Point((int)(x0 + coef*((6.0)*std::cos(hdg_) - (2.0)*std::sin(hdg_))),(int)(y0 + coef*((6.0)*std::sin(hdg_) + (2.0)*std::cos(hdg_))));
    } else {
    acft_pts[0][0]  = cv::Point((int)(x0 + coef*((0.0)*std::cos(hdg_) - (0.0)*std::sin(hdg_))),(int)(y0 + coef*((0.0)*std::sin(hdg_) + (0.0)*std::cos(hdg_))));
    acft_pts[0][1]  = cv::Point((int)(x0 + coef*((-22.0)*std::cos(hdg_) - (32.0)*std::sin(hdg_))),(int)(y0 + coef*((-22.0)*std::sin(hdg_) + (32.0)*std::cos(hdg_))));
    acft_pts[0][2]  = cv::Point((int)(x0 + coef*((-29.0)*std::cos(hdg_) - (114.0)*std::sin(hdg_))),(int)(y0 + coef*((-29.0)*std::sin(hdg_) + (114.0)*std::cos(hdg_))));
    acft_pts[0][3]  = cv::Point((int)(x0 + coef*((-146.0)*std::cos(hdg_) - (206.0)*std::sin(hdg_))),(int)(y0 + coef*((-146.0)*std::sin(hdg_) + (206.0)*std::cos(hdg_))));
    acft_pts[0][4]  = cv::Point((int)(x0 + coef*((-146.0)*std::cos(hdg_) - (260.0)*std::sin(hdg_))),(int)(y0 + coef*((-146.0)*std::sin(hdg_) + (260.0)*std::cos(hdg_))));
    acft_pts[0][5]  = cv::Point((int)(x0 + coef*((-62.0)*std::cos(hdg_) - (211.0)*std::sin(hdg_))),(int)(y0 + coef*((-62.0)*std::sin(hdg_) + (211.0)*std::cos(hdg_))));
    acft_pts[0][6]  = cv::Point((int)(x0 + coef*((-29.0)*std::cos(hdg_) - (211.0)*std::sin(hdg_))),(int)(y0 + coef*((-29.0)*std::sin(hdg_) + (211.0)*std::cos(hdg_))));
    acft_pts[0][7]  = cv::Point((int)(x0 + coef*((-19.0)*std::cos(hdg_) - (302.0)*std::sin(hdg_))),(int)(y0 + coef*((-19.0)*std::sin(hdg_) + (302.0)*std::cos(hdg_))));
    acft_pts[0][8]  = cv::Point((int)(x0 + coef*((-57.0)*std::cos(hdg_) - (324.0)*std::sin(hdg_))),(int)(y0 + coef*((-57.0)*std::sin(hdg_) + (324.0)*std::cos(hdg_))));
    acft_pts[0][9]  = cv::Point((int)(x0 + coef*((-57.0)*std::cos(hdg_) - (356.0)*std::sin(hdg_))),(int)(y0 + coef*((-57.0)*std::sin(hdg_) + (356.0)*std::cos(hdg_))));
    acft_pts[0][10]  = cv::Point((int)(x0 + coef*((-6.0)*std::cos(hdg_) - (345.0)*std::sin(hdg_))),(int)(y0 + coef*((-6.0)*std::sin(hdg_) + (345.0)*std::cos(hdg_))));
    acft_pts[0][11]  = cv::Point((int)(x0 + coef*((0.0)*std::cos(hdg_) - (356.0)*std::sin(hdg_))),(int)(y0 + coef*((0.0)*std::sin(hdg_) + (356.0)*std::cos(hdg_))));
    acft_pts[0][12]  = cv::Point((int)(x0 + coef*((6.0)*std::cos(hdg_) - (345.0)*std::sin(hdg_))),(int)(y0 + coef*((6.0)*std::sin(hdg_) + (345.0)*std::cos(hdg_))));
    acft_pts[0][13]  = cv::Point((int)(x0 + coef*((57.0)*std::cos(hdg_) - (356.0)*std::sin(hdg_))),(int)(y0 + coef*((57.0)*std::sin(hdg_) + (356.0)*std::cos(hdg_))));
    acft_pts[0][14]  = cv::Point((int)(x0 + coef*((57.0)*std::cos(hdg_) - (324.0)*std::sin(hdg_))),(int)(y0 + coef*((57.0)*std::sin(hdg_) + (324.0)*std::cos(hdg_))));
    acft_pts[0][15]  = cv::Point((int)(x0 + coef*((19.0)*std::cos(hdg_) - (302.0)*std::sin(hdg_))),(int)(y0 + coef*((19.0)*std::sin(hdg_) + (302.0)*std::cos(hdg_))));
    acft_pts[0][16]  = cv::Point((int)(x0 + coef*((29.0)*std::cos(hdg_) - (211.0)*std::sin(hdg_))),(int)(y0 + coef*((29.0)*std::sin(hdg_) + (211.0)*std::cos(hdg_))));
    acft_pts[0][17]  = cv::Point((int)(x0 + coef*((62.0)*std::cos(hdg_) - (211.0)*std::sin(hdg_))),(int)(y0 + coef*((62.0)*std::sin(hdg_) + (211.0)*std::cos(hdg_))));
    acft_pts[0][18]  = cv::Point((int)(x0 + coef*((146.0)*std::cos(hdg_) - (260.0)*std::sin(hdg_))),(int)(y0 + coef*((146.0)*std::sin(hdg_) + (260.0)*std::cos(hdg_))));
    acft_pts[0][19]  = cv::Point((int)(x0 + coef*((146.0)*std::cos(hdg_) - (206.0)*std::sin(hdg_))),(int)(y0 + coef*((146.0)*std::sin(hdg_) + (206.0)*std::cos(hdg_))));
    acft_pts[0][20]  = cv::Point((int)(x0 + coef*((29.0)*std::cos(hdg_) - (114.0)*std::sin(hdg_))),(int)(y0 + coef*((29.0)*std::sin(hdg_) + (114.0)*std::cos(hdg_))));
    acft_pts[0][21]  = cv::Point((int)(x0 + coef*((22.0)*std::cos(hdg_) - (32.0)*std::sin(hdg_))),(int)(y0 + coef*((22.0)*std::sin(hdg_) + (32.0)*std::cos(hdg_))));
    }
    const cv::Point* ppt[1] = { acft_pts[0] };
    cv::fillPoly (im, ppt, npt, 1, aircraftColor, lineType);
}