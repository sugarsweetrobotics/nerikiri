
#include <vector>
#include "http.h"

namespace juiz {

 template<typename T, typename K, typename V>
  std::vector<T> map(std::multimap<K, V>& map, std::function<T(const K&,V&)> func) {
    std::vector<T> ret;
    for(auto& [k, v] : map) {
      ret.emplace_back(func(k, v));
    }
    return ret;
  }

  inline juiz::Request convert(const httplib::Request &req) {
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

  inline juiz::Response convert(std::shared_ptr<httplib::Response> response) {
    if (response) {
      return juiz::Response(response->status, response->body, response->headers.find("Content-Type")->second);
    }
    return juiz::Response(404, "", "text/html");
  }

}