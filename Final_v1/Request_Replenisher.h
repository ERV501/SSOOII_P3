#pragma once
#include "FoodStand.h"
#include <string>

using namespace std;

class FoodStand; /* Forwarsd declaration de la clase FoodStand para evitar dependencias circulares con Request_Replenisher */

class Request_Replenisher{

	private:
		FoodStand *foodstand_pointer; /* Puntero al puesto de comida que ha solicitado al reponedor */

	public:
		Request_Replenisher(FoodStand *foodstand_pointer);
		~Request_Replenisher();
		Request_Replenisher();

		string toString_Id_stand(); /* lo usaremos para devolver la info del stand de comida y bebida */

		void simulateReplenishment(int n_popcorn, int n_drink);

};

