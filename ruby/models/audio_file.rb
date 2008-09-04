class AudioFile < ActiveRecord::Base
  belongs_to :audio_file_type
  validates_uniqueness_of :location

  def seconds
    return milliseconds * 1e-3
  end
  
#  def initialize(attributes = nil)
#    super(attributes)
#    self.added = DateTime.now
#  end
end
