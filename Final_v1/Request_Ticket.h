#pragma once

#include <string>

using namespace std;

class Client; /* Forward declaration de la clase cliente para evitar dependencias circulares entre la clase Client y Request_Ticket */

class Request_Ticket
{
private:
	int n_seats; /*Numero de asientos solicitados*/
	Client* client_pointer; /* Puntero al cliente que ha solicitado el ticket*/

public:
	Request_Ticket(int n_seats);
	Request_Ticket();
	~Request_Ticket();

	int getN_seats(); /* get para saber cuantos asientos hemos comprado */

	string toString_Id_client(); /* lo usaremos para devolver la info del cliente */

	void requestRejected();
	void requestAccepted();

	void this_is_my_client(Client* actual_client);

};

