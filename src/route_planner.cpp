#include "route_planner.h"
#include <algorithm>

// using RouteModel::Node; Why does this return an error? ie " 'RouteModel' is not a namespace? "

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
 start_node = &m_Model.FindClosestNode(start_x, start_y); // This changes the address of the pointer pointing to start_node to the newly discovered node. 
 end_node = &m_Model.FindClosestNode(end_x, end_y); 
}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
  return node->distance(*end_node); 
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
  current_node->FindNeighbors(); // Will automatically populate current_node.neighbors 
  for (RouteModel::Node * node : current_node->neighbors)
  {
    node->parent = current_node; 
    node->h_value = CalculateHValue(node); // Note: CalculateHValue(RouteModel::Node const *node); takes in a pointer, so no need to deref here. 
    node->g_value = current_node->g_value + node->distance(*current_node); // Note here: the distance(Node other) method takes in a Node, *not* a pointer, so need to deref. 
    // Add to open_list and set visted to true. 
    node->visited = true;
    open_list.push_back(node); // open_list is private variable of the route_model class 
  }
}


// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.

RouteModel::Node *RoutePlanner::NextNode() {
  // Note that open_list is a vector of *nodes:  std::vector<RouteModel::Node*> open_list;
  // Is NextNode() being called?   
 std::sort(this->open_list.begin(), this->open_list.end(), [](const auto &a, const auto &b) {
   return (a->g_value + a->h_value) > (b->g_value + b->h_value); // will sort in descending order
 }); // Sorted in ascending order by g+h. 
  // Will point to the node with the smallest g+h value. 
  RouteModel::Node *p = this->open_list.back();
  // Remove that node. 
  this->open_list.pop_back();
  // Return the pointer.
  return p;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // TODO: Implement your solution here.
  // Start by adding current_node (=final_node):
  path_found.push_back(*current_node);
  // Now iterate backwards and add to path_found.
  while (current_node != start_node) { 
    RouteModel::Node *tmp = current_node;
    // Update the current_node to its parent node. 
    current_node = current_node->parent;
    // Add the distance.
    distance += current_node->distance(*tmp); 
    // Add the node to path_found.
    path_found.push_back(*current_node); 
  }
   // The order now needs to be reversed. 
    std::reverse(path_found.begin(), path_found.end());
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
   std::cout << "Size of the path: " << path_found.size() << std::endl;
    return path_found;
}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
  RouteModel::Node *current_node = nullptr;
  // TODO: Implement your solution here.
  start_node->visited = true;
  // Add start_node to the open_list:
  open_list.push_back(start_node);
  // Start the search. Initialize the current_node as start_node. 
  current_node = start_node;
  AddNeighbors(current_node); // Something is changing the value of start_node->parent from null. 
  while(current_node != end_node) {
    current_node = NextNode(); 
    AddNeighbors(current_node);
  }
  // current_node is now the end_node. Construct the final_path. The information about the path is stored by assigning parent nodes in AddNeighbors().  
  std::cout << "Reached the end_node, constructing final path..." << std::endl;
  m_Model.path = ConstructFinalPath(current_node);
  std::cout << "Path constructed." << std::endl;
  return;
}