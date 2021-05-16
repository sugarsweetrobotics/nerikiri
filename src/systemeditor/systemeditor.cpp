
#include "webi/webi.h"
#include "juiz/logger.h"

#include "juiz/systemeditor.h"


using namespace juiz;

using namespace webi;
using namespace webi::html;


class SystemEditorImpl : public SystemEditor {
  
public:
  
  SystemEditorImpl(const std::string& name="system_editor", const int32_t api_port=8080, const int32_t port=8000, const int32_t websocket_port=8001, const std::string& baseDir="www");
  virtual ~SystemEditorImpl();
  
  void setBaseDir(const std::string& dir) { baseDir_ = dir; }
public:
  
  virtual bool run() override;
  
  virtual bool shutdown() override;
  
private:
  const std::string name_;
  const int32_t port_;
  const int32_t api_port_;
  const int32_t websocket_port_;
  std::string baseDir_;
  std::condition_variable cond_;
  std::mutex mutex_;

  webi::Server_ptr webi_server_;
public:
  
};


SystemEditor_ptr juiz::systemEditor(const std::string& name, const int32_t api_port, const int32_t port, const int32_t websocket_port) {
  return std::make_unique<SystemEditorImpl>(name, api_port, port, websocket_port);
}

SystemEditorImpl::SystemEditorImpl(const std::string& name, const int32_t api_port, const int32_t port, const int32_t websocket_port, const std::string& base_dir) : name_(name), api_port_(api_port), port_(port), websocket_port_(websocket_port), baseDir_(base_dir) {
  logger::trace("SystemEditorImpl::SystemEditorImpl({}, {}, {}, {})", name, api_port, port, websocket_port);
}


SystemEditorImpl::~SystemEditorImpl() {
  logger::trace("SystemEditorImpl::~SystemEditorImpl({})", name_);

}


bool SystemEditorImpl::run() {
  logger::trace("SystemEditorImpl({})::run() in port {}", name_, port_);
  std::unique_lock<std::mutex> lock(mutex_);
  webi::Webi webi_;  

  logger::trace(" creating webi server");
  webi_server_ = webi_.createServer();
  if (webi_server_) { logger::trace("okayyyyy"); }
  else { logger::trace("hogehoge"); }
  auto document = webi_server_->createDocument();
  logger::trace(" createDocumnet");  

  logger::trace(" set base_dir to {}", baseDir_);
  webi_server_->baseDirectory(baseDir_);

  // Grid Style
  
  GridLayoutStyler gridStyle({ 150, 150, 30 }, { 40, 40 });

  auto on_up_button_clicked = [&document](const ActionEvent& e) {
				std::cout << "Up Button Clicked" << std::endl;
				auto textBox01 = document.getElementById("text_01");
				auto elem = document.getElementById("webi_content");
				textBox01.getValue([elem](const webi::ElementResponse& res) {
						     elem.setInnerHTML("Value is " + res.value);
						   });
			      };
  auto on_down_button_clicked = [&document](const ActionEvent& e) {
				  std::cout << "Down Button Clicked" << std::endl;
				  auto elem = document.getElementById("webi_content");
				  elem.setInnerHTML("Value is Down");
				};

  logger::trace(" set get method handler");
  // Define HTML file and set endpoint and document simaltaneously for server.

  webi_server_->get("/", htmlDoc(
			   head(
				webi::webiScript() // This is needed for webi framework.
				//styleSheet("webi.css")
        ),
			   body(
				WebiToolbar(),
				h1(text("This is Title")),
				p(id("webi_content"),
				  text("This is Test Page of Webi")
				  ),
				div(gridStyle.containerStyle(),
				    p(text("Up button ->"), gridStyle.gridStyle(0, 0), style("color:red;")),
				    textBox("TextBox1", id("text_01"), gridStyle.gridStyle(1, 0)),
				    button("Up", id("button_up"), gridStyle.gridStyle(2, 0),
					   onclick(on_up_button_clicked)),
				    p(text("Down button 2 ->"), gridStyle.gridStyle(0, 1), style("color:blue;")),
				    textBox("TextBox3", id("text_02"), gridStyle.gridStyle(1, 1)),
				    button("Down", id("button_down"), gridStyle.gridStyle(2, 1),
					   onclick(on_down_button_clicked))
				    )
				)
			   )
	      );
  
  logger::trace(" starting webi server on background");
  //webi_server_->runForever(port_, websocket_port_);
  webi_server_->runBackground(port_, websocket_port_);
  cond_.wait(lock);  
  webi_server_->terminateBackground();
  return true;
}


bool SystemEditorImpl::shutdown() {
  logger::trace("SystemEditorImpl({})::shutdown()", name_);
  cond_.notify_all();
  return true;
}
