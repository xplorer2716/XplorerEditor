#include "Dialogs.hpp"

#include "BinaryData.h"
#include "DesignTokens.hpp"
#include "SbomReader.hpp"

#include "xplorer/app/MidiAutomationTable.hpp"
#include "xplorer/model/XpanderTone.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <algorithm>
#include <memory>
#include <regex>

namespace xplorer::app
{
    namespace
    {
        constexpr const char* PROJECT_REPO_URL = "https://github.com/xplorer2716/XplorerEditor";
        constexpr const char* GPL_LICENSE_URL = "https://www.gnu.org/licenses/gpl-3.0.html";

        // Port of the reference AboutForm (Xplorer/View/AboutForm.Designer.cs):
        // a fixed-size white dialog, the `About.jpg` VFD close-up docked on
        // the left (stretched to fill, matching the reference PictureBox), and
        // title/version/copyright/link/licence text at x=146. Clicking anywhere
        // outside the controls closes the window, like the reference form's
        // whole-form Click handler.
        //
        // Third-party licences are deliberately NOT enumerated here: the
        // dependencies button opens the SBOM-driven window instead, so this
        // dialog keeps the reference form's size however many dependencies the
        // product gains. [RQ-GUI-025, RQ-GUI-057]
        class AboutContent final : public juce::Component
        {
        public:
            explicit AboutContent(const juce::String& productNameAndVersion)
                : _link(PROJECT_REPO_URL, juce::URL(PROJECT_REPO_URL)),
                  _licenseLink(GPL_LICENSE_URL, juce::URL(GPL_LICENSE_URL))
            {
                _image = juce::ImageCache::getFromMemory(BinaryData::about_jpg, BinaryData::about_jpgSize);

                juce::Font titleFont{juce::FontOptions{TITLE_SIZE}};
                titleFont.setBold(true);
                _title.setFont(titleFont);
                _title.setText("Xplorer", juce::dontSendNotification);

                _version.setText(productNameAndVersion, juce::dontSendNotification);
                _copyright.setText("Copyright (c) 2012-2026 by Pascal Schmitt", juce::dontSendNotification);

                _notice.setText("This software is released under GNU General Public License v3.0",
                                juce::dontSendNotification);

                // One explicit, token-derived size for every body row. Before
                // this, the Labels used juce::Label's own 15px default while the
                // HyperlinkButtons used HyperlinkButton's 14px default shrunk to
                // 70% of their row height (~12.6px here) -- two unrelated
                // fallbacks, neither chosen, which is what made the links read
                // as noticeably smaller than the text beside them. [RQ-GUI-025]
                const juce::Font bodyFont{juce::FontOptions{BODY_TEXT_SIZE}};

                for (auto* label : {&_version, &_copyright, &_notice})
                {
                    label->setFont(bodyFont);
                }
                for (auto* label : {&_title, &_version, &_copyright, &_notice})
                {
                    label->setColour(juce::Label::textColourId,
                                     label == &_notice ? tokens::semantic::textOnDocumentMuted
                                                       : tokens::semantic::textOnDocument);
                    // Non-interactive: lets background clicks fall through to
                    // this component's own mouseUp (close-on-click), matching
                    // the reference's whole-form Click handler.
                    label->setInterceptsMouseClicks(false, false);
                    addAndMakeVisible(label);
                }
                for (auto* link : {&_link, &_licenseLink})
                {
                    link->setColour(juce::HyperlinkButton::textColourId, tokens::semantic::textLink);
                    // HyperlinkButton centres its text by default, unlike Label
                    // (left-aligned) -- align it with the rest of the column.
                    // resizeToMatchComponentHeight = false is what pins the font
                    // to bodyFont instead of letting the row height shrink it.
                    link->setFont(bodyFont, false, juce::Justification::centredLeft);
                    addAndMakeVisible(link);
                }

                _dependenciesButton.onClick = [] { showDependenciesWindow(); };
                addAndMakeVisible(_dependenciesButton);

                setSize(WIDTH, HEIGHT);
            }

            void paint(juce::Graphics& g) override
            {
                g.fillAll(tokens::semantic::surfaceDocument);
                if (_image.isValid())
                {
                    g.drawImage(_image, 0, 0, IMAGE_WIDTH, getHeight(), 0, 0,
                               _image.getWidth(), _image.getHeight());
                }
                g.setColour(tokens::semantic::documentSeparator);
                g.drawLine(static_cast<float>(TEXT_X), SEPARATOR_Y,
                          static_cast<float>(WIDTH - MARGIN_RIGHT), SEPARATOR_Y, 1.0f);
            }

            void resized() override
            {
                _title.setBounds(TEXT_X, TITLE_Y, TEXT_WIDTH, ROW_HEIGHT);
                _version.setBounds(TEXT_X, VERSION_Y, TEXT_WIDTH, ROW_HEIGHT);
                _copyright.setBounds(TEXT_X, COPYRIGHT_Y, TEXT_WIDTH, ROW_HEIGHT);
                _link.setBounds(TEXT_X + LINK_LABEL_INSET_COMPENSATION, COPYRIGHT_Y + ROW_HEIGHT + LABEL_TO_LINK_GAP,
                               TEXT_WIDTH - LINK_LABEL_INSET_COMPENSATION, ROW_HEIGHT);
                _dependenciesButton.setBounds(TEXT_X, DEPENDENCIES_BUTTON_Y, DEPENDENCIES_BUTTON_WIDTH,
                                             tokens::semantic::dialogRowHeight);
                _notice.setBounds(TEXT_X, NOTICE_Y, TEXT_WIDTH, ROW_HEIGHT);
                _licenseLink.setBounds(TEXT_X + LINK_LABEL_INSET_COMPENSATION, NOTICE_Y + ROW_HEIGHT + LABEL_TO_LINK_GAP,
                                      TEXT_WIDTH - LINK_LABEL_INSET_COMPENSATION, ROW_HEIGHT);
            }

            void mouseUp(const juce::MouseEvent&) override
            {
                if (auto* dw = findParentComponentOfClass<juce::DialogWindow>())
                {
                    dw->exitModalState(0);
                }
            }

        private:
            static constexpr int WIDTH = 474;
            static constexpr int IMAGE_WIDTH = 140;
            static constexpr int TEXT_X = 146;
            static constexpr int TEXT_WIDTH = 320;
            static constexpr int ROW_HEIGHT = 18;
            static constexpr int MARGIN_RIGHT = 9;
            static constexpr int MARGIN_BOTTOM = 11;
            static constexpr float SEPARATOR_Y = 204.0f;
            static constexpr float TITLE_SIZE = tokens::semantic::textDisplay;
            // Every body row -- version, copyright, licence notice, both links.
            // Aliases the size the settings dialog already uses for its own body
            // text, so the two dialogs read as one family. [RQ-DSN-061]
            static constexpr float BODY_TEXT_SIZE = tokens::semantic::textSubtitle;
            static constexpr int TITLE_Y = 9;
            static constexpr int VERSION_Y = 34;
            static constexpr int COPYRIGHT_Y = 59;
            static constexpr int DEPENDENCIES_BUTTON_Y = 110;
            static constexpr int DEPENDENCIES_BUTTON_WIDTH = tokens::semantic::dialogLabelWidth;
            static constexpr int NOTICE_Y = 212;
            // The licence block is the last thing in the dialog: one notice row,
            // its URL below, then the bottom margin. Derived rather than fixed
            // so raising the body size cannot silently clip the last link --
            // which is exactly what a hard-coded 261 would have done.
            static constexpr int HEIGHT =
                NOTICE_Y + ROW_HEIGHT + tokens::semantic::layoutHairline + ROW_HEIGHT + MARGIN_BOTTOM;
            // juce::Label has a default 5px left border (text starts at
            // local x=5); HyperlinkButton only insets 1px (text starts at
            // local x=1) -- shift the links right by the 4px difference so
            // their text lines up with the labels above/below them.
            static constexpr int LINK_LABEL_INSET_COMPENSATION = tokens::global::space4;
            // Same token-derived gap used below every "label + URL" pair
            // (copyright/link, notice/licenseLink) so the two blocks read as
            // one consistent rhythm instead of two hand-picked offsets.
            static constexpr int LABEL_TO_LINK_GAP = tokens::semantic::layoutHairline;

            juce::Image _image;
            juce::Label _title, _version, _copyright, _notice;
            juce::HyperlinkButton _link, _licenseLink;
            juce::TextButton _dependenciesButton{"Dependencies..."};
        };

        // ---- About > Dependencies ------------------------------------------
        // Renders whatever the SBOM shipped beside the executable says, and
        // nothing else: no dependency name, version, licence or URL is written
        // here, which is the whole point of reading a build artifact rather than
        // maintaining a list in C++. [RQ-GUI-057, ADR-ABT-001 (DEC-ABT-001)]

        constexpr int DEP_NAME_COLUMN_WIDTH = tokens::semantic::dialogLabelWidth;
        constexpr int DEP_VERSION_COLUMN_WIDTH = tokens::semantic::dialogChooseWidth;
        constexpr int DEP_LICENSE_COLUMN_WIDTH = tokens::semantic::dialogLabelWidth;
        // The website is the only column whose content is unbounded (a URL), so
        // it takes the slack rather than a measured width.
        constexpr int DEP_WEBSITE_COLUMN_WIDTH =
            tokens::semantic::dialogLabelWidth + tokens::semantic::dialogResetWidth;
        constexpr int DEP_COLUMN_GAP = tokens::semantic::layoutButtonGap;
        constexpr int DEP_CONTENT_WIDTH = DEP_NAME_COLUMN_WIDTH + DEP_VERSION_COLUMN_WIDTH
                                        + DEP_LICENSE_COLUMN_WIDTH + DEP_WEBSITE_COLUMN_WIDTH
                                        + 3 * DEP_COLUMN_GAP;
        constexpr int DEP_ROW_HEIGHT = tokens::semantic::dialogRowHeight;
        constexpr float DEP_TEXT_SIZE = tokens::semantic::textSubtitle;
        // A field the SBOM left absent or marked NOASSERTION/NONE: shown as a
        // neutral dash, never as the sentinel itself. [DEC-ABT-003]
        constexpr const char* DEP_ABSENT_FIELD = "\xe2\x80\x94"; // em dash

        /** One dependency: three text cells plus an activatable website. */
        class DependencyRow final : public juce::Component
        {
        public:
            explicit DependencyRow(const SbomEntry& entry)
                : _website(entry.website, juce::URL(entry.website))
            {
                const juce::Font font{juce::FontOptions{DEP_TEXT_SIZE}};
                setCell(_name, entry.name, font);
                setCell(_version, entry.version, font);
                setCell(_license, entry.license, font);

                if (entry.website.isNotEmpty())
                {
                    _website.setColour(juce::HyperlinkButton::textColourId, tokens::semantic::textLink);
                    _website.setFont(font, false, juce::Justification::centredLeft);
                    addAndMakeVisible(_website);
                }
                else
                {
                    setCell(_websitePlaceholder, {}, font);
                }
            }

            void resized() override
            {
                auto area = getLocalBounds();
                _name.setBounds(area.removeFromLeft(DEP_NAME_COLUMN_WIDTH));
                area.removeFromLeft(DEP_COLUMN_GAP);
                _version.setBounds(area.removeFromLeft(DEP_VERSION_COLUMN_WIDTH));
                area.removeFromLeft(DEP_COLUMN_GAP);
                _license.setBounds(area.removeFromLeft(DEP_LICENSE_COLUMN_WIDTH));
                area.removeFromLeft(DEP_COLUMN_GAP);
                // The link insets its text 1px against a Label's 5px, so it is
                // nudged right to line up with the columns beside it -- the same
                // compensation the About dialog applies.
                _website.setBounds(area.withTrimmedLeft(tokens::global::space4));
                _websitePlaceholder.setBounds(area);
            }

        private:
            void setCell(juce::Label& label, const juce::String& text, const juce::Font& font)
            {
                label.setFont(font);
                label.setColour(juce::Label::textColourId, tokens::semantic::textOnDocument);
                label.setText(text.isNotEmpty() ? text : DEP_ABSENT_FIELD, juce::dontSendNotification);
                addAndMakeVisible(label);
            }

            juce::Label _name, _version, _license, _websitePlaceholder;
            juce::HyperlinkButton _website;
        };

        /** The scrollable body: one row per dependency. */
        class DependencyList final : public juce::Component
        {
        public:
            explicit DependencyList(const std::vector<SbomEntry>& entries)
            {
                _rows.reserve(entries.size());
                for (const auto& entry : entries)
                {
                    auto row = std::make_unique<DependencyRow>(entry);
                    addAndMakeVisible(*row);
                    _rows.push_back(std::move(row));
                }
                setSize(DEP_CONTENT_WIDTH, static_cast<int>(_rows.size()) * DEP_ROW_HEIGHT);
            }

            void resized() override
            {
                auto area = getLocalBounds();
                for (auto& row : _rows)
                {
                    row->setBounds(area.removeFromTop(DEP_ROW_HEIGHT));
                }
            }

        private:
            std::vector<std::unique_ptr<DependencyRow>> _rows;
        };

        /** Why the list is empty, in the user's terms. Every branch is a real,
            reachable configuration -- a developer build with no SBOM copied, a
            truncated download, a future CI change -- so none of them may fail
            silently or show a blank frame. [RQ-GUI-057, DEC-ABT-005] */
        juce::String explanationFor(SbomStatus status, const juce::File& sbomFile)
        {
            const auto location = "\n\nExpected at:\n" + sbomFile.getFullPathName();
            switch (status)
            {
                case SbomStatus::FileNotFound:
                    return "The software bill of materials that lists this application's "
                           "dependencies was not found." + location;
                case SbomStatus::Unreadable:
                    return "The software bill of materials could not be read." + location;
                case SbomStatus::InvalidJson:
                    return "The software bill of materials is not valid JSON and could not "
                           "be parsed." + location;
                case SbomStatus::NotSpdxOrEmpty:
                    return "The software bill of materials contains no dependency "
                           "information." + location;
                case SbomStatus::Loaded:
                    break;
            }
            return {};
        }

        class DependenciesContent final : public juce::Component
        {
        public:
            DependenciesContent()
            {
                const auto sbomFile = defaultSbomFile();
                const auto result = readSbom(sbomFile);

                const juce::Font headerFont{juce::FontOptions{DEP_TEXT_SIZE}};
                if (result.status == SbomStatus::Loaded)
                {
                    const char* const headings[] = {"Component", "Version", "Licence", "Website"};
                    int index = 0;
                    for (auto* header : {&_nameHeader, &_versionHeader, &_licenseHeader, &_websiteHeader})
                    {
                        header->setFont(headerFont);
                        header->setColour(juce::Label::textColourId, tokens::semantic::textOnDocumentMuted);
                        header->setText(headings[index++], juce::dontSendNotification);
                        addAndMakeVisible(header);
                    }

                    _list = std::make_unique<DependencyList>(result.entries);
                    _viewport.setViewedComponent(_list.get(), false);
                    _viewport.setScrollBarsShown(true, false);
                    addAndMakeVisible(_viewport);

                    const auto rows = juce::jlimit(MIN_VISIBLE_ROWS, MAX_VISIBLE_ROWS,
                                                   static_cast<int>(result.entries.size()));
                    setSize(WIDTH, HEADER_BLOCK_HEIGHT + rows * DEP_ROW_HEIGHT
                                       + tokens::semantic::layoutMargin);
                }
                else
                {
                    _message.setFont(headerFont);
                    _message.setColour(juce::Label::textColourId, tokens::semantic::textOnDocument);
                    _message.setJustificationType(juce::Justification::topLeft);
                    _message.setText(explanationFor(result.status, sbomFile), juce::dontSendNotification);
                    addAndMakeVisible(_message);
                    setSize(WIDTH, MESSAGE_HEIGHT);
                }
            }

            void paint(juce::Graphics& g) override
            {
                g.fillAll(tokens::semantic::surfaceDocument);
                if (_list != nullptr)
                {
                    g.setColour(tokens::semantic::documentSeparator);
                    const auto y = static_cast<float>(tokens::semantic::layoutMargin + DEP_ROW_HEIGHT);
                    g.drawLine(static_cast<float>(tokens::semantic::layoutMargin), y,
                               static_cast<float>(getWidth() - tokens::semantic::layoutMargin), y,
                               tokens::semantic::strokeBorder);
                }
            }

            void resized() override
            {
                auto area = getLocalBounds().reduced(tokens::semantic::layoutMargin);
                if (_list == nullptr)
                {
                    _message.setBounds(area);
                    return;
                }

                auto header = area.removeFromTop(DEP_ROW_HEIGHT);
                _nameHeader.setBounds(header.removeFromLeft(DEP_NAME_COLUMN_WIDTH));
                header.removeFromLeft(DEP_COLUMN_GAP);
                _versionHeader.setBounds(header.removeFromLeft(DEP_VERSION_COLUMN_WIDTH));
                header.removeFromLeft(DEP_COLUMN_GAP);
                _licenseHeader.setBounds(header.removeFromLeft(DEP_LICENSE_COLUMN_WIDTH));
                header.removeFromLeft(DEP_COLUMN_GAP);
                _websiteHeader.setBounds(header);

                area.removeFromTop(tokens::semantic::layoutSectionGap);
                _viewport.setBounds(area);
                _list->setSize(juce::jmax(DEP_CONTENT_WIDTH, area.getWidth()), _list->getHeight());
            }

        private:
            static constexpr int WIDTH = DEP_CONTENT_WIDTH + 2 * tokens::semantic::layoutMargin;
            // Outer margin + column headings + the gap under them.
            static constexpr int HEADER_BLOCK_HEIGHT =
                2 * tokens::semantic::layoutMargin + DEP_ROW_HEIGHT + tokens::semantic::layoutSectionGap;
            // Enough rows that the window never opens as a sliver, few enough
            // that a large generated SBOM scrolls instead of filling the screen.
            static constexpr int MIN_VISIBLE_ROWS = 4;
            static constexpr int MAX_VISIBLE_ROWS = 14;
            static constexpr int MESSAGE_HEIGHT = 5 * DEP_ROW_HEIGHT;

            juce::Label _nameHeader, _versionHeader, _licenseHeader, _websiteHeader;
            juce::Label _message;
            juce::Viewport _viewport;
            std::unique_ptr<DependencyList> _list;
        };
    }

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

    namespace
    {
        // Reference RenamePatchForm.Designer.cs (tbxPatchName.MaxLength = 8,
        // CharacterCasing.Upper) and RenamePatchForm.IsPatchNameValid's
        // validChars: the XPANDER display only understands this fixed glyph
        // set in a stored patch name, so it is not a generic-ASCII rule.
        // Single regex, matched one character at a time, so the live
        // keystroke filter below and isPatchNameValid() can never drift
        // apart. [RQ-GUI-025]
        const std::regex PATCH_NAME_CHAR_PATTERN{"[A-Z0-9<>/+*$ -]"};

        bool isAllowedPatchNameChar(char c)
        {
            return std::regex_match(&c, &c + 1, PATCH_NAME_CHAR_PATTERN);
        }

        /// Live filter for the rename dialog's TextEditor: uppercases and
        /// strips characters outside PATCH_NAME_CHAR_PATTERN as the user
        /// types or pastes, and caps the result at the stored patch-name
        /// length -- mirrors the reference TextBox's CharacterCasing/
        /// MaxLength instead of only rejecting on submit. [RQ-GUI-025]
        class PatchNameInputFilter final : public juce::TextEditor::InputFilter
        {
        public:
            juce::String filterNewText(juce::TextEditor& editor, const juce::String& newInput) override
            {
                juce::String kept;
                for (const juce::juce_wchar c : newInput.toUpperCase())
                {
                    // Guard against truncating a non-ASCII code point down to
                    // a byte that happens to collide with an allowed char.
                    if (c >= 0 && c <= 0x7F && isAllowedPatchNameChar(static_cast<char>(c)))
                    {
                        kept += juce::String::charToString(c);
                    }
                }

                const auto keptLength = editor.getTotalNumChars() - editor.getHighlightedRegion().getLength();
                const auto room = juce::jmax(0, model::constants::TONE_NAME_LENGTH - keptLength);
                return kept.substring(0, room);
            }
        };
    }

    bool isPatchNameValid(const std::string& name)
    {
        return name.size() <= static_cast<std::size_t>(model::constants::TONE_NAME_LENGTH)
            && std::all_of(name.begin(), name.end(), isAllowedPatchNameChar);
    }

    void showRenameDialog(const std::string& currentName, std::function<void(const std::string&)> onAccept)
    {
        auto* window = new juce::AlertWindow("Rename patch",
                                             "Allowed: A-Z 0-9 < / > + - * $ and space, max "
                                                 + juce::String(model::constants::TONE_NAME_LENGTH)
                                                 + " characters",
                                             juce::MessageBoxIconType::NoIcon);
        window->addTextEditor("Name", juce::String(currentName));
        window->getTextEditor("Name")->setInputFilter(new PatchNameInputFilter(), true);
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
                                                              "Rename patch", "Invalid patch name.");
                    }
                }),
            false);
    }

    void showAboutDialog(const std::string& productNameAndVersion)
    {
        juce::DialogWindow::LaunchOptions options;
        options.content.setOwned(new AboutContent(juce::String(productNameAndVersion)));
        options.dialogTitle = "About";
        options.dialogBackgroundColour = tokens::semantic::surfaceDocument; // reference AboutForm.BackColor
        options.escapeKeyTriggersCloseButton = true;
        options.useNativeTitleBar = true;
        options.resizable = false;
        options.launchAsync();
    }

    void showDependenciesWindow()
    {
        juce::DialogWindow::LaunchOptions options;
        options.content.setOwned(new DependenciesContent());
        options.dialogTitle = "Dependencies";
        options.dialogBackgroundColour = tokens::semantic::surfaceDocument;
        options.escapeKeyTriggersCloseButton = true;
        options.useNativeTitleBar = true;
        // Resizable: a generated SBOM can carry long component names and URLs
        // that no fixed width fits. [RQ-GUI-057]
        options.resizable = true;
        options.launchAsync();
    }
}
