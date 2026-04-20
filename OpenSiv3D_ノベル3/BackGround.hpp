# pragma once
# include "Common.hpp"

class BackGround
{
public:
	void init(const String& s_asset);//コンストラクタの代わり

	void Change(const String& s_asset);

	void draw();

	String getAsset();

private:
	String asset;
	String asset_before;
	Stopwatch m_stopwatch;
	bool changed;
};
