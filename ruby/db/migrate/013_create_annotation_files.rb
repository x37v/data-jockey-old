class CreateAnnotationFiles < ActiveRecord::Migration
  def self.up
    create_table :annotation_files do |t|
      t.column :audio_work_id, :integer
      t.column :location, :string, :limit => 255
      t.column :created_at, :datetime
      t.column :updated_at, :datetime
    end
  end

  def self.down
    drop_table :annotation_files
  end
end
