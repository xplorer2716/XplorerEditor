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

    rows = []
    for ctrl, g in sorted(geo.items()):
        if ctrl.startswith('$'):
            continue
        kind = types.get(ctrl, '?').split('.')[-1]
        if any(s in kind for s in SKIPPED_KINDS):
            continue
        (x, y), (w, h) = g.get('loc', (0, 0)), g.get('size', (0, 0))
        rows.append((ctrl, kind, x, y, w, h, tags.get(ctrl, '')))

    with open(TABLE_OUT, 'w') as f:
        f.write('// Mechanically extracted from Xplorer/View/MainForm.resx (geometry)\n'
                '// and MainForm.Designer.cs (types, tags). Do not edit by hand;\n'
                '// regenerate with juce/app/core/tools/extract_control_table.py.\n'
                '// Logical canvas coordinates. [RQ-GUI-001]\n')
        for ctrl, kind, x, y, w, h, tag in rows:
            f.write(f'    {{"{ctrl}", ControlKind::{kind}, {x}, {y}, {w}, {h}, "{tag}"}},\n')
    print(f'{len(rows)} controls: {Counter(r[1] for r in rows).most_common()}')


if __name__ == '__main__':
    main()
