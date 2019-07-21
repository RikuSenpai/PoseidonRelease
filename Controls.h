/*
Rest In Peace ApocalypseCheats
*/

#pragma once

#include "GUI.h"


class CTextField : public CControl
{
public:
	CTextField();
	std::string getText();
	void SetText(std::string);
private:
	std::string text;
	bool IsGettingKey;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CTextField2 : public CControl
{
public:
	CTextField2();
	std::string getText();
	void SetText(std::string);
private:
	std::string text;
	bool IsGettingKey;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};
class CCheckBox : public CControl
{
public:
	CCheckBox();
	void SetState(bool s);
	bool GetState();
protected:
	bool Checked;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CLabel : public CControl
{
public:
	CLabel();
	void SetText(std::string text);
protected:
	std::string Text;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CGroupBox : public CControl
{
public:
	CGroupBox();
	void SetText(std::string text);
	void PlaceLabledControl(std::string Label, CTab *Tab, CControl* control);
	void PlaceCheckBox(std::string Label, CTab * Tab, CControl * control);
	void PlaceOtherControl(std::string Label, CTab * Tab, CControl * control);
protected:
	int Items;
	std::string Text;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CSlider : public CControl
{
public:
	CSlider();
	float GetValue();
	void SetValue(float v);
	void SetBoundaries(float min, float max);
protected:
	float Value;
	float Min;
	float Max;
	bool DoDrag;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CSlider1 : public CControl
{
public:
	CSlider1();
	float GetValue();
	void SetValue(float v);
	void SetBoundaries1(float min, float max);
protected:
	float Value;
	float Min;
	float Max;
	bool DoDrag;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};


class CKeyBind : public CControl
{
public:
	CKeyBind();
	int GetKey();
	void SetKey(int key);
protected:
	int Key;
	bool IsGettingKey;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CButton : public CControl
{
public:
	typedef void(*ButtonCallback_t)(void);
	CButton();
	void SetCallback(ButtonCallback_t callback);
	void SetText(std::string text);
protected:
	ButtonCallback_t CallBack;
	std::string Text;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

class CComboBox : public CControl
{
public:
	CComboBox();
	void AddItem(std::string text);
	void SelectIndex(int idx);
	int GetIndex();
	std::string GetItem();
protected:
	std::vector<std::string> Items;
	int SelectedIndex;
	bool IsOpen;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};

/*class CColorSelector : public CControl
{
public:
	CColorSelector();
	int* GetValue() {
		return color;
	}
	void SetColor(int r, int g, int b, int a)
	{
		color[0] = r;
		color[1] = g;
		color[2] = b;
		color[3] = a;
	}

protected:
	int color[4] = { 255, 255, 255, 255 };
	bool is_open;
	bool set_new_pos = false;
	bool toggle = false;
	float brightness = 1.f;
	void Draw(bool hover);
	void OnUpdate();
	void OnClick();
};*/