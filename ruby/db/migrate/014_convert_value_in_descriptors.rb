class ConvertValueInDescriptors < ActiveRecord::Migration
  def self.up
    rename_column(:descriptors, :value, :float_value)
    add_column(:descriptors, :int_value, :integer)
  end

  def self.down
    rename_column(:descriptors, :float_value, :value)
    Descriptor.find(:all).each do |d|
      if d.int_value
        d.update_attribute(:float_value, d.int_value)
      end
    end
    remove_column(:descriptors, :int_value)
  end
end
