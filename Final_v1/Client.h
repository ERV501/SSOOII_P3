#pragma once

#include "Request_Ticket.h"
#include "Request_Food.h"

#include <queue>
#include <mutex>
#include <iostream>


using namespace std;

class Client
{

private:

	int id_client; /*identificador cliente*/

	mutex* mtx_Print; /* para la impresion */

	queue<Request_Food> *queue_Request_Food; /* referente a las peticiones de comida y bebida */
	mutex* mtx_queue_Request_Food;
	condition_variable* cv_FoodStand_start;
	condition_variable* cv_FoodStand_finish;
	Request_Food rq_food;
	bool food_transaction_completed;

	queue<Request_Ticket>* queue_Request_Ticket; /* referente a las peticiones de entradas */
	mutex* mtx_queue_Request_Ticket_mtx;
	condition_variable* cv_TicketStand_start;
	condition_variable* cv_TicketStand_finish;
	Request_Ticket rq_ticket;
	bool ticket_transaction_completed;
	bool ticket_transaction_accepted;


public:
	Client();

	Client(int id_client, mutex* mtx_Print,
		queue<Request_Food>* queue_Request_Food,
		mutex* mtx_queue_Request_Food,
		condition_variable* cv_FoodStand_start,
		condition_variable* cv_FoodStand_finish,
		Request_Food rq_food,
		bool food_transaction_completed,
		queue<Request_Ticket>* queue_Request_Ticket,
		mutex* mtx_queue_Request_Ticket_mtx,
		condition_variable* cv_TicketStand_start,
		condition_variable* cv_TicketStand_finish,
		Request_Ticket rq_ticket,
		bool ticket_transaction_completed,
		bool ticket_transaction_accepted);

	~Client();
	void operator()();

	void main();
	int getId_client(); /* necesario para imprimir id cliente */
	
	void set_this_is_my_client(); /* comunicar a las peticiones que cliente somos */

	void allow_Rq_Ticket();
	void deny_Rq_Ticket();
	void allow_Rq_Food();

};

