#include "ruby.h"
#include "ofMain.h"
#include "zajal.h"

/* global video module and video class */
VALUE zj_mVideos;
VALUE zj_cVideo;

void zj_video_dealloc(void* video) {
  delete (ofVideoPlayer*)video;
}

/* 
 * Load a video from the computer to play.
 * 
 * @param [String] filename File name of the video to load
 */
VALUE zj_video_load(VALUE self, VALUE filename) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  char* data_filename = zj_to_data_path(StringValuePtr(filename));
  video_ptr->loadMovie(data_filename);
  
  free(data_filename);
  
  return Qnil;
}

VALUE zj_video_initialize(int argc, VALUE* argv, VALUE self) {
  VALUE filename;
  rb_scan_args(argc, argv, "01", &filename);
  
  if(!NIL_P(filename)) {
    /* called with a filename, load it */
    rb_funcall(self, rb_intern("load"), 1, filename);
    
  }
  
  return self;
}

VALUE zj_video_close(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  video_ptr->closeMovie();
  
  return Qnil;
}

VALUE zj_video_update(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  video_ptr->update();
  
  return Qnil;
}

/* 
 * Start playing the video.
 */
VALUE zj_video_play(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  video_ptr->play();
  
  return Qnil;
}

/* 
 * Is the movie playing?
 */
VALUE zj_video_playing_p(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  return video_ptr->isPlaying() ? Qtrue : Qfalse;
}

/* 
 * Did the movie load?
 */
VALUE zj_video_loaded_p(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  return video_ptr->isLoaded() ? Qtrue : Qfalse;
}

/* 
 * Is the movie paused?
 */
VALUE zj_video_paused_p(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  return video_ptr->isPaused() ? Qtrue : Qfalse;
}

/* 
 * Is the movie done?
 */
VALUE zj_video_done_p(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  return video_ptr->getIsMovieDone() ? Qtrue : Qfalse;
}

/* 
 * Stop playing the video.
 */
VALUE zj_video_stop(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  video_ptr->stop();
  
  return Qnil;
}

/* 
 * Pause the the video.
 */
VALUE zj_pause_stop(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  video_ptr->setPaused(true);
  
  return Qnil;
}

/* 
 * Return the duration of the video
 */
VALUE zj_video_duration(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  return DBL2NUM(video_ptr->getDuration());
}

VALUE zj_video_draw1(VALUE self, float x, float y) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  video_ptr->draw(x, y);
  
  return Qnil;
}

VALUE zj_video_draw2(VALUE self, float x, float y, float s) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  video_ptr->draw(x, y, video_ptr->width*s, video_ptr->height*s);
  
  return Qnil;
}

VALUE zj_video_draw3(VALUE self, float x, float y, float w, float h) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  video_ptr->draw(x, y, w, h);
  
  return Qnil;
}

/* 
 * Render the video to the screen
 * 
 * @overload draw x, y
 *   Render the video at its natural size
 *   @param [Numeric] x The x coordinate to draw the video at
 *   @param [Numeric] y The y coordinate to draw the video at
 * 
 * @overload draw x, y, s
 *   Render the video at a particular size
 *   @param [Numeric] x The x coordinate to draw the video at
 *   @param [Numeric] y The y coordinate to draw the video at
 *   @param [Numeric] s The amount to scale the video, e.g. +1.0+ is normal
 *     size, +2.0+ is double and +0.5+ is half
 * 
 * @overload draw x, y, w, h
 *   Render the video at a particular size
 *   @param [Numeric] x The x coordinate to draw the video at
 *   @param [Numeric] y The y coordinate to draw the video at
 *   @param [Numeric] w The width to draw the video at
 *   @param [Numeric] h The height to draw the video at
 */
VALUE zj_video_draw(int argc, VALUE* argv, VALUE self) {
  VALUE x, y, w, h;
  rb_scan_args(argc, argv, "22", &x, &y, &w, &h);
  
  rb_iv_set(self, "@was_drawn", Qtrue);
  
  switch(argc) {
    /* called without width and height, just use coords */
    case 2: return zj_video_draw1(self, NUM2DBL(x), NUM2DBL(y));
    
    /* called with size, scale proportionately */
    case 3: return zj_video_draw2(self, NUM2DBL(x), NUM2DBL(y), NUM2DBL(w));
    
    /* called with width and height, use coords and dimentions */
    case 4: return zj_video_draw3(self, NUM2DBL(x), NUM2DBL(y), NUM2DBL(w), NUM2DBL(h));
  }
}

/* 
 * @overload video filename, x, y
 *   Draws a video at the given +x+ and +y+ position. The video will play
 *   normally with audio as long as this method is called every frame.
 *   @param [String] filename File name of the video
 *   @param [Numeric] x Distance from the left side of the window to draw the video
 *   @param [Numeric] y Distance from the top side of the window to draw the video
 * 
 * @overload video filename, x, y, w, h
 *   Draws a video at the given +x+ and +y+ position with a given width and height.
 *   The video will play normally with audio as long as this method is called
 *   every frame.
 *   @param [String] filename File name of the video
 *   @param [Numeric] x Distance from the left side of the window to draw the video
 *   @param [Numeric] y Distance from the top side of the window to draw the video
 *   @param [Numeric] w Width to draw the video at
 *   @param [Numeric] h Height to draw the video at
 */
VALUE zj_video_video(int argc, VALUE* argv, VALUE self) {
  if(argc == 0) {
    rb_raise(rb_eArgError, "Too few arguments to Videos::video!");
    return Qnil;
  }
  
  VALUE filename = argv[0];
  VALUE cached_video = rb_hash_aref(INTERNAL_GET(zj_mVideos, video_hash), filename);
  
  if(NIL_P(cached_video)) {
    /* video never used before, load it from disk, cache it */
    cached_video = rb_funcall(zj_cVideo, rb_intern("new"), 1, filename);
    zj_video_play(cached_video);
    rb_hash_aset(INTERNAL_GET(zj_mVideos, video_hash), filename, cached_video);
  }
  
  /* remove filename from args, draw cached video as normal */
  zj_video_draw(--argc, ++argv, cached_video);
  
  return Qnil;
}

VALUE zj_video_get_frame(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  return INT2FIX(video_ptr->getCurrentFrame());
}

VALUE zj_video_set_frame(VALUE self, int frame) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  video_ptr->setFrame(frame);
  
  return Qnil;
}

/*
 * Get or set the current frame of the video
 */
VALUE zj_video_frame(int argc, VALUE* argv, VALUE self) {
  VALUE new_frame;
  rb_scan_args(argc, argv, "01", &new_frame);
  
  switch(argc) {
    /* called with no arguments, return current frame */
    case 0: return zj_video_get_frame(self);
      
    /* called with one argument, jump to given frame */
    case 1: return zj_video_set_frame(self, NUM2INT(new_frame));
  }
}

VALUE zj_video_get_speed(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  return DBL2NUM(video_ptr->getSpeed());
}

VALUE zj_video_set_speed(VALUE self, float speed) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  video_ptr->setSpeed(speed);
  
  return Qnil;
}

/*
 * Get or set the video's playback speed
 * 
 * A playback speed of 1.0 represents the normal speed of the video file.
 * Values above or below 1.0 represent slower and faster playback speeds,
 * respectively.
 */ 
VALUE zj_video_speed(int argc, VALUE* argv, VALUE self) {
  VALUE new_speed;
  rb_scan_args(argc, argv, "01", &new_speed);
  
  switch(argc) {
    /* called with no arguments, return current speed */
    case 0: return zj_video_get_speed(self);
    
    /* called with one argument, set new speed */
    case 1: return zj_video_set_speed(self, NUM2DBL(new_speed));
  }
}

VALUE zj_video_get_position(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  return DBL2NUM(video_ptr->getPosition());
}

VALUE zj_video_set_position(VALUE self, float pos) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  video_ptr->setPosition(pos);
  
  return Qnil;
}

/*
 * Get or set the current position in the video
 * 
 * Position is a number between 0.0 and 1.0, 0.0 being the first frame of the
 * video, 1.0 being the last, 0.5 being the midpoint and so on.
 */
VALUE zj_video_position(int argc, VALUE* argv, VALUE self) {
  VALUE new_position;
  rb_scan_args(argc, argv, "01", &new_position);
  
  switch(argc) {
    /* called with no arguments, return current position */
    case 0: return zj_video_get_position(self);
    
    /* called with one argument, set new position */
    case 1: return zj_video_set_position(self, NUM2DBL(new_position));
  }
}

VALUE zj_video_get_loop_mode(VALUE self) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  switch(video_ptr->getLoopState()) {
    case OF_LOOP_NONE: return SYM("none");
    case OF_LOOP_PALINDROME: return SYM("palindrome");
    case OF_LOOP_NORMAL: return SYM("normal");
    default: return Qnil;
  }
}

VALUE zj_video_set_loop_mode(VALUE self, ID loop_mode) {
  INIT_DATA_PTR(ofVideoPlayer, video_ptr);
  
  if(loop_mode == rb_intern("none"))
    video_ptr->setLoopState(OF_LOOP_NONE);
  else if(loop_mode == rb_intern("palindrome"))
    video_ptr->setLoopState(OF_LOOP_PALINDROME);
  else if(loop_mode == rb_intern("normal"))
    video_ptr->setLoopState(OF_LOOP_NORMAL);
  else
    rb_raise(rb_eArgError, "Invalid loop mode!");
    
  return Qnil;
}

/* 
 * Get or set the current looping mode of the video
 */
VALUE zj_video_loop_mode(int argc, VALUE* argv, VALUE self) {
  VALUE new_loop_mode;
  rb_scan_args(argc, argv, "01", &new_loop_mode);
  
  switch(argc) {
    /* called with no arguments, return current looping state */
    case 0: return zj_video_get_loop_mode(self);
    
    /* called with one argument, set new looping state */
    case 1: return zj_video_set_loop_mode(self, SYM2ID(new_loop_mode));
  }
}

/* 
 * Create a new video object
 * 
 * @param [String] filename File name of the video to load
 */
VALUE zj_video_new(int argc, VALUE* argv, VALUE klass) {
  ofVideoPlayer* video_ptr = new ofVideoPlayer();
  
  VALUE video = Data_Wrap_Struct(klass, 0, zj_video_dealloc, video_ptr);
  rb_obj_call_init(video, argc, argv);
  
  zj_video_play(video);
  rb_ary_push(INTERNAL_GET(zj_mVideos, video_ary), rb_obj_id(video));
  
  return video;
}

void Init_Videos() {
  zj_mVideos = rb_define_module_under(zj_mZajal, "Videos");
  rb_define_module_under(zj_mVideos, "Internals");
  INTERNAL_SET(zj_mVideos, video_ary, rb_ary_new());
  INTERNAL_SET(zj_mVideos, video_hash, rb_hash_new());
  
  rb_define_private_method(zj_mVideos, "video", RUBY_METHOD_FUNC(zj_video_video), -1);
  
  /* the Image class */
  zj_cVideo = rb_define_class_under(zj_mVideos, "Video", rb_cObject);
  rb_define_singleton_method(zj_cVideo, "new", RUBY_METHOD_FUNC(zj_video_new), -1);
  rb_define_method(zj_cVideo, "initialize", RUBY_METHOD_FUNC(zj_video_initialize), -1);
  rb_define_method(zj_cVideo, "draw", RUBY_METHOD_FUNC(zj_video_draw), -1);
  rb_define_method(zj_cVideo, "load", RUBY_METHOD_FUNC(zj_video_load), 1);
  rb_define_method(zj_cVideo, "close", RUBY_METHOD_FUNC(zj_video_close), 0);
  rb_define_method(zj_cVideo, "update", RUBY_METHOD_FUNC(zj_video_update), 0);
  rb_define_method(zj_cVideo, "play", RUBY_METHOD_FUNC(zj_video_play), 0);
  rb_define_method(zj_cVideo, "duration", RUBY_METHOD_FUNC(zj_video_duration), 0);
  
  rb_define_method(zj_cVideo, "loop_mode", RUBY_METHOD_FUNC(zj_video_loop_mode), -1);
  
  // whats the difference here?
  rb_define_method(zj_cVideo, "stop", RUBY_METHOD_FUNC(zj_video_stop), 0);
  rb_define_method(zj_cVideo, "pause", RUBY_METHOD_FUNC(zj_pause_stop), 0);
  
  rb_define_method(zj_cVideo, "frame", RUBY_METHOD_FUNC(zj_video_frame), -1);
  rb_define_method(zj_cVideo, "speed", RUBY_METHOD_FUNC(zj_video_speed), -1);
  rb_define_method(zj_cVideo, "position", RUBY_METHOD_FUNC(zj_video_position), -1);
  
  rb_define_method(zj_cVideo, "playing?", RUBY_METHOD_FUNC(zj_video_playing_p), 0);
  rb_define_method(zj_cVideo, "loaded?", RUBY_METHOD_FUNC(zj_video_loaded_p), 0);
  rb_define_method(zj_cVideo, "paused?", RUBY_METHOD_FUNC(zj_video_paused_p), 0);
  rb_define_method(zj_cVideo, "done?", RUBY_METHOD_FUNC(zj_video_done_p), 0);
}