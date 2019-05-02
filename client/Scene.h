#pragma once
class CScene
{
protected:
	HDC		m_DC;

public:
	CScene();
	virtual ~CScene();

public:
	virtual void Initialize() = 0;
	virtual void Update(GameInfo gameInfo) = 0;
	virtual void Render(HDC _dc) = 0;
	virtual void Release() = 0;
};

