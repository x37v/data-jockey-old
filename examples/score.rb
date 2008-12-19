require 'yaml'

#a song is basically some text and a volume envelope
#it can include the work_id if you want [which can be used later]
class Song
  attr_reader :length, :work_id, :volume_envelope
  attr_accessor :text, :work_id

  def initialize(work_id = nil, text = "title")
    @volume_envelope = Array.new
    @length = 1
    @work_id = work_id
    @text = text
  end

  def update_envelope(time, value, time_point = nil)
    if time > @length
      @length = time
    end
    @volume_envelope << [time,value,time_point]
  end

 # def to_h
 #   @volume_envelope.sort!
 #   h = Hash.new
 #   h[:text] = @text
 #   h[:work_id] = @work_id
 #   h[:volume_envelope] = @volume_envelope
 #   return h
 # end
end

#a track is a collection of songs over time
class Track
  attr_reader :songs

  def initialize
    @songs = Array.new
  end

  #find the length of the track
  def length
    maxlen = 0
    @songs.each do |time, song|
      if time + song.length > maxlen
        maxlen = time + song.length
      end
    end
    return maxlen
  end

  #add a song to the track at the time given
  def add_song(time, song)
    @songs << [time, song]
  end

  #get an array of tracks where none of the songs overlap, 
  #gather the data from this track
  def get_non_overlapping_tracks
    @songs.sort! {|a,b| a[0] <=> b[0]}
    t1 = Track.new
    t2 = Track.new
    @songs.each do |time, song|
      if t1.songs.empty?
        t1.add_song(time,song)
        next
      end
      #if there is no overlap then add it to t1
      #no overlap lastSongStart + lastSongLen <= time
      if t1.songs[-1][0] + t1.songs[-1][1].length <= time
        t1.add_song(time,song)
      else
        t2.add_song(time,song)
      end
    end
    if t2.songs.length == 0
      return [t1]
    else
      return [t1] + t2.get_non_overlapping_tracks
    end
  end

#  #convert the track to a hash 
#  def to_h
#    h = Hash.new
#    h[:songs] = @songs
#  end
end

class Score
  attr_accessor :author, :title, :creation_time

  #create a score, give an optional title [which can be changed later]
  def initialize(title = "score")
    @track = Track.new
    @title = title
    @creation_time = Time.now
  end

  #add a song to this score, at the time given
  def add_song(time,song)
    @track.add_song(time,song)
  end

  #create a string representation of this score, for printing
  def to_s
    out = String.new
    t = @title
    t = t + " created #{@creation_time}" if @creation_time
    t = t + " by #{@author}" if @author
    out << t + "\n"
    @track.songs.sort.each do |t,s|
      start_time = "%02d:%02d" % [(t.to_i/60), (t.to_i % 60)]
      end_time = "%02d:%02d" % [((t + s.length).to_i/60), ((t + s.length).to_i % 60)]
      out << "#{s.text} [#{start_time} - #{end_time}]\n"
    end
    return out
  end

  #return a score object from the given input string [yaml]
  def Score.from_yaml_string(input_string)
    newob = YAML::load(input_string)
    if newob.class == Score
      return newob
    else
      raise "input data does not describe a valid Score object"
    end
  end

  #return a score object from the given input file [yaml]
  def Score.from_yaml_file(input_file)
    return Score.from_yaml_string(File.open(input_file))
  end
end
