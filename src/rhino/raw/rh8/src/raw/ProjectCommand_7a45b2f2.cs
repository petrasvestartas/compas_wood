using System;

using Rhino;
using Rhino.Commands;

namespace RhinoCodePlatform.Rhino3D.Projects.Plugin
{
  [CommandStyle(Rhino.Commands.Style.ScriptRunner)]
  public class ProjectCommand_7a45b2f2 : Command
  {
    public Guid CommandId { get; } = new Guid("7a45b2f2-f2f0-4eee-a84a-2c502bf88fcf");

    public ProjectCommand_7a45b2f2() { Instance = this; }

    public static ProjectCommand_7a45b2f2 Instance { get; private set; }

    public override string EnglishName => "w_raw_find_joints_types";

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
