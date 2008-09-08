class CreateTagClasses < ActiveRecord::Migration
  def self.up
    create_table :tag_classes do |t|
      t.column :name, :string
    end
  end

  def self.down
    drop_table :tag_classes
  end
end
