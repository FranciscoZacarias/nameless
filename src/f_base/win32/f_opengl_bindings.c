#define FUNC_DECL(name, return_type, parameters) \
    typedef return_type GL_##name parameters; \
    internal GL_##name *name = NULL;

FUNC_DECL(Viewport,                   void,          (GLint x, GLint y, GLint width, GLint height))
FUNC_DECL(ClearColor,                 void,          (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha))
FUNC_DECL(Clear,                      void,          (GLenum mask))
FUNC_DECL(CreateShader,               GLuint,        (GLenum type))
FUNC_DECL(ShaderSource,               void,          (GLuint shader, GLint count, const GLchar **string, const GLint *length))
FUNC_DECL(CompileShader,              void,          (GLuint shader))
FUNC_DECL(CreateProgram,              GLuint,        (void))
FUNC_DECL(AttachShader,               void,          (GLuint program, GLuint shader))
FUNC_DECL(LinkProgram,                void,          (GLuint program))
FUNC_DECL(UseProgram,                 void,          (GLuint program))
FUNC_DECL(GenVertexArrays,            void,          (GLint n, GLuint *arrays))
FUNC_DECL(GenBuffers,                 void,          (GLint n, GLuint *buffers))
FUNC_DECL(BindVertexArray,            void,          (GLuint array))
FUNC_DECL(BindBuffer,                 void,          (GLenum target, GLuint buffer))
FUNC_DECL(BufferData,                 void,          (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage))
FUNC_DECL(VertexAttribPointer,        void,          (GLuint index, GLint size, GLenum type, GLboolean normalized, GLint stride, const GLvoid *pointer))
FUNC_DECL(EnableVertexAttribArray,    void,          (GLuint index))
FUNC_DECL(DrawArrays,                 void,          (GLenum mode, GLint first, GLint count))
FUNC_DECL(GetString,                  const GLchar*, (GLenum name))

internal GL_Viewport                *glViewport                = NULL;
internal GL_ClearColor              *glClearColor              = NULL;
internal GL_Clear                   *glClear                   = NULL;
internal GL_CreateShader            *glCreateShader            = NULL;
internal GL_ShaderSource            *glShaderSource            = NULL;
internal GL_CompileShader           *glCompileShader           = NULL;
internal GL_CreateProgram           *glCreateProgram           = NULL;
internal GL_AttachShader            *glAttachShader            = NULL;
internal GL_LinkProgram             *glLinkProgram             = NULL;
internal GL_UseProgram              *glUseProgram              = NULL;
internal GL_GenVertexArrays         *glGenVertexArrays         = NULL;
internal GL_GenBuffers              *glGenBuffers              = NULL;
internal GL_BindVertexArray         *glBindVertexArray         = NULL;
internal GL_BindBuffer              *glBindBuffer              = NULL;
internal GL_BufferData              *glBufferData              = NULL;
internal GL_VertexAttribPointer     *glVertexAttribPointer     = NULL;
internal GL_EnableVertexAttribArray *glEnableVertexAttribArray = NULL;
internal GL_DrawArrays              *glDrawArrays              = NULL;
internal GL_GetString               *glGetString               = NULL;

#define LOAD_GL_FUNCTION(name) \
  name = (GL_##name*)wglGetProcAddress("gl" #name);            \
  if (!name) {                                                 \
    error_message("Failed to load function: gl%s\n", #name);   \
  }

b32 opengl_init() {

  LOAD_GL_FUNCTION(Viewport)
  LOAD_GL_FUNCTION(ClearColor)
  LOAD_GL_FUNCTION(Clear)
  LOAD_GL_FUNCTION(CreateShader)
  LOAD_GL_FUNCTION(ShaderSource)
  LOAD_GL_FUNCTION(CompileShader)
  LOAD_GL_FUNCTION(CreateProgram)
  LOAD_GL_FUNCTION(AttachShader)
  LOAD_GL_FUNCTION(LinkProgram)
  LOAD_GL_FUNCTION(UseProgram)
  LOAD_GL_FUNCTION(GenVertexArrays)
  LOAD_GL_FUNCTION(GenBuffers)
  LOAD_GL_FUNCTION(BindVertexArray)
  LOAD_GL_FUNCTION(BindBuffer)
  LOAD_GL_FUNCTION(BufferData)
  LOAD_GL_FUNCTION(VertexAttribPointer)
  LOAD_GL_FUNCTION(EnableVertexAttribArray)
  LOAD_GL_FUNCTION(DrawArrays)
  LOAD_GL_FUNCTION(GetString)

  return true;
}

#undef FUNC_DECL
#undef LOAD_GL_FUNCTION