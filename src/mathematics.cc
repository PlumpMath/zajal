#include "ruby.h"
#include "ofMain.h"
#include "zajal.h"

/* external math module */
VALUE zj_mMathematics;

/* 
 * @overload seed_random
 *   Seeds random values using the current time. Useful to have unpredictable
 *   series of random numbers.
 *   @screenshot Unpredictable random numbers
 *     seed_random
 *     text [(random*10).round, (random*10).round, (random*10).round]
 *     text [(random*10).round, (random*10).round, (random*10).round]
 *     text ""
 *     
 *     seed_random
 *     text [(random*10).round, (random*10).round, (random*10).round]
 *     text ""
 *     
 *     seed_random
 *     text [(random*10).round, (random*10).round, (random*10).round]
 *     text [(random*10).round, (random*10).round, (random*10).round]
 * 
 * @overload seed_random k
 *   Seeds random values using +k+. Useful to have predictable series of
 *   random numbers.
 *   @param [Numeric] k The seed to use
 *   @screenshot Predictable random numbers
 *     seed_random 0
 *     text [(random*10).round, (random*10).round, (random*10).round]
 *     text [(random*10).round, (random*10).round, (random*10).round]
 *     text ""
 *     
 *     seed_random 9000
 *     text [(random*10).round, (random*10).round, (random*10).round]
 *     text ""
 *     
 *     seed_random 0
 *     text [(random*10).round, (random*10).round, (random*10).round]
 *     text [(random*10).round, (random*10).round, (random*10).round]
 */
VALUE zj_seed_random(int argc, VALUE* argv, VALUE self) {
  VALUE val;
  rb_scan_args(argc, argv, "01", &val);
  
  if(NIL_P(val)) {
    /*  called without arguments, seed to clock time */
    ofSeedRandom();
    
  } else if(!NIL_P(val)) {
    /*  called with one argument, seed to value */
    ofSeedRandom(NUM2DBL(val));
    
  }
  
  return Qnil;
}
  
/* 
 * Generate a pseudorandom number
 * 
 * @overload random
 *   Generate a pseudorandom number between zero and one
 *   @return [0..1] A pseudorandom number
 * 
 * @overload random max
 *   Generate a pseudorandom number between zero and +max+
 *   @param [Numeric] max The upper bound of the pseudorandom number to generate
 *   @return [0..max] A pseudorandom number
 * 
 * @overload random min, max
 *   Generate a pseudorandom number between +min+ and +max+
 *   @param [Numeric] min The lower bound of the pseudorandom number to generate
 *   @param [Numeric] max The upper bound of the pseudorandom number to generate
 *   @return [min..max] A pseudorandom number
 */
VALUE zj_random(int argc, VALUE* argv, VALUE self) {
  VALUE min, max;
  rb_scan_args(argc, argv, "02", &min, &max);
  
  if(NIL_P(min) && NIL_P(max)) {
    /*  called without arguments, return random number x, 0 < x < 1 */
    return DBL2NUM(ofRandomuf());
    
  } else if(!NIL_P(min) && NIL_P(max)) {
    /*  called with one argument, return random number x, 0 < x < max */
    max = min;
    return DBL2NUM(ofRandom(0, NUM2DBL(max)));
    
  } else if(!NIL_P(min) && !NIL_P(max)) {
    /*  called with two arguments, return random number x, min < x < max */
    return DBL2NUM(ofRandom(NUM2DBL(min), NUM2DBL(max)));
    
  }
  
  return Qnil;
}

/* 
 * @return [0...width] A pseudorandom value
 * @screenshot Bars
 *   rectangle_mode :center
 *   
 *   rectangle width/2, 10, random_width, 20
 *   rectangle width/2, 30, random_width, 20
 *   rectangle width/2, 50, random_width, 20
 *   rectangle width/2, 70, random_width, 20
 *   rectangle width/2, 90, random_width, 20
 * 
 * @screenshot Squares
 *   rectangle_mode :center
 *   
 *   color 240
 *   square random_width, 10, 5
 *   
 *   color 200
 *   square random_width, 30, 5
 *   
 *   color 160
 *   square random_width, 50, 5
 *   
 *   color 120
 *   square random_width, 70, 5
 *   
 *   color 80
 *   square random_width, 90, 5
 */
VALUE zj_random_width(VALUE self) {
  return DBL2NUM(ofRandomWidth());
}

/* 
 * @return [0...height] A pseudorandom value
 */
VALUE zj_random_height(VALUE self) {
  return DBL2NUM(ofRandomHeight());
}

/* 
 * Generate perlin noise, a value between zero and one dependant on parameters
 * 
 * @overload noise x
 *   Generate one dimentional perlin noise
 *   @param [Numeric] x
 *   @return [0...1] The one dimentional noise at +x+
 *   @screenshot Noise across x
 *     for x in 0..width
 *       noise_height = height * noise(x)
 *       line x, noise_height, x, height
 *     end
 *   
 *   @screenshot Smoother noise
 *     for x in 0..width
 *       noise_height = height * noise(x*0.05)
 *       line x, noise_height, x, height
 *     end
 *   
 *   @todo `x, y = width/2, height/2` does not work!
 *   @screenshot Complex effect
 *     x = width/2
 *     y = height/2
 *     s = 70
 *     
 *     for i in 0..600
 *       a = i / 600.0 * (0..TWO_PI)
 *       r = noise(a+1) * (200..255)
 *       g = noise(a+2) * (200..32)
 *       b = noise(a) * (32..255)
 *       
 *       color r, g, b
 *       line x, y,
 *       x + cos(a) * s, y + sin(a) * s
 *     end
 * 
 * @overload noise x, y
 *   Generate two dimentional perlin noise
 *   @param [Numeric] x
 *   @param [Numeric] y
 *   @return [0...1] The two dimentional noise at +x+, +y+
 *   @screenshot Snow
 *     for x in 0..width
 *       for y in 0..height
 *         color 255 * noise(x, y)
 *         point x, y
 *       end
 *     end
 * 
 *   @screenshot Smoother snow
 *     for x in 0..width
 *       for y in 0..height
 *         color 255 * noise(x*0.02, y*0.02)
 *         point x, y
 *       end
 *     end
 * 
 * @overload noise x, y, z
 *   Generate three dimentional perlin noise
 *   @param [Numeric] x
 *   @param [Numeric] y
 *   @param [Numeric] z
 *   @return [0...1] The three dimentional noise at +x+, +y+, +z+
 * 
 * @overload noise x, y, z, w
 *   Generate four dimentional perlin noise
 *   @param [Numeric] x
 *   @param [Numeric] y
 *   @param [Numeric] z
 *   @param [Numeric] w
 *   @return [0...1] The four dimentional noise at +x+, +y+, +z+, +w+
 */
VALUE zj_noise(int argc, VALUE* argv, VALUE self) {
  VALUE x, y, z, w;
  rb_scan_args(argc, argv, "13", &x, &y, &z, &w);
  
  if(!NIL_P(y) && !NIL_P(z) && !NIL_P(w)) {
    /* called with four arguments, return 4D noise */
    return DBL2NUM(ofNoise(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z), NUM2DBL(w)));
    
  } else if(!NIL_P(y) && !NIL_P(z) && NIL_P(w)) {
    /* called with three arguments, return 3D noise */
    return DBL2NUM(ofNoise(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z)));
    
  } else if(!NIL_P(y) && NIL_P(z) && NIL_P(w)) {
    /* called with two arguments, return 2D noise */
    return DBL2NUM(ofNoise(NUM2DBL(x), NUM2DBL(y)));
    
  } else if(NIL_P(y) && NIL_P(z) && NIL_P(w)) {
    /* called with one argument, return 1D noise */
    /* TODO calling ofNoise with one parameter always returns 0.5, why? */
    return DBL2NUM(ofNoise(NUM2DBL(x), 0));
    
  }
  
  return Qnil;
}

/* 
 * Generate signed perlin noise, a value between minus one and one dependant
 * on parameters
 * 
 * @overload signed_noise x
 *   @param [Numeric] x
 *   @return [-1..1] The one dimentional noise at +x+
 *   @screenshot Signed noise
 *     for x in 0..width
 *       noise_height = height/2 * signed_noise(x)
 *       rectangle x, height/2, 1, noise_height
 *     end
 * 
 *   @screenshot Smoother signed noise
 *     for x in 0..width
 *       noise_height = height/2 * signed_noise(x*0.05)
 *       rectangle x, height/2, 1, noise_height
 *     end
 * 
 * @overload signed_noise x, y
 *   @param [Numeric] x
 *   @param [Numeric] y
 *   @return [-1..1] The two dimentional noise at +x+, +y+
 * 
 * @overload signed_noise x, y, z
 *   @param [Numeric] x
 *   @param [Numeric] y
 *   @param [Numeric] z
 *   @return [-1..1] The three dimentional noise at +x+, +y+, +z+
 * 
 * @overload signed_noise x, y, z, w
 *   @param [Numeric] x
 *   @param [Numeric] y
 *   @param [Numeric] z
 *   @param [Numeric] w
 *   @return [-1..1] The four dimentional noise at +x+, +y+, +z+, +w+
 */
VALUE zj_signed_noise(int argc, VALUE* argv, VALUE self) {
  VALUE x, y, z, w;
  rb_scan_args(argc, argv, "13", &x, &y, &z, &w);
  
  if(!NIL_P(y) && !NIL_P(z) && !NIL_P(w)) {
    /* called with four arguments, return 4D noise */
    return DBL2NUM(ofSignedNoise(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z), NUM2DBL(w)));
    
  } else if(!NIL_P(y) && !NIL_P(z) && NIL_P(w)) {
    /* called with three arguments, return 3D noise */
    return DBL2NUM(ofSignedNoise(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z)));
    
  } else if(!NIL_P(y) && NIL_P(z) && NIL_P(w)) {
    /* called with two arguments, return 2D noise */
    return DBL2NUM(ofSignedNoise(NUM2DBL(x), NUM2DBL(y)));
    
  } else if(NIL_P(y) && NIL_P(z) && NIL_P(w)) {
    /* called with one argument, return 1D noise */
    /* TODO calling ofSignedNoise with one parameter always returns 0.5, why? */
    return DBL2NUM(ofSignedNoise(NUM2DBL(x), 0));
    
  }
  
  return Qnil;
}

/* methods added to Numeric class */

VALUE zj_to_deg(VALUE self) {
  /* TODO test if faster using own implementation vs calling of* function */
  /* return DBL2NUM(NUM2DBL(self) * 57.29577951); */
  return DBL2NUM(ofRadToDeg(NUM2DBL(self)));
}

VALUE zj_to_rad(VALUE self) {
  /* TODO test if faster using own implementation vs calling of* function */
  /* return DBL2NUM(NUM2DBL(self) * 0.017453293); */
  return DBL2NUM(ofDegToRad(NUM2DBL(self)));
}

VALUE zj_sign(VALUE self) {
  return DBL2NUM(ofSign(NUM2DBL(self)));
}

VALUE zj_clamp(VALUE self, VALUE min, VALUE max) {
  return DBL2NUM(ofClamp(NUM2DBL(self), NUM2DBL(min), NUM2DBL(max)));
}

VALUE zj_normalize(VALUE self, VALUE min, VALUE max) {
  return DBL2NUM(ofNormalize(NUM2DBL(self), NUM2DBL(min), NUM2DBL(max)));
}

void Init_Mathematics() {
  zj_mMathematics = rb_define_module_under(zj_mZajal, "Mathematics");
  rb_define_module_under(zj_mMathematics, "Internals");
  
  rb_define_private_method(zj_mMathematics, "seed_random", RUBY_METHOD_FUNC(zj_seed_random), -1);
  rb_define_private_method(zj_mMathematics, "random", RUBY_METHOD_FUNC(zj_random), -1);
  rb_define_private_method(zj_mMathematics, "random_width", RUBY_METHOD_FUNC(zj_random_width), 0);
  rb_define_private_method(zj_mMathematics, "random_height", RUBY_METHOD_FUNC(zj_random_height), 0);
  
  rb_define_private_method(zj_mMathematics, "noise", RUBY_METHOD_FUNC(zj_noise), -1);
  rb_define_private_method(zj_mMathematics, "signed_noise", RUBY_METHOD_FUNC(zj_signed_noise), -1);
  
  /* 3.141592654 */
  rb_define_const(zj_mMathematics, "PI", DBL2NUM(PI));
  
  /* 6.283185307 */
  rb_define_const(zj_mMathematics, "TWO_PI", DBL2NUM(TWO_PI));
  
  /* 12.56637061 */
  rb_define_const(zj_mMathematics, "FOUR_PI", DBL2NUM(FOUR_PI));
  
  /* 1.570796327 */
  rb_define_const(zj_mMathematics, "HALF_PI", DBL2NUM(HALF_PI));
  
  rb_define_method(rb_cNumeric, "to_deg", RUBY_METHOD_FUNC(zj_to_deg), 0);
  rb_define_method(rb_cNumeric, "to_rad", RUBY_METHOD_FUNC(zj_to_rad), 0);
  rb_define_method(rb_cNumeric, "sign", RUBY_METHOD_FUNC(zj_sign), 0);
  rb_define_method(rb_cNumeric, "clamp", RUBY_METHOD_FUNC(zj_clamp), 2);
  rb_define_method(rb_cNumeric, "normalize", RUBY_METHOD_FUNC(zj_normalize), 2);
}
