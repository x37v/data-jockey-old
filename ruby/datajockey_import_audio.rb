#!/usr/bin/env ruby

require 'base'
require 'rubygems'
require 'active_record'
require 'yaml'
require 'annotation/create_annotation'

module Datajockey
  def Datajockey::importAudioFile(audioFile, title = nil, artist_name = nil, album_name = nil, album_track = nil)
    unless File.exists?(audioFile)
      raise "#{audioFile} does not exist, aborting import"
    end

    unless ActiveRecord::Base.connected?
      if Datajockey::config["database"]
        ActiveRecord::Base.establish_connection(Datajockey::config["database"])
      else
        raise "No database entry in config file, cannot establish database connection."
      end
    end
    unless Datajockey::config["annotation"]["files"]
      raise 'Datajockey::config["annotation"]["files"] is not set'
    end
    unless File.directory?(Datajockey::config["annotation"]["files"]) and
      File.writable?(Datajockey::config["annotation"]["files"])
      raise 'Datajockey::config["annotation"]["files"] is not a valid, writable directory'
    end

    #make sure we haven't already imported this
    if !AudioFile.find_by_location(audioFile)
      annotation =  Datajockey::Annotation::createAnnotation(audioFile)
      title = annotation["title"] unless title
      artist_name = annotation["artist"] unless artist_name
      album_name = annotation["album"]["name"] unless album_name
      album_track =  annotation["album"]["track"] unless album_track

      #create the audio file
      audio_file = AudioFile.find_or_create_by_location(audioFile)
      #store the time in milliseconds
      audio_file.milliseconds = annotation["seconds"] * 1000

      #try to figure out the file type
      file_extension = audioFile.slice(/\w+$/)
      aft = AudioFileType.get_valid(file_extension)
      audio_file.audio_file_type = aft if aft
      audio_file.channels = annotation["channels"] if annotation["channels"]
      audio_file.save

      #create the work
      work = AudioWork.create(:name => title, :audio_file => audio_file)

      if annotation["tags"]
        annotation["tags"].each do |class_name, t|
          tag_class = TagClass.find_or_create_by_name(class_name)
          #make an array just to reduce code
          t = [t] unless t.is_a?(Array)
          t.each do |tag|
            work.tags << Tag.find_or_create_by_name_and_tag_class_id(tag, tag_class.id)
          end
        end
      end
      artist = nil
      if artist_name
        #find or create the artist
        artist = Artist.find_or_create_by_name(artist_name)

        artist_work = ArtistAudioWork.find_or_create_by_audio_work_id(work.id)
        artist.artist_audio_works << artist_work unless artist.artist_audio_works.member?(artist_work)
      end
      if album_name
        #create or find the album
        album = Album.find_or_create_by_name(album_name)
        #insert the work into the album
        album.audio_works << work
        #set the track number
        work.album_audio_work.update_attribute(:track, album_track) if album_track
        album.artists << artist if artist
      end
      #create the annotation file info
      #write the annotation file
      annotation_file = AnnotationFile.find_or_create_by_audio_work_id(work.id)
      location = File.join(Datajockey::config["annotation"]["files"], work.id.to_s + ".yaml")
      annotation_file.update_attribute(:location, location)
      File.open(location, "w") { |f| f.print annotation.to_yaml }
    end
  end
end

if __FILE__ == $0
  Datajockey::setConfFile(File.join(ENV["HOME"], ".datajockey", "config.yaml"))
  badFiles = []
  ARGV.each do |f|
    begin
      puts "attempting import of #{f}"
      Datajockey::importAudioFile(f)
      puts "success"
    rescue
      puts $!
      badFiles << f
    end
  end
end
