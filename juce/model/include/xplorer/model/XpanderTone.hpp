#pragma once

// Port of Xplorer.Model.XpanderTone (partial classes .cs, .Properties.cs,
// .ModulationMatrix.cs): the edited Xpander/Matrix-12 single patch as a
// parameter map + 20-entry modulation matrix. [RQ-MOD-020..023, RQ-MOD-030..033]

#include "midiapp/model/AbstractTone.hpp"
#include "xplorer/model/ModulationMatrixEntry.hpp"
#include "xplorer/model/XpanderConstants.hpp"
#include "xplorer/model/XpanderParameter.hpp"

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace xplorer::model
{
    class XpanderSinglePatch;

    class XpanderTone final : public midiapp::model::AbstractTone
    {
    public:
        static constexpr int MIN_PROGRAM_NUMBER = 0;
        static constexpr int MAX_PROGRAM_NUMBER = constants::SINGLE_TONES_MAX_COUNT - 1;
        static constexpr int NO_AVAILABLE_MOD_ENTRY = -1;
        static constexpr int UNDEFINED_MODULATION_SOURCE_NUMBER = ModulationMatrixEntry::UNDEFINED_ID_SOURCE;

        /// Receives the mod-edit parameters to transmit, in order (the
        /// reference UpdateModulationParameterDelegate).
        using UpdateModulationParameterDelegate =
            std::function<void(const midiapp::model::AbstractParameter&)>;

        XpanderTone();

        // --- AbstractTone ---
        [[nodiscard]] std::string toneName() const override { return _toneName; }
        /// Truncated/space-padded to 8 characters (reference setter).
        void setToneName(const std::string& name) override;
        [[nodiscard]] std::set<std::string> eligibleParametersForToneMorphing() const override;

        // --- program numbers (reference .Properties.cs) [RQ-MOD-021] ---
        [[nodiscard]] int editingProgramNumber() const { return _editingProgramNumber; }
        void setEditingProgramNumber(int programNumber) { _editingProgramNumber = programNumber; }
        [[nodiscard]] int currentProgramNumber() const { return _currentProgramNumber; }
        /// Wraps around 0..99 (reference reloop behavior).
        void setCurrentProgramNumber(int programNumber);

        // --- SysEx serialization [RQ-MOD-022] ---
        /// Full single-patch dump; program number forced to editingProgramNumber.
        [[nodiscard]] std::vector<std::uint8_t> toByteArray() const;
        void fromByteArray(std::span<const std::uint8_t> data);
        [[nodiscard]] static std::string getNameFromByteArray(std::span<const std::uint8_t> bytes);

        // --- modulation matrix [RQ-MOD-030..033] ---
        [[nodiscard]] const std::array<ModulationMatrixEntry, constants::MODENTRIES_COUNT>&
        modulationMatrix() const { return _modulationMatrix; }

        void clearModulationMatrix();

        /// Entry numbers are 1-based, as in the reference API.
        void changeModulationSource(int newModulationSource, int modulationSourceAmount,
                                    int modulationQuantize, int modulationDestination,
                                    int entryNumber,
                                    const UpdateModulationParameterDelegate& update);
        void changeModulationSourceAmount(int modulationSource, int modulationSourceAmount,
                                          int modulationDestination, int entryNumber,
                                          const UpdateModulationParameterDelegate& update);
        void changeModulationSourceQuantize(int modulationSource, int modulationDestination,
                                            int modulationQuantize, int entryNumber,
                                            const UpdateModulationParameterDelegate& update);
        void changeModulationDestination(int modulationSource, int modulationSourceAmount,
                                         int modulationQuantize, int oldModulationDestination,
                                         int newModulationDestination, int entryNumber,
                                         const UpdateModulationParameterDelegate& update);
        bool addModulationSource(int modulationSource, int modulationSourceAmount,
                                 int modulationQuantize, int newModulationDestination,
                                 int entryNumber, const UpdateModulationParameterDelegate& update);
        [[nodiscard]] bool isMaxSourceCountForDestinationReached(EnumModulationDestinations destination) const;
        [[nodiscard]] int getNextAvailableModEntry() const;

        void randomizeModulationMatrix(bool enableAmount, bool enableQuantize,
                                       bool enableSourceAndDest, std::optional<float> humanizeRatio,
                                       std::optional<unsigned int> seed = std::nullopt);

        // --- randomizer helpers (reference XpanderTone.cs tail) ---
        void detune(bool detuneAnalog = true);
        void defineVCOFrequenciesTuning(EnumRandomVCOFreq tuning);
        void forceEnv2ModVca2AfterRandomizeMatrix(EnumRandomVCAEnv enveloppe);

        [[nodiscard]] static std::string amountSourceParameterNameForEntry(int entryNumber);
        [[nodiscard]] static std::string quantizeSourceParameterNameForEntry(int entryNumber);

    private:
        void initializeParameterMap();
        void fromSinglePatch(const XpanderSinglePatch& singlePatch);
        [[nodiscard]] XpanderSinglePatch toSinglePatch() const;

        [[nodiscard]] XpanderParameter& parameterAt(const std::string& name);
        [[nodiscard]] const XpanderParameter& parameterAt(const std::string& name) const;
        [[nodiscard]] XpanderModMatrixParameter& modMatrixParameterAt(const std::string& name);

        void deleteIdSource(EnumModulationDestinations oldDestination, int entryNumber,
                            const UpdateModulationParameterDelegate& update);
        void updateIdSourceAfterDelete(const ModulationMatrixEntry& entry);
        [[nodiscard]] int getNextAvailableModIdSourceForDest(EnumModulationDestinations destination) const;

        /// Builds the SETSIGN companion parameter and normalizes the amount
        /// clone to its absolute value (reference
        /// AmountSetSignMessageForSetSourceAmountParameter).
        [[nodiscard]] std::unique_ptr<XpanderModMatrixParameter>
        makeSetSignParameter(XpanderModMatrixParameter& amountParameter) const;

        std::string _toneName;
        int _currentProgramNumber = MAX_PROGRAM_NUMBER; // reference DEFAULT_EDITING_NUMBER
        int _editingProgramNumber = MAX_PROGRAM_NUMBER;
        std::array<ModulationMatrixEntry, constants::MODENTRIES_COUNT> _modulationMatrix{};
        std::set<std::string> _parameterNamesForToneMorphing;
    };
}
