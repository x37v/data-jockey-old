class CreateAlbums < ActiveRecord::Migration
  def self.up
    create_table :albums do |t|
      t.column :name, :string
      t.column :year, :year
      t.column :tracks, :integer
      t.column :compilation, :bool, :default => 0
    end
  end

  def self.down
    drop_table :albums
  end
end
