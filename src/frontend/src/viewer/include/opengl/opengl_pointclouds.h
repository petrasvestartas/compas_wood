#pragma once
// #include "../../../stdafx.h"

// #include <glad/glad.h> // holds all OpenGL type declarations
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <opengl_shaders.h>

namespace opengl_geometry
{
	class pointclouds
	{
	private:
		struct vertex
		{
			// position
			glm::vec3 position;
			// color
			glm::vec3 color;
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

		class pointcloud
		{
		public:
			bool draw_dynamic = false;
			std::vector<vertex> vertices;
			// std::vector<unsigned int> indices;//pairs
			unsigned int vao, vbo, ibo;

			std::array<glm::vec3, 4> plane = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)};
			glm::mat4 xform = glm::mat4(1.0f);

			// individual pointclouds
			pointcloud(const std::vector<float> &coord, const std::vector<float> &colors) //, std::vector<Texture> textures
			{
				vertices.reserve(coord.size());

				for (int i = 0; i < coord.size(); i += 3)
				{
					vertex vertex;
					float x = coord[i + 0];
					float y = coord[i + 1];
					float z = coord[i + 2];
					opengl_globals::xyz(x, y, z);
					vertex.position = glm::vec3(x, y, z);

					vertex.color = glm::vec3(colors[i + 0], colors[i + 1], colors[i + 2]);
					vertices.emplace_back(vertex);
				}
			}

			// merged pointclouds
			pointcloud(const std::vector<std::vector<float>> &coord, const std::vector<std::vector<float>> &colors) //, std::vector<Texture> textures
			{
				// reserve memory
				size_t v_count = 0;
				for (int i = 0; i < coord.size(); i++)
				{
					v_count += coord[i].size();
				}

				vertices.reserve(v_count);

				// fill vertics and indices lists
				for (int i = 0; i < coord.size(); i++)
				{
					for (int j = 0; j < coord[i].size(); j += 3)
					{
						vertex vertex;
						float x = coord[i][j + 0];
						float y = coord[i][j + 1];
						float z = coord[i][j + 2];
						opengl_globals::xyz(x, y, z);
						vertex.position = glm::vec3(x, y, z);
						vertex.color = glm::vec3(colors[i][j + 0], colors[i][j + 1], colors[i][j + 2]);
						vertices.emplace_back(vertex);
					}
				}
			}

			//// initializes all the buffer objects/arrays

			inline void opengl_init()
			{
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// init vertex-array and vertex-array-buffer
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// vertex array
				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);

				// bind vertex-array-buffer to the vertex-array
				glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// declare array with data or empty array depending how data will be displayed
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				auto type = !draw_dynamic ? GL_STATIC_DRAW : GL_STREAM_DRAW;
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], type); // target | size | data (poinnting to first element e.g. glm::value_ptr(vertices[0])) | usage

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// set attributes that corresponds to layout id in the vertex shader
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				// vertex Positions
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)0);
				// vertex normals
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, color));

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// bind buffers vao | vbo | ibo
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// glBindVertexArray(0);
			}

			// can be done better if one pointcloud is used without copying the data twice
			inline void update_coordinates(const std::vector<float> &coord, const std::vector<float> &colors) //, std::vector<Texture> textures
			{
				this->draw_dynamic = true;
				for (int i = 0; i < coord.size(); i += 3)
				{
					float x = coord[i + 0];
					float y = coord[i + 1];
					float z = coord[i + 2];
					opengl_globals::xyz(x, y, z);
					vertices[i / 3].position.x = x;
					vertices[i / 3].position.y = y;
					vertices[i / 3].position.z = z;

					vertices[i / 3].color.r = colors[i + 0];
					vertices[i / 3].color.g = colors[i + 1];
					vertices[i / 3].color.b = colors[i + 2];
				}
			}

			inline void update_coordinate(int &id, float &x, float &y, float &z, float &r, float &g, float &b) //, std::vector<Texture> textures
			{
				this->draw_dynamic = true;

				float x_ = x;
				float y_ = y;
				float z_ = z;
				opengl_globals::xyz(x_, y_, z_);
				vertices[id].position.x = x_;
				vertices[id].position.y = y_;
				vertices[id].position.z = z_;

				vertices[id].color.r = r;
				vertices[id].color.g = g;
				vertices[id].color.b = b;
			}

			// render the mesh
			inline void opengl_draw(opengl_shaders::shader &shader)
			{
				// update
				// https://learnopengl.com/Advanced-OpenGL/Advanced-Data
				if (draw_dynamic)
				{
					// for (auto& v : vertices)
					//	v.position.y += 0.001;
					glBindBuffer(GL_ARRAY_BUFFER, vbo); // the GL_ARRAY_BUFFER binding is a global state so you have to update the one that is currently in use.  https://stackoverflow.com/questions/73415705/glbuffersubdata-calling-multiple-times-overrides-the-first-data
					glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vertex), &vertices[0]);
				}

				// draw
				glBindVertexArray(vao);
				glDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size());
				// glBindVertexArray(0);
			}

			inline void opengl_clear()
			{
				glDeleteVertexArrays(1, &vao);
				glDeleteBuffers(1, &vbo);
			}
		};

	public:
		// bool draw_dynamic = true;
		opengl_shaders::shader _shader;

		std::vector<pointcloud> _geo;
		std::vector<float> _line_thickness;
		std::vector<glm::vec4> _colors;
		std::unordered_multimap<std::string, std::vector<int>> _dict;

		inline void clear()
		{
			_geo.clear();
			_colors.clear();
			_line_thickness.clear();
			_dict.clear();
			// clear_geo_in_opengl();
		}

		pointclouds() {}

		// constructor individual polylines
		pointclouds(std::vector<std::vector<float>> &vertices, std::vector<std::vector<float>> &vertices_colors, std::vector<float> &line_thickness, std::vector<glm::vec4> &colors, std::string group_name = "group_name")
		{
			add(vertices, vertices_colors, line_thickness, colors, group_name);
		}

		// constructor for merging polylines
		pointclouds(std::vector<std::vector<float>> &vertices, std::vector<std::vector<float>> &vertices_colors, float &line_thickness, glm::vec4 &color, std::string group_name = "group_name")
		{
			add(vertices, vertices_colors, line_thickness, color, group_name);
		}

		inline void add(std::vector<std::vector<float>> &vertices, std::vector<std::vector<float>> &vertices_colors, std::vector<float> &line_thickness, std::vector<glm::vec4> &colors, std::string group_name = "group_name")
		{
			if (vertices.size() == line_thickness.size() && vertices.size() == colors.size())
			{
				_line_thickness = line_thickness;
				_colors = colors;

				_geo.reserve(vertices.size());
				auto before_insertion = _geo.size();

				for (int i = 0; i < vertices.size(); i++)
					_geo.emplace_back(pointcloud(vertices[i], vertices_colors[i]));

				// insert indices
				std::vector<int> ids(_geo.size() - before_insertion);
				std::iota(ids.begin(), ids.end(), (int)before_insertion);
				_dict.insert(std::make_pair(group_name, ids));
			}
		}

		inline void add(std::vector<std::vector<float>> &vertices, std::vector<std::vector<float>> &vertices_colors, float &line_thickness, glm::vec4 &color, std::string group_name = "group_name")
		{
			_dict.insert(std::make_pair(group_name, std::vector<int>{(int)_geo.size()}));
			_colors = {color};
			_line_thickness.emplace_back(line_thickness);
			_geo.emplace_back(pointcloud(vertices, vertices_colors));
		}

		inline void add(std::vector<std::vector<float>> &vertices, std::vector<std::vector<float>> &vertices_colors, float line_thickness = 3.0f, std::string group_name = "group_name")
		{
			_dict.insert(std::make_pair(group_name, std::vector<int>{(int)_geo.size()}));
			_colors = {glm::vec4(0, 0, 0, 0)};
			_line_thickness.emplace_back(line_thickness);
			_geo.emplace_back(pointcloud(vertices, vertices_colors));
		}

		inline void add(std::vector<float> &vertices, std::vector<float> &vertices_colors, float line_thickness = 3.0f, std::string group_name = "group_name")
		{
			_dict.insert(std::make_pair(group_name, std::vector<int>{(int)_geo.size()}));
			_colors = {glm::vec4(0, 0, 0, 0)};
			_line_thickness.emplace_back(line_thickness);
			_geo.emplace_back(pointcloud(vertices, vertices_colors));
		}

		inline void init_geo_in_opengl()
		{
			if (_geo.size() == 0)
				return;

			// std::cout << (opengl_globals::shaders_folder + opengl_globals::filename_pointcloud_vertex_shader).c_str() << std::endl;
			// std::cout << (opengl_globals::shaders_folder + opengl_globals::filename_pointcloud_fragment_shader).c_str() << std::endl;
			_shader = opengl_shaders::shader(
				(opengl_globals::shaders_folder + opengl_globals::filename_pointcloud_vertex_shader).c_str(),
				(opengl_globals::shaders_folder + opengl_globals::filename_pointcloud_fragment_shader).c_str(),
				//(dir + filename_line_geometry_shader).c_str()
				NULL);

			for (int i = 0; i < _geo.size(); i++)
				_geo[i].opengl_init();
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

			float t = (float)glfwGetTime() * 0.2f;
			float mod = 1.0f;

			float time = std::fmod(t, mod);
			if (time > 0.5)
				time = 1.0f - time;

			//_shader.setFloat("time", time * 2);

			// draw geometry one by one or all at once incase it was merged in constructor
			for (unsigned int i = 0; i < _geo.size(); i++)
			{
				_shader.setMat4("projection_view_model", opengl_globals::projection_view * _geo[i].xform);

				// assign shader properties unique for this geometry
				// if (i == 0)
				// glPointSize(_line_thickness[i]);

				// std::cout << i << " " << _line_thickness[i] << std::endl;
				//_shader_line.setVec4("uniform_fragment_color", _colors[i]);

				// create buffers,
				_geo[i].opengl_draw(_shader);
			}

			// assign back origina line thickness value
			// glPointSize(opengl_globals::default_line_thickness);
		}
	};
}