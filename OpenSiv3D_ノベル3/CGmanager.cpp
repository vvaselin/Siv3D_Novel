#include "CGmanager.hpp"

void CGmanager::change(const String& asset)
{
	this->asset = asset;
	//visibleがtrueならchangedをtrueにしてdrawに反映させる
	if (visible && asset_before != asset)
	{
		changed = true;
	}
	else
	{
		asset_before = asset;
		changed = false;
	}
}

//描画するかどうかを変更
void CGmanager::setVisible(const bool& s_visible)
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
bool CGmanager::draw()
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
				asset_before = asset;// 情報更新
				changed = false;
			}

			TextureAsset(asset_before).drawAt(Scene::CenterF(), ColorF{ 1.0, 1.0 - alpha * 0.8 });
			TextureAsset(asset).drawAt(Scene::CenterF(), ColorF{ 1.0, alpha });
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
			TextureAsset(asset).drawAt(Scene::CenterF(), ColorF{ 1.0, alpha });
		}
		else //通常表示
		{
			TextureAsset(asset).drawAt(Scene::CenterF());
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
			asset.clear();
			asset_before.clear();
		}
		TextureAsset(asset).drawAt(Scene::CenterF(), ColorF{ 1.0, 1.0 - alpha });
	}

	return changed;
}

//changeフラグをTRUEにする
void CGmanager::changeFlag()
{
	changed = true;
}


//セーブデータ用
bool CGmanager::getVisible() const
{
	return visible;
}
String CGmanager::getAsset() const
{
	return asset;
}


