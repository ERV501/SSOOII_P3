#pragma once
#include <string>

using namespace std;

class Client;

class Request_Food
{
private:
	int n_drinks; /*Cantidad de bebidas solicitadas*/
	int n_popcorn; /*Cantidad de palomitas solicitadas*/
	Client* client_pointer; /* Puntero al cliente que ha solicitado el ticket*/

public:

	Request_Food();
	Request_Food(int n_drinks, int n_popcorn);
	~Request_Food();

	int getN_drinks();
	int getN_popcorn();

	string toString_Id_client(); /* lo usaremos para devolver la info del cliente */

	void paymentAccepted();
	void this_is_my_client(Client* actual_client);

};

