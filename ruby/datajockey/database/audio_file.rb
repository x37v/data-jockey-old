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

module Datajockey
  class AudioFile < ActiveRecord::Base
    belongs_to :audio_file_type
    validates_uniqueness_of :location

    #Gives us the lenght of the audio file in seconds.
    def seconds
      return milliseconds.to_f * 1e-3
    end

    #  def initialize(attributes = nil)
    #    super(attributes)
    #    self.added = DateTime.now
    #  end
  end
end
