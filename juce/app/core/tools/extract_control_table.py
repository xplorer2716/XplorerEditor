#!/usr/bin/env python3
"""Regenerates juce/app/core/src/GeneratedControlTable.inc and the main
background image from the reference WinForms sources. Run from the repo root.
[TASK-JUCE-060, RQ-GUI-001, RQ-GUI-007]

Inputs : Xplorer/View/MainForm.resx      (geometry, types, background bitmap)
         Xplorer/View/MainForm.Designer.cs (control types fallback, tags)
Outputs: juce/app/core/src/GeneratedControlTable.inc
         juce/app/assets/main-background.<ext>
"""
import base64
import re
import xml.etree.ElementTree as ET
from collections import Counter

RESX = 'Xplorer/View/MainForm.resx'
DESIGNER = 'Xplorer/View/MainForm.Designer.cs'
TABLE_OUT = 'juce/app/core/src/GeneratedControlTable.inc'
ASSET_DIR = 'juce/app/assets'
SKIPPED_KINDS = ('MenuStrip', 'ToolStripMenuItem', 'ToolStripSeparator')


def main() -> None:
    root = ET.parse(RESX).getroot()
    data = {}
    for d in root.findall('data'):
        v = d.find('value')
        data[d.get('name')] = (v.text if v is not None else None, d.get('type') or '')

    # background bitmap (bytearray.base64 = raw image bytes)
    raw = base64.b64decode(data['$this.BackgroundImage'][0])
    ext = ('png' if raw.startswith(b'\x89PNG') else
           'jpg' if raw[:2] == b'\xff\xd8' else
           'bmp' if raw[:2] == b'BM' else
           'gif' if raw[:3] == b'GIF' else 'bin')
    with open(f'{ASSET_DIR}/main-background.{ext}', 'wb') as f:
        f.write(raw)
    print(f'background: {ext}, {len(raw)} bytes, client size = {data["$this.ClientSize"][0]}')

    # geometry from resx (the form is localizable)
    geo = {}
    for name, (val, typ) in data.items():
        if typ.startswith('System.Drawing.Point') and name.endswith('.Location'):
            geo.setdefault(name[:-9], {})['loc'] = [int(s) for s in val.split(',')]
        elif typ.startswith('System.Drawing.Size') and name.endswith('.Size'):
            geo.setdefault(name[:-5], {})['size'] = [int(s) for s in val.split(',')]

    # types: resx ">>x.Type" metadata first, Designer 'new' expressions as fallback
    types = {n[2:-5]: v.split(',')[0].strip()
             for n, (v, _) in data.items()
             if n.startswith('>>') and n.endswith('.Type') and v}
    src = open(DESIGNER).read()
    for ctrl, typ in re.findall(r'this\.([A-Za-z_0-9]+) = new ([A-Za-z_.0-9]+)\(', src):
        types.setdefault(ctrl, typ)
    tags = dict(re.findall(r'this\.([A-Za-z_0-9]+)\.Tag = "([^"]*)";', src))

    # Control .Text (checkbox / label captions). The form is localizable, so
    # these live in the .resx, not the Designer. [RQ-GUI-032]
    labels = {}
    for name, (val, _) in data.items():
        if name.endswith('.Text') and val and val.strip():
            labels[name[:-5]] = val

    # Parent-child tree: a WinForms control's Location is relative to its
    # parent container, so nested controls (e.g. the family selector radios
    # inside PanelENV) need their ancestors' Locations added to become
    # absolute canvas coordinates.
    parent_of = dict((child, parent) for parent, child
                     in re.findall(r'this\.([A-Za-z_0-9]+)\.Controls\.Add\(this\.([A-Za-z_0-9]+)\)', src))

    def absolute_loc(ctrl):
        x, y = geo.get(ctrl, {}).get('loc', (0, 0))
        node = ctrl
        while node in parent_of:
            node = parent_of[node]
            px, py = geo.get(node, {}).get('loc', (0, 0))
            x += px
            y += py
        return x, y

    rows = []
    for ctrl, g in sorted(geo.items()):
        if ctrl.startswith('$'):
            continue
        kind = types.get(ctrl, '?').split('.')[-1]
        if any(s in kind for s in SKIPPED_KINDS):
            continue
        (x, y), (w, h) = absolute_loc(ctrl), g.get('size', (0, 0))
        rows.append((ctrl, kind, x, y, w, h, tags.get(ctrl, ''), labels.get(ctrl, '')))

    def cpp_escape(text):
        return text.replace('\\', '\\\\').replace('"', '\\"')

    with open(TABLE_OUT, 'w') as f:
        f.write('// Mechanically extracted from Xplorer/View/MainForm.resx (geometry,\n'
                '// captions) and MainForm.Designer.cs (types, tags). Do not edit by\n'
                '// hand; regenerate with juce/app/core/tools/extract_control_table.py.\n'
                '// Logical canvas coordinates. [RQ-GUI-001]\n')
        for ctrl, kind, x, y, w, h, tag, label in rows:
            f.write(f'    {{"{ctrl}", ControlKind::{kind}, {x}, {y}, {w}, {h}, '
                    f'"{tag}", "{cpp_escape(label)}"}},\n')
    print(f'{len(rows)} controls: {Counter(r[1] for r in rows).most_common()}')


if __name__ == '__main__':
    main()


# --- enum combo labels ---------------------------------------------------
# (invoked separately from main(); kept in this file so extraction stays in
# one place). Emits GeneratedEnumLabels.inc: ordered display labels per enum
# type used by ComboBoxValuedControl, from Resources.resx with fall-back to
# the C# enum value name. [RQ-GUI-032]
import xml.etree.ElementTree as _ET
CONSTANTS = 'Xplorer/Common/XpanderConstants.cs'
RESOURCES = 'Xplorer/Properties/Resources.resx'
LABELS_OUT = 'juce/app/core/src/GeneratedEnumLabels.inc'
COMBO_ENUMS = {
    'VCFFilterModes': 'EnumVCFFilterModes',
    'ModulationSources': 'EnumModulationSources',
    'ModulationSourcesModMatrix': 'EnumModulationSourcesModMatrix',
    'ModulationDestination': 'EnumModulationDestinations',
    'LFOWaveShapes': 'EnumLFOWaveShapes',
    'LFOTriggerSource': 'EnumLFOTriggerSources',
    'LFORetrigModes': 'EnumLFORetrigModes',
}


def enum_members(enum_name):
    src = open(CONSTANTS).read()
    m = re.search(r'enum ' + enum_name + r'\s*\{(.*?)\}', src, re.S)
    body = re.sub(r'//.*', '', m.group(1))
    members = []
    for tok in body.split(','):
        tok = tok.strip()
        if not tok:
            continue
        name = tok.split('=')[0].strip()
        members.append(name)
    return members


def extract_enum_labels():
    root = _ET.parse(RESOURCES).getroot()
    labels = {d.get('name'): (d.find('value').text or '')
              for d in root.findall('data') if d.find('value') is not None}
    with open(LABELS_OUT, 'w') as f:
        f.write('// Ordered ComboBox display labels per enum type, extracted from\n'
                '// Resources.resx (key "{EnumName}_{ValueName}") with fall-back to the\n'
                '// enum value name. Ordinal = declaration order. [RQ-GUI-032]\n')
        for prop, enum_name in sorted(COMBO_ENUMS.items()):
            members = enum_members(enum_name)
            f.write(f'    {{"{prop}", {{')
            f.write(', '.join(
                '"' + labels.get(f'{enum_name}_{name}', name).replace('"', '\\"') + '"'
                for name in members))
            f.write('}},\n')
    print(f'enum labels: {list(COMBO_ENUMS)}')


# --- VFD glyph sheet -------------------------------------------------------
# Emits juce/app/assets/vfd-matrix.png: the MATRIXTINY sprite sheet (96
# glyphs of 12x16, ASCII 32-126) out of the MidiApp.UIControls resources.
# The source is a 24-bpp bottom-up BMP; stock JUCE does not read BMP, so it
# is converted to PNG here (no external imaging dependency). [RQ-GUI-033,
# ADR-007]
import base64 as _b64
import struct as _struct
import zlib as _zlib
UICONTROLS_RESOURCES = 'MidiApp/MidiApp.UIControls/Properties/Resources.resx'
VFD_SHEET_OUT = 'juce/app/assets/vfd-matrix.png'


def extract_vfd_sheet():
    root = _ET.parse(UICONTROLS_RESOURCES).getroot()
    raw = None
    for d in root.findall('data'):
        if d.get('name') == 'MATRIXTINY':
            raw = _b64.b64decode(d.find('value').text)
    assert raw is not None and raw[:2] == b'BM', 'MATRIXTINY BMP not found'

    offset = _struct.unpack_from('<I', raw, 10)[0]
    width, height = _struct.unpack_from('<ii', raw, 18)
    bpp = _struct.unpack_from('<H', raw, 28)[0]
    assert bpp == 24, f'expected 24-bpp BMP, got {bpp}'
    row_size = (width * 3 + 3) & ~3  # 4-byte aligned BGR rows, bottom-up

    scanlines = bytearray()
    for y in range(height):
        src = offset + (height - 1 - y) * row_size
        scanlines.append(0)  # PNG filter: none
        for x in range(width):
            b, g, r = raw[src + x * 3:src + x * 3 + 3]
            scanlines += bytes((r, g, b))

    def png_chunk(tag, payload):
        return (_struct.pack('>I', len(payload)) + tag + payload
                + _struct.pack('>I', _zlib.crc32(tag + payload)))

    ihdr = _struct.pack('>IIBBBBB', width, height, 8, 2, 0, 0, 0)  # 8-bit RGB
    with open(VFD_SHEET_OUT, 'wb') as f:
        f.write(b'\x89PNG\r\n\x1a\n' + png_chunk(b'IHDR', ihdr)
                + png_chunk(b'IDAT', _zlib.compress(bytes(scanlines), 9))
                + png_chunk(b'IEND', b''))
    print(f'vfd sheet: {width}x{height} ({width // 12}x{height // 16} glyphs of 12x16)')


# --- friendly parameter display names ------------------------------------
# Emits GeneratedParameterNames.inc: parameter tag -> end-user display name,
# as the reference VfdDisplayHelper resolves via Resources.ResourceManager
# .GetString(parameterName). A parameter key is an all-caps identifier (no
# dot) with a single-line value; enum/message keys (mixed case) are excluded.
# [RQ-GUI-020]
NAMES_OUT = 'juce/app/core/src/GeneratedParameterNames.inc'


def extract_parameter_names():
    root = _ET.parse(RESOURCES).getroot()
    entries = []
    for d in root.findall('data'):
        name = d.get('name')
        value = d.find('value')
        text = value.text if value is not None else None
        if (name and text and re.fullmatch(r'[A-Z][A-Z0-9_]*', name)
                and '\n' not in text and text.strip()):
            entries.append((name, text))
    entries.sort()
    with open(NAMES_OUT, 'w') as f:
        f.write('// Parameter tag -> end-user display name, extracted from\n'
                '// Resources.resx. Mirrors the reference VfdDisplayHelper name\n'
                '// resolution. Regenerate with extract_control_table.py. [RQ-GUI-020]\n')
        for name, text in entries:
            f.write(f'    {{"{name}", "{text.replace(chr(92), chr(92) * 2).replace(chr(34), chr(92) + chr(34))}"}},\n')
    print(f'parameter names: {len(entries)}')


# --- MIDI Continuous-Controller names --------------------------------------
# Emits GeneratedControlChangeNames.inc: the reference ControlChangesNames.Names
# array (index = CC number, last entry = "None" / unassigned), used by the MIDI
# automation-table editor. [RQ-GUI-036, ADR-012]
CC_NAMES_SRC = ('Sanford.Multimedia.Midi/Source/Sanford.Multimedia.Midi/'
                'Messages/ControlChangesNames.cs')
CC_NAMES_OUT = 'juce/app/core/src/GeneratedControlChangeNames.inc'


def extract_control_change_names():
    src = open(CC_NAMES_SRC).read()
    body = re.search(r'string\[\]\s*Names\s*=\s*\{(.*?)\}', src, re.S).group(1)
    names = re.findall(r'"((?:[^"\\]|\\.)*)"', body)
    with open(CC_NAMES_OUT, 'w') as f:
        f.write('// MIDI Continuous-Controller names, extracted from the reference\n'
                '// Sanford ControlChangesNames.Names (index = CC number, last = "None").\n'
                '// Regenerate with extract_control_table.py. [RQ-GUI-036]\n')
        for name in names:
            f.write(f'    "{name}",\n')
    print(f'control-change names: {len(names)}')


if __name__ == '__main__':
    extract_enum_labels()
    extract_parameter_names()
    extract_vfd_sheet()
    extract_control_change_names()
