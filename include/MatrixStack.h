#ifndef MATRIXSTACK_H_
#define MATRIXSTACK_H_
#include <ngl/Mat4.h>
#include <array>
//----------------------------------------------------------------------------------------------------------------------
/// @brief Simple Matrix Stack for Old school OpenGL
/// transformations in a modern GL context
//----------------------------------------------------------------------------------------------------------------------
constexpr size_t s_stackSize=40;

class MatrixStack
{
  public :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor can set depth
    /// @param _depth stack size default 40
    //----------------------------------------------------------------------------------------------------------------------
    MatrixStack() {}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief push the current value of top onto
    /// the stack
    //----------------------------------------------------------------------------------------------------------------------
    void pushMatrix();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief pop the current top of stack and replace
    /// with last
    //----------------------------------------------------------------------------------------------------------------------
    void popMatrix();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief similar to glIdentity() will reset top of stack (but not view)
    //----------------------------------------------------------------------------------------------------------------------
    void identity(){m_stack[m_top].identity(); }

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief return the matrix at the top of the stack
    /// @returns ngl::Mat4 the current top of stack model
    /// transform
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 top() const {return m_stack[m_top];}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set the view matrix which will be post multiplied by current model
    /// and project matrix
    /// @param _v the view matrix to use
    //----------------------------------------------------------------------------------------------------------------------
    void setView(const ngl::Mat4 &_v){  m_view=_v; }
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set the projection matrix which will be post multiplied by current model
    /// and model matrix
    /// @param _v the view projection matrix to use
    //----------------------------------------------------------------------------------------------------------------------
    void setProjection(const ngl::Mat4 &_p) {m_projection=_p;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief create a rotation using x,y,z rolls
    /// @param _x the x rotation in degrees
    /// @param _y the x rotation in degrees
    /// @param _z the x rotation in degrees
    /// @note this is done as follows
    ///  ngl::Mat4 final=z*y*x; where each is a rotation matrix
    //----------------------------------------------------------------------------------------------------------------------
    void rotate(float _x, float _y, float _z);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief axis angle rotation like glRotate
    /// @param _angle the angle of rotation in degrees
    /// @param _x axis value
    /// @param _y axis value
    /// @param _z axis value
    /// @note this works like glRotate so we could do
    /// rotate(45,0,1,0) to rotate 45 degrees in the Y
    //----------------------------------------------------------------------------------------------------------------------
    void rotate(float _angle,float _x, float _y, float _z);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief translate similar to glTranslate
    /// @param _x the x translation
    /// @param _y the x translation
    /// @param _z the x translation
    //----------------------------------------------------------------------------------------------------------------------
    void translate(float _x, float _y, float _z);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief scale similar to glScale a uniform scale around the origin
    /// @param _x the x scale
    /// @param _y the x scale
    /// @param _z the x scale
    //----------------------------------------------------------------------------------------------------------------------
    void scale(float _x, float _y, float _z);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief get the Model View Project matrix
    /// @returns the MVP matrix product
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 MVP()const
    { return m_projection*m_view*m_stack[m_top]; }
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief get the Model View  matrix
    /// @returns the MV matrix product
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 MV()const
    { return m_view*m_stack[m_top]; }

private :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief make this non-copyable
    //----------------------------------------------------------------------------------------------------------------------
    MatrixStack(const MatrixStack &){}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief make this non-copyable
    //----------------------------------------------------------------------------------------------------------------------
    MatrixStack operator=(const MatrixStack &){return *this;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the actual stack
    //----------------------------------------------------------------------------------------------------------------------
    std::array<ngl::Mat4,s_stackSize> m_stack;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the top of the stack
    //----------------------------------------------------------------------------------------------------------------------
    size_t m_top=0;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the depth of the stack
    //----------------------------------------------------------------------------------------------------------------------
    size_t m_depth=s_stackSize;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief view matrix
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_view;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief projection matrix
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_projection;
};

#endif
