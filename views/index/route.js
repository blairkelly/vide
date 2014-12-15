var app = module.parent.exports.app;
var sport = module.parent.exports.serialcoms;
var io = module.parent.exports.io;

var gt1 = 145;
var gt2 = 145;
var pst_status = false;
var pst_ctrl = 'auto';

io.sockets.on('connection', function(socket) {
    console.log("Client: " + socket.handshake.headers.host + " @ " + (new Date()));

    socket.emit('welcome', { 
        message: 'Welcome to Vide',
        address: socket.handshake.headers.host
    });

    socket.on('pst_control', function(data) {
        if (data) {
            pst_ctrl = 'auto';
        }
        else {
            pst_ctrl = 'off';
            sport.write('p0\r');
        }
    });

    socket.on('gt1down', function(data) {
        gt1-=5;
    });
    socket.on('gt1up', function(data) {
        gt1+=5;
    });
    socket.on('gt2down', function(data) {
        gt2-=5;
    });
    socket.on('gt2up', function(data) {
        gt2+=5;
    });
});


sport.on("open", function () {
    var message = null;
    console.log('opened serial port');

    sport.write('p0\r');

    sport.on('data', function (data) {
        var pairs = data.split('&');
        var pieces = null;
        var params = {};
        for(var i = 0; i<pairs.length; i++) {
            pieces = pairs[i].split('=');
            params[pieces[0]] = pieces[1];
        }

        if (params.powerswitchtail) {
            pst_status = parseInt(params.powerswitchtail);
        }

        var t0 = parseInt(params.t0);
        var t1 = parseInt(params.t1);

        if(pst_ctrl == 'auto') {
            if (t0 > gt1) {
                sport.write('p0\r');
            }
            else if (t0 < gt1) {
                if( t1 > (gt2+3) ) {
                    sport.write('p0\r');
                }
                else if (t1 < (gt2-3)) {
                    sport.write('p1\r');
                }
            }
        }

        io.sockets.emit('info', {
            meatpi: params,
            gt1: gt1,
            gt2: gt2,
            pst_status: pst_status
        });
    });
});

app.get('/', function (req, res) {
	res.render('index/index.jade');
});