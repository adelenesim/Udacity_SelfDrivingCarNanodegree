# **Traffic Sign Recognition**

The goal of this project is to build a convolutional neural network to classify traffic signs.

[//]: # (Image References)

[image1]: ./images/image1.png "Visualization"
[image2]: ./images/image2.png "Barplot"
[image3]: ./images/image3.png "Streetsigns"
[image4]: ./images/image4.png "Conv1"
[image5]: ./images/image5.png "Conv2"

## Dataset exploration
### Statistics
Basic statistics of the dataset are shown below. (Evaluated with numpy; see accompanying jupyter notebook for details.)  

* The size of training set is 34799
* The size of the validation set is 4410
* The size of test set is 12630
* The shape of a traffic sign image is 32x32x3
* The number of unique classes/labels in the data set is 43 (from 0 to 42, inclusive)

### Visualization
#### Sample images
Sample images of the different classes in the training set are shown below. From visual inspection, it is clear that the images vary in brightness and contrast quite significantly.  

![alt text][image1]

#### Data distribution
Next, I also visually inspected that the training, validation and test set have similar distributions across the different classes. Because all three sets have similar distributions, a classifier trained on the training set should perform well (provided it is not overfitted) to the validation and test sets. However, it is also clear that some class labels have far fewer training images, making it difficult to train the classifier to recognize images with these labels. The contribution to the loss function for our classifier scales with the fraction of images for each class: if there are fewer images for a given class, the classifier is less able to train to discriminate it, compared to more dominant classes.

![alt text][image2]

## Design and Test a Model Architecture
### Preprocessing
#### 1. Conversion to grayscale  
The images were first converted to grayscale using the Luma formula (see [wikipedia](https://en.wikipedia.org/wiki/Luma_(video)). I also attempted to use the full RGB image for classification, but this turned out to work worse than the grayscale images. Using RGB increases the number of parameters by about 3x, which means training is slower, and requires more data to avoid overfitting. Reducing the complexity of the problem by switching to grayscale did not seem to affect the ability to train a good classifier in practice.

#### 2. Normalization  
Each channel (or just a single channel if a grayscale image was used) was then normalized by subtracting 128, and dividing by 128 to obtain zero mean and variance.

#### 3. Data augmentation (implemented, but not used in final version)  
I implemented a data augmentation pipeline, in part to generate more training data to meet the requirements of fitting a classifier that can train to use all three RGB channels. I did this by following the description for image modification (varying contrast and brightness) as implemented in TensorFlow's [image adjustment API](https://www.tensorflow.org/versions/r0.12/api_docs/python/image/image_adjustments). Unfortunately multiple experiments suggest that this data augmentation pipeline did not help with improving the classifier. The level of image modification was perhaps too trivial, resulting in overfitting to the original training data. More sophisticated modifications like noise introduction, zooming, rotating or cropping should lead to improved data augmentation. Together with using all three RGB channels, robust data augmentation could improve the current model.  

The final preprocessed data therefore only involved conversion to grayscale, followed by normalization.

### Model architecture
My final model consisted of the following layers:

| Layer         		| Description                                 |
|:------------------|:--------------------------------------------|
| Input         		| 32x32x3 RGB image                           |
| Convolution 5x5   | 1x1 stride, valid padding, outputs 28x28x6 	|
| Activation	    	|	ReLU                            						|
| Max pooling	      | 2x2 stride, 2x2 window, outputs 14x14x6			|
| Convolution 5x5   | 1x1 stride, valid padding, outputs 10x10x16	|
| Activation	    	|	ReLU                            						|
| Max pooling	      | 2x2 stride, 2x2 window, outputs 5x5x16	  	|
| Flatten       		| 5x5x6 to 400x1     						        			|
| Fully connected		| 400x1 to 120x1      				        				|
| Activation	    	|	ReLU                            						|
| Dropout       		| Keep probability of 0.9		        					|
| Fully connected		| 120x1 to 84x1      				        					|
| Activation	    	|	ReLU                            						|
| Dropout       		| Keep probability of 0.9		        					|
| Fully connected		| 84x1 to n_classes 				        					|
| Activation 				| Softmax                   									|

### Model training

I used the Adam optimizer (learning rate at 0.001) to train the model, with softmax cross entropy loss. A total of 30 epochs were used, with batch size set at 128. Additionally, I founded that initiating the weights with Xavier initialization led to improved accuracy than initialization with the truncated normal.

My final model results were:
* training set accuracy of 0.998
* validation set accuracy of 0.950
* test set accuracy of 0.930

### Solution approach
While the LeNet-5 architecture was designed for character recognition, its underlying framework was to identify patterns in images to help with classification. This means that the architecture should be robust to be transferred for other image classification tasks. More sophisticated and deeper frameworks have more recently been designed, but these require more data (and computational power) for training, because of the number of parameters in the model. Additionally, a similar (but not identical) architecture was developed by [Pierre Sermanet and Yann LeCun](http://yann.lecun.com/exdb/publis/pdf/sermanet-ijcnn-11.pdf) which showed up to 99.17% accuracy for this same traffic sign classification task. These all suggest that the LeNet-5 architecture is a useful place to start.  

I modified the original LeNet-5 architecture by including [dropout layers](https://www.cs.toronto.edu/~hinton/absps/JMLRdropout.pdf) that randomly set weights to zero. This layer acts as a regularizer, and prevents overfitting to the training set. Dropouts have been so successful since it was introduced in 2014 that most modern architectures make use of them. The accompanying hyperparameter is the keep probability (keep_prob), or the probability that a parameter's value is kept. I experimented with a few different probabilities and found that 0.9 worked best. (It is critical to set keep_prob to 1 when evaluating validation and test set accuracy.)  

I also experimented adding batch normalization before the activation layers, but surprisingly this did not improve performance (as evaluated on validation set), and therefore I removed batch normalization layers from the final model. Adding more convolution filters did not improve the model significantly, but resulted in increased model complexity and training time, and hence was omitted.  

The significantly higher training set accuracy over validation and test set accuracies suggests that despite dropout, there is still some overfitting to the training set. Nonetheless, the performance on the validation and test set (as well as on new images, as discussed below) is still very good.

## Test a Model on New Images
### Acquiring new images
I selected six images from the web for this test. For simplicity, images were cropped to 32x32 using Preview on Mac. The traffic signs with their expected class labels are shown below.

![alt text][image3]

Each image came with different classification challenges.  

| Image			        |     Challenge	        					|
|:------------------|:--------------------------------|
| Label 3 (Speed limit, 60km/h)     		| There are several images classes with speed limits on them. The classifier may mix them up |
| Label 12 (Priority road)     		    	| The all-white background may obfuscate the white edges of the sign |
| Label 14 (Stop sign)				        	| The sign has some skew |
| Label 23 (Slippery road)	      	  	| There are several class labels with triangle signs |
| Label 29 (Bicycles crossing)      		| Few instances of this label in the training set |
| Label 40 (Roundabout mandatory)		    | Noisy background |

The RGB colored images were preprocessed as before (converted to grayscale, then normalized).  

### Performance on new images

Here are the results of the prediction using my trained model:

| Image			                | Prediction                             |
|:--------------------------|:---------------------------------------|
| Speed limit, 60km/h    		| Speed limit, 60km/h                    |
| Priority road     	    	| Priority road                          |
| Stop sign				        	| Stop sign                              |
| Slippery road	      	  	| **Traffic signals (also a triangle sign)** |
| Bicycles crossing      		| Bicycles crossing                      |
| Roundabout mandatory	    | Roundabout mandatory                   |


The model was able to correctly guess 5 of the 6 traffic signs, which gives an accuracy of 83.3%. This is lower than the test set values of 93.0%, but somewhat expected due to the small number (only six) new images tested here. (That is, a 93% accuracy for six images requires 5.58 accurate predictions, which is not possible. Other than 100%, the next closest accuracy is 83.3%, corresponding to only one error, which was what I observed with my model.)

### Model certainty - softmax probabilities
In all cases, my model was very certain of its prediction, though interestingly, for the case with the wrong prediction, the probability of the most likely label was lowest (but still a very high of 0.9011; the next closest prediction was only 0.069). In the example with the wrong prediction, none of the top five highest softmax probabilities corresponded to the right label.

| Image                     | Prediction                             | Probability |     
|:--------------------------|:---------------------------------------|:------------|
| Speed limit, 60km/h    		| Speed limit, 60km/h                    | 1.0         |
| Priority road     	    	| Priority road                          | 0.9997      |
| Stop sign				        	| Stop sign                              | 0.9999      |
| Slippery road	      	  	| **Traffic signals (also a triangle sign)** | **0.9011**  |
| Bicycles crossing      		| Bicycles crossing                      | 1.0         |
| Roundabout mandatory	    | Roundabout mandatory                   | 1.0         |

### Visualizing the neural network  
Using the sixth image of the new images set (i.e. the Roundabout mandatory sign), I visualized the output of applying the different convolution filters.

#### Convolutional layer 1
![alt text][image4]  
This series of filters seem to be picking up different types of edges of the sign, some more focused on the outer sign boundary (e.g. feature map 0), while others seemed to be emphasizing on edges within the sign (e.g. feature map 2), or global sign shape (e.g. feature map 5).

#### Convolutional layer 2
![alt text][image5]
It is much harder to interpret the results from the second convolution due to the hierarchical nature of the architecture. Visually it appears that the different filters are highlighting different types of lines, such as horizontal ones (e.g. feature map 4), or diagonal ones (e.g. feature map 10). As we progress deeper into the network, the filters should be identifying higher order, and more complex features. Unfortunately it is also more challenging to see what these complex features the filters have identified are.
