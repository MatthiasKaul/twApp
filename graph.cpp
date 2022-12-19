#include "graph.hpp"


void Graph::contract(int v, int w){
  if(v == w) return;
  if(v > w) {
    std::swap(v,w); // by default, contract into the smaller index
  }else{
    xCoor[v] = xCoor[w]; //Contract (location wise) into whatever was selected second
    yCoor[v] = yCoor[w];
  }
  for(auto i : adjacencies[w]){
    if(adjacencies[v].count(i) == 0){
      adjacencies[v].insert(i); adjacencies[i].insert(v);
      redAdjacencies[v].insert(i); redAdjacencies[i].insert(v);
    }
    if(redAdjacencies[w].count(i)) {redAdjacencies[v].insert(i); redAdjacencies[i].insert(v);}
  }
  for(auto i : adjacencies[v]){
    if(adjacencies[w].count(i) == 0) {redAdjacencies[v].insert(i); redAdjacencies[i].insert(v);}
  }
  activeVertices--;
  contractionRemap[w] = v;
  contractionSequence.push_back(v);
  contractionSequence.push_back(w);
}

int Graph::maxRedDegree(){
  auto vertices = getVertices();
  int mx = 0;
  for(auto i : vertices){
    int curDeg = 0;

    for(auto j : vertices){
      if( i == j) continue;
      if(isRed(i,j)) curDeg++;
    }
    mx = std::max(mx, curDeg);
  }
  return mx;
}
