#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <memory>

namespace nerikiri {


  class ResultBase {
  public:
    ResultBase() {}  
    virtual ~ResultBase() {}
  };

  template<typename R>
  class Result : public ResultBase {
  private:
      R value_;
      bool matched_;
  public:
      Result(const bool matched, const R& value): matched_(matched), value_(value) {}
      virtual ~Result() {}

      R value() {
        return value_;
      }
  };

  class OptionBase {
  protected:
    std::string short_;
    std::string long_;
    std::string help_;
    bool required_;
    std::string key_;
    bool matched_;
  public:
    OptionBase(const std::string& shorts, const std::string& longs, const std::string& help, const std::string& key, const bool required):
      short_(shorts), long_(longs), help_(help), key_(key), required_(required), matched_(false) {}
    virtual ~OptionBase() {}

    std::string key() const { return key_; }
    bool matched() const { return matched_; }

    bool match(const std::string& str) {
        if (str == short_) {
            matched_ = true;
            return true;
        } else if (str == long_) {
            matched_ = true;
            return true;
        }
        return false;
    }


  public:
     virtual std::shared_ptr<ResultBase> makeResult(std::vector<std::string>::iterator& it, const std::vector<std::string>::const_iterator& end) = 0;
     virtual std::shared_ptr<ResultBase> makeDefaultResult() = 0;
  };

  template<typename T>
  class ValueOption : public OptionBase {
  private:
    T defaultValue_;
    
  public:
    ValueOption(const std::string& shorts, const std::string& longs, const std::string& help, const std::string& key, const bool required, const T& defaultValue): 
      OptionBase(shorts, longs, help, key, required), defaultValue_(defaultValue) {}
    virtual ~ValueOption() {}

    virtual std::shared_ptr<ResultBase> makeResult(std::vector<std::string>::iterator& it, const std::vector<std::string>::const_iterator& end) {
      ++it;
      std::istringstream ss(*it);
      T value;
      ss >> value;
      return std::make_shared<Result<T>>(true, value);
    }

    virtual std::shared_ptr<ResultBase> makeDefaultResult() {
        return std::make_shared<Result<T>>(false, defaultValue_);
    }
  };
  
  template<typename T>
  std::shared_ptr<OptionBase> valueOption(const std::string& shorts, const std::string& longs, const std::string& help, const std::string& key, const bool required, const T& defaultValue) {
      return std::make_shared<ValueOption<T>>(shorts, longs, help, key, required, defaultValue);
  }

  template<const char*>
  std::shared_ptr<OptionBase> valueOption(const std::string& shorts, const std::string& longs, const std::string& help, const std::string& key, const bool required, const char* defaultValue) {
      return std::make_shared<ValueOption<std::string>>(shorts, longs, help, key, required, defaultValue);
  }

  class FlagOption : public OptionBase {
  private:
  public:
    FlagOption(const std::string& shorts, const std::string& longs, const std::string& help, const std::string& key, const bool required): 
      OptionBase(shorts, longs, help, key, required) {}
    virtual ~FlagOption() {}

    virtual std::shared_ptr<ResultBase> makeResult(std::vector<std::string>::iterator& it, const std::vector<std::string>::const_iterator& end) {
      return std::shared_ptr<ResultBase>(new Result<bool>(true, true));
    }

    virtual std::shared_ptr<ResultBase> makeDefaultResult() {
        return std::make_shared<Result<bool>>(false, false);
    }
  };

  class OptionNotFoundException : public std::exception {

  };

  class Options {
  public:
    std::map<std::string, std::shared_ptr<ResultBase>> results;
    bool error_;
    std::vector<std::string> unknown_args;
  public:
    void error(bool flag) { error_ = flag; }

    void emplace(std::pair<std::string, std::shared_ptr<ResultBase>>&& pair) { 
        results.emplace(std::move(pair));
    }

    template<typename T>
    T get(const std::string& key) {
        auto base = std::static_pointer_cast<Result<T>>(results[key]);
        if (base) {
            return base->value();
        }
        throw OptionNotFoundException();
    }
  };

  class ArgParser {
  private:
    std::vector<std::shared_ptr<OptionBase>> options_;

    bool checkMatch(Options& options, std::vector<std::string>::iterator& it, const std::vector<std::string>::const_iterator& end) {
        for(auto& option: options_) {
            if (option->matched()) continue;

            if (option->match(*it)) {
                auto result = option->makeResult(it, end);
                if (!result) {
                    return false;
                }
                options.results.emplace(option->key(), std::move(result));
                return true;
            }
        }
        return false;
    }
  public:
    template<typename T>
    void option(const std::string& shorts, const std::string& longs, const std::string& help, const bool required, const T& defaultValue) {
        std::string key = longs.substr(2);
        options_.push_back(std::shared_ptr<OptionBase>(valueOption<T>(shorts, longs, help, key, required, defaultValue)));
    }

    void option(const std::string& shorts, const std::string& longs, const std::string& help, const bool required) {
        std::string key = longs.substr(2);
        options_.push_back(std::shared_ptr<OptionBase>(new FlagOption(shorts, longs, help, key, required)));
    }
  public:
    Options parse(const int argc, const char** argv) {
        std::vector<std::string> args;
        for(int i = 0;i < argc;i++) {
            args.push_back(std::string(argv[i]));
        }
        return parse(std::move(args));
    }

    Options parse(std::vector<std::string>&& args) {
        Options options;
        auto it = args.begin();
        ++it; // first argument is program name;
        for(;it != args.end();) {
            if (*it == "--") {
                ++it;
                break;
            }
            if(!checkMatch(options, it, args.end())) {
                // Unknown Argument
                options.error(true);
                break;
            }
            ++it;
        }
        for(auto op : options_) {
            if (!op->matched()) {
                options.results.emplace(op->key(), std::move(op->makeDefaultResult()));
            }
        }
        for(;it != args.end();++it) {
            options.unknown_args.push_back(*it);
        }

        return options;
    }
    
  };



}