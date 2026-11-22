# Eye Scan

## Description
**Eye Scan** is open-source iris recognition application written in modern `C++` and `OpenCV`,
designed primarily for development on **Raspberry Pi**-based hardware. The project provides a complete biometric identification pipeline, from iris localization and normalization to feature extraction and template matching.

The implementation is driven by the requirements of embedded hardware, with emphasis on performance, maintainability, and reliability while remaining suitable for research and educational purposes.

### Key Features
- **Raspberry Pi**-oriented system design
- Iris localization and segmentation
- **Log-Gabor** based iris feature encoding
- Binary template generation and matching
- Local-first database architecture with offline operation
- **Azure** cloud synchronization for distributed deployments

## Documentation
- [**System Architecture**](docs/ARCHITECTURE.md) – Structural breakdown of the system.
- [**Hardware Components**](docs/HARDWARE.md) – Details on the hardware setup and components used.
- [**Testing & Validation**](docs/TESTING.md) – Information on testing procedures and validation methods.

## Authors and Acknowledgments
### Author
- [**Aleksa Markovic**](https://github.com/Mrgi23) – Creator & Maintainer

### Acknowledgments
Special thanks to:
- [**School of Electrical Engineering, University of Belgrade**](https://www.etf.bg.ac.rs/) - For supporting development.
- The open-source community for valuable resources and support.

### References
- Ramy Zewail, Ahmed Seif, Nadder Hamdy, Magdy Saeb "Iris Identification Based on Log-Gabor Filtering", *2003 46th Midwest Symposium on Circuits and Systems*, Volume 1, pp. 333-336, 2003.
[Link to Paper](https://sci-hub.reatk.com/10.1109/mwscas.2003.1562286)

## License
This project is licensed under **Apache License 2.0**.

- **You are free to fork this repository and use, modify, and distribute the code in your own personal or commercial projects.**
- **You may NOT modify this repository directly.**
- **You must include the original copyright notice, a copy of the license, and state any significant changes made to the files.**
- **This software is provided "as is", without warranties or liability of any kind.**

For full legal terms, see the [LICENSE](LICENSE) file.

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)

## Contributing
This repository does not accept direct contributions. However, you are free to fork and modify the code for your own use.

If you encounter any issues, feel free to open an issue, but please understand that fixes and feature requests may not be actively addressed.

## Next Steps
For a detailed breakdown of the system’s architecture, see the [System Architecture](docs/ARCHITECTURE.md).
