#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;

// Please note that the Eigen library does not initialize
// VectorXd or MatrixXd objects with zeros upon creation.

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
    * predict the state
  */
  // Following Udacity class 13
  x_ = F_*x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_*P_*Ft+Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
    * update the state by using Kalman Filter equations
  */
  // Following Udacity class 13
  VectorXd z_pred = H_*x_;
  VectorXd y = z-z_pred;
  MatrixXd Ht = H_.transpose();
  MatrixXd S = H_*P_*Ht+R_;
  MatrixXd PHt = P_*Ht;
  MatrixXd K = PHt*S.inverse();

  //new estimate
  x_ = x_ + (K*y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size,x_size);
  P_ = (I-K*H_)*P_;
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
    * update the state by using Extended Kalman Filter equations
  */
  // Following Udacity class 13
  // Difference in generating y
  VectorXd z_pred(3);
  z_pred<<std::pow(x_(0)*x_(0)+x_(1)*x_(1),0.5),
          std::atan2(x_(1),x_(0)),
          (x_(0)*x_(2)+x_(1)*x_(3))/std::pow(x_(0)*x_(0)+x_(1)*x_(1),0.5);
  VectorXd y = z-z_pred;
  // adjust y(1) to be within -pi and pi
  if (y(1) < 0){
    while (y(1) < -M_PI){y(1) += M_PI;}
  }
  else{
    while (y(1)>M_PI){y(1)-=M_PI;}
  }
  MatrixXd Ht = H_.transpose();
  MatrixXd S = H_*P_*Ht+R_;
  MatrixXd PHt = P_*Ht;
  MatrixXd K = PHt*S.inverse();
    
  //new estimate
  x_ = x_ + (K*y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size,x_size);
  P_ = (I-K*H_)*P_;
}
