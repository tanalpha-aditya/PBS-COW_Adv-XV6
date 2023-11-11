// Initialize libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>

// Initialize colours
#define RED "\033[38;5;1m"
#define GREEN "\033[38;5;121m"
#define WHITE "\033[38;5;15m"
#define YELLOW "\033[38;5;223m"
#define CYAN "\033[38;5;51m"
#define BLUE "\033[38;5;33m"
#define RESET "\033[0m"
#define ETIMEDOUT 110

// Struct for customer storing seconds after program start, time taken to wash clothes, patience in seconds
typedef struct coffee
{
    char name[100];
    int time_needed;
} coffee;

typedef struct customer
{
    int cus_number;
    char coffee[100];
    int time_arrival;
    int time_taken;
    int time_patience;
    int flag;
    int barista_number;
    int waittime;
    bool happy;
} customer;

// typedef struct baristaaa
// {
//     int index;
//     sem_t barista;
// }baristaaa;

struct timespec start_time;

int currentTime = 0;
int startTime = 0;
int cus_not_served = 0;
int barista_num = 0;
int numCustomers;
int numBaristas;
int coffeeTypes;
sem_t barista;

// critical section
void *cus_func(void *arg)
{
    customer *cus = (customer *)arg;
    // sleep until arrival time
    sleep(cus->time_arrival);
    printf(WHITE "Customer %d arrives at %d second(s)\n" RESET, cus->cus_number, cus->time_arrival);
    printf(YELLOW "Customer %d orders a %s\n" RESET, cus->cus_number, cus->coffee);

    // to get current time and add patience input data
    struct timespec patience;
    clock_gettime(CLOCK_REALTIME, &patience);
    patience.tv_sec += cus->time_patience;
    patience.tv_sec += 1;

    // using semaphores to check if barista is available and if available then assign it a customer and make that bariasta busy and if not then make the customer wait till the barista is free or his patience runs out
    if (sem_timedwait(&barista, &patience) == -1)
    {
        printf(RED "Customer %d leaves without their order at %d second(s)\n" RESET, cus->cus_number, cus->time_patience + cus->time_arrival + 1);
        cus_not_served++;
    }
    else
    {
        // printf(YELLOW "Customer %d is being served\n" RESET, cus->cus_number);
        int number;
        sem_getvalue(&barista, &number); // Get the current available barista number
        number = numBaristas - number; // Adjust the number as per available baristas
        sleep(1);
        cus->barista_number = number;
        cus->waittime = time(NULL) - startTime - cus->time_arrival;
        printf(CYAN "Barista %d begins preparing the order of customer %d at %ld second(s)\n" RESET, cus->barista_number , cus->cus_number, time(NULL) - startTime);
        barista_num++;

        if (time(NULL) - startTime + cus->time_taken >= cus->time_arrival + cus->time_patience)
        {
            long int a = cus->time_arrival + cus->time_patience - (time(NULL) - startTime );
            // printf("%ld", a);
            sleep(a);
            printf(RED "Customer %d leaves without their order at %d second(s)\n" RESET, cus->cus_number, cus->time_patience + cus->time_arrival + 1);
            cus_not_served++;
            cus->flag=1;
            sleep(cus->time_taken - a);
            // sleep(cus->time_taken - (cus->time_arrival + cus->time_patience - (time(NULL) + startTime)));
        }
        else{
            sleep(cus->time_taken);
        }
        if(cus->flag==1)
        {
            printf(BLUE "Barista %d completes the order of customer %d at %ld second(s)\n" RESET, cus->barista_number, cus->cus_number, time(NULL) - startTime);
        }
        else{
            printf(BLUE "Barista %d completes the order of customer %d at %ld second(s)\n" RESET, cus->barista_number, cus->cus_number, time(NULL) - startTime);
            printf(GREEN "Customer %d leaves with their order at %ld second(s)\n" RESET, cus->cus_number, time(NULL) - startTime);
            cus->happy = true;
        }
        sem_post(&barista);
    }

} // using semaphore time waiting for washing machine

int main()
{
    scanf("%d %d %d", &numBaristas, &coffeeTypes, &numCustomers);        // Input number of baristas, coffee types and customers
    coffee *arr_coffee = (coffee *)malloc(coffeeTypes * sizeof(coffee)); // Allocate memory for coffee array

    for (int i = 0; i < coffeeTypes; i++)
    {
        scanf("%s %d", arr_coffee[i].name, &arr_coffee[i].time_needed); // Input coffee name and time needed to make
    }

    customer *arr_cus = (customer *)malloc(numCustomers * sizeof(customer)); // Allocate memory for customer array

    for (int i = 0; i < numCustomers; i++)
    {
        scanf("%d %s %d %d", &arr_cus[i].cus_number, arr_cus[i].coffee, &arr_cus[i].time_arrival, &arr_cus[i].time_patience); // Input customer arrival time and patience
        arr_cus[i].happy = false;                                                                                             // Set customer happiness to false
        arr_cus[i].flag = 0;
        for (int j = 0; j < coffeeTypes; j++)
        {
            if (strcmp(arr_cus[i].coffee, arr_coffee[j].name) == 0)
            {
                arr_cus[i].time_taken = arr_coffee[j].time_needed; // Set time taken to make coffee
            }
        }
    }
    clock_gettime(CLOCK_REALTIME, &start_time);
    startTime = start_time.tv_sec;
    // sort arr_cuss by time_after_exec, if it is same sort by customer_number

    for (int i = 0; i < numCustomers; i++)
    {
        for (int j = i + 1; j < numCustomers; j++)
        {
            if (arr_cus[i].time_arrival > arr_cus[j].time_arrival)
            {
                customer temp = arr_cus[i];
                arr_cus[i] = arr_cus[j];
                arr_cus[j] = temp;
            }
            else if (arr_cus[i].time_arrival == arr_cus[j].time_arrival)
            {
                if (arr_cus[i].cus_number > arr_cus[j].cus_number)
                {
                    customer temp = arr_cus[i];
                    arr_cus[i] = arr_cus[j];
                    arr_cus[j] = temp;
                }
            }
        }
    }
    // init semaphores
    sem_init(&barista, 0, numBaristas);

    // create threads for each customer
    pthread_t *cus_thread = (pthread_t *)malloc(numCustomers * sizeof(pthread_t));
    for (int i = 0; i < numCustomers; i++)
    {
        pthread_create(&cus_thread[i], NULL, cus_func, (void *)&arr_cus[i]);
    }

    // join threads
    for (int i = 0; i < numCustomers; i++)
    {
        pthread_join(cus_thread[i], NULL);
    }
    double total = 0 ; 
    for ( int i = 0 ; i < numCustomers ; i++){
        total +=arr_cus[i].waittime;
    }
    double avg = total / (double)numCustomers;
    printf("Avg waiting time : %f\n", avg );
    printf("%d coffee wasted\n", cus_not_served);
    // printf("\n");
    // printf("%d coffee wasted\n", cus_not_served);
}
