
#include "auxtools.h"

Image::Image(const char *fn){
    FILE *f=fopen(fn,"rb");
    if (f==0) {
      _success = false;
      return;
    }
    fseek(f,0,SEEK_END);
    long len=ftell(f);
    fseek(f,0,SEEK_SET);
    if (len<24) {
        fclose(f);
        _success = false;
        return;
        }
  // std::cout << fn << std::endl;
  // Strategy:
  // reading GIF dimensions requires the first 10 bytes of the file
  // reading PNG dimensions requires the first 24 bytes of the file
  // reading JPEG dimensions requires scanning through jpeg chunks
  // In all formats, the file is at least 24 bytes big, so we'll read that always
  unsigned char buf[24]; fread(buf,1,24,f);

  // For JPEGs, we need to read the first 12 bytes of each chunk.
  // We'll read those 12 bytes at buf+2...buf+14, i.e. overwriting the existing buf.
  if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF && buf[3]==0xE0 && buf[6]=='J' && buf[7]=='F' && buf[8]=='I' && buf[9]=='F')
  { long pos=2;
    while (buf[2]==0xFF)
    { if (buf[3]==0xC0 || buf[3]==0xC1 || buf[3]==0xC2 || buf[3]==0xC3 || buf[3]==0xC9 || buf[3]==0xCA || buf[3]==0xCB) break;
      pos += 2+(buf[4]<<8)+buf[5];
      if (pos+12>len) break;
      fseek(f,pos,SEEK_SET); fread(buf+2,1,12,f);
    }
  }

  fclose(f);

  // JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
  if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF)
  { y = (buf[7]<<8) + buf[8];
    x = (buf[9]<<8) + buf[10];
    //std::cout << x << std::endl;
    _success = true;
    return;
  }

  // GIF: first three bytes say "GIF", next three give version number. Then dimensions
  if (buf[0]=='G' && buf[1]=='I' && buf[2]=='F')
  { x = buf[6] + (buf[7]<<8);
    y = buf[8] + (buf[9]<<8);
    _success = true;
    return;
  }

  // PNG: the first frame is by definition an IHDR frame, which gives dimensions
  if ( buf[0]==0x89 && buf[1]=='P' && buf[2]=='N' && buf[3]=='G' && buf[4]==0x0D && buf[5]==0x0A && buf[6]==0x1A && buf[7]==0x0A
    && buf[12]=='I' && buf[13]=='H' && buf[14]=='D' && buf[15]=='R')
  { x = (buf[16]<<24) + (buf[17]<<16) + (buf[18]<<8) + (buf[19]<<0);
    y = (buf[20]<<24) + (buf[21]<<16) + (buf[22]<<8) + (buf[23]<<0);
    _success = true;
    return;
  }
  _success = false;
  return;
}

std::tuple<double, double> Image::Pixel2State(std::tuple<double, double> source){
  auto [source_x, source_y] = source;
  source_x /= x;
  source_x *= _dim;
  source_y /= y;
  source_y *= _dim;
  if(source_x >= (_dim - 1)){source_x = _dim - 1;}
  if(source_y >= (_dim - 1)){source_y = _dim - 1;}
  if(source_x <= 0){source_x = 0;}
  if(source_y <= 0){source_y = 0;}
  return std::make_tuple(source_x, source_y);
}

std::tuple<double, double> Image::State2Pixel(std::tuple<double, double> source){
  auto [source_x, source_y] = source;
  source_x *= x;
  source_x /= _dim;
  source_y *= y;
  source_y /= _dim;
  if(source_x >= (x - 1)){source_x = x - 1;}
  if(source_y >= (y - 1)){source_y = y - 1;}
  if(source_x <= 0){source_x = 0;}
  if(source_y <= 0){source_y = 0;}
  return std::make_tuple(source_x, source_y);
}

Reader::Reader(std::string &&path) {
  
  std::ifstream filestream(path);
  if (filestream) {
    std::string line;
    while (getline(filestream, line)) {
      _lines.push_back(line);
    }
  }
}

std::string Reader::getval(int i0, int j0) {
  std::istringstream linestream(_lines[i0]);
  std::string word;
  for(size_t j = 0; j < j0+1; j++ ) {
    linestream >> word;
  }
  return word;
}

void Image::getPixelDistance(){
  //Reader airportgpsloc("../data/us_airports_wgs84.dat");
  //std::vector<std::string> airport_name;
  //std::vector<double> lat, lon;
  //for(size_t i=0; i<airportgpsloc.size(); i++){
    //airport_name.push_back(airportgpsloc.getval(0, 0));
    //std::cout << "Hola" << std::endl;
    //lat.push_back((int)airportgpsloc.getval(0, 1) + (int)airportgpsloc.getval(0, 2)/60. + (int)airportgpsloc.getval(0, 3)/60./60.);
    //if(airportgpsloc.getval(0, 4) == "S"){lat[i] = -lat[i];}
    //lon.push_back((double)airportgpsloc.getval(0, 5) + (double)airportgpsloc.getval(0, 6)/60. + (double)airportgpsloc.getval(0, 7)/60./60.);
    //if(airportgpsloc.getval(0, 8) == "W"){lon[i] = -lon[i];}
  //}
}