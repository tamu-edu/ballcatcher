#define  _CRT_SECURE_NO_DEPRECATE

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <math.h>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

typedef vector<double> doubleVec;
typedef vector<int> intVec;

// return a random integer between x and y
inline int RandInt(int x, int y) {return rand()%(y-x+1)+x;}

// return a random float between 0.0 and 1.0
inline double RandFloat() {return (rand())/(RAND_MAX+1.0);}

// casting given number to a string
/*inline string Stringify(int x) {
  std::ostringstream o;
  o << x;
  return o.str();
}*/

// return a random bool
inline bool RandBool() {
  if(RandInt(0,1)) return true;
  else return false;
}

// return absolute value
inline double Absol(double seed) {
  if(seed<0)
    return -seed;
  else
    return seed;
}

// return a random float in the range -1.0<f<1.0
inline double RandomClamped() {return RandFloat() - RandFloat();}

inline double RandomRanged(double range) {return range*(RandFloat() - RandFloat());}

// convert to a std::string
string itos(int arg);
string ftos(float arg);
string dtos(double arg);

// clamp the first argument between the second two
void Clamp(double &arg, double min, double max);

// write output
bool WriteOutput(const char* fileName, vector<double> toWrite, bool overwrite);
bool WriteOutput(const char* fileName, int toWrite);
bool WriteOutput(const char* fileName, double toWrite);
bool WriteOutput(const char* fileName, vector<intVec> toWrite);
bool WriteOutput(const char* fileName, vector<doubleVec> toWrite);

double to_double(const char* p);

vector<double> ReadFile();

// point to point distance
inline double PtoP(double x1, double y1, double x2, double y2) {
  return sqrt(pow(x1-x2, 2.0)+pow(y1-y2, 2.0));
}

bool inTheVec(vector<int> intVec, int val);
bool inTheVec(vector<double> doubleVec, double val);

#endif
