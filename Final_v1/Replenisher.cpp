#include "Replenisher.h"
#include <string>
#include <iostream>
#include <memory>
#include <chrono>

using namespace std;

Replenisher::Replenisher(int id_replenisher,condition_variable* cv_Replenisher_start, condition_variable* cv_Replenisher_finish, queue<Request_Replenisher>* queue_Request_Replenisher, mutex* mtx_queue_Request_Replenisher, mutex* mtx_print) {

	this->id_replenisher = id_replenisher;
	this->cv_Replenisher_start = cv_Replenisher_start; /* espera */
	this->cv_Replenisher_finish = cv_Replenisher_finish; /* respuesta */
	this->queue_Request_Replenisher = queue_Request_Replenisher;
	this->mtx_queue_Request_Replenisher = mtx_queue_Request_Replenisher;
	this->mtx_print = mtx_print;
}

Replenisher::~Replenisher() {}

void Replenisher::operator()() {
	main();
}

void Replenisher::main() {

	mutex replenisher_available_mtx;
	unique_lock<mutex> lk_print(*mtx_print);
	lk_print.unlock();

	Request_Replenisher rq_replenisher; /* objeto vacio para almacenar la peticion de la cola de peticiones */

	while (1) {
		unique_lock<mutex> replenisher_available_lk(replenisher_available_mtx);
		cv_Replenisher_start->wait(replenisher_available_lk, [this] {return !queue_Request_Replenisher->empty(); }); /* bloquear cuando no haya ningun elemento */

		rq_replenisher = queue_Request_Replenisher->front(); /*guardamos dato*/
		queue_Request_Replenisher->pop(); /*extraemos de la cola ese valor*/
		replenisher_available_lk.unlock(); /*desbloqueamos semaforo*/

		lk_print.lock();/*bloqueamos para garantizar acceso exclusivo a la impresion*/
		cout << "\033[1;35m [Replenisher: " << id_replenisher << "] Resupliying for stand " << rq_replenisher.toString_Id_stand() << " \033[0m\n";
		lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/

		this_thread::sleep_for(chrono::seconds(rand() % 8 + 1));

		lk_print.lock();/*bloqueamos para garantizar acceso exclusivo a la impresion*/
		cout << "\033[1;35m [Replenisher: " << id_replenisher << "] Finished resupliying for stand " << rq_replenisher.toString_Id_stand() << " \033[0m\n";
		lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/

		rq_replenisher.simulateReplenishment(MAX_POPCORNS, MAX_DRINKS);

		cv_Replenisher_finish->notify_all();
	}

}