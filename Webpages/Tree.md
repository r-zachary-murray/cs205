# Tree based methods

There are several different ways of implmenting a tree-based collision detection algorithm.  There are two main classes of tree based methods for 3 dimensional collision detection. Octrees and KDtrees. 

## KDtrees

KDtrees split the domain into two subdomains at a time (always a binary split) based of some analysis of that data in that domain, typically a mean or a median of the data contained therein, and thus have the advantage of being relatively adaptive depending on the method chosen.  Since KDtrees are binary trees are often deeper than octrees, but have fewer nodes at each level. The cells of a KDtree frequently become long an thin (high aspect ratio) for certain datasets. 


## Octrees

Octrees split the domain in half at every step.  An octree implmentation will begin with a cubic domain and split it into 8 smaller, equalliy sized cubes.  Traditional octree implementations are not adatpive, but the algorithym garuntees that every every subdomain is always a cube of aspect ratio 1. 

We elected to use an octree approach for our tree based collision detecton algorithm.  We do so for a number of reasons. 

1. Simplicity:
  The analysis that the KDtree conducts to determine where to split will have to be carried out many times on the dataset over the construction of the final tree.  Sophistocated analysis can also be expensive and slow down the tree creation process. The octree implemenation contains no such step and as such can generated faster. 

2. Neighbour Efficiency:
  Preliminary results suggest tree based methods are most efficient when the tree is highly subdivided, the long, thin geometry of KDtrees may result in larger numbers of neighboring cells that need to be checked for colliding particles. 


## Our Implementation

