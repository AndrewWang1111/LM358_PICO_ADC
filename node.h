#pragma once
#include <iostream>
using namespace std;

class node
{
public:
    node();
    ~node();
    uint16_t data;
    node *next;
};

class link
{
public:
    link(int);
    ~link();
    node *header;
    void insertNode(node *node);
};