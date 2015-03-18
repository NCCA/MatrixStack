#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Transformation.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/NGLStream.h>


//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.01;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.1;

NGLScene::NGLScene(QWindow *_parent) : OpenGLWindow(_parent)
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0;
  m_spinYFace=0;
  setTitle("A Matrix Stack");
  m_rot=0.0f;
  m_freq=1.0;
}


NGLScene::~NGLScene()
{
  ngl::NGLInit *Init = ngl::NGLInit::instance();
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
  Init->NGLQuit();
}

void NGLScene::resizeEvent(QResizeEvent *_event )
{
  if(isExposed())
  {
  int w=_event->size().width();
  int h=_event->size().height();
  // set the viewport for openGL
  glViewport(0,0,w,h);
  m_stack.setView(ngl::perspective(15,(float)w/h,0.05,350));

  // now set the camera size values as the screen size has changed
  renderLater();
  }
}


void NGLScene::initialize()
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

  shader->setShaderParam4f("Colour",1,1,1,1);
  shader->setShaderParam3f("lightPos",1,1,1);
  shader->setShaderParam4f("lightDiffuse",1,1,1,1);

  ngl::Vec3 from(0,2,2);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  int w=this->size().width();
  int h=this->size().height();
  m_stack.setView(ngl::lookAt(from,to,up));

  m_stack.setProjection( ngl::perspective(45,(float)w/h,0.05,350));
  // as re-size is not explicitly called we need to do this.
  glViewport(0,0,width(),height());
  ngl::VAOPrimitives::instance()->createLineGrid("grid",10,10,100);
  startTimer(10);
  ngl::VAOPrimitives::instance()->createSphere("sphere",1.0,20);
}


void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=m_mouseGlobalTX;
  MV=  M*m_stack.MV();
  MVP= m_stack.MVP();
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
 }

void NGLScene::render()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // grab an instance of the shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["nglDiffuseShader"]->use();
  shader->setShaderParam4f("Colour",1,1,1,1);


  // Rotation based on the mouse position for our global transform
  m_stack.pushMatrix();
    m_stack.translate(0,0,-m_modelPos.m_z);
    m_stack.translate(m_modelPos.m_x,m_modelPos.m_y,0);
    m_stack.rotate(m_spinXFace,1,0,0);
    m_stack.rotate(m_spinYFace,0,1,0);
     // get the VBO instance and draw the built in teapot
    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  // draw
    m_stack.pushMatrix();
      m_stack.translate(0,-0.65,0);
      loadMatricesToShader();
      prim->draw("troll");
    m_stack.popMatrix();

    m_stack.pushMatrix();
      m_stack.scale(0.5,0.5,0.5);
      m_stack.translate(-1.0,-1.85,-1.0);
      m_stack.rotate(45,0,1,0);
      loadMatricesToShader();
      prim->draw("troll");
    m_stack.popMatrix();

    m_stack.pushMatrix();
      m_stack.scale(0.5,0.5,0.5);
      m_stack.translate(1.0,-1.85,-1.0);
      loadMatricesToShader();
      prim->draw("troll");
    m_stack.popMatrix();


    for(float i=0; i<2*M_PI; i+=0.01)
    {
      m_stack.pushMatrix();
        float x=cos(i)*2.0;
        float z=sin(i)*2.0;
        float y=sin(i*m_freq)*0.5;
        shader->setShaderParam4f("Colour",x,y,z,1);

        m_stack.rotate(m_rot,0,1,0);
        m_stack.translate(x,y,z);
        m_stack.pushMatrix();
          m_stack.scale(0.04,0.04,0.04);
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
      m_stack.translate(0.0,-1.2,0.0);
      loadMatricesToShader();
      prim->draw("grid");
   m_stack.popMatrix();
  m_stack.popMatrix();

}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
    renderLater();

  }
        // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    renderLater();

   }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

	// check the diff of the wheel position (0 means no change)
	if(_event->delta() > 0)
	{
		m_modelPos.m_z+=ZOOM;
	}
	else if(_event->delta() <0 )
	{
		m_modelPos.m_z-=ZOOM;
	}
	renderLater();
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
    renderLater();
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
  m_rot+=1.0f;
  renderLater();
}



