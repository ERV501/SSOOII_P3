#include "Request_Payment.h"
#include "FoodStand.h"
#include "TicketStand.h"

#include <string>


using namespace std;

Request_Payment::Request_Payment(double price, FoodStand* foodStand,TicketStand* ticketStand) {
	this->price = price;
	this->foodStand = foodStand;
	this->ticketStand = ticketStand;

}

Request_Payment::Request_Payment() {}

Request_Payment::~Request_Payment(){}

double Request_Payment::getPrice() { return price; } /* precio a pagar */

TicketStand * Request_Payment::getPaymentTypeTicket() {
	return ticketStand;
}

FoodStand * Request_Payment::getPaymentTypeFood() {
	return foodStand;
}

void Request_Payment::SimulatePaymentFood() {
	foodStand->simulatePaymentTime();
}

void Request_Payment::SimulatePaymentTicket() {
	ticketStand->simulatePaymentTime();
}

void Request_Payment::setPayingClient(string actual_id_client_paid) {
	this->id_client_paid = actual_id_client_paid; /* para saber a que cliente hay que responder */
}

string Request_Payment::getPayingClient() {
	return id_client_paid;
}

