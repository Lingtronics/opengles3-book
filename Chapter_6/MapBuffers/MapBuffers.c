//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// VertexBufferObjects.c
//
//    This example demonstrates mapping buffer objects
//
#include "esUtil.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // VertexBufferObject Ids
   GLuint vboIds[2];

} UserData;


#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1


int Init ( ESContext *esContext )
{
   UserData *userData = (UserData*) esContext->userData;
   const char vShaderStr[] =  
      "#version 300 es                            \n"
      "layout(location = 0) in vec4 a_position;   \n"
      "layout(location = 1) in vec4 a_color;      \n"
      "out vec4 v_color;                          \n"
      "void main()                                \n"
      "{                                          \n"
      "    v_color = a_color;                     \n"
      "    gl_Position = a_position;              \n"
      "}";

   
   const char fShaderStr[] =
      "#version 300 es            \n"
      "precision mediump float;   \n"
      "in vec4 v_color;           \n"
      "out vec4 o_fragColor;      \n"
      "void main()                \n"
      "{                          \n"
      "    o_fragColor = v_color; \n"
      "}" ;

   GLuint programObject;

   // Create the program object
   programObject = esLoadProgram ( vShaderStr, fShaderStr );
   
   if ( programObject == 0 )
      return GL_FALSE;

   // Store the program object
   userData->programObject = programObject;
   userData->vboIds[0] = 0;
   userData->vboIds[1] = 0;

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
   return GL_TRUE;
}



void DrawPrimitiveWithVBOsMapBuffers(ESContext *esContext, 
                           GLint numVertices, GLfloat *vtxBuf,
                           GLint vtxStride, GLint numIndices, 
                           GLushort *indices)
{
   UserData *userData = (UserData*) esContext->userData;
   GLuint   offset = 0; 

   // vboIds[0] - used to store vertex attribute data 
   // vboIds[l] - used to store element indices 
   if ( userData->vboIds[0] == 0 && userData->vboIds[1] == 0 )
   {
      GLfloat *vtxMappedBuf;
      GLushort *idxMappedBuf;

      // Only allocate on the first draw
      glGenBuffers ( 2, userData->vboIds );
    
      glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[0] );
      glBufferData ( GL_ARRAY_BUFFER, vtxStride * numVertices,
                     NULL, GL_STATIC_DRAW );

      vtxMappedBuf = (GLfloat*) 
         glMapBufferRange ( GL_ARRAY_BUFFER, 0, vtxStride * numVertices, 
                            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
      if ( vtxMappedBuf == NULL )
      {
         esLogMessage( "Error mapping vertex buffer object." );
         return;
      }

      // Copy the data into the mapped buffer
      memcpy ( vtxMappedBuf, vtxBuf, vtxStride * numVertices );

      // Unamp the buffer
      if ( glUnmapBuffer( GL_ARRAY_BUFFER ) == GL_FALSE )
      {
         esLogMessage( "Error unmapping array buffer object." );
         return;
      }

      // Map the index buffer
      glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1] );
      glBufferData ( GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(GLushort) * numIndices,
                     NULL, GL_STATIC_DRAW );
      idxMappedBuf = (GLushort*) 
         glMapBufferRange ( GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort) * numIndices, 
                            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
      if ( idxMappedBuf == NULL )
      {
         esLogMessage( "Error mapping element array buffer object." );
         return;
      }

      // Copy the data into the mapped buffer
      memcpy ( idxMappedBuf, indices, sizeof(GLushort) * numIndices );

      // Unamp the buffer
      if ( glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER ) == GL_FALSE )
      {
         esLogMessage( "Error unmapping element array buffer object." );
         return;
      }
   }

   glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]); 

   glEnableVertexAttribArray(VERTEX_POS_INDX); 
   glEnableVertexAttribArray(VERTEX_COLOR_INDX); 

   glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
                        GL_FLOAT, GL_FALSE, vtxStride, 
                        (const void*)offset);

   offset += VERTEX_POS_SIZE * sizeof(GLfloat); 
   glVertexAttribPointer(VERTEX_COLOR_INDX, 
                        VERTEX_COLOR_SIZE,
                        GL_FLOAT, GL_FALSE, vtxStride,
                        (const void*)offset);

   glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT,
                  0);

   glDisableVertexAttribArray(VERTEX_POS_INDX); 
   glDisableVertexAttribArray(VERTEX_COLOR_INDX); 

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Draw ( ESContext *esContext )
{
   UserData *userData = (UserData*) esContext->userData;
   
   // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
   GLfloat vertices[3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] = 
   {  
       0.0f,  0.5f, 0.0f,        // v0
       1.0f,  0.0f, 0.0f, 1.0f,  // c0
      -0.5f, -0.5f, 0.0f,        // v1
       0.0f,  1.0f, 0.0f, 1.0f,  // c1
       0.5f, -0.5f, 0.0f,        // v2
       0.0f,  0.0f, 1.0f, 1.0f,  // c2
   };
   // Index buffer data
   GLushort indices[3] = { 0, 1, 2 };
      
   glViewport ( 0, 0, esContext->width, esContext->height );   
   glClear ( GL_COLOR_BUFFER_BIT );
   glUseProgram ( userData->programObject );

   DrawPrimitiveWithVBOsMapBuffers ( esContext, 3, vertices, 
      sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
      3, indices );
}

void Shutdown ( ESContext *esContext )
{
   UserData *userData = (UserData*) esContext->userData;

   glDeleteProgram ( userData->programObject );
   glDeleteBuffers ( 2, userData->vboIds );
}

int esMain( ESContext *esContext )
{
   esContext->userData = malloc ( sizeof( UserData ) );

   esCreateWindow ( esContext, "VertexBufferObjects", 320, 240, ES_WINDOW_RGB );
   
   if ( !Init ( esContext ) )
      return GL_FALSE;

   esRegisterShutdownFunc ( esContext, Shutdown );
   esRegisterDrawFunc ( esContext, Draw );
   
   return GL_TRUE;
}
