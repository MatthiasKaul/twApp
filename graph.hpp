#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <set>
#include <tuple>


class Graph {
private:
  bool validCache = false;
  int cachedContractionCost;
  int cachedV, cachedW;

  int n = 0;
  int activeVertices = 0;
  std::vector<float> xCoor = {};
  std::vector<float> yCoor ={};

  std::vector<std::set<int>> adjacencies = {};
  std::vector<std::set<int>> redAdjacencies = {};

  std::vector<int> contractionRemap = {};
  std::vector<int> contractionSequence ={};
public:
  Graph () = default;
  virtual ~Graph () = default;
  int vCount(){return activeVertices;}
  bool isEdge(int v, int w){return adjacencies[v].count(w) > 0;}
  bool isRed(int v, int w){
    std::vector<int> tmp;
    while(contractionRemap[v] != v){
      tmp.push_back(v);
      v = contractionRemap[v];
    }
    for(auto i : tmp){contractionRemap[i] = v;}
    tmp = {};
    while(contractionRemap[w] != w){
      tmp.push_back(w);
      w = contractionRemap[w];
    }
    for(auto i : tmp){contractionRemap[i] = w;}
    return (redAdjacencies[v].count(w) > 0 || redAdjacencies[w].count(v) > 0);
  }
  void contract(int v, int w);
  void move(int v, double x, double y){xCoor[v] = x; yCoor[v] = y;}
  std::pair<double,double> getPos(int v){return {xCoor[v], yCoor[v]};}
  int maxRedDegree();
  void addVertex(double x, double y){
    validCache = false;
    xCoor.push_back(x); yCoor.push_back(y);
    adjacencies.push_back({});
    redAdjacencies.push_back({});
    contractionRemap.push_back(n);
    n++; activeVertices++;
  }
  std::vector<int> getVertices() const {
    std::vector<int> rval;
    for (size_t i = 0; i < n; i++) {
      if(contractionRemap[i] == i) rval.push_back(i);
    }
    return rval;
  }
  void addEdge(int v, int w){
    validCache = false;
    adjacencies[v].insert(w);
    adjacencies[w].insert(v);
  }
  int costOfContraction(int v, int w);
  std::tuple<int,int,int> cheapestContraction();
  int redDegree(int v);
};
#endif /* end of include guard: GRAPH_HPP */
