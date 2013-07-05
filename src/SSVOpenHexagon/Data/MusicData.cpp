// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVOpenHexagon/Data/MusicData.h"
#include "SSVOpenHexagon/Global/Assets.h"
#include "SSVOpenHexagon/Utils/Utils.h"
#include "SSVOpenHexagon/Global/Config.h"

using namespace std;
using namespace sf;
using namespace ssvs;
using namespace ssvu;

namespace hg
{
	MusicData::MusicData(const string& mId, const string& mFileName, const string& mName, const string& mAlbum, const string& mAuthor) :
		id{mId}, fileName{mFileName}, name{mName}, album{mAlbum}, author{mAuthor} { }

	void MusicData::addSegment(int mSeconds) { segments.push_back(mSeconds); }
	int MusicData::getRandomSegment() const { return segments[getRnd(0, segments.size())]; }
	void MusicData::playRandomSegment()
	{
		if(firstPlay) { firstPlay = false; playSegment(0); }
		else playSeconds(getRandomSegment());
	}
	void MusicData::playSegment(int mSegmentIndex)
	{
		playSeconds(segments[mSegmentIndex]);
	}
	void MusicData::playSeconds(int mSeconds)
	{
		if(getNoMusic()) return;

		playMusic(id, seconds(mSeconds));
	}

	string MusicData::getId() const			{ return id; }
	string MusicData::getFileName() const	{ return fileName; }
	string MusicData::getName() const		{ return name; }
	string MusicData::getAlbum() const		{ return album; }
	string MusicData::getAuthor() const		{ return author; }

	void MusicData::setFirstPlay(bool mFirstPlay)	{ firstPlay = mFirstPlay; }
	bool MusicData::getFirstPlay() const			{ return firstPlay; }
}

