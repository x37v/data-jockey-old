#redefining the work filter init method
#we create a proc that calls our 'works' method
#and pass this proc to the original initialize method
module Datajockey
  class WorkFilter
    alias :old_init :initialize
    def initialize(name, description = "Description of WorkFilter")
      this_obj = self
      @proc = Proc.new { 
        clearWorks
        this_obj.works.each do |i|
          if i.is_a?(Integer)
            addWork(i)
          end
        end
      }
      old_init(name, description, @proc)
    end
    #overload this function in order to get your filters to work
    def works
      return []
    end
  end
end

=begin
#this is an example filter that simply returns a random number
class RandFilter < Datajockey::WorkFilter
  def initialize
    super("Random Filter", "This Filter gives one Random Work.")
  end
  def works
    return [rand(400)]
  end
end
=end
