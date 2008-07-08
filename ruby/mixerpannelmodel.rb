module Datajockey
  class MixerPanelModel
    def audibleMixers
      mixers = []
      self.numMixers.times { |i|
        if self.mixerAudible(i)
          mixers << i
        end
      }
      return mixers
    end
    def audibleWorks
      self.audibleMixers.collect { |i| self.mixer(i).work }
    end
  end
end
