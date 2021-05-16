#pragma once

#include <juiz/value.h>

namespace nerikiri {

  /**
   * Buffer for Operation's Input
   */
  class NewestValueBuffer {
  private:
    bool default_;
    Value buffer_;
    Value defaultValue_;
  public:
    NewestValueBuffer(): default_(true) {}
    NewestValueBuffer(const Value& defaultValue) : buffer_(defaultValue), defaultValue_(defaultValue), default_(true) {}

    virtual ~NewestValueBuffer() {}
  public:

    virtual Value push(const Value& v) { buffer_ = (v); default_ = false; return buffer_; }

    virtual Value push(Value&& v) { buffer_ = std::move(v); default_ = false; return buffer_; }

    virtual Value pop() const { return default_ ? defaultValue_ : buffer_; }

    virtual Value& popRef() { return default_ ? defaultValue_ : buffer_; }

    virtual Value defaultValue() const { return defaultValue_; }
    //virtual bool isEmpty() const { return empty_; }
  };
}