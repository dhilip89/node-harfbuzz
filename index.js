/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2015 NAN contributors
 *
 * MIT License <https://github.com/rvagg/nan/blob/master/LICENSE.md>
 ********************************************************************/

var addon = require('./build/Release/addon'),
    util = require("util"),
    fs = require("fs");

module.exports = {
  createFont: function(font, size) {
    var f;
    if(Buffer.isBuffer(font)) {
      f = new addon.HarfBuzzFont(font, size);
      f[util.inspect.custom] = function(depth) { return `HarfBuzzFont { size: ${size} }`; }
      
    } else if(typeof font == "string") {
      var buffer = fs.readFileSync(font);
      f = new addon.HarfBuzzFont(buffer, size);
      Object.defineProperty(f, "file", {value: font, writeable: false });
      f[util.inspect.custom] = function(depth) { return `HarfBuzzFont { size: ${size}, file: ${util.inspect(font)} }`; }
      
    } else {
      throw "Font must be either a file name string or a buffer"
      
    }
    
    Object.defineProperty(f, "size", {value: size, writeable: false });
    
    return f;
  },
  
  shape: function(font, text) {
    return addon.shape(font, text);
  }
}
