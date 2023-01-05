#pragma once

//#include "../../../stdafx.h"

//#include <glad/glad.h> // holds all OpenGL type declarations
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#include <opengl_shaders.h>
//
////assimp
//#include <stb_image.h>
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>

#define MAX_BONE_INFLUENCE 4

namespace opengl_geometry
{
	inline unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

	class meshes
	{
		inline void TriangulateOpenPolyline(std::vector<glm::vec3> &P, std::vector<glm::vec3> &f, bool &reverse) // Tuple<Mesh, List<MeshFace>>
		{
			reverse = false;

			if (P.size() < 3)
				return;

			if (glm::distance(P.front(), P.back()) < 0.01)
				P.pop_back();

			// Simple cases
			if (P.size() == 3)
				f.emplace_back(0, 1, 2);
			else if (P.size() == 4)
			{
				f.emplace_back(0, 1, 2);
				f.emplace_back(0, 2, 3);
			}

			// https://github.com/loltothelol/polygon-trianglulation/blob/master/triangulation.cpp
			// https://github.com/codectile/ear_clipper
			// https://github.com/purvakulkarni15/Polygon-Triangulation/tree/master/Source%20Code
			// https://github.com/Muzkaw/Ear-clipping-triangulation/blob/master/Ear%20clipping%20triangulation/main.cpp

			//// Find best plane for triangulation and remap points
			//// to WorldXY for easier maths.
			// Plane m_plane = P.GetPlane();
			////Plane.FitPlaneToPoints(P, out m_plane);

			// List<Point3d> m_points = new List<Point3d>();

			// for (int i = 0; i < P.Count; ++i)
			//{
			//	Point3d p;
			//	m_plane.RemapToPlaneSpace(P[i], out p);
			//	m_points.Add(p);
			// }

			//// Counter for triangulation loop.
			// int N = m_points.Count;

			//// Constant point count.
			// int num_points = N;

			//// Orient m_points clockwise
			// if (SignedPolygonArea(m_points) < 0)
			//{
			//	m_points.Reverse();
			//	reverse = true;
			// }

			//// Instead of removing points at every iteration, we set a
			//// flag instead, and then skip the flagged vertices in
			//// subsequent steps.

			// List<MeshFace> m_faces = new List<MeshFace>(); // New mesh faces.
			// bool[] flags = new bool[N]; // Vertex flags.

			//// from http://www.csharphelper.com/examples/howto_polygon_geometry.zip
			//// Triangulation loop
			// int A = 0; int B = 0; int C = 0;

			//// Critical loop.
			// bool success = false;
			// while (N > 2)
			//{
			//	success = false;
			//	num_points = m_points.Count;

			//	for (A = 0; A < num_points; ++A)
			//	{
			//		if (flags[A])
			//			continue;

			//		B = (A + 1) % num_points;
			//		while (flags[B])
			//			B = (B + 1) % num_points;

			//		C = (B + 1) % num_points;
			//		while (flags[C])
			//			C = (C + 1) % num_points;

			//		if (FormsEar(m_points, A, B, C))
			//		{
			//			success = true;
			//			break;
			//		}
			//	}

			//	if (success)
			//	{
			//		m_faces.Add(new MeshFace(A, B, C));
			//		flags[B] = true;
			//	}
			//	N--;
			//}

			//// Transform points back to original positions.
			// Transform xform = Transform.plane_to_plane(Plane.WorldXY, m_plane);
			// for (int i = 0; i < m_points.Count; ++i)
			//	m_points[i] = m_plane.PointAt(m_points[i].X, m_points[i].Y, m_points[i].Z);

			//// Construct mesh.
			// m.Vertices.AddVertices(m_points);
			// f.AddFaces(m_faces);
		}

		// load models

	private:
		struct vertex
		{
			// position
			glm::vec3 Position;
			// normal
			glm::vec3 Normal;
			// vertex colors
			glm::vec3 VertexColors = glm::vec3(1, 0, 0);
			// texCoords
			glm::vec2 TexCoords;
			// tangent
			glm::vec3 Tangent;
			// bitangent
			glm::vec3 Bitangent;
			// bone indexes which will influence this vertex
			int m_BoneIDs[MAX_BONE_INFLUENCE];
			// weights from each bone
			float m_Weights[MAX_BONE_INFLUENCE];
		};

		struct texture
		{
			unsigned int id;
			std::string type;
			std::string path;
		};

		class mesh
		{
		public:
			// mesh Data
			// opengl_shaders::shader mesh_shader;
			glm::vec4 obj_color = opengl_globals::object_color;
			float shading_weight = 0;
			std::vector<vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<texture> textures;
			opengl_shaders::shader shader;

			std::array<glm::vec3, 4> plane = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)};
			glm::mat4 xform = glm::mat4(1.0f);
			unsigned int vao;
			unsigned int vbo, ibo;
			// constructor
			mesh(std::vector<vertex> &vertices, std::vector<unsigned int> &indices, std::vector<texture> &textures)
			{
				this->vertices = vertices;
				this->indices = indices;
				this->textures = textures;
			}

			// render the mesh
			inline void draw() // opengl_shaders::shader& shader
			{
				// std::cout << "shader type " << opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode << "shader weight " << shading_weight << "\n";
				int id = this->shading_weight == 0 ? opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode : 3;
				// std::cout << "shader type " << id << "shader weight " << shading_weight << "\n";
				shader.use();
				// float transparency = shading_weight == 0 ? 0.25 : 1;
				float transparency = 1;
				switch (id)
				{
				case (0):
				{
					shader.setMat4("projection_view_model", opengl_globals::projection_view * xform); // draw
					shader.setVec4("uniform_fragment_color", obj_color);							  // draw
				}
				break;
				case (1):
				{
					shader.setMat4("projection_view_model", opengl_globals::projection_view * xform); // draw
					shader.setVec4("uniform_fragment_color", glm::vec4(0.0f, 0.0f, 0.0f, 0.1f));	  // draw
				}
				break;

				case (2):
				case (3):
				case (4):
				{
					shader.setVec3("objectColor", obj_color);
					shader.setVec3("lightColor", opengl_globals::light_color);
					shader.setFloat("f", shading_weight); // GLOBALS

					shader.setFloat("transparency", transparency); // GLOBALS
					glm::vec3 light_pos_ = opengl_globals::light_pos;
					shader.setVec3("lightPos", light_pos_);						// GLOBALS
					shader.setVec3("viewPos", opengl_globals::camera_position); // draw
					shader.setMat4("projection", opengl_globals::projection);	// draw
					shader.setMat4("view", opengl_globals::view);				// draw
					shader.setMat4("model", opengl_globals::model * xform);
				}
				break;

				case (5):
				{
					shader.setMat4("view", opengl_globals::view); // draw
					shader.setMat4("model", opengl_globals::model * xform);
				}
				break;

				case (6):
				{
					shader.setMat4("projection", opengl_globals::projection); // draw
					shader.setMat4("view", opengl_globals::view);			  // draw
					shader.setMat4("model", opengl_globals::model * xform);
					shader.setFloat("time", opengl_globals::time);
				}
				break;
				default:
					break;
				}

				// bind appropriate textures
				unsigned int diffuseNr = 1;
				unsigned int specularNr = 1;
				unsigned int normalNr = 1;
				unsigned int heightNr = 1;
				for (unsigned int i = 0; i < textures.size(); i++)
				{
					glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
					// retrieve texture number (the N in diffuse_textureN)
					std::string number;
					std::string name = textures[i].type;
					if (name == "texture_diffuse")
						number = std::to_string(diffuseNr++);
					else if (name == "texture_specular")
						number = std::to_string(specularNr++); // transfer unsigned int to string
					else if (name == "texture_normal")
						number = std::to_string(normalNr++); // transfer unsigned int to string
					else if (name == "texture_height")
						number = std::to_string(heightNr++); // transfer unsigned int to string

					// now set the sampler to the correct texture unit
					glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
					// and finally bind the texture
					glBindTexture(GL_TEXTURE_2D, textures[i].id);
				}

				// draw mesh
				glBindVertexArray(vao);
				// print("aa");

				// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				// glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

				switch (id)
				{
				case (1):

					// preview
					// https://en.wikibooks.org/wiki/GLSL_Programming/GLUT/Transparent_Textures
					// glDisable(GL_DEPTH_TEST);
					// glEnable(GL_CULL_FACE);
					// glEnable(GL_BLEND);
					// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					// glCullFace(GL_FRONT);

					// offset the wireframe
					glEnable(GL_POLYGON_OFFSET_LINE);
					glPolygonOffset(-1, -1);

					// draw the wireframe
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					// mesh_shader.setVec3("objectColor", opengl_globals::black);
					glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

					// draw the same polygons again
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					// mesh_shader.setVec3("objectColor", obj_color);
					glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

					// restore default polygon mode
					glDisable(GL_POLYGON_OFFSET_LINE);

					// preview
					// glEnable(GL_DEPTH_TEST);
					// glDisable(GL_CULL_FACE);
					// glDisable(GL_BLEND);
					// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					// glCullFace(GL_FRONT);

					break;
				case (3):

					// if (opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode == 1) {
					//	//https://en.wikibooks.org/wiki/GLSL_Programming/GLUT/Transparent_Textures
					//	glDisable(GL_DEPTH_TEST);
					//	glEnable(GL_CULL_FACE);
					//	glEnable(GL_BLEND);
					//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					//	glCullFace(GL_FRONT);
					// }
					// else {
					//	glEnable(GL_DEPTH_TEST);
					// }

					// offset the wireframe
					glEnable(GL_POLYGON_OFFSET_LINE);
					glPolygonOffset(-1, -1);

					// draw the wireframe
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					shader.setVec3("objectColor", opengl_globals::black);
					glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

					// draw the same polygons again
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					shader.setVec3("objectColor", obj_color);
					glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

					// restore default polygon mode
					glDisable(GL_POLYGON_OFFSET_LINE);
					break;
				case (4):
					// draw the wireframe
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
					break;
				default:

					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

					break;
				}

				// print("bb");
				glBindVertexArray(0);

				// always good practice to set everything back to defaults once configured.
				glActiveTexture(GL_TEXTURE0);
			}

			// private:
			//  render data

			// initializes all the buffer objects/arrays
			inline void init_geo_in_opengl()
			{
				// std::cout << "shader type " << opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode << "shader weight " << shading_weight << "\n";
				int id = this->shading_weight == 0 ? opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode : 3;
				// std::cout << "shader type " << id << "shader weight " << shading_weight << "\n";
				switch (id)
				{
				case (0):
				case (1):
					shader = opengl_shaders::shader(
						(opengl_globals::shaders_folder + opengl_globals::filename_vertex_shader).c_str(),
						(opengl_globals::shaders_folder + opengl_globals::filename_fragment_shader).c_str(),
						//(dir + filename_line_geometry_shader).c_str()
						NULL);

					break;
				case (2):
				case (3):
				case (4):
					shader = opengl_shaders::shader(
						(opengl_globals::shaders_folder + opengl_globals::filename_basic_light_vertex_shader).c_str(),
						(opengl_globals::shaders_folder + opengl_globals::filename_basic_light_fragment_shader).c_str(),
						NULL);

					break;
				case (5):

					shader = opengl_shaders::shader(
						(opengl_globals::shaders_folder + opengl_globals::filename_normal_vertex_shader).c_str(),
						(opengl_globals::shaders_folder + opengl_globals::filename_normal_fragment_shader).c_str(),
						NULL);

					break;
				case (6):

					shader = opengl_shaders::shader(
						(opengl_globals::shaders_folder + opengl_globals::filename_explode_vertex_shader).c_str(),
						(opengl_globals::shaders_folder + opengl_globals::filename_explode_fragment_shader).c_str(),
						// NULL
						(opengl_globals::shaders_folder + opengl_globals::filename_explode_geometry_shader).c_str()

					);

					break;
				default:
					break;
				}

				// create buffers/arrays
				glGenVertexArrays(1, &vao);
				glGenBuffers(1, &vbo);
				glGenBuffers(1, &ibo);

				glBindVertexArray(vao);
				// load data into vertex buffers
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				// A great thing about structs is that their memory layout is sequential for all its items.
				// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
				// again translates to 3/2 floats which translates to a byte array.
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

				// set the vertex attribute pointers
				// vertex Positions
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)0);
				// vertex normals
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, Normal));
				// vertex colors
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, VertexColors));
				// vertex texture coords
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, TexCoords));
				// vertex tangent
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, Tangent));
				// vertex bitangent
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, Bitangent));
				// ids
				glEnableVertexAttribArray(6);
				glVertexAttribIPointer(6, 4, GL_INT, sizeof(vertex), (void *)offsetof(vertex, m_BoneIDs));

				// weights
				glEnableVertexAttribArray(7);
				glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, m_Weights));

				glBindVertexArray(0);
			}

			inline void opengl_clear()
			{ // opengl_shaders::shader& shader
				glDeleteVertexArrays(1, &vao);
				glDeleteBuffers(1, &vbo);
				glDeleteBuffers(1, &ibo);
			}
		};

	public:
		// model data

		std::vector<texture> textures_loaded; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
		std::vector<mesh> _geo;
		std::string directory;
		bool gammaCorrection;

		std::unordered_multimap<std::string, std::vector<int>> _dict; // https://www.geeksforgeeks.org/unordered_multimap-and-its-application/

		inline void clear()
		{
			textures_loaded.clear();
			_geo.clear();
			_dict.clear();
			//clear_geo_in_opengl();
		}

		meshes() {}

		// constructor, expects a filepath to a 3D model.
		meshes(std::string const &path, glm::vec4 obj_color, bool gamma = false) : gammaCorrection(gamma)
		{
			add(path, obj_color, gamma);
		}

		meshes(std::vector<std::vector<glm::vec3>> &input_vertices, std::vector<std::vector<glm::vec3>> &input_vertex_normals, std::vector<std::vector<glm::uvec3>> &input_faces, glm::vec4 obj_color, bool gamma = false, std::string group_name = "group_name") : gammaCorrection(gamma)
		{
			for (unsigned int i = 0; i < input_vertices.size(); i++)
				add(input_vertices[i], input_vertex_normals[i], input_faces[i], obj_color);
		}

		meshes(std::vector<std::vector<glm::vec3>> &input_vertices, std::vector<std::vector<glm::vec3>> &input_vertex_normals, std::vector<std::vector<glm::uvec3>> &input_faces, std::vector<glm::vec4> obj_colors, bool gamma = false, std::string group_name = "group_name") : gammaCorrection(gamma)
		{
			for (unsigned int i = 0; i < input_vertices.size(); i++)
				add(input_vertices[i], input_vertex_normals[i], input_faces[i], obj_colors[i]);
		}

		inline void add(
			const std::vector<glm::vec3> &out_vertices,
			const std::vector<glm::vec3> &out_normals,
			const std::vector<glm::ivec3> &out_triangles,
			const glm::vec4 &obj_color,
			float shading_w = 0,
			std::string group_name = "group_name")
		{
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// meshes
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			std::vector<vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<texture> textures;
			for (int i = 0; i < out_vertices.size(); i++)
			{
				vertex vertex;
				// glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
				//  positions

				float x = out_vertices[i].x;
				float y = out_vertices[i].y;
				float z = out_vertices[i].z;
				// print(x, y, z);
				opengl_globals::xyz(x, y, z);
				vertex.Position = glm::vec3(x, y, z);

				// normals

				if (out_normals.size())
				{
					if (out_vertices.size() == out_normals.size())
					{
						float nx = out_normals[i].x;
						float ny = out_normals[i].y;
						float nz = out_normals[i].z;
						opengl_globals::xyz(nx, ny, nz);
						vertex.Normal = glm::vec3(nx, ny, nz);
						// print(nx, ny, nz);
					}
				}

				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

				vertices.push_back(vertex);
			}

			// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for (int i = 0; i < out_triangles.size(); i++)
			{
				// print(cdt.triangles[i].vertices[0], cdt.triangles[i].vertices[1], cdt.triangles[i].vertices[2]);

				indices.emplace_back(out_triangles[i].x);
				indices.emplace_back(out_triangles[i].y);
				indices.emplace_back(out_triangles[i].z);
			}

			// insert indices
			_dict.insert(std::make_pair(group_name, std::vector<int>{(int)_geo.size()}));

			// return a mesh object created from the extracted mesh data
			_geo.emplace_back(vertices, indices, textures);
			// print(vertices.size(), indices.size(), textures.size());

			_geo.back().obj_color = obj_color;
			_geo.back().shading_weight = shading_w;
		}

		inline void add(
			const std::vector<float> &out_vertices,
			const std::vector<float> &out_normals,
			const std::vector<int> &out_triangles,
			const glm::vec4 &obj_color,
			float shading_w = 0,
			std::string group_name = "group_name")
		{
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// meshes
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			std::vector<vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<texture> textures;
			for (int i = 0; i < out_vertices.size(); i += 3)
			{
				vertex vertex;

				//  positions
				vertex.Position = glm::vec3(out_vertices[i + 0], out_vertices[i + 1], out_vertices[i + 2]);
				vertex.VertexColors = obj_color;

				// normals
				if (out_normals.size())
					if (out_vertices.size() == out_normals.size())
						vertex.Normal = glm::vec3(out_normals[i + 0], out_normals[i + 1], out_normals[i + 2]);

				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

				vertices.emplace_back(vertex);
			}

			// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for (int i = 0; i < out_triangles.size(); i += 3)
			{
				indices.emplace_back(out_triangles[i + 0]);
				indices.emplace_back(out_triangles[i + 1]);
				indices.emplace_back(out_triangles[i + 2]);
			}

			// insert indices
			_dict.insert(std::make_pair(group_name, std::vector<int>{(int)_geo.size()}));

			// return a mesh object created from the extracted mesh data
			_geo.emplace_back(vertices, indices, textures);
			_geo.back().obj_color = obj_color;
			_geo.back().shading_weight = shading_w;
		}

		inline void add(
			const std::vector<float> &out_vertices,
			const std::vector<float> &out_normals,
			const std::vector<int> &out_triangles,
			const std::vector<int> &out_triangles_colors,
			const glm::vec4 &obj_color,
			float shading_w = 0,
			std::string group_name = "group_name")
		{
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// meshes
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			std::vector<vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<texture> textures;
			for (int i = 0; i < out_vertices.size(); i += 3)
			{
				vertex vertex;
				// glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
				//  positions

				float x = out_vertices[i + 0];
				float y = out_vertices[i + 1];
				float z = out_vertices[i + 2];
				// print(x, y, z);
				opengl_globals::xyz(x, y, z);
				vertex.Position = glm::vec3(x, y, z);

				// normals

				if (out_normals.size())
				{
					if (out_vertices.size() == out_normals.size())
					{
						float nx = out_normals[i + 0];
						float ny = out_normals[i + 1];
						float nz = out_normals[i + 2];
						opengl_globals::xyz(nx, ny, nz);
						vertex.Normal = glm::vec3(nx, ny, nz);
						// print(nx, ny, nz);
					}
				}

				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

				vertices.push_back(vertex);
			}

			// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for (int i = 0; i < out_triangles.size(); i += 3)
			{
				// print(cdt.triangles[i].vertices[0], cdt.triangles[i].vertices[1], cdt.triangles[i].vertices[2]);
				// if (out_triangles_colors[i / 3] < 2)
				//{
				// std::cout << out_triangles_colors[i / 3] << "\n";
				indices.emplace_back(out_triangles[i + 0]);
				indices.emplace_back(out_triangles[i + 1]);
				indices.emplace_back(out_triangles[i + 2]);
				//}
			}

			// insert indices
			_dict.insert(std::make_pair(group_name, std::vector<int>{(int)_geo.size()}));

			// return a mesh object created from the extracted mesh data
			_geo.emplace_back(vertices, indices, textures);
			// print(vertices.size(), indices.size(), textures.size());

			_geo.back().obj_color = obj_color;
			_geo.back().shading_weight = shading_w;
		}

		inline void add_flat_faces(
			const std::vector<float> &out_vertices,
			const std::vector<float> &out_normals,
			const std::vector<int> &out_triangles,
			const std::vector<int> &out_triangles_colors,
			const glm::vec4 &obj_color,
			float shading_w = 0,
			std::string group_name = "group_name")
		{
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// meshes
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			std::vector<vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<texture> textures;
			vertices.reserve(out_triangles.size());

			for (int i = 0; i < out_triangles.size(); i += 3)
			{

				// Add face
				int v0 = out_triangles[i + 0];
				int v1 = out_triangles[i + 1];
				int v2 = out_triangles[i + 2];

				indices.emplace_back(i + 0);
				indices.emplace_back(i + 1);
				indices.emplace_back(i + 2);

				// Add vertex for each face
				vertex coord_0;
				vertex coord_1;
				vertex coord_2;

				coord_0.Position = glm::vec3(
					out_vertices[v0 * 3 + 0],
					out_vertices[v0 * 3 + 1],
					out_vertices[v0 * 3 + 2]);

				coord_1.Position = glm::vec3(
					out_vertices[v1 * 3 + 0],
					out_vertices[v1 * 3 + 1],
					out_vertices[v1 * 3 + 2]);

				coord_2.Position = glm::vec3(
					out_vertices[v2 * 3 + 0],
					out_vertices[v2 * 3 + 1],
					out_vertices[v2 * 3 + 2]);

				// std::cout << out_vertices.size() << " " << out_normals.size() << "\n";
				if (out_vertices.size() == out_normals.size())
				{

					glm::vec3 dir0 = coord_1.Position - coord_0.Position;
					glm::vec3 dir1 = coord_2.Position - coord_0.Position;
					glm::vec3 normal = glm::cross(dir0, dir1);

					coord_0.Normal = normal;
					coord_1.Normal = normal;
					coord_2.Normal = normal;
				}

				// std::cout << " out_triangles_colors " << out_triangles_colors[i / 3] << "\n";
				if (out_triangles_colors[i / 3] < 2)
				{
					coord_0.VertexColors = obj_color;
					coord_1.VertexColors = obj_color;
					coord_2.VertexColors = obj_color;
				}
				else
				{
					coord_0.VertexColors = glm::vec3(1, 0.8f, 0);
					coord_1.VertexColors = glm::vec3(1, 0.8f, 0);
					coord_2.VertexColors = glm::vec3(1, 0.8f, 0);
				}

				coord_0.TexCoords = glm::vec2(0.0f, 0.0f);
				coord_1.TexCoords = glm::vec2(0.0f, 0.0f);
				coord_2.TexCoords = glm::vec2(0.0f, 0.0f);
				vertices.emplace_back(coord_0);
				vertices.emplace_back(coord_1);
				vertices.emplace_back(coord_2);
			}

			// insert indices
			_dict.insert(std::make_pair(group_name, std::vector<int>{(int)_geo.size()}));
			_geo.emplace_back(vertices, indices, textures);
			_geo.back().obj_color = obj_color;
			_geo.back().shading_weight = shading_w;
		}

		// add polylines without holes
		inline void add(
			const std::vector<std::vector<glm::vec3>> &in_polylines,
			const glm::vec4 &obj_color,
			bool convex_only = false,
			double closed_tolerance = 0.001,
			float shading_w = 0,
			std::string group_name = "group_name")
		{
			std::vector<glm::vec3> out_vertices;
			std::vector<glm::vec3> out_normals;
			std::vector<glm::ivec3> out_triangles;
			// std::cout << ("Hi\n");
			//  opengl_triangulation::triangulate_flat_polylines_with_holes(in_polylines, out_vertices, out_normals, out_triangles);
			opengl_triangulation::triangulate_flat_polylines(in_polylines, out_vertices, out_normals, out_triangles, convex_only, closed_tolerance);
			add(out_vertices, out_normals, out_triangles, obj_color, shading_w, group_name);
		}

		// add polylines with holes
		inline void add(
			const std::vector<std::vector<std::vector<glm::vec3>>> &in_polylines,
			const glm::vec4 &obj_color,
			bool convex_only = false,
			double closed_tolerance = 0.001,
			float shading_w = 0,
			std::string group_name = "group_name")
		{
			std::vector<glm::vec3> out_vertices;
			std::vector<glm::vec3> out_normals;
			std::vector<glm::ivec3> out_triangles;
			// opengl_triangulation::triangulate_flat_polylines_with_holes(in_polylines, out_vertices, out_normals, out_triangles);
			opengl_triangulation::triangulate_flat_polylines_with_holes(in_polylines, out_vertices, out_normals, out_triangles, convex_only, closed_tolerance);
			add(out_vertices, out_normals, out_triangles, obj_color, shading_w, group_name);
		}

		inline void add(const std::vector<glm::vec3> &input_vertices, const std::vector<glm::vec3> &input_vertex_normals, const std::vector<glm::uvec3> &input_faces, const glm::vec4 &obj_color, std::string group_name = "group_name")
		{
			// std::vector<std::vector<glm::vec2>>& input_texture_coord,
			// data to fill
			std::vector<vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<texture> textures;
			for (unsigned int i = 0; i < input_vertices.size(); i++)
			{
				vertex vertex;
				// glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
				//  positions

				float x = input_vertices[i].x;
				float y = input_vertices[i].y;
				float z = input_vertices[i].z;
				opengl_globals::xyz(x, y, z);
				vertex.Position = glm::vec3(x, y, z);

				// normals

				if (input_vertex_normals.size())
				{
					if (input_vertices.size() == input_vertex_normals.size())
					{
						float nx = input_vertices[i].x;
						float ny = input_vertices[i].y;
						float nz = input_vertices[i].z;
						opengl_globals::xyz(nx, ny, nz);
						vertex.Normal = glm::vec3(nx, ny, nz);
					}
				}
				// texture coordinates

				// if (input_texture_coord.size() > 0) // does the mesh contain texture coordinates?
				//{
				//	//if (input_vertices[i].size() == input_texture_coord[i].size()) {
				//	//	glm::vec2 vec;
				//	//	// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
				//	//	// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				//	//	vec.x = m->mTextureCoords[0][i].x;
				//	//	vec.y = m->mTextureCoords[0][i].y;
				//	//	vertex.TexCoords = vec;
				//	//	// tangent
				//	//	vector.x = m->mTangents[i].x;
				//	//	vector.y = m->mTangents[i].y;
				//	//	vector.z = m->mTangents[i].z;
				//	//	vertex.Tangent = vector;
				//	//	// bitangent
				//	//	vector.x = m->mBitangents[i].x;
				//	//	vector.y = m->mBitangents[i].y;
				//	//	vector.z = m->mBitangents[i].z;
				//	//	vertex.Bitangent = vector;
				//	//}
				// }
				// else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

				vertices.push_back(vertex);
			}

			// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for (unsigned int i = 0; i < input_faces.size(); i++)
			{
				indices.emplace_back(input_faces[i].x);
				indices.emplace_back(input_faces[i].y);
				indices.emplace_back(input_faces[i].z);
			}

			// insert indices
			_dict.insert(std::make_pair(group_name, std::vector<int>{(int)_geo.size()}));

			// return a mesh object created from the extracted mesh data
			_geo.emplace_back(vertices, indices, textures);

			_geo.back().obj_color = obj_color;
		}

		inline void add(std::string const &path, glm::vec4 obj_color, bool gamma = false, std::string group_name = "group_name")
		{
			auto before_insertion = _geo.size();
			loadModel(path);

			// insert indices
			std::vector<int> ids(_geo.size() - before_insertion);
			std::iota(ids.begin(), ids.end(), (int)before_insertion);
			_dict.insert(std::make_pair(group_name, ids));
			// for (auto& id : ids)
			//	print(id);

			// colors
			for (int i = before_insertion; i < _geo.size(); i++)
			{
				_geo[i].obj_color = obj_color;
				_geo[i].shading_weight = 1;
				for (auto &v : _geo[i].vertices)
					v.VertexColors = glm::vec3(obj_color.x, obj_color.y, obj_color.z);
			}
		}

		inline void init_geo_in_opengl()
		{
			if (_geo.size() == 0)
				return;

			for (unsigned int i = 0; i < _geo.size(); i++)
				_geo[i].init_geo_in_opengl();
		}

		// draws the model, and thus all its meshes
		inline void draw()
		{
			if (_geo.size() == 0)
				return;
			for (unsigned int i = 0; i < _geo.size(); i++)
				_geo[i].draw();
		}

		inline void clear_geo_in_opengl()
		{
			for (int i = 0; i < _geo.size(); i++)
				_geo[i].opengl_clear();

			for (unsigned int i = 0; i < _geo.size(); i++)
				_geo[i].shader.delete_shader();
		}

		inline void save_model(std::string const &path)
		{
		}

	private:
		// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
		inline void loadModel(std::string const &path)
		{
			// read file via ASSIMP
			Assimp::Importer importer;
			const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
			// check for errors
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
			{
				std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
				return;
			}
			// retrieve the directory path of the filepath
			std::string cur_dir(path);
			auto pos = cur_dir.find_last_of("/\\");
			directory = cur_dir.substr(0, pos);

			// process ASSIMP's root node recursively
			processNode(scene->mRootNode, scene);
		}

		// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		inline void processNode(aiNode *node, const aiScene *scene)
		{
			// process each mesh located at the current node
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				// the node object only contains indices to index the actual objects in the scene.
				// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
				aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
				_geo.emplace_back(processMesh(mesh, scene));
			}
			// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}
		}

		inline mesh processMesh(aiMesh *m, const aiScene *scene)
		{
			// data to fill
			std::vector<vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<texture> textures;

			// walk through each of the mesh's vertices
			for (unsigned int i = 0; i < m->mNumVertices; i++)
			{
				vertex vertex;
				// glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
				//  positions

				float x = m->mVertices[i].x;
				float y = m->mVertices[i].y;
				float z = m->mVertices[i].z;
				opengl_globals::xyz(x, y, z);
				vertex.Position = glm::vec3(x, y, z);

				// normals
				if (m->HasNormals())
				{
					float nx = m->mNormals[i].x;
					float ny = m->mNormals[i].y;
					float nz = m->mNormals[i].z;
					opengl_globals::xyz(nx, ny, nz);
					vertex.Normal = glm::vec3(nx, ny, nz);
				}
				// texture coordinates
				if (m->mTextureCoords[0]) // does the mesh contain texture coordinates?
				{
					glm::vec2 vec;
					// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
					// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
					vec.x = m->mTextureCoords[0][i].x;
					vec.y = m->mTextureCoords[0][i].y;
					vertex.TexCoords = vec;
					// tangent
					float tx = m->mTangents[i].x;
					float ty = m->mTangents[i].y;
					float tz = m->mTangents[i].z;
					opengl_globals::xyz(tx, ty, tz);
					vertex.Tangent = glm::vec3(tx, ty, tz);
					// bitangent
					float bx = m->mBitangents[i].x;
					float by = m->mBitangents[i].y;
					float bz = m->mBitangents[i].z;
					opengl_globals::xyz(bx, by, bz);
					vertex.Bitangent = glm::vec3(bx, by, bz);
				}
				else
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);

				vertices.push_back(vertex);
			}
			// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for (unsigned int i = 0; i < m->mNumFaces; i++)
			{
				aiFace face = m->mFaces[i];
				// retrieve all indices of the face and store them in the indices vector
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			// process materials
			aiMaterial *material = scene->mMaterials[m->mMaterialIndex];
			// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
			// Same applies to other texture as the following list summarizes:
			// diffuse: texture_diffuseN
			// specular: texture_specularN
			// normal: texture_normalN

			// 1. diffuse maps
			std::vector<texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			std::vector<texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			std::vector<texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 4. height maps
			std::vector<texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			// return a mesh object created from the extracted mesh data
			return mesh(vertices, indices, textures);
		}

		// checks all material textures of a given type and loads the textures if they're not loaded yet.
		// the required info is returned as a Texture struct.
		inline std::vector<texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
		{
			std::vector<texture> textures;
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
				bool skip = false;
				for (unsigned int j = 0; j < textures_loaded.size(); j++)
				{
					if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
					{
						textures.push_back(textures_loaded[j]);
						skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
						break;
					}
				}
				if (!skip)
				{ // if texture hasn't been loaded already, load it
					texture texture;
					texture.id = TextureFromFile(str.C_Str(), this->directory);
					texture.type = typeName;
					texture.path = str.C_Str();
					textures.push_back(texture);
					textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
				}
			}
			return textures;
		}
	};

	inline unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
	{
		// std::cout << path << std::endl;
		// std::cout << directory << std::endl;
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << filename << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}
}