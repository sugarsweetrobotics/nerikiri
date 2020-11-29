
#include <nerikiri/container_factory_api.h>

using namespace nerikiri;

    class NullContainerFactory : public ContainerFactoryAPI {
    public:
      NullContainerFactory() : ContainerFactoryAPI("NullContainerFactory", "NullContainer", "null") {}
      virtual ~NullContainerFactory() {}

      virtual std::shared_ptr<ContainerAPI> create(const std::string& fullName) {
        return nullContainer();
      }

      //virtual Value addOperationFactory(const std::shared_ptr<ContainerOperationFactoryBase>& cof) {
      //  return Value::error(logger::warn("NullContainerFactory::addOperationFactory() called. ContainerFactory is null."));
     // }
    };

std::shared_ptr<ContainerFactoryAPI> nerikiri::nullContainerFactory() {
    return std::make_shared<NullContainerFactory>();
}