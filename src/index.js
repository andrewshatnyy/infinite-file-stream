/*eslint no-underscore-dangle: "allow"*/

const fs = require('fs');
const { Readable } = require('stream');

const BUFF_SIZE = 256;

class InfReader extends Readable {
  constructor(options) {
    if (!options.file) throw new Error('Missing file path in options: {file}');
    super(options);
    this.encoding = options.encoding || 'utf-8';
    this.fd = fs.openSync(options.file, 'r');
    this.position = 0;
    this.finish = this.finish.bind(this);
    this.onRead = this.onRead.bind(this);
    this._read = this._read.bind(this);
    this.readSource = this.readSource.bind(this);
    process.on('exit', this.finish);
    process.on('error', this.finish);
  }

  finish() {
    fs.close(this.fd);
  }
  // _read will be called when the stream wants to pull more data in
  // the advisory size argument is ignored in this case.
  onRead(err, bytes, readBuffer) {
    if (err) {
      return process.nextTick(() => this.emmit('error', err));
    }
    this.position += bytes;
    if (bytes > 0) {
      return this.push(readBuffer, this.encoding);
    }
    this.position = 0;
    return this.push(Buffer.from(' '), this.encoding);
  }

  readSource(size) {
    const readSize = size || BUFF_SIZE;
    fs.read(this.fd, Buffer.alloc(readSize), 0, readSize, this.position, this.onRead);
  }

  _read(size) {
    this.readSource(size);
  }
}

module.exports = InfReader;