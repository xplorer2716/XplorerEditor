# Test fixtures

| File | Content | Used by |
|---|---|---|
| `single_patch_oberheim.syx` | Real Xpander single-patch program dump (399 bytes, program 99, name "OBERHEIM") followed by a 7-byte page-select SysEx; copied from `Xplorer/oberheim.syx` | RQ-TST-002 round-trips, RQ-MOD-022/041/043 |

Expected byte vectors for generated SysEx (RQ-TST-003) are transcribed from the
C# reference sources (no .NET runtime in the build environment); each vector
cites the reference file/line it was derived from.
