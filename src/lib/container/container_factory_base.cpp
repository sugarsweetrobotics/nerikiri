#include "container_factory_base.h"

using namespace juiz;

ContainerFactoryBase::ContainerFactoryBase(const std::string& className, const std::string& typeName, const std::string& fullName) : ContainerFactoryAPI(typeName, fullName) {
    logger::trace("ContainerFactoryBase::ContainerFactoryBase({}, {}, {}) called.", className, typeName, fullName);
}

ContainerFactoryBase::~ContainerFactoryBase() {}


void ContainerFactoryBase::setMeshData(const JUIZ_MESH_DATA& mesh) {
    
}

JUIZ_MESH_DATA ContainerFactoryBase::getMeshData() const {

}