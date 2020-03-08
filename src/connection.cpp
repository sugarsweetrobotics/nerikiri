#include "nerikiri/connection.h"



using namespace nerikiri;

Connection::Connection(const ConnectionInfo& info, Broker_ptr broker) : info_(info), broker_(broker) {}

Connection::~Connection() {}
