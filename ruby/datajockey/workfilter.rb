=begin

  Copyright (c) 2008 Alex Norman.  All rights reserved.
	http://www.x37v.info/datajockey

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

#redefining the work filter init method
#we create a proc that calls our 'works' method
#and pass this proc to the original initialize method
module Datajockey
  class WorkFilter
    alias :old_init :initialize
    def initialize(name, description = "Description of WorkFilter")
      this_obj = self
      @proc = Proc.new { 
        clearWorks
        this_obj.works.each do |i|
          if i.is_a?(Integer)
            addWork(i)
          end
        end
      }
      old_init(name, description, @proc)
    end
    #overload this function in order to get your filters to work
    def works
      return []
    end
  end
end

=begin
#this is an example filter that simply returns a random number
class RandFilter < Datajockey::WorkFilter
  def initialize
    super("Random Filter", "This Filter gives one Random Work.")
  end
  def works
    return [rand(400)]
  end
end
=end
