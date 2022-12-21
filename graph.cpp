#include "graph.hpp"


void Graph::contract(int v, int w){
  if(v == w) return;
  validCache = false;
  
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

int Graph::redDegree(int v){
  std::vector<int> tmp;
  int rval = 0;
  for(auto x : redAdjacencies[v]){
    if(contractionRemap[x] == x){
      rval++;
    }else{
      tmp.push_back(x);
    }
  }
  for(auto x : tmp) {
    redAdjacencies[x].erase(v); adjacencies[x].erase(v);
    redAdjacencies[v].erase(x); adjacencies[v].erase(x);
  }
  return rval;
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

int Graph::costOfContraction(int v, int w){
  auto vertices = getVertices();
  int rval = 0;
  for(auto x : vertices){
    auto cost = redDegree(x);
    if(isRed(x,v) && isRed(x,w)) {cost--; goto ENDOFLOOP;}
    if(isRed(x,v) || isRed(x,w)) {goto ENDOFLOOP;}
    if(isEdge(x,v) != isEdge(x,w)) cost++;
    ENDOFLOOP:
      rval = std::max(rval, cost);
  }
  std::set<int> reds;
  for(auto x : redAdjacencies[v]) reds.insert(x);
  for(auto x : redAdjacencies[w]) reds.insert(x);
  for(auto x : adjacencies[v]) if(adjacencies[w].count(x) == 0) reds.insert(x);
  for(auto x : adjacencies[w]) if(adjacencies[v].count(x) == 0) reds.insert(x);
  int tmp = 0;
  for(auto x : reds) if(contractionRemap[x] == x) tmp++;

  return std::max(tmp,rval);
}

std::tuple<int,int,int> Graph::cheapestContraction(){
    if(validCache) return {cachedV, cachedW, cachedContractionCost};
    if(n < 2) return {0,0,0};
    auto vertices = getVertices();
    int cachedContractionCost = n+1;
    for (auto v : vertices) {
      for (auto w : vertices) {
        if(v >= w) continue;
        auto cost = costOfContraction(v,w);
        if(cost < cachedContractionCost){
          cachedContractionCost = cost;
          cachedV = v;
          cachedW = w;
        }
      }
    }
    validCache = true;
    return {cachedV, cachedW, cachedContractionCost};

}
