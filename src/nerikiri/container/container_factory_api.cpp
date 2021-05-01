
#include <nerikiri/container_factory_api.h>

using namespace nerikiri;

/* 
class NullContainerFactory : public ContainerFactoryAPI {
public:
  NullContainerFactory() : ContainerFactoryAPI("NullContainer", "null") {}
  virtual ~NullContainerFactory() {}

  virtual std::shared_ptr<Object> create(const std::string& fullName, const Value& info={}) const override{
    return nullContainer();
  }
};
*/

/*
using NullContainerFactory = NullFactory<ContainerAPI>;

std::shared_ptr<ContainerFactoryAPI> nerikiri::nullContainerFactory() {
    return std::make_shared<NullContainerFactory>();
}
*/