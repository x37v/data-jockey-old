class ArtistAudioWork < ActiveRecord::Base
  belongs_to :artist
  belongs_to :audio_work
  belongs_to :artist_role
end
