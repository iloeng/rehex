/* Reverse Engineer's Hex Editor
 * Copyright (C) 2018-2025 Daniel Collins <solemnwarning@solemnwarning.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "platform.hpp"

#include <assert.h>
#include <memory>
#include <wx/colour.h>
#include <wx/settings.h>

#include "App.hpp"
#include "Palette.hpp"
#include "HSVColour.hpp"

bool REHex::Palette::is_colour_light(const wxColour &colour)
{
	return ((int)(colour.Red()) + (int)(colour.Green()) + (int)(colour.Blue())) / 3 >= 128;
}

REHex::Palette *REHex::active_palette = NULL;

REHex::Palette::Palette(const std::string &name, const std::string &label, const wxColour colours[], int default_highlight_lightness):
	name(name), label(label), default_highlight_lightness(default_highlight_lightness)
{
	for(int i = 0; i <= PAL_MAX; ++i)
	{
		palette[i] = colours[i];
	}
}

const std::string &REHex::Palette::get_name() const
{
	return name;
}

const std::string &REHex::Palette::get_label() const
{
	return label;
}

const wxColour &REHex::Palette::operator[](int index) const
{
	assert(index >= 0);
	assert(index <= PAL_MAX);
	
	return palette[index];
}

wxColour REHex::Palette::blend_colours(int bg_idx, int fg_idx, float fg_opacity) const
{
	const wxColour &bg = (*this)[bg_idx];
	const wxColour &fg = (*this)[fg_idx];
	
	return blend_colours(bg, fg, fg_opacity);
}

wxColour REHex::Palette::blend_colours(int bg_idx, const wxColour &fg, float fg_opacity) const
{
	return blend_colours((*this)[bg_idx], fg, fg_opacity);
}

wxColour REHex::Palette::blend_colours(const wxColour &bg, int fg_idx, float fg_opacity) const
{
	return blend_colours(bg, (*this)[fg_idx], fg_opacity);
}

wxColour REHex::Palette::blend_colours(const wxColour &bg, const wxColour &fg, float fg_opacity)
{
	return wxColour(
		wxColour::AlphaBlend(fg.Red(), bg.Red(), fg_opacity),
		wxColour::AlphaBlend(fg.Green(), bg.Green(), fg_opacity),
		wxColour::AlphaBlend(fg.Blue(), bg.Blue(), fg_opacity));
}

int REHex::Palette::get_default_highlight_lightness() const
{
	return default_highlight_lightness;
}

REHex::Palette *REHex::Palette::create_system_palette()
{
	const wxColour WINDOW        = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	const wxColour WINDOWTEXT    = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
	const wxColour HIGHLIGHT     = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
	const wxColour HIGHLIGHTTEXT = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
	
	std::unique_ptr<Palette> light_palette(create_light_palette());
	std::unique_ptr<Palette> dark_palette(create_dark_palette());
	
	Palette *base_palette = is_colour_light(WINDOW)
		? light_palette.get()
		: dark_palette.get();
	
	const wxColour colours[] = {
		WINDOW,      /* PAL_NORMAL_TEXT_BG */
		WINDOWTEXT,  /* PAL_NORMAL_TEXT_FG */
		
		(is_colour_light(WINDOWTEXT)
			? WINDOWTEXT.ChangeLightness(70)
			: WINDOWTEXT.ChangeLightness(130)),  /* PAL_ALTERNATE_TEXT_FG */
		
		WINDOWTEXT,  /* PAL_INVERT_TEXT_BG */
		WINDOW,      /* PAL_INVERT_TEXT_FG */
		
		HIGHLIGHT,      /* PAL_SELECTED_TEXT_BG */
		HIGHLIGHTTEXT,  /* PAL_SELECTED_TEXT_FG */
		
		(is_colour_light(HIGHLIGHT)
			? HIGHLIGHT.ChangeLightness(70)
			: HIGHLIGHT.ChangeLightness(130)),  /* PAL_SECONDARY_SELECTED_TEXT_BG */
		
		(is_colour_light(HIGHLIGHTTEXT)
			? HIGHLIGHTTEXT.ChangeLightness(70)
			: HIGHLIGHTTEXT.ChangeLightness(130)),  /* PAL_SECONDARY_SELECTED_TEXT_FG */
		
		WINDOW,                      /* PAL_DIRTY_TEXT_BG */
		wxColour(0xFF, 0x00, 0x00),  /* PAL_DIRTY_TEXT_FG */
		
		(is_colour_light(WINDOW)
			? WINDOW.ChangeLightness(80)
			: WINDOW.ChangeLightness(130)),  /* PAL_COMMENT_BG */
		
		WINDOWTEXT,  /* PAL_COMMENT_FG */
		
		(*base_palette)[PAL_CONTRAST_TEXT_1_FG],
		(*base_palette)[PAL_CONTRAST_TEXT_2_FG],
		(*base_palette)[PAL_CONTRAST_TEXT_3_FG],
		(*base_palette)[PAL_CONTRAST_TEXT_4_FG],
		(*base_palette)[PAL_CONTRAST_TEXT_5_FG],
	};
	
	static_assert(sizeof(colours) == sizeof(palette), "Correct number of colours for Palette");
	
	int default_highlight_lightness = is_colour_light(WINDOW) ? 100 : 80;
	
	return new Palette("system", "System colours", colours, default_highlight_lightness);
}

REHex::Palette *REHex::Palette::create_light_palette()
{
	const wxColour colours[] = {
		wxColour(0xFF, 0xFF, 0xFF),  /* PAL_NORMAL_TEXT_BG */
		wxColour(0x00, 0x00, 0x00),  /* PAL_NORMAL_TEXT_FG */
		wxColour(0x69, 0x69, 0x69),  /* PAL_ALTERNATE_TEXT_FG */
		wxColour(0x00, 0x00, 0x00),  /* PAL_INVERT_TEXT_BG */
		wxColour(0xFF, 0xFF, 0xFF),  /* PAL_INVERT_TEXT_FG */
		wxColour(0x00, 0x00, 0xFF),  /* PAL_SELECTED_TEXT_BG */
		wxColour(0xFF, 0xFF, 0xFF),  /* PAL_SELECTED_TEXT_FG */
		wxColour(0x00, 0x00, 0x7F),  /* PAL_SECONDARY_SELECTED_TEXT_BG */
		wxColour(0xFF, 0xFF, 0xFF),  /* PAL_SECONDARY_SELECTED_TEXT_FG */
		wxColour(0xFF, 0xFF, 0xFF),  /* PAL_DIRTY_TEXT_BG */
		wxColour(0xFF, 0x00, 0x00),  /* PAL_DIRTY_TEXT_FG */
		wxColour(0xD3, 0xD3, 0xD3),  /* PAL_COMMENT_BG */
		wxColour(0x00, 0x00, 0x00),  /* PAL_COMMENT_FG */
		
		// wxColour(0xEE, 0x3E, 0x34), /* PAL_CONTRAST_TEXT_1_FG */
		// wxColour(0xFF, 0x00, 0x00),
		wxColour(0xC0, 0x00, 0x00),
		
		// wxColour(0x1D, 0x4F, 0x9C), /* PAL_CONTRAST_TEXT_2_FG */
		// wxColour(0x00, 0x00, 0xFF),
		wxColour(0x00, 0x00, 0xC0),
		
		// wxColour(0x54, 0xB9, 0x48), /* PAL_CONTRAST_TEXT_3_FG */
		// wxColour(0x00, 0xC0, 0x00),
		wxColour(0x00, 0x80, 0x00),
		
		// wxColour(0xBD, 0xA0, 0xCC), /* PAL_CONTRAST_TEXT_4_FG */
		// wxColour(0x80, 0x00, 0x80),
		wxColour(0xC0, 0x00, 0xC0),
		
		// wxColour(0xF0, 0x92, 0x43), /* PAL_CONTRAST_TEXT_5_FG */
		wxColour(0xFF, 0x80, 0x00),
		// wxColour(0xC0, 0x58, 0x00),
	};
	
	static_assert(sizeof(colours) == sizeof(palette), "Correct number of colours for Palette");
	
	return new Palette("light", "Light", colours, 100);
}

REHex::Palette *REHex::Palette::create_dark_palette()
{
	const wxColour colours[] = {
		wxColour(0x00, 0x00, 0x00),  /* PAL_NORMAL_TEXT_BG */
		wxColour(0xFF, 0xFF, 0xFF),  /* PAL_NORMAL_TEXT_FG */
		wxColour(0xC3, 0xC3, 0xC3),  /* PAL_ALTERNATE_TEXT_FG */
		wxColour(0xFF, 0xFF, 0xFF),  /* PAL_INVERT_TEXT_BG */
		wxColour(0x00, 0x00, 0x00),  /* PAL_INVERT_TEXT_FG */
		wxColour(0x00, 0x00, 0xFF),  /* PAL_SELECTED_TEXT_BG */
		wxColour(0xFF, 0xFF, 0xFF),  /* PAL_SELECTED_TEXT_FG */
		wxColour(0x00, 0x00, 0x7F),  /* PAL_SECONDARY_SELECTED_TEXT_BG */
		wxColour(0xFF, 0xFF, 0xFF),  /* PAL_SECONDARY_SELECTED_TEXT_FG */
		wxColour(0x00, 0x00, 0x00),  /* PAL_DIRTY_TEXT_BG */
		wxColour(0xFF, 0x00, 0x00),  /* PAL_DIRTY_TEXT_FG */
		wxColour(0x58, 0x58, 0x58),  /* PAL_COMMENT_BG */
		wxColour(0xFF, 0xFF, 0xFF),  /* PAL_COMMENT_FG */
		
		// wxColour(0xEE, 0x3E, 0x34), /* PAL_CONTRAST_TEXT_1_FG */
		// wxColour(0xFF, 0x00, 0x00),
		wxColour(0xFF, 0x4D, 0x00),
		
		// wxColour(0x1D, 0x4F, 0x9C), /* PAL_CONTRAST_TEXT_2_FG */
		// wxColour(0x00, 0x00, 0xFF),
		wxColour(0x00, 0xB2, 0xFF),
		
		// wxColour(0x54, 0xB9, 0x48), /* PAL_CONTRAST_TEXT_3_FG */
		wxColour(0x00, 0xC0, 0x00),
		// wxColour(0x00, 0x80, 0x00),
		
		wxColour(0xBD, 0xA0, 0xCC), /* PAL_CONTRAST_TEXT_4_FG */
		// wxColour(0x80, 0x00, 0x80),
		// wxColour(0xC0, 0x00, 0xC0),
		
		wxColour(0xF0, 0x92, 0x43), /* PAL_CONTRAST_TEXT_5_FG */
		// wxColour(0xFF, 0x80, 0x00),
		// wxColour(0xC0, 0x58, 0x00),
	};
	
	static_assert(sizeof(colours) == sizeof(palette), "Correct number of colours for Palette");
	
	return new Palette("dark", "Dark", colours, 80);
}
