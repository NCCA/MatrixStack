#Matrix Stack

A simple demo showing how to generate an OpenGL type push / pop matrix stack.

This simple stack has a default depth of 40 with push and pop operations.

The stack contains 2 matrices we can access for shading, MVP which is the combinations of the current top of stack model transform, the View matrix and the projection matrix. This is usually loaded to the shader for the converting the vertices.

We can also access Model * View which is used for some shading calculations.

the top() method will access the current model transformation from the stack.

The model element of the stack is calculated by creating a matrix for the transformation and then post multiplying this by the top of the stack.



