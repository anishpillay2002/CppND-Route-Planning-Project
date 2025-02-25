#include "route_planner.h"
#include <algorithm>
using std::sort;
using std::vector;
using std::reverse;

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    // Find closest node to start_x, start_y
    RoutePlanner::start_node = &m_Model.FindClosestNode(start_x, start_y);
    // Find closest node to end_x, end_y
    RoutePlanner::end_node = &m_Model.FindClosestNode(end_x, end_y);
}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    return node->distance(*this->end_node);
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    // Find neighbors for current node
    current_node->FindNeighbors();

    // Loop over all its neighbors
    for (RouteModel::Node* n:current_node->neighbors){
        // If its not visited, set its parent
        n->parent = current_node;
        // Calculate its g value
        n->g_value = current_node->g_value + current_node->distance(*n);
        // Calculate its h value
        n->h_value = this->CalculateHValue(n);
        // Set it as visited
        n->visited = true;
        // Add to open_list
        this->open_list.emplace_back(n);
    }
}


// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.
bool Compare(const RouteModel::Node* a, const RouteModel::Node* b)
{   
    // Calculating the f score = g+h
    float f1 = a->g_value + a->h_value;
    float f2 = b->g_value + b->h_value;
    // Returing the highest value
    return (f1 > f2);
}

RouteModel::Node *RoutePlanner::NextNode() {
    // Sorting based on custom Compare function
    sort(this->open_list.begin(), this->open_list.end(), Compare);
    // Getting the last element
    RouteModel::Node *current = this->open_list.back();
    // Removing the last element from list as it is the one getting returned
    this->open_list.pop_back();
    return current;
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
    std::vector<RouteModel::Node> path_found ={};
    // TODO: Implement your solution here.
    while(current_node != this->start_node){
        // Adding current node to list
        path_found.emplace_back(*current_node);
        // Adding the distance to its parent
        
        distance += current_node->distance(*current_node->parent);
        // Assigning the parent as the current node and tracing back
        current_node = current_node->parent;
    }

    // Finally, appending the start node
    path_found.emplace_back(*current_node);
    // Reversing the order so we get start to finish
    std::reverse(path_found.begin(), path_found.end());

    // Scaling the distance by a defined value
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = this->start_node;
    current_node->visited = true;
    this->open_list.emplace_back(current_node);
    // this->AddNeighbors(current_node);
    // current_node = this->NextNode();
    // TODO: Implement your solution here.

    while (this->open_list.size() > 0)
    {
        current_node = this->NextNode();
        if (current_node == this->end_node)
        {
            m_Model.path = this->ConstructFinalPath(current_node);
            return;
        }
        else{
            
            this->AddNeighbors(current_node);
        }
    }
    std::cout<<"\nCan't find a path!"<<std::endl;
    m_Model.path = std::vector<RouteModel::Node>{};
}