/* Reverse Engineer's Hex Editor
 * Copyright (C) 2024 Daniel Collins <solemnwarning@solemnwarning.net>
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

#include <wx/sizer.h>
#include <wx/stattext.h>

#include "App.hpp"
#include "SettingsDialogGeneral.hpp"

bool REHex::SettingsDialogGeneral::Create(wxWindow *parent)
{
	wxPanel::Create(parent);
	
	wxBoxSizer *top_sizer = new wxBoxSizer(wxVERTICAL);
	
	wxBoxSizer *cnm_sizer = new wxBoxSizer(wxHORIZONTAL);
	top_sizer->Add(cnm_sizer, 0, wxBOTTOM, SettingsDialog::MARGIN);
	
	cnm_sizer->Add(new wxStaticText(this, wxID_ANY, "Hex cursor navigation:"), 0, wxALIGN_CENTER_VERTICAL);
	
	cnm_byte = new wxRadioButton(this, wxID_ANY, "By byte");
	cnm_sizer->Add(cnm_byte, 0, (wxALIGN_CENTER_VERTICAL | wxLEFT), SettingsDialog::MARGIN);
	
	cnm_byte->SetToolTip("The arrow keys will move the cursor in whole byte increments");
	
	cnm_nibble = new wxRadioButton(this, wxID_ANY, "By nibble");
	cnm_sizer->Add(cnm_nibble, 0, (wxALIGN_CENTER_VERTICAL | wxLEFT), SettingsDialog::MARGIN);
	
	cnm_nibble->SetToolTip("The arrow keys will move the cursor in nibble (half byte) increments");
	
	switch(wxGetApp().settings->get_cursor_nav_mode())
	{
		case CursorNavMode::BYTE:
			cnm_byte->SetValue(true);
			break;
			
		case CursorNavMode::NIBBLE:
			cnm_nibble->SetValue(true);
			break;
	}
	
	goto_offset_modeless = new wxCheckBox(this, wxID_ANY, "Non-modal 'Jump to offset' dialog");
	top_sizer->Add(goto_offset_modeless);
	
	goto_offset_modeless->SetValue(!(wxGetApp().settings->get_goto_offset_modal()));
	goto_offset_modeless->SetToolTip("The 'Jump to offset' dialog will remain open after use and allow interacting with the editor window.");
	
	SetSizerAndFit(top_sizer);
	
	return true;
}

std::string REHex::SettingsDialogGeneral::label() const
{
	return "General";
}

bool REHex::SettingsDialogGeneral::validate() { return true; }

void REHex::SettingsDialogGeneral::save()
{
	if(cnm_byte->GetValue())
	{
		wxGetApp().settings->set_cursor_nav_mode(CursorNavMode::BYTE);
	}
	else if(cnm_nibble->GetValue())
	{
		wxGetApp().settings->set_cursor_nav_mode(CursorNavMode::NIBBLE);
	}
	
	wxGetApp().settings->set_goto_offset_modal(!(goto_offset_modeless->GetValue()));
}

void REHex::SettingsDialogGeneral::reset() {}
