struct node{
	char *elem;
	struct node *nextNodePtr;
}node;
typedef struct node* nodePtr;

nodePtr generateNode(char *elem);
void insertNode(nodePtr *ptr, nodePtr newNode);
nodePtr removeNode(nodePtr *ptr);
void printQueue(nodePtr ptr);
int queueLen(nodePtr ptr);
