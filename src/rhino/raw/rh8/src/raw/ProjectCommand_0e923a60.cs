using System;

using Rhino;
using Rhino.Commands;

namespace RhinoCodePlatform.Rhino3D.Projects.Plugin
{
  [CommandStyle(Rhino.Commands.Style.ScriptRunner)]
  public class ProjectCommand_0e923a60 : Command
  {
    public Guid CommandId { get; } = new Guid("0e923a60-ee1e-4a70-9525-0680491118c7");

    public ProjectCommand_0e923a60() { Instance = this; }

    public static ProjectCommand_0e923a60 Instance { get; private set; }

    public override string EnglishName => "w_raw_install";

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
