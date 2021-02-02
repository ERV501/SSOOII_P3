#pragma once

#include "Request_Food.h"
#include "Request_Payment.h"

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

using namespace std;

class Request_Replenisher;

class FoodStand
{
private:

	/* datos de la peticion */
	int id_food_stand;
	bool replenishment_completed; /* para saber si la operacion de re-stock se ha completado */
	bool is_it_paid; /* para saber si la operacion de pago se ha completado */
	int n_popcorn;
	int n_drink;

	/* espera y respuesta peticion de comida y bebida */
	queue<Request_Food>* queue_Request_Food;
	mutex* mtx_queue_Request_Food;
	condition_variable* cv_FoodStand_start;
	condition_variable* cv_FoodStand_finish;

	/* espera y respuesta peticion de pago */
	queue<Request_Payment>* queue_Request_Payment_Food;
	mutex* mtx_queue_Request_Payment_Food;
	condition_variable* cv_Payment_start;
	condition_variable* cv_Payment_finish;

	/* espera y respuesta peticion al reponedor */
	queue<Request_Replenisher>* queue_Request_Replenisher;
	mutex* mtx_queue_Request_Replenisher;
	condition_variable* cv_Replenisher_start;
	condition_variable* cv_Replenisher_finish;

	/* control de impresion */
	mutex* mtx_Print;

public:
	FoodStand(int id_food_stand, bool replenishment_completed,bool is_it_paid, int n_popcorn, int n_drink, queue<Request_Food>* queue_Request_Food, queue<Request_Replenisher>* queue_Request_Replenisher, queue<Request_Payment>* queue_Request_Payment_Food, mutex* mtx_queue_Request_Food,mutex* mtx_queue_Request_Replenisher,mutex* mtx_queue_Request_Payment_Food,condition_variable* cv_FoodStand_start, condition_variable* cv_FoodStand_finish,condition_variable* cv_Replenisher_start, condition_variable* cv_Replenisher_finish, condition_variable* cv_Payment_start, condition_variable* cv_Payment_finish, mutex* mtx_Print);
	FoodStand();
	~FoodStand();
	void operator()();

	void main();

	void simulatePaymentTime();

	string get_Id_Stand();

	void stand_replenishment(int n_popcorn, int n_drink);

	void resetForNextReplenishment();
};

