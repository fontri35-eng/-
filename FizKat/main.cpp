#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <utility>

class FleetError : public std::runtime_error {
 public:
  FleetError(const std::string& s) : std::runtime_error(s) {}
};

class InspectionError : public FleetError {
 public:
  InspectionError() : FleetError("inspection rejected") {}
};

class CommandError : public FleetError {
 public:
  CommandError(const std::string& s) : FleetError(s) {}
};

struct FocusData {
  std::string id;
  std::string name;

  FocusData(const std::string& a, const std::string& b) : id(a), name(b) {}
};

struct ModelSpec {
  std::string type;
  int limit;

  ModelSpec(const std::string& t, int n) : type(t), limit(n) {}
};

template <typename T>
class Registry {
  std::vector<T> data;

 public:
  void add(const T& x) {
    data.push_back(x);
  }

  int size() const {
    return static_cast<int>(data.size());
  }

  T& operator[](int i) {
    return data[i];
  }

  const T& operator[](int i) const {
    return data[i];
  }
};

class InventoryItem {
 public:
  virtual const std::string& inventoryId() const = 0;
  virtual const std::string& name() const = 0;
  virtual void describe(std::ostream& out) const = 0;
  virtual std::weak_ptr<FocusData> view() const = 0;

  virtual double energy() const {
    return 0;
  }

  virtual bool novice() const {
    return false;
  }

  bool operator<(const InventoryItem& other) const {
    return inventoryId() < other.inventoryId();
  }

  bool operator==(const InventoryItem& other) const {
    return inventoryId() == other.inventoryId();
  }

  virtual ~InventoryItem() {}
};

std::ostream& operator<<(std::ostream& out, const InventoryItem& item) {
  item.describe(out);
  return out;
}

class EnergyConsumer {
 protected:
  double value;

 public:
  EnergyConsumer(double e) : value(e) {
    if (e < 0) {
      throw CommandError("negative energy");
    }
  }

  virtual double dailyEnergy() const = 0;
  virtual ~EnergyConsumer() {}
};

class Item : public InventoryItem {
  std::string id;
  std::string title;
  std::shared_ptr<FocusData> card;

 public:
  Item(const std::string& a, const std::string& b) : id(a), title(b) {
    if (id.empty() || title.empty()) {
      throw CommandError("empty id or name");
    }
    card = std::make_shared<FocusData>(id, title);
  }

  const std::string& inventoryId() const override {
    return id;
  }

  const std::string& name() const override {
    return title;
  }

  void describe(std::ostream& out) const override {
    out << id << " " << title;
  }

  std::weak_ptr<FocusData> view() const override {
    return card;
  }
};

class Transport : public Item {
  int score;
  std::shared_ptr<ModelSpec> model;

 public:
  Transport(const std::string& id, const std::string& name, int n,
            std::shared_ptr<ModelSpec> m)
      : Item(id, name), score(n), model(m) {
    if (score < 1 || score > 10) {
      throw CommandError("score must be from 1 to 10");
    }
  }

  bool novice() const override {
    return score >= model->limit;
  }
};

class Scooter : public Transport, public EnergyConsumer {
 public:
  Scooter(const std::string& id, const std::string& name, double e, int n,
          std::shared_ptr<ModelSpec> m)
      : Transport(id, name, n, m), EnergyConsumer(e) {}

  double dailyEnergy() const override {
    return value;
  }

  double energy() const override {
    return dailyEnergy();
  }
};

class Bike : public Transport {
 public:
  Bike(const std::string& id, const std::string& name, int n,
       std::shared_ptr<ModelSpec> m)
      : Transport(id, name, n, m) {}
};

class EBike : public Transport, public EnergyConsumer {
 public:
  EBike(const std::string& id, const std::string& name, double e, int n,
        std::shared_ptr<ModelSpec> m)
      : Transport(id, name, n, m), EnergyConsumer(e) {}

  double dailyEnergy() const override {
    return value;
  }

  double energy() const override {
    return dailyEnergy();
  }
};

class Helmet : public Item {
 public:
  Helmet(const std::string& id, const std::string& name) : Item(id, name) {}
};

class Dock : public Item, public EnergyConsumer {
 public:
  Dock(const std::string& id, const std::string& name, double e)
      : Item(id, name), EnergyConsumer(e) {}

  double dailyEnergy() const override {
    return value;
  }

  double energy() const override {
    return dailyEnergy();
  }
};

class Skate : public Transport {
 public:
  Skate(const std::string& id, const std::string& name, int n,
        std::shared_ptr<ModelSpec> m)
      : Transport(id, name, n, m) {}
};

template <typename T>
double totalEnergy(const T& items) {
  double sum = 0;
  for (int i = 0; i < static_cast<int>(items.size()); i++) {
    sum += items[i]->energy();
  }
  return sum;
}

bool isBroken(const std::string& name) {
  const std::string word = "broken";
  for (int i = 0; i + static_cast<int>(word.size()) <=
                      static_cast<int>(name.size()); i++) {
    bool found = true;
    for (int j = 0; j < static_cast<int>(word.size()); j++) {
      char c = name[i + j];
      if (c >= 'A' && c <= 'Z') {
        c = c - 'A' + 'a';
      }
      if (c != word[j]) {
        found = false;
      }
    }
    if (found) {
      return true;
    }
  }
  return false;
}

class Fleet {
  std::vector<std::unique_ptr<InventoryItem>> items;
  Registry<std::shared_ptr<ModelSpec>> models;
  std::weak_ptr<FocusData> focus;

  Registry<const InventoryItem*> selected(bool onlyNovice) const {
    Registry<const InventoryItem*> result;
    for (int i = 0; i < static_cast<int>(items.size()); i++) {
      if (!onlyNovice || items[i]->novice()) {
        result.add(items[i].get());
      }
    }

    for (int i = 0; i < result.size(); i++) {
      for (int j = i + 1; j < result.size(); j++) {
        if (*result[j] < *result[i]) {
          const InventoryItem* tmp = result[i];
          result[i] = result[j];
          result[j] = tmp;
        }
      }
    }
    return result;
  }

 public:
  Fleet() {
    models.add(std::make_shared<ModelSpec>("scooter", 6));
    models.add(std::make_shared<ModelSpec>("bike", 6));
    models.add(std::make_shared<ModelSpec>("ebike", 6));
    models.add(std::make_shared<ModelSpec>("skate", 6));
  }

  std::shared_ptr<ModelSpec> model(const std::string& type) const {
    for (int i = 0; i < models.size(); i++) {
      if (models[i]->type == type) {
        return models[i];
      }
    }
    throw CommandError("unknown type");
  }

  void add(std::unique_ptr<InventoryItem> item, bool inspection) {
    for (int i = 0; i < static_cast<int>(items.size()); i++) {
      if (*items[i] == *item) {
        throw CommandError("duplicate id");
      }
    }
    if (inspection && isBroken(item->name())) {
      throw InspectionError();
    }
    items.push_back(std::move(item));
  }

  void printList(bool onlyNovice) const {
    Registry<const InventoryItem*> result = selected(onlyNovice);
    for (int i = 0; i < result.size(); i++) {
      std::cout << *result[i] << '\n';
    }
  }

  double energy() const {
    return totalEnergy(items);
  }

  int count() const {
    return static_cast<int>(items.size());
  }

  int countNovice() const {
    return selected(true).size();
  }

  void setFocus(const std::string& id) {
    for (int i = 0; i < static_cast<int>(items.size()); i++) {
      if (items[i]->inventoryId() == id) {
        focus = items[i]->view();
        return;
      }
    }
    throw CommandError("id not found");
  }

  void showFocus() const {
    std::shared_ptr<FocusData> card = focus.lock();
    if (card) {
      std::cout << card->id << " " << card->name << '\n';
    } else {
      std::cout << "expired\n";
    }
  }

  void remove(const std::string& id) {
    int pos = -1;
    for (int i = 0; i < static_cast<int>(items.size()); i++) {
      if (items[i]->inventoryId() == id) {
        pos = i;
      }
    }
    if (pos == -1) {
      throw CommandError("id not found");
    }

    std::vector<std::unique_ptr<InventoryItem>> next;
    for (int i = 0; i < static_cast<int>(items.size()); i++) {
      if (i != pos) {
        next.push_back(std::move(items[i]));
      }
    }
    items = std::move(next);
  }
};

void checkEnd(std::stringstream& input) {
  std::string extra;
  if (input >> extra) {
    throw CommandError("bad arguments");
  }
}

int main() {
  Fleet fleet;
  std::string line;
  while (std::getline(std::cin, line)) {
    std::stringstream input(line);
    std::string cmd;
    if (!(input >> cmd)) {
      continue;
    }

    try {
      if (cmd == "add") {
        std::string type, id, name;
        if (!(input >> type >> id >> name)) {
          throw CommandError("bad arguments");
        }

        if (type == "scooter" || type == "ebike") {
          double e;
          int n;
          if (!(input >> e >> n)) {
            throw CommandError("bad arguments");
          }
          checkEnd(input);
          if (type == "scooter") {
            fleet.add(std::make_unique<Scooter>(id, name, e, n, fleet.model(type)), true);
          } else {
            fleet.add(std::make_unique<EBike>(id, name, e, n, fleet.model(type)), true);
          }
        } else if (type == "bike" || type == "skate") {
          int n;
          if (!(input >> n)) {
            throw CommandError("bad arguments");
          }
          checkEnd(input);
          if (type == "bike") {
            fleet.add(std::make_unique<Bike>(id, name, n, fleet.model(type)), true);
          } else {
            fleet.add(std::make_unique<Skate>(id, name, n, fleet.model(type)), true);
          }
        } else if (type == "helmet") {
          checkEnd(input);
          fleet.add(std::make_unique<Helmet>(id, name), false);
        } else if (type == "dock") {
          double e;
          if (!(input >> e)) {
            throw CommandError("bad arguments");
          }
          checkEnd(input);
          fleet.add(std::make_unique<Dock>(id, name, e), false);
        } else {
          throw CommandError("unknown type");
        }
      } else if (cmd == "list") {
        checkEnd(input);
        fleet.printList(false);
      } else if (cmd == "novice") {
        checkEnd(input);
        fleet.printList(true);
      } else if (cmd == "energy") {
        checkEnd(input);
        std::cout << std::fixed << std::setprecision(1) << fleet.energy() << '\n';
      } else if (cmd == "count") {
        checkEnd(input);
        std::cout << fleet.count() << '\n';
      } else if (cmd == "count_novice") {
        checkEnd(input);
        std::cout << fleet.countNovice() << '\n';
      } else if (cmd == "focus" || cmd == "remove") {
        std::string id;
        if (!(input >> id)) {
          throw CommandError("bad arguments");
        }
        checkEnd(input);
        if (cmd == "focus") {
          fleet.setFocus(id);
        } else {
          fleet.remove(id);
        }
      } else if (cmd == "show_focus") {
        checkEnd(input);
        fleet.showFocus();
      } else if (cmd == "quit") {
        checkEnd(input);
        break;
      } else {
        throw CommandError("unknown command");
      }
    } catch (const FleetError& e) {
      std::cout << "error: " << e.what() << '\n';
    }
  }
}
