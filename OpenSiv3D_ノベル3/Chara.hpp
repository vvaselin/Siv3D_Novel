# pragma once
# include "Common.hpp"

//キャラの情報
class Chara
{
public:
	enum class MovePattern
	{
		Step,
		Jump,
		BackLeft,
		BackRight,
		Default,
		Wait,
	};

	enum class FadePattern
	{
		Left,
		Right,
		Default
	};


	//キャラを登録
	Chara(const String& s_name, const String& portrait, const Vec2& pos, const double& scale, const bool& s_visible);
	Chara();

	//立ち絵や位置や大きさの変更
	void change(const String& portrait, const double& s_scale, const Vec2& pos);
	//立ち絵や位置や大きさの変更
	void changeQuick(const String& portrait, const double& s_scale, const Vec2& pos);

	//描画するかどうかを変更
	void setVisible(const bool& s_visible);

	bool getVisible() const;

	//TextureAssetから描画
	void draw();

	void changeFlag();

	//スクリプトの名前が登録キャラと一致するか
	bool judgeName(const String& s_name);

	void setMove(MovePattern mp);

	bool updateMove();

	void MoveReset();

	bool getMoving() const;

	//セーブデータ用
	String getName() const;

	bool fading();

private:
	String name;//キャラの名前
	double scale = 0.6, scale_before = 0.6;//キャラの大きさ
	String asset, asset_before;//どの立ち絵を使うか
	bool visible, visible_before = false;
	Vec2 position, position_before;
	bool changed = false, fadeIN = true, fadeOUT = false;
	Stopwatch m_stopwatch; //change用

	bool moving = false;

	Transition transition{ 0.5s, 0.8s };

	Transition m_tra{0.5s,1s};

	MovePattern m_pattern = MovePattern::Default;
	FadePattern f_pattern = FadePattern::Default;
};
