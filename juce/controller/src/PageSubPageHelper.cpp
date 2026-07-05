#include "xplorer/controller/PageSubPageHelper.hpp"

namespace xplorer::controller
{
    using model::EnumPages;
    using model::EnumRotaryEncoders;

    PageSubPageHelper::PageSubPageHelper()
    {
        initializeAuthorizedEditingRotaryEvents();
    }

    void PageSubPageHelper::getPageSubPage(int& page, int& subPage) const
    {
        const std::lock_guard lock(_mutex);
        page = _lastPageSelected;
        subPage = _lastSubPageSelected;
    }

    void PageSubPageHelper::updatePageSubPage(int page, int subPage)
    {
        const std::lock_guard lock(_mutex);
        _lastPageSelected = page;
        _lastSubPageSelected = subPage;
    }

    bool PageSubPageHelper::isAuthorizedRotary(EnumRotaryEncoders encoder) const
    {
        int page = 0;
        int subPage = 0;
        getPageSubPage(page, subPage);
        const auto found = _authorizedRotaryEvents.find({page, subPage});
        return found != _authorizedRotaryEvents.end() && found->second.contains(encoder);
    }

    bool PageSubPageHelper::isLfoRetrig(int parameterPage, int parameterSubPage, int buttonId) const
    {
        // Reference condition kept verbatim (it compares the sub-page against
        // LFO_5, faithfully preserved).
        const bool isPageLfo = parameterPage >= static_cast<int>(EnumPages::LFO_1)
                               && parameterSubPage <= static_cast<int>(EnumPages::LFO_5);
        const bool isRetrig = parameterSubPage == 0x01
                              && buttonId == static_cast<int>(EnumRotaryEncoders::THIRD);
        return isPageLfo && isRetrig;
    }

    bool PageSubPageHelper::isPageEnvLfoRampTrack() const
    {
        const std::lock_guard lock(_mutex);
        return _lastPageSelected >= static_cast<int>(EnumPages::ENV_1)
               && _lastPageSelected <= static_cast<int>(EnumPages::RAMP_4);
    }

    bool PageSubPageHelper::isPageLfo() const
    {
        const std::lock_guard lock(_mutex);
        return _lastPageSelected >= static_cast<int>(EnumPages::LFO_1)
               && _lastPageSelected <= static_cast<int>(EnumPages::LFO_5);
    }

    void PageSubPageHelper::initializeAuthorizedEditingRotaryEvents()
    {
        using E = EnumRotaryEncoders;
        auto add = [this](int page, int subPage, std::set<E> authorized)
        { _authorizedRotaryEvents.emplace(std::make_pair(page, subPage), std::move(authorized)); };
        auto addRange = [&add](EnumPages first, EnumPages last, int subPage, const std::set<E>& authorized)
        {
            for (int page = static_cast<int>(first); page <= static_cast<int>(last); ++page)
            {
                add(page, subPage, authorized);
            }
        };

        add(static_cast<int>(EnumPages::VCO_1_X), 0, {E::FIRST, E::SECOND, E::FOURTH, E::SIXTH});
        add(static_cast<int>(EnumPages::VCO_2_X), 0, {E::FIRST, E::SECOND, E::FOURTH, E::SIXTH});
        add(static_cast<int>(EnumPages::FM_LAG_X), 0, {E::FIRST, E::SECOND, E::FOURTH, E::FIFTH});
        add(static_cast<int>(EnumPages::FM_LAG_X), 1, {E::FIRST});
        addRange(EnumPages::TRACK_1, EnumPages::TRACK_3, 0,
                 {E::FIRST, E::SECOND, E::THIRD, E::FOURTH, E::FIFTH, E::SIXTH});
        add(static_cast<int>(EnumPages::VCF_VCA_X), 0, {E::FIRST, E::SECOND, E::THIRD, E::FIFTH, E::SIXTH});
        addRange(EnumPages::ENV_1, EnumPages::ENV_5, 0,
                 {E::FIRST, E::SECOND, E::THIRD, E::FOURTH, E::FIFTH, E::SIXTH});
        addRange(EnumPages::ENV_1, EnumPages::ENV_5, 1, {E::SECOND, E::FIFTH});
        addRange(EnumPages::LFO_1, EnumPages::LFO_5, 0,
                 {E::FIRST, E::SECOND, E::THIRD, E::FOURTH, E::SIXTH});
        addRange(EnumPages::LFO_1, EnumPages::LFO_5, 1, {E::THIRD});
        addRange(EnumPages::RAMP_1, EnumPages::RAMP_4, 0, {E::SECOND});
        addRange(EnumPages::RAMP_1, EnumPages::RAMP_4, 1, {E::SECOND, E::FIFTH});
    }
}
