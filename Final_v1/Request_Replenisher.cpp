#include "Request_Replenisher.h"

using namespace std;

Request_Replenisher::Request_Replenisher(FoodStand* foodstand_pointer) {
	this->foodstand_pointer = foodstand_pointer; /* para saber a que puesto de comida hay que responder */ /* referencia al objeto en si */
}

Request_Replenisher::Request_Replenisher(){}

Request_Replenisher::~Request_Replenisher(){}

string Request_Replenisher::toString_Id_stand() {
	return (foodstand_pointer->get_Id_Stand());
}

void Request_Replenisher::simulateReplenishment(int n_popcorn, int n_drink) {
	foodstand_pointer->stand_replenishment(n_popcorn, n_drink);
}