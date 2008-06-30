require 'irb'

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


$dataJockeyIOProxy = Datajockey::InterpreterIOProxy.new

class <<STDOUT
  alias :old_write :write
  def write(data)
    $dataJockeyIOProxy.addToOutput(data.to_s)
  end
end


#here we read from the datajockey pipe
class PipedInput < IRB::InputMethod
  def gets()
    File.open(Datajockey::InterpreterIOProxy::pipeLocation, "r") { |f|
      input = nil
      while input == "" or input == nil
        input = f.gets()
      end
      return input
    }
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
      self.instance = Irb.new(workspace, PipedInput.new)
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
    
    IRB.setup(nil)

    workspace = WorkSpace.new(binding)

    if @CONF[:SCRIPT]
      self.instance = Irb.new(workspace, @CONF[:SCRIPT])
    else
      self.instance = Irb.new(workspace, PipedInput.new)
    end

    @CONF[:IRB_RC].call(self.instance.context) if @CONF[:IRB_RC]
    @CONF[:MAIN_CONTEXT] = self.instance.context

    trap 'INT' do
      self.instance.signal_handle
    end

    catch(:IRB_EXIT) do
      self.instance.eval_input
    end
  ensure
    old_args.each { |a| ARGV << a }
  end
end

module IRBHelper
  def meths(o); puts (o.methods.sort - Class.new.methods).join("\n"); end
  def quit!; irb_exit; ::Process.exit!; end
  #def instance; IRB.instance; end
  #def continue; irb_exit; end
end

def dROP! object = nil
  binding = case object
  when Binding
    object
  when NilClass
    Kernel.binding()
  else
    object.send(:binding)
  end

	if defined? IRBHelper
		puts "Helper Methods: #{(Class.new.instance_eval {include IRBHelper;self}.new.methods.sort - Class.new.methods).join(', ')}"
		puts "Variables: #{local_variables.map{|v| "#{v} # => #{eval(v).inspect}"}.join(', ')}"
		include IRBHelper
	end
	
	IRB.start_session binding
end

=begin
class <<STDERR
  alias :old_write :write
  def write(data)
    $dataJockeyIOProxy.addToOutput(data)
  end
end
=end

if defined? IRBHelper
  puts "Helper Methods: #{(Class.new.instance_eval {include IRBHelper;self}.new.methods.sort - Class.new.methods).join(', ')}"
  include IRBHelper
end

IRB.set_binding(Datajockey::ApplicationModelProxy.new)
while true
  catch(:IRB_EXIT) do
    IRB.instance.eval_input
  end
end

