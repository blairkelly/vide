var app = module.parent.exports.app;
var sport = module.parent.exports.serialcoms;

sport.on('data', function (data) {
    
    var pairs = data.split('&');
    var pieces = null;
    var params = {};
    for(var i = 0; i<pairs.length; i++) {
        pieces = pairs[i].split('=');
        params[pieces[0]] = pieces[1];
    }

    console.log(params);
    
});

app.get('/', function (req, res) {
	res.render('index/index.jade');
});