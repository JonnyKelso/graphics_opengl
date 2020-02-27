//#define GL_SILENCE_DEPRECATION
//#define GLFW_INCLUDE_GLCOREARB
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <array>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void check(int shader);
GLuint compileShader(int shaderType, const char* shaderSource, std::string shaderName);

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

class VAOgen
{
public:
    VAOgen()
    {
        glGenVertexArrays(1, &m_VAO);
        
        
    }
    ~VAOgen()
    {
        glDeleteVertexArrays(1, &m_VAO);
    }
    
    GLuint m_VAO;
};
class VBOgen
{
public:
    VBOgen()
    {
        glGenBuffers(1, &m_VBO);
    }
    ~VBOgen()
    {
        glDeleteBuffers(1, &m_VBO);
    }
    
    GLuint m_VBO;
};
class EBOgen
{
public:
    EBOgen()
    {
        glGenBuffers(1, &m_EBO);
    }
    ~EBOgen()
    {
        glDeleteBuffers(1, &m_EBO);
    }
    
    GLuint m_EBO;
};

GLFWwindow* glInitialise(void)
{
    // init lib
    if(!glfwInit())
    {
        std::cout << "Failed to init GLFW" << std::endl;
        return nullptr;
    }
    // ask for 3.2 context on MacOSX
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif //__APPLE__
    
    // create windowed mode GLFW window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // GLAD: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    return window;
}

int main(int argc, char** argv)
{
    GLFWwindow* window = glInitialise();
    if( window == nullptr) { return -1; }
    
    const int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource, "VERTEX");
    const int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource, "FRAGMENT");
    
    // combine shaders into shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // check shader program linking
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::SHADER::LINK_FAILED\n" << infoLog << std::endl;
    }
    
    // don't need the original shader objects now they're successfully linked into a program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up the vertices
    /*std::array<float, 9> vertices{
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };*/
    std::array<float, 18> vertices{
        /* 0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,   // top left
        */
         0.8f,  0.8f, 0.0f,  // 0 - top right
         0.8f, -0.8f, 0.0f,  // 1 - bottom right
        0.0f, -0.8f, 0.0f,   // 2 - bottom middle
        -0.8f, -0.8f, 0.0f,  // 3 - bottom left
        -0.8f,  0.8f, 0.0f,  // 4 - top left
        0.0f, 0.8f, 0.0f     // 5 - top middle
    };
    
    std::array<int, 6> indices{  // note that we start from 0!
        0, 1, 5,   // first triangle
        4, 2, 3    // second triangle
    };
    
   
    VAOgen VAO;
    VBOgen VBO;
    EBOgen EBO;
    
    // bind vertex array first
    glBindVertexArray(VAO.m_VAO);
    
    // then bind and set vertex buffer(s)
    glBindBuffer(GL_ARRAY_BUFFER, VBO.m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    
    // then bind and set element buffer(s)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO.m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
    
    // then configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // we registered VBO as the vertex attributes bound vertex buffer object, so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // also unbind the VAO so other VAO calls won't affect this VAO (not really necessary in practice)
    glBindVertexArray(0);
    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    
    // render loop
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        // render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the buffers

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO.m_VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        
        // set to wireframe mode (GL_FILL for filled mode, GL_LINE for wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        // unbind
        glBindVertexArray(0);
        
        // swap front and back buffers
        glfwSwapBuffers(window);
        // poll and process events
        glfwPollEvents();
    }

    glfwTerminate();
    
    return 0;

}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void check(int shaderIndex, std::string shaderName)
{
    // check vertex shader compilation
    int success;
    char infoLog[512];
    glGetShaderiv(shaderIndex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shaderIndex, 512, NULL, infoLog);
        std::cout << "ERROR::" << shaderName << "::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    else
    {
        std::cout << "Compiled " << shaderName << " shader successfully" << std::endl;
    }
}
GLuint compileShader(int shaderType, const char* shaderSource, std::string shaderName)
{
    // create and compile vertx shader
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    
    check(shader, shaderName);
    
    return shader;
}
