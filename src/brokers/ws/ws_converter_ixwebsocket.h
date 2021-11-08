#pragma once
#include <vector>
#include "juiz/value.h"
#include "ixwebsocket/IXWebSocketMessage.h"

namespace juiz {

 template<typename T, typename K, typename V>
  std::vector<T> map(std::multimap<K, V>& map, std::function<T(const K&,V&)> func) {
    std::vector<T> ret;
    for(auto& [k, v] : map) {
      ret.emplace_back(func(k, v));
    }
    return ret;
  }

  /*
  inline juiz::Request convert(const juiz::ws::Request &req) {
    std::vector<juiz::Header> ret;
    for(const auto& [k, v] : req.headers) {
      ret.push_back(Header(k, v));
    }
    auto r = juiz::Request(req.method, req.body, ret, req.matches);
    for(auto p : req.params) {
      r.params[p.first] = p.second;
    }
    return r;
  }

  inline void apply(httplib::Response &response, juiz::Response &&r) {
    response.status = r.status;
    response.version = r.version;
    if(r.is_file_) {
      auto size = r.file_.tellg();

      response.body.resize(static_cast<size_t>(size));
      r.file_.read(&response.body[0], size);
    } else {
      response.set_content(r.body, r.contentType.c_str());
    }
  }
  */
  inline juiz::ws::Response convert(juiz::Value&& response) {
    //if (!response.hasKey("status")) {
    //  return juiz::ws::Response(404, "", "text/html");
    //}
    //if (!response["status"].isIntValue()) { 
    //  return juiz::ws::Response(404, "", "text/html");
    // }
    return juiz::ws::Response(200, response["body"], "application/json");
  }

  inline juiz::ws::Request apply(const ix::WebSocketMessagePtr & msg) {
    auto request = juiz::json::toValue(msg->str);
    if (request.hasKey("body")) {
      return juiz::ws::Request(request["url"].stringValue(), request["method"].stringValue(), request["body"]);
    } else {
      return juiz::ws::Request(request["url"].stringValue(), request["method"].stringValue(), {});
    }
  }

  inline std::string apply(juiz::ws::Response &&response) {
    std::stringstream ss;
    ss << "{\"status\":200,\"body\":" << juiz::json::toJSONString(response.body) << "}";
    return ss.str();
  }

}