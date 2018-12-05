# Embedded Systems Final
Positionining & Environmental Data Collector


To build and run, ensure you have the following dependencies:

1) POCO
2) Node.js
3) NPM


`
sudo apt-get install libpoco-dev npm nodejs -y
`


Then, run:
`npm install`


To build:
`g++ f.cpp BMP280.cpp NEO6M.cpp LSM303.cpp WS.cpp -lPocoNet`


## [NEO6M.cpp]
This file parses data from the NEO-6M GPS Module.


## [LSM303.cpp]
This file parses the data via the I2C interface from the LSM303 accelerometer and magnetometer.


## [BMP280.cpp]
This file parses the data via I2C for the BMP280, the module providing barometric and temperature data.
