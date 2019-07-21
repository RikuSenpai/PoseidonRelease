
#pragma once

#include "RenderManager.h"

#define _CRT_SECURE_NO_WARNINGS

// Font Instances
namespace Render
{
	// Text functions
	namespace Fonts
	{
		DWORD Default;
		DWORD Menu;
		DWORD MenuBold;
		DWORD Tabs;
		DWORD BOMB;
		DWORD Icon;
		DWORD WaterMark;
		DWORD Slider;
		DWORD ESP;
		DWORD Clock;
		DWORD LBY;
		DWORD xsESP;
		DWORD sESP;
		DWORD PESP;
		DWORD Health;
		DWORD MenuText;
		DWORD Weapons;
		DWORD supremacy;
	};
};

// We don't use these anywhere else, no reason for them to be
// available anywhere else
enum EFontFlags
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

// Initialises the rendering system, setting up fonts etc
void Render::Initialise()
{
	Fonts::Default = 0x1D; // MainMenu Font from vgui_spew_fonts 
	Fonts::Menu = Interfaces::Surface->FontCreate();
	Fonts::MenuBold = Interfaces::Surface->FontCreate();
	Fonts::ESP = Interfaces::Surface->FontCreate();
	Fonts::MenuText = Interfaces::Surface->FontCreate();
	Fonts::Tabs = Interfaces::Surface->FontCreate();
	Fonts::Icon = Interfaces::Surface->FontCreate();
	Fonts::BOMB = Interfaces::Surface->FontCreate();
	Fonts::WaterMark = Interfaces::Surface->FontCreate();
	Fonts::Slider = Interfaces::Surface->FontCreate();
	Fonts::Clock = Interfaces::Surface->FontCreate();
	Fonts::LBY = Interfaces::Surface->FontCreate();
	Fonts::xsESP = Interfaces::Surface->FontCreate();
	Fonts::Health = Interfaces::Surface->FontCreate();
	Fonts::sESP = Interfaces::Surface->FontCreate();
	Fonts::PESP = Interfaces::Surface->FontCreate();
	Fonts::Weapons = Interfaces::Surface->FontCreate();
	Fonts::supremacy = Interfaces::Surface->FontCreate();


	Interfaces::Surface->SetFontGlyphSet(Fonts::Menu, "Calibri", 14, 500, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	Interfaces::Surface->SetFontGlyphSet(Fonts::MenuBold, "Tahoma", 12, 200, 0, 0, FONTFLAG_DROPSHADOW);
	Interfaces::Surface->SetFontGlyphSet(Fonts::Slider, "Verdana Bold", 12, 500, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	//	Interfaces::Surface->SetFontGlyphSet(Fonts::Slider, "Verdana", 11, 600, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW | FONTFLAG_OUTLINE);
	Interfaces::Surface->SetFontGlyphSet(Fonts::WaterMark, "SWEETREVENGE", 28, 600, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE | FONTFLAG_DROPSHADOW); // REPLACE THIS WITH A WATERMARK BECAUSE IM TO LAZY TO TO IT, I NEED SLEEP || OK BRO NO WORRIES
	Interfaces::Surface->SetFontGlyphSet(Fonts::Clock, "Verdana", 16, 600, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
	Interfaces::Surface->SetFontGlyphSet(Fonts::Tabs, "Impact", 42, 400, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	Interfaces::Surface->SetFontGlyphSet(Fonts::BOMB, "Tahoma", 35, 800, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	Interfaces::Surface->SetFontGlyphSet(Fonts::Icon, "Undefeated", 35, 800, 0, 0, FONTFLAG_ANTIALIAS);
	//	Interfaces::Surface->SetFontGlyphSet(Fonts::MenuBold, "Visitor TT2 BRK", 14, 550, 0, 0, FONTFLAG_ANTIALIAS);
	Interfaces::Surface->SetFontGlyphSet(Fonts::ESP, "Tahoma", 11, 600, 0, 0, FONTFLAG_DROPSHADOW); //name and weapon text
	Interfaces::Surface->SetFontGlyphSet(Fonts::Weapons, "Verdana", 12, 400, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	Interfaces::Surface->SetFontGlyphSet(Fonts::PESP, "arial", 11, 700, 0, 0, FONTFLAG_NONE);
	Interfaces::Surface->SetFontGlyphSet(Fonts::MenuText, "Calibri", 16, 500, 0, 0, FONTFLAG_ANTIALIAS);
	Interfaces::Surface->SetFontGlyphSet(Fonts::LBY, "Verdana", 30, FW_EXTRABOLD, 0, FONTFLAG_OUTLINE, FONTFLAG_ANTIALIAS);
	Interfaces::Surface->SetFontGlyphSet(Fonts::Health, "Tahoma", 11, 200, 0, 0, FONTFLAG_OUTLINE); //bars
	Interfaces::Surface->SetFontGlyphSet(Fonts::sESP, "Tahoma", 11, 500, 0, 0, FONTFLAG_DROPSHADOW); //vulnerabilities and ammo
	Interfaces::Surface->SetFontGlyphSet(Fonts::xsESP, "Verdana", 10, 200, 0, 0, FONTFLAG_DROPSHADOW); //helmet/kevlar
	Interfaces::Surface->SetFontGlyphSet(Fonts::supremacy, "Verdana", 12, 0, 0, 0, FONTFLAG_NONE);// "Consolas", 12, 500, 0, 0, FONTFLAG_OUTLINE);


	Utilities::Log("Render System Ready");
}

RECT Render::GetViewport()
{
	RECT Viewport = { 0, 0, 0, 0 };
	int w, h;
	Interfaces::Engine->GetScreenSize(w, h);
	Viewport.right = w; Viewport.bottom = h;
	return Viewport;
}

void Render::DrawRect(int x, int y, int w, int h, Color col)
{
	Interfaces::Surface->DrawSetColor(col);
	Interfaces::Surface->DrawFilledRect(x, y, x + w, y + h);
}


void Render::DrawCircle(float x, float y, float r, float segments, Color color)
{
	Interfaces::Surface->DrawSetColor(color);
	Interfaces::Surface->DrawOutlinedCircle(x, y, r, segments);
}

void Render::DrawRectRainbow(int x, int y, int width, int height, float flSpeed, float &flRainbow)
{
	Color colColor(0, 0, 0);

	flRainbow += 0.0004f;
	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;

		Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
		Render::DrawRect(x + i, y, 1, height, colRainbow);
	}
}

void Render::Clear(int x, int y, int w, int h, Color color)
{
	Interfaces::Surface->DrawSetColor(color);
	Interfaces::Surface->DrawFilledRect(x, y, x + w, y + h);
}

void Render::Outline(int x, int y, int w, int h, Color color)
{
	Interfaces::Surface->DrawSetColor(color);
	Interfaces::Surface->DrawOutlinedRect(x, y, x + w, y + h);
}

#define M_PI 3.14159265358979323846

void Render::DrawTexturedPoly(int n, Vertex_t* vertice, Color col)
{

	static int texture_id = Interfaces::Surface->CreateNewTextureID(true);
	static unsigned char buf[4] = { 255, 255, 255, 255 };

	Interfaces::Surface->DrawSetTextureRGBA(texture_id, buf, 1, 1);
	Interfaces::Surface->DrawSetColor(col);

	Interfaces::Surface->DrawSetTexture(texture_id);
	Interfaces::Surface->DrawTexturedPolygon(n, vertice);
}

void Render::DrawFilledCircle(Vector2D center, Color color, float radius, float points) {
	std::vector<Vertex_t> vertices;
	float step = (float)PI * 2.0f / points;

	for (float a = 0; a < (PI * 2.0f); a += step)
		vertices.push_back(Vertex_t(Vector2D(radius * cosf(a) + center.x, radius * sinf(a) + center.y)));

	DrawTexturedPoly((int)points, vertices.data(), color);
}

void Render::Line(int x, int y, int x2, int y2, Color color)
{
	Interfaces::Surface->DrawSetColor(color);
	Interfaces::Surface->DrawLine(x, y, x2, y2);
}

void Render::PolyLine(int *x, int *y, int count, Color color)
{
	Interfaces::Surface->DrawSetColor(color);
	Interfaces::Surface->DrawPolyLine(x, y, count);
}

bool Render::WorldToScreen(Vector &in, Vector &out)
{
	const matrix3x4& worldToScreen = Interfaces::Engine->WorldToScreenMatrix(); //Grab the world to screen matrix from CEngineClient::WorldToScreenMatrix

	float w = worldToScreen[3][0] * in[0] + worldToScreen[3][1] * in[1] + worldToScreen[3][2] * in[2] + worldToScreen[3][3]; //Calculate the angle in compareson to the player's camera.
	out.z = 0; //Screen doesn't have a 3rd dimension.

	if (w > 0.001) //If the object is within view.
	{
		RECT ScreenSize = GetViewport();
		float fl1DBw = 1 / w; //Divide 1 by the angle.
		out.x = (ScreenSize.right / 2) + (0.5f * ((worldToScreen[0][0] * in[0] + worldToScreen[0][1] * in[1] + worldToScreen[0][2] * in[2] + worldToScreen[0][3]) * fl1DBw) * ScreenSize.right + 0.5f); //Get the X dimension and push it in to the Vector.
		out.y = (ScreenSize.bottom / 2) - (0.5f * ((worldToScreen[1][0] * in[0] + worldToScreen[1][1] * in[1] + worldToScreen[1][2] * in[2] + worldToScreen[1][3]) * fl1DBw) * ScreenSize.bottom + 0.5f); //Get the Y dimension and push it in to the Vector.
		return true;
	}

	return false;
}

void Render::Text(int x, int y, Color color, DWORD font, const char* text)
{
	size_t origsize = strlen(text) + 1;
	const size_t newsize = 500;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);

	Interfaces::Surface->DrawSetTextFont(font);

	Interfaces::Surface->DrawSetTextColor(color);
	Interfaces::Surface->DrawSetTextPos(x, y);
	Interfaces::Surface->DrawPrintText(wcstring, wcslen(wcstring));
	//return;
};
/*
void Render::Text(int x, int y, Color color, DWORD font, const wchar_t* text)
{
Interfaces::Surface->DrawSetTextFont(font);
Interfaces::Surface->DrawSetTextColor(color);
Interfaces::Surface->DrawSetTextPos(x, y);
Interfaces::Surface->DrawPrintText(text, wcslen(text));
}*/

void Render::Textf(int x, int y, Color color, DWORD font, const char* fmt, ...)
{
	if (!fmt) return; //if the passed string is null return
	if (strlen(fmt) < 2) return;

	//Set up va_list and buffer to hold the params 
	va_list va_alist;
	char logBuf[256] = { 0 };

	//Do sprintf with the parameters
	va_start(va_alist, fmt);
	_vsnprintf_s(logBuf + strlen(logBuf), 256 - strlen(logBuf), sizeof(logBuf) - strlen(logBuf), fmt, va_alist);
	va_end(va_alist);

	Text(x, y, color, font, logBuf);
}

RECT Render::GetTextSize(DWORD font, const char* text)
{
	size_t origsize = strlen(text) + 1;
	const size_t newsize = 200;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);

	RECT rect; int x, y;
	Interfaces::Surface->GetTextSize(font, wcstring, x, y);
	rect.left = x; rect.bottom = y;
	rect.right = x;
	return rect;
}

void Render::GradientV(int x, int y, int w, int h, Color c1, Color c2)
{
	Clear(x, y, w, h, c1);
	BYTE first = c2.r();
	BYTE second = c2.g();
	BYTE third = c2.b();
	for (int i = 0; i < h; i++)
	{
		float fi = i, fh = h;
		float a = fi / fh;
		DWORD ia = a * 255;
		Clear(x, y + i, w, 1, Color(first, second, third, ia));
	}
}

void Render::GradientH(int x, int y, int w, int h, Color c1, Color c2)
{
	Clear(x, y, w, h, c1);
	BYTE first = c2.r();
	BYTE second = c2.g();
	BYTE third = c2.b();
	for (int i = 0; i < w; i++)
	{
		float fi = i, fw = w;
		float a = fi / fw;
		DWORD ia = a * 255;
		Clear(x + i, y, 1, h, Color(first, second, third, ia));
	}
}

void Render::Polygon(int count, Vertex_t* Vertexs, Color color)
{
	static int Texture = Interfaces::Surface->CreateNewTextureID(true); //need to make a texture with procedural true
	unsigned char buffer[4] = { 255, 255, 255, 255 };//{ color.r(), color.g(), color.b(), color.a() };

	Interfaces::Surface->DrawSetTextureRGBA(Texture, buffer, 1, 1); //Texture, char array of texture, width, height
	Interfaces::Surface->DrawSetColor(color); // keep this full color and opacity use the RGBA @top to set values.
	Interfaces::Surface->DrawSetTexture(Texture); // bind texture

	Interfaces::Surface->DrawTexturedPolygon(count, Vertexs);
}

void Render::PolygonOutline(int count, Vertex_t* Vertexs, Color color, Color colorLine)
{
	static int x[128];
	static int y[128];

	Render::Polygon(count, Vertexs, color);

	for (int i = 0; i < count; i++)
	{
		x[i] = Vertexs[i].m_Position.x;
		y[i] = Vertexs[i].m_Position.y;
	}

	Render::PolyLine(x, y, count, colorLine);
}

void Render::PolyLine(int count, Vertex_t* Vertexs, Color colorLine)
{
	static int x[128];
	static int y[128];

	for (int i = 0; i < count; i++)
	{
		x[i] = Vertexs[i].m_Position.x;
		y[i] = Vertexs[i].m_Position.y;
	}

	Render::PolyLine(x, y, count, colorLine);
}

bool Render::TransformScreen(const Vector& in, Vector& out)
{
	static ptrdiff_t ptrViewMatrix;
	if (!ptrViewMatrix)
	{//                                                          
		ptrViewMatrix = static_cast<ptrdiff_t>(Utilities::Memory::FindPatternV2("client_panorama.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9"));
		ptrViewMatrix += 0x3;
		ptrViewMatrix = *reinterpret_cast<uintptr_t*>(ptrViewMatrix);
		ptrViewMatrix += 176;
	}
	const matrix3x4& worldToScreen = Interfaces::Engine->WorldToScreenMatrix(); // matrix



	int ScrW, ScrH;

	Interfaces::Engine->GetScreenSize(ScrW, ScrH);

	float w = worldToScreen[3][0] * in[0] + worldToScreen[3][1] * in[1] + worldToScreen[3][2] * in[2] + worldToScreen[3][3];
	out.z = 0; // 0 poniewaz z nie jest nam potrzebne | uzywamy tylko wysokosci i szerokosci (x,y)
	if (w > 0.01)
	{
		float inverseWidth = 1 / w; // inverse na 1 pozycje ekranu
		out.x = (ScrW / 2) + (0.5 * ((worldToScreen[0][0] * in[0] + worldToScreen[0][1] * in[1] + worldToScreen[0][2] * in[2] + worldToScreen[0][3]) * inverseWidth) * ScrW + 0.5);
		out.y = (ScrH / 2) - (0.5 * ((worldToScreen[1][0] * in[0] + worldToScreen[1][1] * in[1] + worldToScreen[1][2] * in[2] + worldToScreen[1][3]) * inverseWidth) * ScrH + 0.5);
		return true;
	}
	return false;
}

int TweakColor(int c1, int c2, int variation)
{
	if (c1 == c2)
		return c1;
	else if (c1 < c2)
		c1 += variation;
	else
		c1 -= variation;
	return c1;
}

void Render::GradientSideways(int x, int y, int w, int h, Color color1, Color color2, int variation)
{
	int r1 = color1.r();
	int g1 = color1.g();
	int b1 = color1.b();
	int a1 = color1.a();

	int r2 = color2.r();
	int g2 = color2.g();
	int b2 = color2.b();
	int a2 = color2.a();

	for (int i = 0; i <= w; i++)
	{
		Render::DrawRect(x + i, y, 1, h, Color(r1, g1, b1, a1));
		r1 = TweakColor(r1, r2, variation);
		g1 = TweakColor(g1, g2, variation);
		b1 = TweakColor(b1, b2, variation);
		a1 = TweakColor(a1, a2, variation);
	}
}	