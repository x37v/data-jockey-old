class CreateAudioWorks < ActiveRecord::Migration
  def self.up
    create_table :audio_works do |t|
      t.column :audio_file_id, :integer
      t.column :name, :string
      t.column :year, :date
    end
  end

  def self.down
    drop_table :audio_works
  end
end
