def do_xfade(beats=4)
  dj_model = Datajockey::ApplicationModel.instance
  #milliseconds
  ticklength = 0.005
  xfade = dj_model.mixerPanel.crossFade
  if xfade.enabled
    start = xfade.position
    #figure out how long the cross fade should last
    time = beats * 60.0 / dj_model.mixerPanel.master.tempo
    #every tick length (find number of ticks)
    ticks = (time / ticklength - 1).to_i
    if ticks > 0
      if start > 0.5
        ticks.downto(0) do |i|
          xfade.setPosition(start * (i.to_f / ticks))
          sleep(ticklength)
        end
        #just in case
        xfade.setPosition(0.0)
      else
        (0..ticks).each do |i|
          xfade.setPosition(start + (1 - start) * (i.to_f / ticks))
          sleep(ticklength)
        end
        #just in case
        xfade.setPosition(1.0)
      end
    end
  end
end
