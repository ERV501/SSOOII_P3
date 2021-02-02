#pragma once
#include "Request_Replenisher.h"
#include <condition_variable>
#include <queue>
#include <mutex>

using namespace std;

class Replenisher
{
private:
	int id_replenisher;

	mutex *mtx_print;

	condition_variable* cv_Replenisher_start;
	condition_variable* cv_Replenisher_finish; /* respuesta al stand */

	queue<Request_Replenisher> * queue_Request_Replenisher;
	mutex* mtx_queue_Request_Replenisher;

public:
	Replenisher(int id_replenisher, condition_variable* cv_Replenisher_start, condition_variable* cv_Replenisher_finish, queue<Request_Replenisher>* queue_Request_Replenisher, mutex* mtx_queue_Request_Replenisher, mutex* mtx_print);
	~Replenisher();
	void operator()();
	void main();
};

