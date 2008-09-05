#!/usr/bin/env ruby

require 'base/datajockey_base.rb'
require 'open3'

spec_centroid = DescriptorType.find_or_create_by_name("spectral centroid average")

class String
  def grabDescriptorHash
    a, bs, c = scan(/\A(.*?)\[(.*?)\]\s:\s(.*?)\Z/).first
    vals = c.split(' ').map{ |f| f.to_f}
    types = bs.split(' ').map { |b| a + b}
    out = Hash.new
    types.size.times {|i|
      out[types[i]] = vals[i]
    }
    return out
  end
end

AnnotationFile.find(:all).each do |a|
  w = a.audio_work
  #does this work already have this tag?
  if !Descriptor.find(:first, :conditions => {:audio_work_id => w.id, :descriptor_type_id => spec_centroid.id})
    puts "computing descriptors for #{w.artists[0].name}'s #{w.name}"
    stdin, stdout, stderr = Open3.popen3("../xtract #{w.audio_file.location}")
    output = stdout.readlines; stderr.readlines
    puts "adding to db"
    output.each do |line|
      line.chomp!
      line.grabDescriptorHash.each do |key,val|
        t = DescriptorType.find_or_create_by_name(key)
        d = w.descriptors.find(:first, :conditions => {:descriptor_type_id => t.id})
        #if it wasn't found then create it
        d = Descriptor.create(:audio_work_id => w.id, :descriptor_type_id => t.id) if !d
        d.update_attribute(:float_value, val)
      end
    end
  end
end
