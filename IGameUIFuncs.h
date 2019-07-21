//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifdef _WIN32
#pragma once
#endif

#include <iostream>

class IGameUIFuncs
{
public:
	virtual bool		IsKeyDown(const char *keyname, bool& isdown) = 0;
	virtual const char	*GetBindingForButtonCode(/*ButtonCode_t*/int code) = 0;
	virtual int/*ButtonCode_t*/ GetButtonCodeForBind(const char *pBind) = 0;
	virtual void		GetVideoModes(struct vmode_s **liststart, int *count) = 0;
	virtual void		SetFriendsID(uint16_t friendsID, const char *friendsName) = 0;
	virtual void		GetDesktopResolution(int &width, int &height) = 0;
	virtual bool		IsConnectedToVACSecureServer() = 0;
};