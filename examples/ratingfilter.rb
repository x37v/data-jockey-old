class RatingFilter < Datajockey::WorkFilter
  attr_accessor :threshold
  def initialize
    @threshold = 4
    super("Rating Filter",
          "This filter shows only those works who a rating equal to or above a given value [default is #{@threshold}]");
  end
  def works
    out = Array.new
    AudioWork.find(:all).each do |w|
      if w.descriptors["rating"] and w.descriptors["rating"].float_value >= @threshold
        out << w.id
      end
    end
    return out
  end
end

dj_model = Datajockey::ApplicationModel.instance
dj_model.addFilter(RatingFilter.new)
