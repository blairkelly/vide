var app = module.parent.exports.app;
var sass = require('node-sass');
var fs = require('fs');
var watch = require('node-watch');

var css_scss_dir_path = __dirname + '/../styles/';
var css_scss_file_path = __dirname + '/../styles/style.scss';
var compiled_css_filepath = 'public/styles/style.css';

var render_sass = function(callback) {
	sass.render({
    	file: css_scss_file_path,
    	success: function(css) {
    		callback(css);
    	}
  	});
}

var compile_and_save_css = function () {
	render_sass(function (css) {
		console.log("Done compile, saving...");
		fs.writeFile(compiled_css_filepath, css, function () {
			console.log("Finished compiling CSS file.");
		});
	});
}

compile_and_save_css();

watch(css_scss_dir_path, function (filename) {
	console.log("Recompiling CSS...");
	compile_and_save_css();
});

app.get('/styles/style.css', function (req, res) {
	res.sendfile(compiled_css_filepath);  	
});