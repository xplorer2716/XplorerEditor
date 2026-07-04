// XpanderTone modulation matrix operations.
// Port of XpanderTone.ModulationMatrix.cs. [RQ-MOD-030..033]
#include "xplorer/model/XpanderTone.hpp"

#include "midiapp/service/Logger.hpp"

#include <chrono>
#include <cmath>
#include <random>

namespace xplorer::model
{
    using midiapp::service::Logger;
    using midiapp::service::TraceLevel;
    using xpl::midi::MidiMessage;

    namespace
    {
        /// Mod-edit command templates (reference static _ADD/_DELETE/_CHANGE
        /// source parameters); page/subpage/id are set per use.
        std::unique_ptr<XpanderModMatrixParameter> makeCommandParameter(
            const std::string& name, EnumModulationEditCommands command, std::uint8_t valueByte, int value)
        {
            const auto& pages = PAGE_SUBPAGE_FOR_MODULATION_DESTINATION[
                static_cast<std::size_t>(EnumModulationDestinations::VCO1_FRQ)];
            return std::make_unique<XpanderModMatrixParameter>(
                name, static_cast<int>(pages.page), pages.subPage,
                static_cast<int>(EnumModulationSourcesModMatrix::KBD),
                static_cast<int>(EnumModulationSourcesModMatrix::NONE), 1,
                MidiMessage::sysEx(std::vector<std::uint8_t>{
                    0xF0, 0x10, 0x02, 0x0F, 0x00, 0x00, 0x00,
                    static_cast<std::uint8_t>(command), 0x00, valueByte, 0x00, 0xF7}),
                value);
        }

        std::unique_ptr<XpanderModMatrixParameter> makeDeleteSourceParameter()
        {
            return makeCommandParameter("DEL_MOD_SRC", EnumModulationEditCommands::DELETESOURCE, 0x00, 0x00);
        }

        std::unique_ptr<XpanderModMatrixParameter> makeAddSourceParameter()
        {
            return makeCommandParameter("ADD_MOD_SRC", EnumModulationEditCommands::ADDSOURCE,
                                        static_cast<std::uint8_t>(EnumModulationSourcesModMatrix::NONE),
                                        static_cast<int>(EnumModulationSourcesModMatrix::NONE));
        }

        std::unique_ptr<XpanderModMatrixParameter> makeChangeSourceParameter()
        {
            return makeCommandParameter("CHANGE_MOD_SRC", EnumModulationEditCommands::CHANGESOURCE, 0x00, 0x00);
        }
    }

    void XpanderTone::clearModulationMatrix()
    {
        for (auto& entry : _modulationMatrix)
        {
            entry.destination = EnumModulationDestinations::VCO1_FRQ;
            entry.reset();
        }
    }

    std::unique_ptr<XpanderModMatrixParameter>
    XpanderTone::makeSetSignParameter(XpanderModMatrixParameter& amountParameter) const
    {
        auto toggleSign = std::make_unique<XpanderModMatrixParameter>(
            amountParameter.name() + "_SETSIGN", amountParameter.page(), amountParameter.subPage(),
            0, 1, 1,
            MidiMessage::sysEx(std::vector<std::uint8_t>{
                0xF0, 0x10, 0x02, 0x0F, 0x00, static_cast<std::uint8_t>(amountParameter.idSource()),
                0x00, static_cast<std::uint8_t>(EnumModulationEditCommands::SETSIGN), 0x00, 0x00, 0x00, 0xF7}),
            0x00);
        if (amountParameter.value() < 0)
        {
            toggleSign->setValue(0x01);
        }
        amountParameter.setValue(std::abs(amountParameter.value()));
        amountParameter.setChanged(false);
        return toggleSign;
    }

    void XpanderTone::changeModulationSource(int newModulationSource, int modulationSourceAmount,
                                             int modulationQuantize, int modulationDestination,
                                             int entryNumber,
                                             const UpdateModulationParameterDelegate& update)
    {
        auto& entry = _modulationMatrix[static_cast<std::size_t>(entryNumber - 1)];
        if (entry.idSource != UNDEFINED_MODULATION_SOURCE_NUMBER)
        {
            if (newModulationSource == static_cast<int>(EnumModulationSourcesModMatrix::NONE))
            {
                deleteIdSource(static_cast<EnumModulationDestinations>(modulationDestination),
                               entryNumber, update);
            }
            else
            {
                auto changeSource = makeChangeSourceParameter();
                const auto& pages = PAGE_SUBPAGE_FOR_MODULATION_DESTINATION[
                    static_cast<std::size_t>(modulationDestination)];
                changeSource->setPage(static_cast<int>(pages.page));
                changeSource->setSubPage(pages.subPage);
                changeSource->setIdSource(entry.idSource);
                changeSource->setValue(newModulationSource); // value is the new source
                changeSource->setChanged(false); // sent directly, not via the worker
                entry.source = static_cast<EnumModulationSourcesModMatrix>(newModulationSource);
                if (update)
                {
                    update(*changeSource);
                }
            }
        }
        else if (newModulationSource != static_cast<int>(EnumModulationSourcesModMatrix::NONE))
        {
            addModulationSource(newModulationSource, modulationSourceAmount, modulationQuantize,
                                modulationDestination, entryNumber, update);
        }
    }

    void XpanderTone::changeModulationSourceAmount(int modulationSource, int modulationSourceAmount,
                                                   int modulationDestination, int entryNumber,
                                                   const UpdateModulationParameterDelegate& update)
    {
        if (modulationSource == static_cast<int>(EnumModulationSourcesModMatrix::NONE))
        {
            return;
        }
        auto& entry = _modulationMatrix[static_cast<std::size_t>(entryNumber - 1)];
        if (entry.idSource == UNDEFINED_MODULATION_SOURCE_NUMBER)
        {
            return;
        }
        auto& amountParameter = modMatrixParameterAt(amountSourceParameterNameForEntry(entryNumber));
        const auto& pages = PAGE_SUBPAGE_FOR_MODULATION_DESTINATION[
            static_cast<std::size_t>(modulationDestination)];
        // do not resend the same value
        if (amountParameter.page() == static_cast<int>(pages.page)
            && amountParameter.subPage() == pages.subPage
            && amountParameter.idSource() == entry.idSource
            && amountParameter.value() == modulationSourceAmount)
        {
            return;
        }
        amountParameter.setPage(static_cast<int>(pages.page));
        amountParameter.setSubPage(pages.subPage);
        amountParameter.setIdSource(entry.idSource);
        amountParameter.setValue(modulationSourceAmount);
        amountParameter.setChanged(false);
        auto clone = std::unique_ptr<XpanderModMatrixParameter>(
            static_cast<XpanderModMatrixParameter*>(amountParameter.clone().release()));
        entry.setAmount(modulationSourceAmount);
        // the original keeps its signed value to permit the resend test
        if (update)
        {
            update(*makeSetSignParameter(*clone));
            update(*clone);
        }
    }

    void XpanderTone::changeModulationSourceQuantize(int modulationSource, int modulationDestination,
                                                     int modulationQuantize, int entryNumber,
                                                     const UpdateModulationParameterDelegate& update)
    {
        if (modulationSource == static_cast<int>(EnumModulationSourcesModMatrix::NONE))
        {
            return;
        }
        auto& entry = _modulationMatrix[static_cast<std::size_t>(entryNumber - 1)];
        if (entry.idSource == UNDEFINED_MODULATION_SOURCE_NUMBER)
        {
            return;
        }
        auto& quantizeParameter = modMatrixParameterAt(quantizeSourceParameterNameForEntry(entryNumber));
        const auto& pages = PAGE_SUBPAGE_FOR_MODULATION_DESTINATION[
            static_cast<std::size_t>(modulationDestination)];
        quantizeParameter.setPage(static_cast<int>(pages.page));
        quantizeParameter.setSubPage(pages.subPage);
        quantizeParameter.setIdSource(entry.idSource);
        quantizeParameter.setValue(modulationQuantize);
        quantizeParameter.setChanged(false);
        entry.setQuantize(modulationQuantize);
        if (update)
        {
            const auto clone = quantizeParameter.clone();
            update(*clone);
        }
    }

    void XpanderTone::changeModulationDestination(int modulationSource, int modulationSourceAmount,
                                                  int modulationQuantize, int oldModulationDestination,
                                                  int newModulationDestination, int entryNumber,
                                                  const UpdateModulationParameterDelegate& update)
    {
        auto& entry = _modulationMatrix[static_cast<std::size_t>(entryNumber - 1)];
        if (entry.idSource != UNDEFINED_MODULATION_SOURCE_NUMBER)
        {
            deleteIdSource(static_cast<EnumModulationDestinations>(oldModulationDestination),
                           entryNumber, update);
        }
        if (static_cast<EnumModulationSourcesModMatrix>(modulationSource)
            != EnumModulationSourcesModMatrix::NONE)
        {
            addModulationSource(modulationSource, modulationSourceAmount, modulationQuantize,
                                newModulationDestination, entryNumber, update);
        }
        else
        {
            // keep the UI-selected destination even with no source assigned
            entry.destination = static_cast<EnumModulationDestinations>(newModulationDestination);
        }
    }

    bool XpanderTone::isMaxSourceCountForDestinationReached(EnumModulationDestinations destination) const
    {
        int sourceCount = 0;
        for (const auto& entry : _modulationMatrix)
        {
            if (entry.destination == destination && entry.idSource != UNDEFINED_MODULATION_SOURCE_NUMBER)
            {
                ++sourceCount;
            }
        }
        return sourceCount >= constants::MAX_MODULATION_SOURCE;
    }

    bool XpanderTone::addModulationSource(int modulationSource, int modulationSourceAmount,
                                          int modulationQuantize, int newModulationDestination,
                                          int entryNumber, const UpdateModulationParameterDelegate& update)
    {
        const int modIdSource = getNextAvailableModIdSourceForDest(
            static_cast<EnumModulationDestinations>(newModulationDestination));
        if (modIdSource != UNDEFINED_MODULATION_SOURCE_NUMBER)
        {
            const auto& pages = PAGE_SUBPAGE_FOR_MODULATION_DESTINATION[
                static_cast<std::size_t>(newModulationDestination)];

            auto addSource = makeAddSourceParameter();
            addSource->setPage(static_cast<int>(pages.page));
            addSource->setSubPage(pages.subPage);
            addSource->setIdSource(0x00);
            addSource->setValue(modulationSource);
            addSource->setChanged(false);
            if (update)
            {
                update(*addSource);
            }

            auto& amountParameter = modMatrixParameterAt(amountSourceParameterNameForEntry(entryNumber));
            amountParameter.setPage(static_cast<int>(pages.page));
            amountParameter.setSubPage(pages.subPage);
            amountParameter.setIdSource(modIdSource);
            amountParameter.setValue(modulationSourceAmount);
            amountParameter.setChanged(false);
            if (update)
            {
                auto clone = std::unique_ptr<XpanderModMatrixParameter>(
                    static_cast<XpanderModMatrixParameter*>(amountParameter.clone().release()));
                update(*makeSetSignParameter(*clone));
                update(*clone);
            }

            auto& quantizeParameter = modMatrixParameterAt(quantizeSourceParameterNameForEntry(entryNumber));
            quantizeParameter.setPage(static_cast<int>(pages.page));
            quantizeParameter.setSubPage(pages.subPage);
            quantizeParameter.setIdSource(modIdSource);
            quantizeParameter.setValue(modulationQuantize);
            quantizeParameter.setChanged(false);
            if (update)
            {
                const auto clone = quantizeParameter.clone();
                update(*clone);
            }
        }
        auto& entry = _modulationMatrix[static_cast<std::size_t>(entryNumber - 1)];
        entry.destination = static_cast<EnumModulationDestinations>(newModulationDestination);
        entry.source = static_cast<EnumModulationSourcesModMatrix>(modulationSource);
        entry.idSource = modIdSource;
        entry.setAmount(modulationSourceAmount);
        entry.setQuantize(modulationQuantize);
        return true;
    }

    void XpanderTone::deleteIdSource(EnumModulationDestinations oldDestination, int entryNumber,
                                     const UpdateModulationParameterDelegate& update)
    {
        auto deleteSource = makeDeleteSourceParameter();
        const auto& pages = PAGE_SUBPAGE_FOR_MODULATION_DESTINATION[static_cast<std::size_t>(oldDestination)];
        deleteSource->setPage(static_cast<int>(pages.page));
        deleteSource->setSubPage(pages.subPage);
        auto& entry = _modulationMatrix[static_cast<std::size_t>(entryNumber - 1)];
        if (entry.idSource != UNDEFINED_MODULATION_SOURCE_NUMBER)
        {
            deleteSource->setIdSource(entry.idSource);
            deleteSource->setChanged(false); // sent directly, before any other command
            if (update)
            {
                update(*deleteSource);
            }
        }
        updateIdSourceAfterDelete(entry);
        entry.reset();
    }

    void XpanderTone::updateIdSourceAfterDelete(const ModulationMatrixEntry& deletedEntry)
    {
        // The synth re-numbers the remaining sources of that destination.
        for (auto& entry : _modulationMatrix)
        {
            if (entry.destination == deletedEntry.destination && entry.idSource > deletedEntry.idSource)
            {
                entry.idSource -= 1;
            }
        }
    }

    int XpanderTone::getNextAvailableModIdSourceForDest(EnumModulationDestinations destination) const
    {
        bool usedSources[constants::MAX_MODULATION_SOURCE] = {};
        for (const auto& entry : _modulationMatrix)
        {
            if (entry.destination == destination && entry.idSource != UNDEFINED_MODULATION_SOURCE_NUMBER)
            {
                usedSources[entry.idSource] = true;
            }
        }
        for (int i = 0; i < constants::MAX_MODULATION_SOURCE; ++i)
        {
            if (!usedSources[i])
            {
                return i;
            }
        }
        Logger::writeLine("XpanderTone", TraceLevel::Info,
                          "GetNextAvailableModIDSourceForDest: No IdSource available");
        return UNDEFINED_MODULATION_SOURCE_NUMBER;
    }

    int XpanderTone::getNextAvailableModEntry() const
    {
        for (int i = 0; i < constants::MODENTRIES_COUNT; ++i)
        {
            if (_modulationMatrix[static_cast<std::size_t>(i)].idSource == UNDEFINED_MODULATION_SOURCE_NUMBER)
            {
                return i;
            }
        }
        Logger::writeLine("XpanderTone", TraceLevel::Info,
                          "GetNextAvailableModEntry: No modulation entry available");
        return NO_AVAILABLE_MOD_ENTRY;
    }

    void XpanderTone::randomizeModulationMatrix(bool enableAmount, bool enableQuantize,
                                                bool enableSourceAndDest,
                                                std::optional<float> humanizeRatio,
                                                std::optional<unsigned int> seed)
    {
        clearModulationMatrix();
        std::mt19937 randomizer(seed.value_or(static_cast<unsigned int>(
            std::chrono::steady_clock::now().time_since_epoch().count())));

        for (int entryIndex = 0; entryIndex < constants::MODENTRIES_COUNT; ++entryIndex)
        {
            auto& entry = _modulationMatrix[static_cast<std::size_t>(entryIndex)];
            int nextAvailableIdSource = UNDEFINED_MODULATION_SOURCE_NUMBER;
            auto destination = EnumModulationDestinations::VCO1_FRQ;
            if (enableSourceAndDest && !humanizeRatio.has_value())
            {
                // the randomized destination must still accept a source
                while (nextAvailableIdSource == UNDEFINED_MODULATION_SOURCE_NUMBER)
                {
                    destination = static_cast<EnumModulationDestinations>(
                        std::uniform_int_distribution<int>(
                            static_cast<int>(EnumModulationDestinations::VCO1_FRQ),
                            static_cast<int>(EnumModulationDestinations::LAG_RATE))(randomizer));
                    nextAvailableIdSource = getNextAvailableModIdSourceForDest(destination);
                }
                entry.destination = destination;
                entry.source = static_cast<EnumModulationSourcesModMatrix>(
                    std::uniform_int_distribution<int>(
                        static_cast<int>(EnumModulationSourcesModMatrix::KBD),
                        static_cast<int>(EnumModulationSourcesModMatrix::NONE))(randomizer));
            }
            if (enableAmount)
            {
                if (!humanizeRatio.has_value())
                {
                    // reference rand.Next(min, max) upper bound is exclusive
                    entry.setAmount(std::uniform_int_distribution<int>(
                        ModulationMatrixEntry::MIN_AMOUNT, ModulationMatrixEntry::MAX_AMOUNT - 1)(randomizer));
                }
                // reference humanize path never changes the amount (value
                // stays 0 then is clamped); kept as-is
            }
            if (enableQuantize && !humanizeRatio.has_value())
            {
                entry.setQuantize(std::uniform_real_distribution<double>(0.0, 1.0)(randomizer) > 0.5
                                      ? ModulationMatrixEntry::MAX_QUANTIZE
                                      : ModulationMatrixEntry::MIN_QUANTIZE);
            }
            // synchronize the amount/quantize parameters of the map
            const auto& pages = PAGE_SUBPAGE_FOR_MODULATION_DESTINATION[static_cast<std::size_t>(destination)];
            auto& amountParameter = modMatrixParameterAt(amountSourceParameterNameForEntry(entryIndex + 1));
            amountParameter.setPage(static_cast<int>(pages.page));
            amountParameter.setSubPage(pages.subPage);
            amountParameter.setValue(entry.amount());
            modMatrixParameterAt(quantizeSourceParameterNameForEntry(entryIndex + 1)).setValue(entry.quantize());
            entry.idSource = nextAvailableIdSource;
        }
    }
}
