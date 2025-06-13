#define Pi 3.141592653589793238462643383279f
class camera {
private:
	glm::vec3 CameraPos;
	glm::vec2 AngularViewDirection;
	glm::mat4 view;
	glm::mat4 perspective;
	float FOV;
	float nearPlane;
	float farPlane;
	float ar;
public:
	
	camera(glm::vec3 CameraPosition,glm::vec2 AngularDirection,float fov,float AR,float Boundaries[2])
		: FOV(fov), nearPlane(Boundaries[0]), farPlane(Boundaries[1]), ar(AR) {

		CameraPos = CameraPosition;
		AngularViewDirection = AngularDirection;
		view = glm::lookAt(CameraPos,
		CameraPos + glm::vec3(cos(AngularViewDirection.x)* cos(AngularViewDirection.y), sin(AngularViewDirection.y), sin(AngularViewDirection.x) * cos(AngularViewDirection.y)),
		glm::vec3(0.0, 1.0, 0.0));
		updatePerspectiveMatrix();
	}
	void moveCameraIfArrow(GLFWwindow* window) {
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			CameraPos = CameraPos + glm::vec3(-sin(AngularViewDirection.x) * 0.1, 0.0,  cos(AngularViewDirection.x) * 0.1);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			CameraPos = CameraPos + glm::vec3(-sin(AngularViewDirection.x) * - 0.1, 0.0,  cos(AngularViewDirection.x) * -0.1);
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			CameraPos = CameraPos + glm::vec3(cos(AngularViewDirection.x) * cos(AngularViewDirection.y + Pi / 2) * 0.1, sin(AngularViewDirection.y + Pi / 2) * 0.1, sin(AngularViewDirection.x) * cos(AngularViewDirection.y + Pi / 2) * 0.1);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			CameraPos = CameraPos + glm::vec3(cos(AngularViewDirection.x) * cos(AngularViewDirection.y+Pi/2) * -0.1, sin(AngularViewDirection.y + Pi / 2) * -0.1, sin(AngularViewDirection.x) * cos(AngularViewDirection.y + Pi / 2) * -0.1);
		}
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
			CameraPos = CameraPos + glm::vec3(cos(AngularViewDirection.x) * cos(AngularViewDirection.y) * 0.1, sin(AngularViewDirection.y) * 0.1, sin(AngularViewDirection.x) * cos(AngularViewDirection.y) * 0.1);
		}
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
			CameraPos = CameraPos + glm::vec3(cos(AngularViewDirection.x) * cos(AngularViewDirection.y) * -0.1, sin(AngularViewDirection.y) * -0.1, sin(AngularViewDirection.x) * cos(AngularViewDirection.y) * -0.1);
		}
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
			AngularViewDirection = AngularViewDirection + glm::vec2(0.0, 0.02);
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			AngularViewDirection = AngularViewDirection + glm::vec2(0.0, -0.02);
		}
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
			AngularViewDirection = AngularViewDirection + glm::vec2(0.02, 0.0);
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			AngularViewDirection = AngularViewDirection + glm::vec2(-0.02, 0.0);
		}
		view = glm::lookAt(CameraPos,
			CameraPos + glm::vec3(cos(AngularViewDirection.x) * cos(AngularViewDirection.y), sin(AngularViewDirection.y),sin(AngularViewDirection.x) * cos(AngularViewDirection.y)),
			glm::vec3(0.0, 1.0, 0.0));
	}
	void updatePerspectiveMatrix() {
		perspective = glm::perspective(glm::radians(FOV), ar, nearPlane, farPlane);
	}
	void setAspectRatio(float newAspect) {
		ar = newAspect;
		updatePerspectiveMatrix();
	}
	glm::mat4 get_view() {
		return view;
	}
	glm::mat4 get_perspective() {
		return perspective;
	}
	glm::vec3 get_position() {
		return CameraPos;
	}
};
