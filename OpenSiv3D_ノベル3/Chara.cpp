# include "Chara.hpp"


Chara::Chara()
	:name{ U"Unknown" }  //アルファベットの名前
	, asset{ U"" } // 初期の立ち絵
	, asset_before{ U"" }
	, visible{ false } //表示するか
	, position{ posJudge(U"L") }
	, position_before{ posJudge(U"L") }
	, scale{ scaleJudge(U"小") }
	, scale_before{ scaleJudge(U"小") }
{

}

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

void Chara::change(const String& portrait, const double& s_scale, const Vec2& pos)
{
	//有効な変更なら反映
	if (portrait != U"NAN") asset = portrait;
	if (s_scale != 0.0) scale = s_scale;
	if (pos != Vec2{ 0,0 }) position = pos;
	//visibleがtrueならchangedをtrueにしてdrawに反映させる
	if (visible && ((asset_before != asset)|| (scale_before != scale) || (position_before != position)))
	{
		transition.reset();
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
	if (s_visible && !visible_before)
	{
		fadeIN = true;
	}
	else if (!s_visible && visible_before)
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
			transition.update(true);

			if (transition.isOne())
			{
				asset_before = asset; scale_before = scale; position_before = position; // 情報更新
				changed = false;
			}

			TextureAsset(name + asset_before)
				.scaled(scale_before)
				.draw(position_before + Vec2{ -TextureAsset(name + asset_before).width() / 2 * scale_before, 200 -scale_before *120 }, ColorF{ 1.0, 1.0 - transition.value()});
		}
		else if (fadeIN) //フェードイン
		{
			transition.update(true);

			if (transition.isOne())
			{
				transition.reset(1.0);
				visible = true;
				visible_before = true;
				fadeIN = false;
			}
		}
	}
	else if (fadeOUT) // フェードアウト
	{
		transition.update(false);

		if (transition.isZero())
		{
			transition.reset(0.0);
			visible = false;
			visible_before = false;
			fadeOUT = false;
		}
	}

	TextureAsset(name + asset)
		.scaled(scale)
		.draw(position +Vec2{ -TextureAsset(name + asset).width()/2*scale, 200 -scale*120}, ColorF{1.0, transition.value()});
}

//changeフラグをTRUEにする
void Chara::changeFlag()
{
	changed = true;
}

//スクリプトの名前が登録キャラと一致するか
bool Chara::judgeName(const String& s_name)
{
	if (name == s_name) return true;

	return false;
}

//セーブデータ用
String Chara::getName() const
{
	return name;
}


bool Chara::getMoving() const
{
	return moving;
}

bool Chara::updateMove()
{
	m_tra.update(moving);

	if (moving)
	{
		switch (m_pattern)
		{
		case Chara::MovePattern::Step:
			position = position_before + Vec2{ Periodic::Sine1_1(0.5s,m_tra.value())* 50,-Periodic::Jump0_1(0.5s,m_tra.value()) * 50 };
			break;
		case Chara::MovePattern::Jump:
		{
			const double p_j = Periodic::Jump0_1(1.0s,m_tra.value());
			position.y = position_before.y - (p_j * 50);
		}
			break;
		case Chara::MovePattern::BackLeft:
			position = position.lerp(position.movedBy(-3, 0), m_tra.easeOut());
			break;
		case Chara::MovePattern::BackRight:
			position = position.lerp(position.movedBy(3, 0), m_tra.easeOut());
			break;
		case Chara::MovePattern::Default:
			m_tra.reset(1.0);
			break;
		case Chara::MovePattern::Wait:
			break;
		default:
			m_tra.reset(1.0);
			break;
		}
	}

	if (m_tra.isOne())
	{
		position_before = position;
		m_pattern = MovePattern::Default;
		moving = false;
	}

	return moving;
}


void Chara::setMove(MovePattern mp)
{
	if (visible)
	{
		m_tra.reset();
		m_pattern = mp;
		moving = true;
	}
}

void Chara::changeQuick(const String& portrait, const double& s_scale, const Vec2& pos)
{
	//有効な変更なら反映
	if (portrait != U"NAN") asset = portrait;
	if (s_scale != 0.0) scale = s_scale;
	if (pos != Vec2{ 0,0 }) position = pos;
	asset_before = asset;
	scale_before = scale;
	position_before = position;
	changed = false;
}

void Chara::MoveReset()
{
	position_before = position;
	visible_before = visible;
	m_pattern = MovePattern::Default;
	moving = false;
}

bool Chara::getVisible() const
{
	return visible;
}

bool Chara::fading()
{
	return (visible != visible_before);
}
