/* global describe, it */

const assert = require('assert');
const fs = require('fs');
const FileStreamer = require('../src/index');

const file = './test/support/to_stream.txt';
const size = fs.statSync(file).size;

describe('FileStreamer', () => {
  it('streams file infinitely', (done) => {
    const stream = new FileStreamer({ file });
    let string = '';
    function onRead(buff) {
      string += buff;
      if (string.length < size) return;
      stream.removeListener('data', onRead);
      assert(true);
      done();
    }

    stream.on('data', onRead);
  });

  it('blows up if filename is missing', () => {
    assert.throws(() => new FileStreamer());
  });
});
