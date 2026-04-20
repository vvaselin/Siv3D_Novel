# include "Chara.hpp"

//名前を登録
Chara::Chara(const String& s_name, const String& portrait, const Vec2& pos, const double& scale, const bool& s_visible)
	:name{ s_name }  //アルファベットの名前
	, asset{ portrait } // 初期の立ち絵
	, asset_before{ portrait }
	, visible{ s_visible } //表示するか
	, position{ pos }
	, position_before{ pos }
	, scale{ scale }
	, scale_before{ scale }
{
	TextureAsset::LoadAsync(name + asset);//最初に表示する立ち絵はロードしておく
}

Chara::Chara()
	:name{ U"Unknown" }  //アルファベットの名前
	, asset{ U"" } // 初期の立ち絵
	, asset_before{ U"" }
	, visible{ false } //表示するか
	, position{ posJudge(U"L")}
	, position_before{ posJudge(U"L") }
	, scale{ scaleJudge(U"小")}
	, scale_before{ scaleJudge(U"小") }
{

}

void Chara::change(const String& portrait, const double& s_scale, const Vec2& pos)
{
	//有効な変更なら反映
	if (portrait != U"NAN") asset = portrait;
	if (s_scale != 0.0) scale = s_scale;
	if (pos != Vec2{ 0,0 }) position = pos;
	//visibleがtrueならchangedをtrueにしてdrawに反映させる
	if (visible && ((asset_before != asset) || (scale_before != scale) || (position_before != position)))
	{
		changed = true;
	}
	else
	{
		asset_before = asset;
		scale_before = scale;
		position_before = position;
		changed = false;
	}
}

//描画するかどうかを変更
void Chara::setVisible(const bool& s_visible)
{
	visible = s_visible;
	if (visible && !visible_before)
	{
		fadeIN = true;
	}
	else if (!visible && visible_before)
	{
		fadeOUT = true;
	}
}

//TextureAssetから描画
void Chara::draw()
{
	if (visible)
	{
		if (changed)
		{
			double alpha = m_stopwatch.sF() * 2;
			if (!m_stopwatch.isRunning()) m_stopwatch.start();
			if (alpha >= 1.0)
			{
				m_stopwatch.reset();
				asset_before = asset; scale_before = scale; position_before = position; // 情報更新
				changed = false;
			}

			TextureAsset(name + asset_before).scaled(scale_before).drawAt(position_before, ColorF{ 1.0, 1.0 - alpha * 0.8 });
			TextureAsset(name + asset).scaled(scale).drawAt(position, ColorF{ 1.0, alpha });
		}
		else if (fadeIN) //フェードイン
		{
			double alpha = m_stopwatch.sF();
			if (!m_stopwatch.isRunning()) m_stopwatch.start();
			if (alpha >= 1.0)
			{
				m_stopwatch.reset();
				visible_before = true;
				fadeIN = false;
			}
			TextureAsset(name + asset).scaled(scale).drawAt(position, ColorF{ 1.0, alpha });
		}
		else //通常表示
		{
			TextureAsset(name + asset).scaled(scale).drawAt(position);
		}
	}
	else if (fadeOUT) // フェードアウト
	{
		double alpha = m_stopwatch.sF();
		if (!m_stopwatch.isRunning()) m_stopwatch.start();
		if (alpha >= 1.0)
		{
			m_stopwatch.reset();
			visible_before = false;
			fadeOUT = false;
		}
		TextureAsset(name + asset).scaled(scale).drawAt(position, ColorF{ 1.0, 1.0 - alpha });
	}

}

//changeフラグをTRUEにする
void Chara::changeFlag()
{
	changed = true;
}

//スクリプトの名前が登録キャラと一致するか
bool Chara::judgeName(const String& s_name)
{
	if (name == s_name)
	{
		return true;
	}
	return false;
}

//セーブデータ用
String Chara::getName() const
{
	return name;
}
bool Chara::getVisible() const
{
	return visible;
}
String Chara::getPos() const
{
	if (position == Vec2{ 500, 600 })
	{
		return U"L";
	}
	else if (position == Vec2{ 1000, 600 })
	{
		return U"C";
	}
	else
	{
		return U"R";
	}
}
String Chara::getScale() const
{
	if (scale == 1.2)
	{
		return U"大";
	}
	else if (scale == 0.4)
	{
		return U"小";
	}
	else
	{
		return U"中";
	}
}
String Chara::getAsset() const
{
	return asset;
}

