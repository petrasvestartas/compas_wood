#pragma once

//#include "../../stdafx.h"
//#include <glad/glad.h> // holds all OpenGL type declarations
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#include <opengl_shaders.h>

// todo
// implement polyline from xml loader

namespace opengl_geometry
{
	class polylines
	{
	private:
		struct vertex
		{
			// position
			glm::vec3 Position;
			//// normal
			// glm::vec3 Normal;
			//// texCoords
			// glm::vec2 TexCoords;
			//// tangent
			// glm::vec3 Tangent;
			//// bitangent
			// glm::vec3 Bitangent;
			////bone indexes which will influence this vertex
			// int m_BoneIDs[MAX_BONE_INFLUENCE];
			////weights from each bone
			// float m_Weights[MAX_BONE_INFLUENCE];
		};

		// struct Texture {
		//	unsigned int id;
		//	std::string type;
		//	std::string path;
		// };

		class polyline
		{
		public:
			std::vector<vertex> vertices;
			std::vector<unsigned int> indices; // pairs
			unsigned int vao{};
			unsigned int vbo{};
			unsigned int ibo{};

			std::array<glm::vec3, 4> plane = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)};
			glm::mat4 xform = glm::mat4(1.0f);

			polyline(const std::vector<glm::vec3> &coord) //, std::vector<Texture> textures
			{
				vertices.reserve(coord.size());
				indices.reserve((coord.size() - 1) * 2);

				for (int i = 0; i < coord.size(); i++)
				{
					vertex vertex;
					float x = coord[i].x;
					float y = coord[i].y;
					float z = coord[i].z;
					opengl_globals::xyz(x, y, z);
					vertex.Position = glm::vec3(x, y, z);
					vertices.emplace_back(vertex);
				}
				for (unsigned int i = 0; i < (coord.size()) - 1; i++)
				{
					indices.emplace_back(i + (unsigned int)0);
					indices.emplace_back(i + (unsigned int)1);
				}
			}

			// merged polylines
			polyline(const std::vector<std::vector<glm::vec3>> &coord) //, std::vector<Texture> textures
			{
				// reserve memory
				size_t v_count = 0;
				size_t i_count = 0;
				for (int i = 0; i < coord.size(); i++)
				{
					v_count += coord[i].size() * 3;
					i_count += coord[i].size() * 3 - 1;
				}

				vertices.reserve(v_count);
				indices.reserve(i_count);

				// fill vertics and indices lists

				for (int i = 0; i < coord.size(); i++)
				{
					for (int j = 0; j < coord[i].size(); j++)
					{
						vertex vertex;
						float x = coord[i][j].x;
						float y = coord[i][j].y;
						float z = coord[i][j].z;
						opengl_globals::xyz(x, y, z);
						vertex.Position = glm::vec3(x, y, z);
						vertices.emplace_back(vertex);
					}
				}

				v_count = 0;
				for (int i = 0; i < coord.size(); i++)
				{
					for (int j = 0; j < (coord[i].size()) - 1; j++)
					{
						// std::cout << v_count + j + 0 << " " << v_count + j + 1 << std::endl;
						indices.emplace_back((unsigned int)v_count + (unsigned int)j + (unsigned int)0);
						indices.emplace_back((unsigned int)v_count + (unsigned int)j + (unsigned int)1);
					}
					v_count += (coord[i].size());
				}

				// std::cout << vertices.size() << std::endl;

				// now that we have all the required data, set the vertex buffers and its attribute pointers.
				// setup_polyline();
			}

			// individual polylines
			polyline(const std::vector<float> &coord) //, std::vector<Texture> textures
			{
				vertices.reserve(coord.size());
				indices.reserve((coord.size() - 1) * 2);

				for (int i = 0; i < coord.size(); i += 3)
				{
					vertex vertex;
					float x = coord[i + (unsigned int)0];
					float y = coord[i + (unsigned int)1];
					float z = coord[i + (unsigned int)2];
					opengl_globals::xyz(x, y, z);
					vertex.Position = glm::vec3(x, y, z);
					vertices.emplace_back(vertex);
				}
				for (unsigned int i = 0; i < (coord.size() / 3) - 1; i++)
				{
					indices.emplace_back(i + (unsigned int)0);
					indices.emplace_back(i + (unsigned int)1);
				}
			}

			// merged polylines
			polyline(const std::vector<std::vector<float>> &coord) //, std::vector<Texture> textures
			{
				// reserve memory
				size_t v_count = 0;
				size_t i_count = 0;
				for (int i = 0; i < coord.size(); i++)
				{
					v_count += coord[i].size();
					i_count += coord[i].size() - 1;
				}

				vertices.reserve(v_count);
				indices.reserve(i_count);

				// fill vertics and indices lists

				for (int i = 0; i < coord.size(); i++)
				{
					for (int j = 0; j < coord[i].size(); j += 3)
					{
						vertex vertex;
						float x = coord[i][j + (unsigned int)0];
						float y = coord[i][j + (unsigned int)1];
						float z = coord[i][j + (unsigned int)2];
						opengl_globals::xyz(x, y, z);
						vertex.Position = glm::vec3(x, y, z);
						vertices.emplace_back(vertex);
					}
				}

				v_count = 0;
				for (int i = 0; i < coord.size(); i++)
				{
					for (int j = 0; j < (coord[i].size() / 3) - 1; j++)
					{
						// std::cout << v_count + j + 0 << " " << v_count + j + 1 << std::endl;
						indices.emplace_back((unsigned int)v_count + (unsigned int)j + (unsigned int)0);
						indices.emplace_back((unsigned int)v_count + (unsigned int)j + (unsigned int)1);
					}
					v_count += (coord[i].size() / 3);
				}

				// std::cout << vertices.size() << std::endl;

				// now that we have all the required data, set the vertex buffers and its attribute pointers.
				// setup_polyline();
			}

			// render the mesh

			//// initializes all the buffer objects/arrays
			inline void setup_polyline()
			{
				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);

				// vertex buffer
				glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW); // target | size | data (poinnting to first element e.g. glm::value_ptr(vertices[0])) | usage
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

				// index buffer
				glGenBuffers(1, &ibo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW); // target | size | data | usage

				// bind buffers vao | vbo | ibo
				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			inline void draw(opengl_shaders::shader &shader)
			{
				// if (opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode == 1) {
				//	glEnable(GL_DEPTH_TEST);
				//	glEnable(GL_CULL_FACE);

				//	glEnable(GL_BLEND);
				//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				//	glCullFace(GL_FRONT);
				//}

				glBindVertexArray(vao);
				// glPointSize(50);
				// glDrawArrays(GL_LINES, 0, my_len);
				// glDrawElements(GL_POINTS, (GLuint)indices.size(), GL_UNSIGNED_INT, NULL); //mode | count (number of indices in the index array) | type | indices
				// glDrawArrays(GL_LINES, 0, (GLuint)indices.size());
				glDrawElements(GL_LINES, (GLuint)indices.size(), GL_UNSIGNED_INT, NULL); // mode | count (number of indices in the index array) | type | indices
				glBindVertexArray(0);

				// if (opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode == 1) {
				//	glDisable(GL_DEPTH_TEST);
				//	glEnable(GL_CULL_FACE);
				// }
			}

			inline void opengl_clear()
			{
				glDeleteVertexArrays(1, &vao);
				glDeleteBuffers(1, &vbo);
				glDeleteBuffers(1, &ibo);
			}
		};

	public:
		opengl_shaders::shader _shader;

		std::vector<polyline> _geo;
		std::vector<float> _line_thickness;
		std::vector<glm::vec4> _colors;
		std::unordered_multimap<std::string, std::vector<int>> _dict;

		polylines() {}

		// constructor individual polylines
		polylines(std::vector<std::vector<float>> &vertices, std::vector<float> &line_thickness, std::vector<glm::vec4> &colors, std::string group_name = "group_name")
		{
			add(vertices, line_thickness, colors, group_name);

			// if (vertices.size() == line_thickness.size() && vertices.size() == colors.size()) {
			//	_line_thickness = line_thickness;
			//	_colors = colors;

			//	_geo.reserve(vertices.size());
			//	for (int i = 0; i < vertices.size(); i++)
			//		_geo.emplace_back(polyline(vertices[i]));
			//}
		}

		// constructor for merging polylines

		polylines(std::vector<std::vector<float>> &vertices, float &line_thickness, glm::vec4 &color, std::string group_name = "group_name")
		{
			add(vertices, line_thickness, color, group_name);
		}

		inline void add(std::vector<std::vector<glm::vec3>> &polylines, const float &line_thickness, const glm::vec4 &color, bool close = true, std::string group_name = "group_name")
		{
			std::vector<std::vector<float>> polylines_coord;
			polylines_coord.reserve(polylines.size());
			for (int i = 0; i < polylines.size(); i++)
			{
				std::vector<float> coord;
				coord.reserve(polylines[i].size() * 3);
				for (int j = 0; j < polylines[i].size(); j++)
				{
					coord.emplace_back(polylines[i][j].x);
					coord.emplace_back(polylines[i][j].y);
					coord.emplace_back(polylines[i][j].z);
				}
				if (glm::distance(polylines[i].front(), polylines[i].back()) > 0.001 && close)
				{
					coord.emplace_back(polylines[i].front().x);
					coord.emplace_back(polylines[i].front().y);
					coord.emplace_back(polylines[i].front().z);
				}
				polylines_coord.emplace_back(coord);
			}
			add(polylines_coord, line_thickness, color, group_name);
		}

		inline void add(std::vector<std::vector<std::vector<glm::vec3>>> &polylines, const float &line_thickness, const glm::vec4 &color, bool close = true, std::string group_name = "group_name")
		{
			int pline_count = 0;
			for (auto &plines_group : polylines)
				for (auto &plines : plines_group)
					pline_count += plines.size();
			std::vector<std::vector<float>> polylines_coord;
			polylines_coord.reserve(pline_count);

			for (int i = 0; i < polylines.size(); i++)
			{
				for (int j = 0; j < polylines[i].size(); j++)
				{
					std::vector<float> coord;
					coord.reserve(polylines[i][j].size() * 3);
					for (int k = 0; k < polylines[i][j].size(); k++)
					{
						coord.emplace_back(polylines[i][j][k].x);
						coord.emplace_back(polylines[i][j][k].y);
						coord.emplace_back(polylines[i][j][k].z);
					}
					if (glm::distance(polylines[i][j].front(), polylines[i][j].back()) > 0.001 && close)
					{
						coord.emplace_back(polylines[i][j].front().x);
						coord.emplace_back(polylines[i][j].front().y);
						coord.emplace_back(polylines[i][j].front().z);
					}
					polylines_coord.emplace_back(coord);
				}
			}
			add(polylines_coord, line_thickness, color, group_name);
		}

		inline void add(std::vector<std::vector<float>> &vertices, std::vector<float> &line_thickness, std::vector<glm::vec4> &colors, std::string group_name = "group_name")
		{
			auto before_insertion = _geo.size();

			for (int i = 0; i < vertices.size(); i++)
				add(vertices[i], line_thickness[i], colors[i], group_name);

			// insert indices
			std::vector<int> ids(_geo.size() - before_insertion);
			std::iota(ids.begin(), ids.end(), (int)before_insertion);
			_dict.insert(std::make_pair(group_name, ids));
		}

		inline void add(std::vector<std::vector<float>> &vertices, const float &line_thickness, const glm::vec4 &color, std::string group_name = "group_name")
		{
			// insert indices
			_dict.insert(std::make_pair(group_name, std::vector<int>{(int)_geo.size()}));
			_colors.emplace_back(color);
			_line_thickness.emplace_back(line_thickness);
			_geo.emplace_back(polyline(vertices));
		}

		inline void add(std::vector<float> &vertices, const float &line_thickness, const glm::vec4 &color, std::string group_name = "group_name")
		{
			// insert indices
			_dict.insert(std::make_pair(group_name, std::vector<int>{(int)_geo.size()}));
			_colors.emplace_back(color);
			_line_thickness.emplace_back(line_thickness);
			_geo.emplace_back(polyline(vertices));
		}

		inline void init_geo_in_opengl()
		{
			if (_geo.size() == 0)
				return;
			_shader = opengl_shaders::shader(
				(opengl_globals::shaders_folder + opengl_globals::filename_vertex_shader).c_str(),
				(opengl_globals::shaders_folder + opengl_globals::filename_fragment_shader).c_str(),
				(opengl_globals::shaders_folder + opengl_globals::filename_geometry_shader).c_str()
				// NULL
			);

			for (int i = 0; i < _geo.size(); i++)
				_geo[i].setup_polyline();
		}

		inline void clear_geo_in_opengl()
		{
			for (int i = 0; i < _geo.size(); i++)
				_geo[i].opengl_clear();
			_shader.delete_shader();
		}

		// draw geomery in the while loop
		inline void draw()
		{
			if (_geo.size() == 0)
				return;

			// enable shader
			_shader.use();

			// assign shader properties unique for all geometry
			//_shader.setMat4("model", opengl_globals::model);
			//_shader.setMat4("view", opengl_globals::view);
			//_shader.setMat4("projection", opengl_globals::projection);
			//_shader.setMat4("projection_view_model", opengl_globals::projection_view);//opengl_globals::projection_view
			_shader.setVec2("u_viewportSize", glm::vec2(opengl_globals::SCR_WIDTH, opengl_globals::SCR_HEIGHT));
			if (_line_thickness.size() > 0)
				_shader.setBool("u_scale_width_by_zoom", _line_thickness[0] < 0);

			// uniform float u_thickness;
			// uniform   vec2    u_viewportSize;
			// draw geometry one by one or all at once incase it was merged in constructor
			for (unsigned int i = 0; i < _geo.size(); i++)
			{
				// assign shader properties unique for this geometry
				// glLineWidth(_line_thickness[i]);
				_shader.setVec4("uniform_fragment_color", _colors[i]);
				_shader.setFloat("u_thickness", _line_thickness[i]);
				_shader.setMat4("projection_view_model", opengl_globals::projection_view * _geo[i].xform);
				// create buffers,
				_geo[i].draw(_shader);
			}

			// assign back origina line thickness value
			// glLineWidth(opengl_globals::default_line_thickness);
		}

		inline void clear()
		{
			_geo.clear();
			_colors.clear();
			_line_thickness.clear();
			_dict.clear();
			//clear_geo_in_opengl();
		}
	};
}