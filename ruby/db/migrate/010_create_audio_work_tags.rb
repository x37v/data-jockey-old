class CreateAudioWorkTags < ActiveRecord::Migration
  def self.up
    create_table :audio_work_tags do |t|
      t.column :audio_work_id, :integer
      t.column :tag_id, :integer
    end
  end

  def self.down
    drop_table :audio_work_tags
  end
end
