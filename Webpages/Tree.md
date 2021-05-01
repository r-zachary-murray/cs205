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

Our implementation begins by assigning each point coordinates and an index, these coordinates are recursively split by octree if there are more than a set number of points contained in the cell. In general this stopping condition is a free parameter and we examine several different values of it in our analysis.

Next we need to check each subdomain and its neighbors with brute force. We recognized neighbors by testing if they share a vertex with the cell in question, we do so by a brute force search over all the vertices for every cell until one is found.  *talk about alternative algorithms*

### Serial Benchmarking




## Parallelization Strategy:

The recursive nature of the tree based algorithm and the splits that'll be necessary to sort the data into bins bodes poorly for GPU parallelization.  While the additional brute force checks of the subcells can be parallelized by GPU, the tree based method implies a large number of small brute force problems. In practice the data from each one of these subproblems will have to be to be transferred to the GPU from memory, incurring overheads each time.

*investigate potential for gpu parallelization of vertex search*

Here we focus on OpenMP parallelization of our serial code and create an MPI OpenMP hybrid parallel version that can be run on distributed memory systems.


### Parallel Benchmarking

