#include <iostream>

#include <nerikiri/value.h>

#include <nerikiri/logger.h>
#include <nerikiri/functional.h>
#include <nerikiri/operation.h>

using namespace nerikiri;

Value Operation::call(const Value& value) {
    logger::trace("Operation({})::call({})", info().at("fullName"), value);
    std::lock_guard<std::mutex> lock(mutex_);
    return this->function_(value);
}