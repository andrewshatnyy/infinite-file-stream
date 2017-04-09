var worker = require('streaming-worker');
var path = require('path');

//to create a stream you a path to addon
const stream = worker(path.join(__dirname, 'build/Release/infinite_file_stream'));

// path to file
const file_path = path.join(__dirname, "test/support/to_stream.txt");

//passing file_path to stream
stream.to.emit("name", file_path);

//using it
stream.from.on("value", function (value){
	console.log(value);
});