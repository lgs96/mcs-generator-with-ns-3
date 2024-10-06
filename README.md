# mmWave ns-3 module #

This is an [ns-3](https://www.nsnam.org "ns-3 Website") module for the simulation
of 5G cellular networks operating at mmWaves. A description of this module can be found in [this paper](https://ieeexplore.ieee.org/document/8344116/ "mmwave paper").

## How to Run and Process Logs (The objective of this repository)

To get MCS and SINR logs, follow these steps:

**Example Parameters**:
```cpp
double frequency0 = 3.5e9;
double simTime = 20;
double txPower = 10;
double ueNum = 10;
double ueSpeed = 9.7;  // Speed in meters per second (e.g., urban driving: 9.7m/s)
```
1. Set the parameters in scratch/mmwave-ca-same-bandwidth.cc.
2.  Run the simulation using this command:
```
. /ns3 run scratch/mmwave-ca-same-bandwidth.cc
```
3. After running the simulation, process the logs using this Python script:
```
python3 process_log.py
```
The output can be seen in the output_logs folder, where separate CSV files for MCS and SINR values will be generated.


## Installation
This repository contains a complete ns-3 installation with the addition of the mmwave module. 

Use these commands to download and build `ns3-mmwave`:
```
git clone https://github.com/nyuwireless-unipd/ns3-mmwave.git
cd ns3-mmwave
./ns3 configure --disable-python --enable-examples && ./ns3 build
```

## Usage example
You can use the following command to run the `mmwave-simple-epc` example. 
```
./ns3 run mmwave-simple-epc
```
Other examples are included in `src/mmwave/examples/`

## Documentation
The documentation of this module is available at [this link](./src/mmwave/doc/mmwave-doc.md).

## Related modules
- MilliCar is an ns-3 module for the simulation of mmWave NR V2X networks. Check [this repo](https://github.com/signetlabdei/millicar) for further details.
- A seperate module is being developed for [mmWave UE Energy Consumption](https://github.com/arghasen10/mmwave-energy "mmwave-energy"). You can use this module for analyzing 
Energy Consumption behaviour of mmwave UE. Check this repository for further details.
- `ns3-mmwave-iab` is an extended version of `ns3-mmWave` adding wireless relaying capabilities to an ns-3 NetDevice, and the possibility of simulating in-band relaying at mmWave frequencies. Check [this repo](https://github.com/signetlabdei/ns3-mmwave-iab) for further details.

## References 
The following papers describe in detail the features implemented in the mmWave
module:
- [End-to-End Simulation of 5G mmWave Networks](https://ieeexplore.ieee.org/document/8344116/ "comst paper") is a comprehensive tutorial with a detailed description of the whole module. We advise the researchers interested in this module to start reading from this paper;
- [Integration of Carrier Aggregation and Dual Connectivity for the ns-3 mmWave Module](https://arxiv.org/abs/1802.06706 "wns3 2018") describes the Carrier Aggregation implementation;
- [Implementation of A Spatial Channel Model for ns-3](https://arxiv.org/abs/2002.09341 "wns3 2020") describes the integration of the spatial channel model based on the 3GPP specifications TR 38.901 V15.0.0;
- [Performance Comparison of Dual Connectivity and Hard Handover for LTE-5G Tight Integration](https://arxiv.org/abs/1607.05425 "simutools paper") describes the Dual Connectivity feature.

These other papers describe features that were implemented in older releases: 
- [ns-3 Implementation of the 3GPP MIMO Channel Model for Frequency Spectrum above 6 GHz](https://dl.acm.org/citation.cfm?id=3067678 "wns3 2017") describes the implementation of the 3GPP channel model based on TR 38.900;
- [Multi-Sector and Multi-Panel Performance in 5G mmWave Cellular Networks](https://arxiv.org/abs/1808.04905 "globecom2018") describes the multi-sector addition to the 3GPP channel model;

If you use this module in your research, please cite:

M. Mezzavilla, M. Zhang, M. Polese, R. Ford, S. Dutta, S. Rangan, M. Zorzi, _"End-to-End Simulation of 5G mmWave Networks,"_ in IEEE Communications Surveys & Tutorials, vol. 20, no. 3, pp. 2237-2263, thirdquarter 2018. [bibtex available here](https://ieeexplore.ieee.org/document/8344116/)

## Future work
We are actively developing new features for the mmWave module, including:
- 3GPP NR beam tracking
- 3GPP NR Integrated Access and Backhaul feature (see [this repo](https://github.com/signetlabdei/ns3-mmwave-iab) for more details)

## About
This module is being developed by [NYU Wireless](http://wireless.engineering.nyu.edu/) and the [University of Padova](http://mmwave.dei.unipd.it/).
This  work  was  supported  in  part by  the  U.S.  Department  of  Commerce  National  Institute  of  Standards  and Technology through the Project “An End-to-End Research Platform for Public Safety  Communications  above  6  GHz”  under  Award  70NANB17H16.



<!-- The new-handover branch offers integration between LTE and mmWave and dual connectivity features.
 -->

## Authors ##

The ns-3 mmWave module is the result of the development effort carried out by different people. The main contributors are: 
- Tommaso Zugno, University of Padova
- Michele Polese, University of Padova
- Matteo Pagin, University of Padova
- Mattia Lecci, University of Padova
- Matteo Drago, University of Padova
- Mattia Rebato, University of Padova
- Menglei Zhang, NYU Wireless
- Marco Giordani, University of Padova
- Marco Mezzavilla, NYU Wireless
- Sourjya Dutta, NYU Wireless
- Russell Ford, NYU Wireless
- Gabriel Arrobo, Intel

## License ##

This software is licensed under the terms of the GNU GPLv2, as like as ns-3. See the LICENSE file for more details.
