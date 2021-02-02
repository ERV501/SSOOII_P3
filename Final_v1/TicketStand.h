#pragma once

#include <string>
#include <condition_variable>
#include <queue>
#include <mutex>

#include "Request_Ticket.h"
#include "Request_Payment.h"


using namespace std;

class TicketStand
{
	private:
		/* datos ticket_stand */
		int id_ticket_stand;
		int* g_seats_bought;
		bool is_it_paid; /*para saber que el pago se ha efectuado*/

		/* control de asientos e impresion */
		mutex* mtx_seats_bought;
		mutex* mtx_Print;

		/* espera y respuesta peticion de ticket */
		queue<Request_Ticket>* queue_Request_Ticket;
		mutex* mtx_queue_Request_Ticket_mtx;
		condition_variable* cv_TicketStand_start;
		condition_variable* cv_TicketStand_finish;


		/* espera y respuesta peticion de pago */
		queue<Request_Payment>* queue_Request_Payment_Ticket;
		mutex* mtx_queue_Request_Payment_Ticket;
		condition_variable* cv_Payment_start;
		condition_variable* cv_Payment_finish;

	public:
		TicketStand();

		TicketStand(int id_ticket_stand, int* g_seats_bought, bool is_it_paid,
			mutex* mtx_seats_bought, mutex* mtx_Print,
			queue<Request_Ticket>* queue_Request_Ticket, mutex* mtx_queue_Request_Ticket_mtx, condition_variable* cv_TicketStand_start, condition_variable* cv_TicketStand_finish,
			queue<Request_Payment>* queue_Request_Payment_Ticket, mutex* mtx_queue_Request_Payment_Ticket, condition_variable* cv_Payment_start, condition_variable* cv_Payment_finish);

		~TicketStand();

		void operator()();

		void main();

		void simulatePaymentTime();
		void resetForNextPayment();
		


		
};

