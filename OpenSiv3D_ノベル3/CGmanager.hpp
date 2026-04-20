# pragma once
# include "Common.hpp"

//キャラの情報
class CGmanager
{
public:

	//描画するかどうかを変更
	void setVisible(const bool& s_visible);

	void change(const String& portrait);

	//TextureAssetから描画
	bool draw();

	void changeFlag();

	//セーブデータ用
	bool getVisible() const;
	String getAsset() const;

private:
	String asset, asset_before;//どの立ち絵を使うか
	bool visible = false, visible_before = false;
	bool changed = false, fadeIN = true, fadeOUT = false;
	Stopwatch m_stopwatch; //change用
};
