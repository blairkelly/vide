var serialcoms = {}

var serialport = require("serialport"),     // include the serialport library
    SerialPort = serialport.SerialPort,      // make a local instance of serial
    serialData = {};                    // object to hold what goes out to the client

var the_serialport = new SerialPort("/dev/tty.usbmodem1411", { 
    baudrate: 57600,
    // look for return and newline at the end of each data packet:
    parser: serialport.parsers.readline("\r\n") 
});

the_serialport.on("open", function () {
    var message = null;
    console.log('opened serial port');
});

//needed to export
module.exports = the_serialport;