#!/usr/bin/env ruby

require 'base/datajockey_base.rb'

median_tempo = DescriptorType.find_or_create_by_name("tempo median")
ave_tempo = DescriptorType.find_or_create_by_name("tempo average")

AnnotationFile.find(:all).each do |a|
  w = a.audio_work
  #does this work already have a tempo tag?
  if !Descriptor.find(:first, :conditions => {:audio_work_id => w.id, :descriptor_type_id => median_tempo.id})
    File.open(a.location, "r"){ |f|
      lines = f.readlines
      if lines.length > 4
        distances = Array.new
        (1..lines.length - 1).each do |i|
          distances << (lines[i].to_f - lines[i - 1].to_f)
        end
        distances.sort!

        #compute median tempo
        d = Descriptor.new(:audio_work_id => w.id, :descriptor_type_id => median_tempo.id)
        d.update_attribute(:float_value, 60.0 / distances[distances.length / 2])

        #compute average tempo
        sum = 0
        distances.each do |d|
          sum += d
        end
        d = Descriptor.new(:audio_work_id => w.id, :descriptor_type_id => ave_tempo.id)
        d.update_attribute(:float_value, 60.0 * distances.length / sum)
      else
        puts "bad annotation file, for #{w.name}.. deleting"
        a.destroy
      end
    }
  end
end
