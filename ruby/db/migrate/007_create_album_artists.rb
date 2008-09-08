class CreateAlbumArtists < ActiveRecord::Migration
  def self.up
    create_table :album_artists do |t|
      t.column :album_id, :integer
      t.column :artist_id, :integer
    end
  end

  def self.down
    drop_table :album_artists
  end
end
