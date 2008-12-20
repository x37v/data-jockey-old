#!/usr/bin/env ruby

require 'score'
require 'gtk2'
require 'cairo'
require 'optparse'

TIME_SCALE = 5
TEXT_SIZE = 5
HEADER_TEXT_SIZE = 10

class Song
  attr_reader :fill_color, :line_color, :text_color
  attr_accessor :max_volume
  alias :old_init :initialize

  def initialize(work_id = nil, text = "title")
    old_init(text)
    initialize_graphical
  end

  def initialize_graphical
    @graphics_inited = true 
    @fill_color = [rand,rand,rand] unless @fill_color
    @line_color = [0,0,0] unless @line_color
    @text_color = @fill_color.map{|x| 1.0 - x} unless @text_color
    @draw_text_box = false unless @draw_text_box
    @max_volume = 1.5 unless @max_volume
  end

  def set_fill_color(r,g,b)
    @fill_color = [r,g,b]
  end

  def set_line_color(r,g,b)
    @line_color = [r,g,b]
  end

  def set_text_color(r,g,b)
    @text_color = [r,g,b]
  end

  def draw(c) #c = draw context
    initialize_graphical unless @graphics_inited
    #store the matrix so that we can clean up later
    matrix = c.matrix
    vol_scale = 100 / @max_volume
    c.translate(0,100)
    c.scale(1,-1)

    #draw the volume envelope
    @volume_envelope.each do |x,y|
      y = @max_volume if y > @max_volume
      c.line_to(TIME_SCALE * x, vol_scale * y)
    end

    #finish up the volume envelope [keep volume constant until end]
    c.line_to(TIME_SCALE * @length, vol_scale * @volume_envelope[-1][1])
    #draw down
    c.line_to(TIME_SCALE * @length, 0)
    #draw back to beg
    c.line_to(0, 0)
    c.close_path

    c.set_source_rgb(*@fill_color)
    c.fill_preserve
    c.set_source_rgb(*@line_color)
    c.set_line_join(Cairo::LINE_JOIN_MITER)
    c.set_line_width(1)
    c.stroke

    c.move_to(5,5)
    c.scale(1,-1)

    text = @text

    if @draw_text_box
      extents = c.text_extents(text)
      c.set_source_rgb(*(@text_color.map{|x| 1.0 - x}))
      c.rectangle(0,0,extents.width + 1,-extents.height)
      c.fill
    end

    c.set_source_rgb(*@text_color)
    c.show_text(text)
    c.stroke

    #clean up transforms
    c.matrix = matrix
  end
end

class Track
  def draw(c) #c = cairo context
    @songs.each do |time, song|
      matrix = c.matrix
      c.translate(TIME_SCALE * time,0)
      song.draw(c)
      c.matrix = matrix
    end
  end
end

class Score
  attr_accessor :display_time_inc, :background_color, :text_color

  alias :old_init :initialize
  class << self
    alias :old_from_yaml_string :from_yaml_string
  end

  def initialize
    old_init
    initialize_graphical
  end

  #initialize the graphical member data
  def initialize_graphical
    @background_color = [1,1,1] unless @background_color
    @text_color = [0,0,0] unless @text_color
    @display_time_inc = 10 unless @display_time_inc
  end

  #calculate the draw size
  def draw_size
    tracks = @track.get_non_overlapping_tracks
    return [100 + 40 + TIME_SCALE * @track.length, 40 + 100 * tracks.length]
  end

  #draw onto the context(c)
  def draw(c)
    tracks = @track.get_non_overlapping_tracks
    matrix = c.matrix

    # fill background 
    c.set_source_rgba(*@background_color)
    c.paint

    c.translate(10,10)
    header = @title
    if @creation_time
      header = header + " created " + @creation_time.strftime("%I:%M %p %d %B %Y")
    end
    if @author
      header = header + " by #{@author}"
    end
    c.set_font_size(HEADER_TEXT_SIZE)
    c.set_source_rgb(@text_color)
    c.show_text(header)

    c.matrix = matrix

    c.translate(20,20)
    #draw second/minute ticks
    matrix = c.matrix
    (0...@track.length.to_i + 1).each do |i|
      if i % 60 == 0
        c.set_source_rgb(0,0,0)
      else
        c.set_source_rgb(0.5,0.5,0.5)
      end
      if i % @display_time_inc == 0
        time_str = "%02d:%02d" % [(i/60), (i % 60)]
        extents = c.text_extents(time_str)
        x = 0.5 - (extents.width / 2 + extents.x_bearing)
        y = 0.5 - (extents.height / 2 + extents.y_bearing)

        c.move_to(x, y + 5)
        c.show_text(time_str)
        c.rectangle(-0.5,10,0.5,tracks.length * 100)
        c.fill
      end
      #c.rectangle(-0.5,10,0.5,tracks.length * 100)
      #c.fill
      c.translate(TIME_SCALE,0)
    end
    c.matrix = matrix
    c.translate(0,10)
    tracks.each do |track|
      track.draw(c)
      c.translate(0,100)
    end
    c.matrix = matrix
  end

  def Score::from_yaml_string(input_string)
    ob = Score::old_from_yaml_string(input_string)
    ob.initialize_graphical
    return ob
  end
end

def gen_pdf(score, pdf_out_file)
  pdf = Cairo::PDFSurface.new(pdf_out_file, *score.draw_size)
  cr = Cairo::Context.new(pdf)
  cr.select_font_face("Sans",
                      Cairo::FONT_SLANT_NORMAL,
                      Cairo::FONT_WEIGHT_NORMAL)

  cr.set_font_size(TEXT_SIZE)
  score.draw(cr)
end

def gen_ps(score, ps_out_file)
  ps = Cairo::PSSurface.new(ps_out_file, *score.draw_size)
  cr = Cairo::Context.new(ps)
  cr.select_font_face("Sans",
                      Cairo::FONT_SLANT_NORMAL,
                      Cairo::FONT_WEIGHT_NORMAL)

  cr.set_font_size(TEXT_SIZE)
  score.draw(cr)
end

def gtk_score(score)
  w = Gtk::Window.new
  sw = Gtk::ScrolledWindow.new
  da = Gtk::DrawingArea.new
  sw.add_with_viewport(da)
  w.add(sw)

  #vp.hadjustment.value, vp.vadjustment.value = score.draw_size
  da.set_size_request(*score.draw_size)

  da.signal_connect('expose-event') { |widget, event|
    cr = widget.window.create_cairo_context
    #cr.clear
    cr.select_font_face("Sans",
                        Cairo::FONT_SLANT_NORMAL,
                        Cairo::FONT_WEIGHT_NORMAL)

    cr.set_font_size(TEXT_SIZE)
    score.draw(cr)
  }

  w.signal_connect("destroy"){Gtk.main_quit}
  w.show_all
  w.set_size_request(400,400)
  Gtk.main
end

if __FILE__ == $0

  #############################
  # options!
  #############################

  play_list_file = nil
  pdf_file_name = nil
  ps_file_name = nil
  show_gtk = false
  show_text = false

  opts = OptionParser.new do |opts|
    # Mandatory argument.
    #opts.on("-i", "--input xml_playlist",
      #"An xml playlist file, generated by Data Jockey") do |file|
      #play_list_file = file
#
      #if not File.exists?(play_list_file)
        #STDERR.puts "** error, #{play_list_file} does not exist **"
        #STDERR.puts "\n"
        #STDERR.puts opts
        #exit
      #end
#
    #end

    opts.on("-p", "--ps filename",
      "generate a post script playlist and store it as filename") do |file|
      if File.exists?(file)
        STDERR.puts "ps output file #{file} exists, do you want to overwrite it? [yes/no]"
        responce = STDIN.readline.chomp
        responce.gsub!(/\s*/,"")
        if not (responce == "y" || responce == "yes")
          STDERR.puts "** ps generation aborted **"
          exit
        end
      end
      ps_file_name = file
      end

    opts.on("-d", "--pdf filename",
      "generate a pdf playlist and store it as filename") do |file|
      if File.exists?(file)
        STDERR.puts "pdf output file #{file} exists, do you want to overwrite it? [yes/no]"
        responce = STDIN.readline.chomp
        responce.gsub!(/\s*/,"")
        if not (responce == "y" || responce == "yes")
          STDERR.puts "** pdf generation aborted **"
          exit
        end
      end
      pdf_file_name = file
      end

    # Boolean switch.
    opts.on("-g", "--gtk", "show a gtk window with the playlist") do |g|
      show_gtk = g
    end

    # Boolean switch.
    opts.on("-t", "--text", "dump a text playlist to stdout") do |t|
      show_text = t
    end

    # No argument, shows at tail.  This will print an options summary.
    opts.on_tail("-h", "--help", "Show this message") do
      STDERR.puts opts
      exit
    end
  end
  opts.banner = "Usage: #{$0} [options] playlistfile.yaml"

  begin
    opts.parse!(ARGV)
  rescue Option_parser::Invalid_option 
    STDERR.puts $!
    STDERR.puts opts
    exit
  end

  play_list_file = ARGV[0]

  if not play_list_file
    STDERR.puts "you must supply a valid yaml input file"
    STDERR.puts opts
    exit
  elsif not File.file?(play_list_file)
    STDERR.puts "** error, #{play_list_file} is not a readable file **"
    STDERR.puts "\n"
    STDERR.puts opts
    exit
  end

  score = Score.from_yaml_file(play_list_file)

  if show_text
    puts score.to_s
  end

  #generate pdf
  if pdf_file_name
    gen_pdf(score,pdf_file_name)
  end

  #generate post script
  if ps_file_name
    gen_ps(score,ps_file_name)
  end

  if show_gtk
    gtk_score(score)
  end
end
