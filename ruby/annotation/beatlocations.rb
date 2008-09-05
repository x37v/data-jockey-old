require 'datajockey_utils'
require 'tempfile'
require 'fileutils'

module Datajockey
  module Analysis
    @@beatroot_jarfile_loc = nil
    def Analysis::getBeatLocations(audioFile)
      unless @@beatroot_jarfile_loc
        #find beatroot jar file!
        if File.exists?("lib/beatroot-0.5.3.jar")
          @@beatroot_jarfile_loc = "lib/beatroot-0.5.3.jar"
        elsif File.exists?("../lib/beatroot-0.5.3.jar")
          @@beatroot_jarfile_loc = "../lib/beatroot-0.5.3.jar"
        elsif File.exists?("../../lib/beatroot-0.5.3.jar")
          @@beatroot_jarfile_loc = "../../lib/beatroot-0.5.3.jar"
        elsif File.exists?("/usr/local/share/datajockey/beatroot.jar")
          @@beatroot_jarfile_loc = "/usr/local/share/datajockey/beatroot.jar"
        elsif File.exists?("/usr/share/datajockey/beatroot.jar")
          @@beatroot_jarfile_loc = "/usr/share/datajockey/beatroot.jar"
        else
          raise "cannot find beatroot jarfile which #{self.name}::getBeatLocations depends on"
        end
      end

      a = Tempfile.new("datajockeybeats")
      tempwav = a.path + ".wav"
      begin
        Datajockey_utils::soundFileToWave(audioFile, tempwav)
        unless system("java", "-jar", @@beatroot_jarfile_loc, "-o", a.path, tempwav)
          raise "Cannot execute beatroot, is the jar file in a known location?"
        end
      ensure
        FileUtils.rm(tempwav)
      end
      a.readlines.collect{|l| l.to_f}
    end
  end
end

#puts Datajockey::Analysis::getBeatLocations("/tmp/test.mp3").join("\n")
