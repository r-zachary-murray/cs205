# Problem Statement. 


Collision detection is a common problem in computer science. Its encountered in Virtual Reality work  https://link.springer.com/chapter/10.1007/978-1-4020-8200-9_9, Nbody dynamics (most common in astronomy) https://www.aanda.org/articles/aa/full_html/2012/01/aa18085-11/aa18085-11.html#S13 and Molecular dynamics http://chemlabs.princeton.edu/torquato/wp-content/uploads/sites/12/files/publications/papers/paper-228.pdf.  

The simplest approach to Collision detection is the brute force search, where pairwise distances between all particles are computed and two particles are deemed colliding should their combined radii be larger than the distance. This algorithm is incredibly simple, but scales poorly with largen numbers of particles O(N^2). 

In addition to large numbers of particles causing a problem, the frequency at which collsion detection routines are called can also add to overhead.  Typically, when integraing systems collision detection routines are called at each timestep, and can be comparable to the time spent on integration in some systems.  Since this problem is so widespread and can induce a significant overhead to many types of calculations it is both a 'big-compute' problem and a problem that is well worth parallelizing.  

Here we take three different approaches to parallelizing collision detection algoithms and compare the results. 
