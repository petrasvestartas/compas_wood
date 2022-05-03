// joinery_solver_rhino7PlugIn.cpp : defines the initialization routines for the plug-in.
//

#include "StdAfx.h"
#include "rhinoSdkPlugInDeclare.h"
#include "joinery_solver_rhino7PlugIn.h"
#include "Resource.h"

// The plug-in object must be constructed before any plug-in classes derived
// from CRhinoCommand. The #pragma init_seg(lib) ensures that this happens.
#pragma warning(push)
#pragma warning(disable : 4073)
#pragma init_seg(lib)
#pragma warning(pop)

// Rhino plug-in declaration
RHINO_PLUG_IN_DECLARE

// Rhino plug-in name
// Provide a short, friendly name for this plug-in.
RHINO_PLUG_IN_NAME(L"joinery_solver_rhino7");

// Rhino plug-in id
// Provide a unique uuid for this plug-in.
RHINO_PLUG_IN_ID(L"7306FFC4-F037-433A-BA2D-CD822917BC28");

// Rhino plug-in version
// Provide a version number string for this plug-in.
RHINO_PLUG_IN_VERSION(__DATE__ "  " __TIME__)

// Rhino plug-in description
// Provide a description of this plug-in.
RHINO_PLUG_IN_DESCRIPTION(L"joinery_solver_rhino7 plug-in for Rhinoceros®");

// Rhino plug-in icon resource id
// Provide an icon resource this plug-in.
// Icon resource should contain 16, 24, 32, 48, and 256-pixel image sizes.
RHINO_PLUG_IN_ICON_RESOURCE_ID(IDI_ICON);

// Rhino plug-in developer declarations
// TODO: fill in the following developer declarations with
// your company information. Note, all of these declarations
// must be present or your plug-in will not load.
//
// When completed, delete the following #error directive.
RHINO_PLUG_IN_DEVELOPER_ORGANIZATION(L"Petras Vestartas");
RHINO_PLUG_IN_DEVELOPER_ADDRESS(L"Undefined");
RHINO_PLUG_IN_DEVELOPER_COUNTRY(L"Undefined");
RHINO_PLUG_IN_DEVELOPER_PHONE(L"Undefined");
RHINO_PLUG_IN_DEVELOPER_FAX(L"Undefined");
RHINO_PLUG_IN_DEVELOPER_EMAIL(L"petrasvestartas@gmail.com");
RHINO_PLUG_IN_DEVELOPER_WEBSITE(L"http://www.petrasvestartas.com");
RHINO_PLUG_IN_UPDATE_URL(L"http://www.petrasvestartas.com");

// The one and only Cjoinery_solver_rhino7PlugIn object
static class Cjoinery_solver_rhino7PlugIn thePlugIn;

/////////////////////////////////////////////////////////////////////////////
// Cjoinery_solver_rhino7PlugIn definition

Cjoinery_solver_rhino7PlugIn& joinery_solver_rhino7PlugIn()
{
	// Return a reference to the one and only Cjoinery_solver_rhino7PlugIn object
	return thePlugIn;
}

Cjoinery_solver_rhino7PlugIn::Cjoinery_solver_rhino7PlugIn()
{
	// Description:
	//   Cjoinery_solver_rhino7PlugIn constructor. The constructor is called when the
	//   plug-in is loaded and "thePlugIn" is constructed. Once the plug-in
	//   is loaded, Cjoinery_solver_rhino7PlugIn::OnLoadPlugIn() is called. The
	//   constructor should be simple and solid. Do anything that might fail in
	//   Cjoinery_solver_rhino7PlugIn::OnLoadPlugIn().

	// TODO: Add construction code here
	m_plugin_version = RhinoPlugInVersion();
}

/////////////////////////////////////////////////////////////////////////////
// Required overrides

const wchar_t* Cjoinery_solver_rhino7PlugIn::PlugInName() const
{
	// Description:
	//   Plug-in name display string.  This name is displayed by Rhino when
	//   loading the plug-in, in the plug-in help menu, and in the Rhino
	//   interface for managing plug-ins.

	// TODO: Return a short, friendly name for the plug-in.
	return RhinoPlugInName();
}

const wchar_t* Cjoinery_solver_rhino7PlugIn::PlugInVersion() const
{
	// Description:
	//   Plug-in version display string. This name is displayed by Rhino
	//   when loading the plug-in and in the Rhino interface for managing
	//   plug-ins.

	// TODO: Return the version number of the plug-in.
	return m_plugin_version;
}

GUID Cjoinery_solver_rhino7PlugIn::PlugInID() const
{
	// Description:
	//   Plug-in unique identifier. The identifier is used by Rhino to
	//   manage the plug-ins.

	// TODO: Return a unique identifier for the plug-in.
	// {7306FFC4-F037-433A-BA2D-CD822917BC28}
	return ON_UuidFromString(RhinoPlugInId());
}

/////////////////////////////////////////////////////////////////////////////
// Additional overrides

BOOL Cjoinery_solver_rhino7PlugIn::OnLoadPlugIn()
{
	// Description:
	//   Called after the plug-in is loaded and the constructor has been
	//   run. This is a good place to perform any significant initialization,
	//   license checking, and so on.  This function must return TRUE for
	//   the plug-in to continue to load.

	// Remarks:
	//    Plug-ins are not loaded until after Rhino is started and a default document
	//    is created.  Because the default document already exists
	//    CRhinoEventWatcher::On????Document() functions are not called for the default
	//    document.  If you need to do any document initialization/synchronization then
	//    override this function and do it here.  It is not necessary to call
	//    CPlugIn::OnLoadPlugIn() from your derived class.

	// TODO: Add plug-in initialization code here.
	return TRUE;
}

void Cjoinery_solver_rhino7PlugIn::OnUnloadPlugIn()
{
	// Description:
	//    Called one time when plug-in is about to be unloaded. By this time,
	//    Rhino's mainframe window has been destroyed, and some of the SDK
	//    managers have been deleted. There is also no active document or active
	//    view at this time. Thus, you should only be manipulating your own objects.
	//    or tools here.

	// TODO: Add plug-in cleanup code here.
}
