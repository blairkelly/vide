var app = module.parent.exports.app;
var sport = module.parent.exports.serialcoms;
var io = module.parent.exports.io;

var gt1 = 145;
var gt2 = 145;
var pst_status = false;
var pst_ctrl = 'auto';

var gt1_array = [];

var t0 = 0;
var t1 = 0;

var thermreads = 4;

var gt2_array = [];

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

    var jump = 1;
    socket.on('gt1down', function(data) {
        gt1-=jump;
    });
    socket.on('gt1up', function(data) {
        gt1+=jump;
    });
    socket.on('gt2down', function(data) {
        gt2-=jump;
    });
    socket.on('gt2up', function(data) {
        gt2+=jump;
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

        if (params.t0) {
            gt1_array.push(parseInt(params.t0));
            if (gt1_array.length > thermreads) {
                gt1_array.shift();
            }
            var gt1_total = 0;
            for (var i=0; i<gt1_array.length; i++) {
                gt1_total+=gt1_array[i];
            }
            t0 = parseInt(gt1_total/gt1_array.length);
            
        }
        if (params.t1) {
            gt2_array.push(parseInt(params.t1));
            if (gt2_array.length > thermreads) {
                gt2_array.shift();
            }
            var gt2_total = 0;
            for (var i=0; i<gt2_array.length; i++) {
                gt2_total+=gt2_array[i];
            }
            t1 = parseInt(gt2_total/gt2_array.length);
            //console.log(params.t1, t1);
        }

        if(pst_ctrl == 'auto') {
            if (t0 >= gt1) {
                if (pst_status) {
                    sport.write('p0\r');
                }
            }
            else if (t0 < gt1) {
                if( t1 >= (gt2+2) ) {
                    if (pst_status) {
                        sport.write('p0\r');
                    }
                }
                else if (t1 < (gt2-2)) {
                    if (!pst_status) {
                        sport.write('p1\r');
                    }
                }
            }
        }

        io.sockets.emit('info', {
            meatpi: params,
            gt1: gt1,
            gt2: gt2,
            t0: t0,
            t1: t1,
            pst_status: pst_status
        });
    });
});

app.get('/', function (req, res) {
	res.render('index/index.jade');
});