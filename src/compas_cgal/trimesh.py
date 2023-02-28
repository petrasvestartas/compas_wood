import numpy as np
from compas.geometry import transform_points_numpy
from compas.files import STL
from compas.files import OFF
from compas.files import PLY
from compas.datastructures import Mesh
from compas.numerical import connectivity_matrix
from compas.numerical import adjacency_matrix
from compas.numerical import degree_matrix
from compas.numerical import normrow

from compas_cgal.meshing import remesh


class TriMesh:
    """Class representing a triangle mesh with its vertices and faces stored as Numpy arrays.

    Parameters
    ----------
    vertices : array_like
        The vertices of the mesh.
    faces : array_like
        The faces of the mesh.

    Attributes
    ----------
    vertices : NDArray[(Any, 3), np.float64]
        The vertices of the mesh.
    faces : NDArray[(Any, 3), np.int32]
        The faces of the mesh.
    edges : list[tuple[int, int]] - readonly
        Edges of the mesh as pairs of vertices.
    adjacency : dict - readonly
        Vertex adjacency dict.
    C : sp.sparse.csr_matrix - readonly
        Sparse connectivity matrix.
    A : sp.sparse.csr_matrix - readonly
        Sparse adjacency matrix.
    D : sp.sparse.csr_matrix - readonly
        Sparse degree matrix.
    centroid : NDArray[(1, 3), np.float64] - readonly
        The centroid of the mesh.
    average_edge_length : float - readonly
        The average length og the edges.

    """

    def __init__(self, vertices, faces):
        self._vertices = None
        self._faces = None
        self._adjacency = None
        self._edges = None
        self.vertices = vertices
        self.faces = faces

    def __iter__(self):
        yield self.vertices
        yield self.faces

    def __getitem__(self, key):
        if key == 0:
            return self.vertices
        if key == 1:
            return self.faces
        raise KeyError

    def __setitem__(self, key, value):
        if key == 0:
            self.vertices = value
        elif key == 1:
            self.faces = value
        else:
            raise KeyError

    @property
    def vertices(self):
        return self._vertices

    @vertices.setter
    def vertices(self, vertices):
        self._adjacency = None
        self._edges = None
        array = np.asarray(vertices, dtype=np.float64)
        rows, cols = array.shape
        if rows < 1 or cols != 3:
            raise Exception("The vertex input data is not valid.")
        self._vertices = array

    @property
    def faces(self):
        return self._faces

    @faces.setter
    def faces(self, faces):
        self._adjacency = None
        self._edges = None
        array = np.asarray(faces, dtype=np.int32)
        rows, cols = array.shape
        if rows < 1 or cols != 3:
            raise Exception("The face input data is not valid.")
        self._faces = array

    @property
    def adjacency(self):
        if not self._adjacency:
            adj = {i: [] for i in range(self.vertices.shape[0])}
            for a, b, c in self.faces:
                if b not in adj[a]:
                    adj[a].append(b)
                if c not in adj[a]:
                    adj[a].append(c)
                if a not in adj[b]:
                    adj[b].append(a)
                if c not in adj[b]:
                    adj[b].append(c)
                if a not in adj[c]:
                    adj[c].append(a)
                if b not in adj[c]:
                    adj[c].append(b)
            self._adjacency = adj
        return self._adjacency

    @property
    def edges(self):
        if not self._edges:
            seen = set()
            edges = []
            for i, nbrs in self.adjacency.items():
                for j in nbrs:
                    if (i, j) in seen:
                        continue
                    seen.add((i, j))
                    seen.add((i, j))
                    edges.append((i, j))
            self._edges = edges
        return self._edges

    @property
    def C(self):
        return connectivity_matrix(self.edges, "csr")

    @property
    def A(self):
        return adjacency_matrix(self.adjacency, "csr")

    @property
    def D(self):
        return degree_matrix(self.adjacency, "csr")

    @property
    def centroid(self):
        return np.mean(self.vertices, axis=0)

    @property
    def average_edge_length(self):
        return np.mean(normrow(self.C.dot(self.vertices)), axis=0)

    @classmethod
    def from_stl(cls, filepath, precision=None):
        """Construct a triangle mesh from the data in an STL file.

        Parameters
        ----------
        filepath : path string | file-like object | URL string
            Location of the file.
        precision : str, optional
            COMPAS precision specifier for parsing geometric data.
            Default is None, in which case the default COMPAS precision is used (:attr:`compas.PRECISION`).

        Returns
        -------
        :class:`TriMesh`

        """
        stl = STL(filepath, precision)
        return cls(stl.parser.vertices, stl.parser.faces)

    @classmethod
    def from_ply(cls, filepath, precision=None):
        """Construct a triangle mesh from the data in a PLY file.

        Parameters
        ----------
        filepath : path string | file-like object | URL string
            Location of the file.
        precision : str, optional
            COMPAS precision specifier for parsing geometric data.
            Default is None, in which case the default COMPAS precision is used (:attr:`compas.PRECISION`).

        Returns
        -------
        :class:`TriMesh`

        """
        ply = PLY(filepath, precision)
        return cls(ply.parser.vertices, ply.parser.faces)

    @classmethod
    def from_off(cls, filepath, precision=None):
        """Construct a triangle mesh from the data in an OFF file.

        Parameters
        ----------
        filepath : path string | file-like object | URL string
            Location of the file.
        precision : str, optional
            COMPAS precision specifier for parsing geometric data.
            Default is None, in which case the default COMPAS precision is used (:attr:`compas.PRECISION`).

        Returns
        -------
        :class:`TriMesh`

        """
        off = OFF(filepath, precision)
        return cls(off.reader.vertices, off.reader.faces)

    @classmethod
    def from_mesh(cls, mesh):
        """Construct a triangle mesh from a COMPAS mesh.

        Parameters
        ----------
        mesh : :class:`~compas.datastructures.Mesh`
            A COMPAS mesh data structure.

        Returns
        -------
        :class:`TriMesh`

        """
        V, F = mesh.to_vertices_and_faces()
        return cls(V, F)

    def to_mesh(self):
        """Convert the triangle mesh to a COMPAS mesh.

        Returns
        -------
        :class:`~compas.datastructures.Mesh`

        """
        return Mesh.from_vertices_and_faces(self.vertices, self.faces)

    def copy(self):
        """Create an independent copy of the triangle mesh.

        Returns
        -------
        :class:`TriMesh`

        """
        cls = type(self)
        return cls(self.vertices.copy(), self.faces.copy())

    def transform(self, T):
        """Transform the triangle mesh.

        Parameters
        ----------
        T : :class:`~compas.geometry.Transformation`
            A 4x4 transformation matrix.

        Returns
        -------
        None

        """
        self.vertices[:] = transform_points_numpy(self.vertices, T)

    def transformed(self, T):
        """Create a transformed copy of the triangle mesh.

        Parameters
        ----------
        T : :class:`~compas.geometry.Transformation`
            A 4x4 transformation matrix.

        Returns
        -------
        :class:`TriMesh`

        """
        mesh = self.copy()
        mesh.transform(T)
        return mesh

    def cull_vertices(self):
        """Remove unused vertices.

        Returns
        -------
        None

        """
        indices = np.unique(self.faces)
        indexmap = {index: i for i, index in enumerate(indices)}
        self.vertices = self.vertices[indices]
        for face in self.faces:
            face[0] = indexmap[face[0]]
            face[1] = indexmap[face[1]]
            face[2] = indexmap[face[2]]

    def remesh(self, target_length, iterations=10):
        """Remesh the triangle mesh.

        Parameters
        ----------
        target_length : float
            The target length of the edges.
        iterations : int, optional
            The number of iterations of remeshing.

        Returns
        -------
        None

        """
        V, F = remesh(self, target_length, iterations)
        self.vertices = V
        self.faces = F
