#include <iostream>

#include <flowcpp/flow.h>

using namespace std;

enum class counter_action_type {
  nothing, increment, decrement
};

struct increment_action {
  int payload() const { return _payload; }

  counter_action_type type() const { return _type; }

  void *meta() const { return _meta; }

  bool error() const { return _error; }

  int _payload = {1};
  counter_action_type _type = {counter_action_type::increment};
  void *_meta = nullptr;
  bool _error = false;
};

struct decrement_action {
  decrement_action(int payload) : _payload{payload} { }

  int payload() const { return _payload; }

  counter_action_type type() const { return _type; }

  void *meta() const { return _meta; }

  bool error() const { return _error; }

  int _payload = {1};
  counter_action_type _type = {counter_action_type::decrement};
  void *_meta = nullptr;
  bool _error = false;
};

struct counter_state {
  string to_string() {
    return "counter: " + std::to_string(_counter);
  }

  int _counter{0};
};

using counter_action = flow::basic_action<int, counter_action_type, void *>;

auto reducer = [](counter_state state, counter_action action) -> counter_state {
  int multiplier = 1;
  switch (flow::type(action)) {
    case counter_action_type::decrement:
      multiplier = -1;
      break;
    case counter_action_type::increment:
      multiplier = 1;
      break;
    case counter_action_type::nothing:
      break;
  }
  state._counter += multiplier * flow::payload(action);
  return state;
};

int main() {

  // store
  auto s = flow::create_store_with_action<counter_state, counter_action>(
      reducer, counter_state(), increment_action{10});

  // disposable
  auto d = s.subscribe(
      [](counter_state state) { cout << state.to_string() << endl; });

  s.dispatch(increment_action{2});

  flow::disposable(d)();

  s.dispatch(decrement_action{10});
  s.dispatch(increment_action{3});
  s.dispatch(decrement_action{5});

  cout << flow::get_state(s)().to_string() << endl;

  return 0;
}