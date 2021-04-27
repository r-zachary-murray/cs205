// Run and compile with
// g++ -o test2 quadtree.cpp ; ./test2

#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timing.h"
#include <string>
#include <algorithm>
#include <tuple>

/////////////////////////////////////////////
//    Helper functions to dislay arrays    //
/////////////////////////////////////////////

void DisplayVV(std::vector<std::vector<int>> &out)
{
    std::cout << '\n';
    for (int i = 0; i < out.size(); i++) {
        std::cout << '\n';
        for (int j = 0; j < out[i].size(); j++)
            std::cout << out[i][j] << " ";
    std::cout << '\n';
    }
}

void DisplayVVf(std::vector<std::vector<double>> &out)
{
    std::cout << '\n';
    for (int i = 0; i < out.size(); i++) {
        std::cout << '\n';
        for (int j = 0; j < out[i].size(); j++)
            std::cout << out[i][j] << " ";
    std::cout << '\n';
    }
}


void DisplayV(std::vector<int> &out)
{
    std::cout << '\n';
    for (int i = 0; i < out.size(); i++) {
            std::cout << out[i] << " ";
    }
    std::cout << '\n';
}

void DisplayVf(std::vector<double> &out)
{
    std::cout << '\n';
    for (int i = 0; i < out.size(); i++) {
            std::cout << out[i] << " ";
    }
    std::cout << '\n';
}

/////////////////////////////////////////////
//         Read data from a file           //
/////////////////////////////////////////////

std::vector<double> ReadFile(std::string fname){ //I have not clue why I don't need an amersand on the filename here...
    std::ifstream finx(fname); //set input file
    std::vector<double> xdata; //make vector of ints
    double element; //variable element is also an int
    while (finx >> element){
        xdata.push_back(element); //push elements to data
    }
    return xdata;
}

/////////////////////////////////////////////
//     Returns Vertices from bounds        //
/////////////////////////////////////////////

std::vector<std::vector<double>> GetVertices(std::vector<double> &bvector){

    double xmax = bvector[0];
    double xmin = bvector[1];
    double ymax = bvector[2];
    double ymin = bvector[3];
    double zmax = bvector[4];
    double zmin = bvector[5];
 
    std::vector<std::vector<double>> vertices;
    std::vector<double> entry;
    entry.push_back(xmin);
    entry.push_back(ymin);
    entry.push_back(zmin);
    vertices.push_back(entry);
    std::vector<double> entry1;
    entry1.push_back(xmin);
    entry1.push_back(ymax);
    entry1.push_back(zmin);
    vertices.push_back(entry1);
    std::vector<double> entry2;
    entry2.push_back(xmax);
    entry2.push_back(ymin);
    entry2.push_back(zmin);
    vertices.push_back(entry2);
    std::vector<double> entry3;
    entry3.push_back(xmax);
    entry3.push_back(ymax);
    entry3.push_back(zmin);
    vertices.push_back(entry3);

    std::vector<double> entry4;
    entry4.push_back(xmin);
    entry4.push_back(ymin);
    entry4.push_back(zmax);
    vertices.push_back(entry4);
    std::vector<double> entry5;
    entry5.push_back(xmin);
    entry5.push_back(ymax);
    entry5.push_back(zmax);
    vertices.push_back(entry5);
    std::vector<double> entry6;
    entry6.push_back(xmax);
    entry6.push_back(ymin);
    entry6.push_back(zmax);
    vertices.push_back(entry6);
    std::vector<double> entry7;
    entry7.push_back(xmax);
    entry7.push_back(ymax);
    entry7.push_back(zmax);
    vertices.push_back(entry7);

    return vertices;
}

/////////////////////////////////////////////
//  Takes a cell and splits bounds into 8  //
/////////////////////////////////////////////


// Genreally there will be 8 corners to each shape 
// I'll use a,b,c,d notation. A1 is >>, B1 is ><, C1 is <> D1 is <<. (for z > zavg) 
// I'll use a,b,c,d notation. A2 is >>, B2 is ><, C2 is <> D2 is <<. (for z < zavg) 

std::vector<std::vector<double>> SplitBounds(std::vector<double> &bvector ){

    double xmax = bvector[0];
    double xmin = bvector[1];
    double ymax = bvector[2];
    double ymin = bvector[3];
    double zmax = bvector[4];
    double zmin = bvector[5];

    double xavg = (xmax+xmin)/2;
    double yavg = (ymax+ymin)/2;
    double zavg = (zmax+zmin)/2;


    std::vector<double> aout1;
    std::vector<double> bout1;
    std::vector<double> cout1;
    std::vector<double> dout1;
    std::vector<double> aout2;
    std::vector<double> bout2;
    std::vector<double> cout2;
    std::vector<double> dout2;

    // These are all for z>zavg
    aout1.push_back(xmax);
    aout1.push_back(xavg);
    aout1.push_back(ymax);
    aout1.push_back(yavg);
    aout1.push_back(zmax);
    aout1.push_back(zavg);

    bout1.push_back(xmax);
    bout1.push_back(xavg);
    bout1.push_back(yavg);
    bout1.push_back(ymin);
    bout1.push_back(zmax);
    bout1.push_back(zavg);

    cout1.push_back(xavg);
    cout1.push_back(xmin);
    cout1.push_back(ymax);
    cout1.push_back(yavg);
    cout1.push_back(zmax);
    cout1.push_back(zavg);

    dout1.push_back(xavg);
    dout1.push_back(xmin);
    dout1.push_back(yavg);
    dout1.push_back(ymin);
    dout1.push_back(zmax);
    dout1.push_back(zavg);


    // These are all for z<zavg
    aout2.push_back(xmax);
    aout2.push_back(xavg);
    aout2.push_back(ymax);
    aout2.push_back(yavg);
    aout2.push_back(zavg);
    aout2.push_back(zmin);

    bout2.push_back(xmax);
    bout2.push_back(xavg);
    bout2.push_back(yavg);
    bout2.push_back(ymin);
    bout2.push_back(zavg);
    bout2.push_back(zmin);

    cout2.push_back(xavg);
    cout2.push_back(xmin);
    cout2.push_back(ymax);
    cout2.push_back(yavg);
    cout2.push_back(zavg);
    cout2.push_back(zmin);

    dout2.push_back(xavg);
    dout2.push_back(xmin);
    dout2.push_back(yavg);
    dout2.push_back(ymin);
    dout2.push_back(zavg);
    dout2.push_back(zmin);

    std::vector<std::vector<double>> res;
    res.push_back(aout1);
    res.push_back(bout1);
    res.push_back(cout1);
    res.push_back(dout1);
    res.push_back(aout2);
    res.push_back(bout2);
    res.push_back(cout2);
    res.push_back(dout2);
    return res;

}


/////////////////////////////////////////////
//  Takes a cell and splits points into 8  //
/////////////////////////////////////////////


std::vector<std::vector<int>> SplitCell(std::vector<double> &xdata,std::vector<double> &ydata,std::vector<double> &zdata,std::vector<int> &index ,std::vector<double> &bvector){

//std::tuple<std::vector<std::vector<int>>, std::vector<std::vector<double>>> SplitCell(std::vector<double> &xdata,std::vector<double> &ydata,std::vector<double> &zdata,std::vector<int> &index ,std::vector<double> &bvector){




    std::vector<int>::size_type szi = index.size();

    double xmax = bvector[0];
    double xmin = bvector[1];
    double ymax = bvector[2];
    double ymin = bvector[3];
    double zmax = bvector[4];
    double zmin = bvector[5];

    double xmean = (xmax+xmin)/2;
    double ymean = (ymax+ymin)/2;
    double zmean = (zmax+zmin)/2;

    std::vector<std::vector<int>> res;
// I'll use a,b,c,d notation. A is >>, B is ><, C is <> D is <<. 
    std::vector<int> avals1;
    std::vector<int> bvals1;
    std::vector<int> cvals1;
    std::vector<int> dvals1;
    std::vector<int> avals2;
    std::vector<int> bvals2;
    std::vector<int> cvals2;
    std::vector<int> dvals2;
    for (unsigned i=0; i<szi; i++){
        if (xdata[index[i]] > xmean && ydata[index[i]] > ymean && zdata[index[i]] > zmean){
            avals1.push_back(index[i]);
        }
        else if (xdata[index[i]] > xmean && ydata[index[i]] <= ymean && zdata[index[i]] > zmean){
            bvals1.push_back(index[i]);
        }
        else if (xdata[index[i]] <= xmean && ydata[index[i]] > ymean && zdata[index[i]] > zmean){
            cvals1.push_back(index[i]);
        }
        else if (xdata[index[i]] <= xmean && ydata[index[i]] <= ymean && zdata[index[i]] > zmean){
            dvals1.push_back(index[i]);
        }
        else if (xdata[index[i]] > xmean && ydata[index[i]] > ymean && zdata[index[i]] <= zmean){
            avals2.push_back(index[i]);
        }
        else if (xdata[index[i]] > xmean && ydata[index[i]] <= ymean && zdata[index[i]] <= zmean){
            bvals2.push_back(index[i]);
        }
        else if (xdata[index[i]] <= xmean && ydata[index[i]] > ymean && zdata[index[i]] <= zmean){
            cvals2.push_back(index[i]);
        }
        else if (xdata[index[i]] <= xmean && ydata[index[i]] <= ymean && zdata[index[i]] <= zmean){
            dvals2.push_back(index[i]);
        }
        else{
            std::cout << index[i] << "Theres an error in cell splitting!"<<'\n';
        }
    }

    res.push_back(avals1);
    res.push_back(bvals1);
    res.push_back(cvals1);
    res.push_back(dvals1);
    res.push_back(avals2);
    res.push_back(bvals2);
    res.push_back(cvals2);
    res.push_back(dvals2);

    return res;
}



/////////////////////////////////////////////
//  Updates leaves vector with new cells   //
/////////////////////////////////////////////


std::vector<std::vector<int>> UpdateLeafPoints(std::vector<std::vector<int>> &input, std::vector<double> &xdata,std::vector<double> &ydata,std::vector<double> &zdata,std::vector<std::vector<double>> &bvector){
   //Next I need to subdivide each cell once again and make a new vector to hold the 2nd order divisions

    std::vector<std::vector<int>> result;
    for (unsigned i=0; i<input.size(); i++){
        std::vector<std::vector<int>> tmp;
        if (input[i].size() > 3){
            tmp = SplitCell(xdata,ydata,zdata,input[i],bvector[i]);
            //tmp is a vector of vectors, to get in the same format as out2 i'll have to flatten it. 
            for(unsigned j=0; j<tmp.size(); j++){
                 result.push_back(tmp[j]);
            }
        }
        else {
            result.push_back(input[i]);
        }
     }

    return result;
}

/////////////////////////////////////////////
//  Updates leaves vector with new bounds  //
/////////////////////////////////////////////


std::vector<std::vector<double>> UpdateLeafBounds(std::vector<std::vector<int>> &input,std::vector<std::vector<double>> &bvector){
   //Next I need to subdivide each cell once again and make a new vector to hold the 2nd order divisions

    std::vector<std::vector<double>> result;
    for (unsigned i=0; i<input.size(); i++){
        std::vector<std::vector<double>> tmp;
        if (input[i].size() > 3){
            tmp = SplitBounds(bvector[i]);
            //tmp is a vector of vectors, to get in the same format as out2 i'll have to flatten it. 
            for(unsigned j=0; j<tmp.size(); j++){
                 result.push_back(tmp[j]);
            }
        }
        else {  // If unsplit return original bounds
            result.push_back(bvector[i]);
        }
     }

    return result;
}

/////////////////////////////////////////////
//      Brute force check on subsets       //
/////////////////////////////////////////////


void CollisionDetect(std::vector<double> &xdata,std::vector<double> &ydata,std::vector<double> &zdata,std::vector<int> &result){
   //Next I need to subdivide each cell once again and make a new vector to hold the 2nd order divisions

    std::vector<double> subx;
    std::vector<double> suby;
    std::vector<double> subz;
    for (unsigned i=0; i<result.size(); i++){
        subx.push_back(xdata[result[i]]);
        suby.push_back(ydata[result[i]]);
        subz.push_back(zdata[result[i]]);
    }

    double dx2;
    double dy2;
    double dz2;
    double r;
//    double dumvar;

    for (unsigned i=0; i<subx.size(); i++){
        for (unsigned j=0; j<subx.size(); j++){
            dx2 = (subx[i] -subx[j])*(subx[i] -subx[j]);
            dy2 = (suby[i] -suby[j])*(suby[i] -suby[j]);
            dz2 = (subz[i] -subz[j])*(subz[i] -subz[j]);
            r = pow(dx2 + dy2 + dz2,0.5);
            if (r < 0.1 && i != j){
                //dumvar=1;
                std::cout <<'\n'<<"Particle: "<< result[i]  << " Collided with Particle:" << result[j] << '\n';
            }

        }
    }
}


int CheckVertex(std::vector<double> &cell_bvec,std::vector<double> &target_bvec){
   //Next I need to subdivide each cell once again and make a new vector to hold the 2nd order divisions

    std::vector<std::vector<double>> cell_vertices;
    std::vector<std::vector<double>> test_vertices;
    cell_vertices = GetVertices(cell_bvec);
    test_vertices = GetVertices(target_bvec);

    int shared;
    shared = 0;
    for (unsigned i=0; i<cell_vertices.size(); i++){
        for (unsigned j=0; j<test_vertices.size(); j++){
            if (cell_vertices[i][0] == test_vertices[j][0] && cell_vertices[i][1] == test_vertices[j][1] && cell_vertices[i][2] == test_vertices[j][2]){
                // If x,y,z match the cell then 
                shared=1;
                //std::cout <<'\n' << '	'<< i  << '	' << j << '\n';
            }

        }
    }

    return shared;

}


int main()
{ 
    //Load Data
    std::vector<double> xdata = ReadFile("xtest3");
    std::vector<double> ydata = ReadFile("ytest3");
    std::vector<double> zdata = ReadFile("ztest3");
//    DisplayVf(xdata);

    //From here the stratagey is to take a list of indexes, and divide them up 
    //We'll also keep track of the vertices associated with each box so we can determine neighbors later. 

    //index all points in the domain
    std::vector<int> index(xdata.size());
    std::iota(index.begin(), index.end(), 0);
    //setup initial bounds vector (this is a vector of length 4 that defines the bounds of each cell)
    double maxX = *std::max_element(xdata.begin(), xdata.end());
    double minX = *std::min_element(xdata.begin(), xdata.end());
    double maxY = *std::max_element(ydata.begin(), ydata.end());
    double minY = *std::min_element(ydata.begin(), ydata.end());
    double maxZ = *std::max_element(zdata.begin(), zdata.end());
    double minZ = *std::min_element(zdata.begin(), zdata.end());

    std::vector<double> bvector;
    bvector.push_back(maxX);
    bvector.push_back(minX);
    bvector.push_back(maxY);
    bvector.push_back(minY);
    bvector.push_back(maxZ);
    bvector.push_back(minZ);


//    std::vector<std::vector<double>> vertices;
//    vertices = GetVertices(maxX,minX,maxY,minY);
//    DisplayVVf(vertices);

//    std::vector<std::vector<double>> outbounds;
//    std::vector<std::vector<int>> outpoints;
//    outbounds = SplitBounds(bvector);
//    DisplayVVf(outbounds);
//    outpoints = SplitCell(xdata,ydata,zdata,index,bvector);
//    DisplayVV(outpoints);

    std::vector<std::vector<int>> initial_splits;
    std::vector<std::vector<double>> initial_bvecs;
    initial_splits.push_back(index);
    initial_bvecs.push_back(bvector);

    std::vector<std::vector<int>> next_splits;
    std::vector<std::vector<double>> next_bvecs;
    for (int i=0; i < 2; i++){
        next_splits = UpdateLeafPoints(initial_splits,xdata,ydata,zdata,initial_bvecs);
        next_bvecs = UpdateLeafBounds(initial_splits,initial_bvecs);
        initial_splits = next_splits;
        initial_bvecs = next_bvecs;
        //std::cout << next_bvecs.size()<<'\n';
    }

//    DisplayVV(next_splits);
//    DisplayVVf(next_bvecs);



    for (int k = 0; k < next_bvecs.size(); k++){
        std::vector<int> check_inds;
        for (int i = 0; i < next_bvecs.size(); i++) {
            int res;
            res = CheckVertex(next_bvecs[k],next_bvecs[i]);
            if (res ==1){  //If they share a vertex, add indexes to whitelist
                for (int j = 0; j<next_splits[i].size(); j++){
                    check_inds.push_back(next_splits[i][j]);
                 }
            }
        }
    DisplayV(check_inds);
    CollisionDetect(xdata,ydata,zdata,check_inds);
    }


//CollisionDetect(xdata,ydata,zdata,index);



    std::cout << '\n';


}
