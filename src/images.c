#include "ruby.h"
#include "ofMain.h"
#include "zajal.h"

/* global images module and image class */
VALUE zj_mImages;
VALUE zj_cImage;

void zj_image_dealloc(void* image) {
  delete (ofImage*)image;
}

VALUE zj_image_new(int argc, VALUE* argv, VALUE klass) {
  ofImage* image_ptr = new ofImage();
  
  VALUE image = Data_Wrap_Struct(klass, 0, zj_image_dealloc, image_ptr);
  rb_obj_call_init(image, argc, argv);
  return image;
}

VALUE image_new() {
  return zj_image_new(0, 0, zj_cImage);
}

VALUE zj_image_load(VALUE self, VALUE filename) {
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  
  char* data_filename = zj_to_data_path(StringValuePtr(filename));
  image_ptr->loadImage(data_filename);
  
  free(data_filename);
  
  return Qnil;
}

int zj_sym_to_image_type(VALUE sym) {
  ID type_id = SYM2ID(sym);
  
  if(type_id == rb_intern("grayscale")) {
    return OF_IMAGE_GRAYSCALE;
    
  } else if(type_id == rb_intern("rgb")) {
    return OF_IMAGE_COLOR;
    
  } else if(type_id == rb_intern("rgba")) {
    return OF_IMAGE_COLOR_ALPHA;
    
  } else {
    rb_raise(rb_eArgError, "Expected :grayscale, :rgb or :rgba !");
    
  }
  
  return 0;
}

VALUE zj_image_initialize(int argc, VALUE* argv, VALUE self) {
  VALUE file = Qnil, width = Qnil, height = Qnil, scale = Qnil;
  VALUE type = SYM("rgb");
  VALUE use_texture = Qtrue;
  bool hash_given = (argc > 0 && TYPE(argv[argc-1]) == T_HASH);
  
  /* scan for normal args, ignore hash if given */
  VALUE file_width;
  rb_scan_args(hash_given ? argc-1 : argc, argv, "02", &file_width, &height);
  NIL_P(height) ? file = file_width : width = file_width;
  
  /* if last arg is options hash, extract local variables */
  if(hash_given) {
    HASH_EXTRACT(argv[argc-1], file);
    HASH_EXTRACT(argv[argc-1], width);
    HASH_EXTRACT(argv[argc-1], height);
    HASH_EXTRACT(argv[argc-1], scale);
    HASH_EXTRACT(argv[argc-1], type);
    HASH_EXTRACT(argv[argc-1], use_texture);
    
  }
  
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  
  /* TODO use existing zj_image functions instead of duplicating functionality */
  image_ptr->setUseTexture(RTEST(use_texture));
  
  if(!NIL_P(file)) {
    /* loading a file */
    rb_funcall(self, rb_intern("load"), 1, file);
    image_ptr->setImageType(zj_sym_to_image_type(type));
    
    if(!NIL_P(width) || !NIL_P(height)) {
      int new_width = NIL_P(width) ? image_ptr->width : NUM2INT(width);
      int new_height = NIL_P(height) ? image_ptr->height : NUM2INT(height);
      image_ptr->resize(new_width, new_height);
      
    } else if(!NIL_P(scale)) {
      float s = NUM2DBL(scale);
      image_ptr->resize(image_ptr->width * s, image_ptr->height * s);
      
    }
    
  } else if(!NIL_P(width) && !NIL_P(height)) {
    /* allocating a blank image */
    image_ptr->allocate(NUM2INT(width), NUM2INT(height), zj_sym_to_image_type(type));
    
  }
  
  return self;
}

VALUE zj_image_draw(int argc, VALUE* argv, VALUE self) {
  /* TODO should drawing an uninitialized image throw an exception? */
  VALUE x, y, w, h;
  rb_scan_args(argc, argv, "22", &x, &y, &w, &h);
  
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  
  if(NIL_P(w) && NIL_P(h)) {
    /* called without width and height, just use coords */
    image_ptr->draw(NUM2DBL(x), NUM2DBL(y));
    
  } else if(!NIL_P(w) && NIL_P(h)) {
    /* called with size, scale proportionately */
    float s = NUM2DBL(w);
    image_ptr->draw(NUM2DBL(x), NUM2DBL(y), image_ptr->width*s, image_ptr->height*s);
    
  } else {
    /* called with width and height, use coords and dimentions */
    image_ptr->draw(NUM2DBL(x), NUM2DBL(y), NUM2DBL(w), NUM2DBL(h));
    
  }
  
  return Qnil;
}

VALUE zj_image_clear(VALUE self) {
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  image_ptr->clear();
  
  return Qnil;
}

VALUE zj_image_save(VALUE self, VALUE filename) {
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  
  char* save_filename = zj_to_data_path(StringValuePtr(filename));
  image_ptr->saveImage(save_filename);
  
  free(save_filename);
  
  return Qnil;
}

VALUE zj_image_resize(int argc, VALUE* argv, VALUE self) {
  VALUE w, h;
  rb_scan_args(argc, argv, "11", &w, &h);
  
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  
  if(!NIL_P(w) && NIL_P(h)) {
    /* called with one argument, scale proportionately */
    float s = NUM2DBL(w);
    image_ptr->resize(image_ptr->width*s, image_ptr->height*s);
  
  } else if(!NIL_P(w) && !NIL_P(h)) {
    /* called with two arguments, resize to given width and height */
    image_ptr->resize(NUM2DBL(w), NUM2DBL(h));
    
  } else {
    rb_raise(rb_eArgError, "Unexpected arguments to Image::resize!");
    
  }
  
  return Qnil;
}

VALUE zj_image_grab_screen(int argc, VALUE* argv, VALUE self) {
  VALUE x, y, w, h;
  rb_scan_args(argc, argv, "04", &x, &y, &w, &h);
  
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  
  if(NIL_P(x) && NIL_P(y) && NIL_P(w) && NIL_P(h)) {
    /* called without arguments, grab whole screen */
    image_ptr->grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    
  } else if(!NIL_P(x) && !NIL_P(y) && !NIL_P(w) && !NIL_P(h)) {
    /* called with all arguments, grab screen according to coords/dimentions */
    image_ptr->grabScreen(NUM2DBL(x), NUM2DBL(y), NUM2DBL(w), NUM2DBL(h));
    
  } else {
    rb_raise(rb_eArgError, "Unexpected arguments to Image::grab_screen!");
    
  }
  
  return self;
}

VALUE zj_image_get_pixel(VALUE self, VALUE x, VALUE y) {
  int _x = NUM2INT(x);
  int _y = NUM2INT(y);
  
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  
  if(_x > image_ptr->width || _y > image_ptr->height || _x < 0 || _y < 0)
    return Qnil;
  
  int Bpp = image_ptr->bpp / 8; // bytes per pixel = 1, 3 or 4
  unsigned char* pixels = image_ptr->getPixels();
  int i = _x * Bpp + _y * image_ptr->width * Bpp;
  
  if(Bpp == 1)
    return rb_ary_new3(1, INT2FIX(pixels[i]));
  else if(Bpp == 3)
    return rb_ary_new3(3, INT2FIX(pixels[i]), INT2FIX(pixels[i+1]), INT2FIX(pixels[i+2]));
  else if(Bpp == 4)
    return rb_ary_new3(4, INT2FIX(pixels[i]), INT2FIX(pixels[i+1]), INT2FIX(pixels[i+2]), INT2FIX(pixels[i+3]));
  else
    return Qnil;
}

VALUE zj_image_each_pixel(int argc, VALUE* argv, VALUE self) {
  // VALUE x, y, w, h;
  // rb_scan_args(argc, argv, "04", &x, &y, &w, &h);
  
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  
  unsigned char* pixels = image_ptr->getPixels();
  int Bpp = image_ptr->bpp/8; // bytes per pixel = 1, 3 or 4
  int total_bytes = image_ptr->width * image_ptr->height * Bpp;
  
  VALUE yield_return;
  
  bool should_update = false;
  
  unsigned int x = 0;
  unsigned int y = 0;
  
  for(int i = 0; i < total_bytes; i += Bpp) {
    switch(Bpp) {
      case 1:
        yield_return = rb_yield_values(3, INT2FIX(x), INT2FIX(y), INT2FIX(pixels[i]));
        break;
      case 3:
        yield_return = rb_yield_values(5, INT2FIX(x), INT2FIX(y), INT2FIX(pixels[i]), INT2FIX(pixels[i+1]), INT2FIX(pixels[i+2]));
        break;
      case 4:
        yield_return = rb_yield_values(6, INT2FIX(x), INT2FIX(y), INT2FIX(pixels[i]), INT2FIX(pixels[i+1]), INT2FIX(pixels[i+2]), INT2FIX(pixels[i+3]));
        break;
    }
    
    if(TYPE(yield_return) == T_ARRAY) {
      should_update = true;
      
      pixels[i]               = (unsigned char)NUM2INT(RARRAY_PTR(yield_return)[0]);
      if(Bpp > 1) pixels[i+1] = (unsigned char)NUM2INT(RARRAY_PTR(yield_return)[1]);
      if(Bpp > 1) pixels[i+2] = (unsigned char)NUM2INT(RARRAY_PTR(yield_return)[2]);
      if(Bpp > 3) pixels[i+3] = (unsigned char)NUM2INT(RARRAY_PTR(yield_return)[3]);
    }
    
    if(++x >= image_ptr->width) x = 0;
    if(x == 0) y++;
  }
  
  if(should_update) image_ptr->update();
  
  return Qnil;
}

VALUE zj_image_image(int argc, VALUE* argv, VALUE self) {
  if(argc == 0) {
    rb_raise(rb_eArgError, "Too few arguments to Images::image!");
    return Qnil;
  }
  
  VALUE filename = argv[0];
  VALUE cached_image = rb_hash_aref(INTERNAL_GET(zj_mImages, image_hash), filename);
  
  if(NIL_P(cached_image)) {
    /* image never used before, load it from disk, cache it */
    cached_image = rb_funcall(zj_cImage, rb_intern("new"), 1, filename);
    rb_hash_aset(INTERNAL_GET(zj_mImages, image_hash), filename, cached_image);
  }
  
  /* remove filename from args, draw cached image as normal */
  zj_image_draw(--argc, ++argv, cached_image);
  
  return Qnil;
}

VALUE zj_image_width(int argc, VALUE* argv, VALUE self) {
  VALUE new_width;
  rb_scan_args(argc, argv, "01", &new_width);
  
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  
  if(NIL_P(new_width)) {
    /* called without arguments, return width */
    return DBL2NUM(image_ptr->width);
    
  } else {
    /* called with width argument, set width=new_width */
    image_ptr->resize(NUM2DBL(new_width), image_ptr->height);
    
    return Qnil;
  }
  
}

VALUE zj_image_height(int argc, VALUE* argv, VALUE self) {
  VALUE new_height;
  rb_scan_args(argc, argv, "01", &new_height);
  
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  
  if(NIL_P(new_height)) {
    /* called without arguments, return width */
    return DBL2NUM(image_ptr->height);
    
  } else {
    /* called with width argument, set width=new_width */
    image_ptr->resize(image_ptr->width, NUM2DBL(new_height));
    
    return Qnil;
  }
  
}

VALUE zj_image_type(int argc, VALUE* argv, VALUE self) {
  VALUE new_type;
  rb_scan_args(argc, argv, "01", &new_type);
  
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  
  ID grayscale_id = rb_intern("grayscale");
  ID rgb_id = rb_intern("rgb");
  ID rgba_id = rb_intern("rgba");
  
  if(NIL_P(new_type)) {
    /* called without arguments, return image type */
    if(image_ptr->type == OF_IMAGE_GRAYSCALE) {
      return ID2SYM(grayscale_id);
      
    } else if(image_ptr->type == OF_IMAGE_COLOR) {
      return ID2SYM(rgb_id);
      
    } else if(image_ptr->type == OF_IMAGE_COLOR_ALPHA) {
      return ID2SYM(rgba_id);
      
    } else {
      rb_bug("Unknown image type encountered!");
      
    }
    
  } else {
    /* called with a new type, set type to new_type */
    ID new_type_id = SYM2ID(new_type);
    if(new_type_id == grayscale_id) {
      image_ptr->setImageType(OF_IMAGE_GRAYSCALE);
      
    } else if(new_type_id == rgb_id) {
      image_ptr->setImageType(OF_IMAGE_COLOR);
      
    } else if(new_type_id == rgba_id) {
      image_ptr->setImageType(OF_IMAGE_COLOR_ALPHA);
      
    } else {
      rb_raise(rb_eArgError, "Expected :grayscale, :rgb or :rgba !");
      
    }
    
  }
  
  return Qnil;
}

VALUE zj_image_bpp(VALUE self) {
  ofImage* image_ptr;
  Data_Get_Struct(self, ofImage, image_ptr);
  
  return DBL2NUM(image_ptr->bpp);
}

VALUE zj_grab_screen(int argc, VALUE* argv, VALUE klass) {
  VALUE new_image = image_new();
  rb_funcall2(new_image, rb_intern("grab_screen"), argc, argv);
  
  return new_image;
}

void Init_Images() {
  zj_mImages = rb_define_module_under(zj_mZajal, "Images");
  rb_define_module_under(zj_mImages, "Internals");
  
  /* image functions */
  INTERNAL_SET(zj_mImages, image_hash, rb_hash_new());
  rb_define_private_method(zj_mImages, "image", RUBY_METHOD_FUNC(zj_image_image), -1);
  rb_define_private_method(zj_mImages, "grab_screen", RUBY_METHOD_FUNC(zj_grab_screen), -1);
  
  /* the Image class */
  zj_cImage = rb_define_class_under(zj_mImages, "Image", rb_cObject);
  rb_define_singleton_method(zj_cImage, "new", RUBY_METHOD_FUNC(zj_image_new), -1);
  rb_define_method(zj_cImage, "initialize", RUBY_METHOD_FUNC(zj_image_initialize), -1);
  rb_define_method(zj_cImage, "draw", RUBY_METHOD_FUNC(zj_image_draw), -1);
  rb_define_method(zj_cImage, "clear", RUBY_METHOD_FUNC(zj_image_clear), 0);
  rb_define_method(zj_cImage, "load", RUBY_METHOD_FUNC(zj_image_load), 1);
  rb_define_method(zj_cImage, "save", RUBY_METHOD_FUNC(zj_image_save), 1);
  rb_define_method(zj_cImage, "resize", RUBY_METHOD_FUNC(zj_image_resize), -1);
  rb_define_method(zj_cImage, "grab_screen", RUBY_METHOD_FUNC(zj_image_grab_screen), -1);
  
  rb_define_method(zj_cImage, "get_pixel", RUBY_METHOD_FUNC(zj_image_get_pixel), 2);
  rb_define_method(zj_cImage, "each_pixel", RUBY_METHOD_FUNC(zj_image_each_pixel), -1);
  
  rb_define_method(zj_cImage, "type", RUBY_METHOD_FUNC(zj_image_type), -1);
  rb_define_method(zj_cImage, "bpp", RUBY_METHOD_FUNC(zj_image_bpp), 0);
  
  rb_define_method(zj_cImage, "width", RUBY_METHOD_FUNC(zj_image_width), -1);
  rb_define_method(zj_cImage, "height", RUBY_METHOD_FUNC(zj_image_height), -1);
}