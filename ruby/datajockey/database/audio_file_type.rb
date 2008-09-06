class AudioFileType < ActiveRecord::Base
  has_many :audio_files
  validates_uniqueness_of :name
  def AudioFileType.get_valid(ft)
    type = AudioFileType.find(:first, :conditions => {:name => ft})
    if type
      return type
    elsif ft =~ /^aif$/
      return AudioFileType.find(:first, :conditions => {:name => 'aiff'})
    else
      return nil
    end
  end
end
