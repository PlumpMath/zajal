#include "ruby.h"
#include "ofMain.h"
#include "zajal.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftmac.h"

/* intermediate class to expose internals of ofTrueTypeFont */
/* TODO should this just be a patch to OF? */
class ZajalTrueTypeFont : public ofTrueTypeFont {
public:
  int getFontSize();
  bool getContours();
};

int ZajalTrueTypeFont::getFontSize() {
  return this->fontSize;
}

bool ZajalTrueTypeFont::getContours() {
  return this->bMakeContours;
}

/* global typography module and font class */
VALUE zj_mTypography;
VALUE zj_cFont;

void zj_typography_reset_stacked_text() {
  INTERNAL_SET(zj_mTypography, stacked_text_x, INTERNAL_GET(zj_mTypography, stacked_text_initial_x));
  INTERNAL_SET(zj_mTypography, stacked_text_y, INTERNAL_GET(zj_mTypography, stacked_text_initial_y));
}

void zj_font_dealloc(void* font) {
  delete (ZajalTrueTypeFont*)font;
}

VALUE zj_font_new(int argc, VALUE* argv, VALUE klass) {
  ZajalTrueTypeFont* font_ptr = new ZajalTrueTypeFont();
  
  VALUE font = Data_Wrap_Struct(klass, 0, zj_font_dealloc, font_ptr);
  rb_obj_call_init(font, argc, argv);
  return font;
}

VALUE zj_font_load(VALUE self, VALUE filename, VALUE size) {
  INIT_DATA_PTR(ZajalTrueTypeFont, font_ptr);
  
  /*
  This is how loading system fonts will probably work on mac, but the function
  FT_GetFilePath_From_Mac_ATS_Name is not included in libof.a. Fix that and get
  back to me.
     
  char path[255];
  long face_index;
  FT_GetFilePath_From_Mac_ATS_Name(StringValuePtr(filename), (UInt8*)&path, 255, &face_index);
  */
  
  char* data_filename = zj_to_data_path(StringValuePtr(filename));
  font_ptr->loadFont(data_filename, NUM2INT(size));
  
  free(data_filename);
  
  return Qnil;
}

VALUE zj_font_initialize(int argc, VALUE* argv, VALUE self) {
  VALUE file = Qnil, size = Qnil, line_height = Qnil;
  VALUE anti_aliased = Qtrue;
  VALUE full_character_set = Qtrue;
  VALUE contours = Qfalse;
  bool hash_given = (TYPE(argv[argc-1]) == T_HASH);
  
  /* scan for normal args, ignore hash if given */
  rb_scan_args(hash_given ? argc-1 : argc, argv, "02", &file, &size);
  
  /* if last arg is options hash, extract local variables */
  if(hash_given) {
    HASH_EXTRACT(argv[argc-1], file);
    HASH_EXTRACT(argv[argc-1], size);
    HASH_EXTRACT(argv[argc-1], line_height);
    HASH_EXTRACT(argv[argc-1], anti_aliased);
    HASH_EXTRACT(argv[argc-1], full_character_set);
    HASH_EXTRACT(argv[argc-1], contours);
    
  }
  
  if(NIL_P(file)) {
    rb_raise(rb_eArgError, "Font name not specified!");
    return Qnil;
  }
  
  if(NIL_P(size)) {
    rb_raise(rb_eArgError, "Font size not specified!");
    return Qnil;
  }
  
  INIT_DATA_PTR(ZajalTrueTypeFont, font_ptr);
  
  VALUE font_file = rb_funcall(rb_const_get(zj_mTypography, rb_intern("Internals")), rb_intern("get_font_file"), 1, file);
  if(NIL_P(font_file)) {
    rb_raise(rb_eArgError, "Font not found!");
    return Qnil;
    
  } else {
    font_ptr->loadFont(StringValuePtr(font_file), NUM2INT(size), RTEST(anti_aliased), RTEST(full_character_set), RTEST(contours));
    if(!NIL_P(line_height)) font_ptr->setLineHeight(NUM2DBL(line_height));
    return self;
  }
  
}

VALUE zj_font_draw(int argc, VALUE* argv, VALUE self) {
  VALUE s, x, y;
  rb_scan_args(argc, argv, "30", &s, &x, &y);
  
  VALUE draw_string = rb_funcall(s, rb_intern("to_s"), 0);
  
  INIT_DATA_PTR(ZajalTrueTypeFont, font_ptr);
  
  if(font_ptr->getContours())
    font_ptr->drawStringAsShapes(StringValuePtr(draw_string), NUM2DBL(x), NUM2DBL(y));
  else
    font_ptr->drawString(StringValuePtr(draw_string), NUM2DBL(x), NUM2DBL(y));
  
  return Qnil;
}

VALUE zj_font_size(VALUE self) {
  INIT_DATA_PTR(ZajalTrueTypeFont, font_ptr);
  
  return INT2FIX(font_ptr->getFontSize());
}

VALUE zj_font_line_height(int argc, VALUE* argv, VALUE self) {
  VALUE new_lineheight;
  rb_scan_args(argc, argv, "01", &new_lineheight);
  
  INIT_DATA_PTR(ZajalTrueTypeFont, font_ptr);
  
  if(!NIL_P(new_lineheight)) {
    /* called with a new line height, set it */
    font_ptr->setLineHeight(NUM2DBL(new_lineheight));
    
  } else {
    /* called without arguments, return current lineheight */
    return DBL2NUM(font_ptr->getLineHeight());
    
  }
  
}

VALUE zj_font_width_of(VALUE self, VALUE string) {
  INIT_DATA_PTR(ZajalTrueTypeFont, font_ptr);
  
  return DBL2NUM(font_ptr->stringWidth(StringValuePtr(string)));
}

VALUE zj_font_height_of(VALUE self, VALUE string) {
  INIT_DATA_PTR(ZajalTrueTypeFont, font_ptr);
  
  return DBL2NUM(font_ptr->stringHeight(StringValuePtr(string)));
}

VALUE zj_font_loaded_p(VALUE self) {
  INIT_DATA_PTR(ZajalTrueTypeFont, font_ptr);
  
  return font_ptr->isLoaded() ? Qtrue : Qfalse;
}

VALUE zj_font_anti_aliased_p(VALUE self) {
  INIT_DATA_PTR(ZajalTrueTypeFont, font_ptr);
  
  return font_ptr->isAntiAliased() ? Qtrue : Qfalse;
}

VALUE zj_font_full_character_set_p(VALUE self) {
  INIT_DATA_PTR(ZajalTrueTypeFont, font_ptr);
  
  return font_ptr->hasFullCharacterSet() ? Qtrue : Qfalse;
}

VALUE zj_font_character_count(VALUE self) {
  INIT_DATA_PTR(ZajalTrueTypeFont, font_ptr);
  
  return INT2FIX(font_ptr->getNumCharacters());
}

/* 
 * Draws text to the screen.
 * 
 * @overload text str
 *   Without a position, {#text} will automatically stack the given string in
 *   the corner of the window. This is very useful for debugging.
 * 
 *   @param [#to_s] str The text to draw to the screen
 * 
 *   @example Stacked text
 *     text "one"
 *     text "on top"
 *     text "of the other"
 *   @example Non-string objects
 *     text Image.new
 *     text 809
 *     text [1, 2, "three"]
 *   @example Debugging
 *     text framerate
 *     k = 9.5/3
 *     text k
 * 
 * @overload text str, x, y
 *   Given a position, {#text} will draw the given string there
 * 
 *   @param [#to_s] str The text to draw to the screen
 *   @param [Numeric] x x coordinate of the left-most character
 *   @param [Numeric] y y coordinate of the baseline.
 * 
 *   @example Positioned text
 *     text "Center", width/2, height/2
 * 
 * @todo Support TrueType fonts
 */
VALUE zj_typography_text(int argc, VALUE* argv, VALUE self) {
  // VALUE text_string = rb_funcall(s, rb_intern("to_s"), 0);
  // ofDrawBitmapString(StringValuePtr(text_string), NUM2DBL(x), NUM2DBL(y));
  // 
  // return Qnil;
  VALUE s, x, y;
  rb_scan_args(argc, argv, "12", &s, &x, &y);
  
  VALUE text_string = rb_funcall(s, rb_intern("to_s"), 0);
  
  if(NIL_P(x) && NIL_P(y)) {
    /* called without coordinates, stack the text */
    ofDrawBitmapString(StringValuePtr(text_string), FIX2INT(INTERNAL_GET(zj_mTypography, stacked_text_x)), FIX2INT(INTERNAL_GET(zj_mTypography, stacked_text_y)));
    INTERNAL_SET(zj_mTypography, stacked_text_y, INT2FIX(FIX2INT(INTERNAL_GET(zj_mTypography, stacked_text_y)) + FIX2INT(INTERNAL_GET(zj_mTypography, stacked_text_line_height))));
    
  } else if(!NIL_P(x) && !NIL_P(y)) {
    /* called with coordinates, draw text at coordinates */
    ofDrawBitmapString(StringValuePtr(text_string), NUM2DBL(x), NUM2DBL(y));
    
  } else {
    rb_raise(rb_eArgError, "Expected 1 or 3 arguments to text!");
    
  }
  
  return Qnil;
}

void Init_Typography() {
  zj_mTypography = rb_define_module_under(zj_mZajal, "Typography");
  rb_define_module_under(zj_mTypography, "Internals");
  
  /* image functions */
  INTERNAL_SET(zj_mTypography, font_hash, rb_hash_new());
  rb_define_private_method(zj_mTypography, "text", RUBY_METHOD_FUNC(zj_typography_text), -1);
  
  zj_cFont = rb_define_class_under(zj_mTypography, "Font", rb_cObject);
  rb_define_singleton_method(zj_cFont, "new", RUBY_METHOD_FUNC(zj_font_new), -1);
  rb_define_method(zj_cFont, "initialize", RUBY_METHOD_FUNC(zj_font_initialize), -1);
  rb_define_method(zj_cFont, "load", RUBY_METHOD_FUNC(zj_font_load), 2);
  rb_define_method(zj_cFont, "draw", RUBY_METHOD_FUNC(zj_font_draw), -1);
  rb_define_method(zj_cFont, "size", RUBY_METHOD_FUNC(zj_font_size), 0);
  rb_define_method(zj_cFont, "line_height", RUBY_METHOD_FUNC(zj_font_line_height), -1);
  rb_define_method(zj_cFont, "width_of", RUBY_METHOD_FUNC(zj_font_width_of), 1);
  rb_define_method(zj_cFont, "height_of", RUBY_METHOD_FUNC(zj_font_height_of), 1);
  rb_define_method(zj_cFont, "loaded?", RUBY_METHOD_FUNC(zj_font_loaded_p), 0);
  rb_define_method(zj_cFont, "anti_aliased?", RUBY_METHOD_FUNC(zj_font_anti_aliased_p), 0);
  rb_define_method(zj_cFont, "full_character_set?", RUBY_METHOD_FUNC(zj_font_full_character_set_p), 0);
  rb_define_method(zj_cFont, "character_count", RUBY_METHOD_FUNC(zj_font_character_count), 0);
}
