// joinery_solver_rhino7.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "joinery_solver_rhino7App.h"

//
//	Note!
//
//    A Rhino plug-in is an MFC DLL.
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// Cjoinery_solver_rhino7App

BEGIN_MESSAGE_MAP(Cjoinery_solver_rhino7App, CWinApp)
END_MESSAGE_MAP()

// The one and only Cjoinery_solver_rhino7App object
Cjoinery_solver_rhino7App theApp;

// Cjoinery_solver_rhino7App initialization

BOOL Cjoinery_solver_rhino7App::InitInstance()
{
  // CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
  // Only standard MFC DLL instance initialization belongs here. 
  // All other significant initialization should take place in
  // Cjoinery_solver_rhino7PlugIn::OnLoadPlugIn().

	CWinApp::InitInstance();

	return TRUE;
}

int Cjoinery_solver_rhino7App::ExitInstance()
{
  // CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
  // Only standard MFC DLL instance clean up belongs here. 
  // All other significant cleanup should take place in either
  // Cjoinery_solver_rhino7PlugIn::OnSaveAllSettings() or
  // Cjoinery_solver_rhino7PlugIn::OnUnloadPlugIn().
  return CWinApp::ExitInstance();
}
