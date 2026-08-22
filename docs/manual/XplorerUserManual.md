# Xplorer – A realtime editor for the Xpander / Matrix-12

![Xplorer UI](/docs/assets/README.XplorerFullScreen.jpg)

## Table of contents

[Release history](#release-history)

[Notice](#notice)

[License](#license)

[System requirements](#system-requirements)

&nbsp;&nbsp;&nbsp;&nbsp;[Computer](#computer)

&nbsp;&nbsp;&nbsp;&nbsp;[MIDI interface](#midi-interface)

&nbsp;&nbsp;&nbsp;&nbsp;[Synthesizer](#synthesizer)

[Introduction](#introduction)

[Installation](#installation)

[Overview](#overview)

&nbsp;&nbsp;&nbsp;&nbsp;[Menus](#menus)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[File](#file)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Patch](#patch)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Tools](#tools)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Help](#help)

&nbsp;&nbsp;&nbsp;&nbsp;[The Quick Buttons](#the-quick-buttons)

[Settings](#settings)

&nbsp;&nbsp;&nbsp;&nbsp;[MIDI page](#midi-page)

&nbsp;&nbsp;&nbsp;&nbsp;[MIDI Ports](#midi-ports)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Output port to synthesizer](#output-port-to-synthesizer)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Input port from synthesizer](#input-port-from-synthesizer)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Controller or DAW input port](#controller-or-daw-input-port)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Connecting a hardware controller to Xplorer](#connecting-a-hardware-controller-to-xplorer)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Connecting a DAW to Xplorer](#connecting-a-daw-to-xplorer)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Using a hardware controller and a DAW](#using-a-hardware-controller-and-a-daw)

&nbsp;&nbsp;&nbsp;&nbsp;[MIDI communication](#midi-communication)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Output MIDI channel](#output-midi-channel)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Smart “all notes off”](#smart-all-notes-off)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[SysEx Delay](#sysex-delay)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Default patch number](#default-patch-number)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Synthesizer type](#synthesizer-type)

&nbsp;&nbsp;&nbsp;&nbsp;[MIDI Automation](#midi-automation)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Understanding Xpander/Matrix-12 parameter pages](#understanding-xpandermatrix-12-parameter-pages)

&nbsp;&nbsp;&nbsp;&nbsp;[User interface page](#user-interface-page)

&nbsp;&nbsp;&nbsp;&nbsp;[Randomizer page](#randomizer-page)



# Release history

|  |  |  |
| --- | --- | --- |
| Version | Release date | Change log |
| 2026.08.XX-YYZZ | August XX, 2026 | JUCE port |
| 1.5.8.1 | May 8, 2012 | First prod release |
| 1.4.15.1 | April 15, 2012 | First beta release |

# Notice

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

You agree that any and all claims, suits or other disputes arising from your use of the software shall be determined in accordance with the laws of France, in the event the author is made a party thereto.

# License

This software is released under **GNU Affero General Public License v3.0**

https://github.com/xplorer2716/XplorerEditor/blob/dev/LICENSE

Permissions of this strongest copyleft license are conditioned on making available complete source code of licensed works and modifications, which include larger works using a licensed work, under the same license. Copyright and license notices must be preserved. Contributors provide an express grant of patent rights. When a modified version is used to provide a service over a network, the complete source code of the modified version must be made available.

# System requirements

Xplorer is a native application based on the JUCE framework.

Supported platforms

| Operating system | Architecture | What you download |
| --- | --- | --- |
| Windows | 64-bit (x64) | Executable |
| Linux | 64-bit (x64) | AppImage |
| macOS | Apple Silicon (arm64) | Application bundle (.app) |

**Note on macOS:** the macOS build is currently considered **experimental**, because it has not yet been tested on real hardware. Please report anything that does not work as expected on the project's GitHub page.

## Computer

* Processor: any 64-bit processor from the last ten years is largely sufficient
* Display: Xplorer draws its interface with vector graphics, so it stays sharp on high-resolution and scaled displays. You can choose the window size from the View menu. 1920 x 1080 or higher is recommended.

## MIDI interface

Minimum:
* A MIDI interface with at least one MIDI OUT port

Recommended:
* A MIDI interface with one MIDI OUT and one MIDI IN, plus a virtual MIDI cable so you can also use Xplorer with a DAW:
  * Windows: LoopBe1 (<http://www.nerds.de/en/loopbe1.html>) or loopMIDI (<https://www.tobias-erichsen.de/software/loopmidi.html>)
  * macOS: no extra software needed - use the IAC Driver built into Audio MIDI Setup
  * Linux: no extra software needed - use the ALSA virtual MIDI ports

## Synthesizer

* Oberheim Xpander or Matrix-12 synthesizer

# Introduction

Xplorer is a real-time and bi-directional software editor for the Oberheim Matrix-12 and Xpander synthesizers.

“Real-time and bi-directional” means that every change you make in Xplorer is reflected into the synthesizer memory with minimal latency, and vice-versa. Whether you change the sound parameters on the synthesizer, in Xplorer, or with a MIDI controller, the software and the synthesizer will simultaneously update the parameters of the sound, without needing to reload the patch into the synthesizer memory.

The main features of Xplorer are:

* Single patch tone file load/save on disk
* Modification of the 226 parameters of a single patch
* Real-time MIDI automation for all parameters with freely assignable control changes, even for filter modes (except Modulation Matrix)
* Copy/paste page for TRACK, ENV, LFO, and RAMP pages (e.g., you can copy all ENV1 page parameters and paste them to ENV3)
* Rename patch, go to patch, store patch, save patch to disk
* Real-time update of all parameters when the user tweaks the synthesizer knobs (dual editing can be done without needing to reload the patch and resynchronize the synthesizer and the PC as most editors need to do)
* Patch randomizer (generates a new patch from random values)
* An on-screen piano keyboard to play the patch you are editing
* A resizable, vector-drawn interface that stays sharp at any size, with colours you can set yourself
* …and more to come in the next releases.


# Installation

1. Download latest release from https://github.com/xplorer2716/XplorerEditor/releases/latest
2. Unzip the archive
3. launch the executable/app/AppImage depending on your OS.

These binaries are not signed. Xplorer is an AGPL project and carries no paid publisher
certificate, so each operating system will ask you to confirm - once.

**Windows** - SmartScreen shows "Windows protected your PC".
Click More info, then Run anyway.

**macOS** - the app is blocked with "Apple could not verify it is free of malware".
Open System Settings → Privacy & Security, scroll to the bottom, and click Open Anyway.
Since macOS 15 the old Control-click → Open shortcut no longer works; the Settings route is the
only one.

**Linux** - make the AppImage executable, then run it:
```
chmod +x Xplorer-*.AppImage && ./Xplorer-*.AppImage
```

If nothing happens, your distribution does not ship FUSE 2:
```
./Xplorer-*.AppImage --appimage-extract-and-run
```


# Overview

![Xplorer UI](/docs/assets/README.XplorerFullScreen.jpg)

Xplorer is not just another generic synthesizer editor with ranges of slider controls that are all the same and don’t really tell you what you are doing.

Xplorer was developed by an Xpander owner for use by Xpander and Matrix-12 owners. With Xplorer you quickly get an overview of all parameters of a single patch, including an overview of the Modulation Matrix. Each functional block of the signal path - VCO, VCF, ENV, LFO, RAMP, LAG, TRACK and the Modulation Matrix - has its own colour, so you can find your way around the panel at a glance.

With Xplorer you can tweak the sound simultaneously using a computer mouse, a hardware MIDI controller, your DAW software, or directly on your Xpander or Matrix-12.

Use the method you prefer to change the sound. Xplorer’s display shows you in real time which parameter is currently being modified.

## Menus

Most menu commands have a keyboard shortcut, so you can keep your hands off the mouse whenever possible.

### File

These commands are straightforward. With Open and Save you can manage your single-patch SysEx files on your hard disk.

Please note that you can simply drag and drop a SysEx file onto Xplorer. If the SysEx file contains a single patch, it will be loaded directly. If the SysEx file is a bank of patches, Xplorer will ask you to confirm the import of the whole bank in the synth.

The “New” (Ctrl-N) command is a bit different. It will load the default “Oberheim“ patch into the current memory location of your synthesizer, as described in the synthesizer owner’s manual. In short, Ctrl-N in Xplorer will give you the same result as using Clear+Store on the synthesizer.

### Patch

* Previous (F5): same as using "-" on the synthesizer

* Next (F6): same as using "+" on the synthesizer

* Go to patch (F7): Opens a dialog asking for a patch number, and switches to that patch when you press Enter.

* Randomize (F8): Randomizes all the parameters of the patch to create a new one, named “Random”. The settings/randomizer dialog enable to define which parameters are impacted.

* Rename (F9): Opens a dialog to set the patch name. Please keep in mind that this is the *patch* name, not the *file* name on your computer.

* Store (F10): Stores the current patch *into the synthesizer memory* at the current memory location. Please keep in mind that this command is not the same as saving the SysEx file on your hard disk.

* Synchronize (F12): In case of communication errors between the synthesizer and Xplorer (such as an unplugged MIDI cable), you can use this command to get the current synthesizer patch into Xplorer. Any changes made in Xplorer will be overwritten by the values of the synthesizer’s current patch.

### Tools


* Settings (Ctrl-G): Opens the Settings window (see next chapter).

* Tune request (F4): Sends a tune request to the synthesizer.

* Piano keyboard: Opens a small on-screen keyboard that plays your synthesizer, so you can hear the patch you are editing without reaching for a MIDI keyboard. You can play it in two ways: with the mouse, by clicking on the keys, or with your computer keyboard - each playable key shows, printed on it, the character that plays it. Those characters are worked out from your own keyboard layout, so the keys shown are the ones actually under your fingers, whether you use a QWERTY, AZERTY or any other layout. The notes are sent to your synthesizer on the output MIDI channel set in the Settings window, so the MIDI output port must be configured for the keyboard to make any sound.

* Single patches:
  - Get all single patches from synth: Gets all single patches from the synthesizer and stores them in a folder of your choice
  - Extract all single patches from file: Extracts all single patches from a SysEx file (an “all data dump”) and stores them in a folder of your choice.

* Backup/Restore:
  - Backup all data: Saves all synthesizer data (single and multi patches) into a SysEx file on disk.
  - Restore all data: Restores a previously saved backup file into the synthesizer’s memory.

### Help

* Xplorer help (F1): Opens this manual in your web browser.

* Check for new releases and Go to website open the corresponding page in your web browser.

* About: Shows the “About” window. When it opens, Xplorer also sends a message to the synthesizer’s display - a quick way to check that MIDI communication towards the synthesizer is working.

![The Quick Buttons](/docs/assets/xplorerabout.jpg)


## The Quick Buttons

![The Quick Buttons](/docs/assets/quickbuttons.jpg)

The quick buttons sit under the display. They give you direct access to the most-used menu commands, without opening a menu: previous and next patch, randomizer, save patch as file, store patch, midi settings.

Also please note that two shortcuts are worth knowing:

* double-click a knob to type an exact value instead of dialling it;
* double-click the display to rename the current patch.

# Settings

The Settings window is available from Xplorer’s Tools menu, or with the Ctrl-G keyboard shortcut.

## MIDI page

![settings/midi](/docs/assets/settings.midi.jpg)

The first thing is to set up your synthesizer MIDI configuration to enable communication with Xplorer. Please check your synthesizer's manual:

**Enable MIDI system exclusive messages**: go to MASTER PAGE/MIDI/ENABLES and select SYSTEMX.

**Disable MIDI echo**: go to MASTER PAGE/MIDI/ENABLES and unselect ECHO.

**Enable Patch change**: go to MASTER PAGE/MIDI/ENABLES and select PATCH.

**Set up MIDI channel**: see below.

**Set up Memory Protection**: see the switch at the back of your synth.

## MIDI Ports

Depending on your MIDI setup and how you want to use Xplorer, you can define the settings in different ways.

### Output port to synthesizer

This is the OUTPUT port of your MIDI interface, which should be connected to the MIDI IN port of your synthesizer.

### Input port from synthesizer

This is the INPUT port of your MIDI interface, which should be connected to the MIDI OUT port of your synthesizer. Please note that the INPUT port and OUTPUT port configuration in Xplorer do not need to be on the same MIDI interface.

### Controller or DAW input port

This port is the port of your MIDI Keyboard or the MIDI Controller you want to use to control your synthesizer. If you are using a DAW MIDI output as the input port, this is the port of a virtual MIDI cable that connects the DAW to Xplorer. Both configurations are described in detail below.

### Connecting a hardware controller to Xplorer

The figure below describes how to connect an external hardware MIDI controller to Xplorer. The MIDI interfaces in the picture below could be external MIDI interfaces, sound card MIDI interfaces, or a mix of both.

### Connecting a DAW to Xplorer

To use Xplorer in conjunction with a DAW (Digital Audio Workstation) you need a virtual MIDI cable.

A **virtual MIDI cable** is software that emulates a MIDI interface. It uses one or more virtual MIDI ports to allow communication of MIDI data between applications. One application is sending MIDI data to the virtual MIDI output port, and the other one is listening to the virtual MIDI input port.

You may ask: why is a virtual MIDI cable needed? It’s because Xplorer is not based on proprietary technologies like VST. This guarantees that it will be still usable and maintainable years from now.

On macOS and Linux you already have one: use the IAC Driver in Audio MIDI Setup on macOS, or the ALSA virtual MIDI ports on Linux. On Windows you need to install one - LoopBe1 (<http://www.nerds.de/en/loopbe1.html>) and loopMIDI (<https://www.tobias-erichsen.de/software/loopmidi.html>) are both free.

Once the virtual MIDI cable software is installed (please follow the installation guidelines), you should see the virtual MIDI ports in your DAW. As an example, here's how loopMIDI can be used in a MIDI track in Cubase.

![virtual midi cable](/docs/assets/virtualmidicable.jpg)

**Using the virtual MIDI cable in Xplorer**

Select as “Controller or DAW input port” the same port you use in your DAW. Following the previous example, we use here loopMidi. Now the connection between Cubase and Xplorer is functionnal.

![virtual midi cable](/docs/assets/virtualmidicablexplorer.jpg)

You’re done!

If the other ports of Xplorer are configured (MIDI Out to synthesizer and MIDI IN from synthesizer), you should hear your synthesizer by playing the MIDI OUT keyboard into your DAW.

### Using a hardware controller and a DAW

The diagram below summarizes the whole process.

![virtual midi cable](/docs/assets/virtualmidicablefullsetup.jpg)


## MIDI communication

### Output MIDI channel

This is the channel number used on the output MIDI port to communicate with your synthesizer. You must ensure that the channel you set in Xplorer is the same as the channel set on your synthesizer. Go to MASTER PAGE/MIDI/CHANNEL to set this up on your synthesizer.

### Smart “all notes off”

When this option is activated, an “all notes off” MIDI message is sent to the synthesizer just before a patch change, to avoid notes that stay stuck on.

### SysEx Delay

This is the delay in milliseconds used between each SysEx message sent to the synthesizer. Keep the default value (30 ms) as long as communication between Xplorer and the synthesizer works fine. If it does not, try increasing it.
On the Xpander, the default 30 ms should be fine. On the Matrix-12, you may need to increase it, up to 100 ms. Values below 10 ms may hang the synthesizer, because it then receives data faster than it can handle.

### Default patch number

This is the patch number the editor uses when you start Xplorer or generate a random patch. A good practice is to set it to 99 and keep patch 99 as your “scratch” patch, so you never overwrite a patch you wanted to keep.

### Synthesizer type

This is the type of synthesizer you have: an Xpander or a Matrix-12. Please set it up accordingly.

## MIDI Automation

This is the table where you can set up MIDI control change messages to automate the parameters (e.g., automate VCF Freq with CC #1, VCF Resonance with CC #2, and so on). To remove a control change assignment, select “None” at the end of the list (you can press “N” on your keyboard to quickly set this value).

The same control change number can be used to automate more than one parameter at a time, although this is not recommended for parameters that are on different pages or subpages (see below, “Understanding Xpander/Matrix-12 parameter pages”).

Please keep in mind that MIDI control change messages are received by the Controller/DAW Input port. If this port is not defined, the MIDI automation feature will not work.

The table can be exported as an HTML file, so you can keep a record of your assignments for live performances.

### Understanding Xpander/Matrix-12 parameter pages

Xpander and Matrix-12 synthesizer parameters are organized into pages and subpages. On the synthesizer itself, the pages are changed by pressing the PATCH PAGE SELECT buttons and the sub-pages with the PAGE 2 buttons.

When Xplorer needs to change the value of a parameter, it has to change the current page and subpage depending on the parameter, whether the parameter is changed with the mouse on the user interface of Xplorer or by MIDI automation.

An example: let’s say we are changing the VCF Freq value, and then we want to change the ENV1 Delay value. Xplorer must first change the page/subpage to page ENV1, subpage 1, because VCF Freq is not on the same page/subpage as ENV1 Delay.

Page and subpage changes are handled slowly by the synthesizer, and there is nothing Xplorer can do to speed them up.

If the synthesizer receives a lot of page/subpage messages in a short amount of time, it may stop handling some of them. This can mainly happen when two parameters in different page/subpages are automated with CC messages and a lot of those messages are received in a short amount of time by Xplorer, resulting in a lot of page/subpage messages sent to the synthesizer. So, again, it’s not recommended to use the same CC to change parameters that appear in different pages/subpages.

## User interface page

![settings/user interface](/docs/assets/settings.ui.jpg)

**Colours**

Knob LED colour: The colour used by all knobs, tick boxes and radio buttons.

Block colours: Each of the eight functional blocks - VCO, LAG, TRACK, VCF, ENV, LFO, RAMP and MODULATION MATRIX - has its own colour, and you can set each one separately. Changing a block’s colour recolours everything that belongs to it: its frame, its background, its section heading and its page selector buttons.

The colours are what let you read the panel at a glance, so being able to retune them matters: screens, lighting and eyesight all differ, and a set of colours that works well for one person may not for another.

Every colour change is previewed **live**: while the colour picker is open, the panel behind it updates immediately, so you can judge the result on the real interface before deciding. Nothing is committed until you click OK - clicking Cancel restores the colours that were in use before you opened the window.

Reset to defaults: Restores all eight block colours **and** the knob LED colour to the original values, with the same live preview.

## Randomizer page

![settings/randomizer](/docs/assets/settings.randomizer.jpg)

This page decides what the Randomize command (F8, or the dice quick button) is allowed to change. On the Xpander and the Matrix-12, a completely random patch is often silent or unusable, so these settings let you keep a few key elements under control while everything else is drawn at random.

Each setting can be left on **Free**, which means “randomize this too, with no constraint”.

**VCO frequency**

Sets the musical interval between the two oscillators instead of leaving their pitches to chance:

* Free: both pitches are random.
* Same note: both oscillators are tuned to the same note.
* Third, Fifth, Seventh, Ninth, Eleventh, Thirteenth: VCO2 is tuned that interval above VCO1.
* Octave: VCO2 is tuned one octave above VCO1.

**VCO detune**

Sets how far the two oscillators are detuned from each other:

* Free: the detune amount is random.
* Digital: a very slight detune, for a clean, precise sound.
* Analog: a wider detune, for the thicker, drifting sound of an analogue oscillator pair.

**VCA2 envelope**

Gives the patch a usable volume shape, which is what most often makes the difference between a random patch you can play and one you cannot hear:

* Free: the envelope is random.
* Organ: instant attack, full sustain, instant release - the note lasts exactly as long as you hold the key.
* Strings: slow attack and slow release.
* Percusive: instant attack then a short decay, with no release.
* Percusive with release: the same, but the sound rings on after you let go of the key.

**VCO2 random**

Three tick boxes - FM, Noise and Sync - deciding whether those features of VCO2 may be switched on at random. Clear them if you find the results too aggressive.

**Matrix random**

Three tick boxes deciding how much of the Modulation Matrix is randomized:

* Amount: the modulation amounts.
* Quantize: the quantize flag of each entry.
* source & dest.: the modulation sources and destinations themselves.

Clearing “source & dest.” is a good way to keep a modulation routing you like while still shaking up the rest of the patch.

**Randomize all**

Sets every option on this page back to “randomize everything, with no constraint”. Note that this button changes the settings on the page - it does not generate a patch. Use Randomize (F8) in the Patch menu for that.


# Website

Go to the website https://xplorer2716.github.io/XplorerEditor.site/ for news, resources and more.
