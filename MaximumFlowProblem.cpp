#include <iostream>
#include <string>
using namespace std;
#include "FHflowGraph.h"

int main()
{
   int final_flow;

   // build graph
   FHflowGraph<string, int> my_G;

   my_G.addEdge("s","a", 3);    my_G.addEdge("s","b", 2); 
   my_G.addEdge("a","b", 1);    my_G.addEdge("a","c", 3); my_G.addEdge("a","d", 4); 
   my_G.addEdge("b","d", 2);
   my_G.addEdge("c","t", 2); 
   my_G.addEdge("d","t", 3);  

   // show the original flow graph
   my_G.showResAdjTable();
   my_G.showFlowAdjTable();

   my_G.setStartVert("s");
   my_G.setEndVert("t");
   final_flow = my_G.findMaxflow();

   cout << "Final flow: " << final_flow << endl;

   my_G.showResAdjTable();
   my_G.showFlowAdjTable();

   cout << endl << endl << endl;

   FHflowGraph<string, int> my_graph_1;
   my_graph_1.addEdge("v1","v2", 2);    my_graph_1.addEdge("v1","v4", 1); 
   my_graph_1.addEdge("v2","v4", 3);    my_graph_1.addEdge("v2","v5", 10); 
   my_graph_1.addEdge("v3","v1", 4);    my_graph_1.addEdge("v3","v6", 5); 
   my_graph_1.addEdge("v4","v3", 2);    my_graph_1.addEdge("v4","v5", 2); 
   my_graph_1.addEdge("v4","v6", 8);    my_graph_1.addEdge("v4","v7", 4); 
   my_graph_1.addEdge("v5","v7", 6); 
   my_graph_1.addEdge("v7","v6", 1);

   my_graph_1.showResAdjTable();
   my_graph_1.showFlowAdjTable();

   my_graph_1.setStartVert("v4");
   my_graph_1.setEndVert("v7");
   final_flow = my_graph_1.findMaxflow();

   cout << "Final flow: " << final_flow << endl;

   my_graph_1.showResAdjTable();
   my_graph_1.showFlowAdjTable();

   return 0; 
}