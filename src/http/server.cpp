
#include <utility>
#include <cstdlib>
#include <typeinfo>
#include "nerikiri/http/server.h"

#include <iostream>
#include <httplib.h>

using namespace nerikiri;

http::Request convert(const httplib::Request& req) {
  return http::Request(req.method, req.body);
}

void apply(httplib::Response& response, http::Response&& r) {
  response.status = r.status;
  response.version = r.version;
  response.body = r.body;
}

struct ServerImpl : public http::HttpServer {
  ServerImpl(const std::string& addr, const int32_t p) : http::HttpServer(addr, p),
						    svr(new httplib::Server()), 
						    server_thread(nullptr) {
  }

  ServerImpl(ServerImpl&& s) : http::HttpServer(s), 
			       svr(s.svr), 
			       server_thread(std::move(s.server_thread)) {
  }
  

  virtual ~ServerImpl() {
    if (svr->is_running()) {
      svr->stop();
      server_thread->join();
    }
  }
  std::shared_ptr<httplib::Server> svr;
  std::unique_ptr<std::thread> server_thread;
};


#define impl(x) dynamic_cast<ServerImpl*>(x.get())

http::HttpServer_ptr http::server(const std::string& address, const int32_t port) {
  logger::trace("http::server({}, {})", address.c_str(), port);
  auto s = std::unique_ptr<http::HttpServer>(new ServerImpl(address, port));
  return std::move(s);
}


std::function<http::HttpServer_ptr(http::HttpServer_ptr&&)> http::serve(const std::string& endpoint, const std::string& method, Callback cb) {
  logger::trace("serve({}, {})", endpoint, method);
  return [endpoint, method, cb](http::HttpServer_ptr&& server) {
	   if (server->failed) return std::move(server);

	   std::function<void(const httplib::Request&)> log_req = [](const httplib::Request& req) {};

	   if (logger::doLog(logger::TRACE)) {
		   log_req = [](const httplib::Request& req) {
			   logger::debug("Request - request.method={}", req.method);
			   logger::trace("request.body={}", req.body);
		   };
	   }
	   else if (logger::doLog(logger::DEBUG)) {
		   log_req = [](const httplib::Request& req) {
			   logger::debug("Request - request.method={}", req.method);
		   };
	   }
	       

	   std::function<void(const httplib::Response&)> log_res = [](const httplib::Response& res) {};
	   if (logger::doLog(logger::TRACE)) {
		   log_res = [](const httplib::Response& res) {
			   logger::trace("response.body={}", res.body);
		   };
	   } else if (logger::doLog(logger::DEBUG)) {
		   log_res = [](const httplib::Response& res) {
		   };
	   }

	   if (method == "GET") {
	     impl(server)->svr->Get(endpoint.c_str(), [cb, log_req, log_res](const httplib::Request& req, httplib::Response& res) {
							log_req(req);
							apply(res, cb(convert(req)));
							log_res(res);
						      });
	   } else if (method == "PUT") {
	     impl(server)->svr->Put(endpoint.c_str(), [cb, log_req, log_res](const httplib::Request& req, httplib::Response& res) {
							log_req(req);
							apply(res, cb(convert(req)));
							log_res(res);
						      });
	   } else if (method == "POST") {
	     impl(server)->svr->Put(endpoint.c_str(), [cb, log_req, log_res](const httplib::Request& req, httplib::Response& res) {
							log_req(req);
							apply(res, cb(convert(req)));
							log_res(res);
						      });
	   } else {
	     server->failed = true;
	     impl(server)->svr = nullptr;
	   }
	   return std::move(server);
  };
}


std::function<http::HttpServer_ptr(http::HttpServer_ptr&&)> http::listen(double timeout) {
  logger::trace("listen({})", timeout);
  return [timeout](http::HttpServer_ptr&& server) {
    int32_t port = server->port;
    auto address = server->address;
    auto svr = impl(server)->svr;
    if (port == 0) {
      logger::info("http::listen automatic bind for any port.");
      server->port = svr->bind_to_any_port(address.c_str(), 0);
    }
    logger::info("http::listen port is {}", server->port);    
    impl(server)->server_thread = std::unique_ptr<std::thread>(new std::thread([&] {
	  if (port == 0) {
	    if (svr->listen_after_bind()) {
	    }
	  } else {
	    if (svr->listen(address.c_str(), port)) {
	      
	    }
	  }
	}));
    
    auto t = std::chrono::system_clock::now();
    for(;;) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      if (svr->is_running()) {
	logger::trace("http::listen server is running.");
	break;
      }
      
      auto d = std::chrono::system_clock::now() - t;
      int to = (int)(timeout*1000);
      
      if (to != 0 && d > std::chrono::milliseconds(to)) {
		logger::trace("http::listen server may not be running, but timeout to wait.");
		break;
      }
    }

    server->port = port;
    return std::move(server);
  };
}


std::function<http::HttpServer_ptr(http::HttpServer_ptr&&)> http::stop() {
  return [&](http::HttpServer_ptr&& server) {
	   impl(server)->svr->stop();
	   impl(server)->server_thread->join();
	   return std::move(server);
  };
}
