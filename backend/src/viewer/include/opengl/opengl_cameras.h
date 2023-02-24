#pragma once
// https://learnopengl.com/Getting-started/camera
//#include "../../../stdafx.h"
//#include <glad/glad.h>
//#include <glm/glm.hpp>
//#include <glm/ext.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/transform.hpp>
//#include <glm/gtx/euler_angles.hpp>

//#include <cmath>
//#include <iostream>

namespace opengl_cameras
{
	enum camera_type
	{
		CAMERA_WALK_ORTHO,
		CAMERA_WALK_PERSPECTIVE,
		CAMERA_ARCBALL_ORTHO,
		CAMERA_ARCBALL_ORTHO_TOP,
		CAMERA_ARCBALL_ORTHO_BOTTOM,
		CAMERA_ARCBALL_ORTHO_LEFT,
		CAMERA_ARCBALL_ORTHO_RIGHT,
		CAMERA_ARCBALL_ORTHO_FRONT,
		CAMERA_ARCBALL_ORTHO_BACK,
		CAMERA_ARCBALL_PERSPECTIVE,
	};

	// camera controls
	static opengl_cameras::camera_type cam_type = opengl_cameras::CAMERA_ARCBALL_ORTHO_TOP;
	static float pan_step = 0.002f;
	static float zoom_value = 1.000f;

	// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	// Default camera values
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;

	// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL

	class test_camera
	{
		glm::vec3 m_pos;
		glm::quat m_orient;

	public:
		test_camera(void) = default;
		test_camera(const test_camera &) = default;

		test_camera(const glm::vec3 &pos) : m_pos(pos) {}
		test_camera(const glm::vec3 &pos, const glm::quat &orient) : m_pos(pos), m_orient(orient) {}

		test_camera &operator=(const test_camera &) = default;

		const glm::vec3 &position(void) const { return m_pos; }
		const glm::quat &orientation(void) const { return m_orient; }

		glm::mat4 view(void) const { return glm::translate(glm::mat4_cast(m_orient), m_pos); }

		void translate(const glm::vec3 &v) { m_pos += v * m_orient; }
		void translate(float x, float y, float z) { m_pos += glm::vec3(x, y, z) * m_orient; }

		void rotate(float angle, const glm::vec3 &axis) { m_orient *= glm::angleAxis(angle, axis * m_orient); }
		void rotate(float angle, float x, float y, float z) { m_orient *= glm::angleAxis(angle, glm::vec3(x, y, z) * m_orient); }

		void yaw(float angle) { rotate(angle, 0.0f, 1.0f, 0.0f); }
		void pitch(float angle) { rotate(angle, 1.0f, 0.0f, 0.0f); }
		void roll(float angle) { rotate(angle, 0.0f, 0.0f, 1.0f); }
	};

	class Camera
	{
	public:
		// camera Attributes
		float x = 0;
		float y = 0;

		float rot_x = 0;
		float rot_y = 0;

		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;
		// euler Angles
		float Yaw;
		float Pitch;
		// camera options
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;

		// constructor with vectors
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
		{
			Position = position;
			WorldUp = up;
			Yaw = yaw;
			Pitch = pitch;
			updateCameraVectors();
		}
		// constructor with scalar values
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
		{
			Position = glm::vec3(posX, posY, posZ);
			WorldUp = glm::vec3(upX, upY, upZ);
			Yaw = yaw;
			Pitch = pitch;
			updateCameraVectors();
		}

		// returns the view matrix calculated using Euler Angles and the LookAt Matrix
		glm::mat4 GetViewMatrix()
		{
			return glm::lookAt(Position, Position + Front, Up);
		}

		// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
		void ProcessKeyboard(Camera_Movement direction, float deltaTime)
		{
			float velocity = MovementSpeed * deltaTime;
			if (direction == FORWARD)
			{
				Position += Front * velocity;
				y += velocity;
			}
			if (direction == BACKWARD)
			{
				Position -= Front * velocity;
				y -= velocity;
			}
			if (direction == LEFT)
			{
				Position -= Right * velocity;
				x -= velocity;
			}
			if (direction == RIGHT)
			{
				Position += Right * velocity;
				x += velocity;
			}
		}

		// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
		{
			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch += yoffset;

			// make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (Pitch > 89.0f)
					Pitch = 89.0f;
				if (Pitch < -89.0f)
					Pitch = -89.0f;
			}

			// update Front, Right and Up Vectors using the updated Euler angles
			updateCameraVectors();
		}

		// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
		void ProcessMouseScroll(float yoffset)
		{
			Zoom -= (float)yoffset;
			if (Zoom < 1.0f)
				Zoom = 1.0f;
			if (Zoom > 45.0f)
				Zoom = 45.0f;
		}

	private:
		// calculates the front vector from the camera's (updated) Euler Angles
		void updateCameraVectors()
		{
			// calculate the new Front vector
			glm::vec3 front;
			front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			front.y = sin(glm::radians(Pitch));
			front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			Front = glm::normalize(front);
			// also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
		}
	};

	/* A simple arcball camera that moves around the camera's focal point.
	 * The mouse inputs to the camera should be in normalized device coordinates,
	 * where the top-left of the screen corresponds to [-1, 1], and the bottom
	 * right is [1, -1].
	 */
	class ArcballCamera
	{
		glm::vec3 input_eye;
		glm::vec3 input_center;
		glm::vec3 input_up;

		// We store the unmodified look at matrix along with
		// decomposed translation and rotation components
		glm::mat4 center_translation, translation;
		glm::quat rotation;
		// camera is the full camera transform,
		// inv_camera is stored as well to easily compute
		// eye position and world space rotation axes
		glm::mat4 camera, inv_camera;

	public:
		/* Create an arcball camera focused on some center point
		 * screen: [win_width, win_height]
		 */
		ArcballCamera(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up);

		/* Rotate the camera from the previous mouse position to the current
		 * one. Mouse positions should be in normalized device coordinates
		 */
		void rotate(glm::vec2 prev_mouse, glm::vec2 cur_mouse);

		/* Pan the camera given the translation vector. Mouse
		 * delta amount should be in normalized device coordinates
		 */
		void pan(glm::vec2 mouse_delta);

		/* Zoom the camera given the zoom amount to (i.e., the scroll amount).
		 * Positive values zoom in, negative will zoom out.
		 */
		void zoom(const float zoom_amount);

		// Get the camera transformation matrix
		const glm::mat4 &transform() const;

		// Get the camera's inverse transformation matrix
		const glm::mat4 &inv_transform() const;

		// Get the eye position of the camera in world space
		glm::vec3 eye() const;

		// Get the eye direction of the camera in world space
		glm::vec3 dir() const;

		// Get the up direction of the camera in world space
		glm::vec3 up() const;

	private:
		void update_camera();
	};

	// Project the point in [-1, 1] screen space onto the arcball sphere
	static glm::quat screen_to_arcball(const glm::vec2 &p);

	inline ArcballCamera::ArcballCamera(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up)
	{
		input_eye = glm::vec3(0, 0, 5);
		input_center = glm::vec3(0);
		input_up = glm::vec3(0, 1, 0);

		const glm::vec3 dir = center - eye;
		glm::vec3 z_axis = glm::normalize(dir);
		glm::vec3 x_axis = glm::normalize(glm::cross(z_axis, glm::normalize(up)));
		glm::vec3 y_axis = glm::normalize(glm::cross(x_axis, z_axis));
		x_axis = glm::normalize(glm::cross(z_axis, y_axis));

		center_translation = glm::inverse(glm::translate(center));
		translation = glm::translate(glm::vec3(0.f, 0.f, -glm::length(dir)));
		rotation = glm::normalize(glm::quat_cast(glm::transpose(glm::mat3(x_axis, y_axis, -z_axis))));

		update_camera();
	}

	inline void ArcballCamera::rotate(glm::vec2 prev_mouse, glm::vec2 cur_mouse)
	{
		// Clamp mouse positions to stay in NDC
		cur_mouse = glm::clamp(cur_mouse, glm::vec2{-1, -1}, glm::vec2{1, 1});
		prev_mouse = glm::clamp(prev_mouse, glm::vec2{-1, -1}, glm::vec2{1, 1});

		const glm::quat mouse_cur_ball = screen_to_arcball(cur_mouse);
		const glm::quat mouse_prev_ball = screen_to_arcball(prev_mouse);

		rotation = mouse_cur_ball * mouse_prev_ball * rotation;
		update_camera();
	}

	inline void ArcballCamera::pan(glm::vec2 mouse_delta)
	{
		const float zoom_amount = std::abs(translation[3][2]);
		glm::vec4 motion(mouse_delta.x * zoom_amount, mouse_delta.y * zoom_amount, 0.f, 0.f);
		// Find the panning amount in the world space
		motion = inv_camera * motion;

		center_translation = glm::translate(glm::vec3(motion)) * center_translation;
		update_camera();
	}

	inline void ArcballCamera::zoom(const float zoom_amount)
	{
		const glm::vec3 motion(0.f, 0.f, zoom_amount);

		translation = glm::translate(motion) * translation;
		update_camera();
	}

	inline const glm::mat4 &ArcballCamera::transform() const
	{
		return camera;
	}

	inline const glm::mat4 &ArcballCamera::inv_transform() const
	{
		return inv_camera;
	}

	inline glm::vec3 ArcballCamera::eye() const
	{
		return glm::vec3{inv_camera * glm::vec4{0, 0, 0, 1}};
	}

	inline glm::vec3 ArcballCamera::dir() const
	{
		return glm::normalize(glm::vec3{inv_camera * glm::vec4{0, 0, -1, 0}});
	}

	inline glm::vec3 ArcballCamera::up() const
	{
		return glm::normalize(glm::vec3{inv_camera * glm::vec4{0, 1, 0, 0}});
	}

	inline void ArcballCamera::update_camera()
	{
		camera = translation * glm::mat4_cast(rotation) * center_translation;
		inv_camera = glm::inverse(camera);
	}

	inline glm::quat screen_to_arcball(const glm::vec2 &p)
	{
		const float dist = glm::dot(p, p);
		// If we're on/in the sphere return the point on it
		if (dist <= 1.f)
		{
			return glm::quat(0.0, p.x, p.y, std::sqrt(1.f - dist));
		}
		else
		{
			// otherwise we project the point onto the sphere
			const glm::vec2 proj = glm::normalize(p);
			return glm::quat(0.0, proj.x, proj.y, 0.f);
		}
	}

	// cameras
	static Camera camera(glm::vec3(0.0f, 0.75f, 3.0f));
	static float to_left = 0.5f;
	static float retreat = 5.0f;
	// static ArcballCamera camera_arc(glm::vec3(to_left, 0, retreat), glm::vec3(to_left, 0, 0), glm::vec3(0, 1, 0));//eye center up
	// static ArcballCamera camera_arc(glm::vec3(retreat, retreat, retreat), glm::vec3(to_left, to_left, to_left), glm::vec3(0, 0, 1));
	static ArcballCamera camera_arc(glm::vec3(to_left, 0, retreat), glm::vec3(to_left, 0, 0), glm::vec3(0, 1, 0));
	inline void set_camera()
	{
		switch (cam_type)
		{
		case CAMERA_WALK_ORTHO: // ortho
			break;

		case CAMERA_WALK_PERSPECTIVE: // perspective + walk around + zoom
			break;

		case (CAMERA_ARCBALL_ORTHO):
			camera_arc = ArcballCamera(glm::vec3(0, 0, retreat), glm::vec3(-to_left, -to_left, -to_left), glm::vec3(0, -1, 0));
			break;

		case (CAMERA_ARCBALL_ORTHO_TOP):

			camera_arc = ArcballCamera(glm::vec3(to_left, 0, retreat), glm::vec3(to_left, 0, 0), glm::vec3(0, 1, 0));
			break;

		case (CAMERA_ARCBALL_ORTHO_BOTTOM):
			camera_arc = ArcballCamera(glm::vec3(-to_left, 0, -retreat), glm::vec3(-to_left, 0, 0), glm::vec3(0, 1, 0));
			break;

		case (CAMERA_ARCBALL_ORTHO_LEFT):
			camera_arc = ArcballCamera(glm::vec3(-retreat, -to_left, 1), glm::vec3(0, -to_left, 1), glm::vec3(0, 0, 1));
			break;

		case (CAMERA_ARCBALL_ORTHO_RIGHT):
			camera_arc = ArcballCamera(glm::vec3(retreat, to_left, 1), glm::vec3(0, to_left, 1), glm::vec3(0, 0, 1));
			break;

		case (CAMERA_ARCBALL_ORTHO_FRONT):
			camera_arc = ArcballCamera(glm::vec3(to_left, -retreat, 1), glm::vec3(to_left, 0, 1), glm::vec3(0, 0, 1));
			break;

		case (CAMERA_ARCBALL_ORTHO_BACK):
			camera_arc = ArcballCamera(glm::vec3(-to_left, retreat, 1), glm::vec3(-to_left, 0, 1), glm::vec3(0, 0, 1));
			break;

		case (CAMERA_ARCBALL_PERSPECTIVE):
			break;

		default: // perspective
			break;
		}
	}

	inline void projection_matrix(bool rotate = false)
	{ // camera_type perpective_type,
		opengl_globals::projection = glm::mat4(1.0f);
		opengl_globals::view = glm::mat4(1.0f);
		opengl_globals::model = glm::mat4(1.0f);
		opengl_globals::projection_view = glm::mat4(1.0f);

		static bool invert = false;
		float angle = opengl_globals::time * 20.0f;
		float ascpect = static_cast<float>(opengl_globals::SCR_WIDTH) / static_cast<float>(opengl_globals::SCR_HEIGHT);
		float orthoX = ascpect > 1.0f ? ascpect : 1.0f;
		float orthoY = ascpect > 1.0f ? 1.0f : 1.0f / ascpect;
		// float zoom = 1 - (camera.Zoom - 44.0f) * 0.1f;
		// float zoom = camera.Zoom * 0.1f;
		// std::cout << camera.Zoom << std::endl;

		switch (cam_type)
		{
		case CAMERA_WALK_ORTHO: // ortho
			opengl_globals::projection = glm::ortho(
				(float)opengl_globals::SCR_CORNER_X * 0.0f,
				(float)opengl_globals::SCR_WIDTH,
				(float)opengl_globals::SCR_CORNER_Y * 0.0f,
				(float)opengl_globals::SCR_HEIGHT, -1.0f, 100.0f);

			opengl_globals::projection = opengl_globals::projection = glm::ortho(-orthoX * opengl_cameras::zoom_value, +orthoX * opengl_cameras::zoom_value, -orthoY * opengl_cameras::zoom_value, +orthoY * opengl_cameras::zoom_value, -1000.1f, 1000.0f);
			// opengl_globals::view = camera.GetViewMatrix();
			opengl_globals::view = glm::eulerAngleYX(camera.Yaw * 0.01, camera.Pitch * 0.01);
			// opengl_globals::model = glm::translate(glm::vec3(camera.Position.x, camera.Position.z, 0.0f));

			opengl_globals::model = glm::lookAt(
				// glm::vec3(0.0f, 1.0f, 1.1f),//eye
				glm::vec3(0.0f + camera.x, 1.0f + camera.y, 1.0f),
				glm::vec3(0.0f + camera.x, 1.0f + camera.y, 0.0f), // center
				glm::vec3(0.0f, 10.0f, 0.0f)					   // up
			);

			// opengl_globals::model = glm::rotate(opengl_globals::model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
			// opengl_globals::model = translation;
			break;

		case CAMERA_WALK_PERSPECTIVE: // perspective + walk around + zoom
			opengl_globals::projection = glm::perspective(glm::radians(camera.Zoom), ascpect, 0.1f, 100.0f);
			opengl_globals::view = camera.GetViewMatrix();

			break;

		case (CAMERA_ARCBALL_ORTHO):
		case (CAMERA_ARCBALL_ORTHO_TOP):
		case (CAMERA_ARCBALL_ORTHO_BOTTOM):
		case (CAMERA_ARCBALL_ORTHO_LEFT):
		case (CAMERA_ARCBALL_ORTHO_RIGHT):
		case (CAMERA_ARCBALL_ORTHO_FRONT):
		case (CAMERA_ARCBALL_ORTHO_BACK):

			opengl_globals::projection = glm::ortho(
				(float)opengl_globals::SCR_CORNER_X * 0.0f,
				(float)opengl_globals::SCR_WIDTH,
				(float)opengl_globals::SCR_CORNER_Y * 0.0f,
				(float)opengl_globals::SCR_HEIGHT, -1.0f, 100.0f);
			orthoX *= 2;
			orthoY *= 2;
			// opengl_globals::projection = glm::ortho(-orthoX * zoom, +orthoX * zoom, -orthoY * zoom, +orthoY * zoom, -1000.1f, 1000.0f);
			opengl_globals::projection = glm::ortho(-orthoX * opengl_cameras::zoom_value, +orthoX * opengl_cameras::zoom_value, -orthoY * opengl_cameras::zoom_value, +orthoY * opengl_cameras::zoom_value, -1000.1f, 1000.0f);
			opengl_globals::view = camera_arc.transform();
			// opengl_globals::view = camera_arc.transform();
			break;

		case (CAMERA_ARCBALL_PERSPECTIVE):

			// https://github.com/Twinklebear/arcball-cpp/blob/master/example/example.cpp
			opengl_globals::projection = glm::perspective(glm::radians(45.f), ascpect, 0.1f, 100.f);
			opengl_globals::view = camera_arc.transform();

			break;

		default: // perspective
			opengl_globals::projection = glm::perspective(glm::radians(65.f), ascpect, 0.1f, 100.f);
			break;
		}

		if (rotate)
			opengl_globals::model = glm::rotate(opengl_globals::model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

		// opengl_globals::model = glm::translate(opengl_globals::model, glm::vec3(0.1f, 0.0f, 0.2f));
		// opengl_globals::light_pos = glm::vec3(0.0f, 1.0f, -10.0f);
		opengl_globals::projection_view = opengl_globals::projection * opengl_globals::view * opengl_globals::model;
		// glm::vec4 light_cam_pos(opengl_globals::light_pos.x, opengl_globals::light_pos.y, opengl_globals::light_pos.z, 1);
		// light_cam_pos = light_cam_pos * (opengl_globals::projection * opengl_globals::model);
		// opengl_globals::light_pos *= glm::vec3(light_cam_pos.x, light_cam_pos.y, light_cam_pos.z);
	}
}