#include <iostream>
#include "weighted_graph.h"

using namespace std;

/*
driver to run methods on a weighted graph, including loading the graph from a file, 
printing it, and finding lowest cost path
*/

int main(int argc, char **argv) 
{
    WeightedGraph wg;
    if (!wg.loadGraph("graphinfo.txt")) {
        return 1;
    }
    //wg.makeUndirected();
    wg.printGraph();
    
    //start and nodes for following:
    int s = 1;
    int t = 0;//wg.getNumNodes()-1;
    
    //report if there is a cycle
    if (wg.hasCycle(s)) {
        cout << "has cycle" << endl;
    } else {
        cout << "no cycle" << endl;
    }
    cout << endl;
    
    //find lowest cost path between two nodes and print it if it exists
 
    int pathLength;
    int path[WeightedGraph::MAX_SIZE];
    float cost = wg.shortestPath(s, t, path, pathLength);
    cout << "shortest path between " << s << " and " << t << ", cost " << cost << ":" << endl;
    if (isinf(cost)) {
        cout << "none" << endl;
    } else {
        for (int i=0; i<=pathLength; i++) {
            cout << path[i] << endl;
        }
        cout << endl;
    }
    
    //find mst from given node
    vector<pair<int,int>> mst = wg.getMst(s);
    cout << "edges in MST:" << endl;
    for (pair<int,int> p : mst) {
        cout << "(" << p.first << ", " << p.second << ")" << endl;
    }
    cout << endl;
       
    return 0;
}