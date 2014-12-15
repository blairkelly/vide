//twenny

var path = require('path');
var find = require('find');  //was specified for route, but not in registry.
var http = require('http');
var express = require('express');
var cookieParser = require('cookie-parser')
var session = require('express-session')
var serialcoms = require('./modules/serialcoms');

var app = express();           // start Express framework

var main_middleware = function main_middleware (req, res, next) {
    next();
}

app.set('views', path.join(__dirname + '/views/'));
app.set('view engine', 'jade');
app.enable('trust proxy');
app.use(cookieParser());
app.use(session({ secret: 'jasmine top' }));
app.use(main_middleware);
app.use(express.static(path.join(__dirname + '/public')));


var server = http.createServer(app); // start an HTTP server
var io = require('socket.io')(server);
server.listen(process.env.PORT || 5700);

module.exports = {
    app: app,
    serialcoms: serialcoms,
    io: io,
};

//routes
require('./routes/style');
find.fileSync('route.js', __dirname + '/views').forEach(function (route_file) {  //-     /\.js$/
    require(route_file);
});