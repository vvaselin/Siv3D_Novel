# include "ScriptManager.hpp"

Chara::MovePattern judgeMP(const String& text);

void ScriptManager::setFile(const FilePath& f, const size_t& n)
{
	bgm.Stop();
	// ファイルをオープンする
	TextReader reader{ f };
	// オープンに失敗
	if (not reader) throw Error{ U"Failed to open script" };
	ScriptFile = f;

	// 行の内容を読み込む変数
	String line;
	messages.clear();
	while (reader.readLine(line))
	{
		messages << line;
	}

	scriptLine = n;
	End = false;
}

void ScriptManager::scriptUpdate()
{
	bool isRead =  not m_box.isReading();

	//セリフを全部表示し終わったら(選択肢表示中は進まない)
	if (not Wait && isRead && !selecting )
	{
		//行が空いているか、最初の文字が'*'なら飛ばす
		while ((messages[scriptLine] == U"") || (messages[scriptLine] == U" ") || (messages[scriptLine][0] == landmark))
		{
			++scriptLine %= messages.size();
		}

		if (messages[scriptLine][0] == atMark)// 命令文
		{
			for (auto& chara : characters)
			{
				chara.MoveReset();
			}

			Array<String> order = messages[scriptLine].split(colon);
			order[0].pop_front(); //@消す

			switch (Orders[order[0]])
			{
			case 1: // キャラの定義
			{
				bool exist = false;
				for (auto& chara : characters)
				{
					if (chara.judgeName(order[1])) //キャラがすでに登録されていれば登録しない
					{
						exist = true;
						break;
					}
				}
				if (!exist)
				{
					const bool visible = (order[5] == U"YES"); //デフォルトで表示
					Vec2 pos = posJudge(order[3]);
					double scale = scaleJudge(order[4]);

					characters << Chara(order[1], order[2], pos, scale, visible); //スクリプトから名前と初期の立ち絵を登録
				}
			}
			break;
			case 2: // 立ち絵の変更
			{
				String portrait = U"NAN";
				Vec2 pos = Vec2{ 0,0 };
				double scale = 0.0;

				for (auto& chara : characters)
				{
					if (chara.judgeName(order[1])) //名前の一致するキャラを見つける
					{
						for (size_t i = 2; i < order.size(); i++)
						{
							if (SerchAsset(chara.getName() + order[i]) != U"NAN")
							{
								portrait = order[i];
							}
							else if (posJudge(order[i]) != Vec2{ 0,0 })
							{
								pos = posJudge(order[i]);
							}
							else if (scaleJudge(order[i]) != 0.0)
							{
								scale = scaleJudge(order[i]);
							}
							else
							{
								Chara::MovePattern mp_ = judgeMP(order[i]);
								chara.setMove(mp_);
							}
						}

						chara.change(portrait, scale, pos);
						break;
					}
				}
			}
			break;
			case 3: // キャラを表示するか切り替え
			{
				bool visible = (order[2] == U"YES");
				for (auto& chara : characters)
				{
					if (chara.judgeName(order[1])) //名前の一致するキャラを見つける
					{
						chara.setVisible(visible);
						if (order.size()>3)
						{
							Chara::MovePattern mp_ = judgeMP(order[3]);
							chara.setMove(mp_);
						}
						break;

					}
				}
			}
			break;
			case 4: // 選択肢表示
			{
				Skip_ = false;
				saveLine = scriptLine;//選択肢表示のところでセーブできるように
				m_box.SwitchOFF();//文字送り停止
				selecting = true;
				targets.clear(); //選択肢を選んで飛ぶ先
				Array<String> selects; //選択肢の集合
				for (size_t i = 1; i <= 2; i++)
				{
					Array<String> c_split = messages[scriptLine + i].split(colon);
					targets << c_split[0];
					selects << c_split[1];
				}
				s_box.setSelects(selects);
			}
			break;
			case 5: // ジャンプ機能
			{
				String goal = landmark + order[1];
				while (messages[scriptLine] != goal)
				{
					++scriptLine %= messages.size();
				}
				
			}
			break;
			case 6: // 集中線
			{
				if (order[1] == U"YES")
				{
					LFflag = true;
					l_effect.init();
				}
				else
				{
					LFflag = false;
				}
			}
			break;
			case 7: // キャラのレイヤー
			{
				int32 num = Parse<int32>(order[2]); //0から
				if (num >= characters.size()) num = characters.size()-1;
				int32 i = 0;
				for (auto& chara : characters)
				{
					if (chara.judgeName(order[1]))
					{
						if (chara.getVisible()) chara.changeFlag();
						Chara tmp = chara;

						characters.erase(characters.begin() + i);
						characters.insert(characters.begin() + num, tmp);

						break;
					}
					i++;
				}
			}
			break;
			case 8: // キャラを動かす
			{
				for (auto& chara : characters)
				{
					if (chara.judgeName(order[1])) //名前の一致するキャラを見つける
					{
						Chara::MovePattern mp_ = judgeMP(order[2]);

						chara.setMove(mp_);
						break;
					}
				}
			}
			break;
			case 9: //ゲーム終了
			{
				bgm.Stop();
				End = true;
			}
			break;
			case 10: //画面揺れ
			{
				shake = true;
			}
			break;
			case 11: //背景切り替え
			{
				if (order[1] == U"OFF") order[1].clear();
				bg.Change(order[1]);
			}
			break;
			case 12: // カメラ
			{
				if (order[1] == U"OFF")
				{

				}

			}
			break;
			case 13: //BGM切り替え
			{
				if (order[1] == U"OFF")
				{
					bgm.Stop();
				}
				else
				{
					bgm.Change(order[1]);
				}

			}
			break;
			case 14: // 新しいスクリプト
			{
				int32 num = 0;
				if (order[2]) num = Parse<int32>(order[2]);
				this->setFile(order[1], num);
			}
			break;
			case 15: //効果音設定
			{
				SE_flag = true;
				SE_name = order[1];
			}
			break;
			case 16: //イベントCG
			{
				if (order[1] == U"OFF")
				{
					cg.setVisible(false);
				}
				else
				{
					cg.change(order[1]);
					cg.setVisible(true);
				}
			}
			break;
			case 17: // ムービー再生
			{
				
			}
			break;
			case 18: // シーン遷移描写
			{
				fade = true;
			}
			break;
			default:
				break;
			}
		}
		else// メッセージボックスに表示する文字
		{
			m_box.SwitchON();
			Array<String> Dialogue = messages[scriptLine].split(colon);
			if (Dialogue.size() == 1) Dialogue.push_front(U""); //colon忘れたときの安全策

			saveLine = scriptLine;
			
			if (Dialogue[1][0]==dot)
			{
				m_box.addText(Dialogue[1]); //セリフ追加
			}
			else
			{
				m_box.setText(Dialogue[0], Dialogue[1]); // メッセージセット
			}
			Log = Dialogue[0] + Dialogue[1];
			LogGen = true;
		}
		++scriptLine %= messages.size(); // 次のメッセージに切り替える
	}

}

void ScriptManager::draw(const bool& textON, const bool& menue, const double& speed, const double& autospeed)
{
	Mat3x2 mat = Mat3x2::Identity();
	if (shake)
	{
		transition.update(true);
		if (transition.isOne()) shake = false;
	}
	else
	{
		transition.update(false);
	}
	if (const double t = transition.value())
	{
		mat = Mat3x2::Translate(RandomVec2(t * 50));
	}

	bool Moving = false;
	for (auto& chara : characters)
	{
		if (chara.updateMove())
		{
			Moving = true;
			break;
		}
	}


	//揺らすためのスコープ
	{
		// 座標変換
		const Transformer2D transformer{ mat };

		//背景表示
		bg.draw();
		//キャラ表示
		for (auto& chara : characters)
		{
			chara.draw();
		}
		//CG表示
		Wait = Moving || fade || cg.draw();
	}

	//集中線
	if (LFflag)
	{
		l_effect.draw();
	}

	if (textON)
	{
		//選択肢表示
		if (selecting)
		{
			s_box.draw(); // 選択ボックス表示
			s_box.isSelected(); // 選択されている選択肢を更新
			if (s_box.isClicked() == 1)
			{
				String goal = landmark + targets[0];
				while (messages[scriptLine] != goal)
				{
					++scriptLine %= messages.size();
				}
				++scriptLine %= messages.size();
				selecting = false;
			}
			else if (s_box.isClicked() == 2)
			{
				String goal = landmark + targets[1];
				while (messages[scriptLine] != goal)
				{
					++scriptLine %= messages.size();
				}
				++scriptLine %= messages.size();
				selecting = false;
			}
		}
		else//セリフ表示
		{
			bool clicked = winClick() && !menue && !Wait; //メニュー表示してなければクリック有効

			if (Auto_ && m_box.getEndFlag())
			{
				const double autotime = spawnTime * Clamp(autospeed, 0.1, 1.0) * 2;

				accumulator += Scene::DeltaTime();

				if (autotime <= accumulator)
				{
					accumulator -= autotime;

					clicked = true;
				}
			}
			else if (Skip_)
			{
				clicked = true;
			}

			m_box.draw(clicked, speed);
		}
	}

	f_tra.update(fade);
	Rect{ 0, 0, Scene::Width(),Scene::Height() }.draw(ColorF{ 0.0 }.withAlpha(f_tra.value()));
	if (f_tra.value() >= 1.0) fade = false;

	if (SE_flag)
	{
		AudioAsset(SE_name).playOneShot(MixBus1);
		SE_flag = false;
		SE_name.clear();
	}
}

void ScriptManager::StopBGM()
{
	bgm.Stop();
}

bool ScriptManager::getLogGen() const
{
	return LogGen;
}

String ScriptManager::getLog()
{
	if (LogGen)
	{
		LogGen = false;
	}
	return Log;
}


bool ScriptManager::getEnd() const
{
	return End;
}


//セーブデータ用
String ScriptManager::getFile() const //今進めているスクリプトファイルを返す
{
	return ScriptFile;
}
int32 ScriptManager::getLine() const //今何行目か
{
	return saveLine;
}

void ScriptManager::setAuto(const bool& Flag)
{
	Auto_ = Flag;
}

bool ScriptManager::getAuto() const
{
	return Auto_;
}

void ScriptManager::setSkip(const bool& Flag)
{
	Skip_ = Flag;
}

bool ScriptManager::getSkip() const
{
	return Skip_;
}

Array<String> ScriptManager::UpdateQuick(const int32& line)
{
	Array<String> Out;
	scriptLine = 0;
	while (scriptLine <= line)
	{
		//行が空いているか、最初の文字が'*'なら飛ばす
		while ((messages[scriptLine] == U"") || (messages[scriptLine] == U" ") || (messages[scriptLine][0] == landmark))
		{
			++scriptLine %= messages.size();
		}

		if (messages[scriptLine][0] == atMark)// 命令文
		{
			Array<String> order = messages[scriptLine].split(colon);
			order[0].pop_front(); //@消す

			switch (Orders[order[0]])
			{
			case 1: // キャラの定義
			{
				bool exist = false;
				for (auto& chara : characters)
				{
					if (chara.judgeName(order[1])) //キャラがすでに登録されていれば登録しない
					{
						exist = true;
						break;
					}
				}
				if (!exist)
				{
					const bool visible = (order[5] == U"YES"); //デフォルトで表示
					Vec2 pos = posJudge(order[3]);
					double scale = scaleJudge(order[4]);

					characters << Chara(order[1], order[2], pos, scale, visible); //スクリプトから名前と初期の立ち絵を登録
				}
			}
			break;
			case 2: // 立ち絵の変更
			{
				String portrait = U"NAN";
				Vec2 pos = Vec2{ 0,0 };
				double scale = 0.0;

				for (auto& chara : characters)
				{
					if (chara.judgeName(order[1])) //名前の一致するキャラを見つける
					{
						for (size_t i = 2; i < order.size(); i++)
						{
							if (SerchAsset(chara.getName() + order[i]) != U"NAN")
							{
								portrait = order[i];
							}
							else if (posJudge(order[i]) != Vec2{ 0,0 })
							{
								pos = posJudge(order[i]);
							}
							else if (scaleJudge(order[i]) != 0.0)
							{
								scale = scaleJudge(order[i]);
							}
							else
							{
								Chara::MovePattern mp_ = judgeMP(order[2]);

								if (scriptLine==line) chara.setMove(mp_);
								break;
							}
						}

						if (scriptLine == line) chara.change(portrait, scale, pos);
						else chara.changeQuick(portrait, scale, pos);
						
						break;
					}
				}
			}
			break;
			case 3: // キャラを表示するか切り替え
			{
				bool visible = (order[2] == U"YES");
				for (auto& chara : characters)
				{
					if (chara.judgeName(order[1])) //名前の一致するキャラを見つける
					{
						chara.setVisible(visible);
						if (scriptLine == line&&order.size() > 3)
						{
							Chara::MovePattern mp_ = judgeMP(order[3]);
							chara.setMove(mp_);
						}
						break;
					}
				}
			}
			break;
			case 4: // 選択肢表示
			{
				if (scriptLine==line)
				{
					Skip_ = false;
					saveLine = scriptLine;//選択肢表示のところでセーブできるように
					m_box.SwitchOFF();//文字送り停止
					selecting = true;
					targets.clear(); //選択肢を選んで飛ぶ先
					Array<String> selects; //選択肢の集合
					for (size_t i = 1; i <= 2; i++)
					{
						Array<String> c_split = messages[scriptLine + i].split(colon);
						targets << c_split[0];
						selects << c_split[1];
					}
					s_box.setSelects(selects);
				}
				else
				{
					targets.clear(); //選択肢を選んで飛ぶ先
					for (size_t i = 1; i <= 2; i++)
					{
						Array<String> c_split = messages[scriptLine + i].split(colon);
						targets << c_split[0];
					}

					String goal;
					int32 tmp = line;

					while (tmp > scriptLine)
					{
						if ((messages[tmp] == landmark + targets[0]) || (messages[tmp] == landmark + targets[1]))
						{
							goal = messages[tmp];
							break;
						}
						--tmp %= messages.size();
					}

					while (messages[scriptLine] != goal)
					{
						++scriptLine %= messages.size();
					}
				}
			}
			break;
			case 5: // ジャンプ機能
			{
				String goal = landmark + order[1];
				while (messages[scriptLine] != goal)
				{
					++scriptLine %= messages.size();
				}
			}
			break;
			case 6: // 集中線
			{
				if (order[1] == U"YES")
				{
					LFflag = true;
					l_effect.init();
				}
				else
				{
					LFflag = false;
				}
			}
			break;
			case 7: // キャラのレイヤー
			{
				int32 num = Parse<int32>(order[2]); //0から
				if (num >= characters.size()) num = characters.size() - 1;
				int32 i = 0;
				for (auto& chara : characters)
				{
					if (chara.judgeName(order[1]))
					{
						if ((chara.getVisible())&&(scriptLine==line)) chara.changeFlag();
						Chara tmp = chara;

						characters.erase(characters.begin() + i);
						characters.insert(characters.begin() + num, tmp);

						break;
					}
					i++;
				}
			}
			break;
			case 8:
			{
				if (scriptLine==line)
				{
					for (auto& chara : characters)
					{
						if (chara.judgeName(order[1])) //名前の一致するキャラを見つける
						{
							Chara::MovePattern mp_ = judgeMP(order[2]);

							chara.setMove(mp_);
							break;
						}
					}
				}
			}
			break;
			case 9: //ゲーム終了
			{
				bgm.Stop();
				End = true;
			}
			break;
			case 10: //画面揺れ
			{
				if (scriptLine==line) shake = true;
			}
			break;
			case 11: //背景切り替え
			{
				if (order[1] == U"OFF") order[1].clear();
				bg.Change(order[1]);
			}
			break;
			case 12: // カメラ
			{
				if (order[1] == U"OFF")
				{

				}

			}
			break;
			case 13: //BGM切り替え
			{
				if (order[1] == U"OFF")
				{
					bgm.Stop();
				}
				else
				{
					bgm.Change(order[1]);
				}
			}
			break;
			case 14: // 新しいスクリプト
			{
				int32 num = 0;
				if (order[2]) num = Parse<int32>(order[2]);
				this->setFile(order[1], num);
			}
			break;
			case 15: //効果音設定
			{
				if (scriptLine == line)
				{
					SE_flag = true;
					SE_name = order[1];
				}
			}
			break;
			case 16: //イベントCG
			{
				if (order[1] == U"OFF")
				{
					cg.setVisible(false);
				}
				else
				{
					cg.change(order[1]);
					cg.setVisible(true);
				}
			}
			break;
			case 17: // ムービー再生
			{

			}
			case 18: // シーン遷移描写
			{
				if (scriptLine == line) fade = true;
			}
			break;
			break;
			default:
				break;
			}
		}
		else// メッセージボックスに表示する文字
		{
			Array<String> Dialogue = messages[scriptLine].split(colon);
			if (Dialogue.size() == 1) Dialogue.push_front(U""); //colon忘れたときの安全策

			saveLine = scriptLine;
			
			if (Dialogue[1][0] == dot)
			{
				m_box.addText(Dialogue[1]); //セリフ追加
			}
			else
			{
				m_box.setText(Dialogue[0], Dialogue[1]); // メッセージセット
			}
			Log = Dialogue[0] + Dialogue[1];
			Out << Log;
		}

		++scriptLine %= messages.size(); // 次のメッセージに切り替える
	}

	return Out;
}

Chara::MovePattern judgeMP(const String& text)
{
	HashTable<String,int32> m_Patterns =
	{
		{U"Step",1},
		{U"Jump",2},
		{U"BackLeft",3},
		{U"BackRight",4},
		{U"Default",5},
		{U"Wait",6},
	};

	switch (m_Patterns[text])
	{
	case 1:
		return Chara::MovePattern::Step;
		break;
	case 2:
		return Chara::MovePattern::Jump;
		break;
	case 3:
		return Chara::MovePattern::BackLeft;
		break;
	case 4:
		return Chara::MovePattern::BackRight;
		break;
	case 5:
		return Chara::MovePattern::Default;
		break;
	case 6:
		return Chara::MovePattern::Default;
		break;
	default:
		return Chara::MovePattern::Default;
		break;
	}
	return Chara::MovePattern::Default;
}

String ScriptManager::getBGM()
{
	return bgm.GetCurrentBGM();
}
