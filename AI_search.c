/*
	CSC D84 - Unit 1 - Search

	This file contains stubs for implementing the different search
	algorithms covered in the course. Please read the assignment
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

	Starter code: F.J.E., Jul. 15
	Updated: F.J.E., Jan. 18
*/

/**********************************************************************
% COMPLETE THIS TEXT BOX:
%
% 1) Student Name: Haowen Rui		
% 2) Student Name: Andreas Alexander
%
% 1) Student number: 1005843688
% 2) Student number: 1005800368
% 
% 1) UtorID: ruihaowe
% 2) UtorID: alexa376
% 
% We hereby certify that the work contained here is our own
%
% ____Haowen Rui______             __Andreas Alexander__
% (sign with your name)            (sign with your name)
***********************************************************************/

#include "AI_search.h"

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

void search(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], 
			int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], 
			int mode, int (*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], 
			int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4]))
{
 /*
   This function is the interface between your solution for the assignment and the driver code. The driver code
   in AI_search_core_GL will call this function once per frame, and provide the following data
   
   Board and game layout:
	
	The game takes place on a grid of size 32x32, the file board_layout.h specifies the size and defines two
	constants 'size_X' and 'size_Y' for the horizontal and vertical size of the board, respectively. For our
	purposes, the grid of locations is represented by a graph with one node per grid location, so given
	the 32x32 cells, the graph has 1024 nodes. 

	To create a maze, we connect cell locations in the grid in such a way that a) there is a path from any
	grid location to any other grid location (i.e. there are no disconnected subsets of nodes in the graph),
	and b) there are loops.

	Since each node represents a grid location, each node can be connected to up to 4 neighbours in the
	top, right, bottom, and left directions respectively:

			node at (i,j-1)
				^
				|
	(node at i-1, j) <- node at (i,j) -> node at (i+1, j)
				|
				v
			node at (i,j+1)

	The graph is theredore stored as an adjacency list with size 1024 x 4, with one row per node in the
	graph, and 4 columns corresponding to the weight of an edge linking the node with each of its 4
	possible neighbours in the order towp, right, bottom, left (clockwise from top).

	Since all we care is whether nodes are connected. Weights will be either 0 or 1, if the weight is
	1, then the neighbouring nodes are connected, if the weight is 0, they are not. For example, if

	graph[i][0] = 0 
	graph[i][1] = 1
	graph[i][2] = 0
	graph[i][3] = 1

	then node i is connected to the right and left neighbours, but not to top or bottom.

	The index in the graph for the node corresponding to grid location (x,y) is

	index = x + (y*size_X) 		or in this case		index = x + (y*32)

	Conversely, if you have the index and want to figure out the grid location,

	x = index % size_X		or in this case		x = index % 32
	y = index / size_Y		or in this case		y = index / 32

	(all of the above are *integer* operations!)

	A path is a sequence of (x,y) grid locations. We store it using an array of size	
	1024 x 2 (since there are 1024 locations, this is the maximum length of any
	path that visits locations only once).

	Agent locations are coordinate pairs (x,y)	

   Arguments:
		gr[graph_size][4]   - This is an adjacency list for the maze
		path[graph_size][2] - An initially empty path for your code to fill.
				      In this case, empty means all entries are initially -1
		visit_order[size_X][size_Y]	- An array in which your code will store the
						  *order* in which grid locations were 
						  visited during search. For example, while
						  doing BFS, the initial location is the
						  start location, it's visit order is 1.
						  Then the search would expand the immediate
						  neighbours of the initial node in some order,
						  these would get a visit order of 2, 3, 4, and
						  5 respectively, and so on.
					
						  This array will be used to display the search
						  pattern generated by each search method.

		cat_loc[10][2], cats   - Location of cats and number of cats (we can have at most 10,
					 but there can be fewer). Only valid cat locations are 0 to (cats-1)
		cheese_loc[10][2], cheeses - Location and number of cheese chunks (again at most 10,
					     but possibly fewer). Valid locations are 0 to (cheeses-1)
		mouse_loc[1][2] - Mouse location - there can be only one!
		mode - Search mode selection:
					mode = 0 	- BFS
					mode = 1	- DFS
					mode = 2	- A*

		(*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[10][2], int cats, int cheeses)
				- This is a pointer to one of the heuristic functions you will implement, either H_cost()
				  or H_cost_nokitty(). The driver in AI_search_core_GL will pass the appropriate pointer
				  depending on what search the user wants to run.
			
				  If the mode is 0 or 1, this pointer is NULL

				  * How to call the heuristic function from within this function : *
					- Like any other function:
						h = heuristic( x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses);

   Return values:
		Your search code will directly update data passed-in as arguments:

		- path[graph_size][2]	: Your search code will update this array to contain the path from
					  the mouse to one of the cheese chunks. The order matters, so
					  path[0][:] must be the mouse's current location, path[1][:]
					  is the next move for the mouse. Each successive row will contain
					  the next move toward the cheese, and the path ends at a location
					  whose coordinates correspond to one of the cheese chunks.
					  Any entries beyond that must remain set to -1
		- visit_order[size_X][size_Y] 	:  Your search code will update this array to contain the
						   order in which each location in the grid was expanded
						   during search. This means, when that particular location
						   was checked for being a goal, and if not a goal, had its
						   neighbours added as candidates for future expansion (in
						   whatever order is dictated by the search mode).

						   Note that since there are 1024 locations, the values in 
						   this array must always be in [0, 1023]. The driver code
						   will then display search order as a yellow-colored
						   brightness map where nodes expanded earlier will look
						   brighter.

		* Your code MUST NOT modify the locations or numbers of cats and/or cheeses, the graph,
	     	  or the location of the mouse - if you try, the driver code will know it *
			
		That's that, now, implement your solution!
 */

 /********************************************************************************************************
 * 
 * TO DO:	Implement code to carry out the different types of search depending on the
 *		mode.
 *
 *		You can do this by writing code within this single function (and being clever about it!)
 *		Or, you can use this function as a wrapper that calls a different search function
 *		 (BFS, DFS, A*) depending on the mode. Note that in the latter case, you will have
 *	    	  to inform your A* function somehow of what heuristic it's supposed to use.
 *
 *		Visiting Order: When adding the neighbours of a node to your list of candidates for
 *				expansion, do so in the order top, right, bottom, left.
 *
 *		NOTE: Your search functions should be smart enough to not choose a path that goes
 *		      through a cat! this is easily done without any heuristics.
 * 
 *		How you design your solution is up to you. But:
 *
 *		- Document your implementation by adding concise and clear comments in this file
 *		- Document your design (how you implemented the solution, and why) in the report
 *
 ********************************************************************************************************/
	printf("mode %d\n", mode);
	printf("visit order: %d\n", path[0][0]);
	printf("mouse: %d, %d\n", mouse_loc[0][0], mouse_loc[0][1]);
	if(mode == 0) {
		// Initialize variables
		int start_x = mouse_loc[0][0], start_y = mouse_loc[0][1];
		int start_index = location_to_int(start_x, start_y);
		int end_indices[cheeses];
		for (int i = 0; i < cheeses; i++){
			end_indices[i] = location_to_int(cheese_loc[i][0], cheese_loc[i][1]);
		}
		int path_index = 0;
		int queue_head = 0, queue_tail = 0;

		// Initialize the path and visit_order arrays to -1
		// memset(path, -1, sizeof(path));
		// memset(visit_order, -1, sizeof(visit_order));

		// Initialize queue and variables for BFS
		int queue[graph_size];
		bool visited[graph_size] = {false};
		int parent[graph_size] = {-1};

		// Enqueue the starting node and mark it as visited
		queue[queue_tail] = start_index;
		queue_tail++;
		visited[start_index] = true;
		visit_order[start_x][start_y] = 1;
		int v = 2;

		// BFS loop
		while (queue_head != queue_tail) {
			int curr_index = queue[queue_head];
			queue_head++;

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
				int tempX, tempY;
				printf("path index: %d\n", path_index);
				for (int i = 0; i < path_index-i; i++){
					tempX = path[path_index-i][0], tempY = path[path_index-i][1];
					path[path_index-i][0] = path[i][0];
					path[path_index-i][1] = path[i][1];
					path[i][0] = tempX;
					path[i][1] = tempY;
				}
				int i = 0;
				while (path[i][0]!=-1) {
					printf("path: %d, %d\n", path[i][0], path[i][1]);
					i++;
				}
				break;
			} // if we haven't reached the end node
			else {
				// Get the x and y coordinates from the current index
				int x = curr_index % size_X;
				int y = curr_index / size_X;

				// Check the top neighbor
				if (gr[curr_index][0] == 1 && !visited[curr_index - size_X]) {
					int top_index = curr_index - size_X;
					int top_x = top_index % size_X;
					int top_y = top_index / size_X;
					queue[queue_tail] = top_index;
					queue_tail++;
					visited[top_index] = true;
					parent[top_index] = curr_index;
					visit_order[top_x][top_y] = v;
					v++;
				}

				// Check the right neighbor
				if (gr[curr_index][1] == 1 && !visited[curr_index + 1]) {
					int right_index = curr_index + 1;
					int right_x = right_index % size_X;
					int right_y = right_index / size_X;
					queue[queue_tail] = right_index;
					queue_tail++;
					visited[right_index] = true;
					parent[right_index] = curr_index;
					visit_order[right_x][right_y] = v;
					v++;
				}

				// Check the bottom neighbor
				if (gr[curr_index][2] == 1 && !visited[curr_index + size_X]) {
					int bottom_index = curr_index + size_X;
					int bottom_x = bottom_index % size_X;
					int bottom_y = bottom_index / size_X;
					queue[queue_tail] = bottom_index;
					queue_tail++;
					visited[bottom_index] = true;
					parent[bottom_index] = curr_index;
					visit_order[bottom_x][bottom_y] = v;
					v++;
				}

				// Check the left neighbor
				if (gr[curr_index][3] == 1 && !visited[curr_index - 1]) {
					int left_index = curr_index - 1;
					int left_x = left_index % size_X;
					int left_y = left_index / size_X;
					queue[queue_tail] = left_index;
					queue_tail++;
					visited[left_index] = true;
					parent[left_index] = curr_index;
					visit_order[left_x][left_y] = v;
					v++;
				}
			}
		}
	} else if (mode == 1) {
		printf("mode is 1");
		// Initialize the visit_order and path arrays
		// Set the starting location of the mouse
		int x = mouse_loc[0][0];
		int y = mouse_loc[0][1];
		int index = x + (y * size_X);
		// Create an array to simulate a stack for DFS
		int toVisit[graph_size];
		int stackPointer = 0;
		toVisit[stackPointer] = index;
		int parent[graph_size];
		// Set the initial visit order
		visit_order[x][y] = 1;
		// Create a variable to store the current visit order
		int currentVisitOrder = 1;
		// Create a variable to store the current path index
		int pathIndex = 0;
		// Create a variable to store the current node
		int currentNode;
		// Create a variable to store the next node
		int nextNode;
		// Create a variable to store the x and y coordinates of the current node
		int currentX;
		int currentY;
		// Create a variable to store the x and y coordinates of the next node
		int nextX;
		int nextY;
		// Create a variable to store the index of the next node
		int nextIndex;
		// Create a variable to check if the mouse found a cheese
		bool foundCheese = false;
		while (stackPointer >= 0) {
			// Get the current node from the stack
			currentNode = toVisit[stackPointer];
			stackPointer--;
			// Get the x and y coordinates of the current node
			currentX = currentNode % size_X;
			currentY = currentNode / size_X;
			// Check if the current node is a cheese
			for (int i = 0; i < cheeses; i++) {
				if (currentX == cheese_loc[i][0] && currentY == cheese_loc[i][1]) {
					foundCheese = true;
					break;
				}
			}
		visit_order[currentX][currentY] = currentVisitOrder;
			currentVisitOrder++;
			// If the mouse found a cheese, update the path and break out of the loop
			if (foundCheese) {
				path[pathIndex][0] = currentX;
				path[pathIndex][1] = currentY;
				pathIndex++;
				break;
				}
				// Check the top neighbor
			if (gr[currentNode][0] == 1) {
				nextX = currentX;
				nextY = currentY - 1;
				nextIndex = nextX + nextY * size_X;
				if (visit_order[nextX][nextY] == 0) {
					parent[nextIndex] = currentNode;
					toVisit[++stackPointer] = nextIndex;
				}
			}
			// Check the right neighbor
			if (gr[currentNode][1] == 1) {
				nextX = currentX + 1;
				nextY = currentY;
				nextIndex = nextX + nextY * size_X;
				if (visit_order[nextX][nextY] == 0) {
					toVisit[++stackPointer] = nextIndex;
					parent[nextIndex] = currentNode;
				}
			}
			// Check the bottom neighbor
			if (gr[currentNode][2] == 1) {
				nextX = currentX;
				nextY = currentY + 1;
				nextIndex = nextX + nextY * size_X;
			
				if (visit_order[nextX][nextY] == 0) {
					toVisit[++stackPointer] = nextIndex;
					parent[nextIndex] = currentNode;
				}
			}
			// Check the left neighbor
			if (gr[currentNode][3] == 1) {
				nextX = currentX - 1;
				nextY = currentY;
				nextIndex = nextX + nextY * size_X;
				if (visit_order[nextX][nextY] == 0) {
					toVisit[++stackPointer] = nextIndex;
					parent[nextIndex] = currentNode;
				}
			}
		}
		// Backtrack to construct the path if cheese is found
		if (foundCheese) {
			stackPointer++;
			currentNode = toVisit[stackPointer];
			currentX = currentNode % size_X;
			currentY = currentNode / size_X;
			while (currentNode != index) {
				path[pathIndex][0] = currentX;
				path[pathIndex][1] = currentY;
				pathIndex++;
				// Check the top neighbor
				if(gr[currentNode][0] == 1 && parent[(currentX)+(currentY*size_X)] == (currentX)+((currentY-1)*size_X)){  
				printf("top\n");
				currentX = currentX;
				currentY = currentY - 1;
				currentNode = currentX + currentY * size_X;
				}
				// Check the right neighbor
				else if(gr[currentNode][1] == 1 && parent[(currentX)+(currentY*size_X)] == (currentX+1)+((currentY)*size_X)){   
				printf("right\n");
				currentX = currentX + 1;
				currentY = currentY;
				currentNode = currentX + currentY * size_X;
				}
				// Check the bottom neighbor
				else if(gr[currentNode][2] == 1 && parent[(currentX)+(currentY*size_X)] == (currentX)+((currentY+1)*size_X)){  
				printf("bot\n");
				currentX = currentX;
				currentY = currentY + 1;
				currentNode = currentX + currentY * size_X;
				}
				// Check the left neighbor
				else if(gr[currentNode][3] == 1 && parent[(currentX)+(currentY*size_X)] == (currentX-1)+((currentY)*size_X)){  
				printf("left\n");
				currentX = currentX - 1;
				currentY = currentY;
				currentNode = currentX + currentY * size_X;
				}
			}
			// Add the starting location to the path
			path[pathIndex][0] = mouse_loc[0][0];
			path[pathIndex][1] = mouse_loc[0][1];
			int temp[pathIndex-1][2];
			int tempX, tempY;
			int j = 0;
			printf("path index: %d\n", pathIndex);
			while (path[j][0]!=-1) {
				printf("path: %d, %d\n", path[j][0], path[j][1]);
				j++;
			}
			printf("PATH END \n");
		    for(int a = 0; a < pathIndex; a++){
				temp[a][0] = -1;
				temp[a][1] = -1;
			}
			for(int i = 0; i < pathIndex; i++){
				temp[i][0] = path[pathIndex-i][0];
				temp[i][1] = path[pathIndex-i][1];
			}
			for(int k = 0; k < pathIndex; k++){
				path[k][0] = temp[k][0];
				path[k][1] = temp[k][1];
			}
			path[pathIndex][0] = -1;
			path[pathIndex][1] = -1;
			j = 0;
			while (path[j][0]!=-1) {
				printf("path: %d, %d\n", path[j][0], path[j][1]);
				j++;
			}
		}
	} else if (mode == 2) {
		int x, y, cost, curr_index, path_idx = 1, path_index = 0;
		int start_x = mouse_loc[0][0], start_y = mouse_loc[0][1];
		int start_index = location_to_int(start_x, start_y);
		int end_indices[cheeses];
		for (int i = 0; i < cheeses; i++){
			end_indices[i] = location_to_int(cheese_loc[i][0], cheese_loc[i][1]);
		}
		for (int i = 0; i < cats; i++) {
			printf("cat: %d, %d", cat_loc[i][0], cat_loc[i][1]);
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
			printf("dequeued: %d\n", cost);
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
				printf("path index: %d\n", path_index);
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
					printf("path: %d, %d\n", path[i][0], path[i][1]);
					i++;
				}
				break;
			} // if we haven't reached the end node
			else {
				// Check all 4 neighbors (top, right, bottom, left)
				if (gr[curr_index][0] && !visited[x][y-1]) {
					cost = heuristic(x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);
					enqueue(&head, x, y-1, cost);
					parent[location_to_int(x, y-1)] = curr_index;
					printf("top enqueued: %d\n", cost);
				}
				if (gr[curr_index][1] && !visited[x+1][y]) {
					cost = heuristic(x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);
					enqueue(&head, x+1, y, cost);
					parent[location_to_int(x+1, y)] = curr_index;
					printf("right enqueued: %d\n", cost);
				}
				if (gr[curr_index][2] && !visited[x][y+1]) {
					cost = heuristic(x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);
					enqueue(&head, x, y+1, cost);
					parent[location_to_int(x, y+1)] = curr_index;
					printf("bottom enqueued: %d\n", cost);
				}
				if (gr[curr_index][3] && !visited[x-1][y]) {
					cost = heuristic(x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);
					enqueue(&head, x-1, y, cost);
					parent[location_to_int(x-1, y)] = curr_index;
					printf("left enqueued: %d\n", cost);
				}
			}
		}
	}
	for (int a = 0; a < 32; a++) {
		for (int b = 0; b < 32; b++) {
			printf("%d ", visit_order[b][a]);
		}
		printf("\n");
	}
	return;
}

int H_cost(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])
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


int H_cost_nokitty(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])
{
/*
	This function computes and returns the heuristic cost for location x,y.
	As discussed in lecture, this means estimating the cost of getting from x,y to the goal. 
	The goal is cheese. 

	However - this time you want your heuristic function to help the mouse avoid being eaten.
	Therefore: You have to somehow incorporate knowledge of the cats' locations into your
	heuristic cost estimate. How well you do this will determine how well your mouse behaves
	and how good it is at escaping kitties.

	This heuristic *does not have to* be admissible.

	Input arguments have the same meaning as in the H_cost() function above.
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
	if (min_cheese_distance == 1) {
		return 0;
	}
	int cat_penalty = 0;
	int cat_dist = graph_size;
	for (int i = 0; i < cats; i++) {
    	int cat_x = cat_loc[i][0];
    	int cat_y = cat_loc[i][1];
    	int distance = abs(x - cat_x) + abs(y - cat_y);
    	int cost = pow(2, 5-distance);
		cat_dist = distance;
		cat_penalty += cost;
	}
	return min_cheese_distance + cat_penalty/cats;
}
