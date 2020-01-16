#pragma once
// Font.h : Useful classes for drawing and formatting text strings

class Sprite;

enum FontEnum
{
	FONT_DEFAULT,
	FONT_SYSTEM = FONT_DEFAULT,		//0
	FONT_ARIAL_BOLD_8,				//1
	FONT_ARIAL_BOLD_14,				//2
	FONT_ARIAL_BLACK_12,			//3          
	FONT_ARIAL_BLACK_24,			//4
	FONT_WINGDING_10,				//5
	FONT_COURIERNEW_18,				//6
	FONT_PALATINO_SMALL,			//7
	FONT_PALATINO_MEDIUM,			//8
	FONT_PALATINO_LARGE,			//9
	FONT_LARGE_LED_DISPLAY,			//10
	FONT_TREBUCHET_10,				//11
	FONT_TREBUCHET_MED,				//12
	FONT_TAHOMA_MED,				//13
	FONT_TAHOMA_LARGE,				//14

	// Define more font enumerations here....and add definitions into 
	// FontHandler::m_FontRecords in Font.cpp

	MAX_FONTS						//15
};



struct TextShadow
{
	Color m_color;
	CPoint m_offset;

	TextShadow(Color const &color, CPoint const &offset=CPoint(1,1))
		: m_color(color), m_offset(offset) { }
};

struct TextStyle
{
	FontEnum m_font;
	Color m_foreground;
	Color m_background;
	int m_format;
	optional<TextShadow> m_shadow;

	TextStyle(
		FontEnum font, 
		Color foreground, 
		Color background=g_Transparent, 
		int format = DT_TOP | DT_LEFT,
		optional<TextShadow> shadow=optional_empty()
	);

	TextStyle & operator = (const TextStyle &sp);
};
	

//////////////////////////////////////////////////////////////////////
// Class Font
//////////////////////////////////////////////////////////////////////

// FUTURE WORK: This should inherit from an interface class, like IFont, to remove OS dependancy!!!

class Font
{
	friend class FontHandler;
public:
	Font(ID3DXFont *font) { m_d3dFont = font;  }
	virtual ~Font() { SAFE_RELEASE(m_d3dFont); }

private:
	ID3DXFont *m_d3dFont;
};


class FontHandler
{
protected:
	static D3DXFONT_DESC m_FontRecords[];
	Font *m_pFonts[MAX_FONTS];

	CSize GetStringSize(std::wstring const &s, Font *font, int format) const;

public:

	FontHandler();
	virtual ~FontHandler();

	CSize const GetStringSize(std::wstring const &s, TextStyle const style) const;
	int	GetHeight(TextStyle const style) const;
	int	GetStringWidth(std::wstring const &s, TextStyle style) const;
	std::wstring const ParseMessage(std::wstring const &s, TextStyle style, const CSize & size ) const;

	HRESULT FontHandler::DrawText(
		Sprite *pSprite,
		const std::wstring strText,
		const TextStyle style,
		const CPoint & origin
		) const;
};




