# Xplorer

Xplorer is a real-time editor for the Oberheim Xpander and Matrix-12 synthesizers.

## Status

The current development effort is moving to a JUCE-based implementation.
The source code for this work is located in the [juce](juce) directory.

This JUCE port is still a work in progress.

## Project structure

- [juce](juce): active JUCE-based source tree
- [Xplorer](Xplorer): legacy .NET-based codebase

## Main features

The JUCE version aims to provide a full editor for Oberheim Xpander and Matrix-12 patches with bidirectional MIDI synchronization. In practice, this means editing single patches from a unified interface, managing patch navigation and storage, sending and receiving SysEx messages, and supporting real-time automation from both the synth and external controllers. The project also covers patch import/export workflows, all-data-dump backup and restore, modulation-matrix editing, copy/paste of page-family settings, and a UI designed to stay responsive while long operations run in the background.

## License

This project is licensed under the GPL v3.
See [LICENSE](LICENSE).


