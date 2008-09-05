#!/usr/bin/env ruby
require 'fileutils'
require 'datajockey'

tmpWavFileName = "/tmp/datajockey-annotation-tmp.wav"
tmpAnnotationFile = "/tmp/datajockey-annotation-tmp.txt"

if ARGV.length != 2
  throw "need to provide input file and output file"
end

begin
  Datajockey::soundFileToWave(ARGV[0], tmpWavFileName)
  if system("java", "-jar", "../../beatroot-0.5.3.jar", "-o", tmpAnnotationFile, tmpWavFileName)
      `utilities/filter_beat_locs.rb #{tmpAnnotationFile} 10 > #{ARGV[1]}`
  end
rescue 
  print "An error occurred: ",$!, "\n"
end

#require 'open3'
#sox doesn't know flac but does know mp3
#`sox #{ARGV[0]} #{tmpWavFileName}`
#stdin, stdout, stderr = Open3.popen3(*(%w{mplayer -ao pcm -vo null} + [ARGV[0]]))
#stdout.readlines; stderr.readlines
#if File.stat("audiodump.wav")
#  if FileUtils.mv("audiodump.wav", tmpWavFileName)
#    if system("java", "-jar", "../../beatroot-0.5.3.jar", "-o", tmpAnnotationFile, tmpWavFileName)
#      `utilities/filter_beat_locs.rb #{tmpAnnotationFile} 10 > #{ARGV[1]}`
#    end
#  end
#end

#`mplayer -ao pcm -vo null #{ARGV[0]} 2> /dev/null && mv audiodump.wav #{tmpWavFileName} && java -jar ../../beatroot-0.5.3.jar -o #{tmpAnnotationFile} #{tmpWavFileName} && ./filter_beat_locs.rb #{tmpAnnotationFile} 10 > #{ARGV[1]}`
