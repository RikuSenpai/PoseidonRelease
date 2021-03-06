/*
Rest In Peace ApocalypseCheats
*/

#include "GUI.h"
#include "Menu.h"
#include "MetaInfo.h"
#include "RenderManager.h"
#include <algorithm>
#include "tinyxml2.h"
#include "Controls.h"
#include "GUI.h"

CGUI GUI;
float MenuAlpha = 0.f;

float Globals::MenuAlpha2 = MenuAlpha;

CGUI::CGUI()
{

}

// Draws all windows 
void CGUI::Draw()
{
	bool ShouldDrawCursor = false;

	for (auto window : Windows)
	{
		if (window->m_bIsOpen)
		{
			ShouldDrawCursor = true;
			DrawWindow(window);
		}

	}

	


}

std::string GetTimeString()
{
	time_t current_time;
	struct tm *time_info;
	static char timeString[10];
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%X", time_info);
	return timeString;
}



#define red Menu::Window.ColorsTab.MenuR
#define green Menu::Window.ColorsTab.MenuG
#define blue Menu::Window.ColorsTab.MenuB

// Handle all input etc
void CGUI::Update()
{

	if (Menu::Window.MiscTab.watermark.GetState())
	{


	//	Render::Clear(1470, 13, 280, 18, Color(30, 206, 6, 170));
		Render::Text(1473, 15, Color(255, 255, 255, 255), Render::Fonts::supremacy, ("poseidon.cc |"));
		Render::Textf(1542, 15, Color(255, 255, 255, 255), Render::Fonts::supremacy, ("%s "), GetTimeString().c_str());
	}

	//Key Array
	std::copy(keys, keys + 255, oldKeys);
	for (int x = 0; x < 255; x++)
	{
		//oldKeys[x] = oldKeys[x] & keys[x];
		keys[x] = (GetAsyncKeyState(x));
	}

	// Mouse Location
	POINT mp; GetCursorPos(&mp);
	Mouse.x = mp.x; Mouse.y = mp.y;

	RECT Screen = Render::GetViewport();

	// Window Binds
	for (auto& bind : WindowBinds)
	{
		if (GetKeyPress(bind.first))
		{
			bind.second->Toggle();
		}
	}

	// Stop dragging
	if (IsDraggingWindow && !GetKeyState(VK_LBUTTON))
	{
		IsDraggingWindow = false;
		DraggingWindow = nullptr;
	}

	// If we are in the proccess of dragging a window
	if (IsDraggingWindow && GetKeyState(VK_LBUTTON) && !GetKeyPress(VK_LBUTTON))
	{
		if (DraggingWindow)
		{
			DraggingWindow->m_x = Mouse.x - DragOffsetX;
			DraggingWindow->m_y = Mouse.y - DragOffsetY;
		}
	}

	// Process some windows
	for (auto window : Windows)
	{
		if (window->m_bIsOpen)
			MenuAlpha = min(MenuAlpha + 6, 255);
		else
			MenuAlpha = max(MenuAlpha - 6, 0);
		{
			// Used to tell the widget processing that there could be a click
			bool bCheckWidgetClicks = false;

			// If the user clicks inside the window
			if (GetKeyPress(VK_LBUTTON))
			{
				bCheckWidgetClicks = false;
				if (IsMouseInRegion(window->m_y, window->m_x + window->m_iWidth, window->m_y + window->m_iHeight, window->m_x))
				{
					// Close Button
					if (IsMouseInRegion(window->m_y, window->m_x + window->m_iWidth, window->m_y + UI_WIN_CLOSE_X, window->m_x + window->m_iWidth - UI_WIN_CLOSE_X))
					{
						window->Toggle();
					}
					else
						// User is selecting a new tab
						if (IsMouseInRegion(window->GetTabArea()))
						{

							bCheckWidgetClicks = true;

							int iTab = 0;
							int TabCount = window->Tabs.size();
							if (TabCount) // If there are some tabs
							{
								int TabSize = UI_TAB_HEIGHT;
								int Dist = Mouse.y - (window->m_y + UI_WIN_TITLEHEIGHT + UI_WIN_TOPHEIGHT);
								if (Dist < (UI_TAB_HEIGHT*TabCount))
								{
									while (Dist > TabSize)
									{
										if (Dist > TabSize)
										{
											iTab++;
											Dist -= TabSize;
										}
										if (iTab == (TabCount - 1))
										{
											break;
										}
									}
									window->SelectedTab = window->Tabs[iTab];

									// Loose focus on the control
									bCheckWidgetClicks = false;
									window->IsFocusingControl = false;
									window->FocusedControl = nullptr;
								}
							}

						}
					// Is it inside the client area?
						else if (IsMouseInRegion(window->GetClientArea()))
						{
							bCheckWidgetClicks = true;
						}
						else
						{
							// Must be in the around the title or side of the window
							// So we assume the user is trying to drag the window
							IsDraggingWindow = true;
							DraggingWindow = window;
							DragOffsetX = Mouse.x - window->m_x;
							DragOffsetY = Mouse.y - window->m_y;

							// Loose focus on the control
							window->IsFocusingControl = false;
							window->FocusedControl = nullptr;
						}
				}
				else
				{
					// Loose focus on the control
					window->IsFocusingControl = false;
					window->FocusedControl = nullptr;
				}
			}

			// Controls 
			if (window->SelectedTab != nullptr)
			{
				// Focused widget
				bool SkipWidget = false;
				CControl* SkipMe = nullptr;

				// this window is focusing on a widget??
				if (window->IsFocusingControl)
				{
					if (window->FocusedControl != nullptr)
					{
						// We've processed it once, skip it later
						SkipWidget = true;
						SkipMe = window->FocusedControl;

						POINT cAbs = window->FocusedControl->GetAbsolutePos();
						RECT controlRect = { cAbs.x, cAbs.y, window->FocusedControl->m_iWidth, window->FocusedControl->m_iHeight };
						window->FocusedControl->OnUpdate();

						if (window->FocusedControl->Flag(UIFlags::UI_Clickable) && IsMouseInRegion(controlRect) && bCheckWidgetClicks)
						{
							window->FocusedControl->OnClick();

							// If it gets clicked we loose focus
							window->IsFocusingControl = false;
							window->FocusedControl = nullptr;
							bCheckWidgetClicks = false;
						}
					}
				}

				for (auto control : window->SelectedTab->Controls)
				{
					if (control != nullptr)
					{
						if (SkipWidget && SkipMe == control)
							continue;

						control->parent = window;

						POINT cAbs = control->GetAbsolutePos();
						RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
						control->OnUpdate();

						if (control->Flag(UIFlags::UI_Clickable) && IsMouseInRegion(controlRect) && bCheckWidgetClicks)
						{
							control->OnClick();
							bCheckWidgetClicks = false;

							// Change of focus
							if (control->Flag(UIFlags::UI_Focusable))
							{
								window->IsFocusingControl = true;
								window->FocusedControl = control;
							}
							else
							{
								window->IsFocusingControl = false;
								window->FocusedControl = nullptr;
							}

						}
					}
				}

				// We must have clicked whitespace
				if (bCheckWidgetClicks)
				{
					// Loose focus on the control
					window->IsFocusingControl = false;
					window->FocusedControl = nullptr;
				}
			}
		}
	}
}


// Returns 
bool CGUI::GetKeyPress(unsigned int key)
{
	if (keys[key] == true && oldKeys[key] == false)
		return true;
	else
		return false;
}

bool CGUI::GetKeyState(unsigned int key)
{
	return keys[key];
}

bool CGUI::IsMouseInRegion(int y, int x2, int y2, int x)
{
	if (Mouse.x > x && Mouse.y > y && Mouse.x < x2 && Mouse.y < y2)
		return true;
	else
		return false;
}

bool CGUI::IsMouseInRegion(RECT region)
{
	return IsMouseInRegion(region.top, region.left + region.right, region.top + region.bottom, region.left);
}

POINT CGUI::GetMouse()
{
	return Mouse;
}

bool CGUI::DrawWindow(CWindow* window)
{
	Render::Clear(window->m_x, window->m_y, window->m_iWidth, window->m_iHeight, Color(25, 25, 25, 255));
	//Render::Text(window->m_x + 420, window->m_y + 10, Color(255, 255, 255, 255), Render::Fonts::MenuBold, "poseidon");

	//welcome message
#define UNLEN 256
	char buffer[UNLEN + 1];
	DWORD size;
	size = sizeof(buffer);
	GetUserName(buffer, &size);
	char title[UNLEN];
	char ch1[25] = "Welcome, ";
	char *ch = strcat(ch1, buffer);

	if (Menu::Window.VisualsTab.WelcomeMessage.GetState())
	{
		Render::Text(window->m_x + 25, window->m_y + 428, Color(255, 255, 255, 255), Render::Fonts::MenuBold, ch);
	}


	//rainbow bar
	Render::GradientH(window->m_x + 7, window->m_y + 7, (window->m_iWidth - 14) / 2, 2, Color(0 * (255 / 255.f), 160 * (255 / 255.f), 255 * (255 / 255.f), 255), Color(160 * (255 / 255.f), 0 * (255 / 255.f), 255 * (255 / 255.f), 255));
	Render::GradientH(window->m_x + 7 + (window->m_iWidth - 14) / 2, window->m_y + 7, (window->m_iWidth - 14) / 2, 2, Color(160 * (255 / 255.f), 0 * (255 / 255.f), 255 * (255 / 255.f), 255), Color(255 * (255 / 255.f), 255 * (255 / 255.f), 0 * (255 / 255.f), 255));
	Render::Clear(window->m_x + 7, window->m_y + 8, (window->m_iWidth - 14), 2, Color(0 * (255 / 255.f), 0 * (255 / 255.f), 0 * (255 / 255.f), 200 * (255 / 255.f)));

	//Inner
	//Render::Outline(window->m_x + 7, window->m_y + 1 + 26, window->m_iWidth - 4 - 10, window->m_iHeight - 2 - 6 - 26, Color(0, 150, 0, MenuAlpha));
	//Render::Clear(window->m_x + 8, window->m_y + 1 + 27, window->m_iWidth - 4 - 12, window->m_iHeight - 2 - 8 - 26, Color(255, 255, 255, MenuAlpha));

	Render::Outline(window->m_x, window->m_y, window->m_iWidth, window->m_iHeight, Color(0, 0, 0, 255));
	Render::Outline(window->m_x + 1, window->m_y + 1, window->m_iWidth - 2, window->m_iHeight - 2, Color(65, 65, 65, 255));
	Render::Outline(window->m_x + 2, window->m_y + 2, window->m_iWidth - 4, window->m_iHeight - 4, Color(45, 45, 45, 255));
	Render::Outline(window->m_x + 3, window->m_y + 3, window->m_iWidth - 6, window->m_iHeight - 6, Color(45, 45, 45, 255));
	Render::Outline(window->m_x + 4, window->m_y + 4, window->m_iWidth - 8, window->m_iHeight - 8, Color(45, 45, 45, 255));
	Render::Outline(window->m_x + 5, window->m_y + 5, window->m_iWidth - 10, window->m_iHeight - 10, Color(65, 65, 65, 255));

	int TabCount = window->Tabs.size();
	if (TabCount) // If there are some tabs
	{
		for (int i = 0; i < TabCount; i++)
		{
			RECT TabArea = { window->m_x, window->m_y + UI_WIN_TITLEHEIGHT + UI_WIN_TOPHEIGHT + (i*UI_TAB_HEIGHT) , UI_TAB_WIDTH, UI_TAB_HEIGHT };
			CTab *tab = window->Tabs[i];



			Color txtColor = UI_COL_SHADOW;

			if (window->SelectedTab == tab)
			{
				// Selected
				txtColor = UI_COL_TABSEPERATOR;
			}
			else if (IsMouseInRegion(TabArea))
			{
				// Hover
				txtColor = Color(Menu::Window.ColorsTab.ColorMenuAccentR.GetValue(), Menu::Window.ColorsTab.ColorMenuAccentG.GetValue(), Menu::Window.ColorsTab.ColorMenuAccentB.GetValue(), 255);
			}

			Render::Text(TabArea.left + 15, TabArea.top + 8, txtColor, Render::Fonts::Tabs, tab->Title.c_str());
			//Render::Clear(window->m_x + 8, window->m_y + 1 + 27, window->m_iWidth - 4 - 12, 2, Color(150, 0, 0, 255));
		}
	}




	// Controls 
	if (window->SelectedTab != nullptr)
	{
		// Focused widget
		bool SkipWidget = false;
		CControl* SkipMe = nullptr;

		// this window is focusing on a widget??
		if (window->IsFocusingControl)
		{
			if (window->FocusedControl != nullptr)
			{
				// We need to draw it last, so skip it in the regular loop
				SkipWidget = true;
				SkipMe = window->FocusedControl;
			}
		}


		// Itterate over all the other controls
		for (auto control : window->SelectedTab->Controls)
		{
			if (SkipWidget && SkipMe == control)
				continue;

			if (control != nullptr && control->Flag(UIFlags::UI_Drawable))
			{
				POINT cAbs = control->GetAbsolutePos();
				RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
				bool hover = false;
				if (IsMouseInRegion(controlRect))
				{
					hover = true;
				}
				control->Draw(hover);
			}
		}

		// Draw the skipped widget last
		if (SkipWidget)
		{
			auto control = window->FocusedControl;

			if (control != nullptr && control->Flag(UIFlags::UI_Drawable))
			{
				POINT cAbs = control->GetAbsolutePos();
				RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
				bool hover = false;
				if (IsMouseInRegion(controlRect))
				{
					hover = true;
				}
				control->Draw(hover);
			}
		}

	}


	return true;
}

void CGUI::RegisterWindow(CWindow* window)
{
	Windows.push_back(window);

	// Resorting to put groupboxes at the start
	for (auto tab : window->Tabs)
	{
		for (auto control : tab->Controls)
		{
			if (control->Flag(UIFlags::UI_RenderFirst))
			{
				CControl * c = control;
				tab->Controls.erase(std::remove(tab->Controls.begin(), tab->Controls.end(), control), tab->Controls.end());
				tab->Controls.insert(tab->Controls.begin(), control);
			}
		}
	}
}

void CGUI::BindWindow(unsigned char Key, CWindow* window)
{
	if (window)
		WindowBinds[Key] = window;
	else
		WindowBinds.erase(Key);
}

void CGUI::SaveWindowState(CWindow* window, std::string Filename)
{
	// Create a whole new document and we'll just save over top of the old one
	tinyxml2::XMLDocument Doc;

	// Root Element is called "AC"
	tinyxml2::XMLElement *Root = Doc.NewElement("AC");
	Doc.LinkEndChild(Root);

	Utilities::Log("Saving Window %s", window->Title.c_str());

	// If the window has some tabs..
	if (Root && window->Tabs.size() > 0)
	{
		for (auto Tab : window->Tabs)
		{
			// Add a new section for this tab
			tinyxml2::XMLElement *TabElement = Doc.NewElement(Tab->Title.c_str());
			Root->LinkEndChild(TabElement);

			Utilities::Log("Saving Tab %s", Tab->Title.c_str());

			// Now we itterate the controls this tab contains
			if (TabElement && Tab->Controls.size() > 0)
			{
				for (auto Control : Tab->Controls)
				{
					// If the control is ok to be saved
					if (Control && Control->Flag(UIFlags::UI_SaveFile) && Control->FileIdentifier.length() > 1 && Control->FileControlType)
					{
						// Create an element for the control
						tinyxml2::XMLElement *ControlElement = Doc.NewElement(Control->FileIdentifier.c_str());
						TabElement->LinkEndChild(ControlElement);

						Utilities::Log("Saving control %s", Control->FileIdentifier.c_str());

						if (!ControlElement)
						{
							Utilities::Log("Errorino :("); // s0 cute
							return;
						}

						CCheckBox* cbx = nullptr;
						CComboBox* cbo = nullptr;
						CKeyBind* key = nullptr;
						CSlider* sld = nullptr;

						// Figure out what kind of control and data this is
						switch (Control->FileControlType)
						{
						case UIControlTypes::UIC_CheckBox:
							cbx = (CCheckBox*)Control;
							ControlElement->SetText(cbx->GetState());
							break;
						case UIControlTypes::UIC_ComboBox:
							cbo = (CComboBox*)Control;
							ControlElement->SetText(cbo->GetIndex());
							break;
						case UIControlTypes::UIC_KeyBind:
							key = (CKeyBind*)Control;
							ControlElement->SetText(key->GetKey());
							break;
						case UIControlTypes::UIC_Slider:
							sld = (CSlider*)Control;
							ControlElement->SetText(sld->GetValue());
							break;
						}
					}
				}
			}
		}
	}

	//Save the file
	if (Doc.SaveFile(Filename.c_str()) != tinyxml2::XML_NO_ERROR)
	{
		MessageBox(NULL, "Failed To Save Config File!", "poseidon.cc", MB_OK);
	}

}
void CGUI::LoadWindowState(CWindow* window, std::string Filename)
{
	tinyxml2::XMLDocument Doc;
	if (Doc.LoadFile(Filename.c_str()) == tinyxml2::XML_NO_ERROR)
	{
		tinyxml2::XMLElement *Root = Doc.RootElement();

		if (Root)
		{
			if (Root && window->Tabs.size() > 0)
			{
				for (auto Tab : window->Tabs)
				{
					tinyxml2::XMLElement *TabElement = Root->FirstChildElement(Tab->Title.c_str());
					if (TabElement)
					{
						if (TabElement && Tab->Controls.size() > 0)
						{
							for (auto Control : Tab->Controls)
							{
								if (Control && Control->Flag(UIFlags::UI_SaveFile) && Control->FileIdentifier.length() > 1 && Control->FileControlType)
								{
									tinyxml2::XMLElement *ControlElement = TabElement->FirstChildElement(Control->FileIdentifier.c_str());

									if (ControlElement)
									{
										CCheckBox* cbx = nullptr;
										CComboBox* cbo = nullptr;
										CKeyBind* key = nullptr;
										CSlider* sld = nullptr;

										switch (Control->FileControlType)
										{
										case UIControlTypes::UIC_CheckBox:
											cbx = (CCheckBox*)Control;
											cbx->SetState(ControlElement->GetText()[0] == '1' ? true : false);
											break;
										case UIControlTypes::UIC_ComboBox:
											cbo = (CComboBox*)Control;
											cbo->SelectIndex(atoi(ControlElement->GetText()));
											break;
										case UIControlTypes::UIC_KeyBind:
											key = (CKeyBind*)Control;
											key->SetKey(atoi(ControlElement->GetText()));
											break;
										case UIControlTypes::UIC_Slider:
											sld = (CSlider*)Control;
											sld->SetValue(atof(ControlElement->GetText()));
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}