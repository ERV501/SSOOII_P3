#include "TicketStand.h"

#include <iostream>

using namespace std;

TicketStand::TicketStand() {};

TicketStand::TicketStand(int id_ticket_stand, int* g_seats_bought, bool is_it_paid,
						mutex* mtx_seats_bought, mutex* mtx_Print,
						queue<Request_Ticket>* queue_Request_Ticket, mutex* mtx_queue_Request_Ticket_mtx, condition_variable* cv_TicketStand_start, condition_variable* cv_TicketStand_finish,
						queue<Request_Payment>* queue_Request_Payment_Ticket, mutex* mtx_queue_Request_Payment_Ticket, condition_variable* cv_Payment_start, condition_variable* cv_Payment_finish) {

	/* datos ticket_stand */
	this->id_ticket_stand = id_ticket_stand;
	this->g_seats_bought = g_seats_bought; /* asientos que ya no están disponibles */
	this->is_it_paid = false; /* para saber que el pago se ha efectuado */

	/* control de asientos e impresion */
	this->mtx_seats_bought= mtx_seats_bought;
	this->mtx_Print = mtx_Print;

	/* espera y respuesta peticion de ticket */
	this->queue_Request_Ticket= queue_Request_Ticket;
	this->mtx_queue_Request_Ticket_mtx= mtx_queue_Request_Ticket_mtx;
	this->cv_TicketStand_start= cv_TicketStand_start;
	this->cv_TicketStand_finish= cv_TicketStand_finish;


	/* espera y respuesta peticion de pago */
	this->queue_Request_Payment_Ticket= queue_Request_Payment_Ticket;
	this->mtx_queue_Request_Payment_Ticket = mtx_queue_Request_Payment_Ticket;
	this->cv_Payment_start = cv_Payment_start;
	this->cv_Payment_finish = cv_Payment_finish;
}

TicketStand::~TicketStand() {};

void TicketStand::operator()() {
	main();
}

void TicketStand::main() {
	Request_Ticket rq_ticket; /* objeto vacio para almacenar la peticion de la cola de peticiones */

	mutex mtx_ticket_ready;
	mutex mtx_payment_completed;

	unique_lock<mutex> lk_print(*mtx_Print);
	lk_print.unlock(); /* desbloqueamos impresion */

	int total_seats_bought = 0; /* llevaremos la cuenta del total de asientos comprados */

	while (1) { /* ejecucion continua*/

		unique_lock<mutex> lk_ticket_ready(mtx_ticket_ready); /*unique_lock para controlar los tickets */

		cv_TicketStand_start->wait(lk_ticket_ready, [this] {return !queue_Request_Ticket->empty(); }); /*bloqueamos la cola cuando no haya algun elemento, se desbloquea si hay */

		unique_lock<mutex> lk_q_Request_Ticket(*mtx_queue_Request_Ticket_mtx);

		/*Dato disponible en la cola, el stand coge el 1º disponible (front) pq hay varos stands*/
		rq_ticket = queue_Request_Ticket->front(); /*guardamos dato*/
		queue_Request_Ticket->pop(); /*extraemos de la cola ese valor*/
		lk_q_Request_Ticket.unlock(); /*desbloqueamos semaforo*/

		unique_lock<mutex> lk_seats_bought(*mtx_seats_bought); /* lo usamos para controlar la operacion de compra de asientos, se crea dentro del while para hacer lock en cada creacion */

		total_seats_bought = *g_seats_bought + rq_ticket.getN_seats(); /* [variable compartida] asientos_no_disponibles = asientos_cogidos_anteriormente + asientos_comprados_ahora */

		/*comprobamos la disponibilidad de asientos*/

		if (N_TOTAL_SEATS < total_seats_bought){ /* NO hay suficientes asientos disponibles */

			lk_print.lock(); /*bloqueamos para garantizar acceso exclusivo a la impresion*/
			cout << "\033[1;36m [TicketStand: " << to_string(id_ticket_stand) << "] Not enough seats available for [Client: "<< rq_ticket.toString_Id_client() << "] \033[0m\n";
			lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/

			rq_ticket.requestRejected();

		}else { /* SI hay suficientes asientos disponibles */

			Request_Payment rq_payment = Request_Payment((rq_ticket.getN_seats() * PRICE_SEAT), NULL, ((TicketStand*)this)); /* enviamos peticion de pago de entradas con precio_por_asiento y puntero a nuestra oficina*/
			
			/* peticion de pago */
			lk_print.lock(); /*bloqueamos para garantizar acceso exclusivo a la impresion*/
			cout << "\033[1;36m [TicketStand: " << to_string(id_ticket_stand) << "] Requesting payment for [Client: " << rq_ticket.toString_Id_client() << "] \033[0m\n";
			lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/

			unique_lock<mutex> lk_queue_Request_Payment(*mtx_queue_Request_Payment_Ticket); /* controlar acceso a la cola de peticiones de pago */
			rq_payment.setPayingClient(rq_ticket.toString_Id_client()); /* Set del id del cliente como string para imprimirlo en el pago */
			queue_Request_Payment_Ticket->push(rq_payment); /* metemos peticon en cola */
			lk_queue_Request_Payment.unlock();

			cv_Payment_start->notify_one();

			unique_lock<mutex> lk_Request_Payment_Completed(mtx_payment_completed);

			cv_Payment_finish->wait(lk_Request_Payment_Completed, [this] {return is_it_paid; }); /* bloqueamos hasta que se realice el pago */

			rq_ticket.requestAccepted(); /* aceptamos peticion */

			/* pago acepado */
			lk_print.lock(); /*bloqueamos para garantizar acceso exclusivo a la impresion*/
			cout << "\033[1;36m [TicketStand: " << to_string(id_ticket_stand) << "] Payment successful for [Client: " << rq_ticket.toString_Id_client() << "] \033[0m\n";
			lk_print.unlock(); /*desbloqueamos para el siguiente hilo*/

			resetForNextPayment(); /* resetear estado */
		}

		lk_seats_bought.unlock(); /* desbloqueamos sistema de compra de asientos */

		cv_TicketStand_finish->notify_all(); /* notificar a todos y luego cada hilo identifica si esta disponible o no */
	}
}

void TicketStand::simulatePaymentTime() {
	this_thread::sleep_for(chrono::seconds(rand() % 3 + 1)); /* simular espera de pago */
	is_it_paid = true; /* estado = pagado */
}

void TicketStand::resetForNextPayment() {
	is_it_paid = false;
}