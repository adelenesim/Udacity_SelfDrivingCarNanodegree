#include "PID.h"

using namespace std;

/*
* Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
  PID::Kp = Kp; PID::Ki = Ki; PID::Kd = Kd;
  PID::p_error = 0.0; PID::d_error = 0.0;
}

void PID::UpdateError(double cte) {
  if (PID::p_error == 0){
    // need to initialize p_error
    PID::p_error = cte;
  }
  double delta = cte-PID::p_error;
  PID::p_error = cte;
  PID::i_error += cte;
  PID::d_error = delta;

}

double PID::TotalError() {
  return PID::Kp*PID::p_error+PID::Ki*PID::i_error+PID::Kd*PID::d_error;
}

double PID::TotalError(double error) {
  return PID::Kp*error;
}
