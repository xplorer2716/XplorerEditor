## First launch

These binaries are **not signed**. Xplorer is an AGPL project and carries no paid publisher
certificate, so each operating system will ask you to confirm — once.

**Windows** — SmartScreen shows *"Windows protected your PC"*.
Click **More info**, then **Run anyway**.

**macOS** — the app is blocked with *"Apple could not verify it is free of malware"*.
Open **System Settings → Privacy & Security**, scroll to the bottom, and click **Open Anyway**.
Since macOS 15 the old Control-click → Open shortcut no longer works; the Settings route is the
only one.

**Linux** — make the AppImage executable, then run it:

```
chmod +x Xplorer-*.AppImage && ./Xplorer-*.AppImage
```

If nothing happens, your distribution does not ship FUSE 2:

```
./Xplorer-*.AppImage --appimage-extract-and-run
```
