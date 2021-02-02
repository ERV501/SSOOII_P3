#include "Definitions.h"
#include "Client.h"
#include "FoodStand.h"
#include "TicketStand.h"
#include "Replenisher.h"
#include "Request_Ticket.h"
#include "Request_Food.h"
#include "Request_Replenisher.h"
#include "Request_Payment.h"
#include "Payment_System.h"

#include <string>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <vector>
#include <condition_variable>

using namespace std;

/* vectores de hilos */
vector <thread> vector_thread_Clients;
vector <thread> vector_thread_TicketStand;
vector <thread> vector_thread_FoodStand;
vector <thread> vector_thread_Replenisher;
thread thread_Payment_System;

/* Comunicacion de oficinas de entrada */
condition_variable cv_TicketStand_start;
condition_variable cv_TicketStand_finish;

/* Comunicacion de oficinas de comida y bebida */
condition_variable cv_FoodStand_start;
condition_variable cv_FoodStand_finish;

/* Comunicacion del reponedor */
condition_variable cv_Replenisher_start;
condition_variable cv_Replenisher_finish;

/* Comunicacion del sistema de pago */
condition_variable cv_Payment_start;
condition_variable cv_Payment_finish;

/* vectores de recursos*/
vector <TicketStand> vector_TicketStand;
vector <FoodStand> vector_FoodStand;
vector <Replenisher> vector_Replenisher;

/* colas de peticiones */
queue <Request_Ticket> queue_Request_Ticket;
queue <Request_Food> queue_Request_Food;
queue <Request_Replenisher> queue_Request_Replenisher;
queue <Request_Payment> queue_Request_Payment;

/* semaforos mutex para garantizar acceso eclusivo */
mutex mtx_queue_Request_Ticket;
mutex mtx_queue_Request_Food;
mutex mtx_queue_Request_Replenisher;
mutex mtx_queue_Request_Payment;
mutex mtx_seats_bought; /* controlar acceso a operacion de compra de asientos */
mutex mtx_Print; /* semafor para impresion */

/* variables globales */
int g_seats_bought = 0; /* variable global compartida que usa cada hilo para conocer los asientos que ya no estan disponibles */

int main(int argc, const char* argv[]);
void checkArgs(int argc, char const* argv[], int& n_TicketStands, int& n_FoodStands, int& n_Replenishers, int& n_clients);
void dataInitialization();
void generateClients(int n_clients);
void launchingFeatures(int & n_TicketStands, int & n_FoodStands, int & n_Replenishers);
void waitForClientFinalization();


int main(int argc, const char* argv[]) {

    int n_TicketStands, n_FoodStands, n_Replenishers, n_clients;

    checkArgs(argc, argv, n_TicketStands, n_FoodStands, n_Replenishers, n_clients); /* comprobar argumentos de ejecucion */

    /*initilization of data*/
    dataInitialization();

    /* lanzar servicios */
    launchingFeatures(n_TicketStands, n_FoodStands, n_Replenishers);

    /* generar clientes aleatorios */
    generateClients(n_clients);

    /* esperar hilos en ejecucion */
    waitForClientFinalization();

    cout << "\033[1;37m--[Main] The cinema has finished its operation successfully --\033[0m\n";

    return EXIT_SUCCESS;
}


void checkArgs(int argc, char const* argv[], int& n_TicketStands, int& n_FoodStands, int& n_Replenishers, int& n_clients) {

    int supported_threads = thread::hardware_concurrency();
    bool check_client = false, check_office = false, check_FoodStands = false, check_teller = false;

    string again;

    if (argc != 5) {
        cout << "\033[1;31m--[ERROR] Execution: . /Cinema_system <n_booking_office> <n_FoodStands> <n_teller> <n_clients> --\033[0m\n";
        exit(EXIT_FAILURE);
    }

    n_TicketStands = atoi(argv[1]);
    n_FoodStands = atoi(argv[2]);
    n_Replenishers = atoi(argv[3]);
    n_clients = atoi(argv[4]); /* los hilos pueden ser los que se quiera */

    if(n_TicketStands > supported_threads || n_FoodStands > supported_threads || n_Replenishers > supported_threads){
        cout << "\033[1;31m--[ERROR] Unsupported threads, maximun = " << supported_threads << "--\033[0m\n";
        exit(EXIT_FAILURE);
    }


}

void generateClients(int n_clients) {

    for (int i = 0; i < n_clients; i++) {
        int r1_seats = (rand() % 8 + 1); /* generar numero aleatorio de 1 a 8, que son los asientos que un cliente puede pedir */
        int r2_drinks = (rand() % 10 + 1); /* generar numero aleatorio de 1 a 10, que son las bebida que un cliente puede pedir */
        int r3_popcorns = (rand() % 10 + 1); /* generar numero aleatorio de 1 a 10, que son las palomitas que un cliente puede pedir */

        /* crear peticiones */
        Request_Ticket rq_ticket(r1_seats);
        Request_Food rq_food(r2_drinks, r3_popcorns);

        /* Crear clientes */
        Client cinema_client(i, &mtx_Print, &queue_Request_Food, &mtx_queue_Request_Food, &cv_FoodStand_start, &cv_FoodStand_finish, rq_food, false, &queue_Request_Ticket, &mtx_queue_Request_Ticket, &cv_TicketStand_start, &cv_TicketStand_finish, rq_ticket, false, false);

        /* Crear hilo e insertar en cola de todos los hilos cliente*/
        vector_thread_Clients.push_back(thread(cinema_client));

        this_thread::sleep_for(chrono::seconds(1)); /* espera de 1s */
    }
}

void waitForClientFinalization() { /* controlar que todos los hilos clientes finalicen antes de finalizar el main */
    for (unsigned int i = 0; i < vector_thread_Clients.size(); i++) {
        if (vector_thread_Clients.at(i).joinable()) {
            vector_thread_Clients.at(i).join(); // esperamos a los hilos que esten en ejecucion
        }
    }
}

void dataInitialization() {

    /* reserva de memoria para colas y vectores */

    vector_thread_Clients = vector <thread>();
    vector_thread_TicketStand = vector <thread>();
    vector_thread_FoodStand = vector <thread>();
    vector_thread_Replenisher = vector <thread>();

    vector_TicketStand = vector <TicketStand>();
    vector_FoodStand = vector <FoodStand>();
    vector_Replenisher = vector <Replenisher>();

    queue_Request_Ticket = queue <Request_Ticket>();
    queue_Request_Food = queue <Request_Food>();
    queue_Request_Replenisher = queue <Request_Replenisher>();
    queue_Request_Payment = queue <Request_Payment>();
}

void launchingFeatures(int &n_TicketStands, int& n_FoodStands, int& n_Replenishers) {

    thread_Payment_System = thread(Payment_System(&mtx_Print,&cv_Payment_start,&cv_Payment_finish,&queue_Request_Payment,&mtx_queue_Request_Payment));
    cout << "\033[1;37m [Main] Payment system is ready \033[0m\n";

    for (int i = 0; i < n_TicketStands; i++) {
        TicketStand ticketStand( i, &g_seats_bought, false,
            &mtx_seats_bought, &mtx_Print,
            &queue_Request_Ticket, &mtx_queue_Request_Ticket, &cv_TicketStand_start, &cv_TicketStand_finish,
            &queue_Request_Payment, &mtx_queue_Request_Payment, &cv_Payment_start, &cv_Payment_finish);

        vector_TicketStand.push_back(ticketStand); /* guardar objeto para mantener referencia */
        vector_thread_TicketStand.push_back(thread(ticketStand)); /* lanzar hilo en objeto y guardar referencia para que el colector no la elimine */

    }
    cout << "\033[1;37m [Main] Ticket stands are ready \033[0m\n";

    for (int i = 0; i < n_FoodStands; i++) {
        FoodStand foodStand(i, false, false, MAX_POPCORNS, MAX_DRINKS, 
            &queue_Request_Food, &queue_Request_Replenisher, &queue_Request_Payment, &mtx_queue_Request_Food,
            &mtx_queue_Request_Replenisher, &mtx_queue_Request_Payment, &cv_FoodStand_start, &cv_FoodStand_finish, 
            &cv_Replenisher_start, &cv_Replenisher_finish, &cv_Payment_start, &cv_Payment_finish, &mtx_Print);

        vector_FoodStand.push_back(foodStand);
        vector_thread_FoodStand.push_back(thread(foodStand));
    }
    cout << "\033[1;37m [Main] Food and Drink stands are ready \033[0m\n";

    for (int i = 0; i < n_Replenishers; i++) {
        Replenisher replenisher(i, &cv_Replenisher_start, &cv_Replenisher_finish, &queue_Request_Replenisher, &mtx_queue_Request_Replenisher, &mtx_Print);

        vector_Replenisher.push_back(replenisher);
        vector_thread_Replenisher.push_back(thread(replenisher));
    }
    cout << "\033[1;37m [Main] Replenishers are ready \033[0m\n";



}