#!/usr/bin/env ruby

#
#how to run: ruby -I ruby/ ./ruby/annotation/create_annotation.rb /tmp/test.mp3
#

require 'yaml'
require 'annotation/beatlocations'

module Datajockey
  module Annotation
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

      return annotation
    end
  end
end

if __FILE__ == $0
  puts Datajockey::Annotation::createAnnotation(ARGV[0]).to_yaml
end

#puts Datajockey::Annotation::createAnnotation("/tmp/test.mp3").to_yaml
