require 'score'

class ScoreLogger
  attr_reader :score
  attr_accessor :poll_period
  def initialize
    @score = Score.new
    @last_audible = Array.new
    @stop_time = nil
    @start_time = Time.now
    @current_songs = Hash.new
    @poll_period = 0.1
  end
  def start
    @score = Score.new
    @last_audible = Array.new
    @stop_time = nil
    @start_time = Time.now
    @current_songs = Hash.new
    me = self
    @mythread = Thread.new{ loop{ update; sleep @poll_period} }
  end
  def stop
    @mythread.kill
    #find those that are no longer audible
    @last_audible.each do |index|
      @current_songs[index] = nil
    end
    @stop_time = Time.now
  end
  protected
  def update
    mixer_panel = Datajockey::ApplicationModel.instance.mixerPanel
    audible = mixer_panel.audibleMixers
    #now, based on the start time
    time = (Time.now - @start_time)

    #find those that are newly audible
    (audible - @last_audible).each do |index|
      mixer = mixer_panel.mixer(index)
      work = AudioWork.find_by_id(mixer.work)
      text = ""
      if work
        artist = work.artists[0]
        if artist
          if work.album
            text = "#{artist.name} | #{work.name} | #{work.album.name}"
          else
            text = "#{artist.name} | #{work.name}"
          end
        else
          text = work.name
        end
      end
      newsong = Song.new(mixer.work, text)
      @current_songs[index] = [newsong, time]
      @score.add_song(time, newsong)
    end

    #remove those no longer audible
    (@last_audible - audible).each do |index|
      @current_songs[index] = nil
    end

    #update the envelope
    audible.each do |index|
      if @current_songs[index]
        mytime = time - @current_songs[index][1]
        mixer = mixer_panel.mixer(index)
        #update the envelope, the current time offset
        @current_songs[index][0].update_envelope(mytime, mixer_panel.mixerVolume(index), mixer.control.progress)
      end
    end

    @last_audible = audible
  end
end
