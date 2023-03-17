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
    // s is the current state, a is the action taken, r is the reward obtained,
    // s_new is the new state, and QTable is the Q-Table
    
    // Get the current Q-Value for the (s,a) pair
    double Qsa = *(QTable + (4 * s) + a);
    
    // Calculate the maximum Q-Value for the new state
    double Qmax = *(QTable + (4 * s_new) + 0);
    for (int i = 1; i < 4; i++) {
        double Qnew = *(QTable + (4 * s_new) + i);
        if (Qnew > Qmax) {
            Qmax = Qnew;
        }
    }
    
    // Calculate the new Q-Value for the (s,a) pair
    double Qnew = Qsa + alpha * (r + lambda * (Qmax - Qsa));
    
    // Update the Q-Table
    *(QTable + (4 * s) + a) = Qnew;
  
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
         
     state = (i+(j*size_X)) + ((k+(l*size_X))*graph_size) + ((m+(n*size_X))*graph_size*graph_size)
     ** Make sure you undestand the state encoding above!
     
     Entries in the Q-table for this state are

     *(QTable+(4*state)+a)      <-- here a is the action in [0,3]
     
     (yes, it's a linear array, no shorcuts with brackets!)
     
     NOTE: There is only one cat and once cheese, so you only need to use cats[0][:] and cheeses[0][:]
   */
  
  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/  

  // get the current state of the game
    int i = mouse_pos[0][0];
    int j = mouse_pos[0][1];
    int k = cats[0][0];
    int l = cats[0][1];
    int m = cheeses[0][0];
    int n = cheeses[0][1];
    int state = (i + j * size_X) + ((k + l * size_X) * graph_size) + ((m + n * size_X) * graph_size * graph_size);
    int index = i + (j * size_X);
    // use the Q-table pct percent of the time, choose randomly otherwise
    int action;
    double c = (double)rand() / (double)((unsigned)RAND_MAX + 1);
    
    if (c <= pct) {
        // choose action with the highest Q-value for the current state
        double maxQ = *(QTable + (4 * state));
        action = 0;
        for (int a = 1; a < 4; a++) {
            double Qval = *(QTable + (4 * state) + a);
            if ((Qval >= maxQ) && (gr[index][a] != 0) || (gr[index][action] == 0)) {
                maxQ = Qval;
                action = a;
            }
        }
    } else {
        // choose a random action
        int possible_actions[4];
        int num_actions = 0;
        for (int a = 0; a < 4; a++) {
            if (gr[index][a] != 0) {
                possible_actions[num_actions] = a;
                num_actions++;
            }
        }
        int rand_idx = rand() % num_actions;
        action = possible_actions[rand_idx];
    }

    // update mouse position based on the chosen action
    int new_i = i;
    int new_j = j;
    if (action == 0 && gr[index][0] != 0) {
        new_j = j - 1;
    } else if (action == 1 && gr[index][1] != 0) {
        new_i = i + 1;
    } else if (action == 2 && gr[index][2] != 0) {
        new_j = j + 1;
    } else if (action == 3 && gr[index][3] != 0) {
        new_i = i - 1;
    }

    // check if the new position is valid (i.e., not a wall)
    if (gr[index][action] == 0) {
        printf("Warning: mouse crossed a wall! Action: %d, index: %d\n", action, index);
        printf("QTable val: %f\n", *(QTable + (4 * state) + action));
    } else if (new_i < 0 || new_i >= size_X || new_j < 0 || new_j >= size_X) {
        printf("Warning: mouse left the map! Action: %d, index: %d\n", action, index);
        printf("QTable val: %f\n", *(QTable + (4 * state) + action));
    }

    int new_s = (new_i + new_j * size_X) + ((k + l * size_X) * graph_size) + ((m + n * size_X) * graph_size * graph_size);
    int new_mouse_pos[1][2];
    new_mouse_pos[0][0] = new_i;
    new_mouse_pos[0][1] = new_j;
    
    double reward = QLearn_reward(gr, new_mouse_pos, cats, cheeses, size_X, graph_size);
    QLearn_update(state, action, reward, new_s, QTable);
    
    return action;
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

  /* The reward func below converges to 0.5-0.6 success rate from 20-50 rounds
  int min_cat_dist = 10000;
  for(int i = 0; i < 1; i++){
    int mouse_dist = abs(cats[i][0]-mouse_pos[0][0]) + abs(cats[i][1]-mouse_pos[0][1]);
    if (mouse_dist < min_cat_dist){
      min_cat_dist = mouse_dist;
    }
  }
  if (min_cat_dist == 0){
      min_cat_dist -= 8 * size_X; 
  }
  int min_cheese_dist = 1000;
  for(int i = 0; i < 1; i++){
    int mouse_dist = abs(cheeses[i][0]-mouse_pos[0][0]) + abs(cheeses[i][1]-mouse_pos[0][1]);
    if (mouse_dist < min_cheese_dist){
      min_cheese_dist = mouse_dist;
    }
  }
  //if (min_cheese_dist == 0){min_cheese_dist += size_X/2;}
  int reward = 2*size_X - min_cheese_dist + min_cat_dist;
  return(reward);			// <--- of course, you will change this as well!   
  */  
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

  return(0);		// <--- replace this while you're at it!

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
   
}

double Qsa(double weights[25], double features[25])
{
  /*
    Compute and return the Qsa value given the input features and current weights
   */

  /***********************************************************************************************
  * TO DO: Complete this function
  ***********************************************************************************************/  
  
  return(0);		// <--- stub! compute and return the Qsa value
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
 
  *maxU=0;	// <--- stubs! your code will compute actual values for these two variables!
  *maxA=0;
  return;
   
}

/***************************************************************************************************
 *  Add any functions needed to compute your features below 
 *                 ---->  THIS BOX <-----
 * *************************************************************************************************/
