#!/usr/bin/env ruby

#
#how to run: ruby -I ruby/ ./ruby/annotation/create_annotation.rb /tmp/test.mp3
#

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
end

module Datajockey
  module Annotation
    def Annotation::set_tag_data(tag_data,attribute,tag)
      if !tag.send(attribute) or tag.send(attribute) =~ /^\s*$/
        return false
      else
        tag_data[attribute] = tag.send(attribute)
      end
    end
    def Annotation::createAnnotation(audioFile)
      annotation = Hash.new
      smoothing = 1
      conf_file = File.join(ENV["HOME"], ".datajockey", "config.yaml")
      if File.exists?(conf_file)
        config = YAML::load(File.open(conf_file))
        if(config["annotation"]["beat locations"]["smoothing"])
          smoothing = config["annotation"]["beat locations"]["smoothing"]
        end
      end

      annotation["beat locations"] = Array.new

      cur = Hash.new
      cur["timepoints"] = beats = getBeatLocations(audioFile)
      cur["mtime"] = cur["ctime"] = DateTime.now
      cur["source"] = "Datajockey::Annotation::getBeatLocations(#{audioFile})"
      annotation["beat locations"] << cur

      cur = Hash.new
      cur["timepoints"] = smoothNumArray(beats, smoothing)
      cur["mtime"] = cur["ctime"] = DateTime.now
      cur["source"] = "Datajockey::Annotation::smoothNumArray(Datajockey::Annotation::getBeatLocations(#{audioFile}), #{smoothing})"
      annotation["beat locations"] << cur
      annotation["tags"] = Hash.new
      begin
        tag = TagFile::File.new(audioFile)
        set_tag_data(annotation,"title",tag)
        set_tag_data(annotation,"artist",tag)
        set_tag_data(annotation["tags"],"genre",tag)
        set_tag_data(annotation,"year",tag)
        if tag.has_attribute("album")
          annotation["album"] = album = Hash.new
          album["name"] = tag.album
          set_tag_data(album,"track",tag)
        end
      rescue TagFile::BadFile
      end

      return annotation
    end
  end
end

if __FILE__ == $0
  puts Datajockey::Annotation::createAnnotation(ARGV[0]).to_yaml
end

#puts Datajockey::Annotation::createAnnotation("/tmp/test.mp3").to_yaml
