///////////// Copyright 2010 DesuraNet. All rights reserved. /////////////
//
//   Project     : desura_libcef_dll_wrapper
//   File        : MenuInfo.cpp
//   Description :
//      [TODO: Write the purpose of MenuInfo.cpp.]
//
//   Created On: 9/18/2010 10:01:52 AM
//   Created By:  <mailto:>
////////////////////////////////////////////////////////////////////////////

#include "MenuInfo.h"

ChromiumMenuItem::ChromiumMenuItem( int action, int type, const char* label, bool isEnabled, bool isChecked )
	: m_action( action )
	, m_type( type )
	, m_label( label )
	, m_isEnabled( isEnabled )
	, m_isChecked( isChecked )
{
}

int ChromiumMenuItem::getAction()
{
	return m_action;
}

int ChromiumMenuItem::getType()
{
	return m_type;
}

const char* ChromiumMenuItem::getLabel()
{
	return m_label;
}

bool ChromiumMenuItem::isEnabled()
{
	return m_isEnabled;
}

bool ChromiumMenuItem::isChecked()
{
	return m_isChecked;
}




ChromiumMenuInfo::ChromiumMenuInfo(CefRefPtr<CefContextMenuParams> info, CefRefPtr<CefMenuModel> model, MenuHandle_t hwnd)
{
	m_MenuInfo = info;
	m_Hwnd = hwnd;

	for (int x=0; x < model->GetCount(); x++)
	{
		m_vMenuItems.push_back( ChromiumMenuItem( model->GetCommandIdAt( x ), model->GetTypeAt( x ), (const char*) model->GetLabelAt( x ).c_str(), model->IsEnabledAt( x ), model->IsCheckedAt( x ) ) );
	}
}

ChromiumDLL::ChromiumMenuInfoI::TypeFlags ChromiumMenuInfo::getTypeFlags()
{
	return (ChromiumDLL::ChromiumMenuInfoI::TypeFlags)m_MenuInfo->GetTypeFlags();
}

ChromiumDLL::ChromiumMenuInfoI::EditFlags ChromiumMenuInfo::getEditFlags()
{
	return (ChromiumDLL::ChromiumMenuInfoI::EditFlags)m_MenuInfo->GetEditStateFlags();
}

void ChromiumMenuInfo::getMousePos(int* x, int* y)
{
	*x = m_MenuInfo->GetXCoord();
	*y = m_MenuInfo->GetYCoord();
}

const char* ChromiumMenuInfo::getLinkUrl()
{
	return (const char*)m_MenuInfo->GetLinkUrl().c_str();
}

const char* ChromiumMenuInfo::getImageUrl()
{
	if ( m_MenuInfo->HasImageContents() && ( m_MenuInfo->GetMediaType() == CM_MEDIATYPE_IMAGE ) )
		return (const char*) m_MenuInfo->GetSourceUrl().c_str();
	else
		return nullptr;
}

const char* ChromiumMenuInfo::getPageUrl()
{
	return (const char*)m_MenuInfo->GetPageUrl().c_str();
}

const char* ChromiumMenuInfo::getFrameUrl()
{
	return (const char*)m_MenuInfo->GetFrameUrl().c_str();
}

const char* ChromiumMenuInfo::getSelectionText()
{
	return (const char*)m_MenuInfo->GetSelectionText().c_str();
}

const char* ChromiumMenuInfo::getMisSpelledWord()
{
	return nullptr;

	// No longer supported
	//return (const char*)m_MenuInfo.misspelledWord.str;
}

const char* ChromiumMenuInfo::getSecurityInfo()
{
	return nullptr;

	// No longer supported
	//return (const char*)m_MenuInfo.securityInfo.str;
}

int* ChromiumMenuInfo::getHWND()
{
	return (int*)m_Hwnd;
}

int ChromiumMenuInfo::getCustomCount()
{
	return m_vMenuItems.size();
}

ChromiumDLL::ChromiumMenuItemI* ChromiumMenuInfo::getCustomItem(size_t index)
{
	if (index >= m_vMenuItems.size())
		return nullptr;

	return &m_vMenuItems[index];
}
