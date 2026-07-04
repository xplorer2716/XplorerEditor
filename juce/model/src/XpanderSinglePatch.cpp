#include "xplorer/model/XpanderSinglePatch.hpp"

#include "midiapp/model/ToneIO.hpp"
#include "xplorer/model/PacketizedBinary.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace xplorer::model
{
    namespace
    {
        constexpr std::uint8_t VALUE_MASK = 0x3F;
        constexpr std::uint8_t SIGN_MASK = 0x40;
        constexpr std::uint8_t QUANTIZE_MASK = 0x80;
        constexpr std::uint8_t EOX = 0xF7;
    }

    void XpanderSinglePatch::fromByteArray(std::span<const std::uint8_t> data)
    {
        if (data.size() < SINGLE_PATCH_LENGTH)
        {
            std::ostringstream message;
            message << "Not enough sysex data. Waiting for " << SINGLE_PATCH_LENGTH
                    << " bytes (SINGLE_PATCH_LENGTH), but get only " << data.size() << " bytes";
            throw midiapp::model::ToneException(message.str());
        }

        // Locate the patch intro anywhere in the buffer (reference scan).
        std::size_t position = 0;
        bool found = false;
        while (position + SINGLE_PATCH_LENGTH <= data.size())
        {
            if (std::equal(PATCH_INTRO.begin(), PATCH_INTRO.end(), data.begin() + static_cast<std::ptrdiff_t>(position)))
            {
                found = true;
                position += SINGLE_PATCH_INTRO_LENGTH; // skip intro + program number
                break;
            }
            ++position;
        }
        if (!found)
        {
            throw midiapp::model::ToneException("No single patch found in sysex data");
        }

        PacketizedBinaryReader reader(data.subspan(position));
        for (auto& vco : vcos)
        {
            vco.freq = reader.readByte();
            vco.detune = reader.readSByte();
            vco.pw = reader.readByte();
            vco.vol = reader.readByte();
            vco.mod = reader.readByte();
            vco.wave = reader.readByte();
        }
        vcf.freq = reader.readByte();
        vcf.res = reader.readByte();
        vcf.fmode = reader.readByte();
        vcf.vca1 = reader.readByte();
        vcf.vca2 = reader.readByte();
        vcf.mod = reader.readByte();
        fmAndLag.f_amp = reader.readByte();
        fmAndLag.f_dest = reader.readByte();
        fmAndLag.lag_in = reader.readByte();
        fmAndLag.lag_rate = reader.readByte();
        fmAndLag.lag_mode = reader.readByte();
        for (auto& lfo : lfos)
        {
            lfo.speed = reader.readByte();
            lfo.retrig_mode = reader.readByte();
            lfo.lag = reader.readByte();
            lfo.wave = reader.readByte();
            lfo.retrig = reader.readByte();
            lfo.sample = reader.readByte();
            lfo.amp = reader.readByte();
        }
        for (auto& env : enveloppes)
        {
            env.flags = reader.readByte();
            env.lfotrig = reader.readByte();
            env.delay = reader.readByte();
            env.attack = reader.readByte();
            env.decay = reader.readByte();
            env.sustain = reader.readByte();
            env.release = reader.readByte();
            env.amp = reader.readByte();
        }
        for (auto& track : tracks)
        {
            track.input = reader.readByte();
            for (auto& point : track.points)
            {
                point = reader.readByte();
            }
        }
        for (auto& ramp : ramps)
        {
            ramp.rate = reader.readByte();
            ramp.flags = reader.readByte();
            ramp.lfotrig = reader.readByte();
        }
        for (auto& entry : modulationEntries)
        {
            entry.source = reader.readByte();
            const std::uint8_t amountSignQuantize = reader.readByte();
            entry.amount = static_cast<std::int8_t>(amountSignQuantize & VALUE_MASK);
            if ((amountSignQuantize & SIGN_MASK) == SIGN_MASK)
            {
                entry.amount = static_cast<std::int8_t>(-entry.amount);
            }
            entry.quantize = (amountSignQuantize & QUANTIZE_MASK) == QUANTIZE_MASK;
            entry.dest = reader.readByte();
        }
        for (auto& nameByte : name)
        {
            nameByte = reader.readByte();
        }
    }

    std::vector<std::uint8_t> XpanderSinglePatch::toByteArray(int programNumber) const
    {
        std::vector<std::uint8_t> data;
        data.reserve(SINGLE_PATCH_LENGTH);
        data.insert(data.end(), PATCH_INTRO.begin(), PATCH_INTRO.end());
        data.push_back(static_cast<std::uint8_t>(programNumber));

        PacketizedBinaryWriter writer(data);
        for (const auto& vco : vcos)
        {
            writer.writeByte(vco.freq);
            writer.writeSByte(vco.detune);
            writer.writeByte(vco.pw);
            writer.writeByte(vco.vol);
            writer.writeByte(vco.mod);
            writer.writeByte(vco.wave);
        }
        writer.writeByte(vcf.freq);
        writer.writeByte(vcf.res);
        writer.writeByte(vcf.fmode);
        writer.writeByte(vcf.vca1);
        writer.writeByte(vcf.vca2);
        writer.writeByte(vcf.mod);
        writer.writeByte(fmAndLag.f_amp);
        writer.writeByte(fmAndLag.f_dest);
        writer.writeByte(fmAndLag.lag_in);
        writer.writeByte(fmAndLag.lag_rate);
        writer.writeByte(fmAndLag.lag_mode);
        for (const auto& lfo : lfos)
        {
            writer.writeByte(lfo.speed);
            writer.writeByte(lfo.retrig_mode);
            writer.writeByte(lfo.lag);
            writer.writeByte(lfo.wave);
            writer.writeByte(lfo.retrig);
            writer.writeByte(lfo.sample);
            writer.writeByte(lfo.amp);
        }
        for (const auto& env : enveloppes)
        {
            writer.writeByte(env.flags);
            writer.writeByte(env.lfotrig);
            writer.writeByte(env.delay);
            writer.writeByte(env.attack);
            writer.writeByte(env.decay);
            writer.writeByte(env.sustain);
            writer.writeByte(env.release);
            writer.writeByte(env.amp);
        }
        for (const auto& track : tracks)
        {
            writer.writeByte(track.input);
            writer.writeBytes(track.points);
        }
        for (const auto& ramp : ramps)
        {
            writer.writeByte(ramp.rate);
            writer.writeByte(ramp.flags);
            writer.writeByte(ramp.lfotrig);
        }
        for (const auto& entry : modulationEntries)
        {
            writer.writeByte(entry.source);
            auto amount = static_cast<std::uint8_t>(std::abs(entry.amount));
            if (entry.amount < 0)
            {
                amount |= SIGN_MASK;
            }
            if (entry.quantize)
            {
                amount |= QUANTIZE_MASK;
            }
            writer.writeByte(amount);
            writer.writeByte(entry.dest);
        }
        writer.writeBytes(name);
        data.push_back(EOX); // not packed
        return data;
    }

    std::string XpanderSinglePatch::getNameFromByteArray(std::span<const std::uint8_t> bytes)
    {
        if (bytes.size() != SINGLE_PATCH_LENGTH)
        {
            return {};
        }
        // Name is located at the top end of the array: 8 packed bytes + EOX.
        const auto offset = bytes.size() - constants::TONE_NAME_LENGTH * 2 - 1;
        PacketizedBinaryReader reader(bytes.subspan(offset));
        std::string result;
        for (int i = 0; i < constants::TONE_NAME_LENGTH; ++i)
        {
            result.push_back(static_cast<char>(reader.readByte()));
        }
        return result;
    }

    std::string XpanderSinglePatch::patchName() const
    {
        return {name.begin(), name.end()};
    }

    void XpanderSinglePatch::setPatchName(const std::string& newName)
    {
        name.fill(' ');
        const auto count = std::min(newName.size(), name.size());
        std::copy_n(newName.begin(), count, name.begin());
    }
}
