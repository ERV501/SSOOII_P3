#pragma once

#include "Definitions.h"

#include <string>

using namespace std;

class FoodStand;

class TicketStand;

class Request_Payment
{
	private:
		/* dinero a pagar */
		double price;
		/* referencias a los tipos de puestos */
		FoodStand* foodStand;
		TicketStand* ticketStand;
		/* Puntero al cliente que ha solicitado el pago*/
		string id_client_paid; /* Id del cliente que esta pagando*/

		
	public:
		Request_Payment(double price, FoodStand* foodStand, TicketStand* ticketStand);
		Request_Payment();
		~Request_Payment();

		double getPrice();

		TicketStand* getPaymentTypeTicket();
		FoodStand* getPaymentTypeFood();

		void SimulatePaymentFood();
		void SimulatePaymentTicket();

		void setPayingClient(string actual_id_client_paid);
		string getPayingClient();
};

