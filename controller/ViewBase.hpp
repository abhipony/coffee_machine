#ifndef CONTROLLER_VIEW_BASE_HPP
#define CONTROLLER_VIEW_BASE_HPP

namespace controller {
template <typename Controller>
class ViewBase {
 public:
  ViewBase() : app_(nullptr) {}
  virtual ~ViewBase() {}
  void              app(Controller* c) { app_ = c; }
  void              unset_app() { app_ = nullptr; }
  Controller&       app() { return *(app_); }
  const Controller& app() const { return *(app_); }
  virtual void      initialize() = 0;
  virtual void      finalize()   = 0;

 private:
  Controller* app_;
};
}  // namespace controller
#endif
