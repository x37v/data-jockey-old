class CreateDescriptors < ActiveRecord::Migration
  def self.up
    create_table :descriptors do |t|
      t.column :descriptor_type_id, :integer
      t.column :audio_work_id, :integer
      t.column :value, :float
    end
  end

  def self.down
    drop_table :descriptors
  end
end
