/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield 
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

#ifndef OSG_PRIMITIVESET
#define OSG_PRIMITIVESET 1

#include "qopenglext.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Vec2d.h"
#include "Vec3d.h"
#include "Vec4d.h"
#include "MixinVector.h"

#include <vector>

namespace osg {

typedef MixinVector<GLsizei> VectorGLsizei;
typedef MixinVector<GLubyte> VectorGLubyte;
typedef MixinVector<GLushort> VectorGLushort;
typedef MixinVector<GLuint> VectorGLuint;

//--class State;

/** A \c PrimitiveFunctor is used (in conjunction with
 *  <tt>osg::Drawable::accept (PrimitiveFunctor&)</tt>) to get access to the
 *  primitives that compose the things drawn by OSG.
 *  <p>If \c osg::Drawable::accept() is called with a \c PrimitiveFunctor
 *  parameter, the \c Drawable will "pretend" it is drawing itself, but instead
 *  of calling real OpenGL functions, it will call <tt>PrimitiveFunctor</tt>'s
 *  member functions that "mimic" the OpenGL calls.
 *  <p>Concrete subclasses of \c PrimitiveFunctor must implement these methods
 *  so that they performs whatever they want.
 */
class PrimitiveFunctor
{
public:

    virtual ~PrimitiveFunctor() {}

    /** Sets the array of vertices used to describe the primitives. Somehow
     *  mimics the OpenGL \c glVertexPointer() function.
     */
    virtual void setVertexArray(unsigned int count,const Vec2* vertices) = 0;

    /** Sets the array of vertices used to describe the primitives. Somehow
     *  mimics the OpenGL \c glVertexPointer() function.
     */
    virtual void setVertexArray(unsigned int count,const Vec3* vertices) = 0;

    /** Sets the array of vertices used to describe the primitives. Somehow
     *  mimics the OpenGL \c glVertexPointer() function.
     */
    virtual void setVertexArray(unsigned int count,const Vec4* vertices) = 0;

    /** Sets the array of vertices used to describe the primitives. Somehow
     *  mimics the OpenGL \c glVertexPointer() function.
     */
    virtual void setVertexArray(unsigned int count,const Vec2d* vertices) = 0;

    /** Sets the array of vertices used to describe the primitives. Somehow
     *  mimics the OpenGL \c glVertexPointer() function.
     */
    virtual void setVertexArray(unsigned int count,const Vec3d* vertices) = 0;

    /** Sets the array of vertices used to describe the primitives. Somehow
     *  mimics the OpenGL \c glVertexPointer() function.
     */
    virtual void setVertexArray(unsigned int count,const Vec4d* vertices) = 0;

    /// Mimics the OpenGL \c glDrawArrays() function.
    virtual void drawArrays(GLenum mode,GLint first,GLsizei count) = 0;

    /// Mimics the OpenGL \c glDrawElements() function.
    virtual void drawElements(GLenum mode,GLsizei count,const GLubyte* indices) = 0;

    /// Mimics the OpenGL \c glDrawElements() function.
    virtual void drawElements(GLenum mode,GLsizei count,const GLushort* indices) = 0;

    /// Mimics the OpenGL \c glDrawElements() function.
    virtual void drawElements(GLenum mode,GLsizei count,const GLuint* indices) = 0;

    /// Mimics the OpenGL \c glBegin() function.
    virtual void begin(GLenum mode) = 0;

    /// Mimics the OpenGL \c glVertex() "family of functions".
    virtual void vertex(const Vec2& vert) = 0;

    /// Mimics the OpenGL \c glVertex() "family of functions".
    virtual void vertex(const Vec3& vert) = 0;

    /// Mimics the OpenGL \c glVertex() "family of functions".
    virtual void vertex(const Vec4& vert) = 0;

    /// Mimics the OpenGL \c glVertex() "family of functions".
    virtual void vertex(float x,float y) = 0;

    /// Mimics the OpenGL \c glVertex() "family of functions".
    virtual void vertex(float x,float y,float z) = 0;

    /// Mimics the OpenGL \c glVertex() "family of functions".
    virtual void vertex(float x,float y,float z,float w) = 0;

    /// Mimics the OpenGL \c glEnd() function.
    virtual void end() = 0;
};

class PrimitiveIndexFunctor
{
public:

    virtual ~PrimitiveIndexFunctor() {}

    virtual void setVertexArray(unsigned int count,const Vec2* vertices) = 0;
    virtual void setVertexArray(unsigned int count,const Vec3* vertices) = 0;
    virtual void setVertexArray(unsigned int count,const Vec4* vertices) = 0;

    virtual void setVertexArray(unsigned int count,const Vec2d* vertices) = 0;
    virtual void setVertexArray(unsigned int count,const Vec3d* vertices) = 0;
    virtual void setVertexArray(unsigned int count,const Vec4d* vertices) = 0;

    virtual void drawArrays(GLenum mode,GLint first,GLsizei count) = 0;
    virtual void drawElements(GLenum mode,GLsizei count,const GLubyte* indices) = 0;
    virtual void drawElements(GLenum mode,GLsizei count,const GLushort* indices) = 0;
    virtual void drawElements(GLenum mode,GLsizei count,const GLuint* indices) = 0;

    virtual void begin(GLenum mode) = 0;
    virtual void vertex(unsigned int pos) = 0;
    virtual void end() = 0;
};

class DrawElements;

class PrimitiveSet //: public BufferData
{
    public:
    
        enum Type
        {
            PrimitiveType,
            DrawArraysPrimitiveType,
            DrawArrayLengthsPrimitiveType,
            DrawElementsUBytePrimitiveType,
            DrawElementsUShortPrimitiveType,
            DrawElementsUIntPrimitiveType
        };

        enum Mode
        {
            POINTS = GL_POINTS,
            LINES = GL_LINES,
            LINE_STRIP = GL_LINE_STRIP,
            LINE_LOOP = GL_LINE_LOOP,
            TRIANGLES = GL_TRIANGLES,
            TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
            TRIANGLE_FAN = GL_TRIANGLE_FAN,
            QUADS = GL_QUADS,
            QUAD_STRIP = GL_QUAD_STRIP,
            POLYGON = GL_POLYGON,
            LINES_ADJACENCY = GL_LINES_ADJACENCY_EXT, 
            LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY_EXT, 
            TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY_EXT, 
            TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY_EXT,
            PATCHES = GL_PATCHES
        };

        PrimitiveSet(Type primType=PrimitiveType,GLenum mode=0, int numInstances=0):
            _primitiveType(primType),
            _numInstances(numInstances),
            _mode(mode) {}
    
        PrimitiveSet(const PrimitiveSet& prim):
            _primitiveType(prim._primitiveType),
            _numInstances(prim._numInstances),
            _mode(prim._mode) {}

        virtual const char* libraryName() const { return "osg"; }
        virtual const char* className() const { return "PrimitiveSet"; }
        
        Type                    getType() const { return _primitiveType; }
        virtual const GLvoid*   getDataPointer() const { return 0; }
        virtual unsigned int    getTotalDataSize() const { return 0; }
        virtual bool            supportsBufferObject() const { return false; }

        virtual DrawElements* getDrawElements() { return 0; }
        virtual const DrawElements* getDrawElements() const { return 0; }
            
        void setNumInstances(int n) { _numInstances = n; }
        int getNumInstances() const { return _numInstances; }

        void setMode(GLenum mode) { _mode = mode; }
        GLenum getMode() const { return _mode; }

        
        virtual void accept(PrimitiveFunctor& functor) const = 0;
        virtual void accept(PrimitiveIndexFunctor& functor) const = 0;
        
        virtual unsigned int index(unsigned int pos) const = 0;
        virtual unsigned int getNumIndices() const = 0;
        virtual void offsetIndices(int offset) = 0;

        virtual unsigned int getNumPrimitives() const; 

        virtual void computeRange() const {}

    protected:

        virtual ~PrimitiveSet() {}

        Type            _primitiveType;
        int             _numInstances;
        GLenum          _mode;
};

class DrawArrays : public PrimitiveSet
{
    public:

        DrawArrays(GLenum mode=0):
            PrimitiveSet(DrawArraysPrimitiveType,mode),
            _first(0),
            _count(0) {}
    
        DrawArrays(GLenum mode, GLint first, GLsizei count, int numInstances=0):
            PrimitiveSet(DrawArraysPrimitiveType, mode, numInstances),
            _first(first),
            _count(count) {}

        DrawArrays(const DrawArrays& da):
            PrimitiveSet(da),
            _first(da._first),
            _count(da._count) {}

        virtual const char* libraryName() const { return "osg"; }
        virtual const char* className() const { return "DrawArrays"; }
        

        void set(GLenum mode,GLint first, GLsizei count)
        {
            _mode = mode;
            _first = first;
            _count = count;
        }

        void setFirst(GLint first) { _first = first; }
        GLint getFirst() const { return _first; }
        
        void setCount(GLsizei count) { _count = count; }
        GLsizei getCount() const { return _count; }

        
        virtual void accept(PrimitiveFunctor& functor) const;
        virtual void accept(PrimitiveIndexFunctor& functor) const;
        
        virtual unsigned int getNumIndices() const { return static_cast<unsigned int>(_count); }
        virtual unsigned int index(unsigned int pos) const { return static_cast<unsigned int>(_first)+pos; }
        virtual void offsetIndices(int offset) { _first += offset; }

    protected:

        virtual ~DrawArrays() {}

        GLint   _first;
        GLsizei _count;
};

class DrawArrayLengths : public PrimitiveSet, public VectorGLsizei
{
    public:

        typedef VectorGLsizei vector_type;

        DrawArrayLengths(GLenum mode=0):
            PrimitiveSet(DrawArrayLengthsPrimitiveType,mode),
            _first(0) {}
    
        DrawArrayLengths(const DrawArrayLengths& dal):
            PrimitiveSet(dal),
            vector_type(dal),
            _first(dal._first) {}

        DrawArrayLengths(GLenum mode, GLint first, unsigned int no, GLsizei* ptr) : 
            PrimitiveSet(DrawArrayLengthsPrimitiveType,mode),
            vector_type(ptr,ptr+no),
            _first(first) {}

        DrawArrayLengths(GLenum mode,GLint first, unsigned int no) : 
            PrimitiveSet(DrawArrayLengthsPrimitiveType,mode),
            vector_type(no),
            _first(first) {}

        DrawArrayLengths(GLenum mode,GLint first) : 
            PrimitiveSet(DrawArrayLengthsPrimitiveType,mode),
            vector_type(),
            _first(first) {}


        virtual const char* libraryName() const { return "osg"; }
        virtual const char* className() const { return "DrawArrayLengths"; }
        

        void setFirst(GLint first) { _first = first; }
        GLint getFirst() const { return _first; }
        
        
        virtual void accept(PrimitiveFunctor& functor) const;
        virtual void accept(PrimitiveIndexFunctor& functor) const;
        
        virtual unsigned int getNumIndices() const;
        virtual unsigned int index(unsigned int pos) const { return _first+pos; }
        virtual void offsetIndices(int offset) { _first += offset; }

        virtual unsigned int getNumPrimitives() const;

    protected:

        virtual ~DrawArrayLengths() {}

        GLint   _first;
};

class DrawElements : public PrimitiveSet
{
    public:
        
        DrawElements(Type primType=PrimitiveType, GLenum mode=0, int numInstances=0):
            PrimitiveSet(primType,mode, numInstances) {}
    
        DrawElements(const DrawElements& copy):
            PrimitiveSet(copy) {}


        virtual DrawElements* getDrawElements() { return this; }
        virtual const DrawElements* getDrawElements() const { return this; }

        virtual void reserveElements(unsigned int numIndices) = 0;
        virtual void setElement(unsigned int, unsigned int) = 0;
        virtual unsigned int getElement(unsigned int) = 0;
        virtual void addElement(unsigned int) = 0;

    protected:

        virtual ~DrawElements() {}
};

class DrawElementsUByte : public DrawElements, public VectorGLubyte
{
    public:
    
        typedef VectorGLubyte vector_type;

        DrawElementsUByte(GLenum mode=0):
            DrawElements(DrawElementsUBytePrimitiveType,mode) {}
    
        DrawElementsUByte(const DrawElementsUByte& array):
            DrawElements(array),
            vector_type(array) {}

        /**
         * \param no Number of intended elements. This will be the size of the underlying vector.
         */
        DrawElementsUByte(GLenum mode, unsigned int no, const GLubyte* ptr, int numInstances=0) : 
            DrawElements(DrawElementsUBytePrimitiveType,mode,numInstances),
            vector_type(ptr,ptr+no) {}

        /**
         * \param no Number of intended elements. This will be the size of the underlying vector.
         */
        DrawElementsUByte(GLenum mode, unsigned int no) : 
            DrawElements(DrawElementsUBytePrimitiveType,mode),
            vector_type(no) {}

        virtual const char* libraryName() const { return "osg"; }
        virtual const char* className() const { return "DrawElementsUByte"; }

        virtual const GLvoid*   getDataPointer() const { return empty()?0:&front(); }
        virtual unsigned int    getTotalDataSize() const { return static_cast<unsigned int>(size()); }
        virtual bool            supportsBufferObject() const { return false; }

        
        virtual void accept(PrimitiveFunctor& functor) const;
        virtual void accept(PrimitiveIndexFunctor& functor) const;

        virtual unsigned int getNumIndices() const { return static_cast<unsigned int>(size()); }
        virtual unsigned int index(unsigned int pos) const { return (*this)[pos]; }
        virtual void offsetIndices(int offset);
        
        virtual void reserveElements(unsigned int numIndices) { reserve(numIndices); }
        virtual void setElement(unsigned int i, unsigned int v)  { (*this)[i] = v; }
        virtual unsigned int getElement(unsigned int i) { return (*this)[i]; } 
        virtual void addElement(unsigned int v) { push_back(GLubyte(v)); } 

    protected:

        virtual ~DrawElementsUByte();
};


class DrawElementsUShort : public DrawElements, public VectorGLushort
{
    public:

        typedef VectorGLushort vector_type;

        DrawElementsUShort(GLenum mode=0):
            DrawElements(DrawElementsUShortPrimitiveType,mode) {}
    
        DrawElementsUShort(const DrawElementsUShort& array):
            DrawElements(array),
            vector_type(array) {}

        /**
         * \param no Number of intended elements. This will be the size of the underlying vector.
         */
        DrawElementsUShort(GLenum mode, unsigned int no, const GLushort* ptr, int numInstances=0) : 
            DrawElements(DrawElementsUShortPrimitiveType,mode,numInstances),
            vector_type(ptr,ptr+no) {}

        /**
         * \param no Number of intended elements. This will be the size of the underlying vector.
         */
        DrawElementsUShort(GLenum mode, unsigned int no) : 
            DrawElements(DrawElementsUShortPrimitiveType,mode),
            vector_type(no) {}

        template <class InputIterator>
        DrawElementsUShort(GLenum mode, InputIterator first,InputIterator last) : 
            DrawElements(DrawElementsUShortPrimitiveType,mode),
            vector_type(first,last) {}

        virtual const char* libraryName() const { return "osg"; }
        virtual const char* className() const { return "DrawElementsUShort"; }

        virtual const GLvoid*   getDataPointer() const { return empty()?0:&front(); }
        virtual unsigned int    getTotalDataSize() const { return 2u*static_cast<unsigned int>(size()); }
        virtual bool            supportsBufferObject() const { return false; }

        
        virtual void accept(PrimitiveFunctor& functor) const;
        virtual void accept(PrimitiveIndexFunctor& functor) const;

        virtual unsigned int getNumIndices() const { return static_cast<unsigned int>(size()); }
        virtual unsigned int index(unsigned int pos) const { return (*this)[pos]; }
        virtual void offsetIndices(int offset);

        virtual void reserveElements(unsigned int numIndices) { reserve(numIndices); }
        virtual void setElement(unsigned int i, unsigned int v) { (*this)[i] = v; }
        virtual unsigned int getElement(unsigned int i) { return (*this)[i]; }
        virtual void addElement(unsigned int v) { push_back(GLushort(v)); }

    protected:

        virtual ~DrawElementsUShort();
};

class DrawElementsUInt : public DrawElements, public VectorGLuint
{
    public:

        typedef VectorGLuint vector_type;

        DrawElementsUInt(GLenum mode=0):
            DrawElements(DrawElementsUIntPrimitiveType,mode) {}
    
        DrawElementsUInt(const DrawElementsUInt& array):
            DrawElements(array),
            vector_type(array) {}

        /**
         * \param mode One of osg::PrimitiveSet::Mode. Determines the type of primitives used.
         * \param no Number of intended elements. This will be the size of the underlying vector.
         */
        DrawElementsUInt(GLenum mode, unsigned int no, const GLuint* ptr, int numInstances=0) : 
            DrawElements(DrawElementsUIntPrimitiveType,mode,numInstances),
            vector_type(ptr,ptr+no) {}

        /**
         * \param no Number of intended elements. This will be the size of the underlying vector.
         */
        DrawElementsUInt(GLenum mode, unsigned int no) : 
            DrawElements(DrawElementsUIntPrimitiveType,mode),
            vector_type(no) {}

        template <class InputIterator>
        DrawElementsUInt(GLenum mode, InputIterator first,InputIterator last) : 
            DrawElements(DrawElementsUIntPrimitiveType,mode),
            vector_type(first,last) {}

        virtual const char* libraryName() const { return "osg"; }
        virtual const char* className() const { return "DrawElementsUInt"; }

        virtual const GLvoid*   getDataPointer() const { return empty()?0:&front(); }
        virtual unsigned int    getTotalDataSize() const { return 4u*static_cast<unsigned int>(size()); }
        virtual bool            supportsBufferObject() const { return false; }

        
        virtual void accept(PrimitiveFunctor& functor) const;
        virtual void accept(PrimitiveIndexFunctor& functor) const;

        virtual unsigned int getNumIndices() const { return static_cast<unsigned int>(size()); }
        virtual unsigned int index(unsigned int pos) const { return (*this)[pos]; }
        virtual void offsetIndices(int offset);

        virtual void reserveElements(unsigned int numIndices) { reserve(numIndices); }
        virtual void setElement(unsigned int i, unsigned int v) { (*this)[i] = v; }
        virtual unsigned int getElement(unsigned int i) { return (*this)[i]; }
        virtual void addElement(unsigned int v) { push_back(GLuint(v)); }

    protected:

        virtual ~DrawElementsUInt();
};

}

#endif
