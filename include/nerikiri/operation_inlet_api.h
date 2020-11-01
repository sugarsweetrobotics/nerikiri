#pragma once 

//#include <nerikiri/connection_api.h>

#include <nerikiri/logger.h>

namespace nerikiri {

  class ConnectionAPI;

  class OperationInletAPI {
  public:
    virtual ~OperationInletAPI() {}
  public:
    virtual std::string name() const = 0;

    virtual Value defaultValue() const = 0;
    
    virtual Value get() const = 0;

    virtual Value put(const Value& value) = 0;

    // virtual Value execute(const Value& value) = 0;

    /**
     * いまの使用は，pull型接続があると，それを優先してデータを取得するが，
     * そうでなければバッファーの値を使う．バッファーは入力側から書き込まれるので，早い者勝ちになる
     */
    // virtual Value collectValues() = 0;

    virtual bool isUpdated() const = 0;

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const = 0;

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) = 0;
    
    virtual Value removeConnection(const std::string& _fullName) = 0;

    //virtual Value invokeOperation() = 0;

   // virtual std::shared_ptr<OperationAPI> owner() const = 0;
  };

  class NullOperationInlet : public OperationInletAPI {
  public:
    virtual ~NullOperationInlet() {}

    virtual std::string name() const override {
      logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__);
      return "null";
    }

    virtual Value defaultValue() const override {
      return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }
    
    virtual Value get() const override {
      return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }

    virtual Value put(const Value& value) override {
      return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }

    /*
    virtual Value execute(const Value& value) override {
      return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }
    

    virtual Value collectValues() override {
      return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }
    
    */
    virtual bool isUpdated() const override {
      logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__);
      return false;
    }

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override {
      logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__);
      return {};
    }

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) override {
      return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }
    
    virtual Value removeConnection(const std::string& _fullName) override {
      return Value::error(logger::error("NullOperationInlet::{}() failed. OperationInlet is null.", __func__));
    }

  };
}