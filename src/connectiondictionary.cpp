#include "nerikiri/logger.h"
#include "nerikiri/connectiondictionary.h"

using namespace nerikiri;

void ConnectionDictionary::add(Connection_ptr&& connection) {
  logger::trace("ConnectionDictionary::add({})", connection->info().at("name").stringValue());
  dictionary_.emplace(connection->info().at("name").stringValue(), std::move(connection));
  logger::trace("ConnectionDictionary::add ends");
}

void ConnectionDictionary::foreach(std::function<void(Connection_ptr&)> func) {
  logger::trace("ConnectionDictionary::foreach()");
  for(auto& [name, connection] : dictionary_) {
    logger::trace(" - for connection {}", str(connection->info()));
    func(connection);
  }
  logger::trace("ConnectionDictionary::foreach ends");
}
    

