// joinery_solver_rhino7.h : main header file for the joinery_solver_rhino7 DLL.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h" // main symbols

// Cjoinery_solver_rhino7App
// See joinery_solver_rhino7App.cpp for the implementation of this class
//

class Cjoinery_solver_rhino7App : public CWinApp
{
public:
  // CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
  // Only standard MFC DLL instance construction belongs here. 
  // All other significant initialization should take place in
  // Cjoinery_solver_rhino7PlugIn::OnLoadPlugIn().
	Cjoinery_solver_rhino7App() = default;

  // CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
  // Only standard MFC DLL instance initialization belongs here. 
  // All other significant initialization should take place in
  // Cjoinery_solver_rhino7PlugIn::OnLoadPlugIn().
	BOOL InitInstance() override;
  
  // CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
  // Only standard MFC DLL instance clean up belongs here. 
  // All other significant cleanup should take place in either
  // Cjoinery_solver_rhino7PlugIn::OnSaveAllSettings() or
  // Cjoinery_solver_rhino7PlugIn::OnUnloadPlugIn().  
	int ExitInstance() override;
  
	DECLARE_MESSAGE_MAP()
};
