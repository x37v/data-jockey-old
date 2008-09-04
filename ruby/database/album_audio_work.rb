class AlbumAudioWork < ActiveRecord::Base
  belongs_to :album
  belongs_to :audio_work
end
