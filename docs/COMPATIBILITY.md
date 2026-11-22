# Eye Scan - Compatibility Notice

## Compatibility Issues
- **Eye Scan** is designed to run on **Raspberry Pi with Debian 13 (Trixie) OS**. Without dedicated hardware components, application cannot run.
- Testing can be done on **Linux** or inside **Windows Subsystem for Linux (WSL)**, and only with actual hardware components mocked.
- **Linux** version must be no less than **Ubunut 24.04** or any other equivalent distribution **(Debian/Fedora/Arch)**.

### Fully Supported Platforms
- **Debian 13 (Trixie) Raspberry Pi OS**

### Fully Supported Platforms for Testing
- **Debian 13 (Trixie) Raspberry Pi OS**
- **Linux (Ubuntu/Debian)**
- **Windows (via WSL)**

### Not Supported
- **Native Windows (MSVC, MinGW)**
  - **Google Test** (`gtest`) requires manual setup on naitve Windows.
  - Profiling tools like `valgrind` are not available on Windows.
- **macOS**

## Installing WSL with Ubuntu
Open `powershell` as Administrator and run
```powershell
wsl --install -d Ubuntu
```
For Windows 10 users, ensure you have **WSL 2** installed:
```powershell
wsl --set-default-version 2
```

## Next Steps
For details on testing and validation methods used in this system, see [Testing & Validation](TESTING.md).
