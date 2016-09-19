/*
	CSC D84 - Unit 1 - Search

	This file contains the API function headers for your assignment.
	Please pay close attention to the function prototypes, and
	understand what the arguments are.

	Stubs for implementing each function are to be found in AI_search.c,
	along with clear ** TO DO markers to let you know where to add code.

	You are free to add helper functions within reason. But you must
	provide a prototype *in this file* as well as the implementation
	in the .c program file.

	Script by: F.J.E., Jul. 2015
*/

#ifndef __AI_search_header

#define __AI_search_header

// Generally needed includes
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<malloc.h>

#include "board_layout.h"

// Function prototypes for D84 - Unit 1 - Search assignment solution

void search(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode, int (*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4]));
int H_cost(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4]);
int H_cost_nokitty(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4]);

// If you need to add any function prototypes yourself, you can do so *below* this line.

int cat_index (int cat_loc[10][2], int cats, int path_graph_location);
int distance (int x_1, int y_1, int x_2, int y_2);
int closest_cheese (int cheese_loc[10][2], int node_x, int node_y);
int is_cat_close (int cat_loc[10][2], int cats, int x, int y);
int cat_danger (int cat_loc[10][2], int cats, int x, int y);
int how_open (int x, int y, double gr[graph_size][4]);

//Nodes and priority queue declarations
//
//
//
//
//
//Nodes and priority queue declarations

struct Node {
	int graph_coordinate;
	int node_heuristic_cost;
	struct Node *next;
	struct Node *previous;
};

struct Node *newnode(int h_cost, int graph_coordinate, struct Node *previous, struct Node *next){ //Insert a new node into our priority queue
	struct Node *new_node = (struct Node *) malloc (sizeof(struct Node));
	new_node->next = next;
	new_node->previous = previous;
	new_node->graph_coordinate = graph_coordinate;
	new_node->node_heuristic_cost = h_cost;
	return new_node;
};

struct pQueue {
	struct Node *head;
	void (*enqueue)(struct pQueue *queue, struct Node *node);
	void (*dequeue)(struct pQueue *queue);
};

void enqueue(struct pQueue *queue, struct Node *node){
	if (queue->head == NULL){
		queue->head = node;
	}

	else if (node->node_heuristic_cost <= queue->head->node_heuristic_cost){
			queue->head->previous = node;
			node->next = queue->head;
			queue->head = node;
	}

	else {
		struct Node *ptr = queue->head;
		while (ptr->next != NULL && ptr->next->node_heuristic_cost < node->node_heuristic_cost){
			ptr = ptr->next;
		}
		node->next = ptr->next;
		node->previous = ptr;
		ptr->next = node;
	}
};

void clean_up(struct Node *node){	//Clean up our entire queue to avoid memory leaks when the search is called again
	if (node->next != NULL){
		clean_up(node);
	}
	else {
		free(node);
	}
};

void dequeue(struct pQueue *queue){
	if (queue->head->next != NULL){
		struct Node *head = queue->head;
		queue->head = head->next;
		queue->head->previous = NULL;
		head->next = NULL;
		head->previous = NULL;
		clean_up(head);
	}
	else {
		clean_up(queue->head);
		queue->head = NULL;
	}
};

struct pQueue *create_pQueue(struct Node *head){
	struct pQueue *new_pQueue = (struct pQueue *) malloc(sizeof(struct pQueue));
	new_pQueue->head = head;
	new_pQueue->enqueue = &enqueue;
	new_pQueue->dequeue = &dequeue;
	return new_pQueue;
};

#endif

