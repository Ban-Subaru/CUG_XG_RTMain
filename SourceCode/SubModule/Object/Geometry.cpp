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
#include <stdlib.h>

#include "Geometry.h"

using namespace osg;

const Geometry::ArrayData Geometry::s_InvalidArrayData;

Geometry::ArrayData::ArrayData(const ArrayData& data):
    array(data.array),
    indices(dynamic_cast<osg::IndexArray*>(data.indices)),
    binding(data.binding),
    normalize(data.normalize)
{
}

Geometry::Vec3ArrayData::Vec3ArrayData(const Vec3ArrayData& data):
    array(dynamic_cast<osg::Vec3Array*>(data.array)),
    indices(dynamic_cast<osg::IndexArray*>(data.indices)),
    binding(data.binding),
    normalize(data.normalize)
{
}

Geometry::Geometry()
{
    // temporary test
    // setSupportsDisplayList(false);

    _vertexData.binding = BIND_PER_VERTEX;

    _fastPath = false;
    _fastPathHint = true;
}

Geometry::Geometry(const Geometry& geometry):
    _vertexData(geometry._vertexData),
    _normalData(geometry._normalData),
    _colorData(geometry._colorData),
    _secondaryColorData(geometry._secondaryColorData),
    _fogCoordData(geometry._fogCoordData),
    _fastPath(geometry._fastPath),
    _fastPathHint(geometry._fastPathHint)
{
    // temporary test

    for(ArrayDataList::const_iterator titr=geometry._texCoordList.begin();
        titr!=geometry._texCoordList.end();
        ++titr)
    {
        _texCoordList.push_back(ArrayData(*titr));
    }

    for(ArrayDataList::const_iterator vitr=geometry._vertexAttribList.begin();
        vitr!=geometry._vertexAttribList.end();
        ++vitr)
    {
        _vertexAttribList.push_back(ArrayData(*vitr));
    }
}

Geometry::~Geometry()
{
    // do dirty here to keep the getGLObjectSizeHint() estimate on the ball
    //--dirtyDisplayList();
    
    // no need to delete, all automatically handled by ref_ptr :-)
}

bool Geometry::empty() const
{
    if (!_primitives.empty()) return false;
    if (!_vertexData.empty()) return false;
    if (!_normalData.empty()) return false;
    if (!_colorData.empty()) return false;
    if (!_secondaryColorData.empty()) return false;
    if (!_fogCoordData.empty()) return false;
    if (!_texCoordList.empty()) return false;
    if (!_vertexAttribList.empty()) return false;
    return true;
}

void Geometry::setVertexArray(Array* array)
{
    _vertexData.array = array; 
}

Array* Geometry::getTexCoordArray(unsigned int unit)
{
    if (unit<_texCoordList.size()) return _texCoordList[unit].array;
    else return 0;
}

const Array* Geometry::getTexCoordArray(unsigned int unit) const
{
    if (unit<_texCoordList.size()) return _texCoordList[unit].array;
    else return 0;
}

IndexArray* Geometry::getTexCoordIndices(unsigned int unit)
{
    if (unit<_texCoordList.size()) return _texCoordList[unit].indices;
    else return 0;
}

const IndexArray* Geometry::getTexCoordIndices(unsigned int unit) const
{
    if (unit<_texCoordList.size()) return _texCoordList[unit].indices;
    else return 0;
}

Geometry::ArrayData& Geometry::getVertexAttribData(unsigned int index)
{
    if (_vertexAttribList.size()<=index)
        _vertexAttribList.resize(index+1);
        
    return _vertexAttribList[index];
}

const Geometry::ArrayData& Geometry::getVertexAttribData(unsigned int index) const
{
    if (_vertexAttribList.size()<=_vertexAttribList.size())
        return s_InvalidArrayData;
        
    return _vertexAttribList[index];
}


GLboolean Geometry::getVertexAttribNormalize(unsigned int index) const
{
    if (index<_vertexAttribList.size()) return _vertexAttribList[index].normalize;
    else return GL_FALSE;
}

bool Geometry::addPrimitiveSet(PrimitiveSet* primitiveset)
{
    if (primitiveset)
    {
        _primitives.push_back(primitiveset);
        return true;
    }
    return false;
}

bool Geometry::removePrimitiveSet(unsigned int i, unsigned int numElementsToRemove)
{
    if (numElementsToRemove==0) return false;

    if (i<_primitives.size())
    {
        if (i+numElementsToRemove<=_primitives.size())
        {
            _primitives.erase(_primitives.begin()+i,_primitives.begin()+i+numElementsToRemove);
        }
        else
        {
            // asking to delete too many elements, report a warning, and delete to
            // the end of the primitive list.
            _primitives.erase(_primitives.begin()+i,_primitives.end());
        }
    
        return true;
    }
    return false;
}

unsigned int Geometry::getPrimitiveSetIndex(const PrimitiveSet* primitiveset) const
{
    for (unsigned int primitiveSetIndex=0;primitiveSetIndex<_primitives.size();++primitiveSetIndex)
    {
        if (_primitives[primitiveSetIndex]==primitiveset) return primitiveSetIndex;
    }
    return _primitives.size(); // node not found.
}

bool Geometry::computeFastPathsUsed()
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // check to see if fast path can be used.
    //
    _fastPath = true;
    if (_vertexData.indices) _fastPath = false;
    else if (_normalData.binding==BIND_PER_PRIMITIVE || (_normalData.binding==BIND_PER_VERTEX && _normalData.indices)) _fastPath = false;
    else if (_colorData.binding==BIND_PER_PRIMITIVE || (_colorData.binding==BIND_PER_VERTEX && _colorData.indices)) _fastPath = false;
    else if (_secondaryColorData.binding==BIND_PER_PRIMITIVE || (_secondaryColorData.binding==BIND_PER_VERTEX && _secondaryColorData.indices)) _fastPath = false;
    else if (_fogCoordData.binding==BIND_PER_PRIMITIVE || (_fogCoordData.binding==BIND_PER_VERTEX && _fogCoordData.indices)) _fastPath = false;
    else 
    {
        for( unsigned int va = 0; va < _vertexAttribList.size(); ++va )
        {
            if (_vertexAttribList[va].binding==BIND_PER_PRIMITIVE)
            {
                _fastPath = false;
                break;
            }
            else
            {
                const Array * array = _vertexAttribList[va].array;
                const IndexArray * idxArray = _vertexAttribList[va].indices;

                if( _vertexAttribList[va].binding==BIND_PER_VERTEX && 
                    array && array->getNumElements()>0 &&
                    idxArray && idxArray->getNumElements()>0 ) 
                {
                    _fastPath = false;
                    break;
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Set up tex coords if required.
    //
    for(unsigned int unit=0;unit<_texCoordList.size();++unit)
    {
        const ArrayData& texcoordData = _texCoordList[unit];
        if (texcoordData.array && texcoordData.array->getNumElements()>0)
        {
            if (texcoordData.indices)
            {
                if (texcoordData.indices->getNumElements()>0)
                {
                    _fastPath = false;         
                    break;
                }
            }
        }
    }
    

    return _fastPath;
}

unsigned int Geometry::getGLObjectSizeHint() const
{
    unsigned int totalSize = 0;
    if (_vertexData.array) totalSize += _vertexData.array->getTotalDataSize();

    if (_normalData.array) totalSize += _normalData.array->getTotalDataSize();

    if (_colorData.array) totalSize += _colorData.array->getTotalDataSize();

    if (_secondaryColorData.array) totalSize += _secondaryColorData.array->getTotalDataSize();

    if (_fogCoordData.array) totalSize += _fogCoordData.array->getTotalDataSize();


    unsigned int unit;
    for(unit=0;unit<_texCoordList.size();++unit)
    {
        const Array* array = _texCoordList[unit].array;
        if (array) totalSize += array->getTotalDataSize();

    }

    bool handleVertexAttributes = true;
    if (handleVertexAttributes)
    {
        unsigned int index;
        for( index = 0; index < _vertexAttribList.size(); ++index )
        {
            const Array* array = _vertexAttribList[index].array;
            if (array) totalSize += array->getTotalDataSize();           
        }
    }

    for(PrimitiveSetList::const_iterator itr=_primitives.begin();
        itr!=_primitives.end();
        ++itr)
    {

        totalSize += 4*(*itr)->getNumIndices();

    }


    // do a very simply mapping of display list size proportional to vertex datasize.
    return totalSize;
}

bool Geometry::getArrayList(ArrayList& arrayList) const
{
    unsigned int startSize = arrayList.size();
    
    if (_vertexData.array) arrayList.push_back(_vertexData.array);
    if (_normalData.array) arrayList.push_back(_normalData.array);
    if (_colorData.array) arrayList.push_back(_colorData.array);
    if (_secondaryColorData.array) arrayList.push_back(_secondaryColorData.array);
    if (_fogCoordData.array) arrayList.push_back(_fogCoordData.array);
    
    for(unsigned int unit=0;unit<_texCoordList.size();++unit)
    {
        Array* array = _texCoordList[unit].array;
        if (array) arrayList.push_back(array);
    }

    for(unsigned int  index = 0; index < _vertexAttribList.size(); ++index )
    {
        Array* array = _vertexAttribList[index].array;
        if (array) arrayList.push_back(array);
    }

    return arrayList.size()!=startSize;
}

bool Geometry::getDrawElementsList(DrawElementsList& drawElementsList) const
{
    unsigned int startSize = drawElementsList.size();
    
    for(PrimitiveSetList::const_iterator itr = _primitives.begin();
        itr != _primitives.end();
        ++itr)
    {
        osg::DrawElements* de = (*itr)->getDrawElements();
        if (de) drawElementsList.push_back(de);
    }
    
    return drawElementsList.size()!=startSize;
}



class CheckArrayValidity
{
public:
    CheckArrayValidity(const osg::Geometry* geometry)
    {
        numPrimitiveSets = geometry->getNumPrimitiveSets();
        primitiveNum = 0;
        maxVertexNumber = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // draw the primitives themselves.
        //
        for(unsigned int primitiveSetNum=0; primitiveSetNum != numPrimitiveSets; ++primitiveSetNum)
        {
            const PrimitiveSet* primitiveset = geometry->getPrimitiveSet(primitiveSetNum);

            GLenum mode=primitiveset->getMode();

            unsigned int primLength;
            switch(mode)
            {
                case(GL_POINTS):    primLength=1; break;
                case(GL_LINES):     primLength=2; break;
                case(GL_TRIANGLES): primLength=3; break;
                case(GL_QUADS):     primLength=4; break;
                default:            primLength=0; break; // compute later when =0.
            }

            // draw primitives by the more flexible "slow" path,
            // sending OpenGL glBegin/glVertex.../glEnd().
            switch(primitiveset->getType())
            {
                case(PrimitiveSet::DrawArraysPrimitiveType):
                {
                    if (primLength==0) primLength=primitiveset->getNumIndices();

                    const DrawArrays* drawArray = static_cast<const DrawArrays*>(primitiveset);

                    unsigned int primCount=0;
                    unsigned int indexEnd = drawArray->getFirst()+drawArray->getCount();
                    for(unsigned int vindex=drawArray->getFirst();
                        vindex<indexEnd;
                        ++vindex,++primCount)
                    {
                        if ((primCount%primLength)==0)
                        {
                            primitiveNum++;
                        }
                    }
                    if ((indexEnd-1) > maxVertexNumber) maxVertexNumber = (indexEnd-1);
                    break;
                }
                case(PrimitiveSet::DrawArrayLengthsPrimitiveType):
                {
                    const DrawArrayLengths* drawArrayLengths = static_cast<const DrawArrayLengths*>(primitiveset);
                    unsigned int vindex=drawArrayLengths->getFirst();
                    for(DrawArrayLengths::const_iterator primItr=drawArrayLengths->begin();
                        primItr!=drawArrayLengths->end();
                        ++primItr)
                    {
                        unsigned int localPrimLength;
                        if (primLength==0) localPrimLength=*primItr;
                        else localPrimLength=primLength;

                        for(GLsizei primCount=0;
                            primCount<*primItr;
                            ++vindex,++primCount)
                        {
                            if ((primCount%localPrimLength)==0)
                            {
                                primitiveNum++;
                            }
                        }

                    }
                    if ((vindex-1) > maxVertexNumber) maxVertexNumber = (vindex-1);
                    break;
                }
                case(PrimitiveSet::DrawElementsUBytePrimitiveType):
                {
                    if (primLength==0) primLength=primitiveset->getNumIndices();

                    const DrawElementsUByte* drawElements = static_cast<const DrawElementsUByte*>(primitiveset);

                    unsigned int primCount=0;
                    for(DrawElementsUByte::const_iterator primItr=drawElements->begin();
                        primItr!=drawElements->end();
                        ++primCount,++primItr)
                    {
                        if ((primCount%primLength)==0)
                        {
                            primitiveNum++;
                        }
                        unsigned int vindex = *primItr;
                        if (vindex > maxVertexNumber) maxVertexNumber = vindex;
                    }

                    break;
                }
                case(PrimitiveSet::DrawElementsUShortPrimitiveType):
                {
                    if (primLength==0) primLength=primitiveset->getNumIndices();

                    const DrawElementsUShort* drawElements = static_cast<const DrawElementsUShort*>(primitiveset);
                    unsigned int primCount=0;
                    for(DrawElementsUShort::const_iterator primItr=drawElements->begin();
                        primItr!=drawElements->end();
                        ++primCount,++primItr)
                    {
                        if ((primCount%primLength)==0)
                        {
                            primitiveNum++;
                        }
                        unsigned int vindex = *primItr;
                        if (vindex > maxVertexNumber) maxVertexNumber = vindex;
                    }

                    break;
                }
                case(PrimitiveSet::DrawElementsUIntPrimitiveType):
                {
                    if (primLength==0) primLength=primitiveset->getNumIndices();

                    const DrawElementsUInt* drawElements = static_cast<const DrawElementsUInt*>(primitiveset);
                    unsigned int primCount=0;
                    for(DrawElementsUInt::const_iterator primItr=drawElements->begin();
                        primItr!=drawElements->end();
                        ++primCount,++primItr)
                    {
                        if ((primCount%primLength)==0)
                        {
                            primitiveNum++;
                        }
                        unsigned int vindex = *primItr;
                        if (vindex > maxVertexNumber) maxVertexNumber = vindex;
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    bool validArray(std::ostream& out, const osg::Geometry::ArrayData& arrayData, const char* arrayName)
    {
        unsigned int numRequired = 0;
        switch(arrayData.binding)
        {
            case(osg::Geometry::BIND_OFF): numRequired = 0; break;
            case(osg::Geometry::BIND_OVERALL): numRequired = 1; break;
            case(osg::Geometry::BIND_PER_PRIMITIVE): numRequired = primitiveNum; break;
            case(osg::Geometry::BIND_PER_PRIMITIVE_SET): numRequired = numPrimitiveSets; break;
            case(osg::Geometry::BIND_PER_VERTEX): numRequired = maxVertexNumber+1; break;
        }

        if (arrayData.indices)
        {
            unsigned int numIndices= arrayData.indices ? arrayData.indices->getNumElements() : 0;
            if (numIndices<numRequired)
            {
                return false;
            }

            unsigned int numNormals = arrayData.array ? arrayData.array->getNumElements() : 0;
            for(unsigned int i=0; i<numIndices; ++i)
            {
                if (arrayData.indices->index(i)>=numNormals)
                {
                    return false;
                }
            }
        }
        else
        {
            unsigned int numElements = arrayData.array ? arrayData.array->getNumElements() : 0;
            if (numElements<numRequired)
            {
                return false;
            }
        }
        return true;
    }

    unsigned int numPrimitiveSets;
    unsigned int primitiveNum;
    unsigned int maxVertexNumber;

};

bool Geometry::verifyArrays(std::ostream& out) const
{
    CheckArrayValidity cav(this);

    bool result = true;

    if (!cav.validArray(out, _vertexData, "Vertex")) result = false;
    if (!cav.validArray(out, _normalData, "Normal")) result = false;
    if (!cav.validArray(out, _colorData, "Color")) result = false;
    if (!cav.validArray(out, _secondaryColorData, "SecondaryColor")) result = false;
    if (!cav.validArray(out, _fogCoordData, "FogCoord")) result = false;

    for(unsigned int ti=0;ti<_texCoordList.size();++ti)
    {
        if (!cav.validArray(out, _texCoordList[ti], "TexCoord")) result = false;
    }

    for(unsigned int vi=0;vi<_vertexAttribList.size();++vi)
    {
        if (!cav.validArray(out, _vertexAttribList[vi], "TexCoord")) result = false;
    }

    return result;
}
