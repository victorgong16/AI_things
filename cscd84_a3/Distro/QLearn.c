/*
	CSC D84 - Unit 3 - Reinforcement Learning
	
	This file contains stubs for implementing the Q-Learning method
	for reinforcement learning as discussed in lecture. You have to
	complete two versions of Q-Learning.
	
	* Standard Q-Learning, based on a full-state representation and
	  a large Q-Table
	* Feature based Q-Learning to handle problems too big to allow
	  for a full-state representation
	    
	Read the assignment handout carefully, then implement the
	required functions below. Sections where you have to add code
	are marked

	**************
	*** TO DO:
	**************

	If you add any helper functions, make sure you document them
	properly and indicate in the report.txt file what you added.
	
	Have fun!

	DO NOT FORGET TO 'valgrind' YOUR CODE - We will check for pointer
	management being done properly, and for memory leaks.

	Starter code: F.J.E. Jan. 16
*/

#include "QLearn.h"
#include <time.h>
#include <stdlib.h>

void QLearn_update(int s, int a, double r, int s_new, double *QTable)
{
 /*
   This function implementes the Q-Learning update as stated in Lecture. It 
   receives as input a <s,a,r,s'> tuple, and updates the Q-table accordingly.
   
   Your work here is to calculate the required update for the Q-table entry
   for state s, and apply it to the Q-table
     
   The update involves two constants, alpha and lambda, which are defined in QLearn.h - you should not 
   have to change their values. Use them as they are.
     
   Details on how states are used for indexing into the QTable are shown
   below, in the comments for QLearn_action. Be sure to read those as well!
 */
 
  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/

  int max_action = *(QTable+(4*s_new)+0);

  for (int i = 0; i < 4; i++){
    if (*(QTable+(4*s_new)+i) > max_action){
      max_action = *(QTable+(4*s_new)+i);
    }
  }

  *(QTable+(4*s)+a) += alpha * ((r + lambda * max_action) - *(QTable+(4*s)+a));
}

int QLearn_action(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], double pct, double *QTable, int size_X, int graph_size)
{
  /*
     This function decides the action the mouse will take. It receives as inputs
     - The graph - so you can check for walls! The mouse must never move through walls
     - The mouse position
     - The cat position
     - The chees position
     - A 'pct' value in [0,1] indicating the amount of time the mouse uses the QTable to decide its action,
       for example, if pct=.25, then 25% of the time the mouse uses the QTable to choose its action,
       the remaining 75% of the time it chooses randomly among the available actions.
       
     Remember that the training process involves random exploration initially, but as training
     proceeds we use the QTable more and more, in order to improve our QTable values around promising
     actions.
     
     The value of pct is controlled by QLearn_core_GL, and increases with each round of training.
     
     This function *must return* an action index in [0,3] where
        0 - move up
        1 - move right
        2 - move down
        3 - move left
        
     QLearn_core_GL will print a warning if your action makes the mouse cross a wall, or if it makes
     the mouse leave the map - this should not happen. If you see a warning, fix the code in this
     function!
     
   The Q-table has been pre-allocated and initialized to 0. The Q-table has
   a size of
   
        graph_size^3 x 4
        
   This is because the table requires one entry for each possible state, and
   the state is comprised of the position of the mouse, cat, and cheese. 
   Since each of these agents can be in one of graph_size positions, all
   possible combinations yield graph_size^3 states.
   
   Now, for each state, the mouse has up to 4 possible moves (up, right,
   down, and left). We ignore here the fact that some moves are not possible
   from some states (due to walls) - it is up to the QLearn_action() function
   to make sure the mouse never crosses a wall. 
   
   So all in all, you have a big table.
        
   For example, on an 8x8 maze, the Q-table will have a size of
   
       64^3 x 4  entries
       
       with 
       
       size_X = 8		<--- size of one side of the maze
       graph_size = 64		<--- Total number of nodes in the graph
       
   Indexing within the Q-table works as follows:
   
     say the mouse is at   i,j
         the cat is at     k,l
         the cheese is at  m,n
         
     state = (i+(j*size_X)) * ((k+(l*size_X))*graph_size) + ((m+(n*size_X))*graph_size*graph_size)
     ** Make sure you undestand the state encoding above!
     
     Entries in the Q-table for this state are

     *(QTable+(4*state)+a)      <-- here a is the action in [0,3]
     
     (yes, it's a linear array, no shorcuts with brackets!)
     
     NOTE: There is only one cat and once cheese, so you only need to use cats[0][:] and cheeses[0][:]
   */
  
  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/  

  int direction = rand() % 4;

  double random_choice = rand() % 100;

  int mouse_index = mouse_pos[0][0] + (mouse_pos[0][1] * size_X);

  int cat_index = cats[0][0]+(cats[0][1]*size_X);

  int cheese_index = cheeses[0][0]+(cheeses[0][1]*size_X);

  int state = mouse_index + (cat_index*graph_size) + (cheese_index*graph_size*graph_size);

  int best_action;

  while (gr[mouse_index][direction] != 1){    //Make sure the randomly picked direction is actually a legal move
      direction = rand() % 4;
  }

  best_action = *(QTable+(4*state)+direction);

  if (random_choice <= pct * 100){  //If we are going to use the QTable, do the following calculation to get the best action according to the QTable entries
    for (int i = 0; i < 4; i++){
      if (gr[mouse_index][i] == 1 && *(QTable+(4*state)+i) > best_action){
        best_action = *(QTable+(4*state)+i);
        direction = i;              //If we find a better direction to go, make sure to change the value to be returned to the best action so far
      }
    }
  }

  return direction; //Since we have already randomly picked a legal direction to go to, if we didn't use the QTable entries just return direction
}

double QLearn_reward(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
    This function computes and returns a reward for the state represented by the input mouse, cat, and
    cheese position. 
    
    You can make this function as simple or as complex as you like. But it should return positive values
    for states that are favorable to the mouse, and negative values for states that are bad for the 
    mouse.
    
    I am providing you with the graph, in case you want to do some processing on the maze in order to
    decide the reward. 
        
    This function should return a maximim/minimum reward when the mouse eats/gets eaten respectively.      
   */

   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/ 

  int c_cheese = closest_cheese(cheeses, mouse_pos[0][0], mouse_pos[0][1]);
  int c_cat = closest_cat(cats, mouse_pos[0][0], mouse_pos[0][1]);

  int cheese_distance = distance(cheeses[c_cheese][0], cheeses[c_cheese][1], mouse_pos[0][0], mouse_pos[0][1]);
  int cat_distance = distance(cats[c_cat][0], cats[c_cat][1], mouse_pos[0][0], mouse_pos[0][1]);

  //int max_distance = distance(0, 0, size_X, graph_size / size_X);

  int positive = 0;
  int negative = 0;

  if (mouse_pos[0][0]==cats[0][0]&&mouse_pos[0][1]==cats[0][1]){    //Cat eats mouse!
    return -1000;
  }
  else if (mouse_pos[0][0]==cheeses[0][0]&&mouse_pos[0][1]==cheeses[0][1]){   //Mouse eats cheese!
    return 600;
  }

  if (cheese_distance < cat_distance){
    if (cheese_distance < 3){
      positive += 8 * (cheese_distance);
    }
    else{
      positive += 4 * (cheese_distance);
    }
  }
  else{ 
    if (cat_distance < 3){
      negative += 8 * (cat_distance);
    }
    else{
      negative += 4 * (cat_distance);
    }
  }

  return positive - negative; 
}

void feat_QLearn_update(double gr[max_graph_size][4],double weights[25], double reward, int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
    This function performs the Q-learning adjustment to all the weights associated with your
    features. Unlike standard Q-learning, you don't receive a <s,a,r,s'> tuple, instead,
    you receive the current state (mouse, cats, and cheese potisions), and the reward 
    associated with this action (this is called immediately after the mouse makes a move,
    so implicit in this is the mouse having selected some action)
    
    Your code must then evaluate the update and apply it to the weights in the weight array.    
   */
  
   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/        

   //printf("reward: %f\n", reward);

   int reward_difference = 0;

   double features[numFeatures];
   evaluateFeatures(gr, features, mouse_pos, cats, cheeses, size_X, graph_size);

   //printf("rewards is: %f\n", reward);

   for (int i = 0; i < numFeatures; i++){
    reward_difference = alpha * reward * features[i];
    weights[i] += reward_difference;
   }

   //printf("rewards: %f\n", reward);
      
}

int feat_QLearn_action(double gr[max_graph_size][4],double weights[25], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], double pct, int size_X, int graph_size)
{
  /*
    Similar to its counterpart for standard Q-learning, this function returns the index of the next
    action to be taken by the mouse.
    
    Once more, the 'pct' value controls the percent of time that the function chooses an optimal
    action given the current policy.
    
    E.g. if 'pct' is .15, then 15% of the time the function uses the current weights and chooses
    the optimal action. The remaining 85% of the time, a random action is chosen.
    
    As before, the mouse must never select an action that causes it to walk through walls or leave
    the maze.    
   */

  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/   

  int direction = rand() % 4;

  double random_choice = rand() % 100;

  int mouse_index = mouse_pos[0][0] + (mouse_pos[0][1] * size_X);

  double maxU;

  while (gr[mouse_index][direction] != 1){    //Make sure the randomly picked direction is actually a legal move
      direction = rand() % 4;
  }

  if (random_choice <= pct * 100){  //When not random we want to pick the max qsa value among possible actions and take that one
    maxQsa(gr, weights, mouse_pos, cats, cheeses, size_X, graph_size, &maxU, &direction); 
  }     

  return direction;	

}

void evaluateFeatures(double gr[max_graph_size][4],double features[25], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
   This function evaluates all the features you defined for the game configuration given by the input
   mouse, cats, and cheese positions. You are free to define up to 25 features. This function will
   evaluate each, and return all the feature values in the features[] array.
   
   Take some time to think about what features would be useful to have, the better your features, the
   smarter your mouse!
   
   Note that instead of passing down the number of cats and the number of cheese chunks (too many parms!)
   the arrays themselves will tell you what are valid cat/cheese locations.
   
   You can have up to 5 cats and up to 5 cheese chunks, and array entries for the remaining cats/cheese
   will have a value of -1 - check this when evaluating your features!
  */

   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/      
  

  //int c_cheese = closest_cheese(cheeses, mouse_pos[0][0], mouse_pos[0][1]);
  //int c_cat = closest_cat(cats, mouse_pos[0][0], mouse_pos[0][1]);

  //int cheese_distance = distance(cheeses[c_cheese][0], cheeses[c_cheese][1], mouse_pos[0][0], mouse_pos[0][1]);
  //int cat_distance = distance(cats[c_cat][0], cats[c_cat][1], mouse_pos[0][0], mouse_pos[0][1]);

  int c_cheese = closest_cheese(cheeses, mouse_pos[0][0], mouse_pos[0][1]);
  int c_cat = closest_cat(cats, mouse_pos[0][0], mouse_pos[0][1]);

  int cheese_distance = distance(cheeses[c_cheese][0], cheeses[c_cheese][1], mouse_pos[0][0], mouse_pos[0][1]);
  int cat_distance = distance(cats[c_cat][0], cats[c_cat][1], mouse_pos[0][0], mouse_pos[0][1]);

  int max_distance = distance(0, 0, size_X, graph_size / size_X);

  features[0] = cheese_distance;
  features[1] = cat_distance; 
   
}

double Qsa(double weights[25], double features[25])
{
  /*
    Compute and return the Qsa value given the input features and current weights
   */

  /***********************************************************************************************
  * TO DO: Complete this function
  ***********************************************************************************************/  
  
  double result = 0;

  for (int i = 0; i < numFeatures; i++){
    result += weights[i] * features[i];   //Add in 
  }

  return result;		// <--- stub! compute and return the Qsa value
}

void maxQsa(double gr[max_graph_size][4],double weights[25],int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size, double *maxU, int *maxA)
{
 /*
   Given the state represented by the input positions for mouse, cats, and cheese, this function evaluates
   the Q-value at all possible neighbour states and returns the max. The maximum value is returned in maxU
   and the index of the action corresponding to this value is returned in maxA.
   
   You should make sure the function does not evaluate moves that would make the mouse walk through a
   wall. 
  */
 
   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/  

  int best_qsa = 0;
  int best_qsa_index = 0;
  int current_qsa = 0;

  int new_mouse_pos[1][2];
  
  double features[numFeatures];

  int mouse_index = mouse_pos[0][0] + (mouse_pos[0][1] * size_X);

  //printf("maxQsa start!\n");

  if (gr[mouse_index][0] == 1){
    new_mouse_pos[0][0] = mouse_pos[0][0];
    new_mouse_pos[0][1] = mouse_pos[0][1] - 1;
    evaluateFeatures (gr, features, new_mouse_pos, cats, cheeses, size_X, graph_size);
    current_qsa = Qsa(weights, features);
    if (current_qsa > best_qsa){
        best_qsa = current_qsa;
        *maxA = 0;
      }
  }

  if (gr[mouse_index][1] == 1){
    new_mouse_pos[0][0] = mouse_pos[0][0] + 1;
    new_mouse_pos[0][1] = mouse_pos[0][1];
    evaluateFeatures (gr, features, new_mouse_pos, cats, cheeses, size_X, graph_size);
    current_qsa = Qsa(weights, features);
    if (current_qsa > best_qsa){
        best_qsa = current_qsa;
        *maxA = 1;
      }
  }

  if (gr[mouse_index][2] == 1){
    new_mouse_pos[0][0] = mouse_pos[0][0];
    new_mouse_pos[0][1] = mouse_pos[0][1] + 1;
    evaluateFeatures (gr, features, new_mouse_pos, cats, cheeses, size_X, graph_size);
    current_qsa = Qsa(weights, features);
    if (current_qsa > best_qsa){
        best_qsa = current_qsa
;        *maxA = 2;
      }
  }

  if (gr[mouse_index][3] == 1){
    new_mouse_pos[0][0] = mouse_pos[0][0] - 1;
    new_mouse_pos[0][1] = mouse_pos[0][1];
    evaluateFeatures (gr, features, new_mouse_pos, cats, cheeses, size_X, graph_size);
    current_qsa = Qsa(weights, features);
    if (current_qsa > best_qsa){
        best_qsa = current_qsa;
        *maxA = 3;
      }
  }

/*
  for (int i = 0; i < 4; i++){
    if (gr[mouse_index][i] == 1){
      evaluateFeatures (gr, features, mouse_pos, cats, cheeses, size_X, graph_size);
      current_qsa = Qsa(weights, features);
      if (current_qsa > best_qsa){
        best_qsa = current_qsa;
        best_qsa_index = i;
      }
    }
  }
*/

  *maxU = best_qsa;	// <--- stubs! your code will compute actual values for these two variables!
  //*maxA = best_qsa_index;

  /*
  printf("direction %d \n", best_qsa_index);

  if (gr[mouse_index][*maxA] != 1){
    printf("AAAAAAAH\n");
  }

    printf("maxqsa end! \n");
  */

  return;
   
}

/***************************************************************************************************
 *  Add any functions needed to compute your features below 
 *                 ---->  THIS BOX <-----
 * *************************************************************************************************/

double distance(int x_1, int y_1, int x_2, int y_2){ //Returns the Manhattan distance from (x_1, y_1) to (x_2, y_2)
  return (abs(x_1 - x_2) + abs(y_1 - y_2)); 
}

int closest_cheese(int cheese_pos[5][2], int node_x, int node_y){ //Finds the closest cheese to a node
  int closest_cheese; 
  int dist = distance(cheese_pos[0][0], cheese_pos[0][1], node_x, node_y);  

  for (int i = 0; i < 5; i++){
    if (distance(cheese_pos[i][0], cheese_pos[i][1], node_x, node_y) <= dist){
      closest_cheese = i;
    }
  }

  return closest_cheese;
}

int closest_cat(int cat_pos[5][2], int node_x, int node_y){ //Finds the closest cat to a node
  int closest_cat;  
  int dist = distance(cat_pos[0][0], cat_pos[0][1], node_x, node_y);  

  for (int i = 0; i < 5; i++){
    if (!(cat_pos[i][0] == 0 && cat_pos[i][1] == 0)){
      if (distance(cat_pos[i][0], cat_pos[i][1], node_x, node_y) <= dist){
        closest_cat = i;
      }
    }
  }

  return closest_cat;
}