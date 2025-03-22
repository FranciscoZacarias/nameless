internal OGL_Shader ogl_make_shader(const char* source_path, GLenum kind) {
  Arena_Temp scratch = scratch_begin(0, 0);
  OGL_Shader result  = glCreateShader(kind);
  GLenum error       = glGetError();
    
  String shader_source = file_load(scratch.arena, StringLiteral(source_path));
  glShaderSource(result, 1, &shader_source.str, &(GLint)shader_source.size);
  glCompileShader(result);
  
  GLint success;
  glGetShaderiv(result, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[1024];
    glGetShaderInfoLog(result, 1024, NULL, infoLog);
    printf("Error %d while compiling shader. Log: %s", success, infoLog);
    glDeleteShader(result);
  }

  scratch_end(&scratch);
  return result;
}

internal OGL_Shader ogl_make_program(Arena *arena, GLuint *shaders, u32 count) {
  OGL_Shader result = glCreateProgram();
  
  for (u32 i = 0; i < count; i += 1){
    glAttachShader(result, shaders[i]);
  }
  glLinkProgram(result);
    
  GLint success;
  glGetProgramiv(result, GL_LINK_STATUS, &success);
  if(!success) {
    char infoLog[1024];
    glGetProgramInfoLog(result, 1024, NULL, infoLog);
    printf("Error %d linking re-compiled default shader program. Log: %s", success, infoLog);
    for (u32 i = 0; i < count; i += 1){
      glDeleteShader(shaders[i]);
    }
  }
  
  return(result);
}
