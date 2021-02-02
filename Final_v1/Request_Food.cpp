#include "Request_Food.h"
#include "Client.h"

Request_Food::Request_Food() {}

Request_Food::Request_Food(int n_drinks, int n_popcorn) {
	this->n_drinks = n_drinks;
	this->n_popcorn = n_popcorn;
	
}

Request_Food::~Request_Food() {}

string Request_Food::toString_Id_client() {
	return to_string(client_pointer->getId_client());
}

int Request_Food::getN_drinks() { return n_drinks; }
int Request_Food::getN_popcorn() { return n_popcorn; }

void Request_Food::paymentAccepted() {
	client_pointer->allow_Rq_Food();
}

void Request_Food::this_is_my_client(Client* actual_client) {
	this->client_pointer = actual_client; /* para saber a que cliente hay que responder: referencia al objeto en si */
}