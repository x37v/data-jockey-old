class Artist < ActiveRecord::Base
  validates_uniqueness_of :name

  has_many :artist_audio_works, :dependent => :destroy
  has_many :audio_works, :through => :artist_audio_works

  #album representations
  has_many :album_artists, :dependent => :destroy
  has_many :albums, :through => :album_artists
end
