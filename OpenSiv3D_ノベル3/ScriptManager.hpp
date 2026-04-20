# pragma once
# include "Common.hpp"
# include "LineEffect.hpp"
# include "Chara.hpp"
# include "MessageBox.hpp"
# include "SelectBox.hpp"
# include "BackGround.hpp"
# include "BGMmanager.hpp"
# include "CGmanager.hpp"
# include "BackLog.hpp"

//スクリプトの読み込み
class ScriptManager
{
public:

	void setFile(const FilePath& f, const size_t& n);

	void scriptUpdate();

	Array<String> UpdateQuick(const int32& line);

	void draw(const bool& text, const bool& menue, const double& speed, const double& autospeed);

	void StopBGM();

	bool getLogGen() const;

	String getLog();
	
	bool getEnd() const;

	void setAuto(const bool& Flag);
	bool getAuto() const;

	void setSkip(const bool& Flag);
	bool getSkip() const;

	String getBGM();

	//セーブデータ用
	String getFile() const;//今進めているスクリプトファイルを返す
	int32 getLine() const; //何行目か

private:
	//スクリプトの文章
	Array<String> messages;
	//登場するキャラクター
	Array<Chara> characters;
	//バックログの文章
	String Log;
	//テキストボックス
	MessageBox m_box;
	//選択肢
	SelectBox s_box;
	//背景
	BackGround bg;
	//BGM
	BGMmanager bgm;
	//集中線
	LineEffect l_effect;
	bool LFflag = false;
	//イベントCG
	CGmanager cg;

	size_t scriptLine = 0;
	int32 saveLine = 0; //セーブデータ用行数
	//命令文
	HashTable<String, int32> Orders =
	{
		{U"new_chara",1},
		{U"change",2},
		{U"visible",3},
		{U"select",4},
		{U"jump",5},
		{U"l_effect",6},
		{U"layer",7},
		{U"move",8},
		{U"end",9},
		{U"shake",10},
		{U"bg_image",11},
		{U"camera", 12},
		{U"bgm", 13},
		{U"script", 14},
		{U"se", 15},
		{U"cg", 16},
		{U"movie",17},
		{U"blackout",18},
		{U"endspeed",19},
	};
	const String::value_type atMark{ U'@' }; //命令文
	const String::value_type colon{ U':' }; //区切り文字
	const String::value_type landmark{ U'*' }; //ターゲットかコメントアウト
	const String::value_type dot{ U'.' }; //追加のセリフ

	//選択肢による分岐
	bool selecting = false;
	Array<String> targets;

	//進行中のスクリプトファイル
	FilePath ScriptFile;

	//バックログ生成フラグ
	bool LogGen = false;

	//バックログを消すフラグと消すテキスト
	bool LogDel = false;
	Array<String> DelText;

	bool Start = false;
	bool End = false;

	//SEを鳴らすか, SEの名前
	bool SE_flag = false;
	String SE_name;

	//画面揺れ
	Transition transition{ 0.2s, 0.3s }; // トランジション
	bool shake = false;

	//オートモード
	bool Auto_ = false;
	const double spawnTime = 1.0;
	double accumulator = 0.0;

	bool Skip_ = false;

	bool Wait = false;

	bool fade = false;
	Transition f_tra{1.0s,0.3s};

};

