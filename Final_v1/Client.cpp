#include "Client.h"

using namespace std;

Client::Client() {}

Client::Client(int id_client, mutex* mtx_print,queue<Request_Food>* queue_Request_Food,mutex* mtx_queue_Request_Food,condition_variable* cv_FoodStand_start,condition_variable* cv_FoodStand_finish,Request_Food rq_food,bool food_transaction_completed,queue<Request_Ticket>* queue_Request_Ticket,mutex* mtx_queue_Request_Ticket_mtx,condition_variable* cv_TicketStand_start,condition_variable* cv_TicketStand_finish,Request_Ticket rq_ticket,bool ticket_transaction_completed, bool ticket_transaction_accepted) {
	
	this->id_client = id_client;

	this->mtx_Print = mtx_print; /* para la impresion */

	this->queue_Request_Food = queue_Request_Food; /* referente a las peticiones de comida y bebida */
	this->mtx_queue_Request_Food = mtx_queue_Request_Food;
	this->cv_FoodStand_start = cv_FoodStand_start;
	this->cv_FoodStand_finish = cv_FoodStand_finish;
	this->rq_food = rq_food;
	this->food_transaction_completed = food_transaction_completed;

	this->queue_Request_Ticket = queue_Request_Ticket; /* referente a las peticiones de entradas */
	this->mtx_queue_Request_Ticket_mtx = mtx_queue_Request_Ticket_mtx;
	this->cv_TicketStand_start = cv_TicketStand_start;
	this->cv_TicketStand_finish = cv_TicketStand_finish;
	this->rq_ticket = rq_ticket;
	this->ticket_transaction_completed = ticket_transaction_completed;
	this->ticket_transaction_accepted = ticket_transaction_accepted;
}

Client::~Client() {}

void Client::operator()() {
	main();
}

void Client::main() {

	set_this_is_my_client(); /* determinar puntero del cliente a las peticiones correspondientes */
	
	unique_lock<mutex> lk_print(*mtx_Print); /*mutex para controlar la impresion de mensajes*/
	lk_print.unlock();
				
	lk_print.lock(); /*bloqueamos para garantizar acceso exclusivo a la impresion*/
	cout << "\033[1;33m [Client: " << to_string(id_client) << "] Queuing for \"" << rq_ticket.getN_seats() << " tickets\" \033[0m\n";
	lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/

	/* Operacion tickets */
	unique_lock<mutex> lk_queue_Request_Tickets(*mtx_queue_Request_Ticket_mtx);
	queue_Request_Ticket->push(rq_ticket);
	lk_queue_Request_Tickets.unlock();

	cv_TicketStand_start->notify_one(); /* Iniciar turno taquilla */

	mutex mtx_Tickets_Ready;
	unique_lock<mutex> lk_Tickets_Ready(mtx_Tickets_Ready);
	cv_TicketStand_finish->wait(lk_Tickets_Ready, [this] {return ticket_transaction_completed; });/* Esperar respuesta tickets */

	if (!ticket_transaction_accepted) {
		lk_print.lock(); /*bloqueamos para garantizar acceso exclusivo a la impresion*/
		cout << "\033[1;33m [Client: " << to_string(id_client) << "] Ticket request failed and client walks away\033[0m\n";
		lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/
		exit; /* finalizar ejecucion hilo */
	}
	
	this_thread::sleep_for(chrono::seconds(rand() % 5 + 1));
	 /* Fin operacion tickets */

	/* Operacion comida y bebida */
	unique_lock<mutex> lk_queue_Request_FoodStand(*mtx_queue_Request_Food);
	queue_Request_Food->push(rq_food);
	lk_queue_Request_FoodStand.unlock();

	cv_FoodStand_start->notify_one();

	lk_print.lock(); /*bloqueamos para garantizar acceso exclusivo a la impresion*/
	cout << "\033[1;33m [Client: " << to_string(id_client) << "] Queuing for \""<< rq_food.getN_popcorn() <<" popcorns and "<< rq_food.getN_drinks() <<" drinks\" \033[0m\n";
	lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/

	mutex mtx_Food_Ready;
	unique_lock<mutex> lk_Food_Ready(mtx_Food_Ready);
	cv_FoodStand_finish->wait(lk_Tickets_Ready, [this] {return food_transaction_completed; });/* Esperar respuesta tickets */

	lk_print.lock(); /*bloqueamos para garantizar acceso exclusivo a la impresion*/
	cout << "\033[1;33m [Client: " << to_string(id_client) << "] Is served and enters the movie theatre \033[0m\n";
	lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/
	/* Operacion comida y bebida */

}

int Client::getId_client(){
	return id_client;
}

void Client::set_this_is_my_client() {
	this->rq_food.this_is_my_client((Client*)this);
	this->rq_ticket.this_is_my_client((Client*)this);
}

void Client::allow_Rq_Ticket() {
	ticket_transaction_accepted = true;
	ticket_transaction_completed = true;
}

void Client::deny_Rq_Ticket() {
	ticket_transaction_accepted = false;
	ticket_transaction_completed = true;
}

void Client::allow_Rq_Food() {
	food_transaction_completed = true;
}

