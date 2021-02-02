#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

#include "Request_Payment.h"

using namespace std;

class Payment_System
{
	private:
		mutex* mtx_print;
		condition_variable* cv_Payment_start;
		condition_variable* cv_Payment_finish;
		queue <Request_Payment>* queue_Request_Payment;
		mutex* mtx_queue_Request_Payment;

	public:
		Payment_System(mutex *mtx_print, condition_variable *cv_Payment_start, condition_variable * cv_Payment_finish, queue <Request_Payment> *queue_Request_Payment, mutex *mtx_queue_Request_Payment);
		~Payment_System();
		void operator()();
		void main();
};

