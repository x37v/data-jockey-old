class ThemeExplorerFilter < Datajockey::WorkFilter
  def initialize
    super("Theme Explorer",
          "This filter shows only those works who have common theme tags with the current audible works.")
  end
  def works
    #find the audible work ids
    audible = Datajockey::ApplicationModel.instance.mixerPanel.audibleWorks
    #find the actual DB entries
    audible_works = AudioWork.find(:all, :conditions => {:id => audible})
    return [] if audible_works.size == 0
    #find their theme tags
    tags = (audible_works.collect {|w| w.tags['theme']}).flatten
    return [] if tags.size == 0
    #find all audio works with those tags
    (tags.collect {|t| t.audio_works.collect{|w| w.id} }).flatten
  end
end

dj_model = Datajockey::ApplicationModel.instance
dj_model.addFilter(ThemeExplorerFilter.new)
