#include "node.h"

node::~node() {}
link::~link() {}
node::node()
{
    data = 0;
    next = nullptr;
}

link::link(int nodeCounts)
{
    header = new node;
    header->data = 0;
    header->next = header;
    for (int i = 0; i < nodeCounts; i++)
    {
        node *tempNode = new node;
        tempNode->next = header->next;
        header->next = tempNode;
    }
}

void link::insertNode(node *node)
{
    node->next = header->next;
    header->next = node;
}
