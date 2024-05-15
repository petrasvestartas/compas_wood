
import System
import Rhino
from ghpythonlib.componentbase import executingcomponent as component


class joint_parameters_component(component):
    def RunScript(self, _ss_e_ip, _ss_e_op, _ts_e_p, _cr_c_ip, _tt_e_p, _ss_e_r, _b):

        joint_parameters = [
            300,
            0.5,
            3,  # 1-9 ss_e_ip (side-to-side edge in-plane)
            50,
            0.64,
            15,  # 10-19 ss_e_op (side-to-side edge out-of-plane)
            450,
            0.5,
            20,  # 20-29 ts_e_p (top-to-side edge plane)
            300,
            0.5,
            30,  # 30-39 cr_c_ip (cross cutting in-plane)
            6,
            0.95,
            40,  # 40-49 tt_e_p  (top-to-top edge plane)
            300,
            0.5,
            58,  # 50-59 ss_e_r (side-to-side edge rotated)
            300,
            1.0,
            60,  # 60-69 b (boundary)
        ]

        if _ss_e_ip:
            for i in range(len(_ss_e_ip)):
                joint_parameters[0 * 3 + i] = _ss_e_ip[i]

        if _ss_e_op:
            for i in range(len(_ss_e_op)):
                joint_parameters[1 * 3 + i] = _ss_e_op[i]

        if _ts_e_p:
            for i in range(len(_ts_e_p)):
                joint_parameters[2 * 3 + i] = _ts_e_p[i]

        if _cr_c_ip:
            for i in range(len(_cr_c_ip)):
                joint_parameters[3 * 3 + i] = _cr_c_ip[i]

        if _tt_e_p:
            for i in range(len(_tt_e_p)):
                joint_parameters[4 * 3 + i] = _tt_e_p[i]

        if _ss_e_r:
            for i in range(len(_ss_e_r)):
                joint_parameters[5 * 3 + i] = _ss_e_r[i]

        if _b:
            for i in range(len(_b)):
                joint_parameters[6 * 3 + i] = _b[i]

        _joint_p = joint_parameters

        return _joint_p
