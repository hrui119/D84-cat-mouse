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
#include <stdio.h>
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
  
  //Q(s,a) += alpha (reward + gamma(Q(new state, max action of new state) - Q(s,a)))
  double max_next_s = -1;
  for (int i = 0; i < 4; i++){
      if(max_next_s < *(QTable+(4*s_new) + i)){
        max_next_s =  *(QTable+(4*s_new) + i);
      } 
  }
  *(QTable+(4*s)+a) = alpha*(r + lambda * (max_next_s-*(QTable+(4*s)+a)));
  return;
  
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
  //Q_table initialized to all 0s
  int states = (size_X+(size_X*size_X)) + ((size_X+(size_X*size_X))*graph_size) + ((size_X+(size_X*size_X))*graph_size*graph_size);
  int pi[states];
  //initialize all pi(s) to be first valid move
  for (int x = 0; x < states; x++){
    int ind = (get_x(x, graph_size, size_X) + (get_y(x, graph_size, size_X) * graph_size)); 
    if(gr[ind][0] != 0){
      pi[x] = 0;
    } 
    else if(gr[ind][1] != 0){
      pi[x] = 1;
    } 
    else if(gr[ind][2] != 0){
      pi[x] = 2;
    } 
    else if(gr[ind][3] != 0){
      pi[x] = 3;
    } 
  }
  int init_s = (mouse_pos[0][0]+(mouse_pos[0][1]*size_X)) + ((cats[0][0]+(cats[0][1]*size_X))*graph_size) + ((cheeses[0][0]+(cheeses[0][1]*size_X))*graph_size*graph_size);
  // alpha defined in QLearn.h

  int k = 100;
  int a = 0;
  int M = 10;
  for (int j = 1;  j <= k; j++){
    double p_rand = 1-(j/k);
    for (int i = 1; i <= M; i++){
      double c = (double)rand() / (double)((unsigned)RAND_MAX + 1);

      if (c <= pct){
        int random_act = rand()%4;
        int curr_index = mouse_pos[0][1] + (mouse_pos[0][1] * size_X);
        while(gr[curr_index][random_act] !=1){
          random_act = rand()%4;
          curr_index = mouse_pos[0][1] + (mouse_pos[0][1] * size_X);
        }
        a = random_act;
      }
      else{
        a = pi[init_s];
      }
      double reward = QLearn_reward(gr, mouse_pos, cats, cheeses, size_X, graph_size);
      int next_s = -1;
      //we have experience tuple = (s, a, r, next s) from the above
      if(a == 0){
        int next_s = (mouse_pos[0][0]+((mouse_pos[0][1]-1)*size_X)) + ((cats[0][0]+(cats[0][1]*size_X))*graph_size) + ((cheeses[0][0]+(cheeses[0][1]*size_X))*graph_size*graph_size);
      }
      else if(a == 1){
        int next_s = (mouse_pos[0][0]+1 +((mouse_pos[0][1])*size_X)) + ((cats[0][0]+(cats[0][1]*size_X))*graph_size) + ((cheeses[0][0]+(cheeses[0][1]*size_X))*graph_size*graph_size);
      }
      else if(a == 2){
        int next_s = (mouse_pos[0][0]+((mouse_pos[0][1]+1)*size_X)) + ((cats[0][0]+(cats[0][1]*size_X))*graph_size) + ((cheeses[0][0]+(cheeses[0][1]*size_X))*graph_size*graph_size);
      }
      else if(a == 3){
        int next_s = (mouse_pos[0][0]-1+((mouse_pos[0][1])*size_X)) + ((cats[0][0]+(cats[0][1]*size_X))*graph_size) + ((cheeses[0][0]+(cheeses[0][1]*size_X))*graph_size*graph_size);
      }
      //call QLearn_update to update Q(s,a)
      QLearn_update(init_s, a, reward, next_s, QTable);
    }
    int curr_largest = 0;
    int largest_i = -1;
    for(int y = 0; y <= 3; y++){
      if(curr_largest < *(QTable+(4*init_s)+y)){
        largest_i = y;
        curr_largest = *(QTable+(4*init_s)+y);
      }
    }
    pi[init_s] = largest_i;
  }
  return(pi[init_s]);		// <--- of course, you will change this!
  
}
int get_x(int s, int graph_size, int size_X){
  int mn = floor(s/(graph_size*graph_size));
  int kl = floor((s-(mn*graph_size*graph_size))/graph_size);
  int ij = s-(mn*graph_size*graph_size)-(kl*graph_size);
  int i = ij%size_X;
  return i;
}
int get_y(int s, int graph_size, int size_X){
  int mn = floor(s/(graph_size*graph_size));
  int kl = floor((s-(mn*graph_size*graph_size))/graph_size);
  int ij = s-(mn*graph_size*graph_size)-(kl*graph_size);
  int i = ij%size_X;
  int j = (ij-i)/size_X;
  return j;
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
  int min_cat_dist = 10000;
  for(int i = 0; i < 1; i++){
    int mouse_dist = abs(cats[i][0]-mouse_pos[0][0]) + abs(cats[i][1]-mouse_pos[0][1]);
    if (mouse_dist < min_cat_dist){
      min_cat_dist = mouse_dist;
    }
  }
  if (min_cat_dist == 0){
    return -100; 
  }
  int min_cheese_dist = 1000;
  for(int i = 0; i < 1; i++){
    int mouse_dist = abs(cheeses[i][0]-mouse_pos[0][0]) + abs(cheeses[i][1]-mouse_pos[0][1]);
    if (mouse_dist < min_cheese_dist){
      min_cheese_dist = mouse_dist;
    }
  }
  if (min_cheese_dist == 0){
    return 100;
  }
  int reward = 2*size_X - min_cheese_dist + min_cat_dist;
  return(reward);		// <--- of course, you will change this as well!     
}


void feat_QLearn_update(double gr[max_graph_size][4],double weights[25], double reward, int mouse_pos[1][2], int new_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
    This function performs the Q-learning adjustment to all the weights associated with your
    features. Unlike standard Q-learning, you don't receive a <s,a,r,s'> tuple, instead,
    you receive the current state (mouse, cats, and cheese potisions), and the reward 
    associated with this action (this is called immediately after the mouse makes a move,
    so implicit in this is the mouse having selected some action)
    
    Your code must then evaluate the update and apply it to the weights in the weight array.
   */
  for (int i = 0; i < numFeatures; i++) {
    double feat_orig[25], feat_after[25];
    evaluateFeatures(gr, feat_orig, mouse_pos, cats, cheeses, size_X, graph_size);
    evaluateFeatures(gr, feat_after, new_pos, cats, cheeses, size_X, graph_size);
    double qs = Qsa(weights, feat_orig);
    double qs_aft = Qsa(weights, feat_after);
    weights[i] += alpha * (reward + (lambda * qs_aft) - qs) * feat_orig[i];
  }
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
  srand((unsigned) time(NULL));
  double w;
  int K = 100;
  double features[25];
  // init variables
  evaluateFeatures(gr, features, mouse_pos, cats, cheeses, size_X, graph_size);
  for (int i = 0; i < numFeatures; i++) {
    w = ((double) (rand()%(100+1-0)-50))/100;
    while (w == 0) {
      w = ((double) (rand()%(100+1-0)-50))/100;
    }
    weights[i] = w;
  }
  // actual refining
  for (int j = 0; j < K; j++) {
    //double p_random = 1 / (j / K);
    for (int i = 0; i < 4; i++) {
      double c = ((double) rand()) / (double)((unsigned) RAND_MAX + 1);
      int curr_index = mouse_pos[0][0] + (mouse_pos[0][1] * size_X);
      int a;
      double maxU;
      if (c > pct) {
        a = (rand()%(4));
        while (gr[curr_index][a] != 1) {
          a = (rand()%(4));
        }
      } else {
        // optimal action
        maxQsa(gr, weights, mouse_pos, cats, cheeses, size_X, graph_size, &maxU, &a);
      }
      int new_pos[1][2];
      if (a == 0) {
        new_pos[0][0] = mouse_pos[0][0];
        new_pos[0][1] = mouse_pos[0][1] - 1;
        evaluateFeatures(gr, features, new_pos, cats, cheeses, size_X, graph_size);
      } else if (a == 1) {
        new_pos[0][0] = mouse_pos[0][0] + 1;
        new_pos[0][1] = mouse_pos[0][1];
        evaluateFeatures(gr, features, new_pos, cats, cheeses, size_X, graph_size);
      } else if (a == 2) {
        new_pos[0][0] = mouse_pos[0][0];
        new_pos[0][1] = mouse_pos[0][1] + 1;
        evaluateFeatures(gr, features, new_pos, cats, cheeses, size_X, graph_size);
      } else if (a == 3) {
        new_pos[0][0] = mouse_pos[0][0] - 1;
        new_pos[0][1] = mouse_pos[0][1];
        evaluateFeatures(gr, features, new_pos, cats, cheeses, size_X, graph_size);
      }
      double reward = QLearn_reward(gr, new_pos, cats, cheeses, size_X, graph_size);
      feat_QLearn_update(gr, weights, reward, mouse_pos, new_pos, cats, cheeses, size_X, graph_size);
    }
  }
  double maxU;
  int maxA;
  maxQsa(gr, weights, mouse_pos, cats, cheeses, size_X, graph_size, &maxU, &maxA);
  return maxA;

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

   m distance to cat descending
   m distance to cheese descending
  */
  int dist[5];
  // cat dist descending
  for (int i = 0; i < 5; i++) {
    if (cats[i][0] != -1) {
      int d = abs(mouse_pos[0][0] - cats[i][0]) + abs(mouse_pos[0][1] - cats[i][1]);
      if (i == 0) {
        dist[0] = d;
      } else {
        // insert
        int temp = d, temp2;
        for (int j = 0; j < i; j++) {
          if (temp > dist[j]) {
            temp2 = dist[j];
            dist[j] = temp;
            temp = dist[j+1];
            dist[j+1] = temp2;
          }
        }
      }
    } else {
      dist[i] = 0;
    }
  }
  for (int i = 0; i < 5; i++) {
    features[i] = dist[i];
  }
  // cheese dist desc
  for (int i = 0; i < 5; i++) {
    if (cheeses[i][0] != -1) {
      int d = abs(mouse_pos[0][0] - cheeses[i][0]) + abs(mouse_pos[0][1] - cheeses[i][1]);
      if (i == 0) {
        dist[0] = d;
      } else {
        // insert
        int temp = d, temp2;
        for (int j = 0; j < i; j++) {
          if (temp > dist[j]) {
            temp2 = dist[j];
            dist[j] = temp;
            temp = dist[j+1];
            dist[j+1] = temp2;
          }
        }
      }
    } else {
      dist[i] = 0;
    }
  }
  for (int i = 0; i < 5; i++) {
    features[i+5] = dist[i];
  }
}

double Qsa(double weights[25], double features[25])
{
  /*
    Compute and return the Qsa value given the input features and current weights
   */
  double result = 0;
  for (int i = 0; i < numFeatures; i++) {
    result += weights[i] * features[i];
  }
  return result;
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

  *maxU=0;
  *maxA=0;
  double features[25];
  int curr_index = mouse_pos[0][0] + (mouse_pos[0][1] * size_X);
  double max = -100000000000000000;
  int new_pos[1][2];
  for (int k = 0; k < 4; k++) {
    if (k == 0) {
      new_pos[0][0] = mouse_pos[0][0];
      new_pos[0][1] = mouse_pos[0][1] - 1;
      evaluateFeatures(gr, features, new_pos, cats, cheeses, size_X, graph_size);
    } else if (k == 1) {
      new_pos[0][0] = mouse_pos[0][0] + 1;
      new_pos[0][1] = mouse_pos[0][1];
      evaluateFeatures(gr, features, new_pos, cats, cheeses, size_X, graph_size);
    } else if (k == 2) {
      new_pos[0][0] = mouse_pos[0][0];
      new_pos[0][1] = mouse_pos[0][1] + 1;
      evaluateFeatures(gr, features, new_pos, cats, cheeses, size_X, graph_size);
    } else if (k == 3) {
      new_pos[0][0] = mouse_pos[0][0] - 1;
      new_pos[0][1] = mouse_pos[0][1];
      evaluateFeatures(gr, features, new_pos, cats, cheeses, size_X, graph_size);
    }
    if (gr[curr_index][k] == 1) {
      double val = Qsa(weights, features);
      if (val > max) {
        *maxA = k;
        *maxU = val;
      }
    }
  }
  return;
   
}
