module Datajockey
  class DJMixerChannelModel
    def volume=(val)
      setVolume(val)
    end
    def mute(val = true)
      setMuted(val)
    end
  end
end
