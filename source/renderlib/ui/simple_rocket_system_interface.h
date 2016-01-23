#pragma once

#include <Rocket/Core/SystemInterface.h>
#include <Rocket/Core/Input.h>

namespace renderlib {

	namespace ui {
		class RocketSystemInterface : public Rocket::Core::SystemInterface
		{
		public:
			RocketSystemInterface() { }
			virtual ~RocketSystemInterface() { }

			Rocket::Core::Input::KeyIdentifier TranslateKey(const int key_glfw) const;
			int GetKeyModifiers()  const;
			virtual float GetElapsedTime();
			bool LogMessage(Rocket::Core::Log::Type type,
				const Rocket::Core::String& message);

		private:
			// Non-copyable, non-assignable.
			RocketSystemInterface(RocketSystemInterface&);
			RocketSystemInterface& operator=(const RocketSystemInterface&);
		};

	} // namespace ui
};  // namespace renderlib
