#include <sstream>
#include <iostream>
#include <string>
#include <Rocket/Core/Input.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger/Debugger.h>
#include <Rocket/Core.h>
#include <Rocket/Core/Types.h>
#include "InputManager.h"
#include "WindowManager.h"
#include "RenderManager.h"
#include "ui/simple_ui.h"
#include "ui/simple_rocket_render_interface.h"
#include "ui/simple_rocket_system_interface.h"
#include "ui/simple_rocket_file_interface.h"
#include "ui/simple_rocket_event_listener.h"
#include "midi/MidiHelper.h"

//#define UI_FPS_DOC "fps_counter.rml"
#define UI_FPS_DOC "fps_counter.rml"
#define UI_SETTINGS_DOC "settings.rml"
#define UI_TEXT_DOC "text.rml"

#define ROCKET_RESOURCE_PATH "assets/ui/"
#define SAFE_REMOVE_REFERENCE(x) if (x) { x->RemoveReference(); x = NULL; }
#define SAFE_DELETE(x) if (x) { delete x; x = NULL; }

#define MAX_CONSOLE_LINES 32

using std::wstring;
using std::runtime_error;
using Rocket::Core::String;
using Rocket::Core::Element;
using Rocket::Core::ElementText;
using Rocket::Core::Factory;
using Rocket::Core::Variant;
using Rocket::Controls::ElementFormControlSelect;
using Rocket::Core::Vector2i;
using Rocket::Core::Vector2f;

namespace renderlib {

namespace ui {

  const UIEnumVal UI::instruments_[] = {
    UIEnumVal(PERCUSSIVE_PROGRAM::AGOGO, "AGOGO"),
    UIEnumVal(PERCUSSIVE_PROGRAM::MELODIC_TOM, "MELODIC TOM"),
    UIEnumVal(PERCUSSIVE_PROGRAM::REVERSE_CYMBAL, "REVERSE CYMBAL"),
    UIEnumVal(PERCUSSIVE_PROGRAM::STEEL_DRUMS, "STEEL DRUMS"),
    UIEnumVal(PERCUSSIVE_PROGRAM::SYNTH_DRUM, "SYNTH DRUM"),
    UIEnumVal(PERCUSSIVE_PROGRAM::TAIKO_DRUM, "TAIKO DRUM"),
    UIEnumVal(PERCUSSIVE_PROGRAM::TINKLE_BELL, "TINKLE BELL"),
    UIEnumVal(PERCUSSIVE_PROGRAM::WOODBLOCK, "WOODENBLOCK")
  };
  const uint32_t UI::num_instruments_ = 8;

  const UIEnumVal UI::drums_[] = {
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Metronome_Click, "Metronome_Click"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Metronome_Bell, "Metronome_Bell"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Acoustic_Bass_Drum, "Acoustic_Bass_Drum"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Bass_Drum_1, "Bass_Drum_1"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Side_Stick, "Side_Stick"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Acoustic_Snare, "Acoustic_Snare"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Hand_Clap, "Hand_Clap"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Electric_Snare, "Electric_Snare"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Low_Floor_Tom, "Low_Floor_Tom"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Closed_Hi_Hat, "Closed_Hi_Hat"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::High_Floor_Tom, "High_Floor_Tom"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Pedal_Hi_Hat, "Pedal_Hi_Hat"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Low_Tom, "Low_Tom"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Open_Hi_Hat, "Open_Hi_Hat"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Low_Mid_Tom, "Low_Mid_Tom"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Hi_Mid_Tom, "Hi_Mid_Tom"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Crash_Cymbal_1, "Crash_Cymbal_1"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::High_Tom, "High_Tom"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Ride_Cymbal_1, "Ride_Cymbal_1"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Chinese_Cymbal, "Chinese_Cymbal"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Ride_Bell, "Ride_Bell"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Tambourine, "Tambourine"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Splash_Cymbal, "Splash_Cymbal"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Cowbell, "Cowbell"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Crash_Cymbal_2, "Crash_Cymbal_2"),
	  UIEnumVal(DRUM_CHANNEL_INSTRUMENTS::Ride_Cymbal_2, "Ride_Cymbal_2")
  };
  const uint32_t UI::num_drums_ = 26;

  UI::UI():EngineObject("UI") 
  {
	  ui_running_ = false;
	  render_interface_ = NULL;
	  system_interface_ = NULL;
	  file_interface_ = NULL;
	  event_listener_ = NULL;
	  midi_doc_ = NULL;
	  app_doc_ = NULL;
	  fps_doc_ = NULL;
	  text_doc_ = NULL;
	  mouse_over_count_ = 0;
	  context_ = NULL;
	  button_callbacks_ = NULL;
	  app_text_str2ind_ = NULL;
	  app_content_ = NULL;
	  midi_content_ = NULL;
	  fps_content_ = NULL;
	  text_content_ = NULL;

    frame_time_accum_ = 0;
    frame_count_ = 0;
    min_fps_ = std::numeric_limits<double>::infinity();
    max_fps_ = 0;
  }

  UI::~UI() {
    // The order of calls in this destructor is VERY important.  You must
    // remove document and context references first.  Request shutdown, THEN
    // delete the interface elements.
	  SAFE_REMOVE_REFERENCE(app_doc_);
	  SAFE_REMOVE_REFERENCE(midi_doc_);
	  SAFE_REMOVE_REFERENCE(fps_doc_);
	  for (uint32_t i = 0; i < app_text_doc_.size(); i++) {
		  SAFE_REMOVE_REFERENCE(app_text_doc_[i]);
	  }
	  SAFE_REMOVE_REFERENCE(context_);
	  Rocket::Core::Shutdown();

	  ui_running_ = false;
	  SAFE_DELETE(render_interface_);
	  SAFE_DELETE(file_interface_);
	  SAFE_DELETE(event_listener_);
	  SAFE_DELETE(system_interface_);
	  SAFE_DELETE(button_callbacks_);
	  SAFE_DELETE(app_text_str2ind_);

  }

  void UI::loadRocketFontSafe(const String& file_name) {
    if (!Rocket::Core::FontDatabase::LoadFontFace(file_name)) {
		std::stringstream ss;
		ss << "UI::loadRocketFontSafe() - ERROR: LoadFontFace failed trying to load font: "
			<< file_name.CString();
      throw runtime_error(ss.str());
    }
  }

  void UI::init() {
    registerObject();

    if (ui_running_) {
      throw std::runtime_error("UI::init() - UI is already running!");
    }

    button_callbacks_ = new std::unordered_map<std::string, ButtonCallback>();
    app_text_str2ind_ = new std::unordered_map<std::string, uint32_t>();

    //render_interface_ = new RocketRenderInterface(Renderer::g_renderer());
    render_interface_ = new RocketRenderInterface();
    system_interface_ = new RocketSystemInterface();
    file_interface_ = new RocketFileInterface(ROCKET_RESOURCE_PATH);
    event_listener_ = new RocketEventListener(this);

    Rocket::Core::SetFileInterface(file_interface_);
    Rocket::Core::SetRenderInterface(render_interface_);
    Rocket::Core::SetSystemInterface(system_interface_);

    if(!Rocket::Core::Initialise()) {
		  throw runtime_error("UI::init() - ERROR: Rocket::Core::"
        "Initialise() failed!");
    }
    Rocket::Controls::Initialise();

    loadRocketFontSafe("Delicious-Bold.otf");
    loadRocketFontSafe("Delicious-BoldItalic.otf");
    loadRocketFontSafe("Delicious-Italic.otf");
    loadRocketFontSafe("Delicious-Roman.otf");

    createUIContext();

    loadUIContent();

    ui_running_ = true;
  }

  void UI::createUIContext() {
    Vector2i dimensions((int)WindowManager::_mainWindowSize.x, (int)WindowManager::_mainWindowSize.y);
    context_ = Rocket::Core::CreateContext("default", dimensions);
    if (context_ == NULL) {
      throw runtime_error("UI::init() - ERROR: Rocket::Core::" 
        "CreateContext() failed!");
    }
    context_->AddEventListener("mouseover", event_listener_);
    context_->AddEventListener("mouseout", event_listener_);
    context_->AddEventListener("dragstart", event_listener_);
    context_->AddEventListener("dragend", event_listener_);

    Rocket::Debugger::Initialise(context_);
    debugger_visible_ = false;
    Rocket::Debugger::SetVisible(debugger_visible_);
  }

  void UI::loadUIContent() {
	  loadRocketDocument(UI_FPS_DOC, fps_doc_, fps_content_);
	  loadFPSElements();
	  setVisibility(false, fps_doc_);

	  Vector2 ui_size(1440, 100);

	  loadRocketDocument(UI_SETTINGS_DOC, text_doc_, text_content_);
	  loadTextElements();
	  setVisibility(false, text_doc_);
	  setDocSize(ui_size, text_doc_);

	  ui_size.set(350, 200);
	  loadRocketDocument(UI_SETTINGS_DOC, midi_doc_, midi_content_);
	  loadRendererElements();
	  setVisibility(false, midi_doc_);
	  setDocSize(ui_size, midi_doc_);

	  ui_size.set(250, 200);
	  loadRocketDocument(UI_SETTINGS_DOC, app_doc_, app_content_);
	  loadAppElements();
	  setVisibility(true, app_doc_);
	  setDocSize(ui_size, app_doc_);


#if defined(__APPLE__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

	  // Shift the render document so it's off to the right
	  float left_render = 0.0f;
	  left_render = app_doc_->GetProperty("left")->value.Get<float>();
	  float width_app = 0.0f;
	  width_app = app_doc_->GetProperty("width")->value.Get<float>();
	  float val_float = left_render + width_app;
	  std::string val = std::to_string(val_float);
	  const char* val_c_str = val.c_str();
	  midi_doc_->SetProperty("left", val_c_str);
	  midi_doc_->UpdateLayout();

	  // Shift the text document so it's on the bottom
	  float win_height = RenderManager::getInstance().getFramebufferSize().y;
	  float height_doc = 0.0f;
	  height_doc = text_doc_->GetProperty("height")->value.Get<float>();
	  val_float = win_height - height_doc - 80;
	  val = std::to_string(val_float);
	  val_c_str = val.c_str();
	  text_doc_->SetProperty("top", val_c_str);
    
	  float win_width = RenderManager::getInstance().getFramebufferSize().x;
	  val = std::to_string(win_width - 20);
	  val_c_str = val.c_str();
	  text_doc_->SetProperty("width", val_c_str);
	  text_doc_->UpdateLayout();
#if defined(__APPLE__)
#pragma GCC diagnostic pop
#endif    

  }

  void UI::loadRocketDocument(const char* file, 
    Rocket::Core::ElementDocument*& doc, Rocket::Core::Element*& content) 
    const {
    if (doc) {
      doc->RemoveReference();
    }
    doc = context_->LoadDocument(file);
    if (!doc) {
      throw runtime_error(std::string("UI::loadRocketDocument() - ERROR:"
        " failed trying to load document: ") + std::string(file));
    }
    content = doc->GetElementById("content");
    if (content == NULL) {
      throw runtime_error("UI::loadRocketDocument() - couldnt find content");
    }
  }

  void UI::setDocumentTitle(Rocket::Core::ElementDocument* doc,
    const std::string& text) const {
    Element* title_elem = doc->GetElementById("title");
    if (title_elem) {
      title_elem->SetInnerRML(text.c_str());
    } else {
      throw std::runtime_error(" UI::setDocumentTitle() - ERROR: "
        "Couldn't find title element!");
    }
  }

  void UI::setDocSize(const Vector2& size,
    Rocket::Core::ElementDocument* doc) {
    doc->SetProperty("width", std::to_string((int)size.x).c_str());
    doc->SetProperty("height", std::to_string((int)size.y).c_str());
    doc->UpdateLayout();
  }

  void UI::addCloseButton(Rocket::Core::ElementDocument* doc, 
    ButtonCallback close_func, const char* close_button_name) const {
    Element* title_elem = doc->GetElementById("title_bar");
    if (!title_elem) {
      throw std::runtime_error(" UI::addCloseButton() - ERROR: Couldn't find title element!");
      return;
    }

    Element* sel_elem = doc->CreateElement("closebutton");
    if (sel_elem == NULL) {
      throw runtime_error("UI::addCloseButton() - couldn't create close elem!");
      return;
    }
    sel_elem->SetId("close_button");
    const char* val = "button";
    const char* name = "type";
    sel_elem->SetAttribute(name, val);
    // Generate a unique identifier
    sel_elem->SetAttribute("value", close_button_name);
    sel_elem->AddEventListener("click", event_listener_, true);
    std::pair<std::string, ButtonCallback> p(close_button_name,close_func);
    button_callbacks_->insert(p);

    title_elem->AppendChild(sel_elem);
    sel_elem->RemoveReference();

  }

  /*
  void UI::reloadUIContent() {
    UI* g_ui = Renderer::g_renderer()->ui();
    SAFE_REMOVE_REFERENCE(g_ui->midi_doc_);
    SAFE_REMOVE_REFERENCE(g_ui->fps_doc_);
    for (uint32_t i = 0; i < app_text_doc_.size(); i++) {
      SAFE_REMOVE_REFERENCE(app_text_doc_[i]);
    }
    g_ui->context_->UnloadAllDocuments();
    Rocket::Core::Factory::ClearStyleSheetCache();
    Rocket::Core::ReleaseTextures();
    Rocket::Core::ReleaseCompiledGeometries();

    g_ui->context_->Update();  // Releases unloaded documents

    g_ui->loadUIContent();
  }
  */

  void UI::loadFPSElements() const {
    // Create dynamic elements here:
    addHeadingText("FPS:", fps_doc_, fps_content_, "fps_text");
  }

  void UI::addCheckbox(const char* bool_setting_name, const char* text) const {
    addCheckbox(bool_setting_name, text, app_doc_, app_content_);
  }

  void UI::addCheckbox(const char* name, const char* text, 
    Rocket::Core::ElementDocument* doc, Rocket::Core::Element* content) const {
    if (!doc) {
      doc = midi_doc_; 
    }
    if (!content) {
      content = midi_content_; 
    }

    // Add a checkbox input element
    Element* checkbox_elem = doc->CreateElement("input");
    checkbox_elem->SetAttribute("type", "checkbox");
    checkbox_elem->SetAttribute("value", name);
    checkbox_elem->SetProperty("overflow-x", "auto");
    checkbox_elem->SetProperty("overflow-y", "auto");
    checkbox_elem->SetProperty("clip", "auto");
    checkbox_elem->SetId(name);
    checkbox_elem->AddEventListener("change", event_listener_, false);

    bool val = true;
    //TODO: Replicate settings system
    //GET_SETTING(name, bool, val);
    if (val) {
      checkbox_elem->SetAttribute("checked", "");
    } else {
      checkbox_elem->RemoveAttribute("checked");
    }
    content->AppendChild(checkbox_elem);
    checkbox_elem->RemoveReference();

    // Add a text element for the text string
    Element* text_elem = doc->CreateTextNode(text);
    if (text_elem == NULL) {
      throw runtime_error("UI::addCheckbox() - couldn't create text elem!");
    }
    text_elem->SetInnerRML("<br/>");
    content->AppendChild(text_elem);
    text_elem->RemoveReference();
  }

  void UI::addHeadingText(const char* text, const char* elem_name) const {
    addHeadingText(text, app_doc_, app_content_, elem_name);
  }

  void UI::addHeadingText(const char* text, Rocket::Core::ElementDocument* doc, 
    Rocket::Core::Element* content, const char* name) const {
    // Add a text element for the text string
    Element* text_elem = doc->CreateTextNode(text);
    if (text_elem == NULL) {
      throw runtime_error("UI::addText() - couldn't create text elem!");
    }
    if (name != NULL) {
      text_elem->SetId(name);
    }
    text_elem->SetInnerRML("<br/>");
    content->AppendChild(text_elem);
    text_elem->RemoveReference();
  }
  
  void UI::addPreText(const char* text, Rocket::Core::ElementDocument* doc,
	  Rocket::Core::Element* content, const char* name) const {
	  // Add a text element for the text string
	  Element* text_elem = doc->CreateTextNode(text);
	  if (text_elem == NULL) {
		  throw runtime_error("UI::addText() - couldn't create text elem!");
	  }
	  if (name != NULL) {
		  text_elem->SetId(name);
	  }
	  text_elem->SetProperty("white-space", "pre-wrap");
	  text_elem->SetInnerRML("<br/>");
	  content->AppendChild(text_elem);
	  text_elem->RemoveReference();
  }

  void UI::setSelectboxVal(const std::string& name, const int val) const {
    setSelectboxVal(name, val, app_doc_);
  }

  void UI::addButton(const char* elem_name, const char* text, 
    ButtonCallback func) const { 
    addButton(elem_name, text, func, app_doc_, app_content_);
  }

  void UI::setSelectboxVal(const std::string& name, const int val, 
    Rocket::Core::ElementDocument* doc) const {
    Element* elem = doc->GetElementById(name.c_str());
    if (elem == NULL) {
      std::stringstream ss;
      ss << "UI::changeCheckboxValue() - ERROR: Couldn't find an element";
      ss << " with the ID: " << name.c_str();
      throw runtime_error(ss.str());
    }
    String elem_type = elem->GetAttribute("type")->Get<String>();
    if (elem_type != "selectbox") {
      std::stringstream ss;
      ss << "UI::setSelectboxVal() - ERROR: Found an element";
      ss << " with the ID: " << name.c_str() << L", but it is not a";
      ss << " selectbox";
      throw runtime_error(ss.str());
    }

    int cval = -1;
    for (int32_t i = 0; i < elem->GetNumChildren() && cval == -1; i++) {
      Element* child_elem = elem->GetChild(i);
      if (::atoi(child_elem->GetAttribute("value")->Get<String>().CString()) == 
        val) {
        cval = i;
      }
    }
    if (cval != -1) {
      dynamic_cast<ElementFormControlSelect*>(elem)->SetSelection(cval);
    }
  }

  void UI::addSelectbox(const char* int_setting_name, const char* text, 
    const UIEnumVal box_vals[], const uint32_t num_vals, 
    Rocket::Core::ElementDocument* doc, Rocket::Core::Element* content) const {
    // Create the containing select element
    Element* sel_elem = doc->CreateElement("select");
    if (sel_elem == NULL) {
      throw runtime_error("UI::addSelectbox() - couldn't create select "
        "elem!");
    }
    sel_elem->SetId(int_setting_name);
    sel_elem->SetAttribute("type", "selectbox");
    sel_elem->SetAttribute("value", int_setting_name);
    content->AppendChild(sel_elem);

    // Add the option elements:
    for (uint32_t i = 0; i < num_vals; i ++) {
      Element* option_elem = doc->CreateElement("option");
      if (option_elem == NULL) {
        throw runtime_error("UI::addSelectbox() - couldn't create option "
          "elem!");
      }
      option_elem->SetInnerRML(box_vals[i].Label);
      option_elem->SetAttribute("value", box_vals[i].Value);
      sel_elem->AppendChild(option_elem);
      option_elem->RemoveReference();
    }

    int value = 0;
    //GET_SETTING(int_setting_name, int, value);
    // Now we need to find out which selection value in the array corresponds
    // to the selection (they might be out of order compared to the enum val)
    int cval = -1;
    for (uint32_t i = 0; i < num_vals && cval == -1; i++) {
      if (box_vals[i].Value == value) {
        cval = i;
      }
    }
    if (cval == -1) {
        std::cerr << "UI::addSelectbox() - A combo-box value corresponding to "
			   << "the current setting value does not exist!" << std::endl;
      //throw runtime_error("UI::addSelectbox() - A combo-box value "
      //  "corresponding to the current setting value does not exist!");
    }
    else{
      dynamic_cast<ElementFormControlSelect*>(sel_elem)->SetSelection(cval);
    }

    sel_elem->AddEventListener("change", event_listener_, false);
    sel_elem->RemoveReference();

    // Add a text element for the text string
    Element* text_elem = doc->CreateTextNode(text);
    if (text_elem == NULL) {
      throw runtime_error("UI::addSelectbox() - couldn't create text elem!");
    }
    text_elem->SetInnerRML("<br/>");
    content->AppendChild(text_elem);
    text_elem->RemoveReference();
  }

  void UI::addSelectboxItem(const char* int_setting_name, 
    const UIEnumVal& item) const {
    Element* sel_elem = app_content_->GetElementById(int_setting_name);
    if (app_content_ == NULL) {
      throw runtime_error("UI::addSelectboxVal() - couldn't find selectbox "
        "elem!");
    }

    // Add the option element:
    Element* option_elem = app_doc_->CreateElement("option");
    if (option_elem == NULL) {
      throw runtime_error("UI::addSelectbox() - couldn't create option "
        "elem!");
    }
    option_elem->SetInnerRML(item.Label);
    option_elem->SetAttribute("value", item.Value);
    sel_elem->AppendChild(option_elem);
    option_elem->RemoveReference();

    int value = 0;
    //GET_SETTING(int_setting_name, int, value);
    // Now we need to find out which selection value in the array corresponds
    // to the selection (they might be out of order compared to the enum val)
    int cval = -1;
    for (int32_t i = 0; i < sel_elem->GetNumChildren() && cval == -1; i++) {
      Element* elem = sel_elem->GetChild(i);
      if (::atoi(elem->GetAttribute("value")->Get<String>().CString()) == 
        value) {
        cval = i;
      }
    }
    if (cval != -1) {
      dynamic_cast<ElementFormControlSelect*>(sel_elem)->SetSelection(cval);
    }
  }

  void UI::addSelectbox(const char* int_setting_name, const char* text) const {
    // Create the containing select element
    Element* sel_elem = app_doc_->CreateElement("select");
    if (sel_elem == NULL) {
      throw runtime_error("UI::addSelectbox() - couldn't create select "
        "elem!");
    }
    sel_elem->SetId(int_setting_name);
    sel_elem->SetAttribute("type", "selectbox");
    sel_elem->SetAttribute("value", int_setting_name);
    app_content_->AppendChild(sel_elem);

    sel_elem->AddEventListener("change", event_listener_, false);
    sel_elem->RemoveReference();

    // Add a text element for the text string
    Element* text_elem = app_doc_->CreateTextNode(text);
    if (text_elem == NULL) {
      throw runtime_error("UI::addSelectbox() - couldn't create text elem!");
    }
    text_elem->SetInnerRML("<br/>");
    app_content_->AppendChild(text_elem);
    text_elem->RemoveReference();
  }
  void UI::addSlider(const char* name, const char* text, float min, float max, 
    Rocket::Core::ElementDocument* doc, Rocket::Core::Element* content) const {
    //
    //if (!doc) {
    //  doc = midi_doc_; 
    //}
    //if (!content) {
    //  content = midi_content_; 
    //}

    //// Create the containing select element
    //Element* sel_elem = doc->CreateElement("input");
    //if (sel_elem == NULL) {
    //  throw wruntime_error(L"UI::addSlider() - couldn't create range elem!");
    //}
    //sel_elem->SetId(name);
    //sel_elem->SetAttribute("type", "range.horizontal");
    //sel_elem->SetAttribute("min", "0");
    //sel_elem->SetAttribute("max", "100");
    ////sel_elem->SetAttribute("value", name);
    //content->AppendChild(sel_elem);
    //
    //
    //// Add the option elements:
    //for (uint32_t i = 0; i < num_vals; i ++) {
    //  Element* option_elem = doc->CreateElement("option");
    //  if (option_elem == NULL) {
    //    throw wruntime_error(L"UI::addSelectbox() - couldn't create option "
    //      L"elem!");
    //  }
    //  option_elem->SetInnerRML(box_vals[i].Label);
    //  option_elem->SetAttribute("value", box_vals[i].Value);
    //  sel_elem->AppendChild(option_elem);
    //  option_elem->RemoveReference();
    //}

    //int value = 0;
    //GET_SETTING(name, int, value);
    //// Now we need to find out which selection value in the array corresponds
    //// to the selection (they might be out of order compared to the enum val)
    //int cval = -1;
    //for (uint32_t i = 0; i < num_vals && cval == -1; i++) {
    //  if (box_vals[i].Value == value) {
    //    cval = i;
    //  }
    //}
    //if (cval == -1) {
    //  throw wruntime_error("UI::addSelectbox() - A combo-box value "
    //    "corresponding to the current setting value does not exist!");
    //}
    //dynamic_cast<ElementFormControlSelect*>(sel_elem)->SetSelection(cval);

    //sel_elem->AddEventListener("change", event_listener_, false);
    //sel_elem->RemoveReference();

    //// Add a text element for the text string
    //Element* text_elem = doc->CreateTextNode(text);
    //if (text_elem == NULL) {
    //  throw wruntime_error(L"UI::addSelectbox() - couldn't create text elem!");
    //}
    //text_elem->SetInnerRML("<br/>");
    //content->AppendChild(text_elem);
    //text_elem->RemoveReference();
    //
  }

  void UI::addButton(const char* name, const char* text, ButtonCallback func,
    Rocket::Core::ElementDocument* doc, Rocket::Core::Element* content) const {
    // Create the containing select element
    Element* sel_elem = doc->CreateElement("button");
    if (sel_elem == NULL) {
      throw runtime_error("UI::addButton() - couldn't create button elem!");
    }
    sel_elem->SetId(name);
    sel_elem->SetAttribute("type", "button");
    sel_elem->SetAttribute("value", name);
    sel_elem->AddEventListener("click", event_listener_, true);
    button_callbacks_->insert({ name, func });

    Element* text_elem = doc->CreateTextNode(text);
    if (text_elem == NULL) {
      throw runtime_error("UI::addButton() - couldn't create text elem!");
    }
    sel_elem->AppendChild(text_elem);
    text_elem->RemoveReference();

    content->AppendChild(sel_elem);
    sel_elem->RemoveReference();

    addLineBreak(doc, content);
  }

  void UI::addTitleBarButton(Rocket::Core::ElementDocument* doc,
	  ButtonCallback func, const char* name, const char* text) const {
	  Element* title_elem = doc->GetElementById("title_bar");
	  if (!title_elem) {
		  throw std::runtime_error(" UI::addCloseButton() - ERROR: Couldn't find title element!");
		  return;
	  }

	  Element* sel_elem = doc->CreateElement("button");
	  if (sel_elem == NULL) {
		  throw runtime_error("UI::addTitleBarButton() - couldn't create close elem!");
		  return;
	  }
	  sel_elem->SetId(name);
	  sel_elem->SetAttribute("type", "button");
	  sel_elem->SetAttribute("value", name);
	  sel_elem->AddEventListener("click", event_listener_, true);
	  button_callbacks_->insert({ name, func });

	  Element* text_elem = doc->CreateTextNode(text);
	  if (text_elem == NULL) {
		  throw runtime_error("UI::addTitleBarButton() - couldn't create text elem!");
	  }
	  sel_elem->AppendChild(text_elem);
	  text_elem->RemoveReference();

	  title_elem->AppendChild(sel_elem);
	  sel_elem->RemoveReference();
  }

  void UI::createTextWindow(const char* wnd_name, const char* str) {
    uint32_t index;
    auto search = app_text_str2ind_->find(wnd_name);
    //std::unordered_map<std::string, uint32_t>::iterator search = app_text_str2ind_->find(name);
    if (search == app_text_str2ind_->end()){
      throw std::runtime_error("UI::createTextWindow() - ERROR: "
        "A window with this name already exists");
    }
    app_text_doc_.push_back(NULL);
    app_text_content_.push_back(NULL);
    index = (uint32_t)app_text_doc_.size() - 1;
    app_text_str2ind_->insert({ wnd_name, index });

    loadRocketDocument(UI_TEXT_DOC, app_text_doc_[index], 
      app_text_content_[index]);
    setDocumentTitle(app_text_doc_[index], wnd_name);
    addHeadingText(str, app_text_doc_[index], app_text_content_[index], 
      "app_text");
  }

  const uint32_t UI::getTextWindowID(const std::string& name) const {
    auto search = app_text_str2ind_->find(name);
    //std::unordered_map<std::string, uint32_t>::iterator search = app_text_str2ind_->find(name);
    if (search == app_text_str2ind_->end()){
      throw std::runtime_error("UI::getTextWindowID() - ERROR: "
        "A window with this name does not exist!");
    }
    return search->second;
  }

  void UI::setTextWindowString(const char* wnd_name, const char* text) {
    uint32_t index = getTextWindowID(wnd_name);
    Element* elem = app_text_doc_[index]->GetElementById("app_text");
    if (elem == NULL) {
      throw runtime_error("UI::setTextWindowString() - INTERNAL ERROR: "
        " Couldn't not find text element to modify!");
    }
    reinterpret_cast<ElementText*>(elem)->SetText(text);
  }

  void UI::setTextWindowPos(const char* wnd_name, const vmath::Vector2& pos) {
    uint32_t index = getTextWindowID(wnd_name);
    app_text_doc_[index]->SetProperty("left", 
      std::to_string(pos[0]).c_str());
    app_text_doc_[index]->SetProperty("top", 
      std::to_string(pos[1]).c_str());
  }

  int UI::getElementWidth(const char* elem_name) {
    Element* elem = app_doc_->GetElementById(elem_name);
    int width = elem->GetProperty<int>("width");
    return width;
  }

  void UI::setElementWidth(const char* elem_name, int width) {
    Element* elem = app_doc_->GetElementById(elem_name);
    elem->SetProperty("width", std::to_string(width).c_str());
  }

  void UI::setTextWindowVisibility(const char* wnd_name, const bool visible) {
    uint32_t index = getTextWindowID(wnd_name);
    setVisibility(visible, app_text_doc_[index]);
  }

  void UI::addLineBreak(Rocket::Core::ElementDocument* doc, 
    Rocket::Core::Element* content) const {
    // Create an empty text element so we can insert a line break
    Element* text_elem = doc->CreateTextNode(" ");
    if (text_elem == NULL) {
      throw runtime_error("UI::addButton() - couldn't create text elem!");
    }
    content->AppendChild(text_elem);
    text_elem->SetInnerRML("<br/>");
    text_elem->RemoveReference();
  }
  void UI::renderFrame() const {
    if (!ui_running_) {
      return;
    }

    if (context_) {
      context_->Render();
    }
  }

  void UI::keyboardInputCB(int key, int scancode, int action, int mods) const {
    if (!ui_running_) {
      return;
    }
    if (context_) {
      if (action == KeyInfo::KeyAction::PRESS) {
        context_->ProcessKeyDown(system_interface_->TranslateKey(key), 
          system_interface_->GetKeyModifiers());
      } else {
        context_->ProcessKeyUp(system_interface_->TranslateKey(key), 
          system_interface_->GetKeyModifiers());
      }
//      if (action == PRESSED) {
//        context_->ProcessKeyDown(system_interface_->TranslateKey(key), 
//          system_interface_->GetKeyModifiers(Renderer::g_renderer()));
//      } else {
//        context_->ProcessKeyUp(system_interface_->TranslateKey(key), 
//          system_interface_->GetKeyModifiers(Renderer::g_renderer()));
//      }
    }
  }

  void UI::mousePosCB(double x, double y) const {
    if (!ui_running_) {
      return;
    }
    if (context_) {
//      context_->ProcessMouseMove((int)floor(x), (int)floor(y), 
//        system_interface_->GetKeyModifiers(Renderer::g_renderer()));
      context_->ProcessMouseMove((int)floor(x), (int)floor(y),
        system_interface_->GetKeyModifiers());
    }
  }

  void UI::mouseButtonCB(int button, int action, int mods) const {
    if (!ui_running_) {
      return;
    }
    if (context_) {
//      if (action == PRESSED) {
//        context_->ProcessMouseButtonDown(button, 
//          system_interface_->GetKeyModifiers(Renderer::g_renderer()));
//      } else {
//        context_->ProcessMouseButtonUp(button, 
//          system_interface_->GetKeyModifiers(Renderer::g_renderer()));
//      }
      if (action == KeyInfo::KeyAction::PRESS) {
        context_->ProcessMouseButtonDown(button, 
          system_interface_->GetKeyModifiers());
      } else {
        context_->ProcessMouseButtonUp(button, 
          system_interface_->GetKeyModifiers());
      }
    }
  }

  void UI::mouseWheelCB(double xoffset, double yoffset) const {
    if (!ui_running_) {
      return;
    }
    context_->ProcessMouseWheel(-(int)floor(yoffset), 
                                system_interface_->GetKeyModifiers());
//    context_->ProcessMouseWheel(-(int)floor(yoffset),
//      system_interface_->GetKeyModifiers(Renderer::g_renderer()));
  }

  void UI::setWindowSize(const int width, const int height) {
    if (!ui_running_) {
      return;
    }
    if (context_) {
      Vector2i dimensions(width, height);
      context_->SetDimensions(dimensions);
    }
    if (render_interface_) {
      render_interface_->resize(width, height);
    }
    // loadDocumentAndElements();  // Reload the document
  }

  void UI::update(const double dt) {
    if (context_) {
      context_->Update();
    }

    bool render_ui_fps = true;
    //GET_SETTING("render_ui_fps", bool, render_ui_fps);
    //setVisibility(render_ui_fps, fps_doc_);
    if (render_ui_fps) {
      fps_doc_->PushToBack();
      // Update the framerate counter
      frame_time_accum_ += dt;
      frame_count_++;
      double cur_frame_fps = 1.0 / dt;
      min_fps_ = std::min<double>(min_fps_, cur_frame_fps);
      max_fps_ = std::max<double>(max_fps_, cur_frame_fps);
      if (frame_time_accum_ >= 1.0f) {
        double fps = frame_count_ / frame_time_accum_;
        if (fps_doc_) {
          Element* elem = fps_doc_->GetElementById("fps_text");
          if (elem == NULL) {
            throw runtime_error("UI::update() - couldnt find fps_text!");
          }
          sprintf(fps_buffer_, "FPS: %.1f (AVE) %.1f (MIN) %.1f (MAX)",
            fps, min_fps_, max_fps_);
          reinterpret_cast<ElementText*>(elem)->SetText(fps_buffer_);
        }
        min_fps_ = std::numeric_limits<double>::infinity();
        max_fps_ = 0;
        frame_count_ = 0;
        frame_time_accum_ = 0;
      }
    }
  }

  void UI::debugger_visible(bool val) {
    debugger_visible_ = val;
    Rocket::Debugger::SetVisible(val);
  }

  unsigned long UI::event_count_ = 0;
  void UI::processEvent(const Rocket::Core::Event& event) {
    Rocket::Core::String type = event.GetType();
    event_count_++;

    //if (event == "resize") {
    //  if (midi_content_ != NULL && midi_content_->HasChildNodes()) {
    //    midi_content_->GetLastChild()->ScrollIntoView();
    //  }
    //  return;
    //}

    if (type == "mouseover" || type == "dragstart") {
      mouse_over_count_++;
    } else if (type == "mouseout" || type == "dragend") {
      mouse_over_count_--;
    } else if (type == "change") {
      // An input element changed:
      Element* target = event.GetTargetElement();
      if (target != NULL) {
        // All our "changable" elements need to have a type attribute (we
        // need to put them there).  Make sure this one has one and that
        // it is of type string.
        if (target->GetAttribute("type") == NULL || 
          target->GetAttribute("type")->GetType() != Variant::Type::STRING) {
            std::stringstream ss;
            ss << "UI::processEvent() - ERROR: the current target element";
            ss << " either does not have an attribute named type or that";
            ss << " attribute is not of type string";
            throw runtime_error(ss.str());
        }
        String elem_type = target->GetAttribute("type")->Get<String>();
        // Do the same for the value element (which stores the setting string)
        if (target->GetAttribute("value") == NULL || 
          target->GetAttribute("value")->GetType() != Variant::Type::STRING) {
            std::stringstream ss;
            ss << "UI::processEvent() - ERROR: the current target element";
            ss << " either does not have an attribute named value or that";
            ss << " attribute is not of type string";
            throw runtime_error(ss.str());
        }
        String val_str = target->GetAttribute("value")->Get<String>();

        std::cout << "UI processing event type: " << type.CString();
        std::cout << " val_str: " << val_str.CString() << std::endl;
        // Now handle the different types of elements
        if (elem_type == "checkbox") {  // Handle checkbox
          bool old_value = false;
          //GET_SETTING(val_str.CString(), bool, old_value);

          // Set the internal setting (in the setting manager)
          bool value = false;
          if (target->GetAttribute("checked")) {
            value = true;
          }
          //SET_SETTING(val_str.CString(), bool, value);

          // Handle special cases (that need to do something immediately)
          if (val_str == "fullscreen") {
            if (old_value != value) {
              //Renderer::requestReloadRenderer();
              return;
            }
          }

          // Handle special cases that require the app to do something after 
          // the variable has been set.
          if (val_str == "render_ui") {
            setVisibility(value, midi_doc_);
          }
          if (val_str == "render_ui_fps") {
            setVisibility(value, fps_doc_);
          }
          if (val_str == "use_drums") {

            EngineObjectMessage msg("MidiDrum", "UseDrums", &value);
            sendOutMessage(msg);

          }
        } else if (elem_type == "selectbox") {
          Rocket::Controls::ElementFormControlSelect* select = 
            dynamic_cast<Rocket::Controls::ElementFormControlSelect*>(target);
          int cval = select->GetSelection();
          Rocket::Controls::SelectOption* sel_opt = select->GetOption(cval);
          int val = ::atoi(sel_opt->GetValue().CString());

          //SET_SETTING(val_str.CString(), int, val);

          // Handle special cases that require the app to do something after
          // the variable has been set
          if (val_str == "instrument") {
            //TODO: use message system to send message to MidiDrum

            EngineObjectMessage msg("MidiDrum", "InstrumentSelected", &val);
            sendOutMessage(msg);

          }
          if (val_str == "drums") {

            EngineObjectMessage msg("MidiDrum", "DrumSelected", &val);
            sendOutMessage(msg);

          }
          if (val_str == "midi_output"){
            EngineObjectMessage msg("MidiDrum", "UseMidiPort", &val);
            sendOutMessage(msg);
          }
        }
      }  // end if (target != NULL)
    } else if (type == "click") {
      Element* target = event.GetTargetElement();
      if (target != NULL) {
        String val_str = target->GetAttribute("value")->Get<String>();

        std::cout << "UI processing event type: " << type.CString();
        std::cout << " val_str: " << val_str.CString() << std::endl;

        ButtonCallback func;
        auto search = button_callbacks_->find(val_str.CString());
        if (search == button_callbacks_->end()) {
        //if (!button_callbacks_->lookup(val_str.CString(), func)) {
          throw runtime_error(std::string("UI::processEvent() - No button "
            "callback with name ") + val_str.CString());
        }
        func = search->second;
        if (func != NULL) {
          func();
        }
      } else {
        throw runtime_error("UI::processEvent() - Button has no target!");
      }
    }
    if (mouse_over_count_ < 0) {
      throw runtime_error("UI::processEvent() - ERROR: mouse_over_count < 0");
    }
  }

  void UI::setVisibility(const bool vis, Rocket::Core::ElementDocument*& doc) {
    if (vis) {
      doc->Show();
    } else {
      doc->Hide();
    }
  }

  void UI::setSettingsVisibility(const bool visible) const {
    if (visible) {
      app_doc_->Show();
    } else {
      app_doc_->Hide();
    }
  }

  void UI::setRendererCheckboxVal(const std::string& name, const bool val) 
    const {
    setCheckboxVal(name, val, midi_doc_);
  } 

  void UI::setCheckboxVal(const std::string& name, const bool val) 
    const {
    setCheckboxVal(name, val, app_doc_);
  } 

  void UI::setCheckboxVal(const std::string& name, const bool val,
    Rocket::Core::ElementDocument* doc) const {
    Element* elem = doc->GetElementById(String(name.c_str()));
    if (elem == NULL) {
      std::stringstream ss;
      ss << "UI::changeCheckboxValue() - ERROR: Couldn't find an element";
      ss << " with the ID: " << name.c_str();
      throw runtime_error(ss.str());
    }
    String elem_type = elem->GetAttribute("type")->Get<String>();
    if (elem_type != "checkbox") {
      std::stringstream ss;
      ss << "UI::changeCheckboxValue() - ERROR: Found an element";
      ss << " with the ID: " << name.c_str() << L", but it is not a";
      ss << " checkbox";
      throw runtime_error(ss.str());
    }
    if (val) {
      if (elem->GetAttribute("checked") == NULL) {
       elem->SetAttribute("checked", "");
      }
    } else {
      if (elem->GetAttribute("checked") != NULL) {
        elem->RemoveAttribute("checked");
      }
    }
  } 

  void UI::loadRendererElements() const {
    Rocket::Core::ElementDocument* d = midi_doc_;
    Rocket::Core::Element* c = midi_content_;

    setDocumentTitle(d, "Midi Settings");
    addCloseButton(d, hideMidiDoc, "hide_render_doc_button");

    // Create dynamic elements here:
    addHeadingText("Midi Settings:", d, c);
    addSelectbox("instrument", "Instrument", instruments_, num_instruments_, d, c);
    addCheckbox("use_drums", "Use Drum Channel", app_doc_, app_content_);
    addSelectbox("drums", "Drum Channel", drums_, num_drums_, d, c);

    
  }

  void UI::loadAppElements() const {
    setDocumentTitle(app_doc_, "Settings");
    addCloseButton(app_doc_, hideAppDoc, "hide_app_doc_button");

    addHeadingText("UI Settings:", app_doc_, app_content_);
    addCheckbox("render_ui_fps", "Render FPS", app_doc_, app_content_);

    addButton("show_midi_doc__button", "Midi Settings", 
      UI::showMidiDoc, app_doc_, app_content_);

    addButton("show_text_doc_button", "Console", 
      UI::showTextDoc, app_doc_, app_content_);
  }

  void UI::loadTextElements() const {
	  setDocumentTitle(text_doc_, "Console");
    addPreText("", text_doc_, text_content_, "log_text");
	  addTitleBarButton(text_doc_, clearLog, "clear_console", "Clear Console");
	  addCloseButton(text_doc_, hideTextDoc, "hide_text_doc_button");

  }

  void UI::showMidiDoc() {
    UI* ui = UI::getInstance();
    ui->setVisibility(true, ui->midi_doc_);
    //SET_SETTING("render_ui_renderer", bool, true);
  }

  void UI::hideMidiDoc() {
    UI* ui = UI::getInstance();
    ui->setVisibility(false, ui->midi_doc_);
    //SET_SETTING("render_ui_renderer", bool, false);
  }

  void UI::showAppDoc() {
    UI* ui = UI::getInstance();
    ui->setVisibility(true, ui->app_doc_);
    //SET_SETTING("render_ui_app", bool, true);
  }

  void UI::hideAppDoc() {
    UI* ui = UI::getInstance();
    ui->setVisibility(false, ui->app_doc_);
    //SET_SETTING("render_ui_app", bool, false);
  }

  void UI::showTextDoc() {
    UI* ui = UI::getInstance();
    ui->setVisibility(true, ui->text_doc_);
	  //SET_SETTING("render_ui_text", bool, true);
  }

  void UI::hideTextDoc() {
    UI* ui = UI::getInstance();
    ui->setVisibility(false, ui->text_doc_);
	  //SET_SETTING("render_ui_text", bool, false);
  }
  
  void UI::hideFPSDoc() {
    UI* ui = UI::getInstance();
    ui->setVisibility(false, ui->fps_doc_);
  }
  
  void UI::showFPSDoc() {
    UI* ui = UI::getInstance();
    ui->setVisibility(true, ui->fps_doc_);
  }

  void UI::clearLog() {
    UI* ui = UI::getInstance();
    ui->clearText(ui->text_doc_, "log_text");
  }

  void UI::log(std::string logstring) {
    UI* ui = UI::getInstance();
    ui->appendText(logstring, ui->text_doc_, "log_text");
  }

  void UI::clearText(Rocket::Core::ElementDocument* doc, std::string elementName)
  {
	  Element* elem = doc->GetElementById(elementName.c_str());
	  if (elem == NULL) {
		  throw runtime_error("UI::update() - couldnt find fps_text!");
	  }
	  logLines.clear();
	  logString = "";

	  reinterpret_cast<ElementText*>(elem)->SetText(logString.c_str());
  }

  void UI::appendText(std::string str, Rocket::Core::ElementDocument* doc,std::string elementName)
  {
	  Element* elem = doc->GetElementById(elementName.c_str());
	  if (elem == NULL) {
		  throw runtime_error("UI::update() - couldnt find fps_text!");
	  }
	  logLines.push_front(str);
    
	  if (logLines.size() > MAX_CONSOLE_LINES)
	  {
		  logLines.pop_back();
	  }
	  std::stringstream ss;
	  for (std::string s : logLines)
	  {
		  ss << s;
	  }
	  logString = ss.str();
    
	  reinterpret_cast<ElementText*>(elem)->SetText(logString.c_str());
  }

  void UI::processMessage(EngineObjectMessage& msg)
  {
	  if (msg.messageType.compare("InstrumentSelected") == 0)
	  {
		  std::cout << "UI received message: " << msg.messageType << std::endl;
		  if (msg.data)
		  {
			  setSelectboxVal("instrument", *((int*)(msg.data)), midi_doc_);
		  }
	  }
	  if (msg.messageType.compare("DrumSelected") == 0)
	  {
		  std::cout << "UI received message: " << msg.messageType << std::endl;
		  if (msg.data)
		  {
			  setSelectboxVal("drums", *((int*)(msg.data)), midi_doc_);
		  }
	  }
	  if (msg.messageType.compare("UseDrums") == 0)
	  {
		  std::cout << "UI received message: " << msg.messageType << std::endl;
		  if (msg.data)
		  {
			  setCheckboxVal("use_drums", *((bool*)(msg.data)), midi_doc_);
		  }
	  }
	  if (msg.messageType.compare("MidiOutputs") == 0)
	  {
		  std::cout << "UI received message: " << msg.messageType << std::endl;
		  if (msg.data)
		  {
			  std::pair<UIEnumVal*,int>* dat = (std::pair<UIEnumVal*, int>*)msg.data;
			  UIEnumVal* vals = dat->first;
			  int num = dat->second;
			  addSelectbox("midi_output", "Midi Outs", vals, num, midi_doc_, midi_content_);
		  }
	  }
  }

  UI* UI::getInstance()
  {
	  auto weakUIPtr = EngineObjectManager::getInstance().getObjectFromTag("UI");
	  if (auto p = weakUIPtr.lock())
	  {
		  UI* ui = (UI*)p.get();
		  return ui;
	  }
	  return nullptr;
  }

}  // namespace ui
}  // namespace renderlib
