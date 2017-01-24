/******************************************************************************
 * Copyright (c) 2013, GraphSQL Inc.
 * All rights reserved.
 * Project: GraphSQL-GAP
 *
 *  Created on: November 24, 2013
 *      Author: Adam Anthony
 ******************************************************************************/

#ifndef GDBMSxc_global_map_hpp
#define GDBMSxc_global_map_hpp

#include <list>
#include <utility>
#include <iostream>

namespace gperun{
/**
   * @brief The UnionFind class implements a union-find data structure usable in the GSQL global variable interface.
   *
   * This is a centralized Union-find data structure.
   *
   *
   *
   */
  class UnionFind {
  private:
    int * groups;
    int N;
    int nGroups;

  public:
    // base union find operations.
    UnionFind(int n){
      N = n;
      //space for one extra, since there is no 0 vertex in the example
      groups = new int[N];
      //every node starts in its own group
      nGroups = N;
      //a root node will have a value of -GROUPSIZE
      //each node starts in its own group.
      for(int i = 0; i< N; i++) groups[i] = -1;
    }


    ~UnionFind(){
      delete [] groups;
    }





    void union_sets(int u, int v){

      //this will prevent buffer overflows...
      if(u > N || v > N || u <0 || v <0){

        std::cerr<<"ERROR in ADD: either u or v is not a valid vertex: ("<<u<<","<<v<<")"<<std::endl;
        return;
      }

      int gU = findParent(u);
      int gV = findParent(v);

      //case 1: they are in the same group, we do nothing.
      if(gU != gV){
        //case 2: they are in different groups, and we
        //will do union by rank, the smaller group merging
        //into the larger.
        if(groups[gU] > groups[gV]){
          //remember!  the root holds -size as its value,
          //so > is correct here.  This means that gU
          //is the smaller group.  So gV becomes the
          //new root of this group.
          //update the size of the group
          groups[gV] += groups[gU];
          //gU is no longer a root, but a direct child of gV
          groups[gU] = gV;

        }else{
          //gV is the smaller group
          groups[gU] += groups[gV];
          groups[gV] = gU;

        }
        //and the number of total sets decreases by 1
        nGroups--;

      }




    }


    int numGroups(){
      return nGroups;
    }


    //helper function for pretty much everything else.
    //this function will recursively find the root node of the
    //group, identified as the index of its position in the groups array
    int findParent(int u){

      //this will prevent buffer overflows...
      if(u > N  || u <0 ){

        std::cerr<<"ERROR in findParent: u is not a valid vertex: "<<u<<std::endl;

        return -1;
      }


      //if the parent has a negative value, this means that it is a root.
      if(groups[u] <0 ) return u;


      //if u is the node, then we store its parent in the groups array.
      int root = findParent(groups[u]);

      //path compression means that once we find the root,
      //we will update the link to point straight to the root
      //over time, this will shorten all the paths and make this
      //find operation very fast.
      groups[u] = root;

      return root;
    }

    //basic, find u's group, see if v is in there.
    //if u does not have a group, then the answer must be false.  but this seems
    //like a good place to throw an error message.
    bool isSameGroup(int u, int v){

      //this will prevent buffer overflows...
      if(u > N || v > N || u <0 || v <0){

        std::cerr<<"ERROR in isSameGroup: either u or v is not a valid vertex: ("<<u<<","<<v<<")"<<std::endl;
        return false;
      }
      return findParent(u) == findParent(v);
    }

    //find u's group, report its size.  special case is if u is not in the set.
    //seems logical to return 0, meaning its set is empty (since it is not actually in the data)
    int sizeGroup(int u){
      //this will prevent buffer overflows...
      if(u > N  || u <0 ){

        std::cerr<<"ERROR in sizeGroup: u is not a valid vertex: "<<u<<std::endl;
        //0 is at least a valid index that does not exist.
        return 0;
      }
      return -1*groups[findParent(u)];
    }
  };
} // namespace gperun


#endif
