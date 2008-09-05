#!/usr/bin/env ruby

#
#how to run: ruby -I ruby/ ./ruby/annotation/create_annotation.rb /tmp/test.mp3
#

require 'base'
require 'rubygems'
require 'rtaglib'
require 'yaml'
require 'annotation/beatlocations'

class TagFile::File
  def has_attribute(name)
    if !self.send(name) or self.send(name) =~ /^\s*$/
      return false
    else
      return true
    end
  end
  def store_attribute_in_hash(hash, attr_name, newname = nil)
    name = attr_name
    if newname
      name = newname
    end
    if has_attribute(attr_name)
      hash[name] = self.send(attr_name)
    else
      return false
    end
  end
end

module Datajockey
  module Annotation
    def Annotation::createAnnotation(audioFile)
      annotation = Hash.new
      smoothing = 1
      if(Datajockey::config["annotation"]["beat locations"]["smoothing"])
        smoothing = Datajockey::config["annotation"]["beat locations"]["smoothing"]
      end

      annotation["beat locations"] = Array.new

      cur = Hash.new
      cur["timepoints"] = beats = getBeatLocations(audioFile)
      cur["mtime"] = cur["ctime"] = Time.now
      cur["source"] = "Datajockey::Annotation::getBeatLocations(#{audioFile})"
      annotation["beat locations"] << cur

      cur = Hash.new
      cur["timepoints"] = smoothNumArray(beats, smoothing)
      cur["mtime"] = cur["ctime"] = DateTime.now
      cur["source"] = 
      "Datajockey::Annotation::smoothNumArray(Datajockey::Annotation::getBeatLocations(#{audioFile}), #{smoothing})"
      annotation["beat locations"] << cur

      #get id3tag data from file if it exists
      begin
        tag = TagFile::File.new(audioFile)
        tag.store_attribute_in_hash(annotation,"title")
        tag.store_attribute_in_hash(annotation,"artist")
        tag.store_attribute_in_hash(annotation,"year")
        annotation["channels"] = tag.channels.to_i if tag.has_attribute("channels")
        annotation["seconds"] = tag.length.to_i if tag.has_attribute("length")

        #genre is a tag
        annotation["tags"] = Hash.new
        tag.store_attribute_in_hash(annotation["tags"],"genre")

        #album
        if tag.has_attribute("album")
          annotation["album"] = album = Hash.new
          album["name"] = tag.album
          album["track"] = tag.track.to_i if tag.has_attribute("track")
        end
      rescue TagFile::BadFile
      end

      return annotation
    end
  end
end

if __FILE__ == $0
  Datajockey::setConfFile(File.join(ENV["HOME"], ".datajockey", "config.yaml"))
  puts Datajockey::Annotation::createAnnotation(ARGV[0]).to_yaml
end

#puts Datajockey::Annotation::createAnnotation("/tmp/test.mp3").to_yaml
