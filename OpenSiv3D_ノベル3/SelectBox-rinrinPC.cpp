# include "SelectBox.hpp"


//選択肢の登録
void SelectBox::setSelects(const Array<String>& message)
{
	status = 0;
	selects.clear();
	selects = message;
}
//描画
void SelectBox::draw()
{
	if (KeyUp.down() || KeyDown.down() || KeyLeft.down() || KeyRight.down())
	{
		++posnum %= pos.size();
		Cursor::SetPos(pos[posnum]);
	}

	if (status == 0) //フェードイン
	{
		double alpha = m_stopwatch.sF() * 5;
		if (!m_stopwatch.isRunning()) m_stopwatch.start();
		if (alpha >= 1.0)
		{
			m_stopwatch.reset();
			status = 1;
		}
		rect1.draw(ColorF{ c1 , alpha });
		s_font(selects[0]).drawAt(pos[0], ColorF{ 0, alpha });
		rect2.draw(ColorF{ c1 , alpha });
		s_font(selects[1]).drawAt(pos[1], ColorF{ 0, alpha });
	}
	else if (status == 2) //フェードアウト
	{
		double alpha = m_stopwatch.sF() * 5;
		if (!m_stopwatch.isRunning()) m_stopwatch.start();
		if (alpha >= 1.0)
		{
			m_stopwatch.reset();
		}
		rect1.draw(ColorF{ c1 , 1 - alpha });
		s_font(selects[0]).drawAt(pos[0], ColorF{ 0, 1 - alpha });
		rect2.draw(ColorF{ c1 , 1 - alpha });
		s_font(selects[1]).drawAt(pos[1], ColorF{ 0, 1 - alpha });
	}
	else if (status == 1) //通常表示 status=1
	{
		rect1.draw(ColorF{ c1 });
		s_font(selects[0]).drawAt(pos[0], ColorF{ 0 });
		rect2.draw(ColorF{ c1 });
		s_font(selects[1]).drawAt(pos[1], ColorF{ 0 });
	}
}
//どれを選択中か
void SelectBox::isSelected()
{
	if (rect1.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
		num = 1;
	}
	else if (rect2.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
		num = 2;
	}
	else
	{
		num = 0;
	}
}

int32 SelectBox::isClicked()
{
	if (rect1.leftClicked() || (num == 1 && KeyEnter.down()))
	{
		status = 2;
		return num;
	}
	else if (rect2.leftClicked() || (num == 2 && KeyEnter.down()))
	{
		status = 2;
		return num;
	}
	return -1;
}
