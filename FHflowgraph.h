// File FHflowGraph.h EXPERIMENT
// Template definitions for FHflowGraph.  
#ifndef FHflowGraph_H
#define FHflowGraph_H
#include <limits.h>
#include <utility>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <stack>
#include <iostream>
#include <functional>
using namespace std;

template <class Object, typename CostType>
class FHflowVertex
{
   typedef FHflowVertex<Object, CostType>* VertPtr;
   typedef map<VertPtr, CostType> EdgePairList;

public:
   static int n_sort_key;
   static stack<int> key_stack;
   static enum { SORT_BY_DATA, SORT_BY_DIST } e_sort_type;
   static bool SetNSortType( int which_type );
   static void pushSortKey() { key_stack.push(n_sort_key); }
   static void popSortKey() { n_sort_key = key_stack.top();
   key_stack.pop(); };

   static int const INFINITY = INT_MAX;
   EdgePairList flow_adj_list, res_adj_list;
   Object data;
   CostType dist;
   VertPtr next_in_path;

   FHflowVertex( const Object & x = Object() );

   void addToFlowAdjList(VertPtr neighbor, CostType cost);
   void addToResAdjList(VertPtr neighbor, CostType cost);
   bool operator<( const FHflowVertex<Object, CostType> & rhs) const;
   const FHflowVertex<Object, CostType> & operator=
      ( const FHflowVertex<Object, CostType> & rhs);
   void showFlowAdjList();
   void showResAdjList();
};

template <class Object, typename CostType>
int FHflowVertex<Object, CostType>::n_sort_key 
   = FHflowVertex<Object, CostType>::SORT_BY_DATA;

template <class Object, typename CostType>
stack<int> FHflowVertex<Object, CostType>::key_stack;
// ------------------------------------------------------

template <class Object, typename CostType>
bool FHflowVertex<Object, CostType>::SetNSortType( int which_type )
{
   switch (which_type)
   {
   case SORT_BY_DATA:
   case SORT_BY_DIST:
      n_sort_key = which_type;
      return true;
   default:
      return false;
   }
}

template <class Object, typename CostType>
FHflowVertex<Object, CostType>::FHflowVertex(const Object & x) 
   : data(x), dist((CostType)INFINITY),
   next_in_path(NULL)
{
   // nothing to do
}

template <class Object, typename CostType>
void FHflowVertex<Object, CostType>::addToResAdjList
   (FHflowVertex<Object, CostType> *neighbor, CostType cost)
{ 
   res_adj_list[neighbor] = cost;
}

template <class Object, typename CostType>
void FHflowVertex<Object, CostType>::addToFlowAdjList
   (FHflowVertex<Object, CostType> *neighbor, CostType cost)
{ 
   flow_adj_list[neighbor] = cost;
}

template <class Object, typename CostType>
bool FHflowVertex<Object, CostType>::operator<(
   const FHflowVertex<Object, CostType> & rhs) const
{
   switch (n_sort_key)
   {
   case SORT_BY_DIST:
      return (dist < rhs.dist);
   case SORT_BY_DATA:
      return (data < rhs.data);
   default:
      return false;
   }
}

template <class Object, typename CostType>
const FHflowVertex<Object, CostType> & FHflowVertex<Object, CostType>
   ::operator=(const FHflowVertex<Object, CostType> & rhs)
{
   flow_adj_list = rhs.flow_adj_list;
   res_adj_list = rhs.res_adj_list;
   data = rhs.data;
   dist = rhs.dist;
   next_in_path = rhs.next_in_path;;
   return *this;
}

template <class Object, typename CostType>
void FHflowVertex<Object, CostType>::showResAdjList()
{
   EdgePairList::iterator iter;

   cout << "Residual Adj List for " << data << ": ";
   for (iter = res_adj_list.begin(); iter != res_adj_list.end(); ++iter)
      cout << iter->first->data << "(" << iter->second << ") ";
   cout << endl;
}

template <class Object, typename CostType>
void FHflowVertex<Object, CostType>::showFlowAdjList()
{
   EdgePairList::iterator iter;

   cout << "Flow Adj List for " << data << ": ";
   for (iter = flow_adj_list.begin(); iter != flow_adj_list.end(); ++iter)
      cout << iter->first->data << "(" << iter->second << ") ";
   cout << endl;
}

//============================================================================

template <class Object, typename CostType>
class FHflowGraph
{
   // internal typedefs to simplify syntax
   typedef FHflowVertex<Object, CostType> Vertex;
   typedef FHflowVertex<Object, CostType>* VertPtr;
   typedef map<VertPtr, CostType> EdgePairList;
   typedef set<VertPtr> VertPtrSet;
   typedef set<Vertex> VertexSet;

private:
   VertPtrSet vert_ptr_set;
   VertPtr start_vert_ptr, end_vert_ptr;
   VertexSet vertex_set;

public:
   FHflowGraph () {}
   void addEdge(const Object &source, const Object &dest, CostType cost);
   VertPtr addToVertexSet(const Object & object);
   void showFlowAdjTable();
   void showResAdjTable();
   bool setStartVert(const Object &x);
   bool setEndVert(const Object &x);
   void clear();

   CostType findMaxflow();
  
private:
   VertPtr getVertexWithThisData(const Object & x);

   bool establishNextFlowPath();
   CostType getLimitingFlowOnResPath();
   bool adjustPathByCost(CostType cost);
   CostType getCostOfResEdge(VertPtr src, VertPtr dst);
   bool addCostToResEdge(VertPtr src, VertPtr dst, CostType cost);
   bool addCostToFlowEdge(VertPtr src, VertPtr dst, CostType cost);
};

template <class Object, typename CostType>
FHflowVertex<Object, CostType>* FHflowGraph<Object, CostType>::addToVertexSet(
   const Object & object)
{
   pair<typename VertexSet::iterator, bool> ret_val;
   VertPtr v_ptr;

   // save sort key for client
   Vertex::pushSortKey();
   Vertex::SetNSortType(Vertex::SORT_BY_DATA);

   // build and insert vertex into master list
   ret_val = vertex_set.insert( Vertex(object) );

   // get pointer to this vertex and put into vert pointer list
   v_ptr = (VertPtr)&*ret_val.first;
   vert_ptr_set.insert(v_ptr);

   Vertex::popSortKey();  // restore client sort key
   return v_ptr;
}

template <class Object, typename CostType>
void FHflowGraph<Object, CostType>::clear()
{
   vertex_set.clear();
   vert_ptr_set.clear();
}

template <class Object, typename CostType>
void FHflowGraph<Object, CostType>::addEdge(
   const Object &source, const Object &dest, CostType cost )
{
   VertPtr src, dst;

   src = addToVertexSet(source);
   dst = addToVertexSet(dest);

   src->addToResAdjList(dst, cost);
   dst->addToResAdjList(src, 0);
   src->addToFlowAdjList(dst, 0);
}

template <class Object, typename CostType>
bool FHflowGraph<Object, CostType>::setStartVert(const Object &x)
{
   if( x.length() == NULL)
      return false;

   start_vert_ptr = getVertexWithThisData(x);
   return true;
}

template <class Object, typename CostType>
bool FHflowGraph<Object, CostType>::setEndVert(const Object &x)
{
   if( x.length() == NULL)
      return false;

   end_vert_ptr = getVertexWithThisData(x);
   return true;
}

template <class Object, typename CostType>
void FHflowGraph<Object, CostType>::showResAdjTable()
{
   typename VertPtrSet::iterator iter;

   cout << "------------------------ \n";
   for (iter = vert_ptr_set.begin(); iter != vert_ptr_set.end(); ++iter)
      (*iter)->showResAdjList();
   cout << endl;
}

template <class Object, typename CostType>
void FHflowGraph<Object, CostType>::showFlowAdjTable()
{
   typename VertPtrSet::iterator iter;

   cout << "------------------------ \n";
   for (iter = vert_ptr_set.begin(); iter != vert_ptr_set.end(); ++iter)
      (*iter)->showFlowAdjList();
   cout << endl;
}

template <class Object, typename CostType>
CostType FHflowGraph<Object, CostType>::findMaxflow()
{
   typename VertPtrSet::iterator v_iter;
   typename EdgePairList::iterator edge_pr_iter;
   VertPtr vp;
   CostType min_cost, total_flow = 0;

   for(v_iter = vert_ptr_set.begin(); v_iter != vert_ptr_set.end(); ++v_iter)
   {
      establishNextFlowPath();
      min_cost = getLimitingFlowOnResPath();
      if(!adjustPathByCost(min_cost))
         break;
   }

   for (edge_pr_iter = start_vert_ptr->flow_adj_list.begin(); 
         edge_pr_iter != start_vert_ptr->flow_adj_list.end();
         edge_pr_iter++)
      total_flow += edge_pr_iter->second;

   return total_flow;
}

template <class Object, typename CostType>
bool FHflowGraph<Object, CostType>::establishNextFlowPath()
{
   typename VertPtrSet::iterator v_iter;
   typename EdgePairList::iterator edge_pr_iter;
   VertPtr w_ptr, v_ptr, s_ptr;
   CostType cost_vw;
   queue<VertPtr> partially_processed_verts;

   s_ptr = getVertexWithThisData(start_vert_ptr->data);
   if (s_ptr == NULL)
      return false;

   for (v_iter = vert_ptr_set.begin(); 
      v_iter != vert_ptr_set.end(); ++v_iter)
   {
      (*v_iter)->dist = Vertex::INFINITY;
      (*v_iter)->next_in_path = NULL;
   }
   
   s_ptr->dist = 0;
   partially_processed_verts.push( s_ptr );

   // outer loop
   while( !partially_processed_verts.empty() )
   {
      v_ptr = partially_processed_verts.front(); 
      partially_processed_verts.pop();
 
      for (edge_pr_iter = v_ptr->res_adj_list.begin(); 
         edge_pr_iter != v_ptr->res_adj_list.end();
         edge_pr_iter++)
      {
         w_ptr = edge_pr_iter->first;
         cost_vw = edge_pr_iter->second;

         if((v_ptr->dist + cost_vw < w_ptr->dist) && cost_vw != 0)
         {
            w_ptr->dist = v_ptr->dist + cost_vw;
            w_ptr->next_in_path = v_ptr; 
            partially_processed_verts.push(w_ptr);
            
            if(w_ptr == end_vert_ptr)
               return true;
         }
      }
   }
   return false;
}

template <class Object, typename CostType>
CostType FHflowGraph<Object, CostType>::getLimitingFlowOnResPath()
{
   VertPtr vp, np;
   CostType min = INT_MAX;
   stack<VertPtr> path_stack;

   if (start_vert_ptr == NULL || end_vert_ptr == NULL)
      return CostType();

   for (vp = end_vert_ptr; vp != start_vert_ptr; vp = vp->next_in_path)
   {
      if(vp->next_in_path == NULL)
         return false;
      path_stack.push(vp);
   }
   path_stack.push(vp);

   while (true)
   {
      vp = path_stack.top();
      path_stack.pop();

      if ( path_stack.empty() )
         break;

      np = path_stack.top();

      if(getCostOfResEdge(vp , np) < min)
         min = getCostOfResEdge(vp , np);
   }
   return min;
}

template <class Object, typename CostType>
bool FHflowGraph<Object, CostType>::adjustPathByCost(CostType cost)
{
   VertPtr vp, np;
   stack<VertPtr> path_stack;

   if (start_vert_ptr == NULL || end_vert_ptr == NULL)
      return CostType();

   for (vp = end_vert_ptr; vp != start_vert_ptr; vp = vp->next_in_path)
   {
      if(vp->next_in_path == NULL)
         return false;
      path_stack.push(vp);
   }
   path_stack.push(vp);

   while (true)
   {
      vp = path_stack.top();
      path_stack.pop();

      if ( path_stack.empty() )
         break;

      np = path_stack.top();

      if(!addCostToResEdge(vp, np, -cost))
         return false;

      if(!addCostToResEdge(np, vp, cost))
         return false;

      if(!addCostToFlowEdge(vp, np, cost))
         if(!addCostToFlowEdge(np, vp, -cost))
            return false;
   }
   return true;
}

template <class Object, typename CostType>
CostType FHflowGraph<Object, CostType>::getCostOfResEdge
   (VertPtr src, VertPtr dst)
{
   typename EdgePairList::iterator edge_pr_iter;

   for (edge_pr_iter = src->res_adj_list.begin(); 
         edge_pr_iter != src->res_adj_list.end();
         edge_pr_iter++)
   {
      if ( edge_pr_iter->first == dst )
         return edge_pr_iter->second;
   }
   return INT_MAX;
}

template <class Object, typename CostType>
bool FHflowGraph<Object, CostType>::addCostToResEdge(
   VertPtr src, VertPtr dst, CostType cost)
{
   typename EdgePairList::iterator edge_pr_iter;

   for (edge_pr_iter = src->res_adj_list.begin(); 
         edge_pr_iter != src->res_adj_list.end();
         edge_pr_iter++)
   {
      if ( edge_pr_iter->first == dst )
      {
         edge_pr_iter->second = edge_pr_iter->second + cost;
         return true;
      }
   }
   return false;
}

template <class Object, typename CostType>
bool FHflowGraph<Object, CostType>::addCostToFlowEdge
   (VertPtr src, VertPtr dst, CostType cost)
{
   typename EdgePairList::iterator edge_pr_iter;

   for (edge_pr_iter = src->flow_adj_list.begin(); 
         edge_pr_iter != src->flow_adj_list.end();
         edge_pr_iter++)
   {
      if ( edge_pr_iter->first == dst )
      {
         edge_pr_iter->second = edge_pr_iter->second + cost;
         return true;
      }
   }
   return false;
}

template <class Object, typename CostType>
FHflowVertex<Object, CostType>* FHflowGraph<Object, CostType>::
   getVertexWithThisData(const Object & x)
{
   typename VertexSet::iterator find_result;
   Vertex vert(x);

   Vertex::pushSortKey();  // save client sort key
   Vertex::SetNSortType(Vertex::SORT_BY_DATA);
   find_result = vertex_set.find(vert);
   Vertex::popSortKey();  // restore client value

   if ( find_result == vertex_set.end() )
      return NULL;
   return  (VertPtr)&*find_result;  
}

#endif