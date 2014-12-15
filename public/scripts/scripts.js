var serverip = window.location.hostname;
var serverport = window.location.port;
var app_socket = io.connect('//'+serverip+':'+serverport);

app_socket.on('welcome', function(data) {
    console.log(data.message);
    console.log('Handshake address: ' + data.address);
});

app_socket.on('info', function(data) {
    if (data.meatpi) {

        if(data.meatpi.t0) {
            $('.thermistor_1').text(parseFloat(data.t0).toFixed(1));
            $('.thermistor_2').text(parseFloat(data.t1).toFixed(1));

            $('.goaltemp-1 span.display').text(data.gt1);
            $('.goaltemp-2 span.display').text(data.gt2);
        }

        if (data.pst_status) {
            $('.powerstatus span').removeClass('off').addClass('on').text("ON");
        } 
        else if (!data.pst_status) {
            $('.powerstatus span').removeClass('on').addClass('off').text("OFF");
        }
    }
});


$('.turnon').click(function() {
    app_socket.emit('pst_control', true);
});

$('.turnoff').click(function() {
    app_socket.emit('pst_control', false);
});

$('.goaltemp-1 span.ctrl.down').click(function () {
    app_socket.emit('gt1down', true);
});
$('.goaltemp-1 span.ctrl.up').click(function () {
    app_socket.emit('gt1up', true);
});
$('.goaltemp-2 span.ctrl.down').click(function () {
    app_socket.emit('gt2down', true);
});
$('.goaltemp-2 span.ctrl.up').click(function () {
    app_socket.emit('gt2up', true);
});
