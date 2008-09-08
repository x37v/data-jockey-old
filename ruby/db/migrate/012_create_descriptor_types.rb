class CreateDescriptorTypes < ActiveRecord::Migration
  def self.up
    create_table :descriptor_types do |t|
      t.column :name, :string
    end
  end

  def self.down
    drop_table :descriptor_types
  end
end
