# **Model Predictive Control (MPC) project reflection**

The goal of this project is to implement a PID controller to move a vehicle around the lake race track. In this reflection I will discuss some strategies I took to optimize the PID controller hyperparameters.

## Model description
The model is a straightforward implementation of what was discussed in class. I consisted of the following steps:
1. Transform the waypoints into vehicle coordinates, then fit the transformed waypoints to a polynomial. Here I used a 3rd order polynomial which gives sufficient flexibility for curve fitting while keeping the number of parameters small. This is our reference path.
2. Using points determined by the timestep length and duration, estimate the path of the vehicle based on current position and parameters.
3. With the optimal reference path from (1), optimize the parameters of the vehicle state so that (2) is as close to (1) as possible. We can track "closeness" by the cross track error (CTE) and orientation error.
4. (3) is done by defining a cost function that includes CTE, orientation error and deviation from reference speed. To encourage smooth steering, as discussed in class, I also included terms to minimize the use of actuators, and to discourage sharp changes in steering and acceleration.

### Cost function
I followed the cost function discussed in class, and manually tuned the weights on the different terms. The optimal weights depends also on the reference speed of the vehicle that I set to 35mph. For smooth steering, I found it was necessary to have a high relative weight on the term that minimizes steep changes in steering.

### Effect of timestep length (N) and elapsed duration (dt)
I chose `N=25` and `dt=0.05`. This gave a prediction horizon of 1.25 seconds, which yielded good performance. Higher `N` leads to more computational cost, and for the same `dt`, would give a higher prediction horizon. It is unnecessary, and unhelpful to have a large prediction horizon, since there could be substantial changes in the surroundings within a long prediction horizon. Using a small `dt` gives finer resolution information, but this also comes at a higher cost. Having a `dt` much lower than the latency of the car is also excessive. Through experimentation, the aforementioned `N=25` and `dt=0.05` worked well for a reference speed of 35mph. I also tried using smaller `N` (e.g. 10) and larger `dt` (e.g. 0.1), but found them to work less optimally. For different speeds, optimal `N` and `dt` will vary.

### Handling latency
I handled latency with the help of [discussions](https://discussions.udacity.com/t/p5-simulated-car-shakes-crazy/547199/5) in the Udacity forums. Concretely, we want to estimate where the vehicle would have traveled during the latency period (estimated to be 100ms). This is done by simple kinematic equations, with the current state's initial position, speed and acceleration. We then used this new state's parameters to estimate the vehicle's path during optimization.
