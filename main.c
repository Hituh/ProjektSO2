#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

/*Manual*/
void instruction_manual()
{
    char *filename = "manual.txt";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("MAN_ERR: could not open file %s", filename);
        return;
    }
    char ch;
    while ((ch = fgetc(fp)) != EOF)
        putchar(ch);
    fclose(fp);
}
/*Manual*/

/*Variables*/
typedef struct List
{
    int carid;
    struct List *next;
} list;

int id_on_bridge = 0;                         // nr samochodu na moscie
int amount_of_cars = 10;                      // liczba krazacych samochodow
int amountcityA = 0, amountcityB = 0, amountqueueA = 0, amountqueueB = 0; // liczba samochodu w miescie A,miescie B,kolejce A,kolejce B
list *listqueueA = NULL;                          // ktore samochody w kolejce A
list *listqueueB = NULL;                          // ktore samochody w kolejce B

list *listcityA = NULL; // ktore samochody w miasto A
list *listcityB = NULL; // ktore samochody w miasto B

list *listbridgequeue = NULL; // samochody czekajace przy moscie ze strony A i strony B

/*Mutexes*/
pthread_mutex_t mutex_most; // mutex mostu(id_on_bridge)
pthread_mutex_t queue_a;    // mutex kolejki A (m_A)
pthread_mutex_t queue_b;    // mutex kolejki B (m_B)
pthread_mutex_t city_a;     // mutex miasto A (city_A)
pthread_mutex_t city_b;     // mutex miasto B (city_B)

/*Conditional Variables*/
pthread_cond_t before_bridge; // samochod jest przed mostem
pthread_cond_t after_bridge;  // samochod jest za mostem
pthread_mutex_t mutex_A;   // mutex kolejki_A i miasta_A 
pthread_mutex_t mutex_B;   // mutex kolejki_B i miasta_B 
pthread_mutex_t most_lock; // mutex mostu
/*Variables*/

/*List*/
//Add items to the list
void listadd(list **head, int carid)
{
    list *new_node = (list *)malloc(sizeof(list));
    new_node->carid = carid;
    new_node->next = NULL;

    if (*head == NULL)
    {
        *head = new_node;
        return;
    }
    list *temp = *head;
    while (temp->next != NULL)
        temp = temp->next;
    temp->next = new_node;
}

//Remove items from the list
void listremove(list **head, int carid)
{
    list *temp = *head;
    if (temp->carid == carid)
    {
        *head = temp->next;
        free(temp);
        return;
    }
    while (temp->next != NULL)
    {
        if (temp->next->carid == carid)
        {
            list *temp2 = temp->next;
            temp->next = temp->next->next;
            free(temp2);
            return;
        }
        temp = temp->next;
    }
}

//Delete whole list
void listdelete(list **head)
{
    list *temp = *head;
    while (temp != NULL)
    {
        list *temp2 = temp;
        temp = temp->next;
        free(temp2);
    }
    *head = NULL;
}

//Return first item from the list
int first(list *head)
{
    if (head == NULL)
        return -1;
    return head->carid;
}
/*List*/

/*Displaying + common*/


//Display car on the bridge
void displaybridge()
{
    if (id_on_bridge == 0) //No car on the bridge
    {
        printf("A-%d %d>>> [       ] <<<%d %d-B\n",
               amountcityA, amountqueueA, amountqueueB, amountcityB);
    }
    else if (id_on_bridge > 0) //Car on the bridge going from A to B
    {
        printf("A-%d %d>>> [>> %d >>] <<<%d %d-B\n",
               amountcityA, amountqueueA, id_on_bridge, amountqueueB, amountcityB);
    }
    else//Car on the bridge going from B to A
    {
        printf("A-%d %d>>> [<< %d <<] <<<%d %d-B\n",
               amountcityA, amountqueueA, -id_on_bridge, amountqueueB, amountcityB);
    }
}

//Wait for 1 second
void wait()
{
    usleep(1000000);
}
/*Displaying + common*/

/*Mutex*/
void *CarMutex(void *id)
{
    char c;
    int numer = (int)id;
    if (numer > 0)
    {
        c = 'A';
    }
    else
    {
        c = 'B';
        numer = (-numer);
    }
        wait();
        if (c == 'A')
        // samochod przemieszcza sie z miasta A do kolejki A
        {
            //Move a car from city A to queue A
            pthread_mutex_lock(&city_a);
            pthread_mutex_lock(&queue_a);

            amountcityA--;
            amountqueueA++;

            displaybridge();
            pthread_mutex_unlock(&queue_a);
            pthread_mutex_unlock(&city_a);
            // samochod przemieszcza sie z kolejki A na most
            pthread_mutex_lock(&mutex_most);
            pthread_mutex_lock(&queue_a);

            amountqueueA--;
            id_on_bridge = numer;

            displaybridge();

            pthread_mutex_unlock(&queue_a);

            wait();
            // samochod przemieszcza sie z  mostu do miasta B
            pthread_mutex_lock(&city_b);
            amountcityB++;
            id_on_bridge = 0;

            c = 'B';
            displaybridge();
            pthread_mutex_unlock(&mutex_most);
            pthread_mutex_unlock(&city_b);
        }
        else if (c == 'B')
        { // samochod przemieszcza sie z miasta B do kolejki B
            pthread_mutex_lock(&city_b);
            pthread_mutex_lock(&queue_b);

            amountcityB--;
            amountqueueB++;

            displaybridge();
            pthread_mutex_unlock(&queue_b);
            pthread_mutex_unlock(&city_b);
            // samochod przemieszcza sie z kolejki B na most
            pthread_mutex_lock(&mutex_most);
            pthread_mutex_lock(&queue_b);

            amountqueueB--;
            id_on_bridge = -numer;

            displaybridge();

            pthread_mutex_unlock(&queue_b);

            wait();
            // samochod przemieszcza sie z  mostu do miasta A
            pthread_mutex_lock(&city_a);
            amountcityA++;
            id_on_bridge = 0;

            c = 'A';
            displaybridge();
            pthread_mutex_unlock(&mutex_most);
            pthread_mutex_unlock(&city_a);
        }
    
}
/*Mutex*/

/*Conditional Variables*/
void *most()
{
    int numer = 0;
    while (1)
    {
        pthread_mutex_lock(&most_lock);
        // oczekiwanie na samochod przed mostem
        pthread_cond_wait(&before_bridge, &most_lock);
        numer = first(listbridgequeue);

        if (numer > 0)
        {
            pthread_mutex_lock(&mutex_A);
            pthread_mutex_lock(&mutex_B);
            listremove(&listbridgequeue, numer);
            amountqueueA--;
            id_on_bridge = numer;
            displaybridge();

            wait();
            pthread_mutex_unlock(&mutex_A);
            pthread_mutex_unlock(&mutex_B);
            pthread_cond_signal(&after_bridge);
        }
        else if (numer < 0)
        {
            pthread_mutex_lock(&mutex_A);
            pthread_mutex_lock(&mutex_B);
            listremove(&listbridgequeue, numer);
            amountqueueB--;
            id_on_bridge = numer;
            displaybridge();

            wait();
            pthread_mutex_unlock(&mutex_A);
            pthread_mutex_unlock(&mutex_B);
            pthread_cond_signal(&after_bridge);
        }

        pthread_mutex_unlock(&most_lock);
    }
}
void *CarVar(void *id)
{
    char c;
    int numer = (int)id;
    if (numer > 0)
    {
        c = 'A';
    }
    else
    {
        c = 'B';
        numer = (-numer);
    }

    while (1)
    {
        wait();

        if (c == 'A')
        { // przejscie z miasta A do kolejki A
            pthread_mutex_lock(&mutex_A);
            pthread_mutex_lock(&mutex_B);

            amountcityA--;
            amountqueueA++;

            displaybridge();
            listadd(&listbridgequeue, numer);
            pthread_mutex_unlock(&mutex_A);
            pthread_mutex_unlock(&mutex_B);
            // wyslanie sygnalu do mostu

            pthread_cond_signal(&before_bridge);

            pthread_mutex_lock(&mutex_B);
            // oczekiwanie na zejscie z mostu
            pthread_cond_wait(&after_bridge, &mutex_B);
            // samochod jest w miescie B
            amountcityB++;
            displaybridge();
            c = 'B';
            pthread_mutex_unlock(&mutex_B);
        }

        else if (c == 'B')
        { // przejscie z miasta B do kolejki B
            pthread_mutex_lock(&mutex_A);
            pthread_mutex_lock(&mutex_B);

            amountcityB--;
            amountqueueB++;

            displaybridge();
            listadd(&listbridgequeue, -numer);
            pthread_mutex_unlock(&mutex_A);
            pthread_mutex_unlock(&mutex_B);
            // wyslanie sygnalu do mostu

            pthread_cond_signal(&before_bridge);

            pthread_mutex_lock(&mutex_A);
            // oczekiwanie na zejscie z mostu
            pthread_cond_wait(&after_bridge, &mutex_A);
            // samochod jest w miescie B
            amountcityA++;
            displaybridge();
            c = 'A';
            pthread_mutex_unlock(&mutex_A);
        }
    }
}
/*Conditional Variables*/

int main(int argc, char *argv[])
{
    /*Required inputs check*/
    // if first argument is -h or -help display manual
    if (argc >= 2)
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0)
        {
            instruction_manual();
            exit(0);
        }
    int type = 0; // 0 for Mutex/Semaphore, 1 for Conditional Variables
    int choice;
    while ((choice = getopt(argc, argv, "N:T:")) != -1)
    {
        switch (choice)
        {
        case 'N':
            amount_of_cars = atoi(optarg);
            if (amount_of_cars == 0)
            {
                printf("Invalid argument for N\n");
                printf("Valid argument is only a positive integer\n");
                printf("Unless you wrote 0, in which case it will be set to default = 10, since there is no point of running the program with 0 cars.\n");
                exit(1);
            }
            break;
        case 'T':
            type = atoi(optarg);
            if (type != 0 && type != 1)
            {
                printf("Invalid variable -T. Please use '-T 0'for Mutexes and Semaphores, '-T 1' for Conditional Variables\n");
                exit(1);
            }
            break;
        default:
            printf("Wrong arguments\n");
            exit(1);
        }
    }
    /*Required inputs check/

    /*Main Code*/
    if (type == 0) // Mutex
    {
        pthread_mutex_init(&mutex_most, NULL);
        pthread_mutex_init(&queue_a, NULL);
        pthread_mutex_init(&queue_b, NULL);
        pthread_mutex_init(&city_a, NULL);
        pthread_mutex_init(&city_b, NULL);

        pthread_t *samochod_watki = malloc(sizeof(pthread_t) * amount_of_cars);
        int i = 0;
        amountcityA = amount_of_cars / 2 + (amount_of_cars % 2);
        amountcityB = amount_of_cars / 2;

        displaybridge();
        for (i = 0; i < amount_of_cars; ++i)
        {
            if ((i % 2) == 0)
            {
                pthread_create(&samochod_watki[i], NULL, CarMutex, (void *)(i + 1));
            }
            else
            {
                pthread_create(&samochod_watki[i], NULL, CarMutex, (void *)(-i - 1));
            }
        }

        for (i = 0; i < amount_of_cars; i++)
        {
            pthread_join(samochod_watki[i], NULL);
        }

        pthread_mutex_destroy(&queue_a);
        pthread_mutex_destroy(&queue_b);
        pthread_mutex_destroy(&city_a);
        pthread_mutex_destroy(&city_b);
        pthread_mutex_destroy(&mutex_most);

        listdelete(&listqueueA);
        listdelete(&listqueueB);
        listdelete(&listcityA);
        listdelete(&listcityB);

        return 0;
    }
    if (type == 1) // Conditional Variables
    {
        pthread_mutex_init(&most_lock, NULL);
        pthread_mutex_init(&mutex_A, NULL);
        pthread_mutex_init(&mutex_B, NULL);
        pthread_cond_init(&before_bridge, NULL);
        pthread_cond_init(&after_bridge, NULL);

        pthread_t *samochod_watki = malloc(sizeof(pthread_t) * amount_of_cars);
        pthread_t most_watek;
        pthread_create(&most_watek, NULL, most, NULL);
        int i = 0;
        amountcityA = amount_of_cars / 2 + (amount_of_cars % 2);
        amountcityB = amount_of_cars / 2;

        displaybridge();
        for (i = 0; i < amount_of_cars; ++i)
        {
            if ((i % 2) == 0)
            {
                pthread_create(&samochod_watki[i], NULL, CarVar, (void *)(i + 1));
            }
            else
            {
                pthread_create(&samochod_watki[i], NULL, CarVar, (void *)(-i - 1));
            }
        }

        for (i = 0; i < amount_of_cars; i++)
        {
            pthread_join(samochod_watki[i], NULL);
        }

        pthread_join(most_watek, NULL);
        pthread_mutex_destroy(&most_lock);
        pthread_mutex_destroy(&mutex_A);
        pthread_mutex_destroy(&mutex_B);
        pthread_cond_destroy(&before_bridge);
        pthread_cond_destroy(&after_bridge);

        listdelete(&listqueueA);
        listdelete(&listqueueB);
        listdelete(&listcityA);
        listdelete(&listcityB);
        listdelete(&listbridgequeue);
        return 0;
    }
    /*Main Code*/
}
