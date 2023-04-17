#include <stdio.h>
#include <stdlib.h>

int HEIGHT = 0;                // Height of the 2-3 tree

typedef struct Node 
{
	int values_num;            // number of values(1 or 2) in the node;
	int values[2];             // values in the node (in the values_num = 2 case: values[0] < values[1]);
	struct Node* children[3];  // children of the node;
} Node;
typedef struct path_and_length {
	Node** path;        // path of nodes, starting from the root; it's an array of pointers to the node
	int len;            // length of the path (number of nodes in the 'path')
} path_and_len; // we need this structure to access the parent of the node.

void swap(int *xp, int *yp) 
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}


Node* create_node(int val) 
{
	Node* new_node = malloc(sizeof(struct Node));
	new_node->values_num = 1;
	new_node->values[0] = val;
	new_node->children[0] = NULL;
	new_node->children[1] = NULL;
	new_node->children[2] = NULL;

	return new_node;
}

Node* search(Node* node, int val) 
{// node != NULL
	if (node->children[0] == NULL) return node; //we are in the leaf node

	if (node->values_num == 1) 
	{
		if (val == node->values[0]) {return node;} 
		else 
		if (val < node->values[0]) {return search(node->children[0], val);}
		else 
		if (val > node->values[0]) {return search(node->children[1], val);}
	} else 
	if (node->values_num == 2) 
	{
		if ((val == node->values[0]) || (val == node->values[1])) {return node;} 
		else 
		if (val < node->values[0]) {return search(node->children[0], val);} 
		else 
		if ((node->values[0] < val) && (val < node->values[1])) {return search(node->children[1], val);}
		else 
		if (val > node->values[1]) {return search(node->children[2], val);}
	}
}
path_and_len* search_with_ancestors(path_and_len* path_of_nodes, int val) 
{
	Node* current_node = path_of_nodes->path[path_of_nodes->len-1];

	if (current_node->children[0] == NULL) return path_of_nodes; //current_note is the leaf node

	if (current_node->values_num == 1) 
	{
		if (val == current_node->values[0]) 
		{
			return path_of_nodes;
		} else 
		if (val < current_node->values[0]) 
		{
			path_of_nodes->path[path_of_nodes->len] = current_node->children[0];
			path_of_nodes->len += 1;
			return search_with_ancestors(path_of_nodes, val);
		} else 
		if (val > current_node->values[0]) 
		{
			path_of_nodes->path[path_of_nodes->len] = current_node->children[1];
			path_of_nodes->len += 1;
			return search_with_ancestors(path_of_nodes, val);
		}
	} 
	else 
	if (current_node->values_num == 2) 
	{
		if ((val == current_node->values[0]) || (val == current_node->values[1])) 
		{
			return path_of_nodes;
		} else 
		if (val < current_node->values[0]) 
		{
			path_of_nodes->path[path_of_nodes->len] = current_node->children[0];
			path_of_nodes->len += 1;
			return search_with_ancestors(path_of_nodes, val);
		} else 
		if ((current_node->values[0] < val) && (val < current_node->values[1])) 
		{
			path_of_nodes->path[path_of_nodes->len] = current_node->children[1];
			path_of_nodes->len += 1;
			return search_with_ancestors(path_of_nodes, val);;
		} else 
		if (val > current_node->values[1]) 
		{
			path_of_nodes->path[path_of_nodes->len] = current_node->children[2];
			path_of_nodes->len += 1;
			return search_with_ancestors(path_of_nodes, val);
		}
	}
}

Node* promote(path_and_len* path_of_nodes, Node* lower_node_1, Node* lower_node_2, int val) 
{// lower_node_1 - the left child-node which we need to add to the current node, and the lower_node_2 - is the right one;
 // 'promote' changes the current tree, and returns a root of the new changed tree;	
	if (path_of_nodes->len == 0) 
	{// we reached the root-node, so we need to create a new root
		Node* new_root = create_node(val);
		new_root->children[0] = lower_node_1;
		new_root->children[1] = lower_node_2;

		HEIGHT += 1;
		return new_root;
	}
	
	Node* current_node = path_of_nodes->path[path_of_nodes->len - 1];

	// if current_node has only 1 value:
	//         cur_node
	//        /       \ 
	//  child_0      child_1
	if (current_node->values_num == 1) 
	{	
		current_node->values_num = 2;

		// 1. val is on the left:
		//
		//             (val, cur_node)   
		//            /    |         \ 
		// lower_node_1 lower_node_2 child_1  
		if (val < current_node->values[0]) 
		{
			current_node->values[1] = current_node->values[0];
			current_node->values[0] = val;
			
			current_node->children[2] = current_node->children[1];
			current_node->children[0] = lower_node_1;
			current_node->children[1] = lower_node_2;
		} else 
		// 2. val is on the right:
		//
		//         (cur_node, val)   
		//        /         |    \ 
		//  child_0 lower_node_1 lower_node_2
		if (current_node->values[0] < val)
		{
			current_node->values[1] = val;

			current_node->children[1] = lower_node_1;
			current_node->children[2] = lower_node_2;
		}
		
		return path_of_nodes->path[0];
	}


    // if current_node has 2 values:
	//         cur_node
	//       /    |     \ 
	// child_0  child_1 child_2
	if (current_node->values_num == 2) 
	{
		// 1. val is on the left:
		//                  (promote cur_node[0])
		//             val       cur_node[0]     cur_node[1]    
		//            /  \                        /   \ 
		// lower_node_1  lower_node_2       child_1  child_2    
		if (val < current_node->values[0]) 
		{
			int median = current_node->values[0];
			Node* node_to_add = create_node(val);
			node_to_add->children[0] = lower_node_1;
			node_to_add->children[1] = lower_node_2;

			current_node->values_num = 1;
			current_node->values[0] = current_node->values[1];
			current_node->children[0] = current_node->children[1];
			current_node->children[1] = current_node->children[2];

			path_of_nodes->len -= 1;
			return promote(path_of_nodes, node_to_add, current_node, median);
		} else
		// 2. val is in the middle:
		//                 (promote val)
		//   cur_node[0]       val         cur_node[1]    
		//      /   \                        /    \ 
		// child_0  lower_node_1    lower_node_2  child_2
		if ((current_node->values[0] < val) && (val < current_node->values[1])) {
			int median = val;
			Node* node_to_add = create_node(current_node->values[1]);
			node_to_add->children[0] = lower_node_2;
			node_to_add->children[1] = current_node->children[2];

			current_node->values_num = 1;
			current_node->children[1] = lower_node_1;

			path_of_nodes->len -= 1;
			return promote(path_of_nodes, current_node, node_to_add, median);
		} else 
		// 3. val is on the right:
		//             (promote cur_node[1])
		//    cur_node[0]     cur_node[1]      val
		//       /  \                         /   \ 
		// child_0  child_1         lower_node_1  lower_node_2
		if (val > current_node->values[1]) {
			int median = current_node->values[1];
			Node* node_to_add = create_node(val);
			node_to_add->children[0] = lower_node_1;
			node_to_add->children[1] = lower_node_2;

			current_node->values_num = 1;
			
			path_of_nodes->len -= 1;
			return promote(path_of_nodes, current_node, node_to_add, median);
		}

	}	
}

void free_path(path_and_len* path_of_nodes) 
{
	free(path_of_nodes->path);
	free(path_of_nodes);
}
Node* insert(Node* root, int val) 
{//Returns the root of the tree after inserting the val
	//path from root to leaf node in which we need to insert:
	path_and_len* path_of_nodes = malloc(sizeof(path_and_len));
	path_of_nodes->path = malloc(HEIGHT*sizeof(Node*));
	path_of_nodes->path[0] = root;
	path_of_nodes->len = 1;
	path_of_nodes = search_with_ancestors(path_of_nodes, val);

	Node* last_node = path_of_nodes->path[path_of_nodes->len - 1];

	// Check if the val already exist in the tree
	if (last_node->values[0] == val) {free_path(path_of_nodes);return root;}
	if (last_node->values_num == 2) 
	{
		if (last_node->values[1] == val) {free_path(path_of_nodes);return root;}
	}

	// now 'last_node' - is the leaf node

	if (last_node->values_num == 1) {
		last_node->values_num = 2;
		last_node->values[1] = val;
		if (last_node->values[0] > last_node->values[1]) swap(&(last_node->values[0]), &(last_node->values[1]));

		free_path(path_of_nodes);
		return root;
	}

	// 'last_node' already had 2 values, 
	//      so we need to promote median value to the parent node
	if (val < last_node->values[0]) {swap(&val, &(last_node->values[0]));}  // 
	else																    // making 'val' the median
	if (val > last_node->values[1]) {swap(&val, &(last_node->values[1]));}  // 

	Node* node_to_add = create_node(last_node->values[1]);
	last_node->values_num = 1;
	path_of_nodes->len -= 1;
	Node* new_root = promote(path_of_nodes, last_node, node_to_add, val);

	//clear the path_of_nodes
	free_path(path_of_nodes);

	return new_root;
}

void delete_value(path_and_len* path, int val) 
{//we need to replace the val with its in-order successor(it's in the leaf node)
 //  and then remove the value from that leaf node.
    
	Node* current_node = path->path[path->len - 1];
	if (current_node->children[0] == NULL) 
	{//leaf node case
		if (current_node->values_num == 2) {
			current_node->values_num = 1;
			if (current_node->values[0] != val) {current_node->values[0] = current_node->values[1];}
		}
		else 
		{
			current_node->values_num = 0;
		}
		return;
	}
	else 
	{//Internal node case
	//Let's just search for the 'val' in the right subtree.
		if (current_node->values[0] == val) 
		{
			path->len += 1;
			path->path[path->len - 1] = current_node->children[1];
			path = search_with_ancestors(path, val);

			Node* leaf_node = path->path[path->len - 1]; //in-order successor of 'val'
			swap(&(current_node->values[0]), &(leaf_node->values[0]));
			leaf_node->values[0] = leaf_node->values[1];
			leaf_node->values_num -= 1;
			return;
		} 
		else 
        if (current_node->values[1] == val)
		{
			path->len += 1;
			path->path[path->len - 1] = current_node->children[2];
			path = search_with_ancestors(path, val);

			Node* leaf_node = path->path[path->len - 1]; //in-order successor of 'val'
			swap(&(current_node->values[1]), &(leaf_node->values[0]));
			leaf_node->values[0] = leaf_node->values[1];
			leaf_node->values_num -= 1;
			return;
		}
	}
}
Node* merge(path_and_len* path) 
{//The current node in the path(the last node) is empty, we need to merge it.
 //Returns the root of the modified tree.

	if (path->len == 1) 
	{//root case, Figure 13.19e from the 'mini-presentation.pptx'.
		Node* new_root = path->path[0]->children[0]; //change the root to a child of previous root
		free(path->path[0]); //clear the previous empty root
		HEIGHT -= 1;

		return new_root; 
	}

	Node* current_node = path->path[path->len - 1];
	Node* parent_node = path->path[path->len - 2];
	if (parent_node->values_num == 1) {
		if (parent_node->children[0]->values_num == 1) 
		{//Figure 13.19d from the 'mini-presentation.pptx'.
			//Make the left sibling of the current_node the node with 2 values
			parent_node->children[0]->values[1] = parent_node->values[0];
			parent_node->children[0]->children[2] = current_node->children[0];
			parent_node->children[0]->values_num = 2;

			parent_node->values_num = 0;     //make the parent node empty
			parent_node->children[1] = NULL;
			free(current_node); //remove the current_node from the tree

			path->len -= 1;
			return merge(path);
		}
		else 
	    if (parent_node->children[0]->values_num == 2) 
		{//Figure 13.19c from the 'mini-presentation.pptx'.
			current_node->values[0] = parent_node->values[0];
			current_node->children[1] = current_node->children[0];
			current_node->children[0] = parent_node->children[0]->children[2];
			current_node->values_num = 1;

			parent_node->values[0] = parent_node->children[0]->values[1];

			parent_node->children[0]->values_num = 1; //make a left_sibling a node with 1 value

			return path->path[0];	//since we don't have an empty node anymore.
		}
		else
		if (parent_node->children[1]->values_num == 1) 
		{//Similar to Figure 13.19d from the 'mini-presentation.pptx'.
			//Make the right sibling of the current_node the node with 2 values
			parent_node->children[1]->values[1] = parent_node->children[1]->values[0];
			parent_node->children[1]->values[0] = parent_node->values[0];

			parent_node->children[1]->children[2] = parent_node->children[1]->children[1];
			parent_node->children[1]->children[1] = parent_node->children[1]->children[0];
			parent_node->children[1]->children[0] = current_node->children[0];
			parent_node->children[1]->values_num = 2;

			parent_node->values_num = 0;     //make the parent node empty
			parent_node->children[0] = parent_node->children[1];
			free(current_node); //remove the current_node from the tree

			path->len -= 1;
			return merge(path);
		}
		else 
		if (parent_node->children[1]->values_num == 2) 
		{//Similar to Figure 13.19c from the 'mini-presentation.pptx'.
			current_node->values[0] = parent_node->values[0];
			current_node->children[1] = parent_node->children[1]->children[0];
			current_node->values_num = 1;

			parent_node->values[0] = parent_node->children[1]->values[0];

			parent_node->children[1]->children[0] = parent_node->children[1]->children[1];
			parent_node->children[1]->children[1] = parent_node->children[1]->children[2];
			parent_node->children[1]->values_num = 1; //make a right_sibling a node with 1 value

			return path->path[0];	//since we don't have an empty node anymore.
		}
	}
	else 
    if ((parent_node->values_num == 2) && (parent_node->children[2]->values_num == 0))
	{//the current empty node is the rightmost children of the parent node.
		if (parent_node->children[1]->values_num == 1) 
		{//Similar to Figure 13.19d from the 'mini-presentation.pptx'.
			//Make the middle child of the parent_node the node with 2 values
			parent_node->children[1]->values[1] = parent_node->values[1];
			parent_node->children[1]->children[2] = current_node->children[0];
			parent_node->children[1]->values_num = 2;

			parent_node->values_num = 1;     //make the parent node a node with 1 value

			free(current_node); //remove the current_node from the tree

			return path->path[0]; //since we don't have an empty node anymore.
		}
		else 
	    if (parent_node->children[1]->values_num == 2) 
		{//Similar to Figure 13.19c from the 'mini-presentation.pptx'.
			current_node->values[0] = parent_node->values[1];
			current_node->children[1] = current_node->children[0];
			current_node->children[0] = parent_node->children[1]->children[2];
			current_node->values_num = 1;

			parent_node->values[1] = parent_node->children[1]->values[1];

			parent_node->children[1]->values_num = 1; //make a middle child of the parent node a node with 1 value

			return path->path[0];	//since we don't have an empty node anymore.
		}
	} 
	else 
    if ((parent_node->values_num == 2) && (parent_node->children[1]->values_num == 0)) 
	{//the current empty node is the middle children of the parent node.
		if (parent_node->children[0]->values_num == 1) 
		{//Similar to Figure 13.19d from the 'mini-presentation.pptx'.
			//Make the leftmost child of the parent_node the node with 2 values
			parent_node->children[0]->values[1] = parent_node->values[0];
			parent_node->children[0]->children[2] = current_node->children[0];
			parent_node->children[0]->values_num = 2;

			parent_node->values[0] = parent_node->values[1];      //
			parent_node->children[1] = parent_node->children[2];  //
			parent_node->values_num = 1;                          // make the parent node a node with 1 value
 
			free(current_node); //remove the current_node from the tree

			return path->path[0]; //since we don't have an empty node anymore.
		}
		else 
	    if (parent_node->children[0]->values_num == 2) 
		{//Similar to Figure 13.19c from the 'mini-presentation.pptx'.
			current_node->values[0] = parent_node->values[0];
			current_node->children[1] = current_node->children[0];
			current_node->children[0] = parent_node->children[0]->children[2];
			current_node->values_num = 1;

			parent_node->values[0] = parent_node->children[0]->values[1];

			parent_node->children[0]->values_num = 1; //make a leftmost child of the parent node a node with 1 value

			return path->path[0];	//since we don't have an empty node anymore.
		}
	}
	else
	if ((parent_node->values_num == 2) && (parent_node->children[0]->values_num == 0)) 
	{//the current empty node is the left children of the parent node.
		if (parent_node->children[1]->values_num == 1) 
		{//Similar to Figure 13.19d from the 'mini-presentation.pptx'.
			//Make the middle child of the parent_node the node with 2 values
			parent_node->children[1]->values[1] = parent_node->children[1]->values[0];
			parent_node->children[1]->values[0] = parent_node->values[0];

			parent_node->children[1]->children[2] = parent_node->children[1]->children[1];
			parent_node->children[1]->children[1] = parent_node->children[1]->children[0];
			parent_node->children[1]->children[0] = current_node->children[0];
			parent_node->children[1]->values_num = 2;

			parent_node->values[0] = parent_node->values[1];      //
			parent_node->children[0] = parent_node->children[1];  //
			parent_node->children[1] = parent_node->children[2];  //
			parent_node->values_num = 1;                          // make the parent node a node with 1 value
 
			free(current_node); //remove the current_node from the tree

			return path->path[0]; //since we don't have an empty node anymore.
		}
		else 
	    if (parent_node->children[1]->values_num == 2) 
		{//Similar to Figure 13.19c from the 'mini-presentation.pptx'.
			current_node->values[0] = parent_node->values[0];
			current_node->children[1] = parent_node->children[1]->children[0];
			current_node->values_num = 1;

			parent_node->values[0] = parent_node->children[1]->values[0];

			parent_node->children[1]->values[0] = parent_node->children[1]->values[1];
			parent_node->children[1]->children[0] = parent_node->children[1]->children[1];
			parent_node->children[1]->children[1] = parent_node->children[1]->children[2]; 
			parent_node->children[1]->values_num = 1; //make a middle child of the parent node a node with 1 value

			return path->path[0];	//since we don't have an empty node anymore.
		}
	}
	

}
Node* delete_node(Node* root, int val) 
{//returns the root of the tree after deleting the val
	//path from root to the node which we need to delete:
	path_and_len* path = malloc(sizeof(path_and_len));
	path->path = malloc(HEIGHT*sizeof(Node*));
	path->path[0] = root;
	path->len = 1;
	path = search_with_ancestors(path, val);

	Node* last_node = path->path[path->len - 1];

	// Check if the 'val' exist in the tree
	if ((last_node->values[0] == val) || ((last_node->values_num == 2) && (last_node->values[1] == val))) 
	{
		delete_value(path, val);
		if (path->path[path->len - 1]->values_num == 0) 
		{//check if we have an empty node to merge
			root = merge(path);
		}
	}

	free_path(path);
	return root;
}



//==================================
// display the 2-3 tree in the console
void print_node(Node* node, int tab_count) 
{	
	for (int i = 0; i < tab_count; ++i) {printf("      ");}
	if (node->values_num == 1) 
	{
		printf("(%d)\n", node->values[0]);

		if (node->children[0] == NULL) return;
		print_node(node->children[1], tab_count + 1);
		print_node(node->children[0], tab_count + 1);
	}
	if (node->values_num == 2) 
	{
		printf("(%d, %d)\n", node->values[0], node->values[1]);

		if (node->children[0] == NULL) return;
		print_node(node->children[2], tab_count + 1);
		print_node(node->children[1], tab_count + 1);
		print_node(node->children[0], tab_count + 1);
	}
}
void display_tree(Node* root) 
{
	printf("Height: %d\n\n", HEIGHT);
	
    print_node(root, 0);
	printf("\n");
}
//==================================

void free_memory(Node* node) {
	if (node->values_num >= 1) {
		if (node->children[0] != NULL) free_memory(node->children[0]);
		if (node->children[1] != NULL) free_memory(node->children[1]);
	}
    if (node->values_num == 2) {
		if (node->children[2] != NULL) free_memory(node->children[2]);
	}

	free(node);
}

int main() 
{
	Node* root;
	root = create_node(9); HEIGHT = 1;
	root = insert(root, 5);
	root = insert(root, 8);
	root = insert(root, 3);
	root = insert(root, 2);
	root = insert(root, 4);
	root = insert(root, 7);
	printf("Tree after inserting the nodes: \n");
	display_tree(root);

	root = delete_node(root, 5);
	printf("Tree after deleting the node with value %d: \n", 5);
	display_tree(root);


	free_memory(root);
	return 0;
}