class CreateFileIdTable < ActiveRecord::Migration
  def self.up
		create_table :audio_files do |t|
      t.column :audio_file_type_id, :integer
      t.column :location, :string, :limit => 255
      t.column :milliseconds, :integer
      t.column :channels, :integer, :default => 2
      t.column :created_at, :datetime
      t.column :updated_at, :datetime
    end

		create_table :audio_file_types do |t|
      t.column :name, :string
    end

    AudioFileType.create :name => 'mp3'
    AudioFileType.create :name => 'ogg'
    AudioFileType.create :name => 'flac'
    AudioFileType.create :name => 'wav'
    AudioFileType.create :name => 'aiff'
  end

  def self.down
    drop_table :audio_files
    drop_table :audio_file_types
  end
end
