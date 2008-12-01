=begin
	This file is part of Data Jockey.
	
	Data Jockey is free software: you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation, either version 3 of the License, or (at your
	option) any later version.
	
	Data Jockey is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
	Public License for more details.
	
	You should have received a copy of the GNU General Public License along
	with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
=end

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
