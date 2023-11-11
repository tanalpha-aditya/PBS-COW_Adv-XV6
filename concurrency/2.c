#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <semaphore.h>
#include <stdbool.h>
#include <string.h>

int N, K, F, T, Cust_Inside;

int global_order_counter = 1;
struct timespec timer_start;
int startTime = 0;
struct timespec start_time;

// Initialize colours
#define RED "\033[38;5;1m"
#define ORANGE "\e[38;2;255;85;0m"
#define GREEN "\033[38;5;121m"
#define WHITE "\033[38;5;15m"
#define YELLOW "\033[38;5;223m"
#define CYAN "\033[38;5;51m"
#define BLUE "\033[38;5;33m"
#define RESET "\033[0m"
#define ETIMEDOUT 110

typedef struct machine
{
    int ID;
    int start, end;
    bool available;
} machine;

typedef struct flavour
{
    char name[100];
    int time_needed;
} flavour;

typedef struct topping
{
    char name[100];
    int qty;
} topping;

typedef struct customer
{
    // int status;
    int cus_number;
    int time_arrival;
    int order_number;
    struct order
    {
        int num_topping;
        char flavourr[100];
        int flavour_time;
        char topping_name[10][100];
    } order[10];
} customer;

machine arr_machine[100];
flavour arr_flavour[100];
topping arr_topping[100];
customer arr_customer[100];
sem_t machineSemaphore;
sem_t cusSemaphore;
sem_t parlourSemaphore;
int customer_num;

pthread_t machine_threads[100];
pthread_t customer_threads[100];
int orderProcessed[100][10];  // Assuming a maximum of 100 customers and 10 orders per customer

// void customer_arrive( int customer_index)
// {
//     sleep(arr_customer[])
// }
// Define the order struct
typedef struct orderr
{
    int num_topping;
    char flavourr[100];
    int flavour_time;
    char topping_name[10][100];
} orderr;
struct OrderThreadArgs
    {
        orderr *ord;
        int customerNumber;
        int assigned;
        int order_num;
    };

void *process_order_thread(void *arg)
{
    struct OrderThreadArgs
    {
        orderr *ord;
        int customerNumber;
        int assigned;
        int order_num;
    };

    struct OrderThreadArgs *threadArgs = (struct OrderThreadArgs *)arg;
    orderr *ord = threadArgs->ord;
    int customerNumber = threadArgs->customerNumber;
    int assigned = threadArgs->assigned;
    int served = 0;
    while (served < ord->num_topping)
    {
        // Find an available machine
        int j;
        for (j = 1; j <= N; ++j)
        {

            struct timespec t;
            gettimeofday(&t, NULL);
            u_int64_t now_time = t.tv_sec - timer_start.tv_sec;
                        // printf("nowtimerrr : %d %d %d\n", now_time, customerNumber,  ord->flavour_time);

            if ( now_time + ord->flavour_time >= arr_machine[N].end){
                // printf("return NULL on dddorder %d\n", threadArgs->order_num);
                ord->flavour_time = -99;
                sem_post(&machineSemaphore);
                return NULL;
            }
            sleep(customerNumber/100);
            if (arr_machine[j].available && now_time >= arr_machine[j].start && now_time <= arr_machine[j].end && (now_time + ord->flavour_time <= arr_machine[j].end) && (assigned == 0))
            {
                sem_wait(&machineSemaphore);

                // served = 1;
                assigned == 1;
                threadArgs->assigned=1;
                // Assign the machine to the order
                arr_machine[j].available = false;
                sem_post(&machineSemaphore);
                served++;
                // Start preparing the ice cream
                sleep(1);
                if ( arr_customer[customer_num].time_arrival )
                printf(CYAN"Machine %d starts preparing ice cream %d of customer %d at %d second(s)\n"RESET, j, threadArgs->order_num , customerNumber, time(NULL) - startTime);
                // printf("%s \n", ord->flavourr);
                sleep(ord->flavour_time);
                printf(BLUE"Machine %d completes preparing ice cream %d of customer %d at %d second(s)\n"RESET, j, threadArgs->order_num , customerNumber, time(NULL) - startTime);

                // Release the machine
                // sem_wait(&machineSemaphore);
                arr_machine[j].available = true;
                // sem_post(&machineSemaphore);

                // served++;
                // break;
                // printf("return NULL on uuu %d\n", threadArgs->order_num);
                // printf("%d ;orifja;eorighj\n", arr_machine[N].end);
                return NULL;
            }
        }
        // struct timespec t;
        // gettimeofday(&t, NULL);
        // u_int64_t now_time = t.tv_sec - timer_start.tv_sec;
        // printf("nowtimesss : %d", now_time);
        
    }
    // printf("return NULL on order %d\n", threadArgs->order_num);
    return NULL;
}
void *customer_start_thread(void *arg)
{
    // printf("lol\n");
    //  customer_arrive(*((int *)(arg)));
    customer *cus = (customer *)arg;
    // sleep till the customer arrive
    int cur_order_counter = global_order_counter;
    sleep(cus->time_arrival);
    if (Cust_Inside >= K)
    {
        printf("Customer %d left because parlour was full\n", cus->cus_number);
        return NULL;
    }
    Cust_Inside++;
    printf(WHITE"Customer %d enters at %d second(s)\n"RESET, cus->cus_number, cus->time_arrival);
    printf(YELLOW"Customer %d order %d icecream(s)\n"RESET, cus->cus_number, cus->order_number);
    for (int i = 0; i < cus->order_number; i++)
    {
        printf(YELLOW"Ice cream %d: %s "RESET, i + 1, cus->order[i].flavourr);
        for (int j = 0; j < cus->order[i].num_topping; j++)
        {
            printf(YELLOW"%s "RESET, cus->order[i].topping_name[j]);
        }
        printf("\n");
    }

    global_order_counter++;
    // check for ingrediants sufficiency 
    for ( int i = 0 ; i < cus->order_number ; i++){
        for( int j = 0; j < cus->order[i].num_topping; i++){
            char name[100];
            strcpy(name, cus->order[i].topping_name[j]);
            for( int k = 0 ; k < T ; k ++)
            {
                if(strcmp(arr_topping[k].name, name)){
                    if( arr_topping[k].qty > 0){
                        arr_topping[k].qty--;
                    }
                    else if( arr_topping[k].qty == -1)
                    {
                        ;
                    }
                    else {
                        printf(RED"Customer %d left at %d second(s) with an unfulfilled order\n"RESET, cus->cus_number, cus->time_arrival);
                        sleep(1); // because the spot will be empty from t+1 sec
                        return NULL;
                    }
                }
            }
        }
    }

    int canceledOrder = 0;  // Flag to track if any order has been canceled
    time_t endTime = timer_start.tv_sec + arr_machine[N].end;  // Calculate the end time for machine availability

        // Launch threads for each order
        // printf("enter\n");
    pthread_t order_threads[cus->order_number];
    struct OrderThreadArgs orderArgs[cus->order_number];

    for (int i = 0; i < cus->order_number; ++i)
    {
        orderArgs[i].ord = &cus->order[i];
        orderArgs[i].customerNumber = cus->cus_number;
        orderArgs[i].assigned = 0;
        orderArgs[i].order_num = i +1;

        pthread_create(&order_threads[i], NULL, process_order_thread, (void *)&orderArgs[i]);
    }
    int flag = 0 ;
    for (int i = 0; i < cus->order_number; ++i)
    {
        // printf("time %d\n",orderArgs[i].ord->flavour_time);
        pthread_join(order_threads[i], NULL);
        if ( orderArgs[i].ord->flavour_time == -99){
            flag = 1;
        }
    }
        if (flag ==1){
            printf(RED"Customer %d was not serviced due to unavailability of machines\n"RESET, cus->cus_number);
            Cust_Inside--;
        }
        else{
            printf(GREEN"Customer %d has collected their order(s) and left at %d second(s)\n"RESET, cus->cus_number, time(NULL) - timer_start.tv_sec);
            Cust_Inside--;
        }
    
    // else
    // {
    //     printf("Customer %d has collected their order(s) and left at %d second(s)\n", cus->cus_number, time(NULL) - timer_start.tv_sec);
    //     Cust_Inside--;
    // }
    // printf("hello\n");
    // printf("Customer %d has collected their order(s) and left at %d second(s)\n", cus->cus_number, time(NULL) - timer_start.tv_sec);
    // Cust_Inside--;
        // Wait for all order threads to finish
    
    return NULL;

    // sem_wait(&parlourSemaphore); // wait for parlour
    // int total_served = 0;
    // int pending_order = 0;
    // pending_order = cus->order_number;

    // for ( int i =0 ; i < cus->order_number ; i++){
    //     // printf(" num %d\n", pending_order);
    //     // char flavour[100];
    //     // strcpy(flavour, cus->order[i].flavourr);
    //     sem_wait(&machineSemaphore);
    //     for (int j = 1; j <= N; j++){
    //         struct timespec t;
    //         gettimeofday(&t, NULL);
    //         u_int64_t now_time=t.tv_sec-timer_start.tv_sec;
    //         if(!arr_machine[j].available)
    //         {
    //             sem_post(&machineSemaphore);
    //         }
    //         else if ( now_time >= arr_machine[j].start && now_time <= arr_machine[j].end){
    //             if ( now_time + cus->order[i].flavour_time <= arr_machine[j].end){
    //                     // pending_order++;
    //                     total_served++;
    //                     arr_machine[j].available = false;
    //                     // sem_post(&machineSemaphore);
    //                     sleep(1); // order prep starts 1 sec after ordering 
    //                     printf("Machine %d starts preparing ice cream %d of customer %d at %d second(s)\n", j, i+1, cus->cus_number, time(NULL) - startTime);
    //                     sleep(cus->order[i].flavour_time);
    //                     printf("Machine %d completes preparing ice cream %d of customer %d at %d second(s)\n", j, i+1, cus->cus_number, time(NULL) - startTime);
    //                     pending_order--;
    //                     // printf(" num %d\n", pending_order);
    //                     arr_machine[j].available = true;
    //                     sem_post(&machineSemaphore);
    //                     break;
    //             }
    //         }
    //         else
    //         {
    //             sem_post(&machineSemaphore);
    //         }
    //     }
    //     if( pending_order == 0){
    //         break;
    //     }
    // }
    // if(total_served == cus->order_number){
    //     while(pending_order);
    //     struct timespec t;
    //     gettimeofday(&t, NULL);
    //     printf("Customer %d has collected their order(s) and left at %d second(s)\n", cus->cus_number, t.tv_sec-timer_start.tv_sec);
    //     // cus->status = 3;
    //     Cust_Inside--;
    //     sem_post(&parlourSemaphore);
    // }else{
    //     printf("Customer %d was not serviced due to unavailabiltiy of machines\n", cus->cus_number);
    //     Cust_Inside--;
    //     sem_post(&parlourSemaphore);
    // }
    // return NULL;
}


void *machine_start_thread(void *arg)
{
    // printf("chef %d %d\n",*((int *)(arg)),chefs[*((int *)(arg))].arrival_time);
    //      printf("1\n");

    // machine_arrival(*((int *)(arg)));
    machine *mac = (machine *)arg;

    int a = mac->start;
    sleep(a);
    printf(ORANGE"Machine %d has started working at %d second(s)\n"RESET, mac->ID, mac->start);
    sem_post(&machineSemaphore);
    if (mac->available == false)
    {
        mac->available = true;
    }
    sleep(mac->end - mac->start);
    if (mac->available == true)
    {
        mac->available = false;
    }
    sem_wait(&machineSemaphore);
    printf(ORANGE"Machine %d has stopped working at %d second(s)\n"RESET, mac->ID, mac->end);

    return NULL;
}
void machine_arrival(int machine_index)
{
    int a = arr_machine[machine_index].start;
    // printf("2 %d\n", a);

    sleep(a);
    printf("Machine %d has started working at %d second(s)\n", machine_index, arr_machine[machine_index].start);
    sem_post(&machineSemaphore);
    if (arr_machine[machine_index].available == false)
    {
        arr_machine[machine_index].available = true;
    }
    sleep(arr_machine[machine_index].end - arr_machine[machine_index].start);
    if (arr_machine[machine_index].available == true)
    {
        arr_machine[machine_index].available = false;
    }
    sem_wait(&machineSemaphore);
    printf("Machine %d has stopped working at %d\n", machine_index, arr_machine[machine_index].end);
}

void printMachines(machine *arr_machine, int N)
{
    printf("Machines:\n");
    for (int i = 0; i < N; i++)
    {
        printf("Machine ID: %d, Start: %d, End: %d, Available: %s\n",
               arr_machine[i].ID, arr_machine[i].start, arr_machine[i].end,
               arr_machine[i].available ? "true" : "false");
    }
}

void printFlavours(flavour *arr_flavour, int F)
{
    printf("Flavours:\n");
    for (int i = 0; i < F; i++)
    {
        printf("Flavour Name: %s, Time Needed: %d\n", arr_flavour[i].name, arr_flavour[i].time_needed);
    }
}

void printToppings(topping *arr_topping, int T)
{
    printf("Toppings:\n");
    for (int i = 0; i < T; i++)
    {
        printf("Topping Name: %s, Quantity: %d\n", arr_topping[i].name, arr_topping[i].qty);
    }
}

void printCustomers()
{
    printf("Customers:\n");
    for (int i = 0; i < customer_num; i++)
    {
        printf("Customer Number: %d, Time Arrival: %d, Order Number: %d\n",
               arr_customer[i].cus_number, arr_customer[i].time_arrival, arr_customer[i].order_number);

        for (int j = 0; j < arr_customer[i].order_number; j++)
        {
            printf("Order %d: Flavour: %s, Toppings_num: %d", j + 1, arr_customer[i].order[j].flavourr, arr_customer[i].order[j].num_topping);
            for (int k = 0; k < arr_customer[i].order[j].num_topping; k++)
            {
                printf("%s ", arr_customer[i].order[j].topping_name[k]);
            }
            printf("\n");
        }
    }
}
int main()
{
    scanf("%d %d %d %d", &N, &K, &F, &T);

    // machine *arr_machine = (machine *)malloc(N * sizeof(machine));
    Cust_Inside = 0;
    sem_init(&machineSemaphore, 0, 0);
    sem_init(&parlourSemaphore, 0, 100);
    for (int i = 1; i <= N; ++i)
    {
        arr_machine[i].ID = i;
        scanf("%d %d", &arr_machine[i].start, &arr_machine[i].end);
        arr_machine[i].available = false;
    }

    // flavour *arr_flavour = (flavour *)malloc(F * sizeof(flavour));

    for (int i = 0; i < F; i++)
    {
        scanf("%s %d", arr_flavour[i].name, &arr_flavour[i].time_needed);
    }

    // topping *arr_topping = (topping *)malloc(T * sizeof(topping));

    for (int i = 0; i < T; i++)
    {
        scanf("%s %d", arr_topping[i].name, &arr_topping[i].qty);
    }

    // now i want to take input of customers till the point customers are coming or "\n" is not entered
    // customer *arr_customer = (customer *)malloc(100 * sizeof(customer));
    customer_num = 0;
    while (1)
    {
        // printf("Check1\n");
        if (scanf("%d %d %d", &arr_customer[customer_num].cus_number, &arr_customer[customer_num].time_arrival, &arr_customer[customer_num].order_number) == 3)
        {

            for (int i = 0; i < arr_customer[customer_num].order_number; i++)
            {
                int num_topping = 0;
                scanf("%s", arr_customer[customer_num].order[i].flavourr);

                char name[100];
                strcpy(name,arr_customer[customer_num].order[i].flavourr );
                // arr_customer[customer_num].status = 1;
                for ( int m = 0; m < F; m++){
                    if ( !strcmp (name, arr_flavour[m].name)){
                        arr_customer[customer_num].order[i].flavour_time = arr_flavour[m].time_needed;
                    }
                }
                int k = 0;
                // printf("flavboeu le liya \n");
                while (scanf("%s", arr_customer[customer_num].order[i].topping_name[k]) == 1)
                {
                    k++;
                    // printf("ddddd\n");

                    char c = getchar();
                    if (c == '\n')
                    {
                        ungetc(c, stdin);
                        break;
                    }
                }
                // printf("%d %d\n", i, k);
                arr_customer[customer_num].order[i].num_topping = k;
            }
            customer_num++;
            char c = getchar();
            if (c == 'end')
            {
                ungetc(c, stdin);
                break;
            }
        }
        else
        {
            break;
        }
    }
    clock_gettime(CLOCK_REALTIME, &start_time);
    startTime = start_time.tv_sec;
    // Print the input to check if it was taken correctly
    // printMachines(arr_machine, N);
    // printFlavours(arr_flavour, F);
    // printToppings(arr_topping, T);
    // printCustomers();
    // input done
    // printf("yryr");
    // sem_init(&cusSemaphore, 0, 100);
    gettimeofday(&timer_start, NULL);
    pthread_t *machine_threads = (pthread_t *)malloc((N + 1) * sizeof(pthread_t));
    for (int i = 1; i <= N; i++)
    {
        pthread_create(&machine_threads[i], NULL, machine_start_thread, (void *)&arr_machine[i]);
        // printf("yo\n");
    }

    // printf("Created\n");

    // sleep(20);
    pthread_t *customer_threads = (pthread_t *)malloc(100 * sizeof(pthread_t));
    // similarly make customer threads
    for (int i = 0; i < customer_num; i++)
    {
        pthread_create(&customer_threads[i], NULL, customer_start_thread, (void *)&arr_customer[i]);
    }
    for (int i = 1; i <= N; ++i)
    {
            // printf("dhrfbgkrgbryghbr1\n");

        pthread_join(machine_threads[i],NULL);
    }
    for (int i = 0; i < customer_num; i++)
    {
            // printf("dhrfbgkrgbryghbr2\n");

        pthread_join(customer_threads[i], NULL);
    }
    // sleep(20);
    printf("Parlour Closed\n");
    return 0;
}

//STATUS:
// 1 : not yet started ( waiting)
// 2 : in process
// 3 : completed 



// 2 3 2 3
// 0 10
// 4 10
// vanilla 3
// chocolate 4
// caramel -1
// brownie 4
// strawberry 4
// 1 1 2
// vanilla caramel
// chocolate brownie strawberry
// 2 2 1
// vanilla strawberry caramel
// end
