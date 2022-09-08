#ifndef AUXTOOLS_H
#define AUXTOOLS_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>

class Image{
    public:
      Image(const char *fn);
      std::tuple<double, double> Pixel2State(std::tuple<double, double> source);
      std::tuple<double, double> State2Pixel(std::tuple<double, double> source);
      void getPixelDistance();
      unsigned int _dim{5};

    protected:
      //

    private:
      double _pixeldistance{3.};
      bool _success{false};
      int x;
      int y;

};

class Reader {
 public:
   Reader(std::string &&path);
   std::string getval(int i0, int j0);
   int size(){ return _lines.size(); }

 private:
   std::vector<std::string> _lines;
};

#endif