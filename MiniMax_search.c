/*
	CSC D84 - Unit 2 - MiniMax search and adversarial games

	This file contains stubs for implementing a MiniMax search
        procedure with alpha-beta pruning. Please read the assignment
	handout carefully - it describes the game, the data you will
	have to handle, and the search functions you must provide.

	Once you have read the handout carefully, implement your search
	code in the sections below marked with

	**************
	*** TO DO:
	**************

	Make sure to add it to your report.txt file - it will be marked!

	Have fun!

	DO NOT FORGET TO 'valgrind' YOUR CODE - We will check for pointer
	management being done properly, and for memory leaks.

	Starter code: F.J.E. Sep. 15
*/

#include "MiniMax_search.h"
//#include <limits.h>

typedef struct Node {
	/**
	 * a node struct for the linked list
	*/
    int x;
    int y;
    double cost;
	struct Node* next;
} Node;

Node* newNode(int x, int y, int cost){
	/**
	 * returns a new node given the input params
	 * input: 
	 * x: x coordinate
	 * y: y coordinate
	 * cost: cost of going to the x and y coordinate
	*/
	Node* n = (Node*) malloc(sizeof(Node)); 
    n->x = x;
	n->y = y;
    n->cost = cost;
    n->next = NULL;
    return n;
}

bool isEmpty(Node** head){
	/**
	 * returns if the linked list is empty
	*/
    return (*head) == NULL;
}
int location_to_int(int x, int y) {
	/**
	 * translate coordinates to location for graph
	 * input: 
	 * x: x coordinate
	 * y: y coordinate
	*/
	return x + (y * size_X);
}

bool in_end_indices(int *end_indices, int length, int curr_loca) {
	/**
	 * see if the current location is in the end indices
	 * input: 
	 * end_indices: a list of end indices
	 * length: the length of the former list
	 * curr_loca: the current location
	*/
	for (int i = 0; i < length; i++) {
		if(end_indices[i] == curr_loca) {
			return true;
		}
	}
	return false;
}
double MiniMax(double gr[graph_size][4], int path[1][2], double minmax_cost[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode, double (*utility)(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4]), int agentId, int depth, int maxDepth, double alpha, double beta)
{
 /*
   This function is the interface between your solution for the assignment and the driver code. The driver code
   in MiniMax_search_core_GL will call this function once per frame, and provide the following data
   
   Board and game layout:

	Exactly the same as for Assignment 1 - have a look at your code if you need a reminder of how the adjacency
	list and agent positions are stored.	

	Note that in this case, the path will contain a single move - at the top level, this function will provide
	an agent with the 'optimal' mini-max move given the game state.


   IMPORTANT NOTE: Mini-max is a recursive procedure. This function will need to fill-in the mini-max values for 
	 	   all game states down to the maximum search depth specified by the user. In order to do that,
		   the function needs to be called with the correct state at each specific node in the mini-max
		   search tree.

		   The game state is composed of:

			* Mouse, cat, and cheese positions (and number of cats and cheeses)
			
		   At the top level (when this function is called by the mini-max driver code), the game state
		   correspond to the current situation of the game. But once you start recursively calling
		   this function for lower levels of the search tree the positions of agents will have changed.
		   
		   Therefore, you will need to define local variables to keep the game state at each node of the
		   mini-max search tree, and you will need to update this state when calling recursively so that
		   the search does the right thing.

		   This function *must check* whether:
			* A candidate move results in a terminal configuration (cat eats mouse, mouse eats cheese)
			  at which point it calls the utility function to get a value
	 		* Maximum search depth has been reached (depth==maxDepth), at which point it will also call
			  the utility function to get a value
			* Otherwise, call recursively using the candidate configuration to find out what happens
			  deeper into the mini-max tree.

   Arguments:
		gr[graph_size][4]   		- This is an adjacency list for the maze
		path[1][2] 			- Your function will return the optimal mini-max move in this array.
		minmax_cost[size_X][size_Y]	- An array in which your code will store the
						  minimax value for maze locations expanded by
						  the search *when called for the mouse, not
						  for the cats!*

						  This array will be used to provide a visual 
						  display of minimax values during the game.

		cat_loc[10][2], cats   - Location of cats and number of cats (we can have at most 10,
					 but there can be fewer). Only valid cat locations are 0 to (cats-1)
		cheese_loc[10][2], cheeses - Location and number of cheese chunks (again at most 10,
					     but possibly fewer). Valid locations are 0 to (cheeses-1)
		mouse_loc[1][2] - Mouse location - there can be only one!
		mode - Search mode selection:
					mode = 0 	- No alpha-beta pruning
					mode = 1	- Alpha-beta pruning

		(*utility)(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4]);
				- This is a pointer to the utility function which returns a value for a specific game configuration

				   NOTE: Unlike the search assignment, this utility function also gets access to the graph so you can do any processing 					 that requires knowledge of the maze for computing the utility values.

				  * How to call the utility function from within this function : *
					- Like any other function:
						u = utility(cat_loc, cheese_loc, mouse_loc, cats, cheeses, depth, gr);
						
		agentId: Identifies which agent we are doing MiniMax for. agentId=0 for the mouse, agentId in [1, cats] for cats. Notice that recursive calls
                         to this function should increase the agentId to reflect the fact that the next level down corresponds to the next agent! For a game
                         with two cats and a mouse, the agentIds for the recursion should look like 0, 1, 2, 0, 1, 2, ...
	
		depth: Current search depth - whether this is a MIN or a MAX node depends both on depth and agentId.
		
		maxDepth: maximum desired search depth - once reached, your code should somehow return
			  a minimax utility value for this location.

		alpha. beta: alpha and beta values passed from the parent node to constrain search at this
			     level.

   Return values:
		Your search code will directly update data passed-in as arguments:
		
		- Mini-Max value	: Notice this function returns a double precision number. This is
					  the minimax value at this level of the tree. It will be used 
					  as the recursion backtracks filling-in the mini-max values back
					  from the leaves to the root of the search tree. 

		- path[1][2]		: Your MiniMax function will return the location for the agent's 
					  next location (i.e. the optimal move for the agent). 
		- minmax_cost[size_X][size_Y] 	:  Your search code will update this array to contain the
						   minimax value for locations that were expanded during
						   the search. This must be done *only* for the mouse.

						   Values in this array will be in the range returned by
						   your utility function.

		* Your code MUST NOT modify the locations or numbers of cats and/or cheeses, the graph,
	     	  or the location of the mouse - if you try, the driver code will know it *
			
		That's that, now, implement your solution!
 */

 /********************************************************************************************************
 * 
 * TO DO:	Implement code to perform a MiniMax search. This will involve a limited-depth BFS-like
 *              expansion. Once nodes below return values, your function will propagate minimax utilities
 *		as per the minimax algorithm.
 *	
 *		Note that if alpha-beta pruning is specified, you must keep track of alphas and betas
 *		along the path.
 *
 *		You can use helper functions if it seems reasonable. Add them to the MiniMax_search.h
 *		file and explain in your code why they are needed and how they are used.
 *
 *		Recursion should appear somewhere.
 *
 *		MiniMax cost: If the agentId=0 (Mouse), then once you have a MiniMax value for a location
 *		in the maze, you must update minmax_cost[][] for that location.
 *
 *		How you design your solution is up to you. But:
 *
 *		- Document your implementation by adding concise and clear comments in this file
 *		- Document your design (how you implemented the solution, and why) in the report
 *
 ********************************************************************************************************/

	int i, x, y;
	double value, utilityValue;
	int mouse_loc_copy[1][2], cat_loc_copy[10][2], cheese_loc_copy[10][2], path_copy[1][2];

	// make copies of the game state
	for(i=0; i<cats; i++) {
		cat_loc_copy[i][0] = cat_loc[i][0];
		cat_loc_copy[i][1] = cat_loc[i][1];
	}
	for(i=0; i<cheeses; i++) {
		cheese_loc_copy[i][0] = cheese_loc[i][0];
		cheese_loc_copy[i][1] = cheese_loc[i][1];
	}
	mouse_loc_copy[0][0] = mouse_loc[0][0];
	mouse_loc_copy[0][1] = mouse_loc[0][1];

	if (agentId == 0) {
		x = mouse_loc[0][0];
		y = mouse_loc[0][1];
	} else {
		x = cat_loc[agentId-1][0];
		y = cat_loc[agentId-1][1];
	}
	//value = -9999999999999;

	// Check if maximum search depth has been reached
	if(depth == maxDepth)
	{
		return utility(cat_loc_copy, cheese_loc_copy, mouse_loc_copy, cats, cheeses, depth, gr);
	}

	// Check if the game is in a terminal state
	for(i=0; i<cats; i++) {
		if(cat_loc[i][0] == mouse_loc[0][0] && cat_loc[i][1] == mouse_loc[0][1])
		{
		return utility(cat_loc_copy, cheese_loc_copy, mouse_loc_copy, cats, cheeses, depth, gr);
		}
	}
	for(i=0; i<cheeses; i++)
	{
		if(cheese_loc[i][0] == mouse_loc[0][0] && cheese_loc[i][1] == mouse_loc[0][1])
		{
		return utility(cat_loc_copy, cheese_loc_copy, mouse_loc_copy, cats, cheeses, depth, gr);
		}
	}
	// Recursively call MiniMax for each child node for mouse
	double child_value[4];
	int move[4][2];
	move[0][0] = x; move[0][1] = y-1;
	move[1][0] = x+1; move[1][1] = y;
	move[2][0] = x; move[2][1] = y+1;
	move[3][0] = x-1; move[3][1] = y;
	for (i=0; i<4; i++) {
		int curr_index = x + (y * size_X);
		if (gr[curr_index][i] == 1) {
			if (agentId == 0) {
				mouse_loc_copy[0][0] = move[i][0];
				mouse_loc_copy[0][1] = move[i][1];
				child_value[i] = MiniMax(gr, path_copy, minmax_cost, cat_loc_copy, cats, cheese_loc_copy, cheeses, mouse_loc_copy, mode, utility, (agentId+1)%(cats+1), depth+1, maxDepth, alpha, beta);
				minmax_cost[move[i][0]][move[i][1]] = child_value[i];
				//printf("%f\n", child_value[i]);
			} else {
				cat_loc_copy[agentId-1][0] = move[i][0];
				cat_loc_copy[agentId-1][1] = move[i][1];
				child_value[i] = MiniMax(gr, path_copy, minmax_cost, cat_loc_copy, cats, cheese_loc_copy, cheeses, mouse_loc_copy, mode, utility, (agentId+1)%(cats+1), depth+1, maxDepth, alpha, beta);
			}
		} else {
			if (agentId == 0) {
				child_value[i] = -99999999;
			} else {
				child_value[i] = 99999999;
			}
		}
	}
	double minmax = child_value[0];
	int minmax_index = 0;
	if (agentId == 0) {
		for (i=0; i<4; i++) {
			if (child_value[i] > minmax) {
				minmax = child_value[i];
				minmax_index = i;
				//printf("Mouse Child value %d Depth %d\n", minmax, depth);
			}
		}
		if (depth==0){
		printf("Mouse FOURTH CHILD val %f,%f,%f,%f\n", child_value[0], child_value[1], child_value[2], child_value[3]);}
	} else {
		for (i=0; i<4; i++) {
			if (child_value[i] < minmax) {
				minmax = child_value[i];
				minmax_index = i;
				//printf("Cat child value %f Depth %d\n", minmax, depth);
			}
		}
	}
	path[0][0]=move[minmax_index][0];
 	path[0][1]=move[minmax_index][1];
 	//printf("Agent ID: %d MINIMAX IS %f AT DEPTH %d\n AT CURRENT LOC x:%d, y:%d, CHEESE:(%d,%d)", agentId,minmax, depth,x,y,cheese_loc[0][0], cheese_loc[0][1]);
	return minmax;
}

double utility(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4])
{
 /*
	This function computes and returns the utility value for a given game configuration.
	As discussed in lecture, this should return a positive value for configurations that are 'good'
	for the mouse, and a negative value for locations that are 'bad' for the mouse.

	How to define 'good' and 'bad' is up to you. Note that you can write a utility function
	that favours your mouse or favours the cats, but that would be a bad idea... (why?)

	Input arguments:

		cat_loc - Cat locations
		cheese_loc - Cheese locations
		mouse_loc - Mouse location
		cats - # of cats
		cheeses - # of cheeses
		depth - current search depth
		gr - The graph's adjacency list for the maze

		These arguments are as described in A1. Do have a look at your solution!
 */

  int value;
  int min_cheese_dist = 150;
  int dist_curr_cheese;
  int min_cat_dist = 150;
  int dist_curr_cat;
  int closest_ch[1][2];
  int closest_cat[1][2];
  for(int i = 0; i < cheeses; i++){
    dist_curr_cheese = abs(cheese_loc[i][0] - mouse_loc[0][0]) + abs(cheese_loc[i][1] - mouse_loc[0][1]);
    if(dist_curr_cheese < min_cheese_dist){
      min_cheese_dist = dist_curr_cheese;
      closest_ch[0][0] = cheese_loc[i][0]; closest_ch[0][1] = cheese_loc[i][1];
    }
  }
  for(int i = 0; i < cats; i++){
    dist_curr_cat = abs(cat_loc[i][0] - mouse_loc[0][0]) + abs(cat_loc[i][1] - mouse_loc[0][1]);
    if(dist_curr_cat < min_cat_dist){
      min_cat_dist = dist_curr_cat;
      closest_cat[0][0] = cat_loc[i][0]; closest_cat[0][1] = cat_loc[i][1];
    }
  }
  //printf("CAT DIST: %d\n", min_cat_dist);
  //printf("CHEESE DIST: %d\n", min_cheese_dist);
  value =  0 - min_cheese_dist;
  
  int cat_ind = -1 * pow((5/6), min_cat_dist - 20);
  int cheese_ind = int(1.5*pow((5/6), min_cheese_dist - 20))+1;
  //value = cheese_ind + cat_ind;
  int cat_to_cheese = abs(closest_cat[0][0]-closest_ch[0][0]) + abs(closest_cat[0][1]-closest_ch[0][1]);
  int cat_to_ch_ind = pow((5/6), cat_to_cheese - 20);
  //idea use search to get length of path and compare it to distance from cat to cheese?
  //idea use UCS to cheese and also UCS from cat to mouse then compare the paths and also take into acc cat to cheese dist
  int path[graph_size][2];
  int visit_order[size_X][size_Y];
  
  int leng = UCS(gr, path, visit_order, cat_loc, cats, cheese_loc, cheeses, mouse_loc);
  //printf("PATH LENGTH: %d \n", leng);
  
  
  return 0- leng - depth;
}

void dequeue(Node** head){
	/**
	 * pop the first node in the linked list and free space
	 * input: 
	 * head: head of the linked list
	*/
    Node* temp = *head;
    (*head) = (*head)->next;
    free(temp);
}

void enqueue(Node** head, int x, int y, int cost){
	/**
	 * enqueue the new node into the linked list head
	 * input: 
	 * head: the head of the linked list
	 * x: x coordinate
	 * y: y coordinate
	 * cost: cost of going to the x and y coordinate
	*/
    Node* start = *head;
    Node* temp = newNode(x, y, cost);
	if ((*head) == NULL){
		*head = temp;
	} else {
		if ((*head)->cost >= cost){
			temp->next = *head; 
			(*head) = temp; 
		} else {
			while (start->next != NULL && start->next->cost <= cost){
				start = start->next;
			}
			temp->next = start->next; 
			start->next = temp; 
		}
	}
    
}
int UCS(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], 
			int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2])
{
		int x, y, cost, curr_index, path_idx = 1, path_index = 0;
		int start_x = mouse_loc[0][0], start_y = mouse_loc[0][1];
		int start_index = location_to_int(start_x, start_y);
		int end_indices[cheeses];
		for (int i = 0; i < cheeses; i++){
			end_indices[i] = location_to_int(cheese_loc[i][0], cheese_loc[i][1]);
		}
		for (int i = 0; i < cats; i++) {
			//printf("cat: %d, %d", cat_loc[i][0], cat_loc[i][1]);
		}
		// Initialize 2D arrays to keep track of visited locations and path
		int visited[size_X][size_Y] = {0};
		int parent[graph_size] = {-1};
		// Initialize queue and add starting location to it
		Node* head = newNode(mouse_loc[0][0], mouse_loc[0][1], 0); 
		while (!isEmpty(&head)) {
			x = head->x;
			y = head->y;
			cost = head->cost;
			dequeue(&head);
			//printf("dequeued: %d\n", cost);
			// Check if current location has been visited before
			if (visited[x][y]) {
			    continue;
			}
			visited[x][y] = 1;
			visit_order[x][y] = path_idx;
			path_idx++;
			curr_index = location_to_int(x, y);
			// Check if we have reached the end node
			if (in_end_indices(end_indices, cheeses, curr_index)){
				// Backtrack to construct the path
				int curr = curr_index;
				while (curr != start_index) {
					int x = curr % size_X;
					int y = curr / size_X;
					path[path_index][0] = x;
					path[path_index][1] = y;
					path_index++;
					curr = parent[curr];
				}
				// Add the start node to the path
				path[path_index][0] = start_x;
				path[path_index][1] = start_y;
				// reverse the path
				int tempX, tempY;
				//printf("path index: %d\n", path_index);
				for (int i = 0; i < path_index-i; i++){
					tempX = path[path_index-i][0], tempY = path[path_index-i][1];
					path[path_index-i][0] = path[i][0];
					path[path_index-i][1] = path[i][1];
					path[i][0] = tempX;
					path[i][1] = tempY;
				}
				// print the path
				int i = 0;
				while (path[i][0]!=-1) {
					//printf("path: %d, %d\n", path[i][0], path[i][1]);
					i++;
				}
				break;
			} // if we haven't reached the end node
			else {
				// Check all 4 neighbors (top, right, bottom, left)
				if (gr[curr_index][0] && !visited[x][y-1]) {
					cost = Hcost(x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);
					enqueue(&head, x, y-1, cost);
					parent[location_to_int(x, y-1)] = curr_index;
					//printf("top enqueued: %d\n", cost);
				}
				if (gr[curr_index][1] && !visited[x+1][y]) {
					cost = Hcost(x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);
					enqueue(&head, x+1, y, cost);
					parent[location_to_int(x+1, y)] = curr_index;
					//printf("right enqueued: %d\n", cost);
				}
				if (gr[curr_index][2] && !visited[x][y+1]) {
					cost = Hcost(x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);
					enqueue(&head, x, y+1, cost);
					parent[location_to_int(x, y+1)] = curr_index;
					//printf("bottom enqueued: %d\n", cost);
				}
				if (gr[curr_index][3] && !visited[x-1][y]) {
					cost = Hcost(x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);
					enqueue(&head, x-1, y, cost);
					parent[location_to_int(x-1, y)] = curr_index;
					//printf("left enqueued: %d\n", cost);
				}
			}
		}
		return path_index;
}
int Hcost(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])
{
/*
   This function computes and returns the heuristic cost for location x,y.
   As discussed in lecture, this means estimating the cost of getting from x,y to the goal.
   The goal is cheese. Which cheese is up to you.
   Whatever you code here, your heuristic must be admissible.
   Input arguments:
       x,y - Location for which this function will compute a heuristic search cost
       cat_loc - Cat locations
       cheese_loc - Cheese locations
       mouse_loc - Mouse location
       cats - # of cats
       cheeses - # of cheeses
       gr - The graph's adjacency list for the maze
       These arguments are as described in the search() function above
*/
	int min_cheese_distance = graph_size;
	for (int i = 0; i < cheeses; i++) {
    	int cheese_x = cheese_loc[i][0];
    	int cheese_y = cheese_loc[i][1];
    	int distance = abs(x - cheese_x) + abs(y - cheese_y);
    	if(min_cheese_distance > distance){
        	min_cheese_distance = distance;
        }
	}
	return min_cheese_distance;
}
int checkForTerminal(int mouse_loc[1][2],int cat_loc[10][2],int cheese_loc[10][2],int cats,int cheeses)
{
 /* 
   This function determines whether a given configuration constitutes a terminal node.
   Terminal nodes are those for which:
     - A cat eats the mouse
     or
     - The mouse eats a cheese
   
   If the node is a terminal, the function returns 1, else it returns 0
 */

 // Check for cats having lunch
 for (int i=0; i<cats; i++)
  if (mouse_loc[0][0]==cat_loc[i][0]&&mouse_loc[0][1]==cat_loc[i][1]) return(1);

 // Check for mouse having lunch
 for (int i=0; i<cheeses; i++)
  if (mouse_loc[0][0]==cheese_loc[i][0]&&mouse_loc[0][1]==cheese_loc[i][1]) return(1);

 return(0);

}

