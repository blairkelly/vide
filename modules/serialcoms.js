var config = require('../config');
var serialcoms = {};

console.log("Serial address: " + config.serialaddress);

var serialport = require("serialport"),     // include the serialport library
    SerialPort = serialport.SerialPort,      // make a local instance of serial
    serialData = {};                    // object to hold what goes out to the client

var the_serialport;
var open_serialport = function () {
    the_serialport = new SerialPort(config.serialaddress, { 
        baudrate: 57600,
        // look for return and newline at the end of each data packet:
        parser: serialport.parsers.readline("\r\n") 
    });
}
setTimeout(function () {
    //give the raspberry pi time to fully populate /dev
    open_serialport();
}, 27000);

//needed to export
module.exports = the_serialport;