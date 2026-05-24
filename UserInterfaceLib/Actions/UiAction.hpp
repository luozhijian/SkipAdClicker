#ifndef AUTOMATIOTEST_USERINTERFACELIB_ACTIONS_UIACTION_HPP
#define AUTOMATIOTEST_USERINTERFACELIB_ACTIONS_UIACTION_HPP

#include "../../Utilities/Common.hpp"

#include <string>

namespace automationtest::userinterfacelib {

enum class ControlType {
    None,
    Bitmap,
    Window,
    Pane,
    Button,
    ButtonWithArc,
    ButtonRound,
    TextBox,
    RadioBox,
    Combox,
    ListBox,
    Scrollbar
};

class UiAction {
public:
    virtual ~UiAction() = default;

    ControlType control_type {ControlType::None};
    std::string action {};
    std::string caption {};
};

class UiActionButton : public UiAction {
};

class UiActionEditBox : public UiAction {
public:
    std::string input_text {};
};

} // namespace automationtest::userinterfacelib

#endif // AUTOMATIOTEST_USERINTERFACELIB_ACTIONS_UIACTION_HPP
