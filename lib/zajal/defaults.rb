module Zajal
  module Events
    module Internals
      # FIXME the kludges keep on coming...
      @initial_defaults = {
        size: [500, 500],
        title: ["Zajal"],
        fullscreen: [false],
        cursor: [true],
        framerate: [30],
        vertical_sync: [true],
        smoothing: [false],
        alpha_blending: [true],
        color: [240],
        background: [32],
        background_auto: [true],
        circle_resolution: [32],
        rectangle_mode: [:corner],
        line_width: [1.0],
        fill: [true]
      }
      
      @defaults = nil
      
      def self.reset_defaults
        @defaults = @initial_defaults.clone
      end
      
      reset_defaults
      
      @defaults_proc = Proc.new do
        # FIXME size and background are problematic, removed!
        %w(background title fullscreen cursor framerate vertical_sync smoothing
           size alpha_blending color background_auto circle_resolution
           rectangle_mode line_width fill).each do |meth|
             eval "#{meth}(*@defaults[:#{meth}])"
           end
           
      end
      
    end
  end
end