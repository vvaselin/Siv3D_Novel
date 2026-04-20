# include "BackGround.hpp"

void BackGround::init(const String& s_asset)
{
	asset = s_asset;
	asset_before = U"";
	changed = true;

	TextureAsset::LoadAsync(asset);//最初に表示する絵はロードしておく
}

void BackGround::Change(const String& s_asset)
{
	asset = s_asset;
	if ((asset_before != asset) || asset.isEmpty())
	{

		changed = true;
		TextureAsset::LoadAsync(asset);//最初に表示する絵はロードしておく
	}
	else
	{
		asset_before = asset;
		changed = false;
	}
}

void BackGround::draw()
{
	if (changed)
	{
		double alpha = m_stopwatch.sF();
		if (!m_stopwatch.isRunning()) m_stopwatch.start();
		if (alpha >= 1.0)
		{
			m_stopwatch.reset();
			asset_before = asset;
			changed = false;
		}
		if (!TextureAsset(asset_before).isEmpty())TextureAsset(asset_before).drawAt(Scene::CenterF(), ColorF{ 1.0, 1.0 - alpha });
		if (!TextureAsset(asset).isEmpty()) TextureAsset(asset).drawAt(Scene::CenterF(), ColorF{ 1.0, alpha });
	}
	else
	{
		if (!TextureAsset(asset).isEmpty()) TextureAsset(asset).drawAt(Scene::CenterF());
	}
}

String BackGround::getAsset()
{
	return asset;
}


