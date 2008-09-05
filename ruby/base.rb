require 'yaml'
require 'rubygems'
require 'active_record'
[
  "database/album_artist", 
  "database/album_audio_work", 
  "database/album", 
  "database/annotation_file", 
  "database/artist_audio_work", 
  "database/artist", 
  "database/artist_role", 
  "database/audio_file", 
  "database/audio_file_type", 
  "database/audio_work", 
  "database/audio_work_tag", 
  "database/descriptor", 
  "database/descriptor_type", 
  "database/tag_class", 
  "database/tag", 
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
end

