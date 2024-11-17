#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#define SEM_MUTEX_KEY "keys/mutex_key"
#define SEM_FULL_KEY "keys/full_key"
#define SEM_EMPTY_KEY "keys/empty_key"
#define BUF_KEY "keys/buffer"
#define BUFFERS_N 3
#define N 5
#define SPECIAL 100
#define PROJ_ID 1

struct Message {
    char text[256]; 
};

struct Special{
    struct Message spec_array[SPECIAL];
    int front;
    int rear;
};

struct Buffer {
    int front;
    int rear;
    int length;
    struct Special spec_message;
    struct Message array[N];
};

int mutex_sem[3];
int empty_sem[3];
int full_sem[3];
int shmid[3];
struct Buffer *buffer[3];

union semun 
{
    int val;
    struct semid_ds *buf;
    ushort array [1];
};

int create_sem_id(char* filepath, int proj_id)
{
    key_t s_key;
    int sem;
    if ((s_key = ftok (filepath, proj_id)) == -1) {
        perror ("ftok"); exit (1);
    }
    if ((sem = semget (s_key, 1, 0660 | IPC_CREAT)) == -1) {
        perror ("semget"); exit (1);
    }
    return sem;
}

void set_sem_val(int sem_id, int value)
{
    union semun sem_attr;
    sem_attr.val = value;
    if (semctl (sem_id, 0, SETVAL, sem_attr) == -1) {
        perror ("semctl SETVAL"); exit (1);
    }
}

void deallocate_sem(int sem_id)
{
    union semun ignored_argument;
    semctl (sem_id, 1, IPC_RMID, ignored_argument);
}

int sem_wait(int sem_id)
{
    struct sembuf operations[1];
    operations[0].sem_num = 0;
    operations[0].sem_op = -1;
    operations[0].sem_flg = SEM_UNDO;
    return semop (sem_id, operations, 1);
}

int sem_signal(int sem_id)
{
    struct sembuf operations[1];
    operations[0].sem_num = 0;
    operations[0].sem_op = 1;
    operations[0].sem_flg = SEM_UNDO;
    return semop (sem_id, operations, 1);
}

void wstaw(int id, struct Message m)
{
    sem_wait(empty_sem[id]);
    sem_wait(mutex_sem[id]);
    buffer[id]->rear = (buffer[id]->rear + 1) % N;
    strncpy(buffer[id]->array[buffer[id]->rear].text, m.text, sizeof(buffer[id]->array[buffer[id]->rear].text));
    buffer[id]->length ++;
    sem_signal(mutex_sem[id]);
    sem_signal(full_sem[id]);
}

void wez(int id, struct Message *m)
{
    bool special = false;
    sem_wait(full_sem[id]);
    sem_wait(mutex_sem[id]);
    if (buffer[id]->spec_message.front != buffer[id]->spec_message.rear)
    {
        buffer[id]->spec_message.front = (buffer[id]->spec_message.front + 1) % SPECIAL;
        strncpy(m->text, buffer[id]->spec_message.spec_array[buffer[id]->spec_message.front].text, sizeof(m->text));
        special = true;
    } 
    else
    {
        buffer[id]->front = (buffer[id]->front + 1) % N;
        strncpy(m->text, buffer[id]->array[buffer[id]->front].text, sizeof(m->text));
        buffer[id]->length --;
    }
    sem_signal(mutex_sem[id]);
    if(!special)
        sem_signal(empty_sem[id]);
}

void specjalna(struct Message spec_m)
{
    for (int i=0; i<BUFFERS_N; i++)
    {
        sem_wait(mutex_sem[i]);
        buffer[i]->spec_message.rear = (buffer[i]->spec_message.rear + 1) % SPECIAL;
        strncpy(buffer[i]->spec_message.spec_array[buffer[i]->spec_message.rear].text, spec_m.text,
        sizeof(buffer[i]->spec_message.spec_array[buffer[i]->spec_message.rear].text));
        sem_signal(mutex_sem[i]);
        sem_signal(full_sem[i]);
    }

}

void display(int id) 
{
    int i = buffer[id]->front + 1;
    printf("\n\nBuffer nr: %d\n", id);
    // printf("front %d, text %d\n", buffer[id]->front, buffer[id]->rear);
    if (buffer[id]->length != 0)
    {
        while (i != buffer[id]->rear) {
            printf("Message %s,  ", buffer[id]->array[i].text);
            i = (i + 1) % N;
        }
            printf("Message %s\n", buffer[id]->array[buffer[id]->rear].text);       
    }
    printf("%d/%d\n", buffer[id]->length, N);
    i = buffer[id]->spec_message.front + 1;
    if (buffer[id]->spec_message.front != buffer[id]->spec_message.rear)
    {
        while (i != buffer[id]->spec_message.rear) {
            printf("Special message %s,  ", buffer[id]->spec_message.spec_array[i].text);
            i = (i + 1) % N;
        }
        printf("Special message %s\n", buffer[id]->spec_message.spec_array[buffer[id]->spec_message.rear].text); 
    }
}

int create_buffer(char* filepath)
{
    key_t key = ftok(filepath, PROJ_ID);
    int shmid = shmget(key, sizeof(struct Buffer), IPC_CREAT | 0666);

    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

void deallocate_buffer(struct Buffer *buffer, int id)
{
    shmdt (buffer);
    shmctl (id, IPC_RMID, 0);
}

void system_setup()
{
    char new_str[50];
    int i;
    for (i=0; i<BUFFERS_N; i++)
    {
        snprintf(new_str, sizeof(new_str), "%s%d", SEM_MUTEX_KEY, i);
        mutex_sem[i] = create_sem_id(new_str, PROJ_ID);
        set_sem_val(mutex_sem[i], 1); 

        snprintf(new_str, sizeof(new_str), "%s%d", SEM_EMPTY_KEY, i);
        empty_sem[i] = create_sem_id(new_str, PROJ_ID);
        set_sem_val(empty_sem[i], N);

        snprintf(new_str, sizeof(new_str), "%s%d", SEM_FULL_KEY, i);
        full_sem[i] = create_sem_id(new_str, PROJ_ID);
        set_sem_val(full_sem[i], 0);

        snprintf(new_str, sizeof(new_str), "%s%d", BUF_KEY, i);
        shmid[i] = create_buffer(new_str);
        buffer[i] = shmat(shmid[i], NULL, 0);
        buffer[i]->front = -1;
        buffer[i]->rear = -1;
        buffer[i]->length = 0;
        buffer[i]->spec_message.front= -1;
        buffer[i]->spec_message.rear = -1;
    }
}

void system_clean()
{
int i;
    for (i=0; i<BUFFERS_N; i++)
    {
        deallocate_sem(mutex_sem[i]);
        deallocate_sem(empty_sem[i]);
        deallocate_sem(full_sem[i]);
        deallocate_buffer(buffer[i], shmid[i]);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
        return 0;
    int option = atoi(argv[1]);
    struct Message test[16];
    struct Message spec[5];
    struct Message read;
    char temp_str[50]; 
    for (int i=0; i<16;i++)
    {
        snprintf(temp_str, sizeof(temp_str), "%s%d", "Nr", i);
        strcpy(test[i].text, temp_str);
    }
    for (int i=0; i<5;i++)
    {
        snprintf(temp_str, sizeof(temp_str), "%s%d", "spc ", i);
        strcpy(spec[i].text, temp_str);
    }
    system_setup();
    switch(option)
    {
        case 1:
            printf("-------------------------------------\n");
            printf("   %s\n", "Test normal usage");
            printf("-------------------------------------\n");
            wstaw(0, test[0]);
            wstaw(0, test[1]);
            wstaw(0, test[2]);
            wstaw(1, test[3]);
            wstaw(1, test[4]);
            wstaw(1, test[5]);
            printf("Send special message from PID: %d\n", getpid());
            specjalna(spec[0]);
            display(0);
            display(1);
            sleep(1);
            if (fork()==0)
            {
                wez(0, &read);
                printf("Read: %d, Pid: %d, Message: %s\n", 0, getpid(), read.text);
                fflush(stdout);
                if (fork()==0)
                {
                    wez(0, &read);
                    printf("Read: %d, Pid: %d, Message: %s\n", 0, getpid(), read.text);
                    fflush(stdout);
                    if (fork()==0)
                    {
                        wez(1, &read);
                        printf("Read: %d, Pid: %d, Message: %s\n", 1, getpid(), read.text);
                        fflush(stdout);
                    }
                    else
                    {
                        wez(1, &read);
                        printf("Read: %d, Pid: %d, Message: %s\n", 1, getpid(), read.text);
                        fflush(stdout);
                    }
                }
                else
                {
                    wez(1, &read);
                    printf("Read: %d, Pid: %d, Message: %s\n", 1, getpid(), read.text);
                    fflush(stdout);
                }        
            }
            else
            {
                wez(1, &read);
                printf("\nRead: %d, Pid: %d, Message: %s\n", 1, getpid(), read.text);
                fflush(stdout);
                display(0);
                display(1);
            }
            break;
        case 2:
            printf("-------------------------------------\n");
            printf("%s\n", "Test empty buffer + READ");
            printf("-------------------------------------\n");
            if (fork()==0)
            {
                sleep(5);
                printf("Writing from PID: %d\n", getpid());
                fflush(stdout);
                wstaw(1, test[0]);
            }
            else
            {
                printf("Trying to read from PID: %d\n", getpid());
                display(1);
                fflush(stdout);
                wez(1, &read);
                printf("Read: %d, Pid: %d, Message: %s\n", 1, getpid(), read.text);
                fflush(stdout);
                fflush(stdout);
            }
            break;
        case 4:
            printf("-------------------------------------\n");
            printf("   %s\n", "Test all buffers full + SPECIAL");
            printf("-------------------------------------\n");
            wstaw(0, test[0]);
            wstaw(0, test[1]);
            wstaw(0, test[2]);
            wstaw(0, test[3]);
            wstaw(0, test[4]);
            wstaw(1, test[0]);
            wstaw(1, test[1]);
            wstaw(1, test[2]);
            wstaw(1, test[3]);
            wstaw(1, test[4]);
            wstaw(2, test[0]);
            wstaw(2, test[1]);
            wstaw(2, test[2]);
            wstaw(2, test[3]);
            wstaw(2, test[4]);
            display(0);
            display(1);
            display(2);
            specjalna(spec[0]);
            display(0);
            wez(0, &read);
            printf("Read: %d, Pid: %d, Message: %s\n", 0, getpid(), read.text);
            display(1);
            wez(1, &read);
            printf("Read: %d, Pid: %d, Message: %s\n", 1, getpid(), read.text);
            display(2);
            wez(2, &read);
            printf("Read: %d, Pid: %d, Message: %s\n", 2, getpid(), read.text);
            break;
        case 3:
            printf("-------------------------------------\n");
            printf("%s\n", "Test all buffers full + WRITE NORMAL MESSAGE");
            printf("-------------------------------------\n");
            wstaw(0, test[0]);
            wstaw(0, test[1]);
            wstaw(0, test[2]);
            wstaw(0, test[3]);
            wstaw(0, test[4]);
            wstaw(1, test[0]);
            wstaw(1, test[1]);
            wstaw(1, test[2]);
            wstaw(1, test[3]);
            wstaw(1, test[4]);
            wstaw(2, test[0]);
            wstaw(2, test[1]);
            wstaw(2, test[2]);
            wstaw(2, test[3]);
            wstaw(2, test[4]);
            display(0);
            display(1);
            display(2);
            if (fork() == 0)
            {
                printf("Trying to write from PID: %d\n", getpid());
                fflush(stdout);
                wstaw(0, test[5]);
                wstaw(1, test[6]);
                wstaw(2, test[7]);
                display(0);
                display(1);
                display(2);
                
            }
            else
            {
                sleep(5);
                wez(0, &read);
                printf("Read: %d, Pid: %d, Message: %s\n", 0, getpid(), read.text);
                wez(1, &read);
                printf("Read: %d, Pid: %d, Message: %s\n", 1, getpid(), read.text);
                wez(2, &read);
                printf("Read: %d, Pid: %d, Message: %s\n", 2, getpid(), read.text);
                fflush(stdout);
            }
            break;
        case 5:
            printf("-------------------------------------\n");
            printf("%s\n", "Test multiple special messages");
            printf("-------------------------------------\n");
            wstaw(0, test[0]);
            wstaw(0, test[1]);
            wstaw(1, test[2]);
            wstaw(1, test[3]);
            wstaw(1, test[4]);
            if (fork()==0)
            {
                printf("Sending special 1 and 2 from PID: %d\n", getpid());
                specjalna(spec[1]);
                specjalna(spec[2]);
                
            }
            else
            {   
                printf("Sending special 0 from PID: %d\n", getpid());              
                specjalna(spec[0]);
                sleep(1);
                display(0);
                display(1);
                display(2);
                if(fork()==0)
                {
                    wez(0, &read);
                    printf("\nRead: %d, Pid: %d, Message: %s\n", 0, getpid(), read.text);
                    wez(0, &read);
                    printf("Read: %d, Pid: %d, Message: %s\n", 0, getpid(), read.text);
                }
                else
                {
                    wait(NULL);
                    wez(0, &read);
                    printf("Read: %d, Pid: %d, Message: %s\n", 0, getpid(), read.text);
                    wez(2, &read);
                    printf("Read: %d, Pid: %d, Message: %s\n", 2, getpid(), read.text);
                    sleep(1);
                    display(0);
                    display(1);
                    display(2);
                }
            }
            break;
        case 6:
            wstaw(1, test[2]);
            wstaw(1, test[3]);
            wstaw(1, test[4]);
            display(1);
            printf("Sending special from PID: %d\n", getpid());
            specjalna(spec[0]);
            if (fork()==0)
            {
                printf("Sending special from PID: %d\n", getpid());
                specjalna(spec[1]);
                
            }
            else
            {
                sleep(1);
                display(1);
            }
    }

    system_clean();
    return 0;
}
