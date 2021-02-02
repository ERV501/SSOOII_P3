#include "FoodStand.h"
#include "Request_Replenisher.h"
#include "Definitions.h"
#include <iostream>
#include <memory>


using namespace std;
/******************************************************* Puesto de comida y bebida *************************************************/
FoodStand::FoodStand() {}

FoodStand::FoodStand(int id_food_stand, bool replenishment_completed,bool is_it_paid, int n_popcorn, int n_drink, queue<Request_Food>* queue_Request_Food, queue<Request_Replenisher>* queue_Request_Replenisher, queue<Request_Payment>* queue_Request_Payment_Food,mutex* mtx_queue_Request_Food, mutex* mtx_queue_Request_Replenisher, mutex* mtx_queue_Request_Payment_Food, condition_variable* cv_FoodStand_start,condition_variable* cv_FoodStand_finish, condition_variable* cv_Replenisher_start,condition_variable* cv_Replenisher_finish, condition_variable* cv_Payment_start,condition_variable* cv_Payment_finish, mutex* mtx_Print) {

	/* datos de la peticion */
	this->id_food_stand = id_food_stand;
	this->replenishment_completed = false; /* al comienzo, no se rellena */
	this->is_it_paid = false; /* al comienzo, no esta pagado */
	this->n_popcorn = n_popcorn;
	this->n_drink = n_drink;

	/* espera y respuesta peticion de comida y bebida */
	this->queue_Request_Food = queue_Request_Food;
	this->mtx_queue_Request_Food = mtx_queue_Request_Food;
	this->cv_FoodStand_start = cv_FoodStand_start; /*cv de espera turno de puesto */
	this->cv_FoodStand_finish = cv_FoodStand_finish; /*cv para respuesta de turno del puesto*/

	/* espera y respuesta peticion de pago */
	this->queue_Request_Replenisher = queue_Request_Replenisher;
	this->mtx_queue_Request_Replenisher = mtx_queue_Request_Replenisher;
	this->cv_Replenisher_start = cv_Replenisher_start; /* espera turno */
	this->cv_Replenisher_finish = cv_Replenisher_finish; /* respuesta */

	/* espera y respuesta peticion al reponedor */
	this->queue_Request_Payment_Food = queue_Request_Payment_Food;
	this->mtx_queue_Request_Payment_Food = mtx_queue_Request_Payment_Food;
	this->cv_Payment_start = cv_Payment_start; /* espera turno */
	this->cv_Payment_finish = cv_Payment_finish;  /* respuesta */

	/* control de impresion */
	this->mtx_Print = mtx_Print;
}

FoodStand::~FoodStand() {}

void FoodStand::operator()() {

	main();

}

void FoodStand::main() {
	mutex mtx_Request_Food;
	mutex mtx_Request_Replenisher;
	mutex mtx_Request_Payment_completed;

	unique_lock<mutex> lk_print(*mtx_Print); /*mutex para controlar la impresion de mensajes*/

	Request_Food rq_food; /* objeto vacio para almacenar la peticion de la cola de peticiones */

	lk_print.unlock();

	while (1) { /* ejecucion continua*/

		unique_lock<mutex> lk_Request_Food(mtx_Request_Food); /*unique_lock para controlar las peticiones*/

		cv_FoodStand_start->wait(lk_Request_Food, [this] {return !queue_Request_Food->empty(); }); /*bloqueamos la cola cuando no haya ningun elemento*/

		unique_lock<mutex> lk_q_Request_Food(*mtx_queue_Request_Food);

		/*Dato disponible en la cola, el stand coge el 1º disponible (front) pq hay varios stands*/
		rq_food = queue_Request_Food->front(); /*guardamos dato*/
		queue_Request_Food->pop(); /*extraemos de la cola ese valor*/
		lk_q_Request_Food.unlock(); /*desbloqueamos semaforo*/

		if (rq_food.getN_popcorn() > n_popcorn || rq_food.getN_drinks() > n_drink) { /*comprobamos la disponibilidad*/

			lk_print.lock(); /*bloqueamos para garantizar acceso exclusivo a la impresion*/
			cout << "\033[1;32m [FoodStand: " << id_food_stand << "] Resupliying food and drinks \033[0m\n";
			lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/

			/* Peticion al reponedor (Request Replenisher) */
			Request_Replenisher rq_replenisher((FoodStand*)this); /* creamos una peticion con un puntero al puesto de comida que la necesita */
			unique_lock<mutex> lk_rq_replenisher(*mtx_queue_Request_Replenisher); /* Acceso exclusivo a cola de peticiones */
			queue_Request_Replenisher->push(rq_replenisher);
			lk_rq_replenisher.unlock(); /* Desbloquemos cola de peticiones*/

			/* Llamada a reponedor */
			cv_Replenisher_start->notify_one(); /* despertamos 1 reponedor */
			unique_lock<mutex> lk_replenisher_operation(mtx_Request_Replenisher);
			cv_Replenisher_finish->wait(lk_replenisher_operation, [this] {return replenishment_completed; }); /* esperamos a que se complete la reposicion */
			/* Reposicion completada */

			resetForNextReplenishment();
			
			lk_print.lock(); /*bloqueamos para garantizar acceso exclusivo a la impresion*/
			cout << "\033[1;32m [FoodStand: "<< id_food_stand << "] Food and drinks ressuplied \033[0m\n";
			lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/
		}

		lk_print.lock(); /*bloqueamos para garantizar acceso exclusivo a la impresion*/
		cout << "\033[1;32m [FoodStand: " << id_food_stand << "] Requesting payment for [Client: " << rq_food.toString_Id_client() << "] \033[0m\n";
		lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/

		/* peticion de pago */
		double price = ((rq_food.getN_popcorn()) * PRICE_POPCORN) + ((rq_food.getN_drinks()) * PRICE_DRINK);
		Request_Payment rq_payment = Request_Payment(price,(FoodStand*)this,NULL); /* Creacion de peticion de pago (de comida y bebida) con precio y puntero a nuestra oficina */
		unique_lock<mutex>lk_q_Request_Payment(*mtx_queue_Request_Payment_Food);
		rq_payment.setPayingClient(rq_food.toString_Id_client()); /* Set del id del cliente como string para imprimirlo en el pago */
		queue_Request_Payment_Food->push(rq_payment);
		lk_q_Request_Payment.unlock();

		cv_Payment_start->notify_one(); /* despertamos pago */

		/* esperamos pago */
		unique_lock<mutex> lk_Payment_completed(mtx_Request_Payment_completed);
		
		lk_print.lock(); /*bloqueamos para garantizar acceso exclusivo a la impresion*/
		cout << "\033[1;32m [FoodStand: " << id_food_stand << "] Payment complete succesfully for [Client: " << rq_food.toString_Id_client() << "] \033[0m\n";

		lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/

		
		/* esperamos que el pago se complete */
		cv_Payment_finish->wait(lk_Payment_completed, [this] {return is_it_paid; });

		/* realizamos cambios en el stock */
		n_popcorn = n_popcorn - rq_food.getN_popcorn();
		n_drink = n_drink - rq_food.getN_drinks();

		/* estado de la peticion = aceptado */
		rq_food.paymentAccepted();
		cv_FoodStand_finish->notify_all();
	}
}

void FoodStand::simulatePaymentTime() {
	this_thread::sleep_for(chrono::seconds( rand()%3 + 1 )); /* simular espera de pago */
	is_it_paid = true; /* estado = pagado */
}

string FoodStand::get_Id_Stand() {
	string msg = "[FoodStand: " + to_string(id_food_stand) + "]";
	return msg;
}

void FoodStand::stand_replenishment(int n_popcorn, int n_drink) {
	this->n_popcorn += n_popcorn;
	this->n_drink += n_drink;
	replenishment_completed = true;
}

void FoodStand::resetForNextReplenishment() {
	replenishment_completed = false;
}