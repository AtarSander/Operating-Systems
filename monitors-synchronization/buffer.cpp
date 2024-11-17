#include "buffer.h"
#include "monitor.h"
#include <queue>
#include <string>
#include <iostream>


void Buffer::write(Message mes)
{
    m.enter();
    if (count == N)
        m.wait(full);
    norm_q.push(mes);
    count += 1;
    if (count == 1)
        m.signal(empty);
    m.leave();
}


void Buffer::read(Message &mes)
{
    m.enter();
    if (spec_q.empty())
    {
        if (count == 0)
            m.wait(empty);
        mes = norm_q.front();
        norm_q.pop();
        count -= 1;
        if (count == N-1)
            m.signal(full);
    }
    else
    {
        mes = spec_q.front();
        spec_q.pop();
    }
    m.leave();
}


void Buffer::special(Message mes)
{
    m.enter();
    spec_q.push(mes);
    m.leave();
}


void Buffer::display()
{
    m.enter();
    std::queue<Message> temp = norm_q; 

    while (!temp.empty()) {
        std::cout << temp.front().text << " "<<std::flush;  
        temp.pop(); 
    }

    std::cout << std::endl<<std::flush;

    std::queue<Message> temp2 = spec_q; 

    while (!temp2.empty()) {
        std::cout << temp2.front().text << " "<<std::flush;  
        temp2.pop(); 
    }
    std::cout << std::endl<<std::flush;
    m.leave();
}
