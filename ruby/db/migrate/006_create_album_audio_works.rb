class CreateAlbumAudioWorks < ActiveRecord::Migration
  def self.up
    create_table :album_audio_works do |t|
      t.column :album_id, :integer
      t.column :audio_work_id, :integer
      t.column :track, :integer
    end
  end

  def self.down
    drop_table :album_audio_works
  end
end
