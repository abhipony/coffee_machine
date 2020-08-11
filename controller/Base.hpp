#ifndef CONTROLLER_BASE_HPP
#define CONTROLLER_BASE_HPP

namespace controller {
class Aborted {};
class Base {
 public:
  virtual ~Base() {}
  virtual int run() = 0;
};
}  // namespace controller

#endif
