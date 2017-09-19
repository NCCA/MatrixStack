#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/Camera.h>
#include <ngl/Transformation.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/NGLStream.h>


NGLScene::NGLScene()
{
  setTitle("A Matrix Stack");
  m_rot=0.0f;
  m_freq=1.0;
  m_stack.setView(ngl::perspective(15,static_cast<float>(width())/height(),0.05f,350.0f));

}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_stack.setProjection( ngl::perspective(45.0f,static_cast<float>(width())/height(),0.05f,350.0f));
}


void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
   // now to load the shader and set the values
  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["nglDiffuseShader"]->use();

  shader->setUniform("Colour",1.0f,1.0f,1.0f,1.0f);
  shader->setUniform("lightPos",1.0f,1.0f,1.0f);
  shader->setUniform("lightDiffuse",1.0f,1.0f,1.0f,1.0f);

  ngl::Vec3 from(0,2,2);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  int w=this->size().width();
  int h=this->size().height();
  // set the view matrix for the stack
  m_stack.setView(ngl::lookAt(from,to,up));
  // set the projection for the stack
  m_stack.setProjection( ngl::perspective(45,(float)w/h,0.05,350));
  // as re-size is not explicitly called we need to do this.
  glViewport(0,0,width(),height());
  ngl::VAOPrimitives::instance()->createLineGrid("grid",10,10,100);
  ngl::VAOPrimitives::instance()->createSphere("sphere",1.0,20);
  startTimer(10);
}


void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  normalMatrix=m_stack.MV();
  normalMatrix.inverse();
  shader->setUniform("MVP",m_stack.MVP());
  shader->setUniform("normalMatrix",normalMatrix);
 }

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  // grab an instance of the shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["nglDiffuseShader"]->use();
  shader->setUniform("Colour",1.0f,1.0f,1.0f,1.0f);


  // Rotation based on the mouse position for our global transform
  m_stack.pushMatrix();
    m_stack.translate(0,0,m_modelPos.m_z);
    m_stack.translate(m_modelPos.m_x,m_modelPos.m_y,0);
    m_stack.rotate(m_win.spinXFace,1,0,0);
    m_stack.rotate(m_win.spinYFace,0,1,0);
     // get the VBO instance and draw the built in teapot
    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  // draw
    m_stack.pushMatrix();
      m_stack.translate(0,-0.65f,0);
      loadMatricesToShader();
      prim->draw("troll");
    m_stack.popMatrix();

    m_stack.pushMatrix();
      m_stack.scale(0.5,0.5,0.5);
      m_stack.translate(-1.0,-1.85f,-1.0);
      m_stack.rotate(45,0,1,0);
      loadMatricesToShader();
      prim->draw("troll");
    m_stack.popMatrix();

    m_stack.pushMatrix();
      m_stack.scale(0.5,0.5,0.5);
      m_stack.translate(1.0,-1.85f,-1.0);
      loadMatricesToShader();
      prim->draw("troll");
    m_stack.popMatrix();


    for(float i=0; i<2*M_PI; i+=0.01)
    {
      m_stack.pushMatrix();
        float x=cosf(i)*2.0f;
        float z=sinf(i)*2.0f;
        float y=sinf(i*m_freq)*0.5f;
        shader->setUniform("Colour",x,y,z,1.0f);

        m_stack.rotate(m_rot,0,1,0);
        m_stack.translate(x,y,z);
        m_stack.pushMatrix();
          m_stack.scale(0.04f,0.04f,0.04f);
          if(i>180)
            m_stack.rotate(m_rot*2,0,0,1);
          else
            m_stack.rotate(m_rot*2,0,1,0);


          loadMatricesToShader();
          prim->draw("sphere");
        m_stack.popMatrix();
      m_stack.popMatrix();
    }

    m_stack.pushMatrix();
      m_stack.translate(0.0,-1.2f,0.0);
      loadMatricesToShader();
      prim->draw("grid");
   m_stack.popMatrix();
  m_stack.popMatrix();

}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
  // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  case Qt::Key_I : m_freq+=1; break;
  case Qt::Key_O : m_freq-=1; break;

  default : break;
  }
  // finally update the GLWindow and re-draw
  //if (isExposed())
    update();
}

void NGLScene::timerEvent(QTimerEvent *)
{
  m_rot+=1.0f;
  update();
}



