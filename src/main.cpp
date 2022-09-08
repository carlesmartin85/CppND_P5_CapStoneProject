#include <iostream>
#include <algorithm>
#include <memory>
#include <thread>
#include <string>
#include <vector>

#include "auxtools.h"
#include "airport.h"
#include "acft.h"
#include "performance.h"
#include "graphics.h"

// United States setup
void createTrafficObjects_US(std::vector<std::shared_ptr<Airport>> &airports, std::vector<std::shared_ptr<ACFT>> &acfts, std::string &filename){
    // background image map filename
    filename = "../data/us.jpg"; // File size: 44.8KiB, width: 680px, height: 480px, frames: 1, type: jpg

    // get dimensions and set up c'tor saved as shared pointer from background imagemap
    std::shared_ptr<Image> im_ptr = std::make_shared<Image>(filename.c_str());

    // Read and set up airport data for designation, location and runway designation
    Reader r_airports("../data/us_airports.dat");
    for (size_t i = 0; i < r_airports.size(); i++){
        airports.push_back(std::make_shared<Airport>());
        airports.at(i)->setUp(im_ptr, std::move(r_airports.getval(i,0)), std::move(std::stod(r_airports.getval(i,1))), std::move(std::stod(r_airports.getval(i,2))), std::move(std::stoi(r_airports.getval(i,3))));
    }

    // Set up aircraft data for origin, destination and estimated time of departure (ETD)
    Reader r_acfts("../data/us_acfts.dat");
    for (size_t i = 0; i < r_acfts.size(); i++){
        acfts.push_back(std::make_shared<ACFT>());
        acfts.at(i)->setUp(im_ptr, std::move(r_acfts.getval(i, 0)), std::move(r_acfts.getval(i, 1)), std::move(r_acfts.getval(i, 2)), airports);
    }

    return;
}

/* Main function */
int main(){

    /* PART 1 : Set up phase */

    // create traffic objects
    std::vector<std::shared_ptr<Airport>> airports; // Airports
    std::vector<std::shared_ptr<ACFT>> acfts; // Aircrafts
    std::string backgroundImg; // Background image
    createTrafficObjects_US(airports, acfts, backgroundImg); // Create traffic objects

    // add all objects into common vector
    std::vector<std::shared_ptr<TrafficObject>> trafficObjects;

    // add all airports into vector
    std::for_each(airports.begin(), airports.end(), [&trafficObjects](std::shared_ptr<Airport> &airport) {
        std::shared_ptr<TrafficObject> trafficObject = std::dynamic_pointer_cast<TrafficObject>(airport);
        trafficObjects.push_back(trafficObject);
    });

    // add all aircrafts into vector
    std::for_each(acfts.begin(), acfts.end(), [&trafficObjects](std::shared_ptr<ACFT> &acft) {
        std::shared_ptr<TrafficObject> trafficObject = std::dynamic_pointer_cast<TrafficObject>(acft);
        trafficObjects.push_back(trafficObject);
    });

    /* PART 2 : simulate traffic objects */

    // simulate traffic objects
    std::for_each(trafficObjects.begin(), trafficObjects.end(), [trafficObjects](std::shared_ptr<TrafficObject> &it) {

        if (it->getType() == ObjectType::objectAirport){
            // cast object type from TrafficObject to Airport
            std::shared_ptr<Airport> airport = std::dynamic_pointer_cast<Airport>(it);

            // simulate
            airport->simulate(/*trafficObjects*/);
        }
        else if (it->getType() == ObjectType::objectACFT){
            // cast object type from TrafficObject to ACFT
            std::shared_ptr<ACFT> acft = std::dynamic_pointer_cast<ACFT>(it);

            // simulate
            acft->simulate(trafficObjects);
        }
    });

    /* PART 3 : Launch visualization */

    // draw all objects in vector
    Graphics *graphics = new Graphics();
    graphics->setBgFilename(backgroundImg);
    graphics->setTrafficObjects(trafficObjects);
    graphics->simulate();

}