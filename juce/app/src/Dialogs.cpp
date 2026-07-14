#include "Dialogs.hpp"

#include "xplorer/app/MidiAutomationTable.hpp"
#include "xplorer/model/XpanderTone.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <memory>

namespace xplorer::app
{
    void applyMidiSettings(controller::XpanderController& controller,
                           settings::ISettingsService& settingsService, xpl::midi::MidiBackend&)
    {
        const auto& midi = settingsService.allUsersSettings().midiConfig;
        controller.setSynthOutputDevice(midi.synthOutputDeviceName);
        controller.setSynthInputDevice(midi.synthInputDeviceName);
        controller.setAutomationInputDevice(midi.automationInputDeviceName);
        // Reference MIDI channel is 1-based in settings, 0-based on the wire.
        controller.setMidiChannel(juce::jlimit(0, 15, midi.midiChannel - 1));
        controller.setParameterTransmitDelay(juce::jmax(0, midi.sysexTransmitDelay));
        controller.setEditingProgramNumber(
            juce::jlimit(model::XpanderTone::MIN_PROGRAM_NUMBER, model::XpanderTone::MAX_PROGRAM_NUMBER,
                         midi.editingProgramNumber));

        // Load the persisted "NAME;CC" table into the controller dictionary so
        // incoming CCs drive the mapped parameters and the VFD shows the CC.
        // Reference SettingsManager.LoadSettings. [RQ-GUI-036, ADR-JUC-012]
        controller.controlChangeAutomationTable().clear();
        for (const auto& entry : midi.automationTable)
        {
            if (const auto parsed = parseAutomationEntry(entry))
            {
                controller.controlChangeAutomationTable().add(parsed->first, parsed->second);
            }
        }
    }

    namespace
    {
        const juce::String SYSEX_WILDCARD = "*.syx;*.mid";

        /// Two-step file/folder picker for the extract dialog: chooses a bank
        /// sysex file, then a destination folder, then runs the extraction.
        class ExtractFlow final : private juce::DeletedAtShutdown
        {
        public:
            explicit ExtractFlow(controller::XpanderController& controller) : _controller(controller) {}

            void start()
            {
                _bankChooser = std::make_unique<juce::FileChooser>(
                    "Choose a bank (all data dump) sysex file", juce::File(), SYSEX_WILDCARD);
                _bankChooser->launchAsync(
                    juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                    [this](const juce::FileChooser& fc)
                    {
                        const auto file = fc.getResult();
                        if (file == juce::File())
                        {
                            delete this;
                            return;
                        }
                        _bankFile = file;
                        chooseFolder();
                    });
            }

        private:
            void chooseFolder()
            {
                _folderChooser = std::make_unique<juce::FileChooser>(
                    "Choose a destination folder", juce::File(), juce::String());
                _folderChooser->launchAsync(
                    juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                    [this](const juce::FileChooser& fc)
                    {
                        const auto folder = fc.getResult();
                        if (folder == juce::File())
                        {
                            delete this;
                            return;
                        }
                        runExtraction(folder);
                        delete this;
                    });
            }

            void runExtraction(const juce::File& folder)
            {
                try
                {
                    const auto tones = _controller.extractSinglePatchesFromAllDataDumpFileToDirectory(
                        _bankFile.getFullPathName().toStdString(), folder.getFullPathName().toStdString());
                    if (tones.empty())
                    {
                        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                              "Single patches",
                                                              "Unable to extract single patches from file!");
                    }
                    else
                    {
                        juce::AlertWindow::showMessageBoxAsync(
                            juce::MessageBoxIconType::InfoIcon, "Single patches",
                            juce::String(static_cast<int>(tones.size()))
                                + " files extracted successfully to folder "
                                + folder.getFullPathName());
                    }
                }
                catch (const std::exception& e)
                {
                    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                          "Single patches extraction", e.what());
                }
            }

            controller::XpanderController& _controller;
            juce::File _bankFile;
            std::unique_ptr<juce::FileChooser> _bankChooser;
            std::unique_ptr<juce::FileChooser> _folderChooser;
        };

        /// Background thread that runs the blocking restore loop and mirrors
        /// its progression into the modal progress window. [RQ-GUI-026]
        class RestoreThread final : public juce::ThreadWithProgressWindow
        {
        public:
            RestoreThread(controller::XpanderController& controller, std::string fileName)
                : juce::ThreadWithProgressWindow("All data dump restore", true, false),
                  _controller(controller), _fileName(std::move(fileName))
            {
            }

            void run() override
            {
                try
                {
                    _controller.restoreAllDataDumpToSynth(
                        _fileName,
                        [this](int current, int count)
                        {
                            setProgress(count > 0 ? static_cast<double>(current) / static_cast<double>(count)
                                                  : 0.0);
                            setStatusMessage("Sending data [" + juce::String(current) + "/"
                                             + juce::String(count) + "]");
                        });
                }
                catch (const std::exception& e)
                {
                    _error = e.what();
                }
            }

            void threadComplete(bool /*userPressedCancel*/) override
            {
                if (_error.isNotEmpty())
                {
                    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                          "All data dump restore", _error);
                }
                delete this; // self-owned; launched detached. [RQ-GUI-026]
            }

        private:
            controller::XpanderController& _controller;
            std::string _fileName;
            juce::String _error;
        };
    }

    void showExtractSingleTonesDialog(controller::XpanderController& controller)
    {
        (new ExtractFlow(controller))->start();
    }

    void runRestoreAllDataWithProgress(controller::XpanderController& controller, const std::string& fileName)
    {
        // Detached, self-deleting: keeps the message thread responsive while
        // the blocking send loop runs on the worker. [RQ-GUI-026]
        (new RestoreThread(controller, fileName))->launchThread();
    }

    void showStoreOrGotoDialog(const std::string& title, int currentProgram,
                               std::function<void(int)> onAccept)
    {
        auto* window = new juce::AlertWindow(title, "Choose a program number", juce::MessageBoxIconType::NoIcon);
        juce::StringArray items;
        for (int i = model::XpanderTone::MIN_PROGRAM_NUMBER; i <= model::XpanderTone::MAX_PROGRAM_NUMBER; ++i)
        {
            items.add(juce::String(i));
        }
        window->addComboBox("Program", items);
        window->getComboBoxComponent("Program")->setSelectedItemIndex(
            juce::jlimit(0, items.size() - 1, currentProgram));
        window->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

        window->enterModalState(true,
            juce::ModalCallbackFunction::create(
                [window, onAccept = std::move(onAccept)](int result)
                {
                    std::unique_ptr<juce::AlertWindow> owner(window);
                    if (result != 0)
                    {
                        onAccept(window->getComboBoxComponent("Program")->getText().getIntValue());
                    }
                }),
            false);
    }

    bool isPatchNameValid(const std::string& name)
    {
        static const std::string valid = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789</>+-*$ ";
        for (const char c : name)
        {
            if (valid.find(c) == std::string::npos)
            {
                return false;
            }
        }
        return true;
    }

    void showRenameDialog(const std::string& currentName, std::function<void(const std::string&)> onAccept)
    {
        auto* window = new juce::AlertWindow("Rename patch",
                                             "Allowed: A-Z 0-9 < / > + - * $ and space",
                                             juce::MessageBoxIconType::NoIcon);
        window->addTextEditor("Name", juce::String(currentName));
        window->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

        window->enterModalState(true,
            juce::ModalCallbackFunction::create(
                [window, onAccept = std::move(onAccept)](int result)
                {
                    std::unique_ptr<juce::AlertWindow> owner(window);
                    if (result == 0)
                    {
                        return;
                    }
                    auto name = window->getTextEditorContents("Name").toUpperCase().toStdString();
                    if (isPatchNameValid(name))
                    {
                        onAccept(name);
                    }
                    else
                    {
                        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                              "Rename patch", "Invalid characters in name.");
                    }
                }),
            false);
    }

    void showAboutDialog(const std::string& productNameAndVersion)
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::InfoIcon, "About Xplorer",
            juce::String(productNameAndVersion)
                + "\n\nReal-time editor for the Oberheim Xpander / Matrix-12."
                  "\ngithub.com/xplorer2716/XplorerEditor");
    }
}
