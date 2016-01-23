#pragma once
#include <unordered_map>
#include <list>
#include <stdint.h>
#include "vmath.hpp"
#include "EngineObject.h"

namespace Rocket {namespace Core {template< typename T > class StringBase;};};
namespace Rocket { namespace Core { typedef StringBase< char > String; }; };
namespace Rocket { namespace Core { class Context; }; };
namespace Rocket { namespace Core { class Event; }; };
namespace Rocket { namespace Core { class Element; }; };
namespace Rocket { namespace Core { class ElementDocument; }; };

namespace renderlib {

namespace ui {

  class RocketSystemInterface;
  class RocketRenderInterface;
  class RocketFileInterface;
  class RocketEventListener;
  class RocketEvent;

  struct UIEnumVal {
  public:
    int          Value;
    const char * Label;
    UIEnumVal(const int Value_, const char * Label_) : Value(Value_), Label(Label_) {}
    UIEnumVal() : Value(0), Label(nullptr) {}
  };

  // Function pointer for button callbacks
  typedef void (*ButtonCallback) (void);

  class UI : public EngineObject {
  public:
    // Constructor / Destructor
          UI();
    ~UI();

    //Engine Object Messaging functions
    virtual void processMessage(EngineObjectMessage& msg);

    // Functions used by the Renderer: You shouldn't need to touch these
    void init();
    void update(const double dt);
    bool debugger_visible() const { return debugger_visible_; }
    void debugger_visible(const bool val);
    inline bool ui_running() { return ui_running_; }
    void renderFrame() const;
    void setWindowSize(const int width, const int height);

    void setUIRunning(bool r) { ui_running_ = r; }
    bool getUIRunning() { return ui_running_; }
        void toggleUIRunning() { ui_running_ = !ui_running_; setSettingsVisibility(ui_running_); }
    void setSettingsVisibility(const bool visible) const;
    void setRendererCheckboxVal(const std::string& name,
      const bool val) const;
    void reloadUIContent();

    // Event handlers
    void keyboardInputCB(int key, int scancode, int action, int mods) const;
    void mousePosCB(double x, double y) const;
    void mouseButtonCB(int button, int action, int mods) const;
    void mouseWheelCB(double xoffset, double yoffset) const;
    void processEvent(const Rocket::Core::Event& event);


    // mouse_over_ui - Query if the mouse is currently over the UI
    inline bool mouse_over_ui() { return mouse_over_count_ > 1; }

    // UI Manipulation functions: Addition of elements
    void addHeadingText(const char* text, const char* elem_name = NULL) const;
    void addCheckbox(const char* bool_setting_name, const char* text) const;
    void addSelectbox(const char* int_setting_name, const char* text) const;
    void addSelectboxItem(const char* int_setting_name, const UIEnumVal& item)
      const;
    void setCheckboxVal(const std::string& name, const bool val) const;
    void setSelectboxVal(const std::string& name, const int val) const;
    void addButton(const char* elem_name, const char* text,
      ButtonCallback func) const;
    void addTitleBarButton(Rocket::Core::ElementDocument* doc,
      ButtonCallback func, const char* name, const char* text) const;

    void createTextWindow(const char* wnd_name, const char* str);
    void setTextWindowString(const char* wnd_name, const char* text);
    void setTextWindowPos(const char* wnd_name, const vmath::Vector2& pos);
    void setTextWindowVisibility(const char* wnd_name, const bool visible);
    int getElementWidth(const char* elem_name);
    void setElementWidth(const char* elem_name, int width);

    static void log(std::string s);
    static UI* getInstance();

  private:
    bool ui_running_;
    bool debugger_visible_;
    RocketRenderInterface* render_interface_;
    RocketSystemInterface* system_interface_;
    RocketFileInterface* file_interface_;
    Rocket::Core::Context* context_;
    RocketEventListener* event_listener_;
    Rocket::Core::ElementDocument* app_doc_;
    Rocket::Core::ElementDocument* midi_doc_;
    Rocket::Core::ElementDocument* fps_doc_;
    Rocket::Core::ElementDocument* text_doc_;
    Rocket::Core::Element* app_content_;
    Rocket::Core::Element* midi_content_;
    Rocket::Core::Element* fps_content_;
    Rocket::Core::Element* text_content_;
    std::unordered_map<std::string, uint32_t>* app_text_str2ind_;
    std::vector<Rocket::Core::ElementDocument*> app_text_doc_;
    std::vector<Rocket::Core::Element*> app_text_content_;
    int mouse_over_count_;
    static const UIEnumVal instruments_[];
    static const uint32_t num_instruments_;
    static const UIEnumVal drums_[];
    static const uint32_t num_drums_;
    std::unordered_map<std::string, ButtonCallback>* button_callbacks_;
    static unsigned long event_count_;
    double frame_time_accum_;
    double min_fps_;
    double max_fps_;
    char fps_buffer_[256];
    unsigned long frame_count_;
    std::string logString;
    std::list<std::string> logLines;

    // Internal functions to create document elements: Here we can specify a
    // UI document destination.
    void addHeadingText(const char* text,
      Rocket::Core::ElementDocument* doc,
      Rocket::Core::Element* content,
      const char* elem_name = NULL)  const;
    void addPreText(const char* text,
      Rocket::Core::ElementDocument* doc,
      Rocket::Core::Element* content,
      const char* elem_name = NULL)  const;
    void addCheckbox(const char* bool_setting_name, const char* text,
      Rocket::Core::ElementDocument* doc,
      Rocket::Core::Element* content) const;
    void addSelectbox(const char* int_setting_name, const char* text,
      const UIEnumVal box_vals[], const uint32_t num_vals,
      Rocket::Core::ElementDocument* doc,
      Rocket::Core::Element* content) const;
    // TO DO: Fix slider
    void addSlider(const char* name, const char* text, float min, float max,
      Rocket::Core::ElementDocument* doc,
      Rocket::Core::Element* content) const;
    void addButton(const char* elem_name, const char* text,
      ButtonCallback func, Rocket::Core::ElementDocument* doc,
      Rocket::Core::Element* content) const;
    void setCheckboxVal(const std::string& name, const bool val,
      Rocket::Core::ElementDocument* doc) const;
    void setSelectboxVal(const std::string& name, const int val,
      Rocket::Core::ElementDocument* doc) const;
    void addLineBreak(Rocket::Core::ElementDocument* doc,
      Rocket::Core::Element* content) const;

    // Inner helper functions
    static void loadRocketFontSafe(const Rocket::Core::String& file_name);
    void loadRocketDocument(const char* file,
      Rocket::Core::ElementDocument*& doc, Rocket::Core::Element*& content)
      const;
    void loadFPSElements() const;
    static void setVisibility(const bool visible,
      Rocket::Core::ElementDocument*& doc);
    void loadRendererElements() const;
    void loadAppElements() const;
    void loadTextElements() const;
    void loadUIContent();
    void createUIContext();
    static void showMidiDoc();
    static void hideMidiDoc();
    static void showAppDoc();
    static void hideAppDoc();
    static void showTextDoc();
    static void hideTextDoc();
    static void clearLog();
    void setDocumentTitle(Rocket::Core::ElementDocument* doc,
      const std::string& title) const;
    void addCloseButton(Rocket::Core::ElementDocument* doc,
      ButtonCallback close_func, const char* close_button_name) const;

    void setDocSize(const vmath::Vector2& size,
      Rocket::Core::ElementDocument* doc);
    const uint32_t getTextWindowID(const std::string& name) const;
    static void hideFPSDoc();
    static void showFPSDoc();

    void clearText(Rocket::Core::ElementDocument* doc, std::string elementName);
    void appendText(std::string str, Rocket::Core::ElementDocument* doc, std::string elementName);


    // Non-copyable, non-assignable.
    UI(UI&) = delete;
    UI& operator=(const UI&) = delete;
  };
};  // namespace ui
};  // namespace renderlib
