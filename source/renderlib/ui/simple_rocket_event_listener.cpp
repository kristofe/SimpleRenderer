#include "ui/simple_rocket_event_listener.h"
#include "ui/simple_ui.h"

using Rocket::Core::String;

namespace renderlib {
namespace ui {

  RocketEventListener::RocketEventListener(UI* ui) {
    ui_ = ui;
  }

  RocketEventListener::~RocketEventListener() {
    // Nothing to do
  }

  // Instances a new event handle for Invaders.
  void RocketEventListener::ProcessEvent(Rocket::Core::Event& event) {
    ui_->processEvent(event);
  }

}  // namespace ui
}  // namespace renderlib
