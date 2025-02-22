using System;
using System.Reflection;

namespace RhinoCodePlatform.Rhino3D.Projects.Plugin
{
  static class ProjectInterop
  {
    static readonly Guid s_rhinocode = new Guid("c9cba87a-23ce-4f15-a918-97645c05cde7");
    static readonly PropertyInfo s_platformsProp = default;
    static readonly Type s_platformSpecType = default;
    static readonly object s_rhinoSpec = default;
    static readonly Type s_invokeContextType = default;

    static ProjectInterop()
    {
      // ensure plugin is loaded
      Rhino.PlugIns.PlugIn.LoadPlugIn(s_rhinocode);

      s_platformsProp = Type.GetType("Rhino.Runtime.Code.RhinoCode, Rhino.Runtime.Code").GetProperty("Platforms");
      s_platformSpecType = Type.GetType("Rhino.Runtime.Code.Platform.PlatformSpec, Rhino.Runtime.Code");
      s_rhinoSpec = Activator.CreateInstance(s_platformSpecType, new object[] { "mcneel.rhino3d.rhino" });
      s_invokeContextType = Type.GetType("Rhino.Runtime.Code.Platform.InvokeContext, Rhino.Runtime.Code");
    }

    public static object GetProjectServer()
    {
      object platformRegistry = s_platformsProp.GetValue(default);
      dynamic rhinoPlatform = platformRegistry.GetType().InvokeMember("QueryLatest", BindingFlags.InvokeMethod, default, platformRegistry, new object[] { s_rhinoSpec });
      return rhinoPlatform?.ProjectServer;
    }

    public static object CreateInvokeContext() => Activator.CreateInstance(s_invokeContextType, new object[] { });
  }
}
