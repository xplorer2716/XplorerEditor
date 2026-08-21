Xplorer – A realtime editor for the Xpander / Matrix-12

![](data:image/jpeg;base64...)

**Table of contents**

[Release history 3](#__RefHeading___Toc338714074)

[Notice 4](#__RefHeading___Toc338714075)

[System requirements 4](#__RefHeading___Toc338714076)

[Introduction 5](#__RefHeading___Toc338714077)

[Installation 5](#__RefHeading___Toc338714078)

[Product activation 5](#__RefHeading___Toc338714079)

[Overview 6](#__RefHeading___Toc338714080)

[Using Xplorer on Touch Screens 7](#__RefHeading___Toc338714081)

[Recommended Touch Screen Gestures 7](#__RefHeading___Toc338714082)

[Predefined values for knobs 8](#__RefHeading___Toc338714083)

[Menus 8](#__RefHeading___Toc338714084)

[File 9](#__RefHeading___Toc338714085)

[Patch 9](#__RefHeading___Toc338714086)

[Tools 10](#__RefHeading___Toc338714087)

[Help 10](#__RefHeading___Toc338714088)

[The Quick Buttons 11](#__RefHeading___Toc338714089)

[Settings 11](#__RefHeading___Toc338714090)

[MIDI page 11](#__RefHeading___Toc338714091)

[MIDI Ports 12](#__RefHeading___Toc338714092)

[Output port to synthesizer 12](#__RefHeading___Toc338714093)

[Input port from synthesizer 12](#__RefHeading___Toc338714094)

[Controller or DAW input port 12](#__RefHeading___Toc338714095)

[Connecting a hardware controller to Xplorer 12](#__RefHeading___Toc338714096)

[Connecting a DAW to Xplorer 13](#__RefHeading___Toc338714097)

[Using a hardware controller and a DAW 15](#__RefHeading___Toc338714098)

[MIDI communication 16](#__RefHeading___Toc338714099)

[Output MIDI channel 16](#__RefHeading___Toc338714100)

[Smart “all not off” 16](#__RefHeading___Toc338714101)

[SysEx Delay 16](#__RefHeading___Toc338714102)

[Default patch number 16](#__RefHeading___Toc338714103)

[Synthesizer type 16](#__RefHeading___Toc338714104)

[MIDI Automation 16](#__RefHeading___Toc338714105)

[Understanding Xpander/Matrix-12 parameter pages 16](#__RefHeading___Toc338714106)

[User interface page 17](#__RefHeading___Toc338714107)

[Credits 17](#__RefHeading___Toc338714108)

[Appendix 18](#__RefHeading___Toc338714109)

[Oberheim Xpander LFO Rate to BPM 18](#__RefHeading___Toc338714110)

**Figures**

[Figure 1 - Activation of Xplorer 6](#__RefHeading___Toc338714111)

[Figure 2 – Xplorer user interface 6](#__RefHeading___Toc338714112)

[Figure 3 – Knob gesture (circular movement) 7](#__RefHeading___Toc338714113)

[Figure 4 – List of choice gesture 8](#__RefHeading___Toc338714114)

[Figure 5 – predefined values shortcuts 8](#__RefHeading___Toc338714115)

[Figure 6 – File menu 9](#__RefHeading___Toc338714116)

[Figure 7 – Patch menu 9](#__RefHeading___Toc338714117)

[Figure 8 – Setting menu 10](#__RefHeading___Toc338714118)

[Figure 9 – Help menu 10](#__RefHeading___Toc338714119)

[Figure 10 – “About” message on synthesizer’s display 11](#__RefHeading___Toc338714120)

[Figure 11 – Quick buttons 11](#__RefHeading___Toc338714121)

[Figure 12 – Settings window (MIDI Page) 11](#__RefHeading___Toc338714122)

[Figure 13 – Connecting a hardware controller to Xplorer 13](#__RefHeading___Toc338714123)

[Figure 14 – using a virtual MIDI cable in FL Studio (1/2) 14](#__RefHeading___Toc338714124)

[Figure 15 – using a virtual MIDI cable in FL Studio (2/2) 14](#__RefHeading___Toc338714125)

[Figure 16 – using a virtual MIDI cable in Cubase 14](#__RefHeading___Toc338714126)

[Figure 17 – using a virtual MIDI cable in Xplorer 15](#__RefHeading___Toc338714127)

[Figure 18 – using Xplorer with a DAW and a hardware controller 15](#__RefHeading___Toc338714128)

[Figure 19 – Settings window (User interface) 17](#__RefHeading___Toc338714129)

# Release history

|  |  |  |
| --- | --- | --- |
| Version | Release date | Change log |
| 1.10.24.1 | October 24, 2012 | * Windows 8 support. * Drag and drop support for single patch SysEx files. * Enhanced single patch data detection in SysEx files. * Extract single patches from an "all data data dump" SysEx file into a given folder. * Get all single patches from synthesizer into a given folder. * All data dump request backup and restore (save and restore synthesizer whole memory to/from a sysex file). * MIDI status is now displayed as 3 leds under the VFD Display. * “Smart all not off” mode to reduce stuck notes when changing patch. * Links to website added to help menu. |
| 1.8.12.3 | August 12, 2012 | * Tune request, linear knob movement, predefined knob values. |
| 1.5.8.1 | May 8, 2012 | First final release |
| 1.4.15.1 | April 15, 2012 | First beta release |

# Notice

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

You agree that any and all claims, suits or other disputes arising from your use of the software shall be determined in accordance with the laws of France, in the event the author is made a party thereto.

You agree to submit to the jurisdiction of the court in Strasbourg, France for all actions, whether in contract or in tort, arising from your use or purchase of the software.

# System requirements

The following are the system requirements for using Xplorer.

PC or compatible hardware /Mac BootCamp / Virtual machine (\*)

* Processor: 400 MHz Pentium processor or equivalent (Minimum); Core 2 Duo processor or equivalent (Recommended)
* RAM:96 MB (Minimum); 512 MB (Recommended)
* Hard Disk: Up to 500 MB of available space may be required
* CD or DVD Drive: Not required
* Display: 1280 x 900, 256 colors (Minimum); 1680 x 1050 high color, 32-bit (Recommended)

(\*) Virtual machines like[[1]](#footnote-2):

* Parallels Desktop (<http://www.parallels.com/products/desktop/>)
* Oracle VM Virtual Box ([https://www.virtualbox.org/](https://www.virtualbox.org/%29%20%20https%3A/www.virtualbox.org/%29%20%20)),
* VMWare Player (<http://www.vmware.com/products/player/overview.html>)

Windows®

Minimum:

* Windows XP SP3, Windows Vista, Windows 7 or Windows 8 with latest service pack, 32 or 64 bits (note: Xplorer does not run on Windows RT).
* Microsoft .NET Framework 4.0 Client Profile (may be already installed on your operating system)

MIDI Interface

Minimum:

* MIDI compatible interface with at least one MIDI OUT port

Recommended:

* MIDI interface with one MIDI OUT and one MIDI IN, one Virtual MIDI cable driver (like LoopBe1 - <http://www.nerds.de/en/loopbe1.html>, or MIDI Yoke - <http://www.midiox.com/>)

Synthesizer

* Oberheim[[2]](#footnote-3) Xpander or Matrix-12 synthesizer

# Introduction

Xplorer is a real-time and bi-directional software editor for the Oberheim Matrix-12 and Xpander synthesizers.

“Real-time and bi-directional” means that every change you make in Xplorer is reflected into the synthesizer memory with minimal latency[[3]](#footnote-4), and vice-versa. Whether you change the sound parameters on the synthesizer, in Xplorer, or with a MIDI controller, the software and the synthesizer will simultaneously update the parameters of the sound, without needing to reload the patch into the synthesizer memory.

The main features of Xplorer are:

* Single patch tone file load/save on disk
* Modification of the 226 parameters of a single patch
* Real-time MIDI automation for all parameters with freely assignable control changes, even for filter modes (except Modulation Matrix)
* Copy/paste page for TRACK, ENV, LFO, and RAMP pages (e.g., you can copy all ENV1 page parameters and paste them to ENV3)
* Rename patch, go to patch, store patch, save patch to disk
* Real-time update of all parameters when the user tweaks the synthesizer knobs (dual editing can be done without needing to reload the patch and resynchronize the synthesizer and the PC as most editors need to do)
* Patch randomizer (generates a new patch from random values)
* Auto-sized user interface depending on system’s default font size
* …and more to come in the next releases.

As a registered user, you get:

* Lifetime free updates for the Xplorer software
* Free access to the latest Xplorer source code starting 2018/01/01

# Installation

You should have received the download link by email. Extract the downloaded archive and launch the setup.exe file.

# Product activation

The first time Xplorer is started, it will ask you for the activation of the product. You should have received your license key file by email. Please select the location of the file and click OK. The application will restart. Please keep your license key file in a safe place for future reuse. It’s your personal license key file. Do not share it.

![](data:image/jpeg;base64...)

Figure 1 - Activation of Xplorer

# Overview

![](data:image/jpeg;base64...)

Figure 2 – Xplorer user interface

Xplorer is not just another generic synthesizer editor with ranges of slider controls that are all the same and don’t really tell you what you are doing.

Xplorer was developed by an Xpander owner for use by Xpander and Matrix-12 owners. With Xplorer you quickly get an overview of all parameters of a single patch, including an overview of the Modulation Matrix.

With Xplorer you can tweak the sound simultaneously using a computer mouse, a hardware MIDI controller, your DAW software, or directly on your Xpander or Matrix-12.

Use the method you prefer to change the sound. Xplorer’s display shows you in real-time which parameter is currently being modified.

## Using Xplorer on Touch Screens

The user interface of Xplorer was designed to be usable on touch screens. Most users of touch screens enlarge the system font size to have graphics more suited to this type of use.

The Xplorer user interface automatically changes based on the system font size. If you increase the system font size to 125%, Xplorer’s user interface will be 25% bigger. Larger controls are then easier to manipulate.

With a +25% font size, Xplorer user interface fits perfectly into a 1680\*1050 resolution, which is the native resolution of many common LCD screens.

### Recommended Touch Screen Gestures

To optimize the user experience, the following gestures are recommended:

Knobs

Depending on the choice made in the settings, the knobs can be tweaked using a linear movement or a circular movement.

Linear movement: This is the default movement (the mouse or the finger moves up or down to increase/decrease knob’s value).

Circular movement: This is similar to what you do when you turn the knob to the left or right.

![](data:image/x-wmf;base64...)

Figure 3 – Knob gesture (circular movement)

To use circular movement on touch screens, press the knob with your finger, turn in the direction of the current value of the knob, and then turn left or right according to your wishes, so you can see the knob while changing the value.

List of choice (combo boxes)

Forget about the right down arrow. Press the combo box with your finger and then release it from the screen to show the content of the list. Then move your finger up or down (depending on your choice) and release it to make a selection.

![](data:image/x-wmf;base64...)

Figure 4 – List of choice gesture

Check boxes and radio buttons

Press either on the control itself or its label.

## Predefined values for knobs

To speed up the tone programming, each knob with positive values (e.g.: from 0 to 63) has 10 predefined values. Each knob with negative values (detune, mod matrix amount...) has 11 predefined values (10 + 1 for zero).

Default value can be chosen directly when mouse is over the knob by pressing the "1" to "0" or "-" top keys on the keyboard ("1" for the minimum, "0" or "-" for the maximum).

![](data:image/jpeg;base64...)

Figure 5 – predefined values shortcuts

For VCO Freq knobs, default values are set to harmonic values (5th, 7th, one octave up, etc...). If you change the predefined value of a VCO Freq while playing a note or a chord, you’ll notice a lovely little glide. Hey, you’re playing with an analog synth!

## Menus

All menu items have defined shortcuts to minimize use of the mouse whenever possible.

### File

![](data:image/jpeg;base64...)

Figure 6 – File menu

These commands are straightforward. With Open, Save, and Save As, you can manage your single patch sysex files on your hard disk.

Please note that you can simply drag and drop a SysEx file onto Xplorer. If the SysEx file contains a single patch, it will be loaded directly.

The “New” (Ctrl-N) command is a bit different. It will load the default “Oberheim“ patch into the current memory location of your synthesizer, as described in the synthesizer owner’s manual.

In short, Ctrl-N in Xplorer will give you the same result as using Clear+Store on the synthesizer.

### Patch

![](data:image/jpeg;base64...)

Figure 7 – Patch menu

Previous (F5): same as using “-“ on the synthesizer

Next (F6): same as using “+“ on the synthesizer

Go to patch (F7): This command opens a dialog to get the patch number, and goes to this patch when Enter is pressed.

Randomize (F8): This command randomizes all the parameters of the patch to get a new patch. The new patch name will be called “Random”. Press the F8 key to get soundscape inspiration!

Rename (F9): Opens a dialog to set the patch name. Please keep in mind that this is the *patch* name, not the *file* name on your computer.

Store (F10): Stores the current patch into the synthesizer memory at the current memory location. Please keep in mind that this command is not the same as saving the sysex file on your hard disk.

Synchronize (F12): In case of communication errors between the synthesizer and Xplorer (such as an unplugged MIDI cable), you can use this command to get the current synthesizer patch into Xplorer. Changes made in Xplorer will be erased by the synthesizer current patch values.

### Tools

![](data:image/jpeg;base64...)

Figure 8 – Setting menu

Settings (Ctrl-G): Opens the Settings window (see next chapter).

Tune request (F4): Sends a tune request to the synthesizer.

Single patches:

Get all single patches from synth: Get all single patches from synth and store them to given folder[[4]](#footnote-5)

Extract all single patches from file: extract all single patches from a SysEx file (aka “all data dump”) and store them to a given folder.

Backup/Restore:

Backup all data: save all synth data (single and multi patches) into a SysEx file on disk.

Restore all data: restore a previously backup file into synthesizer’s memory.

### Help

![](data:image/jpeg;base64...)

Figure 9 – Help menu

Xplorer help (F1): Opens this manual.

Check for new releases, go to website, and go to forum: redirects your web browser to the corresponding website page.

About: Shows the infamous “About” window. Note: when the “About” window is shown, a message is sent on the display of the synthesizer. This can be a quick way to verify the MIDI output communication to the synthesizer.

![](data:image/jpeg;base64...)

Figure 10 – “About” message on synthesizer’s display

## The Quick Buttons

The quick buttons are located under the display. They give another way to access the main menu commands without going into the menu.

![](data:image/jpeg;base64...)

Figure 11 – Quick buttons

# Settings

The settings window is available thru Xplorer’s menu or the Ctrl-G keyboard shortcut.

## MIDI page

![](data:image/jpeg;base64...)

Figure 12 – Settings window (MIDI Page)

The first thing is to set up your synthesizer MIDI configuration to enable communication with Xplorer.

Enable MIDI system exclusive messages: go to MASTER PAGE/MIDI/ENABLES and select SYSTEMX.

Disable MIDI echo: go to MASTER PAGE/MIDI/ENABLES and unselect ECHO.

Enable Patch change: go to MASTER PAGE/MIDI/ENABLES and select PATCH.

Set up MIDI channel: see below “MIDI communication/Output MIDI channel”.

Set up Memory Protection: see the switch at the back of your synth.

## MIDI Ports

Depending on your MIDI setup and how you want to use Xplorer, you can define the settings in different ways.

### Output port to synthesizer

This is the OUTPUT port of your MIDI interface, which should be connected to the MIDI IN port of your synthesizer.

### Input port from synthesizer

This is the INPUT port of your MIDI interface, which should be connected to the MIDI OUT port of your synthesizer. Please note that the INPUT port and OUTPUT port configuration in Xplorer do not need to be on the same MIDI interface.

### Controller or DAW input port

This port is the port of your MIDI Keyboard or the MIDI Controller you want to use to control your synthesizer. In you’re using a DAW MIDI output as the INPUT port, this is the port of a MIDI Virtual cable that connects the DAW with Xplorer. Let’s see these configurations in detail.

### Connecting a hardware controller to Xplorer

The figure below describes how to connect an external hardware MIDI controller to Xplorer. The MIDI interfaces in the picture below could be external MIDI interfaces, sound card MIDI interfaces, or a mix of both.

![](data:image/x-wmf;base64...)

Figure 13 – Connecting a hardware controller to Xplorer

### Connecting a DAW to Xplorer

To use Xplorer in conjunction with a DAW (like Cubase[[5]](#footnote-6), Ableton Live, FL Studio, Reaper, etc…) you need a virtual MIDI cable.

A virtual MIDI cable is software that emulates a MIDI interface. It uses one or more virtual MIDI ports to allow communication of MIDI data between applications. One application is sending MIDI data to the virtual MIDI output port, and the other one is listening to the virtual MIDI input port.

You may ask: why is a virtual MIDI cable needed? It’s because Xplorer is not based on proprietary technologies like VST[[6]](#footnote-7). This guarantees that it will be still usable and maintainable years from now.

There are many virtual MIDI cable applications available. In the example below we will see how to use Xplorer with a DAW and MIDI Yoke, which is a free virtual MIDI cable (see <http://www.midiox.com/>). If you use Windows 7, you may prefer to use LoopBe1, which is free too (see <http://www.nerds.de/en/loopbe1.html>).

Once the virtual MIDI cable software is installed (please follow the installation guidelines), you should see the virtual MIDI ports in your DAW.

Using the virtual MIDI Cable in your DAW (FL Studio)

In this example, we use MIDI Yoke Port #1 with Xplorer, and in FL Studio we use Port #21. (This internal port number is specific and may vary depending on your setup).

![](data:image/jpeg;base64...)

Figure 14 – using a virtual MIDI cable in FL Studio (1/2)

In FL Studio, you can use this port as a standard MIDI OUT.

![](data:image/jpeg;base64...)

Figure 15 – using a virtual MIDI cable in FL Studio (2/2)

Using the virtual MIDI Cable in your DAW (Cubase)

Add a MIDI Track and set the output MIDI channel of the track to the corresponding MIDI Channel.

![](data:image/png;base64...)

Figure 16 – using a virtual MIDI cable in Cubase

Using the virtual MIDI Cable in Xplorer

Select as “Controller or DAW input port” the same port you use in your DAW. In this case, it is MIDI Yoke Port #1.

![](data:image/jpeg;base64...)

Figure 17 – using a virtual MIDI cable in Xplorer

You’re done!

If the other ports of Xplorer are configured (MIDI Out to synthesizer and MIDI IN from synthesizer), you should hear your synthesizer by playing the MIDI OUT keyboard into your DAW.

### Using a hardware controller and a DAW

The diagram below summarizes the whole process.

![](data:image/jpeg;base64...)

Figure 18 – using Xplorer with a DAW and a hardware controller

## MIDI communication

### Output MIDI channel

This is the channel number used on the output MIDI port to communicate with your synthesizer. You must ensure that the channel you set in Xplorer is the same as the channel set on your synthesizer. Go to Master Page/MIDI/Channel to set this up on your synthesizer

### Smart “all not off”

When this option is activated, an “all not off” MIDI message is sent to the synthesizer before a single patch change occurs to avoid stuck notes.

### SysEx Delay

This is the delay in milliseconds used between each SysEx message sent to the synthesizer. It is a good idea to use the default value (30 ms) if communication between Xplorer and the synthesizer is working fine. If not, try to increase this value. Values lower than 10 ms may hang up the synthesizer because it’s receiving data too quickly.

### Default patch number

This is the default patch number used by the editor when you start up Xplorer or when you generate a random patch. A good practice is to set it to 99, and to leave patch 99 as your “testing” patch. This will prevent you from accidentally overwriting patches that you may want to keep.

### Synthesizer type

This is the type of synthesizer you have: an Xpander or a Matrix-12. Please set it up accordingly.

## MIDI Automation

This is the table where you can set up MIDI control change messages to automate the parameters (e.g., automate VCF Freq with CC #1, VCF Resonance with CC #2, and so on). To remove a control change assignment, select “None” at the end of the list (you can press “N” on your keyboard to quickly set this value).

The same control change number can be used to automate more than one parameter at a time, although this not recommend for parameters that are on different pages or subpages (see below, “Understanding Xpander/Matrix-12 parameter pages”).

Please keep in mind that MIDI control change messages are received by the Controller/DAW Input port. If this port is not defined, the MIDI automation feature will not work.

Right-clicking on the table enables an HTML export of the content of the list, so you can keep track of your assignments and recreate them for other patches.

### Understanding Xpander/Matrix-12 parameter pages

Xpander and Matrix-12 synthesizer parameters are organized into pages and subpages. On the synthesizer itself, the pages are changed by pressing the PATCH PAGE SELECT buttons and the sub-pages with the PAGE 2 buttons.

When Xplorer needs to change the value of a parameter, it has to change the current page and subpage depending on the parameter, whether the parameter is changed with the mouse on the user interface of Xplorer or by MIDI automation.

An example: let’s say we are changing the VCF Freq value, and then we want to change the ENV1 Delay value. We must first change the page/subpage to page ENV1, subpage 1, since VCF Freq is not on the same page/sub page than ENV1 Delay).

Page and subpage changes are handled slowly by the synthesizer, and it not possible to do anything to improve that.

If a lot of page/subpage messages are received by the synthesizer in a short amount of time this can result in a loss of page/suppage message handling. This can mainly happen when two parameters in different page/subpages are automated with CC messages and a lot of those messages are received in a short amount of time by Xplorer, resulting in a lot of page/subpage messages sent to the synthesizer. So, again, it’s not recommended to use the same CC to change parameters that appear in different pages/subpages.

## User interface page

![](data:image/jpeg;base64...)

Figure 19 – Settings window (User interface)

Color: You can choose your own color for the user interface. This color is applied to all knobs and buttons.

Knob movement: you can choose linear or circular movement for the knobs.

# Credits

Wes Milholen: user’s manual and website design.

Sean Hancock, Eduardo Saponara, Doug Terrbonne, Tony Cappelini: beta testing.

Hielo Patagonia Sounds: macro photography.

# Appendix

## Oberheim Xpander LFO Rate to BPM[[7]](#footnote-8)

![](data:image/jpeg;base64...)

1. Oracle VM Virtual Box, VMWare Player, Parallels Desktop, Microsoft Windows XP, Vista, Windows 7 are all registered trademarks or trademarks of their respective owners [↑](#footnote-ref-2)
2. As described at <http://www.trademarkia.com/oberheim-77616105.html>, Oberheim is a trademark of Bank of America, National Association, Charlotte, NC 28255 [↑](#footnote-ref-3)
3. Depending on both hardware and software specifications of the synthesizer [↑](#footnote-ref-4)
4. In short, this command is the same as doing manually a backup into a file and extracting all single patches from this file. [↑](#footnote-ref-5)
5. Cubase, Ableton Live, FL Studio, Reaper are respectively trademarks of Steinberg Media Technologies GmbH, Ableton AG, Image Line and Cockos Incorporated. [↑](#footnote-ref-6)
6. VST is a trademarks of Steinberg Media Technologies GmbH [↑](#footnote-ref-7)
7. Experimental measures. May vary depending on your synthesizer. [↑](#footnote-ref-8)