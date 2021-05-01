## We generate a few different mock datasets to test your algorithms. They were each generated with 10^k points where k $\in$ {3,4,5,6}. Below are the descirptions of each dataset as well as a graphic repersentation of them for 10^3 points.  

### Uniform Distribution

![Normal](/Tests/uniform/out.gif)

This dataset was made by choosing the x,y,z coordinates to be uniform random variables over [0,1]. We take it to be a control case for our algorithms, its is the most regular and symmetric of our datasets. 

### Gaussian Distribution

![Normal](/Tests/gaussian/out.gif)

This dataset was made by choosing the x,y,z coordinates to be normally distributed random variables with $\mu=0$, $\sigma=\frac{1}{3}$. Unlike the normal uniform distribution, the density of points in this dataset changes with location, but it still exhibits significant symmetry. 

### LogNormal Distribution

![Normal](/Tests/lognormal/out.gif)

This dataset was made by choosing the x,y,z coordinates to be lognormally distributed random variables with $\mu=0$, $\sigma=\frac{1}{3}$. This dataset incorporates both variations in the density of points and assymetry in overall points distribution and may serve as a challenge to our load-balancing procedures. 
