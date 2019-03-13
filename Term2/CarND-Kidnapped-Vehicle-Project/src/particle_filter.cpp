/*
 * particle_filter.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: Tiffany Huang
 */

#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h> 
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>

#include "particle_filter.h"

using namespace std;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
	// Set the number of particles. Initialize all particles to first position (based on estimates of
	//   x, y, theta and their uncertainties from GPS) and all weights to 1. 
	// Add random Gaussian noise to each particle.
	// NOTE: Consult particle_filter.h for more information about this method (and others in this file).
    // Following the Udacity class
    
    // Create Gaussian noise generator
    std::normal_distribution<double> dist_x(x, std[0]);
    std::normal_distribution<double> dist_y(y, std[1]);
    std::normal_distribution<double> dist_theta(theta, std[3]);
    
    // Random engine
    std::default_random_engine gen;
    num_particles = 1000;
    for (int i = 0; i < num_particles; ++i) {
        // add to the particles vector
        Particle currparticle;
        currparticle.x = dist_x(gen);
        currparticle.y = dist_y(gen);
        currparticle.theta = dist_theta(gen);
        currparticle.weight = 1;
        weights.push_back(1);
        particles.push_back(currparticle);
    }
    is_initialized = true;
}

void ParticleFilter::prediction(double delta_t, double std_pos[], double velocity, double yaw_rate) {
	// Add measurements to each particle and add random Gaussian noise.
	// NOTE: When adding noise you may find std::normal_distribution and std::default_random_engine useful.
	//  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
	//  http://www.cplusplus.com/reference/random/default_random_engine/
    
    
    // Random engine
    std::default_random_engine gen;
    
    if (fabs(yaw_rate)>0.0001){
        // update each particle
        for (int i = 0; i < num_particles; ++i) {
            // x
            double currx = particles[i].x + velocity/yaw_rate*(std::sin(particles[i].theta+yaw_rate*delta_t)-std::sin(particles[i].theta));
            // y
            double curry = particles[i].y + velocity/yaw_rate*(std::cos(particles[i].theta)-std::cos(particles[i].theta+yaw_rate*delta_t));
            // theta
            double currtheta = particles[i].theta + yaw_rate*delta_t;
            
            // add noise
            // Create Gaussian noise generator
            std::normal_distribution<double> dist_x(currx, std_pos[0]);
            std::normal_distribution<double> dist_y(curry, std_pos[1]);
            std::normal_distribution<double> dist_theta(currtheta, std_pos[3]);
            particles[i].x = dist_x(gen);
            particles[i].y = dist_y(gen);
            particles[i].theta = dist_theta(gen);
        }
    }
    else{
        // yaw rate is ~zero
        // update each particle
        for (int i = 0; i < num_particles; ++i) {
            // x
            double currx = particles[i].x + velocity*delta_t*std::cos(particles[i].theta);
            // y
            double curry = particles[i].y + velocity*delta_t*std::sin(particles[i].theta);
            // theta
            double currtheta = particles[i].theta;
            
            // add noise
            // Create Gaussian noise generator
            std::normal_distribution<double> dist_x(currx, std_pos[0]);
            std::normal_distribution<double> dist_y(curry, std_pos[1]);
            std::normal_distribution<double> dist_theta(currtheta, std_pos[3]);
            particles[i].x = dist_x(gen);
            particles[i].y = dist_y(gen);
            particles[i].theta = dist_theta(gen);
        }
    }
}

void ParticleFilter::dataAssociation(std::vector<LandmarkObs> predicted, std::vector<LandmarkObs>& observations) {
	// Find the predicted measurement that is closest to each observed measurement and assign the
	//   observed measurement to this particular landmark.
	// NOTE: this method will NOT be called by the grading code. But you will probably find it useful to 
	//   implement this method and use it as a helper during the updateWeights phase.
    for (int i = 0; i < observations.size(); ++i) {
        double bestdist = std::numeric_limits<double>::infinity();
        for (int j = 0; j < predicted.size(); ++j) {
            double currdist = dist(observations[i].x,observations[i].y,predicted[j].x,predicted[j].y);
            if (currdist<bestdist){
                observations[i].id = predicted[j].id;
                bestdist = currdist;
            }
        }
    }
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
		const std::vector<LandmarkObs> &observations, const Map &map_landmarks) {
	// Update the weights of each particle using a mult-variate Gaussian distribution. You can read
	//   more about this distribution here: https://en.wikipedia.org/wiki/Multivariate_normal_distribution
	// NOTE: The observations are given in the VEHICLE'S coordinate system. Your particles are located
	//   according to the MAP'S coordinate system. You will need to transform between the two systems.
	//   Keep in mind that this transformation requires both rotation AND translation (but no scaling).
	//   The following is a good resource for the theory:
	//   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
	//   and the following is a good resource for the actual equation to implement (look at equation 
	//   3.33
	//   http://planning.cs.uiuc.edu/node99.html
    for (int p = 0; p < num_particles; ++p){
        // for each particle, we map all the observations to map space
        // based on particle information
        std::vector<LandmarkObs> observationsmap;
        double sinparticle = std::sin(particles[p].theta);
        double cosparticle = std::cos(particles[p].theta);
        for (int i = 0; i < observations.size(); ++i) {
            LandmarkObs currobs;
            currobs.x = cosparticle*observations[i].x-sinparticle*observations[i].y+particles[p].x;
            currobs.y = sinparticle*observations[i].x+cosparticle*observations[i].y+particles[p].y;
            currobs.id = -1;
            observationsmap.push_back(currobs);
        }
        // for speed up, prune away those landmarks that are too far from the particle
        // and outside sensor_range
        std::vector<LandmarkObs> predicted;
        for (int i = 0; i < map_landmarks.landmark_list.size(); ++i) {
            double currdist = dist(map_landmarks.landmark_list[i].x_f, map_landmarks.landmark_list[i].y_f, particles[p].x,particles[p].y);
            if (currdist <= sensor_range){
                LandmarkObs currobs;
                currobs.x = map_landmarks.landmark_list[i].x_f;
                currobs.y = map_landmarks.landmark_list[i].y_f;
                currobs.id = map_landmarks.landmark_list[i].id_i;
                predicted.push_back(currobs);
            }
        }
        // if no predicted landmarks in range, set weight to zero
        if (predicted.size() == 0){
            particles[p].weight = 0;
            weights[p] = 0;
        }
        else{
            // find best assignments
            dataAssociation(predicted, observationsmap);
            std::vector<int> associations;
            std::vector<double> sense_x;
            std::vector<double> sense_y;
            // set associations
            for (int i = 0; i < observationsmap.size(); ++i) {
                associations.push_back(observationsmap[i].id);
                for (int j = 0; j < predicted.size(); ++j) {
                    if (predicted[j].id == observationsmap[i].id){
                        sense_x.push_back(predicted[j].x); sense_y.push_back(predicted[j].y);
                        break;
                    }
                }
            }
            //set associations
            SetAssociations(particles[p],associations,sense_x,sense_y);
            
            // calculate probabilities, following Udacity class
            // normalization term
            double gauss_norm = (1/(2*M_PI*std_landmark[0]*std_landmark[1]));
            // loop over each observation
            double currweight = 1;
            for (int i = 0; i < associations.size(); ++i) {
                double mu_x = sense_x[i]; double mu_y = sense_y[i];
                double exponent = (std::pow(observationsmap[i].x-mu_x,2))/(2*std::pow(std_landmark[0],2))+(std::pow(observationsmap[i].y-mu_y,2))/(2*std::pow(std_landmark[0],2));
                currweight *= gauss_norm*std::exp(-exponent);
            }
            particles[p].weight = currweight;
            weights[p] = currweight;
        }
    }
}

void ParticleFilter::resample() {
	// Resample particles with replacement with probability proportional to their weight.
	// NOTE: You may find std::discrete_distribution helpful here.
	//   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
    // Random engine
    std::default_random_engine gen;
    std::discrete_distribution<double> distribution(weights.begin(), weights.end());
    // resample with replacement
    std::vector<Particle> newparticles;
    for (int i=0;i<num_particles;++i){
        newparticles.push_back(particles[distribution(gen)]);
    }
    particles = newparticles;
}

Particle ParticleFilter::SetAssociations(Particle& particle, const std::vector<int>& associations, 
                                     const std::vector<double>& sense_x, const std::vector<double>& sense_y)
{
    //particle: the particle to assign each listed association, and association's (x,y) world coordinates mapping to
    // associations: The landmark id that goes along with each listed association
    // sense_x: the associations x mapping already converted to world coordinates
    // sense_y: the associations y mapping already converted to world coordinates

    particle.associations= associations;
    particle.sense_x = sense_x;
    particle.sense_y = sense_y;
    return particle;
}

string ParticleFilter::getAssociations(Particle best)
{
	vector<int> v = best.associations;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<int>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseX(Particle best)
{
	vector<double> v = best.sense_x;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseY(Particle best)
{
	vector<double> v = best.sense_y;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
