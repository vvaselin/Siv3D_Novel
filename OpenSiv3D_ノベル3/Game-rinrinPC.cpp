# include "Game.hpp"

void s_manager_Init(ScriptManager& s_manager, ScriptManager& reader);

SasaGUI::ScrollBar vbar(SasaGUI::Orientation::Vertical);

Game::Game(const InitData& init)
	: IScene{ init }
	, b_log{ vbar }
{
	SasaGUI::ScrollBar vbar(SasaGUI::Orientation::Vertical);
	

	if (getData().e_Continue)
	{
		ScriptManager s_manager_;
		BackLog b_log_{vbar};
		{
			// バイナリファイルをオープン
			Deserializer<BinaryReader> reader{ U"saveData/Qsave.bin" };
			if (not reader) throw Error{ U"Failed to open `Qsave.bin`" };

			//スクリプトの情報復元
			reader(s_manager_);
			s_manager_Init(s_manager, s_manager_);

			//バックログ読み込み
			reader(b_log_);
			b_log.getMessages().clear();
			for (const auto& message:b_log_.getMessages())
			{
				b_log.addMessage(message);
			}
		}

		Print << U"ロードしました";

	}
	else
	{
		s_manager.setFile(getData().File, getData().ScriptLine);
	}
}

void Game::update()
{
	//スクリプトによる描画
	s_manager.scriptUpdate();
	s_manager.draw(TextWatch);

	//End命令が来たらタイトル画面へ
	if (s_manager.getEnd() && s_manager.getLine() >= 1) changeScene(State::Title);

	if (LogWatch) //バックログ描画
	{
		Rect{ 0, 0, 1920, 1080 }.draw(Palette::Skyblue.withA(200));
		if (b_log.ReadLog() && Mouse::Wheel() >= 1)
		{
			LogWatch = false;
			TextWatch = true;
		}
		//バックログ更新と描画
		b_log.update();
		b_log.draw();
	}
	else if (Mouse::Wheel() <= -1) //バックログ起動
	{
		//バックログの情報更新
		b_log.length_update();
		LogWatch = true;
		TextWatch = false;
		b_log.update();
		b_log.ShowNew();
	}
	else//通常描画
	{
		if (s_manager.getLogGen())//ログの生成
		{
			b_log.addMessage(s_manager.getLog()); // ログの登録
		}

		//未完成
		if (s_manager.getLogDel())
		{
			size_t size = s_manager.getDelText().size();

			for (size_t i = 0; i < size; i++)
			{
				if (s_manager.getDelText().back() == b_log.getMessages().back())
				{
					b_log.popback_Log(); //ログの消去
				}
				s_manager.popback_del();
			}
		}
	}

	//セーブ
	if (SimpleGUI::Button(U"\U000F1C18 Q.SAVE", Scene::CenterF() + Vec2{ 600, 500 }, 200))
	{
		Serializer<BinaryWriter> writer{ U"saveData/Qsave.bin" };
		if (not writer) throw Error{ U"Failed to open `Qsave.bin`" };

		// シリアライズに対応したデータを記録
		writer(s_manager);
		writer(b_log);

		Print << U"セーブしました";

	}
	if (SimpleGUI::Button(U"Q.LOAD", Scene::CenterF() + Vec2{ 300, 500 }, 200))
	{
		// ゲームシーンに遷移
		changeScene(State::Game,0.3s);
		getData().e_Continue = true;
	}
	if (SimpleGUI::Button(U"AUTO", Scene::CenterF() + Vec2{ 0, 500 }, 200))
	{
		if (s_manager.getAuto())
		{
			s_manager.setAuto(false);
		}
		else
		{
			s_manager.setAuto(true);
			s_manager.setSkip(false);
		}
	}
	if (SimpleGUI::Button(U"SKIP", Scene::CenterF() + Vec2{ -300, 500 }, 200))
	{
		if (s_manager.getSkip())
		{
			s_manager.setSkip(false);
		}
		else
		{
			s_manager.setSkip(true);
			s_manager.setAuto(false);
		}
	}

}
void Game::draw() const
{
	Scene::SetBackground(Palette::Black);
}

void s_manager_Init(ScriptManager& s_manager, ScriptManager& reader)
{
	s_manager.BGinit(reader.BGimage());
	s_manager.AddChara(reader.getChara());
	s_manager.setFile(reader.getFile(), reader.getLine());
	s_manager.BGMinit(reader.getBGM());
	s_manager.CGinit(reader.getCG());
}
