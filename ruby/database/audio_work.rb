class AudioWork < ActiveRecord::Base
  belongs_to :audio_file
  has_many :artist_audio_works, :dependent => :destroy
  has_many :artists, :through => :artist_audio_works

  has_many :audio_work_tags, :dependent => :destroy
  has_many :tags, :through => :audio_work_tags do
    #here we make it so you can do tags["tag name"] or tags[index]
    def [](arg)
      if arg.is_a?(Integer) or arg.is_a?(Range)
        return find(:all, :order => :id)[arg]
      end
      tag_class = TagClass.find_by_name(arg)
      if tag_class
        return find(:all, :conditions => {:tag_class_id => tag_class.id})
      else
        return []
      end
    end
  end

  has_one :album_audio_work, :dependent => :destroy

  has_many :descriptors, :dependent => :destroy do
    #here we make it so that you can do descriptors["name"] or descriptors[index]
    def [](arg)
      if arg.is_a?(Integer) or arg.is_a?(Range)
        #we have an order so that [0] is always [0]
        return find(:all, :order => :id)[arg]
      else
        return find(:first, :conditions => {:descriptor_type_id => DescriptorType.find_by_name(arg)})
      end
    end
  end
  has_many :descriptor_types, :through => :descriptors
  has_one :annotation_file, :dependent => :destroy

  def album
    self.album_audio_work.album
  end

  #class method
  def AudioWork.regex_search(params)
    #make sure we have valid parameters
    if !params or !params.is_a?(Hash) or params.size == 0
      return []
    end
    #set up default arguments
    params = {:artist_name => '.*',
      :album_name => '.*',
      :audio_work_name => '.*'}.merge(params)
    params.each do |key, val|
      if val == ''
        params[key] = '.*'
      end
    end

    works = []
    aw = AudioWork.find(:all, :include => [:artists, :album_audio_work],
                   :conditions => ['audio_works.name REGEXP ? AND artists.name REGEXP ?',
                     params[:audio_work_name], params[:artist_name]])

    #if you allow any album you should also allow songs with no album
    if params[:album_name] == '.*' or params[:album_name] =~ /^\s*$/
      works = aw
    else
      aw.each do |work|
        if work.album and work.album.name =~ /#{params[:album_name]}/i
          works << work
        end
      end
    end

    return works


    #return AudioWork.find(:all, :include => :artists, 
    #  :conditions => ['audio_works.name REGEXP ? AND artists.name REGEXP ?',
    #    params[:audio_work_name], params[:artist_name]])

    #return AudioWork.find(:all, :include => [:album_audio_work, :artists], 
    #  :conditions => ['audio_works.name REGEXP ? AND artists.name REGEXP ? AND albums.name REGEXP ?',
    #    params[:audio_work_name], params[:artist_name], params[:album_name]])
  end
end
