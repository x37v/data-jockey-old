class Descriptor < ActiveRecord::Base
  belongs_to :descriptor_type
  belongs_to :audio_work
end
