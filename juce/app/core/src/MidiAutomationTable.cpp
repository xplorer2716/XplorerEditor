#include "xplorer/app/MidiAutomationTable.hpp"

#include <array>
#include <charconv>
#include <string>
#include <string_view>

namespace xplorer::app
{
    namespace
    {
        std::string htmlEscape(const std::string& text)
        {
            std::string out;
            out.reserve(text.size());
            for (const char c : text)
            {
                switch (c)
                {
                    case '&': out += "&amp;"; break;
                    case '<': out += "&lt;"; break;
                    case '>': out += "&gt;"; break;
                    default: out += c; break;
                }
            }
            return out;
        }

        const std::array<std::string, 129>& ccNames()
        {
            static const std::array<std::string, 129> names = {
#include "GeneratedControlChangeNames.inc"
            };
            return names;
        }
    }

    int controlChangeNameCount() { return static_cast<int>(ccNames().size()); }

    int unassignedControlChange() { return controlChangeNameCount() - 1; }

    const std::string& controlChangeName(int ccNumber)
    {
        const auto& names = ccNames();
        if (ccNumber < 0 || ccNumber >= static_cast<int>(names.size()))
        {
            return names.back(); // "None"
        }
        return names[static_cast<std::size_t>(ccNumber)];
    }

    std::optional<std::pair<std::string, int>> parseAutomationEntry(const std::string& entry)
    {
        // Split on the last ';' (parameter names never contain one).
        const auto delimiter = entry.find_last_of(';');
        if (delimiter == std::string::npos || delimiter == 0 || delimiter + 1 >= entry.size())
        {
            return std::nullopt;
        }
        const std::string name = entry.substr(0, delimiter);
        const std::string_view number(entry.data() + delimiter + 1, entry.size() - delimiter - 1);
        int cc = 0;
        const auto result = std::from_chars(number.data(), number.data() + number.size(), cc);
        if (result.ec != std::errc{} || result.ptr != number.data() + number.size())
        {
            return std::nullopt;
        }
        // Reference clamps stored values to 1..128.
        cc = cc < 1 ? 1 : (cc > 128 ? 128 : cc);
        return std::make_pair(name, cc);
    }

    std::string buildMidiMappingHtml(const std::vector<std::pair<std::string, std::string>>& rows,
                                     const std::string& generatedOn)
    {
        // Compact, light, professional; print-friendly. Kept intentionally
        // dense (tight cell padding, no decorative whitespace).
        constexpr auto* website = "https://github.com/xplorer2716/XplorerEditor";
        std::string html;
        html.reserve(2048 + rows.size() * 96);
        html += "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n"
                "<title>Xplorer MIDI Automation Table</title>\n<style>\n"
                "body{font-family:'Segoe UI',Roboto,Helvetica,Arial,sans-serif;color:#222;"
                "background:#fff;margin:24px;font-size:13px;line-height:1.35}\n"
                "h1{font-size:18px;margin:0 0 2px;color:#1a2733}\n"
                ".sub{color:#667;font-size:12px;margin-bottom:14px}\n"
                ".sub a{color:#2673b8;text-decoration:none}\n"
                "table{border-collapse:collapse;width:auto;min-width:360px}\n"
                "th,td{padding:3px 12px;border:1px solid #d7dde3;text-align:left}\n"
                "th{background:#eef2f6;font-weight:600;font-size:11px;text-transform:uppercase;"
                "letter-spacing:.4px;color:#425065}\n"
                "tbody tr:nth-child(even){background:#f7f9fb}\n"
                "td.cc{color:#334;white-space:nowrap}\n"
                ".foot{color:#8a97a3;font-size:11px;margin-top:12px}\n"
                "@media print{body{margin:0}th{background:#e5e5e5!important;"
                "-webkit-print-color-adjust:exact;print-color-adjust:exact}}\n"
                "</style>\n</head>\n<body>\n";
        html += "<h1>MIDI Automation Table</h1>\n";
        html += std::string("<div class=\"sub\">Xplorer &mdash; Oberheim Xpander/Matrix-12 real-time editor"
                            "<br><a href=\"")
                + website + "\">" + website + "</a></div>\n";
        html += "<table>\n<thead><tr><th>Parameter</th><th>Control Change</th></tr></thead>\n<tbody>\n";
        for (const auto& [parameter, ccName] : rows)
        {
            html += "<tr><td>" + htmlEscape(parameter) + "</td><td class=\"cc\">"
                    + htmlEscape(ccName) + "</td></tr>\n";
        }
        html += "</tbody>\n</table>\n";
        html += "<div class=\"foot\">Generated on " + htmlEscape(generatedOn) + "</div>\n";
        html += "</body>\n</html>\n";
        return html;
    }
}
