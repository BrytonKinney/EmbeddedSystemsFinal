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
