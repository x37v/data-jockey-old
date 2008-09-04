class AudioWorkTag < ActiveRecord::Base
  belongs_to :audio_work
  belongs_to :tag
end
