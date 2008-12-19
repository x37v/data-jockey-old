=begin
#this class provides a very simple log of the works that are currently audible
#this is how I use it:
al = AudibleLogger.new
alt = Thread.new { al.start; loop{ sleep(0.01); al.update}}
alt.kill; al.stop
puts al.works.join("\n")
=end

class AudibleLogger
  attr_reader :start_time, :stop_time, :works
  def initialize
    start
  end
  def start
    @works = Array.new
    @last_audible = Array.new
    @stop_time = nil
    @start_time = Time.now
  end
  def update
    audible = Datajockey::ApplicationModel.instance.mixerPanel.audibleWorks
    #find those that are now audible
    (audible - @last_audible).each do |w_id|
      @works << [w_id, :start, Time.now]
    end
    #find those that are no longer audible
    (@last_audible - audible).each do |w_id|
      @works << [w_id, :stop, Time.now]
    end
    @last_audible = audible
  end
  def stop
    #find those that are no longer audible
    @last_audible.each do |w_id|
      @works << [w_id, :stop, Time.now]
    end
    @stop_time = Time.now
  end
end
