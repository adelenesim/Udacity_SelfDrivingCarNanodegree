# **Finding Lane Lines on the Road**
Adelene Sim  
11th September 2017


## Objective:

The goals of this project are the following:
* Make a pipeline that finds lane lines on the road
* Reflect on your work in a written report
---
## Reflection

### Line searching pipeline
My pipeline consists of six steps, using the given helper functions:  
1. Convert the RGB color image to grayscale  
I found from various experiments that using the blue color channel helped with the yellow line identification, and hence the grayscale image was taken directly from the blue channel signal.

2. Smooth the image with a Gaussian kernel  
A kernel size of 5 was chosen for Gaussian blurring.

3. Perform Canny edge detection  
This was performed on the Gaussian blurred image, with a low and high threshold of 50 and 150, respectively.

4. Select mask to filter region of interest  
The masked region was selected based on the size of the image, with manual curation: a lower triangle was chosen so that the road of interest is fully captured.

5. Hough transformation to identify lines in the edge image  
Lines were identified from the masked Canny edge image, with tunable parameters. (Chosen parameters were rho=1, theta=np.pi/180, threshold=20, min_line_len=20, max_line_gap=10. Various parameters were tested, and these yielded generally reasonable results.)

6. Map the line image onto the original, for illustration


### Modifications to the draw_lines() function

The draw_lines() function was modified via a voting process to identify the main lines for the left and right lane markings. The general procedure was:

1. For each line identified, determine the gradient (m) and intercept (b). Discard cases with unreasonable m and b. (We expect m and b to be within certain ranges, given the position of the camera on the car.) Discretize the m and b into bins for voting.
2. For the corresponding (m,b) tuple, evaluate and add the weighted (by line length) vote.
3. For each positive and negative m, select the (at most) top two lines with the highest weighted votes. The left and right lane lines have opposite gradients, and hence the final two lines drawn should therefore have opposite gradients. For each sign of m, at most two lines were chosen because each lane marking has two line edges.
4. Take the weighted average of the m and b for all lines that fall within the same bin.
5. From the equation of the lines identified, plot the lines within the masked region.

### Potential shortcomings with your current pipeline
The current implementation has lines drawn independently per frame, leading to visible jitter in the videos.  
Additionally, despite the careful filtering, weighted voting and averaging, the challenge video still shows that the current implementation is susceptible to noise in the frames.

### Possible improvements to your pipeline
To reduce jitter, the m and b identified in each frame could be averaged with the m and b in of previous frames (use a small rolling window average), so that the effect of any spurious and erroneous line detection is minimized. This should make the line movement smoother.  
Additional filters could be introduced to reduce the effects of noise in the videos.  
