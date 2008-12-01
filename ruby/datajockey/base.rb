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

require 'yaml'
require 'rubygems'
require 'active_record'

[
  "datajockey/database/album_artist", 
  "datajockey/database/album_audio_work", 
  "datajockey/database/album", 
  "datajockey/database/annotation_file", 
  "datajockey/database/artist_audio_work", 
  "datajockey/database/artist", 
  "datajockey/database/artist_role", 
  "datajockey/database/audio_file", 
  "datajockey/database/audio_file_type", 
  "datajockey/database/audio_work", 
  "datajockey/database/audio_work_tag", 
  "datajockey/database/descriptor", 
  "datajockey/database/descriptor_type", 
  "datajockey/database/tag_class", 
  "datajockey/database/tag", 
].each do |m|
  require m
end

#datajockey config stuff
module Datajockey
  @@conf_file = nil
  @@conf = nil
  def Datajockey::setConfFile(file)
    @@conf_file = file
  end
  def Datajockey::config
    unless @@conf
      unless @@conf_file
        raise "config file has not been set"
      end
      @@conf = YAML::load(File.open(@@conf_file))
    end
    return @@conf
  end
  def Datajockey::connect
    unless ActiveRecord::Base.connected?
      if Datajockey::config["database"]
        ActiveRecord::Base.establish_connection(Datajockey::config["database"])
      else
        raise "No database entry in config file, cannot establish database connection."
      end
    end
  end
end

