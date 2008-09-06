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

