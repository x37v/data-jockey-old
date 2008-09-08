class AddIndicies < ActiveRecord::Migration
  def self.up
    add_index("artist_audio_works", ["artist_id"])
    add_index("artist_audio_works", ["audio_work_id"])
    add_index("album_audio_works", ["audio_work_id"])
    add_index("album_audio_works", ["album_id"])
    add_index("descriptors", ["audio_work_id"])
    add_index("audio_works", ["audio_file_id"])
    add_index("descriptors", ["descriptor_type_id"])
    add_index("descriptors", ["audio_work_id", "descriptor_type_id"])
    #add_index("descriptors", ["audio_work_id", "descriptor_type_id"])
  end

  def self.down
    remove_index("artist_audio_works", "artist_id") 
    remove_index("artist_audio_works", "audio_work_id") 
    remove_index("album_audio_works", "audio_work_id") 
    remove_index("album_audio_works", "album_id") 
    remove_index("descriptors", "audio_work_id") 
    remove_index("audio_works", "audio_file_id")
    #XXX THIS DOESN'T WORK!
    remove_index("descriptors", "descriptor_type_id") 
    remove_index("descriptors", ["audio_work_id", "descriptor_type_id"])
  end
end
