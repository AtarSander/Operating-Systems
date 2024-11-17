#include <iostream>
#include <pthread.h>
#include "buffer.h"
int const buffers_n = 3;
Buffer buf[3];

struct thread_args{
    Message m_params[10];
    int len;
    int num;
    int sleep = 0;
};

void* special(void* parameters)
{
    struct thread_args* p = (struct thread_args*) parameters;
    std::cout<<"Sending special from thread "<<pthread_self()<<std::endl;
    for(int i=0; i<buffers_n; i++)
    {
        buf[i].special(p->m_params[0]);
    }
    return NULL;
}

void* producer(void* parameters)
{
    struct thread_args* p = (struct thread_args*) parameters;
    if (p->sleep != 0)
    {   
        sleep(p->sleep);
        std::cout<<"=======BUF "<<p->num<<"========"<<std::endl<<std::flush;
    }
    else
    {
        std::cout<<"=======BUF "<<p->num<<"========"<<std::endl<<std::flush;
        std::cout<<"Trying to write from thread "<<pthread_self()<<std::endl<<std::flush;
    }
        
    
    for (int i=0; i<p->len; i++)
    {
        buf[p->num].write(p->m_params[i]);
    }
    std::cout<<"Writing from thread "<<pthread_self()<<std::endl<<std::flush;
    return NULL;
}

void* consumer(void* parameters)
{
    struct thread_args* p = (struct thread_args*) parameters;
    
    if (p->sleep != 0)
    {
        sleep(p->sleep);
        std::cout<<"=======BUF "<<p->num<<"========"<<std::endl<<std::flush;
    }
    else
    {
        std::cout<<"=======BUF "<<p->num<<"========"<<std::endl;
        std::cout<<"Trying to read from thread "<<pthread_self()<<std::endl<<std::flush;
    }   
    for (int i=0; i<p->len; i++)
    {
        buf[p->num].read(p->m_params[i]);
        std::cout<<"Read "<<p->m_params[i].text <<" from thread "<< pthread_self()<<std::endl<<std::flush;
    }
    return NULL;
}

void* display(void* parameters)
{
    struct thread_args* p = (struct thread_args*) parameters;
    std::cout<<"=======BUF "<<p->num<<"========"<<std::endl<<std::flush;
    buf[p->num].display();
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 0;
    int option = atoi(argv[1]);
    int thread_count = 0;
    struct Message test[16];
    struct Message spec[5];
    struct Message read;
    char temp_str[50]; 

    for (int i=0; i<16;i++)
    {
        std::string temp_str = "Nr" + std::to_string(i);
        test[i].text = temp_str;
    }
    for (int i=0; i<5;i++)
    {
        std::string temp_str = "spc" + std::to_string(i);
        spec[i].text = temp_str;
    }
    switch(option)
    {
        case 1:
            std::cout<<"-------------------------------------\n";
            std::cout<<"         Test normal usage\n";
            std::cout<<"-------------------------------------\n";
            thread_count = 15;
            {
            pthread_t tid1[thread_count];
            struct thread_args write0, write1, write2, wnext, reading0,
            reading1, reading2, dis0, dis1, dis2, spc0, spc1;
            for (int i=0; i<5; i++)
                write0.m_params[i] = test[i];
            for (int i=0; i<5; i++)
                write1.m_params[i] = test[i];
            for (int i=0; i<5; i++)
                write2.m_params[i] = test[i];
            write0.len = 3;
            write0.num = 0;
            write0.sleep = 3;
            write1.len = 5;
            write1.num = 1;
            write2.len = 5;
            write2.num = 2;
            dis0.num = 0;
            dis1.num = 1;
            dis2.num = 2;
            reading0.num = 0;
            reading0.len = 2;
            reading1.num = 2;
            reading1.len = 4; 
            reading2.num = 1;
            reading2.len = 3;
            reading2.sleep = 3;
            spc0.m_params[0] = spec[0];
            wnext.m_params[0] = test[5];
            wnext.m_params[1] = test[6];
            wnext.num = 1;
            wnext.len = 2;
            pthread_create (&(tid1[0]), NULL, &producer, &write0);
            pthread_create (&(tid1[1]), NULL, &producer, &write1);
            pthread_create (&(tid1[2]), NULL, &producer, &write2);
            pthread_create (&(tid1[11]), NULL, &consumer, &reading0);
            pthread_join(tid1[11], (void**)NULL);
            pthread_join(tid1[0], (void**)NULL);
            pthread_join(tid1[1], (void**)NULL);
            pthread_join(tid1[2], (void**)NULL);
            pthread_create (&(tid1[10]), NULL, &special, &spc0);
            pthread_join(tid1[10], (void**)NULL);
            pthread_create (&(tid1[12]), NULL, &display, &dis0);
            pthread_join(tid1[12], (void**)NULL);
            pthread_create (&(tid1[13]), NULL, &display, &dis1);
            pthread_join(tid1[13], (void**)NULL);
            pthread_create (&(tid1[14]), NULL, &display, &dis2);
            pthread_join(tid1[14], (void**)NULL);
            pthread_create (&(tid1[3]), NULL, &consumer, &reading1);
            pthread_join(tid1[3], (void**)NULL);
            pthread_create (&(tid1[4]), NULL, &producer, &wnext);
            pthread_create (&(tid1[5]), NULL, &consumer, &reading2);
            pthread_join(tid1[4], (void**)NULL);
            pthread_join(tid1[5], (void**)NULL);
            pthread_create (&(tid1[9]), NULL, &display, &dis0);
            pthread_join(tid1[9], (void**)NULL);
            pthread_create (&(tid1[8]), NULL, &display, &dis1);
            pthread_join(tid1[8], (void**)NULL);
            pthread_create (&(tid1[7]), NULL, &display, &dis2);
            pthread_join(tid1[7], (void**)NULL);
            }
            break;
        case 2:
            std::cout<<"-------------------------------------\n";
            std::cout<<"    Test empty buffer + READ\n";
            std::cout<<"-------------------------------------\n";
            thread_count = 2;
            {
            pthread_t tid2[thread_count];
            struct thread_args r, s;
            r.m_params[0] = read;
            r.len = 1;
            r.num = 0;
            s.m_params[0] = test[0];
            s.len = 1;
            s.num = 0;
            s.sleep = 5;
            pthread_create (&(tid2[1]), NULL, &consumer, &r);
            pthread_create (&(tid2[0]), NULL, &producer, &s);
            for (int i=0; i< thread_count; i++)
                pthread_join(tid2[i], (void**)NULL);
            }
            break;
        case 3:
            std::cout<<"-------------------------------------\n";
            std::cout<<"    Test all buffer full + WRITE\n";
            std::cout<<"-------------------------------------\n";
            thread_count = 7;
            {
            pthread_t tid3[thread_count];
            struct thread_args full0, full1, full2, next, reading, dis;
            for (int i=0; i<5; i++)
                full0.m_params[i] = test[i];
            for (int i=0; i<5; i++)
                full1.m_params[i] = test[i];
            for (int i=0; i<5; i++)
                full2.m_params[i] = test[i];
            full0.len = 5;
            full0.num = 0;
            full1.len = 5;
            full1.num = 1;
            full2.len = 5;
            full2.num = 2;
            next.m_params[0] = test[5];
            next.len = 1;
            next.num = 2;
            reading.m_params[0] = read;
            reading.len = 1;
            reading.num = 2;
            reading.sleep = 5;
            dis.num = 2;
            pthread_create (&(tid3[0]), NULL, &producer, &full0);
            pthread_create (&(tid3[1]), NULL, &producer, &full1);
            pthread_create (&(tid3[2]), NULL, &producer, &full2);
            pthread_create (&(tid3[3]), NULL, &producer, &next);
            pthread_create (&(tid3[4]), NULL, &display, &dis);
            pthread_create (&(tid3[5]), NULL, &consumer, &reading);
            for (int i=0; i< thread_count-1; i++)
                pthread_join(tid3[i], (void**)NULL);
            pthread_create (&(tid3[6]), NULL, &display, &dis);
            pthread_join(tid3[6], (void**)NULL);
            }
            break;
        case 4:
            std::cout<<"-------------------------------------\n";
            std::cout<<"   Test all buffers full + SPECIAL\n";
            std::cout<<"-------------------------------------\n";
            thread_count = 9;
            {
            pthread_t tid4[thread_count];
            struct thread_args full0, full1, full2, specm, reading1, reading2, dis0, dis1, dis2;
            for (int i=0; i<5; i++)
                full0.m_params[i] = test[i];
            for (int i=0; i<5; i++)
                full1.m_params[i] = test[i];
            for (int i=0; i<5; i++)
                full2.m_params[i] = test[i];
            full0.len = 5;
            full0.num = 0;
            full1.len = 5;
            full1.num = 1;
            full2.len = 5;
            full2.num = 2;
            specm.m_params[0] = spec[0];
            reading1.len = 2;
            reading1.num = 0;
            reading2.len = 1;
            reading2.num = 2;
            dis0.num = 0;
            dis1.num = 1;
            dis2.num = 2;
            pthread_create (&(tid4[0]), NULL, &producer, &full0);
            pthread_create (&(tid4[1]), NULL, &producer, &full1);
            pthread_create (&(tid4[2]), NULL, &producer, &full2);
            pthread_create (&(tid4[3]), NULL, &special, &specm);
            for (int i=0; i<4; i++)
                    pthread_join(tid4[i], (void**)NULL);
            pthread_create (&(tid4[6]), NULL, &display, &dis0);
            pthread_join(tid4[6], (void**)NULL);
            pthread_create (&(tid4[7]), NULL, &display, &dis1);
            pthread_join(tid4[7], (void**)NULL);
            pthread_create (&(tid4[8]), NULL, &display, &dis2);
            pthread_join(tid4[8], (void**)NULL);
            pthread_create (&(tid4[4]), NULL, &consumer, &reading1);
            pthread_create (&(tid4[5]), NULL, &consumer, &reading2);
            pthread_join(tid4[4], (void**)NULL);
            pthread_join(tid4[5], (void**)NULL);
            pthread_create (&(tid4[6]), NULL, &display, &dis0);
            pthread_join(tid4[6], (void**)NULL);
            pthread_create (&(tid4[7]), NULL, &display, &dis1);
            pthread_join(tid4[7], (void**)NULL);
            pthread_create (&(tid4[8]), NULL, &display, &dis2);
            pthread_join(tid4[8], (void**)NULL);
            }
            break;
        case 5:
            std::cout<<"-------------------------------------\n";
            std::cout<<"   Test multiple special messages\n";
            std::cout<<"-------------------------------------\n";
            thread_count = 11;
            {
                pthread_t tid5[thread_count];
                struct thread_args full0, full1, full2, specm0, specm1, specm2, reading1, reading2, dis0, dis1, dis2;
                for (int i=0; i<5; i++)
                    full0.m_params[i] = test[i];
                for (int i=0; i<5; i++)
                    full1.m_params[i] = test[i];
                for (int i=0; i<5; i++)
                    full2.m_params[i] = test[i];
                specm0.m_params[0] = spec[0];
                specm1.m_params[0] = spec[1];
                specm2.m_params[0] = spec[2];
                full0.len = 3;
                full0.num = 0;
                full1.len = 5;
                full1.num = 1;
                full2.len = 4;
                full2.num = 2;
                dis0.num = 0;
                dis1.num = 1;
                dis2.num = 2;
                reading1.num = 1;
                reading1.len = 4;
                reading2.num = 2;
                reading2.len = 2; 
                pthread_create (&(tid5[0]), NULL, &producer, &full0);
                pthread_create (&(tid5[1]), NULL, &producer, &full1);
                pthread_create (&(tid5[2]), NULL, &producer, &full2);
                pthread_create (&(tid5[3]), NULL, &special, &specm0);
                pthread_create (&(tid5[4]), NULL, &special, &specm1);
                pthread_create (&(tid5[5]), NULL, &special, &specm2);
                for (int i=0; i<6; i++)
                    pthread_join(tid5[i], (void**)NULL);
                pthread_create (&(tid5[6]), NULL, &display, &dis0);
                pthread_join(tid5[6], (void**)NULL);
                pthread_create (&(tid5[7]), NULL, &display, &dis1);
                pthread_join(tid5[7], (void**)NULL);
                pthread_create (&(tid5[8]), NULL, &display, &dis2);
                pthread_join(tid5[8], (void**)NULL);
                pthread_create (&(tid5[9]), NULL, &consumer, &reading1);
                pthread_create (&(tid5[10]), NULL, &consumer, &reading2);
                pthread_join(tid5[9], (void**)NULL);
                pthread_join(tid5[10], (void**)NULL);
                pthread_create (&(tid5[6]), NULL, &display, &dis0);
                pthread_join(tid5[6], (void**)NULL);
                pthread_create (&(tid5[7]), NULL, &display, &dis1);
                pthread_join(tid5[7], (void**)NULL);
                pthread_create (&(tid5[8]), NULL, &display, &dis2);
                pthread_join(tid5[8], (void**)NULL);
            }
            break;
    }
    return 0;
}