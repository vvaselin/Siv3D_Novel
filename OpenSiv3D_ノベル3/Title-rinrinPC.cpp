# include "Title.hpp"

Title::Title(const InitData& init)
	: IScene{ init }
{
	AssetInit();
}

void Title::update()
{
	if (SimpleGUI::Button(U"NEW GAME", Scene::CenterF() + Vec2{ 200, 200 }))
	{
		getData().e_Continue = false;
		getData().File = U"script/test3.txt";
		getData().ScriptLine = 0;
		// ゲームシーンに遷移
		changeScene(State::Game);
	}
	if (SimpleGUI::Button(U"CONTINUE", Scene::CenterF() + Vec2{ 500, 200 }))
	{
		// ゲームシーンに遷移
		changeScene(State::Game);
		getData().e_Continue = true;
	}
}

void Title::draw() const
{
	Scene::SetBackground(Palette::Mediumaquamarine);

	FontAsset(U"TitleFont")(U"Title").drawAt(Scene::Center());
}
