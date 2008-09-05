#XXX should test for other locations
conf_file = File.join(ENV["HOME"], ".datajockey", "config.yaml")

#eventually it would be nice to test to see if we're in the graphical application

begin
  require 'datajockey'
rescue LoadError 
  STDERR.puts "\n\n*******************************************"
  STDERR.puts "cannot load DataJockey library for Ruby, make sure you have it installed"
  STDERR.puts "ruby intepreter will not execute properly"
  STDERR.puts "*******************************************\n\n"
  loop{ sleep(1) }
end

externallibs = [
  'irb',
  'rubygems',
  'active_record',
  'yaml',
]

djclassfiles = [
  "applicationmodel",
  "interpreter",
  "mixerchannelmodel",
  "mixerpannelmodel",
  "workfilter",
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
]

#before we install the files are in "ruby"
if File.directory?("ruby") and File.exists?("ruby/applicationmodel.rb")
  djclassfiles = djclassfiles.collect {|f| File.join("ruby", f)}
else
  djclassfiles = djclassfiles.collect {|f| File.join("datajockey", f)}
end

#so we can see what it is and print the error
curlib = nil
begin
  externallibs.each do |lib|
    curlib = lib
    require lib
  end
  djclassfiles.each do |lib|
    curlib = lib
    require lib
  end
rescue LoadError
  STDERR.puts "\n\n*******************************************"
  STDERR.puts "cannot load ruby library \"#{curlib}\" which datajockey requires"
  STDERR.puts "ruby intepreter will not execute properly"
  STDERR.puts "*******************************************\n\n"
  loop{ sleep(1) }
end

# mercilessly borrowed from: http://errtheblog.com/posts/9-drop-to-irb
#
# I've cleaned up a few things and moved some stuff around, not tested at all of course.
#
# Usage:
#
# raggi@mbk:~/dev/gosu$ ruby -rraggi/irb/drop -e "puts 'hi'; dROP! []; puts 'bye'"hi
# Helper Methods: continue, instance, meths, quit!
# Variables: object # => [], binding # => #<Binding:0x39c9c>
# >> self.concat [:foo,:bar,:baz]
# => [:foo, :bar, :baz]
# >> reverse!
# => [:baz, :bar, :foo]
# >> continue
# bye

class RedirectOutput < IO
    def initialize
        super(2)  
    end
    def write(text)
      Datajockey::InterpreterIOProxy::addToOutput(text.to_s)
    end
end

#redirect stdout
$stdout = RedirectOutput.new

#here we read from datajockey 
class DataJockeyInput < IRB::InputMethod
  def gets()
    until Datajockey::InterpreterIOProxy::newInput
      sleep(0.001)
    end
    input = Datajockey::InterpreterIOProxy::getInput + "\n"
    raise RubyLex::TerminateLineInput if input =~ /DATAJOCKEY_CANCEL_INPUT/
    return input
  end
  def readable_atfer_eof?()
    true
  end
end

module IRB
  class <<self
    attr_accessor :instance
  end

  def self.set_binding(binding)
    old_args = ARGV
  	ARGV.size.times { ARGV.shift }
    
    IRB.setup(nil)

    workspace = WorkSpace.new(binding)

    if @CONF[:SCRIPT]
      self.instance = Irb.new(workspace, @CONF[:SCRIPT])
    else
      self.instance = Irb.new(workspace, DataJockeyInput.new)
      #self.instance = Irb.new(workspace)
    end

    @CONF[:IRB_RC].call(self.instance.context) if @CONF[:IRB_RC]
    @CONF[:MAIN_CONTEXT] = self.instance.context

    trap 'INT' do
      self.instance.signal_handle
    end
  ensure
    old_args.each { |a| ARGV << a }
  end
  
  def self.start_session(binding)
    old_args = ARGV
  	ARGV.size.times { ARGV.shift }

    set_binding(binding)

    catch(:IRB_EXIT) do
      self.instance.eval_input
    end
  ensure
    old_args.each { |a| ARGV << a }
  end
end

module IRBHelper
  def meths(o); puts (o.methods.sort - Class.new.methods).join("\n"); end
  #def quit!; irb_exit; ::Process.exit!; end
  #def instance; IRB.instance; end
  #def continue; irb_exit; end
end

if defined? IRBHelper
  puts "Helper Methods: #{(Class.new.instance_eval {include IRBHelper;self}.new.methods.sort - Class.new.methods).join(', ')}"
  include IRBHelper
end

#process events coming from data jockey
Thread.start {
  loop {
    sleep(0.001)
    #process the incoming events
    Datajockey::InterpreterIOProxy.processEvents
  }
}

unless ActiveRecord::Base.connected?
  conf = YAML::load(File.open(conf_file))
  if conf["database"]
    ActiveRecord::Base.establish_connection(conf["database"])
  else
    puts "NOTE:"
    puts "No database entry in config file, cannot establish database connection."
  end
end

#include Datajockey

#set the binding
IRB.set_binding(Datajockey::ApplicationModel.instance)

#evaluate the input [run the interp]
loop {
  catch(:IRB_EXIT) do
    IRB.instance.eval_input
  end
}

