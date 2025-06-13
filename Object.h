struct Vertex {
	float x, y, z;  // Position
	float u, v;     // Texture Coordinates
	float nx, ny, nz; // Normals
};
enum case_state {
	no_state,cross,miss,check,choose
};
class object {
private:
	GLuint texture;
	GLuint vao;
	int N = 0;
	GLuint ImportObject(const std::string& filename, int& N) {
		std::string filename_bis = "data/" + filename;
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;

		std::ifstream file(filename_bis);
		if (!file.is_open()) {
			std::cerr << "Failed to open OBJ file: " << filename << std::endl;
			return 0;
		}

		std::string line;
		while (std::getline(file, line)) {
			std::istringstream iss(line);
			std::string prefix;
			iss >> prefix;

			if (prefix == "v") {  // Vertex position
				glm::vec3 pos;
				iss >> pos.x >> pos.y >> pos.z;
				positions.push_back(pos);
			}
			else if (prefix == "vt") {  // Texture coordinate
				glm::vec2 tex;
				iss >> tex.x >> tex.y;
				texCoords.push_back(tex);
			}
			else if (prefix == "vn") {  // Normal
				glm::vec3 norm;
				iss >> norm.x >> norm.y >> norm.z;
				normals.push_back(norm);
			}
			else if (prefix == "f") {  // Faces
				std::string vertexInfo;
				for (int i = 0; i < 3; i++) {
					iss >> vertexInfo;
					std::istringstream viss(vertexInfo);
					std::string v, vt, vn;

					std::getline(viss, v, '/');
					std::getline(viss, vt, '/');
					std::getline(viss, vn, '/');

					int vIdx = std::stoi(v) - 1;
					int vtIdx = vt.empty() ? -1 : std::stoi(vt) - 1;
					int vnIdx = vn.empty() ? -1 : std::stoi(vn) - 1;

					Vertex vertex;
					vertex.x = positions[vIdx].x;
					vertex.y = positions[vIdx].y;
					vertex.z = positions[vIdx].z;

					if (vtIdx >= 0) {
						vertex.u = -texCoords[vtIdx].y;
						vertex.v = texCoords[vtIdx].x;
					}
					else {
						vertex.u = vertex.v = 0;
					}

					if (vnIdx >= 0) {
						vertex.nx = normals[vnIdx].x;
						vertex.ny = normals[vnIdx].y;
						vertex.nz = normals[vnIdx].z;
					}
					else {
						vertex.nx = vertex.ny = vertex.nz = 0;
					}

					vertices.push_back(vertex);
					indices.push_back(vertices.size() - 1);
				}
			}
		}

		file.close();

		// OpenGL Buffer Setup
		GLuint VAO, VBO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
		N = indices.size();

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);

		// Texture coordinate attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Normal attribute
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		return VAO;
	}
	GLuint ImportTexture(const std::string& filename) {
		std::string filename_bis = "data/" + filename;
		GLuint texture;
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int imWidth, imHeight, imNrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filename_bis.c_str(), &imWidth, &imHeight, &imNrChannels, 0);
		if (data)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imWidth, imHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to Load texture" << std::endl;
			const char* reason = stbi_failure_reason();
			std::cout << reason << std::endl;
		}
		stbi_image_free(data);
		return texture;
	}
protected:
	glm::mat4 model = glm::mat4(1.0);
public:

	object(const std::string& object_filename, const std::string& texture_filename){
		vao = ImportObject(object_filename, N);
		texture = ImportTexture(texture_filename);
	}
	
	void set_model(glm::mat4 Model) {
		model = Model;
	}
	glm::mat4 get_model() {
		return model;
	}
	GLuint get_VAO() {
		return vao;
	}
	int get_N() {
		return N;
	}
	GLuint get_texture() {
		return texture;
	}
};

class base : public object {
private:
public:
	base(const std::string& object_filename, const std::string& texture_filename) :object(object_filename, texture_filename) {
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				Ships[i][j] = false;
			}
		}
	}
	bool Ships[10][10]; // 0 nothing , 1 ship
};

class ship : public object {
private:
	glm::vec2 size;
	bool orientation = 0;
	double movement_time = glfwGetTime();
public:
	ship(const std::string& object_filename, const std::string& texture_filename,int Size) :object(object_filename,texture_filename) {
		size = glm::vec2(1, Size);
		model = glm::translate(model, glm::vec3(0.0, 1.5, 0.0));
	}

	glm::vec2 get_size() {
		return size;
	}

	void change_orientation() {
		size = glm::vec2(size.y, size.x);
		orientation = !orientation;
	}

	bool get_orientation() {
		return orientation;
	}
	void Positioning(GLFWwindow* window,double now) {
		if ((now - movement_time) >= 0.1) {
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && model[3].x < (10 - size.x)) {
				model[3] = model[3] + glm::vec4(1.0, 0.0, 0.0, 0.0);
				movement_time = glfwGetTime();
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && model[3].x > 0) {
				model[3] = model[3] + glm::vec4(-1.0, 0.0, 0.0, 0.0);
				movement_time = glfwGetTime();
			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && model[3].z < 0 - (int)orientation) {
				model[3] = model[3] + glm::vec4(0.0, 0.0, 1.0, 0.0);
				movement_time = glfwGetTime();
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && model[3].z > (-10 + size.y - (int)orientation)) {
				model[3] = model[3] + glm::vec4(0.0, 0.0, -1.0, 0.0);
				movement_time = glfwGetTime();
			}
			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && model[3].z > (-12 + size.x) && model[3].x < 11 - size.y) {

				model = glm::rotate(model, (2 * (int)orientation - 1) * (Pi / 2.0f), glm::vec3(0.0, 1.0, 0.0));
				model[3] = model[3] + glm::vec4(0.0, 0.0, (2 * (int)orientation - 1) * 1.0, 0.0);
				change_orientation();
				movement_time = glfwGetTime();
			}
		}
	}
};

class tablette : public object {
private:
	static bool initialized ;
	static GLuint cross_tex;
	static GLuint miss_tex;
	static GLuint check_tex;
	static GLuint choose_tex;
	GLuint interact;
	GLubyte screen_data[100];
	int ship_counter = 10;
	glm::vec2 cursor = glm::vec2(1.0, 1.0);
	case_state pre_cursor = no_state;

	void new_texture(GLuint& lambda_tex,std::string filename_tex) {
		std::string filename_tex_bis = "data/" + filename_tex;
		glGenTextures(1, &lambda_tex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lambda_tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int imWidth, imHeight, imNrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filename_tex_bis.c_str(), &imWidth, &imHeight, &imNrChannels, 0);
		if (data)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imWidth, imHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to Load texture" << std::endl;
			const char* reason = stbi_failure_reason();
			std::cout << reason << std::endl;
		}
		stbi_image_free(data);
	}
	void new_interact(GLuint& interaction) {
		for (int i = 0; i < 100; ++i) {
			screen_data[i] = 0.1*255;
		}

		glGenTextures(1, &interaction);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, interaction);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Allocate and upload the full base texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 10, 10, 0, GL_RED, GL_UNSIGNED_BYTE, screen_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	double attack_time = glfwGetTime();
public:
	tablette(const std::string& object_filename, const std::string& texture_filename) :object(object_filename, texture_filename) {
		if (!initialized) {
			new_texture(cross_tex, "Tablette_cross.jpg");
			new_texture(miss_tex, "Tablette_miss.jpg");
			new_texture(check_tex, "Tablette_check.jpg");
			new_texture(choose_tex, "Tablette_choose.jpg");
			new_interact(interact);
		}
	}
	GLuint get_cross_tex() {
		return cross_tex;
	}
	GLuint get_miss_tex() {
		return miss_tex;
	}
	GLuint get_check_tex() {
		return check_tex;
	}
	GLuint get_choose_tex() {
		return choose_tex;
	}
	GLuint get_interact() {
		return interact;
	}

	void set_screen_data(int x, int y, case_state a) {

		float temp = (a * 0.2) + 0.1; // setup everything for this to work ( i.e same order of states everywhere )
		screen_data[(y - 1) * 10 + (x - 1)] = temp * 255;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, interact);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 10, 10, 0, GL_RED, GL_UNSIGNED_BYTE, screen_data);
		glGenerateMipmap(GL_TEXTURE_2D);

	}
	bool Attack(base& Base) {
		if (Base.Ships[int(cursor.x)-1][int(cursor.y)-1]) {

			set_screen_data(cursor.x, cursor.y, cross);
			Base.Ships[int(cursor.x)][int(cursor.y)] = false;
			decrease_counter();
			return true;
			
		}
		else{
			set_screen_data(cursor.x, cursor.y, miss);
			return false;
		}
	}
	void Choose_Attack(GLFWwindow* window,double now) {
		if ((now - attack_time) >= 0.1) {
			bool change = false;
			glm::vec2 temp = cursor;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && cursor.y > 1) {
				cursor = cursor + glm::vec2(0.0, -1.0);
				change = true;
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && cursor.y < 10) {
				cursor = cursor + glm::vec2(0.0, 1.0);
				change = true;
			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && cursor.x < 10) {
				cursor = cursor + glm::vec2(1.0, 0.0);
				change = true;
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && cursor.x > 1) {
				cursor = cursor + glm::vec2(-1.0, 0.0);
				change = true;
			}
			if (change) {
				attack_time = glfwGetTime();
				set_screen_data(temp.x, temp.y, pre_cursor);
				pre_cursor = case_state(round(((float(screen_data[(int(cursor.y) - 1) * 10 + (int(cursor.x) - 1)]) / 255.0)-0.1)*5));
				set_screen_data(cursor.x, cursor.y, choose);
			}
			
		}
	}
	void set_cursor(int x, int y) {
		cursor = glm::vec2(float(x), float(y));
	}
	void decrease_counter() {
		ship_counter--;
	}
	void setup_pre_cursor() {
		pre_cursor = case_state(round(((float(screen_data[(int(1) - 1) * 10 + (int(1) - 1)]) / 255.0) - 0.1) * 5));
	}
	case_state get_actual_screen_data() {
		return pre_cursor;
	}
	glm::vec2 get_cursor() {
		return cursor;
	}
	bool check_win() {
		if (ship_counter > 0) {
			return false;
		}
		else {
			return true;
		}
	}
};