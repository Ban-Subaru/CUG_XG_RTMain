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

#ifndef OSG_ARRAY
#define OSG_ARRAY 1

#include "MixinVector.h"
#include "config.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Vec2d.h"
#include "Vec3d.h"
#include "Vec4d.h"
#include "Vec4ub.h"
#include "Vec2s.h"
#include "Vec3s.h"
#include "Vec4s.h"
#include "Vec2b.h"
#include "Vec3b.h"
#include "Vec4b.h"


namespace osg {




#if defined(OSG_GLES1_AVAILABLE)

#ifdef __APPLE__
	//if its apple include the target defines so we can check for IOS
#include "TargetConditionals.h"
#include <OpenGLES/ES1/gl.h>
#else
#include <GLES/gl.h>
#endif

#elif defined(OSG_GLES2_AVAILABLE)

#ifdef __APPLE__
	//if its apple include the target defines so we can check for IOS
#include "TargetConditionals.h"
#include <OpenGLES/ES2/gl.h>
#else
#include <GLES2/gl2.h>
#endif


#else

#ifndef WIN32

	// Required for compatibility with glext.h sytle function definitions of 
	// OpenGL extensions, such as in src/osg/Point.cpp.
#ifndef APIENTRY
#define APIENTRY
#endif

#else // WIN32

#if defined(__CYGWIN__) || defined(__MINGW32__)

#ifndef APIENTRY
#define GLUT_APIENTRY_DEFINED
#define APIENTRY __stdcall
#endif
	// XXX This is from Win32's <windef.h> 
#ifndef CALLBACK
#define CALLBACK __stdcall
#endif

#else // ! __CYGWIN__

	// Under Windows avoid including <windows.h>
	// to avoid name space pollution, but Win32's <GL/gl.h> 
	// needs APIENTRY and WINGDIAPI defined properly. 
	// XXX This is from Win32's <windef.h> 
#ifndef APIENTRY
#define GLUT_APIENTRY_DEFINED
#if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define WINAPI __stdcall
#define APIENTRY WINAPI
#else
#define APIENTRY
#endif
#endif

	// XXX This is from Win32's <windef.h> 
#ifndef CALLBACK
#if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif
#endif

#endif // __CYGWIN__

	// XXX This is from Win32's <wingdi.h> and <winnt.h> 
#ifndef WINGDIAPI
#define GLUT_WINGDIAPI_DEFINED
#define DECLSPEC_IMPORT __declspec(dllimport)
#define WINGDIAPI DECLSPEC_IMPORT
#endif

	// XXX This is from Win32's <ctype.h>
#if !defined(_WCHAR_T_DEFINED) && !(defined(__GNUC__)&&((__GNUC__ == 3)||(__GNUC__ == 4)))
	typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#endif // WIN32

#if defined(OSG_GL3_AVAILABLE)

#define GL3_PROTOTYPES 1
#include <GL3/gl3.h>

#else
#ifndef __gl_h_
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif
#endif

#ifndef GL_APIENTRY
#define GL_APIENTRY APIENTRY
#endif // GL_APIENTRY

#endif






	/*//从新定义OpenGL中的该变量
	typedef int GLint;
	typedef unsigned int GLenum;
	typedef void GLvoid;
	typedef unsigned int GLuint;
	typedef signed char GLbyte;
	typedef short GLshort;
	typedef unsigned char GLubyte;
	typedef int GLsizei;
	typedef unsigned short GLushort;
	typedef float GLfloat;
	typedef double GLdouble;
	typedef unsigned char GLboolean;


	// BeginMode 
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007
#define GL_QUAD_STRIP                     0x0008
#define GL_POLYGON                        0x0009


	#define GL_TRUE                           1
	#define GL_FALSE                          0

	#define GL_BYTE                           0x1400
	#define GL_UNSIGNED_BYTE                  0x1401
	#define GL_SHORT                          0x1402
	#define GL_UNSIGNED_SHORT                 0x1403
	#define GL_INT                            0x1404
	#define GL_UNSIGNED_INT                   0x1405
	#define GL_FLOAT                          0x1406
	#define GL_2_BYTES                        0x1407
	#define GL_3_BYTES                        0x1408
	#define GL_4_BYTES                        0x1409
	#define GL_DOUBLE                         0x140A*/

class ArrayVisitor;
class ConstArrayVisitor;

class ValueVisitor;
class ConstValueVisitor;

class Array //--: public BufferData
{

    public:
    
        enum Type
        {
            ArrayType = 0,
            ByteArrayType     = 1,
            ShortArrayType    = 2,
            IntArrayType      = 3,
            UByteArrayType    = 4,
            UShortArrayType   = 5,
            UIntArrayType     = 6,
            Vec4ubArrayType   = 7,
            FloatArrayType    = 8,
            Vec2ArrayType     = 9,
            Vec3ArrayType     = 10,
            Vec4ArrayType     = 11,
            Vec2sArrayType    = 12,
            Vec3sArrayType    = 13,
            Vec4sArrayType    = 14,
            Vec2bArrayType    = 15,
            Vec3bArrayType    = 16,
            Vec4bArrayType    = 17,
            DoubleArrayType   = 18,
            Vec2dArrayType    = 19,
            Vec3dArrayType    = 20,
            Vec4dArrayType    = 21,
            //--MatrixArrayType   = 22            
        };

        Array(Type arrayType=ArrayType,GLint dataSize=0,GLenum dataType=0):
            _arrayType(arrayType),
            _dataSize(dataSize),
            _dataType(dataType) {}

        /*Array(const Array& array,const CopyOp& copyop=CopyOp::SHALLOW_COPY):
            BufferData(array,copyop),
            _arrayType(array._arrayType),
            _dataSize(array._dataSize),
            _dataType(array._dataType) {}*/

        //--virtual bool isSameKindAs(const Object* obj) const { return dynamic_cast<const Array*>(obj)!=NULL; }
        virtual const char* libraryName() const { return "osg"; }
        //--virtual const char* className() const;

        virtual void accept(ArrayVisitor&) = 0;
        virtual void accept(ConstArrayVisitor&) const = 0;

        virtual void accept(unsigned int index,ValueVisitor&) = 0;
        virtual void accept(unsigned int index,ConstValueVisitor&) const = 0;

        /** Return -1 if lhs element is less than rhs element, 0 if equal,
          * 1 if lhs element is greater than rhs element. */
        virtual int compare(unsigned int lhs,unsigned int rhs) const = 0;

        Type                    getType() const { return _arrayType; }
        GLint                   getDataSize() const { return _dataSize; }
        GLenum                  getDataType() const { return _dataType; }
        virtual const GLvoid*   getDataPointer() const = 0;
        virtual unsigned int    getTotalDataSize() const = 0;
        virtual unsigned int    getNumElements() const = 0;

        /** Frees unused space on this vector - i.e. the difference between size() and max_size() of the underlying vector.*/
        virtual void trim() {}

        /** Set the VertexBufferObject.*/
        //--inline void setVertexBufferObject(osg::VertexBufferObject* vbo) { setBufferObject(vbo); }

        /** Get the VertexBufferObject. If no VBO is assigned returns NULL*/
        //--inline osg::VertexBufferObject* getVertexBufferObject() { return dynamic_cast<osg::VertexBufferObject*>(_bufferObject.get()); }

        /** Get the const VertexBufferObject. If no VBO is assigned returns NULL*/
        //--inline const osg::VertexBufferObject* getVertexBufferObject() const { return dynamic_cast<const osg::VertexBufferObject*>(_bufferObject.get());  }

    protected:

        virtual ~Array() {}

        Type                                    _arrayType;
        GLint                                   _dataSize;
        GLenum                                  _dataType;
};

template<typename T, Array::Type ARRAYTYPE, int DataSize, int DataType>
class TemplateArray : public Array, public MixinVector<T>
{
    public:

        TemplateArray() : Array(ARRAYTYPE,DataSize,DataType) {}

        /*TemplateArray(const TemplateArray& ta,const CopyOp& copyop=CopyOp::SHALLOW_COPY):
            Array(ta,copyop),
            MixinVector<T>(ta) {}*/

        TemplateArray(unsigned int no) : 
            Array(ARRAYTYPE,DataSize,DataType),
            MixinVector<T>(no) {}

        TemplateArray(unsigned int no,const T* ptr) : 
            Array(ARRAYTYPE,DataSize,DataType),
            MixinVector<T>(ptr,ptr+no) {}

        template <class InputIterator>
        TemplateArray(InputIterator first,InputIterator last) : 
            Array(ARRAYTYPE,DataSize,DataType),
            MixinVector<T>(first,last) {}        

        TemplateArray& operator = (const TemplateArray& array)
        {
            if (this==&array) return *this;
            this->assign(array.begin(),array.end());
            return *this;
        } 

        //--virtual Object* cloneType() const { return new TemplateArray(); }
        //--virtual Object* clone(const CopyOp& copyop) const { return new TemplateArray(*this,copyop); }        

        inline virtual void accept(ArrayVisitor& av);
        inline virtual void accept(ConstArrayVisitor& av) const;

        inline virtual void accept(unsigned int index,ValueVisitor& vv);
        inline virtual void accept(unsigned int index,ConstValueVisitor& vv) const;

        virtual int compare(unsigned int lhs,unsigned int rhs) const
        {
            const T& elem_lhs = (*this)[lhs];
            const T& elem_rhs = (*this)[rhs];
            if (elem_lhs<elem_rhs) return -1;
            if (elem_rhs<elem_lhs) return 1;
            return 0;
        }

        /** Frees unused space on this vector - i.e. the difference between size() and max_size() of the underlying vector.*/
        virtual void trim()
        {
            MixinVector<T>( *this ).swap( *this );
        }

        virtual const GLvoid*   getDataPointer() const { if (!this->empty()) return &this->front(); else return 0; }
        virtual unsigned int    getTotalDataSize() const { return static_cast<unsigned int>(this->size()*sizeof(T)); }
        virtual unsigned int    getNumElements() const { return static_cast<unsigned int>(this->size()); }

        typedef T ElementDataType; // expose T

    protected:
    
        virtual ~TemplateArray() {}
};

class IndexArray : public Array
{

    public:

        IndexArray(Type arrayType=ArrayType,GLint dataSize=0,GLenum dataType=0):
            Array(arrayType,dataSize,dataType) {}

        /*IndexArray(const Array& array,const CopyOp& copyop=CopyOp::SHALLOW_COPY):
            Array(array,copyop) {}*/

        //--virtual bool isSameKindAs(const Object* obj) const { return dynamic_cast<const IndexArray*>(obj)!=NULL; }

        virtual unsigned int index(unsigned int pos) const = 0;

    protected:

        virtual ~IndexArray() {}
};

template<typename T, Array::Type ARRAYTYPE, int DataSize, int DataType>
class TemplateIndexArray : public IndexArray, public MixinVector<T>
{
    public:

        TemplateIndexArray() : IndexArray(ARRAYTYPE,DataSize,DataType) {}

        /*TemplateIndexArray(const TemplateIndexArray& ta,const CopyOp& copyop=CopyOp::SHALLOW_COPY):
            IndexArray(ta,copyop),
            MixinVector<T>(ta) {}*/

        TemplateIndexArray(unsigned int no) : 
            IndexArray(ARRAYTYPE,DataSize,DataType),
            MixinVector<T>(no) {}

        TemplateIndexArray(unsigned int no,T* ptr) : 
            IndexArray(ARRAYTYPE,DataSize,DataType),
            MixinVector<T>(ptr,ptr+no) {}

        template <class InputIterator>
        TemplateIndexArray(InputIterator first,InputIterator last) : 
            IndexArray(ARRAYTYPE,DataSize,DataType),
            MixinVector<T>(first,last) {}        

        TemplateIndexArray& operator = (const TemplateIndexArray& array)
        {
            if (this==&array) return *this;
            this->assign(array.begin(),array.end());
            return *this;
        } 

        //--virtual Object* cloneType() const { return new TemplateIndexArray(); }
        //--virtual Object* clone(const CopyOp& copyop) const { return new TemplateIndexArray(*this,copyop); }        

        inline virtual void accept(ArrayVisitor& av);
        inline virtual void accept(ConstArrayVisitor& av) const;

        inline virtual void accept(unsigned int index,ValueVisitor& vv);
        inline virtual void accept(unsigned int index,ConstValueVisitor& vv) const;

        virtual int compare(unsigned int lhs,unsigned int rhs) const
        {
            const T& elem_lhs = (*this)[lhs];
            const T& elem_rhs = (*this)[rhs];
            if (elem_lhs<elem_rhs) return -1;
            if (elem_rhs<elem_lhs) return 1;
            return 0;
        }

        /** Frees unused space on this vector - i.e. the difference between size() and max_size() of the underlying vector.*/
        virtual void trim()
        {
            MixinVector<T>( *this ).swap( *this );
        }

        virtual const GLvoid*   getDataPointer() const { if (!this->empty()) return &this->front(); else return 0; }
        virtual unsigned int    getTotalDataSize() const { return static_cast<unsigned int>(this->size()*sizeof(T)); }
        virtual unsigned int    getNumElements() const { return static_cast<unsigned int>(this->size()); }

        virtual unsigned int    index(unsigned int pos) const { return (*this)[pos]; }

        typedef T ElementDataType; // expose T

    protected:

        virtual ~TemplateIndexArray() {}
};

typedef TemplateIndexArray<GLbyte,Array::ByteArrayType,1,GL_BYTE>               ByteArray;
typedef TemplateIndexArray<GLshort,Array::ShortArrayType,1,GL_SHORT>            ShortArray;
typedef TemplateIndexArray<GLint,Array::IntArrayType,1,GL_INT>                  IntArray;
typedef TemplateIndexArray<GLubyte,Array::UByteArrayType,1,GL_UNSIGNED_BYTE>    UByteArray;
typedef TemplateIndexArray<GLushort,Array::UShortArrayType,1,GL_UNSIGNED_SHORT> UShortArray;
typedef TemplateIndexArray<GLuint,Array::UIntArrayType,1,GL_UNSIGNED_INT>       UIntArray;

typedef TemplateArray<GLfloat,Array::FloatArrayType,1,GL_FLOAT>                 FloatArray;

typedef TemplateArray<Vec2,Array::Vec2ArrayType,2,GL_FLOAT>                     Vec2Array;
typedef TemplateArray<Vec3,Array::Vec3ArrayType,3,GL_FLOAT>                     Vec3Array;
typedef TemplateArray<Vec4,Array::Vec4ArrayType,4,GL_FLOAT>                     Vec4Array;

typedef TemplateArray<Vec4ub,Array::Vec4ubArrayType,4,GL_UNSIGNED_BYTE>         Vec4ubArray;

typedef TemplateArray<Vec2s,Array::Vec2sArrayType,2,GL_SHORT>                   Vec2sArray;
typedef TemplateArray<Vec3s,Array::Vec3sArrayType,3,GL_SHORT>                   Vec3sArray;
typedef TemplateArray<Vec4s,Array::Vec4sArrayType,4,GL_SHORT>                   Vec4sArray;

typedef TemplateArray<Vec2b,Array::Vec2bArrayType,2,GL_BYTE>                    Vec2bArray;
typedef TemplateArray<Vec3b,Array::Vec3bArrayType,3,GL_BYTE>                    Vec3bArray;
typedef TemplateArray<Vec4b,Array::Vec4bArrayType,4,GL_BYTE>                    Vec4bArray;

typedef TemplateArray<GLdouble,Array::DoubleArrayType,1,GL_DOUBLE>               DoubleArray;
typedef TemplateArray<Vec2d,Array::Vec2dArrayType,2,GL_DOUBLE>                   Vec2dArray;
typedef TemplateArray<Vec3d,Array::Vec3dArrayType,3,GL_DOUBLE>                   Vec3dArray;
typedef TemplateArray<Vec4d,Array::Vec4dArrayType,4,GL_DOUBLE>                   Vec4dArray;

//--typedef TemplateArray<Matrixf,Array::MatrixArrayType,16,GL_FLOAT>                MatrixfArray;


class ArrayVisitor
{
    public:
        ArrayVisitor() {}
        virtual ~ArrayVisitor() {}
        
        virtual void apply(Array&) {}
        virtual void apply(ByteArray&) {}
        virtual void apply(ShortArray&) {}
        virtual void apply(IntArray&) {}
        virtual void apply(UByteArray&) {}
        virtual void apply(UShortArray&) {}
        virtual void apply(UIntArray&) {}
        virtual void apply(FloatArray&) {}
        virtual void apply(DoubleArray&) {}

        virtual void apply(Vec2Array&) {}
        virtual void apply(Vec3Array&) {}
        virtual void apply(Vec4Array&) {}
        
        virtual void apply(Vec4ubArray&) {}

        virtual void apply(Vec2bArray&) {}        
        virtual void apply(Vec3bArray&) {}        
        virtual void apply(Vec4bArray&) {}        

        virtual void apply(Vec2sArray&) {}
        virtual void apply(Vec3sArray&) {}
        virtual void apply(Vec4sArray&) {}

        virtual void apply(Vec2dArray&) {}
        virtual void apply(Vec3dArray&) {}
        virtual void apply(Vec4dArray&) {}
        
        //--virtual void apply(MatrixfArray&) {}
};

class ConstArrayVisitor
{
    public:
        ConstArrayVisitor() {}
        virtual ~ConstArrayVisitor() {}
        
        virtual void apply(const Array&) {}
        virtual void apply(const ByteArray&) {}
        virtual void apply(const ShortArray&) {}
        virtual void apply(const IntArray&) {}
        virtual void apply(const UByteArray&) {}
        virtual void apply(const UShortArray&) {}
        virtual void apply(const UIntArray&) {}
        virtual void apply(const FloatArray&) {}
        virtual void apply(const DoubleArray&) {}

        virtual void apply(const Vec2Array&) {}
        virtual void apply(const Vec3Array&) {}
        virtual void apply(const Vec4Array&) {}

        virtual void apply(const Vec4ubArray&) {}

        virtual void apply(const Vec2bArray&) {}        
        virtual void apply(const Vec3bArray&) {}        
        virtual void apply(const Vec4bArray&) {}        

        virtual void apply(const Vec2sArray&) {}
        virtual void apply(const Vec3sArray&) {}
        virtual void apply(const Vec4sArray&) {}

        virtual void apply(const Vec2dArray&) {}
        virtual void apply(const Vec3dArray&) {}
        virtual void apply(const Vec4dArray&) {}
        
        //--virtual void apply(const MatrixfArray&) {}
};


class ValueVisitor
{
    public:
        ValueVisitor() {}
        virtual ~ValueVisitor() {}
        
        virtual void apply(GLbyte&) {}
        virtual void apply(GLshort&) {}
        virtual void apply(GLint&) {}
        virtual void apply(GLushort&) {}
        virtual void apply(GLubyte&) {}
        virtual void apply(GLuint&) {}
        virtual void apply(GLfloat&) {}
        virtual void apply(GLdouble&) {}
        
        
        virtual void apply(Vec2&) {}
        virtual void apply(Vec3&) {}
        virtual void apply(Vec4&) {}
        
        virtual void apply(Vec4ub&) {}

        virtual void apply(Vec2b&) {}        
        virtual void apply(Vec3b&) {}        
        virtual void apply(Vec4b&) {}        

        virtual void apply(Vec2s&) {}
        virtual void apply(Vec3s&) {}
        virtual void apply(Vec4s&) {}

        virtual void apply(Vec2d&) {}
        virtual void apply(Vec3d&) {}
        virtual void apply(Vec4d&) {}
        
        //--virtual void apply(Matrixf&) {}
};

class ConstValueVisitor
{
    public:
        ConstValueVisitor() {}
        virtual ~ConstValueVisitor() {}
        
        virtual void apply(const GLbyte&) {}
        virtual void apply(const GLshort&) {}
        virtual void apply(const GLint&) {}
        virtual void apply(const GLushort&) {}
        virtual void apply(const GLubyte&) {}
        virtual void apply(const GLuint&) {}
        virtual void apply(const GLfloat&) {}
        virtual void apply(const GLdouble&) {}

        virtual void apply(const Vec4ub&) {}

        virtual void apply(const Vec2&) {}
        virtual void apply(const Vec3&) {}
        virtual void apply(const Vec4&) {}

        virtual void apply(const Vec2b&) {}
        virtual void apply(const Vec3b&) {}
        virtual void apply(const Vec4b&) {}        
        
        virtual void apply(const Vec2s&) {}
        virtual void apply(const Vec3s&) {}
        virtual void apply(const Vec4s&) {}
        
        virtual void apply(const Vec2d&) {}
        virtual void apply(const Vec3d&) {}
        virtual void apply(const Vec4d&) {}
        
        //--virtual void apply(const Matrixf&) {}
};

template<typename T, Array::Type ARRAYTYPE, int DataSize, int DataType>
inline void TemplateArray<T,ARRAYTYPE,DataSize,DataType>::accept(ArrayVisitor& av) { av.apply(*this); }

template<typename T, Array::Type ARRAYTYPE, int DataSize, int DataType>
inline void TemplateArray<T,ARRAYTYPE,DataSize,DataType>::accept(ConstArrayVisitor& av) const { av.apply(*this); }

template<typename T, Array::Type ARRAYTYPE, int DataSize, int DataType>
inline void TemplateArray<T,ARRAYTYPE,DataSize,DataType>::accept(unsigned int index,ValueVisitor& vv) { vv.apply( (*this)[index] ); }

template<typename T, Array::Type ARRAYTYPE, int DataSize, int DataType>
inline void TemplateArray<T,ARRAYTYPE,DataSize,DataType>::accept(unsigned int index,ConstValueVisitor& vv) const {  vv.apply( (*this)[index] );}

template<typename T, Array::Type ARRAYTYPE, int DataSize, int DataType>
inline void TemplateIndexArray<T,ARRAYTYPE,DataSize,DataType>::accept(ArrayVisitor& av) { av.apply(*this); }

template<typename T, Array::Type ARRAYTYPE, int DataSize, int DataType>
inline void TemplateIndexArray<T,ARRAYTYPE,DataSize,DataType>::accept(ConstArrayVisitor& av) const { av.apply(*this); }

template<typename T, Array::Type ARRAYTYPE, int DataSize, int DataType>
inline void TemplateIndexArray<T,ARRAYTYPE,DataSize,DataType>::accept(unsigned int index,ValueVisitor& vv) { vv.apply( (*this)[index] ); }

template<typename T, Array::Type ARRAYTYPE, int DataSize, int DataType>
inline void TemplateIndexArray<T,ARRAYTYPE,DataSize,DataType>::accept(unsigned int index,ConstValueVisitor& vv) const {  vv.apply( (*this)[index] );}

}

#endif
