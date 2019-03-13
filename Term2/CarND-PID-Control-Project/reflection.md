# **PID project reflection**

The goal of this project is to implement a PID controller to move a vehicle around the lake race track. In this reflection I will discuss some strategies I took to optimize the PID controller hyperparameters.

[//]: # (Image References)

[image1]: ./images/Kp.png "Kp"
[image2]: ./images/Ki.png "Ki"
[image3]: ./images/Kd.png "Kd"

## Effect of each component
### Speed
The choice of PID controller parameters depends heavily on the speed at which the vehicle is traveling. I found that it was harder to find a good compromise between adequate steering and smooth turning as the vehicle sped up. To circumvent this, I used a separate PID to control the throttle so that vehicle speed was kept around 20 mph.

### Varying Kp
The proportional (P) term of the PID controller tries to adjust steering to reduce the vehicle position error (measured by cross track error, or CTE). As a result, varying the size of the correction with scaling factor Kp leads to significant changes in extent of steering correction. Intuitively, if Kp is too small, the steering correction is inadequate, and the vehicle is more likely to veer off the track. If Kp is too large, conversely, the vehicle's movement will be very jerky, as a large steering adjustment will be deemed necessary even with small changes to CTE.   

From Figure 1, with Ki and Kd fixed, we observed that with small Kp, we see relatively larger CTE as the vehicle travels across the track, because there is insufficient steering correction. While CTE will be smaller on average with a larger Kp, we are also more likely to observe sharp, and more immediate changes in steering angles.  

![alt text][image1]
Figure 1: Changes in CTE and steering angle as the vehicle travels through the track. Three different Kp values are shown, with Ki fixed at 0.005 and Kd at 40.

### Varying Ki
The integral (I) term controls correction to any systematic error in measurement, and this is aggregated across the trajectory. If Ki is too small, large systematic error would lead to higher overall CTE. As Ki increases, systematic error is more easily corrected, leading to smaller CTE. However, the integral term is unstable earlier in the trajectory, and a large Ki would usually lead to over-correction early on (Figure 2). As the vehicle travels more along the track, this over-correction problem is resolved. There was less substantial difference in steering angles with Ki changes.

![alt text][image2]
Figure 2: Changes in CTE and steering angle as the vehicle travels through the track. Three different Ki values are shown, with Kp fixed at 0.3 and Kd at 40.

### Varying Kd
The derivative (D) term is critical in ensuring smooth steering, and Kd tuning is highly coupled with Kp. There is a natural trade-off between both parameters. A high Kp would help with keeping the vehicle on track, but could be too reactive, and a high Kd could help to smooth out any sharp steering changes. However, if Kd is too high, it would smooth out any steering changes that are required, and hence need a higher Kp to compensate. Figure 3 shows that sharp steering changes are minimized when a higher Kd is used.   

![alt text][image3]
Figure 3: Changes in CTE and steering angle as the vehicle travels through the track. Three different Ki values are shown, with Kp fixed at 0.3 and Ki at 0.005.

## Choosing final hyperparameters
I used hand-tuning and trial and error to determine the final hyperparameters, based on the properties I discussed above. First, I chose a large enough Kp so that the vehicle would stay on track. Then, I adjusted Kd so that the vehicle movement was sufficiently smooth. I noticed that there was no significant systematic error, so that Ki could be kept relatively small.   

My final choice of parameters are: 0.30 for Kp, 0.005 for Ki and 40 for Kd. These parameters appeared to be a good compromise to keep the vehicle on track and close to the center. The larger Kd also helped smooth the vehicle movement.

## Directions for improvement
Hand-tuning is tedious and not exhaustive. A method like twiddle would be more appropriate for larger scale and finer tuning.
