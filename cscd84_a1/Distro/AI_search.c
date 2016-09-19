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

	Starter code: F.J.E. Jul. 15
*/

#include "AI_search.h"

int cat_index (int cat_loc[10][2], int cats, int path_x, int path_y){ //Used to check if a cat is on the current considered path. If it is we don't add that direction into the search

	for (int i = 0; i < cats; i++){
		if (cat_loc[i][0] == path_x && cat_loc[i][1] == path_y){
			return 0;
		}
	}

	return 1;

}

int distance(int x_1, int y_1, int x_2, int y_2){ //Returns the Manhattan distance from (x_1, y_1) to (x_2, y_2)
	return (abs(x_1 - x_2) + abs(y_1 - y_2)); 
}

int closest_cheese(int cheese_loc[10][2], int cheeses, int node_x, int node_y){ //Finds the closest cheese to a node that will be used in our heurstics
	int closest = 0;	//Keeps track of which cheese is the closest
	int dist = distance(cheese_loc[0][0], cheese_loc[0][1], node_x, node_y);	//Will be used to store the distance from the node to a particular cheese

	for (int i = 0; i < cheeses; i++){
		if (distance(cheese_loc[i][0], cheese_loc[i][1], node_x, node_y) <= dist){
			closest = i;
		}
	}

	return closest;
}

int is_cat_close (int cat_loc[10][2], int cats, int x, int y){ //Used to check if a given node is too close to a cat. Called by H_cost_nokitties

	int how_close = 0;

	for (int i = 0; i < cats; i++){
		if (distance(cat_loc[i][0], cat_loc[i][1], x, y) < 5){
			how_close++;
		}
	}

	return how_close;
}

int cat_danger (int cat_loc[10][2], int cats, int x, int y){ //Used to check if a given node is too close to a cat. Called by H_cost_nokitties
	
	int danger_meter = 0;

	for (int i = 0; i < cats; i++){
		danger_meter = danger_meter + distance(cat_loc[i][0], cat_loc[i][1], x, y);
	}

	return danger_meter;
}

int how_open (int x, int y, double gr[graph_size][4]){
	int openness = 0;
	int graph_coord = x + y * 32;

	for (int i = 0; i < 4; i++){
		if (gr[graph_coord][i] == 0){
			openness++;
		}
	}

	return openness;

}


void search(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode, int (*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4]))
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
 
 if (mode == 0){
 	//BFS

 	int visited[graph_size]; //Keeps track of what nodes have been visited already by BFS
 	int bfs_queue[graph_size]; //Array queue for our BFS to go through

	int path_x = 0; //Used for path updating later
	int path_y = 0;
	int path_depth = visit_order[path_x][path_y];
	int path_current = 0;
	int cheese_coordinate = 0; //Location of cheeses

 	for (int m = 0; m < graph_size; m++){
 		visited[m] = -1; //Initialize all nodes to be unvisited at start
 		bfs_queue[m] = -1; //Initialize our queue to be all -1s at start
 		visit_order[m % size_X][m / size_Y] = 0;
 	}

 	for (int m = 0; m < graph_size; m++){
 		for (int n = 0; n < 2; n++){
 			path[m][n] = -1;	//initialize all paths to be -1 at start
 		}
 	}

 	int visit_x = mouse_loc[0][0]; //Used for visit_order later
 	int visit_y = mouse_loc[0][1];

 	path[0][0] = visit_x;
 	path[0][1] = visit_y;

 	bfs_queue[0] = mouse_loc[0][0] + (mouse_loc[0][1] * 32); //First node in our queue is the starting location of the mouse
 	visited[bfs_queue[0]] = bfs_queue[0];

 	int tail = 1; //End of the queue

 	visit_order[visit_x][visit_y] = 0; //Visit order of starting node is 0
 	
 	for (int head = 0; head < graph_size; head++){
 		if (bfs_queue[head] != -1){	//Checking that the queue is not empty

 			visit_x = bfs_queue[head] % size_X; //Update our visit_order coordinates
 			visit_y = bfs_queue[head] / size_Y;

 			if (gr[bfs_queue[head]][0] == 1 && visited[bfs_queue[head] - 32] == -1 && cat_index(cat_loc, cats, visit_x, visit_y - 1)){			//If top neighbour is reachable and have not been visited
 				bfs_queue[tail] = bfs_queue[head] - 32;											//Add the node to the queue
 				visited[bfs_queue[head] - 32] = bfs_queue[head];								//We check off that the current node has been visited
 				visit_order[visit_x][visit_y - 1] = visit_order[visit_x][visit_y] + 1;			//Set the correct visit order for the reachable node
 				tail++;																			//Update the tail that points to the end of our queue																	
 			}
 			
 			if (gr[bfs_queue[head]][1] == 1 && visited[bfs_queue[head] + 1] == -1 && cat_index(cat_loc, cats, visit_x + 1, visit_y)){				//If right neighbour is reachable and have not been visited
 				bfs_queue[tail] = bfs_queue[head] + 1;
 				visited[bfs_queue[head] + 1] = bfs_queue[head];
 				visit_order[visit_x + 1][visit_y] = visit_order[visit_x][visit_y] + 1;
 				tail++;
 			}
 			
 			if (gr[bfs_queue[head]][2] == 1 && visited[bfs_queue[head] + 32] == -1 && cat_index(cat_loc, cats, visit_x, visit_y + 1)){				//If bottom neighbour is reachable and have not been visited
 				bfs_queue[tail] = bfs_queue[head] + 32;
 				visited[bfs_queue[head] + 32] = bfs_queue[head];
 				visit_order[visit_x][visit_y + 1] = visit_order[visit_x][visit_y] + 1;
 				tail++;
 			}
 			if (gr[bfs_queue[head]][3] == 1 && visited[bfs_queue[head] - 1] == -1 && cat_index(cat_loc, cats, visit_x - 1, visit_y)){				//If left neighbour is reachable and have not been visited
 				bfs_queue[tail] = bfs_queue[head] - 1;
 				visited[bfs_queue[head] - 1] = bfs_queue[head];
 				visit_order[visit_x - 1][visit_y] = visit_order[visit_x][visit_y] + 1;
 				tail++;
 			}
	 		
	 		for (int i = 0; i < cheeses; i++){
	 			cheese_coordinate = cheese_loc[i][0] + (cheese_loc[i][1] * 32); //Get the graph location of the cheeses
	 			if (visited[cheese_coordinate] != -1){ 							//If a coordinate with a cheese on it has been visited
	 				
	 				for (int j = 0; j < graph_size; j++){ //Just making sure our mouse doesn't go out of bounds after eating a cheese
	 					path[j][0] = cheese_loc[i][0];
	 					path[j][1] = cheese_loc[i][1];
	 				}

	 				path_x = cheese_loc[i][0];
	 				path_y = cheese_loc[i][1];
	 				path_depth = visit_order[path_x][path_y];
	 				while (path_depth > 0){										//Trace back the path depth to find the path to the cheese
	 					path_current = path_x + (path_y * 32);					
	 					path[path_depth][0] = path_x;
	 					path[path_depth][1] = path_y;
	 					path_x = visited[path_current] % size_X;
	 					path_y = visited[path_current] / size_Y;
	 					path_depth--;
	 				}
					
	 				return;						
	 			}
	 		}
 		}
 	}
 }


 else if (mode == 1){
 	//DFS

 	int visited[graph_size]; //Keeps track of what nodes have been visited already by DFS
 	int dfs_stack[graph_size]; //Array stack for our DFS to go through

	int cheese_coordinate = 0; //Location of cheeses
	int stack_depth = 0;

 	for (int m = 0; m < graph_size; m++){
 		visited[m] = -1; //Initialize all nodes to be unvisited at start
 		dfs_stack[m] = -1; //Initialize our stack to be all -1s at start
 		visit_order[m % size_X][m / size_Y] = 0;
 	}

 	for (int m = 0; m < graph_size; m++){
 		for (int n = 0; n < 2; n++){
 			path[m][n] = -1;	//initialize all paths to be -1 at start
 		}
 	}

 	int visit_x = mouse_loc[0][0]; //Used for visit_order later
 	int visit_y = mouse_loc[0][1];

 	path[0][0] = visit_x;
 	path[0][1] = visit_y;

 	dfs_stack[0] = mouse_loc[0][0] + (mouse_loc[0][1] * 32); //First node in our stack is the starting location of the mouse
 	visited[dfs_stack[0]] = dfs_stack[0];

 	int stack_top = 0; //Top of our stack

 	visit_order[visit_x][visit_y] = 0; //Visit order of starting node is 0

 	int depth = 1;

 	while (stack_top >= 0){
 		if (dfs_stack[stack_top] != -1){		//We have not run out of elements to expand

 			visit_x = dfs_stack[stack_top] % size_X; //Update our visit_order coordinates
 			visit_y = dfs_stack[stack_top] / size_Y;

 			if (gr[dfs_stack[stack_top]][0] == 1 && visited[dfs_stack[stack_top] - 32] == -1 && cat_index(cat_loc, cats, visit_x, visit_y - 1)){ 			//If top neighbour is reachable and have not been visited
 				dfs_stack[stack_top + 1] = dfs_stack[stack_top] - 32;									//Add the node to the stack
 				visited[dfs_stack[stack_top] - 32] = visited[dfs_stack[stack_top]];						//We check off that the current node has been visited
 				visit_order[visit_x][visit_y - 1] = depth;											//Set the correct visit order for the reachable node
 				stack_top++;		
 				depth++;																	//Update the stack_top index that points to the end of our queue
 			}

 			else if (gr[dfs_stack[stack_top]][1] == 1 && visited[dfs_stack[stack_top] + 1] == -1 && cat_index(cat_loc, cats, visit_x + 1, visit_y)){ 	//If right neighbour is reachable and have not been visited
 				dfs_stack[stack_top + 1] = dfs_stack[stack_top] + 1;										
 				visited[dfs_stack[stack_top] + 1] = visited[dfs_stack[stack_top]];										
 				visit_order[visit_x + 1][visit_y] = depth;						
 				stack_top++;
 				depth++;																				
 			}

 			else if (gr[dfs_stack[stack_top]][2] == 1 && visited[dfs_stack[stack_top] + 32] == -1 && cat_index(cat_loc, cats, visit_x, visit_y + 1)){ //If bottom neighbour is reachable and have not been visited
 				dfs_stack[stack_top + 1] = dfs_stack[stack_top] + 32;										
 				visited[dfs_stack[stack_top] + 32] = visited[dfs_stack[stack_top]];										
 				visit_order[visit_x][visit_y + 1] = depth;						
 				stack_top++;								
 				depth++;												
 			}

 			else if (gr[dfs_stack[stack_top]][3] == 1 && visited[dfs_stack[stack_top] - 1] == -1 && cat_index(cat_loc, cats, visit_x - 1, visit_y)){ 	//If left neighbour is reachable and have not been visited
 				dfs_stack[stack_top + 1] = dfs_stack[stack_top] - 1;										
 				visited[dfs_stack[stack_top] - 1] = visited[dfs_stack[stack_top]];										
 				visit_order[visit_x - 1][visit_y] = depth;						
 				stack_top++;								
 				depth++;												
 			}

 			else { 				//If there are no unvisited nodes left or this node is walled in we "pop" the top element
 				stack_top--; 	//Move our pointer down the stack by 1
 			}

	 		for (int i = 0; i < cheeses; i++){
	 			cheese_coordinate = cheese_loc[i][0] + (cheese_loc[i][1] * 32); 			//Get the graph location of the cheeses

	 			if (visited[cheese_coordinate] != -1){ 										//If a coordinate with a cheese on it has been visited
	 				
	 				
	 				for (int j = 0; j < graph_size; j++){ 									//Just making sure our mouse doesn't go out of bounds after eating a cheese
	 					path[j][0] = cheese_loc[i][0];
	 					path[j][1] = cheese_loc[i][1];
	 				}
					
					
	 				while (stack_depth <= stack_top){										//Build our path by going through our stack from the  up			
	 					path[stack_depth][0] = dfs_stack[stack_depth] % size_X;
	 					path[stack_depth][1] = dfs_stack[stack_depth] / size_Y;
	 					stack_depth++;
	 				}

	 				return;						
	 			}
	 		}
 		}
 	}
 }

 
 else if (mode == 2){

 	//////////////////////
	//A*
 	//////////////////////
	int visited[graph_size]; //Keeps track of what nodes have been visited already by A*
	int graph_coord = 0;
	struct Node *current_node;
	int depth = 0;
	int p_cost = 1;
	int h_cost = 0;

	int path_increment = 0;

	int visit_x = 0;
	int visit_y = 0;

	int cheese_coordinate = 0;

	for (int i = 0; i < graph_size; i++){
 		visited[i] = -1; //Initialize all nodes to be unvisited at start
 		visit_order[i % size_X][i / size_Y] = 0;
 	}

 	for (int m = 0; m < graph_size; m++){
 		for (int n = 0; n < 2; n++){
 			path[m][n] = -1;	//initialize all paths to be -1 at start
 		}
 	}

 	struct Node *mouse = newnode(H_cost(mouse_loc[0][0], mouse_loc[0][1], cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr), (mouse_loc[0][0] + (mouse_loc[0][1] * 32)), NULL, NULL);
 	struct pQueue *pQueue = create_pQueue(mouse);

 	while (pQueue->head != NULL){
 		graph_coord = pQueue->head->graph_coordinate;

		visit_x = pQueue->head->graph_coordinate % size_X; //Update our visit_order coordinates
 		visit_y = pQueue->head->graph_coordinate / size_Y;

 		pQueue->dequeue(pQueue);

 		if (gr[graph_coord][0] == 1 && visited[graph_coord - 32] == -1 && cat_index(cat_loc, cats, visit_x, visit_y - 1)){ //Top
 			h_cost = heuristic(visit_x, visit_y - 1, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr) + p_cost;
 			current_node = newnode(h_cost, visit_x + (visit_y - 1) * 32, NULL, NULL);
 			pQueue->enqueue(pQueue, current_node);
 			visited[graph_coord - 32] = graph_coord;
 			visit_order[visit_x][visit_y - 1] = depth;
 			depth++;
 		}

 		if (gr[graph_coord][1] == 1 && visited[graph_coord + 1] == -1 && cat_index(cat_loc, cats, visit_x + 1, visit_y)){ //Right
 			h_cost = heuristic(visit_x + 1, visit_y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr) + p_cost;
 			current_node = newnode(h_cost, (visit_x + 1) + visit_y * 32, NULL, NULL);
 			pQueue->enqueue(pQueue, current_node);
 			visited[graph_coord + 1] = graph_coord;
 			visit_order[visit_x + 1][visit_y] = depth;
 			depth++;
 		}

 		if (gr[graph_coord][2] == 1 && visited[graph_coord + 32] == -1 && cat_index(cat_loc, cats, visit_x, visit_y + 1)){ //Bottom
 			h_cost = heuristic(visit_x, visit_y + 1, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr) + p_cost;
 			current_node = newnode(h_cost, visit_x + (visit_y + 1) * 32, NULL, NULL);
 			pQueue->enqueue(pQueue, current_node);
 			visited[graph_coord + 32] = graph_coord;
 			visit_order[visit_x][visit_y + 1] = depth;
 			depth++;
 		}

 		if (gr[graph_coord][3] == 1 && visited[graph_coord - 1] == -1 && cat_index(cat_loc, cats, visit_x - 1, visit_y)){ //Left
 			h_cost = heuristic(visit_x - 1, visit_y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr) + p_cost;
 			current_node = newnode(h_cost, (visit_x - 1) + visit_y * 32, NULL, NULL);
 			pQueue->enqueue(pQueue, current_node);
 			visited[graph_coord - 1] = graph_coord;
 			visit_order[visit_x - 1][visit_y] = depth;
 			depth++;
 		}

 		//p_cost++;

 		for (int i = 0; i < cheeses; i++){
 			cheese_coordinate = cheese_loc[i][0] + (cheese_loc[i][1] * 32); 			//Get the graph location of the cheeses

 			if (visited[cheese_coordinate] != -1){ 										//If a coordinate with a cheese on it has been visited

 				int mouse_location = mouse_loc[0][0] + mouse_loc[0][1] * 32;

 				while (cheese_coordinate != mouse_location){
 					path[path_increment][0] = cheese_coordinate % size_X;
 					path[path_increment][1] = cheese_coordinate / size_Y;
 					cheese_coordinate = visited[cheese_coordinate];
 					path_increment++;
 				}

 				path[path_increment][0] = mouse_loc[0][0];
 				path[path_increment][1] = mouse_loc[0][1];

 				int temp[graph_size][2];

 				for (int i = path_increment; i >= 0; i--){
 					temp[path_increment - i][0] = path[i][0];
 					temp[path_increment - i][1] = path[i][1];
 				}
 				
 				for (int i = path_increment; i < graph_size; i++){
 					temp[i][0] = temp[path_increment][0];
 					temp[i][1] = temp[path_increment][1];
 				}

 				for (int i = 0; i < graph_size; i++){
 					path[i][0] = temp[i][0];
 					path[i][1] = temp[i][1];
 				}

 				return;						
 			}
 		}
 	} 
 }


 else {
 	//Default random search

 	 path[0][0]=mouse_loc[0][0];
	 path[0][1]=mouse_loc[0][1];
	 path[1][0]=mouse_loc[0][0];
	 path[1][1]=mouse_loc[0][1];

	 return;
 }

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

	int cheese = closest_cheese(cheese_loc, cheeses, x, y); //Find the closest cheese
	int cheese_distance = distance(x, y, cheese_loc[cheese][0], cheese_loc[cheese][1]); //Distance from current point to cheese
	int point_distance = distance(x, y, mouse_loc[0][0], mouse_loc[0][1]); //Distance from the mouse to cheese

	return cheese_distance;
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

	int cheese = closest_cheese(cheese_loc, cheeses, x, y); //Find the closest cheese
	int cheese_distance = distance(x, y, cheese_loc[cheese][0], cheese_loc[cheese][1]); //Distance from current point to cheese
	int point_distance = distance(x, y, mouse_loc[0][0], mouse_loc[0][1]); //Distance from the mouse to cheese
	int danger = cat_danger(cat_loc, cats, x ,y); //How "dangerous" a square is
	int how_close = is_cat_close(cat_loc, cats, x, y); //How many cats are too close to a square
	int closeness = how_open(x, y, gr);

	if (cheese_distance < 10){
		return cheese_distance;
	}
	else{
		return cheese_distance + how_close * 50 + closeness * 25;
	}
}