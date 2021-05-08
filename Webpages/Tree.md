# Tree based methods

All tree based collision detection algorithms fundamentally work by dividing up the domain recursively into smaller subdomains then checking collections of these subdomains for collisions using brute force. In the case of tree-based algorithms these sections can adaptively adjust to the particles in the simulation domain and yield improvements in performance over what would be expected from a static grid.



There are several different ways of implementing a tree-based collision detection algorithm.  There are two main classes of tree based methods for 3 dimensional collision detection. Octrees and KDtrees.

## KDtrees

KDtrees split the domain into two subdomains at a time (always a binary split) based on some analysis of that data in that domain, typically a mean or a median of the data contained therein, and thus have the advantage of being relatively adaptive depending on the method chosen.  Since KDtrees are binary trees are often deeper than octrees, but have fewer nodes at each level. The cells of a KDtree frequently become long and thin (high aspect ratio) for certain datasets.


## Octrees

Octrees split the domain in half at every step.  An octree implementation will begin with a cubic domain and split it into 8 smaller, equally sized cubes.  Traditional octree implementations are not adaptive, but the algorithm guarantees that every subdomain is always a cube of aspect ratio 1.

We elected to use an octree approach for our tree based collision detection algorithm.  We do so for a number of reasons.

1. Simplicity:
  The analysis that the KDtree conducts to determine where to split will have to be carried out many times on the dataset over the construction of the final tree.  Sophisticated analysis can also be expensive and slow down the tree creation process. The octree implementation contains no such step and as such can be generated faster.

2. Neighbour Efficiency:
  Preliminary results suggest tree based methods are most efficient when the tree is highly subdivided, the long, thin geometry of KDtrees may result in larger numbers of neighboring cells that need to be checked for colliding particles.

## Our Implementation

Our implementation begins by assigning each point coordinates and an index, these coordinates are recursively split by octree if there are more than a set number of points contained in the cell. In general this stopping condition is a free parameter and we examine several different values of it in our analysis. This fact that the splitting of the points only depends on the nummber of such points within a cell also allows us to only have to store the 'eaves' of the tree without any need for mote sophistocated structures!

  After subdiving the domain there are several different paths one can take witha tree-based collision detection algorithm. Since we;re using octrees our subdivision is independnent of anything except the numbe of points in a given cell. Thus we will need to have a strategy for dealing with cases where finite-sized elements might cross these cells. The traditional approach is to climb through the tree and locate the direct neighbours of each cell, then conduct collision detection within those subsets with a traditional method.   An example of this with quadtrees is shown below
  
  
 We attempted this approach but abandoned it for two reaons:
 
 1) While algorithms exist for this type of neighbor finding, see the tutorial here:
 https://geidav.wordpress.com/2017/12/02/advanced-octrees-4-finding-neighbor-nodes/
 as well as the paper here:
 http://jbehley.github.io/papers/behley2015icra.pdf
 these algorithms are extremely sophistocated and would be inappropriate for a project of this length. (Especially while trying to implment two other methods!). We experimented with using a brute force search over vertices for this detection, but the overhead from this process is more than what is saved from subdiving the domain.

Put v1 benchmark here. 

 2) In addition to being complicated the above algorithms require advanced data structures, which in addition to being complicated, increase the memory overhead of the programs. 

Instead of these approaches after splitting up the domain, we simply check each cell to assess if any points overlap the cell boundaries, and those to a seperate list and preform a brute force check on them. In general this is a small number of points, since for any given cube, the total volume it defines will be much greater than the volume near it's surface and thus most of the points will be contained in a single cell and won't contribute too much to a brute force search. 


### Serial Benchmarking

Recalling Prof.Sondak's advice on the Pareto principle, we assume 80% of the effect of our parallelization will come from about 20% of our code. As such we do rigours benchmarking of the serial code to inform our parallelization efforts. Below we plot the execution time of our code as a function of parameter "K". K controls how many times the code attempts to subdivide the domain (it's still the case that subdivision will fail if a subdomain contains <3 points).  The three different colors corresspond to our different datasets. The solid lines repersent the times spent 

checking all the subdomains with brute force. The dashed-dotted lines repersent the time spent on reading in the files. The dashed lines repersent the time brute force checking those points that lie near subdomain boundaries. The dotted lines repersent the time spent on tree construction and finding boundary points. 

We can see two broad trends, First the time spent reading in the files increases as the number of points increases. This makes sense since the files are bigger. The time to construct the tree and find the boundary points also increase with the number of points. Lastly we see that the time spent on brute force checking the boundary points is effectively negligible, (this also makes sense since I expect there to be few of them). 

Next we show a comparison with the brute force method. Here the solid lines are the gaussian initial conditions, the dahsed lines the uniform condition and the dotted lines the lognromal distribution. We can see here in the worst scenario for k=1, the overhead associated with the implmentation causes causes little speedup. (The black line shown here for comparison is the gaussian execution time described in the brute force section.)

We also show a plot of the resulting speedup as a function of k, calculated by divinding the total brute force execution time by  we can see that for certain datasets k=5 approaches a 500x speedup over brute force!




## Parallelization Strategy:

The recursive nature of the tree based algorithm and the splits that'll be necessary to sort the data into bins bodes poorly for GPU parallelization.  While the additional brute force checks of the subcells can be parallelized by GPU, the tree based method implies a large number of small brute force problems. In practice the data from each one of these subproblems will have to be to be transferred to the GPU from memory, incurring overheads each time.

*investigate potential for gpu parallelization of vertex search*

Here we focus on OpenMP parallelization of our serial code and test an OpenACC GPU acclerated version of the code.


### Parallel Benchmarking

