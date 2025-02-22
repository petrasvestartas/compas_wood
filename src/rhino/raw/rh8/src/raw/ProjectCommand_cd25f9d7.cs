using System;

using Rhino;
using Rhino.Commands;

namespace RhinoCodePlatform.Rhino3D.Projects.Plugin
{
  [CommandStyle(Rhino.Commands.Style.ScriptRunner)]
  public class ProjectCommand_cd25f9d7 : Command
  {
    public Guid CommandId { get; } = new Guid("cd25f9d7-93be-431e-8f3b-9c2fa92ee500");

    public ProjectCommand_cd25f9d7() { Instance = this; }

    public static ProjectCommand_cd25f9d7 Instance { get; private set; }

    public override string EnglishName => "w_raw_plane";

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
