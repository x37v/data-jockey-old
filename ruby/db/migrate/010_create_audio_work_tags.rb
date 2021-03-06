=begin
	This file is part of Data Jockey.
	
	Data Jockey is free software: you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation, either version 3 of the License, or (at your
	option) any later version.
	
	Data Jockey is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
	Public License for more details.
	
	You should have received a copy of the GNU General Public License along
	with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
=end

class CreateAudioWorkTags < ActiveRecord::Migration
  def self.up
    create_table :audio_work_tags do |t|
      t.column :audio_work_id, :integer
      t.column :tag_id, :integer
    end
  end

  def self.down
    drop_table :audio_work_tags
  end
end
