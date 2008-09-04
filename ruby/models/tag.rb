class Tag < ActiveRecord::Base
  belongs_to :tag_class
  has_many :audio_work_tags, :dependent => :destroy
  has_many :audio_works, :through => :audio_work_tags
  def inheritance_column()
    'tag_class_id'
  end
end
