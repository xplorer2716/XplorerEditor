#include "xplorer/model/XpanderToneIO.hpp"

#include "xplorer/model/XpanderSinglePatch.hpp"
#include "xplorer/model/XpanderTone.hpp"

#include <algorithm>
#include <format>
#include <fstream>

namespace xplorer::model
{
    using midiapp::model::AbstractTone;
    using midiapp::model::ToneException;

    namespace
    {
        std::vector<std::uint8_t> readFileBytes(const std::string& fileName)
        {
            std::ifstream stream(fileName, std::ios::binary);
            if (!stream.good())
            {
                throw ToneException("File does not exist: " + fileName);
            }
            return {std::istreambuf_iterator<char>(stream), {}};
        }
    }

    bool SinglePatchIterator::isSinglePatch(std::span<const std::uint8_t> frame)
    {
        return frame.size() >= XpanderSinglePatch::PATCH_INTRO.size()
               && std::equal(XpanderSinglePatch::PATCH_INTRO.begin(),
                             XpanderSinglePatch::PATCH_INTRO.end(), frame.begin());
    }

    std::optional<std::vector<std::uint8_t>> SinglePatchIterator::next()
    {
        while (auto frame = _iterator.next())
        {
            if (isSinglePatch(*frame))
            {
                return frame;
            }
        }
        return std::nullopt;
    }

    SysexFileType determineSysexFileType(const std::string& fileName)
    {
        std::vector<std::uint8_t> content;
        try
        {
            content = readFileBytes(fileName);
        }
        catch (const ToneException&)
        {
            return SysexFileType::Unknown;
        }

        xpl::midi::SysexStreamIterator iterator(content);
        int sysexCount = 0;
        bool firstIsSinglePatch = false;
        while (auto frame = iterator.next())
        {
            ++sysexCount;
            if (sysexCount == 1)
            {
                firstIsSinglePatch = SinglePatchIterator::isSinglePatch(*frame);
            }
            if (sysexCount > 1)
            {
                return SysexFileType::AllDataDump; // several frames: bank
            }
        }
        if (sysexCount == 1 && firstIsSinglePatch)
        {
            return SysexFileType::SingleTone;
        }
        // one unrecognized frame (e.g. multi patch) counts as a dump
        return sysexCount == 1 ? SysexFileType::AllDataDump : SysexFileType::Unknown;
    }

    void XpanderToneReader::readTone(const std::string& filename, AbstractTone& tone)
    {
        try
        {
            const auto content = readFileBytes(filename);
            SinglePatchIterator iterator(content);
            const auto frame = iterator.next();
            if (!frame.has_value())
            {
                throw ToneException("No single patch data found");
            }
            static_cast<XpanderTone&>(tone).fromByteArray(*frame);
        }
        catch (const std::exception& e)
        {
            throw ToneException(std::format("Unable to load sysex data from {}.\r\n{}", filename, e.what()));
        }
    }

    std::vector<std::pair<std::string, std::unique_ptr<AbstractTone>>>
    XpanderToneReader::readTones(const std::string& filename)
    {
        std::vector<std::pair<std::string, std::unique_ptr<AbstractTone>>> result;
        try
        {
            const auto content = readFileBytes(filename);
            SinglePatchIterator iterator(content);
            while (auto frame = iterator.next())
            {
                auto tone = std::make_unique<XpanderTone>();
                tone->fromByteArray(*frame);
                auto name = tone->toneName();
                result.emplace_back(std::move(name), std::move(tone));
            }
            if (result.empty())
            {
                throw ToneException("No single patch data found");
            }
        }
        catch (const std::exception& e)
        {
            throw ToneException(std::format("Unable to load sysex data from {}.\r\n{}", filename, e.what()));
        }
        return result;
    }

    void XpanderToneWriter::writeTone(const std::string& filename, const AbstractTone& tone)
    {
        std::ofstream stream(filename, std::ios::binary | std::ios::trunc);
        const auto data = static_cast<const XpanderTone&>(tone).toByteArray();
        stream.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
        if (!stream.good())
        {
            throw ToneException(std::format("Unable to save tone {} to file {}", tone.toneName(), filename));
        }
    }
}
