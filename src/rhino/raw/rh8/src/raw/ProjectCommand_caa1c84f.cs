using System;

using Rhino;
using Rhino.Commands;

namespace RhinoCodePlatform.Rhino3D.Projects.Plugin
{
  [CommandStyle(Rhino.Commands.Style.ScriptRunner)]
  public class ProjectCommand_caa1c84f : Command
  {
    public Guid CommandId { get; } = new Guid("caa1c84f-1408-4d61-8ae0-b49abb510a82");

    public ProjectCommand_caa1c84f() { Instance = this; }

    public static ProjectCommand_caa1c84f Instance { get; private set; }

    public override string EnglishName => "w_raw_from_mesh";

    protected override string CommandContextHelpUrl => "";

    protected override Rhino.Commands.Result RunCommand(RhinoDoc doc, RunMode mode)
    {
      // NOTE:
      // Initialize() attempts to loads the core rhinocode plugin
      // and prepare the scripting platform. This call can not be in any static
      // ctors of Command or Plugin classes since plugins can not be loaded while
      // rhino is loading this plugin. The call has an initialized check and is
      // very fast after the first run.
      ProjectPlugin.Initialize();

      return ProjectPlugin.RunCode(this, CommandId, doc, mode);
    }
  }
}
