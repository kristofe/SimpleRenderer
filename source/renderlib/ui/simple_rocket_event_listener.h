#pragma once

#include <Rocket/Core/EventListener.h>

namespace renderlib {
namespace ui {

  class UI;

  class RocketEventListener : public Rocket::Core::EventListener {
  public:
    RocketEventListener(UI* ui);
		virtual ~RocketEventListener();
    
    /// Process the incoming Event
		virtual void ProcessEvent(Rocket::Core::Event& event);


  private:
    UI* ui_;

    // Non-copyable, non-assignable.
    RocketEventListener(RocketEventListener&);
    RocketEventListener& operator=(const RocketEventListener&);
  };

};  // namespace ui
};  // namespace renderlib
