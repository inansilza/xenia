/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2014 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_UI_WINDOW_H_
#define XENIA_UI_WINDOW_H_

#include <memory>
#include <string>

#include "xenia/base/delegate.h"
#include "xenia/ui/graphics_context.h"
#include "xenia/ui/loop.h"
#include "xenia/ui/menu_item.h"
#include "xenia/ui/ui_event.h"
#include "xenia/ui/window_listener.h"

namespace xe {
namespace ui {

typedef void* NativePlatformHandle;
typedef void* NativeWindowHandle;

class ImGuiDrawer;

class Window {
 public:
  static std::unique_ptr<Window> Create(Loop* loop, const std::wstring& title);

  virtual ~Window();

  Loop* loop() const { return loop_; }
  virtual NativePlatformHandle native_platform_handle() const = 0;
  virtual NativeWindowHandle native_handle() const = 0;

  MenuItem* main_menu() const { return main_menu_.get(); }
  void set_main_menu(std::unique_ptr<MenuItem> main_menu) {
    main_menu_ = std::move(main_menu);
    OnMainMenuChange();
  }

  const std::wstring& title() const { return title_; }
  virtual bool set_title(const std::wstring& title) {
    if (title == title_) {
      return false;
    }
    title_ = title;
    return true;
  }

  virtual bool SetIconFromBuffer(void* buffer, size_t size) = 0;

  virtual bool is_fullscreen() const { return false; }
  virtual void ToggleFullscreen(bool fullscreen) {}

  virtual bool is_bordered() const { return false; }
  virtual void set_bordered(bool enabled) {}

  bool has_focus() const { return has_focus_; }
  virtual void set_focus(bool value) { has_focus_ = value; }

  bool is_cursor_visible() const { return is_cursor_visible_; }
  virtual void set_cursor_visible(bool value) { is_cursor_visible_ = value; }

  int32_t width() const { return width_; }
  int32_t height() const { return height_; }
  virtual void Resize(int32_t width, int32_t height) = 0;
  virtual void Resize(int32_t left, int32_t top, int32_t right,
                      int32_t bottom) = 0;

  GraphicsContext* context() const { return context_.get(); }
  ImGuiDrawer* imgui_drawer() const { return imgui_drawer_.get(); }
  bool is_imgui_input_enabled() const { return is_imgui_input_enabled_; }
  void set_imgui_input_enabled(bool value);

  void AttachListener(WindowListener* listener);
  void DetachListener(WindowListener* listener);

  virtual bool Initialize() { return true; }
  void set_context(std::unique_ptr<GraphicsContext> context) {
    context_ = std::move(context);
    if (context_) {
      MakeReady();
    }
  }

  void Layout();
  virtual void Invalidate();

  virtual void Close() = 0;

 public:
  Delegate<UIEvent*> on_closing;
  Delegate<UIEvent*> on_closed;

  Delegate<UIEvent*> on_painting;
  Delegate<UIEvent*> on_paint;
  Delegate<UIEvent*> on_painted;

  Delegate<KeyEvent*> on_key_down;
  Delegate<KeyEvent*> on_key_up;
  Delegate<KeyEvent*> on_key_char;

  Delegate<MouseEvent*> on_mouse_down;
  Delegate<MouseEvent*> on_mouse_move;
  Delegate<MouseEvent*> on_mouse_up;
  Delegate<MouseEvent*> on_mouse_wheel;

 protected:
  Window(Loop* loop, const std::wstring& title);

  void ForEachListener(std::function<void(WindowListener*)> fn);
  void TryForEachListener(std::function<bool(WindowListener*)> fn);

  virtual bool MakeReady();

  virtual bool OnCreate();
  virtual void OnMainMenuChange();
  virtual void OnClose();
  virtual void OnDestroy();

  virtual void OnResize(UIEvent* e);
  virtual void OnLayout(UIEvent* e);
  virtual void OnPaint(UIEvent* e);

  virtual void OnVisible(UIEvent* e);
  virtual void OnHidden(UIEvent* e);

  virtual void OnGotFocus(UIEvent* e);
  virtual void OnLostFocus(UIEvent* e);

  virtual void OnKeyDown(KeyEvent* e);
  virtual void OnKeyUp(KeyEvent* e);
  virtual void OnKeyChar(KeyEvent* e);

  virtual void OnMouseDown(MouseEvent* e);
  virtual void OnMouseMove(MouseEvent* e);
  virtual void OnMouseUp(MouseEvent* e);
  virtual void OnMouseWheel(MouseEvent* e);

  void OnKeyPress(KeyEvent* e, bool is_down, bool is_char);

  Loop* loop_ = nullptr;
  std::unique_ptr<MenuItem> main_menu_;
  std::wstring title_;
  int32_t width_ = 0;
  int32_t height_ = 0;
  bool has_focus_ = true;
  bool is_cursor_visible_ = true;
  bool is_imgui_input_enabled_ = false;

  std::unique_ptr<GraphicsContext> context_;
  std::unique_ptr<ImGuiDrawer> imgui_drawer_;

  uint32_t frame_count_ = 0;
  uint32_t fps_ = 0;
  uint64_t fps_update_time_ns_ = 0;
  uint64_t fps_frame_count_ = 0;
  uint64_t last_paint_time_ns_ = 0;

  bool modifier_shift_pressed_ = false;
  bool modifier_cntrl_pressed_ = false;
  bool modifier_alt_pressed_ = false;
  bool modifier_super_pressed_ = false;

  // All currently-attached listeners that get event notifications.
  bool in_listener_loop_ = false;
  std::vector<WindowListener*> listeners_;
  std::vector<WindowListener*> pending_listener_attaches_;
  std::vector<WindowListener*> pending_listener_detaches_;
};

}  // namespace ui
}  // namespace xe

#endif  // XENIA_UI_WINDOW_H_
