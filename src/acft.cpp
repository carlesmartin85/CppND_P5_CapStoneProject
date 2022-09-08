
#include "acft.h"

void ACFT::setUp(std::shared_ptr<Image> im_ptr, std::string name, std::string orig, std::string dest, std::vector<std::shared_ptr<Airport>> airports){
    setType(ObjectType::objectACFT);
    setName(name);
    ACFT::setLoc(orig, airports);
    _orig = orig;
    _dest = dest;
    _heading = 0;

    setDest(dest, airports);
    _segment = Cruise;
    _fl = A320::ceiling;
    ACFT::setHdng(orig, airports);
    _ias = 1.4;//A320::IAS[Cruise];
    _im_ptr = im_ptr;
    _ql_statedim = _im_ptr->_dim;
}

void ACFT::setLoc(std::string airport_name, std::vector<std::shared_ptr<Airport>> airports){
    for(std::shared_ptr<Airport> airport : airports){
        if(airport_name == airport->getName()){
             auto [_lon, _lat] = airport->getLoc();
             TrafficObject::setLoc(_lon, _lat);
             return;
        }
    }
}

void ACFT::setHdng(std::string airport_name, std::vector<std::shared_ptr<Airport>> airports){
    for(std::shared_ptr<Airport> airport : airports){
        if(airport_name == airport->getName()){
            _heading = airport->getRwy();
            return;
        }
    }
}

unsigned int ACFT::getFl(){ return _fl;}

std::tuple<double, double> ACFT::getLoc(double t){
    return std::make_tuple(_lon + getIas()*std::sin(getHdng()*M_PI/18.)*t, _lat - getIas()*std::cos(getHdng()*M_PI/18.)*t);
}

unsigned int ACFT::getHdng(){ return _heading; }

double ACFT::getIas(){ return _ias; }

void ACFT::setDest(std::string airport_name, std::vector<std::shared_ptr<Airport>> airports){
    for(std::shared_ptr<Airport> airport : airports){
        if(airport_name == airport->getName()){
            auto [x, y] = airport->getLoc();
            _lonD = x;
            _latD = y;
            return;
        }
    }
}

std::tuple<double, double> ACFT::getDest(){
    return std::make_tuple(_lonD, _latD);
}

void ACFT::simulate(std::vector<std::shared_ptr<TrafficObject>> trafficObjects){
    // launch drive function in a thread
    std::vector<std::shared_ptr<TrafficObject>> trafficObjects_ = trafficObjects;
    threads.emplace_back(std::thread(&ACFT::command, this, trafficObjects_));
}

// virtual function which is executed in a thread
void ACFT::command(std::vector<std::shared_ptr<TrafficObject>> trafficObjects){
    // print id of the current thread
    std::unique_lock<std::mutex> lck(_mtx);
    std::cout << "A/C " << getName() << " driven by thread id = " << std::this_thread::get_id() << std::endl;
    lck.unlock();

    // initalize variables
    //bool hasEnteredIntersection = false;
    double cycleDuration = 1; // duration of a single simulation cycle in ms
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // init stop watch
    lastUpdate = std::chrono::system_clock::now();
    while (true){
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= cycleDuration){

            this->ql_algorithm(trafficObjects);

            this->ql_setNextLoc();
        } // eof if clause
    } // eof simulation loop
}

void ACFT::ql_setUpRewardMatrixbyDest(){
    auto [i0, j0] = _im_ptr->Pixel2State(this->getDest());
    for(size_t i=0; i < _ql_statedim; i++){
        for(size_t j=0; j < _ql_statedim; j++){
            _ql_reward[i][j] += -_ql_destreward*(std::abs((int)i - (int)i0) + std::abs((int)j - (int)j0));
        }
    }
}

void ACFT::ql_setUpRewardMatrixbyTraffic(double t, std::vector<std::shared_ptr<TrafficObject>> trafficObjects){
    for(std::shared_ptr<TrafficObject> trafficObject : trafficObjects){
        if(trafficObject->getId() != getId() && trafficObject->getType() == objectACFT){
            auto [i0, j0] = _im_ptr->Pixel2State(this->getLoc(t));
            for(size_t i = -_ql_separation; i < _ql_separation; i++){
                for(size_t j = -_ql_separation; j < _ql_separation; j++){
                    if((size_t)i < _ql_statedim || (size_t)j < _ql_statedim || (size_t)i >= 0 || (size_t)j >= 0){
                        _ql_reward[i][j] += -_ql_conflictreward;
                    }
                }
            }
        }
    }
}

void ACFT::ql_setUpTransitionProbabilityMatrix(double t){

    // Initzialize transition probability matrix : _ql_transition[i0][j0][a][i1][j1]
    _ql_transition.resize(_ql_statedim, std::vector<std::vector<std::vector<std::vector<double>>>>(_ql_statedim, std::vector<std::vector<std::vector<double>>>(_ql_actiondim, std::vector<std::vector<double>>(_ql_statedim, std::vector<double>(_ql_statedim, 0.)))));
    
    // Fill for Action::StayOnTrack
    size_t i0, j0, a; // indices for _ql_transition[i0][j0][a][i1][j1]
    
    a = Action::TurnLeft;
    for(i0=0;i0<_ql_statedim;i0++){
        for(j0=0;j0<_ql_statedim;j0++){
            auto [x0, y0] = _im_ptr->State2Pixel(std::make_tuple((double)i0, (double)j0));
            auto [x1, y1] = _im_ptr->Pixel2State(getNextLoc(x0, y0, a));
            auto [i1, j1] = _im_ptr->Pixel2State(std::make_tuple((double)x1, (double)y1));

            if((size_t)i1 < _ql_statedim || (size_t)j1 < _ql_statedim || (size_t)i1 >= 0 || (size_t)j1 >= 0){
                _ql_transition[i0][j0][a][(size_t)i1][(size_t)j1] = 1.;
            }
        }
    }

    a = Action::StayOnTrack;
    for(i0=0;i0<_ql_statedim;i0++){
        for(j0=0;j0<_ql_statedim;j0++){
            auto [x0, y0] = _im_ptr->State2Pixel(std::make_tuple((double)i0, (double)j0));
            auto [x1, y1] = _im_ptr->Pixel2State(getNextLoc(x0, y0, a));
            auto [i1, j1] = _im_ptr->Pixel2State(std::make_tuple((double)x1, (double)y1));

            if(((size_t)i1 < _ql_statedim) || ((size_t)j1 < _ql_statedim)){_ql_transition[i0][j0][a][(size_t)i1][(size_t)j1] = 1.;}
        }
    }

    a = Action::TurnRight ;
    for(i0=0;i0<_ql_statedim;i0++){
        for(j0=0;j0<_ql_statedim;j0++){
            auto [x0, y0] = _im_ptr->State2Pixel(std::make_tuple((double)i0, (double)j0));
            auto [x1, y1] = _im_ptr->Pixel2State(getNextLoc(x0, y0, a));
            auto [i1, j1] = _im_ptr->Pixel2State(std::make_tuple((double)x1, (double)y1));

            if(((size_t)i1 < _ql_statedim) || ((size_t)j1 < _ql_statedim)){_ql_transition[i0][j0][a][(size_t)i1][(size_t)j1] = 1.;}
        }
    }
}

std::tuple<double, double> ACFT::getNextLoc(double x0, double y0, size_t a){
    return std::make_tuple(x0 + getIas()*std::sin((getHdng() + ((double)a - 1)*1.)*M_PI/18.)*_ql_delta_t, y0 - getIas()*std::cos((getHdng() + ((double)a - 1)*1.)*M_PI/18.)*_ql_delta_t);
}

void ACFT::ql_setNextLoc(){
    auto [x0 ,y0] = this->getLoc(.0);
    auto [i0, j0] = _im_ptr->Pixel2State(this->getLoc(.0));
    size_t a = _ql_pi_star[i0][j0];
    this->_heading = (size_t)(getHdng() + ((double)a - 1)*1.);
    auto [x1 ,y1] = this->getLoc(_ql_delta_t);
    this->TrafficObject::setLoc(x1, y1);
}

void ACFT::ql_algorithm(std::vector<std::shared_ptr<TrafficObject>> trafficObjects){

    // Initzialize Q and V matrix
    _ql_Q.resize(_ql_statedim, std::vector<std::vector<double>>(_ql_statedim, std::vector<double>(_ql_actiondim, 0.)));
    _ql_V.resize(_ql_statedim, std::vector<double>(_ql_statedim, 0.));
    _ql_pi_star.resize(_ql_statedim, std::vector<double>(_ql_statedim, 1.));

    for(size_t T = 0; T < _ql_forwarditerations; T++){
        _ql_reward.resize(_ql_statedim, std::vector<double>(_ql_statedim, 0.));
        this->ql_setUpRewardMatrixbyDest();
        this->ql_setUpRewardMatrixbyTraffic(0.0/*(double)T*_ql_delta_t*/, trafficObjects);
        this->ql_setUpTransitionProbabilityMatrix(0.0/*(double)T*_ql_delta_t*/);
        for(size_t i0 = 0; i0 < _ql_statedim; i0++){
            for(size_t j0 = 0; j0 < _ql_statedim; j0++){
                for(size_t a = 0; a < _ql_actiondim; a++){
                    for(size_t i1 = 0; i1 < _ql_actiondim; i1++){
                        for(size_t j1 = 0; j1 < _ql_actiondim; j1++){
                            _ql_Q[i0][j0][a] += _ql_transition[i0][j0][a][i1][j1]*(_ql_reward[i0][j0] + _ql_gamma*_ql_V[i1][j1]);
                        }
                    }
                    if(_ql_V[i0][j0] < _ql_Q[i0][j0][a]){
                        _ql_V[i0][j0] = _ql_Q[i0][j0][a];
                        _ql_pi_star[i0][j0] = a;
                    }
                }
            }
        }
    }
    //printMat2D(_ql_V);
}

void ACFT::printMat2D(std::vector<std::vector<double>> mat){
    for(size_t i = 0; i < mat.size(); i++){
        for(size_t j = 0; j < mat[i].size(); j++){
            std::unique_lock<std::mutex> lck(_mtx);
            std::cout << mat[i][j] << " ";
            lck.unlock();
        }
        std::unique_lock<std::mutex> lck(_mtx);
        std::cout << std::endl;
        lck.unlock();
    }
    std::unique_lock<std::mutex> lck(_mtx);
    std::cout << std::endl;
    lck.unlock();
}