#ifndef _WEIGHTED_GRAPH_H_
#define _WEIGHTED_GRAPH_H_

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <utility>
#include <queue>
#include <string.h>
#include <limits>
#include <cmath>

using namespace std; 

class WeightedGraph {
    public:
        static const int MAX_SIZE = 100;            
        static const int WHITE = 0;
        static const int GREY = 1;
        static const int BLACK = 2;
    private:
        int numNodes;
        vector<pair<int,float>> adj[MAX_SIZE];
        //priority queue element, store cost, the node, and the 
        //    parent for reconstructing path later
        struct QueueElem {
            float cost;
            int node, parent;
            QueueElem(float newCost, int newNode, int newParent):
                    cost(newCost), node(newNode), parent(newParent) { }
        };
        //compare operator of element for priority queue
        struct QueueElemCompare {
            bool operator ()(const QueueElem &q1, const QueueElem&q2) const {
                return q1.cost > q2.cost;
            }
        };
    public:
        WeightedGraph() {
            numNodes = 0;
        }
        int getNumNodes() {
            return numNodes;
        }
        
        //determine if directed graph has cycle, starting at s
        bool hasCycle(int s) {
            int colour[MAX_SIZE];
            memset(colour, WHITE, sizeof(colour));
            return cycleDfs(s, colour);
        }
        //recursive function for hasCycle
        bool cycleDfs(int s, int *colour) {
            colour[s] = GREY;
            for (pair<int, float> p : adj[s]) {
                //for each descendent
                if (colour[p.first] == WHITE) {
                    //not seen yet, recurse
                    return cycleDfs(p.first, colour);
                } else if (colour[p.first] == GREY) {
                    //seen already, has cycle
                    return true;;
                } 
            }
            //no descendents
            colour[s] = BLACK;
            return false;
        }
        
        /* shortest path between given nodes, wrapper for dijkstra */
        float shortestPath(int s, int t, int *path, int &pathLength) {
            
            //call dijkstra to get array of pairs of parent & cost for all nodes
            pair<int,float> dist[MAX_SIZE];
            dijkstra(s, dist);
            
            float cost = dist[t].second;
            if (isinf(cost)) {
                //if cost is inf, then was never reached so no path, just return
                return cost;
            }
            
            //get path length
            pathLength = 0;
            int n = t;
            while (n != s) {
                pathLength++;
                n = dist[n].first;  
            }
            //follow parents back to get sequence of nodes in shortest path
            path[pathLength] = t;
            n = t;
            int index = pathLength;
            while (n != s) {
                n = dist[n].first;
                index--;
                path[index] = n;
            }
            path[index-1] = n;
            
            return cost;  
        }
        
        /* return vector of pairs, for each node, that is parent and shortest cost  */
        void dijkstra(int s, pair<int,float> *dist) {
           
            //set up queue, visited array, and initialize distance to infinity
            priority_queue<QueueElem,vector<QueueElem>, QueueElemCompare> pq;
            bool visited[MAX_SIZE];
            memset(visited, 0, sizeof(visited)); 
            int i;
            for (i =0; i<MAX_SIZE; i++) {
                dist[i].second = numeric_limits<float>::infinity();; 
            }
            
            //dijjkstra algorithm, fills in distance array of pairs of 
            //    parent and cost per node visited
            pq.push(QueueElem(0.0,s,-1));
            while (!pq.empty()) {
                QueueElem curr = pq.top();
                pq.pop();

                if (visited[curr.node]) {  
                    continue;
                }
                visited[curr.node] = true;
                dist[curr.node] = make_pair(curr.parent,curr.cost);

                for (pair<int,float> p : adj[curr.node]) {
                    pq.push(QueueElem(curr.cost + p.second, p.first, curr.node));
                }                    
            }
        }
        
        /*return vector of pairs of nodes, giving edges of minimum spanning tree of graph
        from input node s*/
        vector<pair<int,int>> getMst(int s) {
            vector<pair<int,int>> mst;
            priority_queue<QueueElem,vector<QueueElem>, QueueElemCompare> pq;
            bool visited[MAX_SIZE];
            memset(visited, 0, sizeof(visited)); 
            
            //prim's algorithm, like dijkstra except edge weights instead of total
            //path cost are pushed to priority queue
            pq.push(QueueElem(0.0,s,-1));
            while (!pq.empty()) {
                QueueElem curr = pq.top();
                pq.pop();

                if (visited[curr.node]) {  
                    continue;
                }
                visited[curr.node] = true;
                if (curr.node != s) {
                    mst.push_back(make_pair(curr.parent,curr.node));
                }

                for (pair<int,float> p : adj[curr.node]) {
                    pq.push(QueueElem(p.second, p.first, curr.node));
                }  
            }
            return mst;
        }
        
        
        /*loading graph from file: nodes have to be listed sequentially in the format:
            node, neighbour1 weight1, neighbour2 weight2, ...neighbour_n weight_n;*/
        bool loadGraph(string fileName) {
            const char *cfilename = fileName.c_str();
	        ifstream ins(cfilename);
	        if (!ins) {
		        cerr << "failed to open " << cfilename << endl;
		        return false;
            }
            
            int node1, node2;
            float wgt;
            string delim;
            
            //find number of nodes from first pass through file
            while (ins >> node1 >> delim) {
                if (node1 != numNodes) {
                    cout << "invalid file format" << endl;
                    return false;
                }
                while ((delim.compare(",") == 0) && (ins >> node2 >> wgt >> delim)) {
                }
                numNodes++;
            }
            
            //reset to start of file
            ins.clear();
            ins.seekg(0, ios::beg);
            if (numNodes > MAX_SIZE) {
                cout << "too many nodes" << endl;
                return false;
            }
            
            //read info about list of neighbours and weights for each node
            while (ins >> node1 >> delim) {
                while ((delim.compare(",") == 0) && (ins >> node2 >> wgt >> delim)) {
                    if (node2 >= numNodes) {
                        continue;
                    }
                    adj[node1].push_back(make_pair(node2,wgt));                     
                }
            }
            ins.close();
            
            return true;
        }
        //make it undirected by enforcing symmetry - warn about inconsistent weights
        void makeUndirected() {
            int node1, node2;
            float wgt;
            bool found;
            
            for (node1=0; node1<numNodes; node1++) {
                for (pair<int,float> p : adj[node1]) {
                    node2 = p.first;
                    wgt = p.second;
                    found = false;
                    for (pair<int, float> p2 : adj[node2]) {
                        if (p2.first == node1) {
                            found = true;
                            if (p2.second != wgt) {
                                cout << "inconsistent weights, keeping first one found" << endl;
                            }
                        }
                    }
                    if (!found) {
                        adj[node2].push_back(make_pair(node1,wgt));
                    }
                }
            }
        }
        
        //print graph: nodes are 0 to numNodes-1, 
        //then list all the edges
        void printGraph() {
            int node;
            float wgt;        
            cout << "nodes 0 to " << numNodes-1 << endl;
            cout << "edges: " << endl;            
            for (node = 0; node < numNodes; node++) {
                for (pair<int,float> p : adj[node]) {
                    cout << "(" << node <<", " << p.first << ", " << p.second << ")" << endl;
                }
            }
            cout << endl;
        }
};

#endif