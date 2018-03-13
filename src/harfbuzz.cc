#include "hb.h"
#include "hb-ot.h"

#include <sstream>
#include <string>
#include <nan.h>

#include <iostream>

using v8::FunctionTemplate;
using v8::Handle;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Number;
using v8::Array;

class HarfBuzzFont : public Nan::ObjectWrap {
public:
  hb_font_t* _font;
  unsigned int _size;
  
  static NAN_MODULE_INIT(Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("HarfBuzzFont").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    
    constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
    Nan::Set(target, Nan::New("HarfBuzzFont").ToLocalChecked(),
      Nan::GetFunction(tpl).ToLocalChecked());
  }
    
  static NAN_METHOD(New) {
    if (info.IsConstructCall()) {
      char* fontFileBuffer = (char*) node::Buffer::Data(info[0]->ToObject());
      size_t fontFileBufferLength = node::Buffer::Length(info[0]->ToObject());
      unsigned int fontSize = info[1]->Uint32Value();  
      
      hb_blob_t* blob = hb_blob_create(fontFileBuffer, fontFileBufferLength, HB_MEMORY_MODE_READONLY, NULL, NULL);
      hb_face_t* face = hb_face_create(blob, 0);
      hb_font_t* font = hb_font_create(face);

      hb_ot_font_set_funcs(font);
      hb_font_set_scale(font, fontSize, fontSize);
      
      HarfBuzzFont *obj = new HarfBuzzFont(font, fontSize);
      obj->Wrap(info.This());
      info.GetReturnValue().Set(info.This());
      
    } else {
      const int argc = 2;
      v8::Local<v8::Value> argv[argc] = {info[0], info[1]};
      v8::Local<v8::Function> cons = Nan::New(constructor());
      info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }
  }
        
  static inline Nan::Persistent<v8::Function> & constructor() {
    static Nan::Persistent<v8::Function> my_constructor;
    return my_constructor;
  }
  
  explicit HarfBuzzFont(hb_font_t* font, unsigned int size) : _font(font), _size(size) {}
};

NAN_METHOD(Shape) {
  HarfBuzzFont* font = Nan::ObjectWrap::Unwrap<HarfBuzzFont>(info[0]->ToObject());
  String::Utf8Value input(info[1]->ToString());
  
  hb_buffer_t *buf = hb_buffer_create();

  hb_buffer_add_utf8(buf, *input, -1, 0, -1);
  hb_buffer_guess_segment_properties (buf);
  hb_shape(font->_font, buf, NULL, 0);

  unsigned int        glyph_count = hb_buffer_get_length (buf);
  hb_glyph_info_t     *glyph_info = hb_buffer_get_glyph_infos(buf, NULL);
  hb_glyph_position_t *glyph_pos  = hb_buffer_get_glyph_positions(buf, NULL);

  Local<Array> glyphs = Nan::New<Array>(glyph_count);
  for (size_t i = 0; i < glyph_count; i++) {
    Local<Object> g = Nan::New<Object>();
    
    hb_codepoint_t gid = glyph_info[i].codepoint;
    char glyphname[32];
    hb_font_get_glyph_name (font->_font, gid, glyphname, sizeof (glyphname));
    
    g->Set(Nan::New("name").ToLocalChecked(), Nan::New<String>(glyphname).ToLocalChecked());
    g->Set(Nan::New("xOffset").ToLocalChecked(), Nan::New<Number>(glyph_pos[i].x_offset));
    g->Set(Nan::New("yOffset").ToLocalChecked(), Nan::New<Number>(glyph_pos[i].y_offset));
    g->Set(Nan::New("xAdvance").ToLocalChecked(), Nan::New<Number>(glyph_pos[i].x_advance));
    g->Set(Nan::New("yAdvance").ToLocalChecked(), Nan::New<Number>(glyph_pos[i].y_advance));
    g->Set(Nan::New("codepoint").ToLocalChecked(), Nan::New<Number>(glyph_info[i].codepoint));
    g->Set(Nan::New("cluster").ToLocalChecked(), Nan::New<Number>(glyph_info[i].cluster));
    g->Set(Nan::New("mask").ToLocalChecked(), Nan::New<Number>(glyph_info[i].mask));

    glyphs->Set(i, g);
  }
  
  info.GetReturnValue().Set(glyphs);
}

void InitAll(Handle<Object> exports) {
  exports->Set(Nan::New<String>("shape").ToLocalChecked(), Nan::New<FunctionTemplate>(Shape)->GetFunction());
  HarfBuzzFont::Init(exports);
}

NODE_MODULE(harfbuzz, InitAll)
