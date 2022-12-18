#include "graph.hpp"


void Graph::contract(int v, int w){
  if(v > w) std::swap(v,w); // by default, contract into the smaller index
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
  int mx = 0;
  for(int i = 0; i  < n; i++){
    if(contractionRemap[i] != i) continue;
    int curDeg = 0;
    for(auto j : redAdjacencies[i]){
      if(contractionRemap[j] != j){
        adjacencies[i].erase(j);
        redAdjacencies[i].erase(j);
        continue;
      }
      curDeg++;
    }
    mx = std::max(mx, curDeg);
  }
  return mx;
}
