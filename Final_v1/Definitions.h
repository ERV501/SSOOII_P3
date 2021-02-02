#pragma once

#define N_TOTAL_SEATS 5
#define N_THREADS 100
#define MAX_DRINKS 10
#define MAX_POPCORNS 10

#define PRICE_SEAT 5.5
#define PRICE_POPCORN 2.5
#define PRICE_DRINK 1.5



enum class Type_of_Request { /*Tipos de peticiones*/
	ticket_request, supply_request
};