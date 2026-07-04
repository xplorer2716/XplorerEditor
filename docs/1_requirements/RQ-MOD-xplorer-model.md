# RQ-MOD — Xplorer Model (Tone, Parameters, SysEx I/O)

Scope: C++ port of `Xplorer/Model` + `Xplorer/Common` constants. All formats bit-exact versus the reference implementation and the Oberheim MIDI spec.

## Constants & structure

- **RQ-MOD-001** — The model shall define the Xpander structural constants identical to the reference: 100 single tones, 100 multi patches, 8-char tone name, 20 modulation entries, max 6 sources per modulation destination, 2 VCO, 5 LFO, 5 ENV, 3 tracks × 5 points, 4 ramps.
- **RQ-MOD-002** — The model shall define all reference enumerations (pages, modulation sources/destinations, filter modes, LFO wave shapes/trigger modes, envelope modes, ramp flags, trigger types, modulation-edit commands, rotary encoders) with values identical to the reference.
- **RQ-MOD-003** — The model shall define the page/sub-page mapping for every modulation destination, identical to the reference table.

## Parameters

- **RQ-MOD-010** — The model shall provide the Xpander parameter type carrying page, sub-page, parameter ID and generating the parameter-edit SysEx message and the page-select SysEx message with byte layouts identical to the reference.
- **RQ-MOD-011** — The model shall provide the signed-parameter variant encoding negative values exactly as the reference (`XpanderSignedParameter`).
- **RQ-MOD-012** — The model shall provide the modulation-matrix parameter variant generating modulation-edit SysEx identical to the reference (`XpanderModMatrixParameter`).
- **RQ-MOD-013** — The model shall provide the full-tone parameter variant used to transmit a complete single patch (`XpanderFullToneParameter`).

## Tone

- **RQ-MOD-020** — The Xpander tone shall own the complete parameter map of the reference implementation: same parameter names, order, bounds, steps, pages and default values (226 editable parameters).
- **RQ-MOD-021** — The tone shall expose editing program number and current program number bounded to 0–99.
- **RQ-MOD-022** — When serialized with `ToByteArray`/deserialized with `FromByteArray`, the tone shall produce/consume the 399-byte single-patch program dump (6-byte header) bit-exactly, such that deserialize(serialize(tone)) is identity and byte streams from real hardware round-trip unchanged.
- **RQ-MOD-023** — The tone shall extract the patch name from a single-patch dump byte array as the reference does (8 characters, Oberheim charset mapping).

## Modulation matrix

- **RQ-MOD-030** — The tone shall expose the 20-entry modulation matrix with per-entry source, amount (signed), quantize flag and destination, kept consistent with the underlying parameters.
- **RQ-MOD-031** — When changing a modulation source, amount, quantize or destination, the tone shall update the matching entry and produce the same parameter updates as the reference (including entry allocation via next-available-entry and source deletion semantics).
- **RQ-MOD-032** — The tone shall enforce the max-6-sources-per-destination rule and report available destinations/source availability for an entry, identically to the reference.
- **RQ-MOD-033** — When the modulation matrix is randomized (amount / quantize / source+destination independently enabled, optional humanize), the tone shall respect matrix consistency rules of the reference, including the forced ENV2→VCA2 option.

## Binary packing & file I/O

- **RQ-MOD-040** — The model shall implement the Oberheim nibble packetization (reader/writer) bit-exactly (`PacketizedBinaryReader`/`Writer`): packed two 4-bit nibbles per transmitted byte, low nibble first.
- **RQ-MOD-041** — The tone reader shall load a single-patch `.syx` file into a tone, and enumerate all single patches contained in an all-data-dump `.syx` file as (name, tone) pairs, accepting all file layouts the reference accepts.
- **RQ-MOD-042** — The tone writer shall write a tone as a single-patch `.syx` file byte-identical to the reference output for the same tone state.
- **RQ-MOD-043** — The model shall classify a `.syx` file as single patch, all-data dump, or unknown, using the same detection rules as the reference (`DetermineSysexFileType`).

## Compatibility

- **RQ-MOD-050** — Patch files produced by the reference .NET Xplorer shall load with identical resulting parameter values, and files produced by the C++ port shall load in the reference implementation unchanged.
