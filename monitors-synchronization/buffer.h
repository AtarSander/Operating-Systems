#ifndef BUFFER_H
#define BUFFER_H
#include "monitor.h"
#include <queue>
#include <string>

struct Message
{
    std::string text;
};

class Buffer
{
    public:
        Buffer(int N=5) : N(N){};
        void write(Message m);
        void read(Message &m);
        void special(Message m);
        void display();
    private: 
        std::queue<Message> norm_q;
        std::queue<Message> spec_q;
        Monitor m;
        Condition full;
        Condition empty;
        int count = 0;
        int N;
};
#endif