#include "controller/Base.hpp"
#include "controller/ViewBase.hpp"
#include "lib/json/json.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <string>

namespace my_model {}

namespace my_controller {
class App : public controller::Base {
public:
  class ViewIntf : public controller::ViewBase<App> {
  public:
    virtual void display_message(const std::string &message) = 0;
    virtual std::string input_beverage() = 0;
    virtual void display_available_beverages(
        const std::map<std::string, std::map<std::string, int>>
            &beverages_) = 0;
  };

  App(ViewIntf *iv);
  ~App();
  int run();

  void restock(const std::map<std::string, int> &ingredients);

private:
  ViewIntf *view_;
  int outlets_;
  // the bool in available_ingredients_ indicates whether the ingredient needs
  // restock.
  std::map<std::string, std::pair<int, bool>> available_ingredients_;
  std::map<std::string, std::map<std::string, int>> beverages_;

  void parse_initial_input_();
  void try_to_brew_(const std::string &beverage);
};

App::App(ViewIntf *iv) {
  view_ = iv;
  view_->app(this);
  view_->initialize();
}

App::~App() {
  view_->finalize();
  view_->unset_app();
}

void App::restock(const std::map<std::string, int> &ingredients) {
  for (auto it : ingredients) {
    if (available_ingredients_.find(it.first) == available_ingredients_.end())
      continue;
    if (available_ingredients_[it.first].second == true) {
      available_ingredients_[it.first].first += it.second;
    }
  }
}

void App::parse_initial_input_() {
  std::fstream fs("inputs.json");
  if (!fs.good()) {
    // We should crash here.
    exit(0);
  }
  using json = nlohmann::json;
  json input;
  fs >> input;

  outlets_ = input["machine"]["outlets"]["count_n"];

  for (const auto &it : input["machine"]["total_items_quantity"].items()) {
    available_ingredients_[it.key()] = {it.value(), false};
  }

  for (const auto &it1 : input["machine"]["beverages"].items()) {
    auto beverage_name = it1.key();
    std::map<std::string, int> beverage_ingredient;
    for (const auto &it2 : it1.value().items()) {
      beverage_ingredient[it2.key()] = it2.value();
    }
    beverages_[beverage_name] = beverage_ingredient;
  }
}

void App::try_to_brew_(const std::string &beverage) {
  if (beverages_.find(beverage) == beverages_.end()) {
    view_->display_message("THIS BEVERAGE IS NOT AVAILABLE");
    return;
  }

  auto temp_available_ingredients = available_ingredients_;
  for (const auto &it : beverages_[beverage]) {
    auto ingredient_name = it.first;
    auto required_amount = it.second;
    if (temp_available_ingredients.find(ingredient_name) ==
        temp_available_ingredients.end()) {
      view_->display_message(beverage + " cannot be prepared because " +
                             ingredient_name + " is not available");
      return;
    }

    if (temp_available_ingredients[ingredient_name].first < required_amount) {
      view_->display_message(beverage + " cannot be prepared because item " +
                             ingredient_name + " is not sufficient");
      temp_available_ingredients[ingredient_name].second = true;
      return;
    }

    temp_available_ingredients[ingredient_name].first -= required_amount;
  }

  view_->display_message(beverage + " is prepared");
  available_ingredients_ = temp_available_ingredients;
}

int App::run() {
  parse_initial_input_();

  while (true) {
    view_->display_available_beverages(beverages_);
    auto beverage = view_->input_beverage();
    try_to_brew_(beverage);
  }
  return 0;
}
} // namespace my_controller

namespace my_view {
class View : public my_controller::App::ViewIntf {
public:
  View() {}
  void initialize() {}
  void finalize() {}
  void display_message(const std::string &message) override;
  std::string input_beverage() override;
  void display_available_beverages(
      const std::map<std::string, std::map<std::string, int>> &beverages)
      override;
};

void View::display_message(const std::string &message) {
  std::cout << message << "\n\n";
}

void View::display_available_beverages(
    const std::map<std::string, std::map<std::string, int>> &beverages) {
  std::cout << "LIST OF BEVERAGES : \n";
  for (const auto &it : beverages) {
    std::cout << it.first << "\n";
  }
}

std::string View::input_beverage() {
  std::cout
      << "\nINPUT A BEVERAGE YOU WOULD LIKE TO HAVE FROM THE LIST ABOVE\n";
  std::string input;
  while (true) {
    std::cin >> input;
    if (!input.empty())
      return input;
  }
}
} // namespace my_view

int main(int argc, char **argv) {
  my_view::View iv;
  my_controller::App app(&iv);
  return app.run();
}
