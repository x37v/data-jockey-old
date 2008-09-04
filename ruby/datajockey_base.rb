require 'rubygems'
require 'active_record'

RAILS_DIR = "../../railscode"
RAILS_MODEL_DIR = File.join(RAILS_DIR, 'app/models/')
RAILS_DB_CONF_FILE = File.join(RAILS_DIR, 'config/database.yml')

#this allows us to define methods in loops
#like foo.define_method(:blah) {1231}
class Module
  public :define_method
end

class Object
  def metaclass
    class<<self;self;end end
  def define_method(name,&blk)
    self.metaclass.define_method(name, &blk) 
  end
end

#put the modeldir into the requirepath so that our models can be added
$:.push(RAILS_MODEL_DIR)

models = Dir.entries(RAILS_MODEL_DIR).delete_if {|file| file !~ /^\w*\.rb$/}
models.each do |model|
  #require File.join(RAILS_MODEL_DIR, model)
  require model
end

if !$datajockey_connected_to_db
  def connect_to_database(environment)
    conf = YAML::load(File.open(RAILS_DB_CONF_FILE))
    ActiveRecord::Base.establish_connection(conf[environment])
  end
  connect_to_database('development')
  $datajockey_connected_to_db = true
end
