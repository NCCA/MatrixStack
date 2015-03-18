#ifndef MATRIXSTACK_H__
#define MATRIXSTACK_H__
#include <ngl/Mat4.h>
#include <vector>
class MatrixStack
{
  public :
    MatrixStack(int _depth=40);
    void pushMatrix();
    void popMatrix();
    ngl::Mat4 top() const {return m_stack[m_top];}
    void setView(const ngl::Mat4 &_v);
    void setProjection(const ngl::Mat4 &_p);
    void rotate(float _x, float _y, float _z);
    void rotate(float _angle,float _x, float _y, float _z);
    void translate(float _x, float _y, float _z);
    void scale(float _x, float _y, float _z);
    ngl::Mat4 MVP()const
    { return m_stack[m_top]*m_view*m_projection; }
    ngl::Mat4 MV()const
    { return m_stack[m_top]*m_view; }

private :
    std::vector<ngl::Mat4> m_stack;
    int m_top;
    int m_depth;
    ngl::Mat4 m_view;
    ngl::Mat4 m_projection;
};

#endif
