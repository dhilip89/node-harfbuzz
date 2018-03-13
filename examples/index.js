var fs = require('fs');
var hb = require('../index.js');

var font = hb.createFont(fs.readFileSync(process.argv[2]), 64);
var glyphs = hb.shape(font, process.argv[3]);

console.log(glyphs);