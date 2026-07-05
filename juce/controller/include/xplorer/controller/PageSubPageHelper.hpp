#pragma once

// Port of Xplorer.Controller.PageSubPageHelper: tracks the synth's current
// page/sub-page and knows which rotary encoders may edit on it. [RQ-CTL-020, RQ-CTL-022]

#include "xplorer/model/XpanderConstants.hpp"

#include <map>
#include <mutex>
#include <set>
#include <utility>

namespace xplorer::controller
{
    class PageSubPageHelper
    {
    public:
        PageSubPageHelper();

        void getPageSubPage(int& page, int& subPage) const;
        void updatePageSubPage(int page, int subPage);

        [[nodiscard]] bool isAuthorizedRotary(model::EnumRotaryEncoders encoder) const;
        [[nodiscard]] bool isLfoRetrig(int parameterPage, int parameterSubPage, int buttonId) const;
        [[nodiscard]] bool isPageEnvLfoRampTrack() const;
        [[nodiscard]] bool isPageLfo() const;

    private:
        void initializeAuthorizedEditingRotaryEvents();

        mutable std::mutex _mutex;
        int _lastPageSelected = static_cast<int>(model::EnumPages::UNKNOWN);
        int _lastSubPageSelected = static_cast<int>(model::EnumPages::UNKNOWN);
        std::map<std::pair<int, int>, std::set<model::EnumRotaryEncoders>> _authorizedRotaryEvents;
    };
}
