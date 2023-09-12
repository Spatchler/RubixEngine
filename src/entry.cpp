#include <rubixEngine.hpp>

int main() {
  App::get().init();
  App::get().pollFPS = true;

  while (App::get().open) {
    App::get().update();
  }

  return 0;
}
