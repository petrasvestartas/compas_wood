from ghpythonlib.componentbase import executingcomponent as component
import System
import Rhino


class MyComponent(component):
    def RunScript(
        self,
        _cuts0: System.Collections.Generic.List[Rhino.Geometry.Curve],
        _cuts1: System.Collections.Generic.List[Rhino.Geometry.Curve],
        _type: int,
    ):

        if not _cuts0:
            return

        if not _cuts1:
            return

        if not _type:
            return

        _custom_joints = []
        _custom_types = []
        type = 39 if _type is None else _type

        for i in range(len(_cuts0)):
            _custom_joints.append(_cuts0[i])
            _custom_types.append(type)

        for i in range(len(_cuts1)):
            _custom_joints.append(_cuts1[i])
            _custom_types.append(-type)

        # return outputs if you have them; here I try it for you:
        return (_custom_joints, _custom_types)
