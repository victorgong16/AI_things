/*
	CSC D84 - Unit 2 - MiniMax and adversarial games

	This file contains the API function headers for your assignment.
	Please pay close attention to the function prototypes, and
	understand what the arguments are.

	Stubs for implementing each function are to be found in MiniMax_search.c,
	along with clear ** TO DO markers to let you know where to add code.

	You are free to add helper functions within reason. But you must
	provide a prototype *in this file* as well as the implementation
	in the .c program file.

	Script by: F.J.E., Sep. 2015
*/

#ifndef __MiniMax_search_header

#define __MiniMax_search_header

// Generally needed includes
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<malloc.h>
#include<string.h>

#include "board_layout.h"

// Function prototypes for D84 - Unit 2 - MiniMax assignment solution
double MiniMax(double gr[graph_size][4], int path[1][2], double minmax_cost[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode, double (*utility)(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4]), int agentId, int depth, int maxDepth, double alpha, double beta);

double utility(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4]);

int checkForTerminal(int mouse_loc[1][2],int cat_loc[10][2],int cheese_loc[10][2],int cats,int cheeses);
// If you need to add any function prototypes yourself, you can do so *below* this line.

double distance(int x_1, int y_1, int x_2, int y_2);
int closest_cheese(int cheese_loc[10][2], int cheeses, int node_x, int node_y);
int closest_cat(int cat_loc[10][2], int cats, int node_x, int node_y);
int deadends (int x, int y, double gr[graph_size][4]);
int average_cat_distance(int cat_loc[10][2], int cats, int node_x, int node_y);

//Nodes and tree declarations
//
//
//
//
//
//Nodes and tree declarations

struct Node {
	int index;
	struct Node *up;
	struct Node *right;
	struct Node *down;
	struct Node *left;
};

struct Node *newnode(int i){ //Insert a new node into our priority queue
	struct Node *new_node = (struct Node *) malloc (sizeof(struct Node));
	new_node->index = i;
	new_node->up = NULL;
	new_node->right = NULL;
	new_node->down = NULL;
	new_node->left = NULL;
	return new_node;
};

struct Node deleteNode(struct Node *node){
	if (node->up != NULL){
		deleteNode(node->up);
	}
	if (node->right != NULL){
		deleteNode(node->right);
	}
	if (node->down != NULL){
		deleteNode(node->down);
	}
	if (node->left != NULL){
		deleteNode(node->left);
	}
	free(node);
};

struct minimax_tree {
	struct Node *root;
	void (*add)(struct minimax_tree *m_tree, struct Node *node);
	void (*del)(struct minimax_tree *m_tree, struct Node *node);
};

void m_tree_add (struct minimax_tree *root, struct Node *node){
	NULL;
};

void m_tree_delete (struct minimax_tree *root, struct Node *node){
	NULL;
};

struct minimax_tree *create_minimax_tree (struct Node *node){
	struct minimax_tree *new_m_tree = (struct minimax_tree *) malloc(sizeof(struct minimax_tree));
	new_m_tree->root = node;
	new_m_tree->add = &m_tree_add;
	new_m_tree->del = &m_tree_delete;
	return new_m_tree;
};

#endif

