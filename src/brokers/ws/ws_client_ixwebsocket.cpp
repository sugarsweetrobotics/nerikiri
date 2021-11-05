#include <math.h>
#include <sstream>

#include "juiz/utils/json.h"
#include "ws_client.h"
#include "ws_converter_ixwebsocket.h"

#include <ixwebsocket/IXWebSocket.h>
#include <mutex>
#include <condition_variable>

using namespace juiz;

struct Request {
  const std::string url;
  const std::string type;
  const std::string body;
};

class WebSocketClientImpl : public juiz::ws::WebSocketClient {
private:
  ix::WebSocket webSocket;
  uint64_t message_counter_;

  // "ステート"変数＋ミューテックスmtx＋条件変数cv
  int state;  // 注: 変数型やその個数は目的による
  std::mutex mtx;
  std::condition_variable cv;
  std::string message_buffer_;
  bool is_message_received_;

public:
  WebSocketClientImpl(const std::string& addr, const int port) : message_counter_(0), is_message_received_(false) {
    std::stringstream ss;
    ss << addr << ":" << port;
    webSocket.setUrl(ss.str());

    // Optional heart beat, sent every 45 seconds when there is not any traffic
    // to make sure that load balancers do not kill an idle connection.
    webSocket.setPingInterval(45);

    // Per message deflate connection is enabled by default. You can tweak its parameters or disable it
    webSocket.disablePerMessageDeflate();

    webSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
          this->message_buffer_ = msg->str;
          this->is_message_received_ = true;
        }
      }
    );

  }

  bool isMessageReceived() const { return is_message_received_; }

  virtual ~WebSocketClientImpl() {}
public:
  virtual void setTimeout(double sec) {
    double to_sec = floor(sec);
    double m = sec - to_sec;
    double to_usec = floor(m/1000000);      
    //client_.set_read_timeout(to_sec, to_usec);
    //client_.set_write_timeout(to_sec, to_usec);
  }
public:
  virtual juiz::ws::Response request(const std::string& url, const std::string& method) {
    if (method == "GET" || method == "POST" || method == "PUT" || method == "DELETE") {
      std::stringstream ss;
      ss << "{\"url\":\"" << url << "\",\"method\":\"" << method << "\",\"id\":" << message_counter_ << "}";
      webSocket.send(ss.str());
      std::unique_lock<std::mutex> lk(mtx);
      cv.wait(lk, [this]{ return this->isMessageReceived(); });
      auto response = juiz::json::toValue(message_buffer_);
      return convert(std::move(response));
    } 
    return juiz::ws::Response(405);
  }

  virtual juiz::ws::Response request(const std::string& url, const std::string& method, const juiz::ws::Request& req) {
      if (method == "GET" || method == "DELETE") {
        return request(url, method);
      } else if (method == "POST" || method == "PUT") {
        std::stringstream ss;
        ss << "{\"url\":\"" << url << "\",\"method\":\"" << method << "\",\"id\":" << message_counter_ << ",\"body\":\"" << req.body << "\"}";
        webSocket.send(ss.str());
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [this]{ return this->isMessageReceived(); });
        auto response = juiz::json::toValue(message_buffer_);
        return convert(std::move(response));
      }
      return juiz::ws::Response(405);
  }
  
};



juiz::ws::WebSocketClient_ptr juiz::ws::client(const std::string& addr, const int port) {
    return juiz::ws::WebSocketClient_ptr(new WebSocketClientImpl(addr, port));
}
