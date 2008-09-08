class CreateArtistAudioWorks < ActiveRecord::Migration
  def self.up
    create_table :artist_roles do |t|
      t.column :name, :string
    end

    ArtistRole.create :name => 'composer'
    ArtistRole.create :name => 'performer'

    create_table :artist_audio_works do |t|
      t.column :artist_id, :integer
      t.column :audio_work_id, :integer
      t.column :artist_role_id, :integer
    end
  end

  def self.down
    drop_table :artist_audio_works
    drop_table :artist_roles
  end
end
