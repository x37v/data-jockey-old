require 'datajockey_utils'
require 'tempfile'
require 'fileutils'

module Datajockey
  module Annotation
    @@beatroot_jarfile_loc = nil
    def Annotation::getBeatLocations(audioFile)
      unless @@beatroot_jarfile_loc
        beat_root_search = [
          "lib/beatroot-0.5.3.jar",
          "/usr/local/share/datajockey/beatroot-0.5.3.jar",
          "/usr/local/share/datajockey/beatroot.jar",
          "/usr/share/datajockey/beatroot-0.5.3.jar",
          "/usr/share/datajockey/beatroot.jar",
        ]
        #find beatroot jar file!
        beat_root_search.each do |p|
          if File.exists?(p)
            @@beatroot_jarfile_loc = p
            break
          end
        end
        if not @@beatroot_jarfile_loc
          raise "cannot find beatroot jarfile which #{self.name}::getBeatLocations depends on"
        end
      end

      a = Tempfile.new("datajockeybeats")
      tempwav = a.path + ".wav"
      begin
        Datajockey_utils::soundFileToWave(audioFile, tempwav)
        if ENV["DATAJOCKEY_JAVA_OPTS"]
          unless system("java", ENV["DATAJOCKEY_JAVA_OPTS"], "-jar", @@beatroot_jarfile_loc, "-o", a.path, tempwav)
            raise "Cannot execute beatroot, is the jar file in a known location?"
          end
        else
          unless system("java", "-jar", @@beatroot_jarfile_loc, "-o", a.path, tempwav)
            raise "Cannot execute beatroot, is the jar file in a known location?"
          end
        end
      rescue => e
        raise e
      ensure
        if File.exists?(tempwav)
          FileUtils.rm(tempwav)
        end
      end
      return a.readlines.collect{|l| l.to_f}
    end

    def Annotation::smoothNumArray(a, numloops = 1)
      res = Array.new(a)
      numloops.times do
        for i in 1..(res.length - 2) do
          #find the mid point
          mid = (res[i + 1] - res[i - 1]) / 2.0 + res[i - 1]
          #find the difference of the mid point and the actual point
          dif = mid - res[i]
          #add 1/2 of the difference to the actual point
          res[i] += (dif / 2.0)
        end
      end
      return res
    end

  end
end

#puts Datajockey::Annotation::smoothNumArray(Datajockey::Annotation::getBeatLocations("/tmp/test.mp3"), 10).join("\n")
