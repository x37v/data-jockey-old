class CreateTags < ActiveRecord::Migration
  def self.up
    create_table :tags do |t|
      t.column :tag_class_id, :integer
      t.column :name, :string
    end
  end

  def self.down
    drop_table :tags
  end
end
