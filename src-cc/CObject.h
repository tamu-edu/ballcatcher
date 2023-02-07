#ifndef COBJECT_H
#define COBJECT_H

#include <vector>
#include <math.h>

#include "utils.h"
#include "CParams.h"

using namespace std;

class CObject {
private:
  double x;
  double y;
  double speed;
  bool caught;
  bool fallen;
  bool checked;
  bool ballOrBread;
public:
  CObject();
  CObject(double xval, double yval, double sval);
  ~CObject();

  double getX() {return x;}
  double getY() {return y;}
  double getSpeed() {return speed;}

  bool isFallen() {return fallen;}
  bool isCaught() {return caught;}
  bool isChecked() {return checked;}

  inline void setX(double d) {x = d;}
  inline void setY(double d) {y = d;}
  inline void markFallen() {fallen = true;}
  inline void markCaught() {caught = true;}
  inline void markChecked() {checked = true;}
};

#endif
