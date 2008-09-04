class Album < ActiveRecord::Base
  has_many :album_audio_works, :dependent => :destroy
  has_many :audio_works, :through => :album_audio_works

  has_many :album_artists, :dependent => :destroy
  has_many :artists, :through => :album_artists
end
