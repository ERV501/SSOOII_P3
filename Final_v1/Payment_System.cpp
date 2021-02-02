#include "Payment_System.h"
#include <iostream>

using namespace std;

Payment_System::Payment_System(mutex *mtx_print, condition_variable* cv_Payment_start, condition_variable* cv_Payment_finish, queue <Request_Payment>* queue_Request_Payment, mutex* mtx_queue_Request_Payment) {
	this->mtx_print = mtx_print;
	this->cv_Payment_start = cv_Payment_start;
	this->cv_Payment_finish = cv_Payment_finish;
	this->queue_Request_Payment = queue_Request_Payment;
	this->mtx_queue_Request_Payment = mtx_queue_Request_Payment;
}

Payment_System::~Payment_System(){}

void Payment_System::operator()() {
	main();
}

void Payment_System::main() {

	Request_Payment rq_paymet; /* peticion de pago a recoger */
	mutex mtx_Request_Payment_ready; /* garantizar el acceso exclusivo a los pagos */

	unique_lock<mutex>lk_Print(*mtx_print);
	lk_Print.unlock();

	while(true){
		unique_lock<mutex> lk_ready_Request_Payment(mtx_Request_Payment_ready);
		cv_Payment_start->wait(lk_ready_Request_Payment, [this] {return !queue_Request_Payment->empty(); });

		unique_lock<mutex> lk_queue_Request_Payment(*mtx_queue_Request_Payment);
		rq_paymet = queue_Request_Payment->front();
		queue_Request_Payment->pop();
		lk_queue_Request_Payment.unlock();

		lk_Print.lock(); /*bloqueamos para garantizar acceso exclusivo a la impresion*/
		cout << "\033[1;37m [Payment System] "<< rq_paymet.getPrice()<<" euros have been paid by [Client: " << rq_paymet.getPayingClient() << "] \033[0m\n";
		lk_Print.unlock(); /*desbloqueamos para el siguiente hilo*/

		if (rq_paymet.getPaymentTypeTicket() != NULL) {
			rq_paymet.SimulatePaymentTicket();
		}
		else if (rq_paymet.getPaymentTypeFood() != NULL) {
			rq_paymet.SimulatePaymentFood();
		}

		cv_Payment_finish->notify_all();
	}

}