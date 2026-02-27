#include "Dynamic.h"

namespace {
class DynamicView: public AViewContainer {
public:
    DynamicView() {
    }

    void setContent(_<AView> v) {
        ALayoutInflater::inflate(this, std::move(v));
    }
};
}

namespace aui::declarative::experimental {

_<AView> Dynamic::operator()() {
    auto v = _new<DynamicView>();
    content.bindTo(ASlotDef{AUI_SLOT(v.get())::setContent});
    return v;
}

}