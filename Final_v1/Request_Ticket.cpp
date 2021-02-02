#include "Request_Ticket.h"
#include "Client.h"

using namespace std;

Request_Ticket::Request_Ticket(int n_seats) {
	this->n_seats = n_seats;
}

Request_Ticket::Request_Ticket() {}

Request_Ticket::~Request_Ticket() {}

int Request_Ticket::getN_seats() {return n_seats;}

string Request_Ticket::toString_Id_client() {
	return to_string(client_pointer->getId_client());
}

void Request_Ticket::this_is_my_client(Client *actual_client) {
	this->client_pointer = actual_client;/* para saber a que cliente hay que responder */ /* referencia al objeto en si */
}

void Request_Ticket::requestAccepted() {
	client_pointer->allow_Rq_Ticket();
}

void Request_Ticket::requestRejected() {
	client_pointer->deny_Rq_Ticket();
}