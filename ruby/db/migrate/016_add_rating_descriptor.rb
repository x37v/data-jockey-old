class AddRatingDescriptor < ActiveRecord::Migration
  def self.up
    #create a 'rating' descriptor type
    DescriptorType.find_or_create_by_name("rating")
  end

  def self.down
    r = DescriptorType.find_by_name("rating")
    if r
      r.descriptors.destroy_all
      r.destroy
    end
  end
end
