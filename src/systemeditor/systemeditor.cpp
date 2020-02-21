#include "nerikiri/systemeditor.h"
#include "nerikiri/logger.h"

#include "webi/webi.h"

using namespace nerikiri;

using namespace webi;
using namespace webi::html;


class SystemEditorImpl : public SystemEditor {
  
public:
  
  SystemEditorImpl(const std::string& name="system_editor", const int32_t port=8000, const int32_t websocket_port=8001, const std::string& baseDir="www");
  virtual ~SystemEditorImpl();
  
  void setBaseDir(const std::string& dir) { baseDir_ = dir; }
public:
  
  virtual bool run() override;
  
  virtual bool shutdown() override;
  
private:
  const std::string name_;
  const int32_t port_;
    const int32_t websocket_port_;
  std::string baseDir_;
  std::condition_variable cond_;
  std::mutex mutex_;

  webi::Server_ptr server_;
public:
  
};


SystemEditor_ptr nerikiri::systemEditor(const std::string& name, const int32_t port, const int32_t websocket_port) {
  return std::make_unique<SystemEditorImpl>(name, port, websocket_port);
}

SystemEditorImpl::SystemEditorImpl(const std::string& name, const int32_t port, const int32_t websocket_port, const std::string& base_dir) : name_(name), port_(port), websocket_port_(websocket_port), baseDir_(base_dir) {
  logger::trace("SystemEditorImpl::SystemEditorImpl({}, {}, {})", name, port, websocket_port);
}


SystemEditorImpl::~SystemEditorImpl() {
  logger::trace("SystemEditorImpl::~SystemEditorImpl({})", name_);

}


bool SystemEditorImpl::run() {
  logger::trace("SystemEditorImpl({})::run() in port {}", name_, port_);
  std::unique_lock<std::mutex> lock(mutex_);
  webi::Webi webi_;  

  logger::trace(" creating webi server");
  server_ = webi_.createServer();
  if (server_) { logger::trace("okayyyyy"); }
  else { logger::trace("hogehoge"); }
  auto document = server_->createDocument();
  logger::trace(" createDocumnet");  

  logger::trace(" set base_dir to {}", baseDir_);
  server_->baseDirectory(baseDir_);

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

  server_->get("/", htmlDoc(
			   head(
				webi::webiScript(), // This is needed for webi framework.
				styleSheet("webi.css")),
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
				    p(text("Down button ->"), gridStyle.gridStyle(0, 1), style("color:blue;")),
				    textBox("TextBox2", id("text_02"), gridStyle.gridStyle(1, 1)),
				    button("Down", id("button_down"), gridStyle.gridStyle(2, 1),
					   onclick(on_down_button_clicked))
				    )
				)
			   )
	      );
  
  logger::trace(" starting webi server on background");
  server_->runForever(port_, websocket_port_);
  //  server_->runBackground(port_, websocket_port_);
  cond_.wait(lock);  
  return false;
}


bool SystemEditorImpl::shutdown() {
  logger::trace("SystemEditorImpl({})::shutdown()", name_);
  cond_.notify_all();
  return true;
}
