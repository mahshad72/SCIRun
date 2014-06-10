/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2012 Scientific Computing and Imaging Institute,
   University of Utah.

   
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/
/// @todo Documentation Core/Datatypes/Geometry.h

#ifndef CORE_DATATYPES_GEOMETRY_H
#define CORE_DATATYPES_GEOMETRY_H 

#include <cstdint>
#include <list>
#include <vector>
#include <Core/Datatypes/Datatype.h>
#include <Core/GeometryPrimitives/BBox.h>
#include <Core/Datatypes/share.h>

// CPM modules
#include <glm/glm.hpp>
#include <var-buffer/VarBuffer.hpp>

namespace SCIRun {
namespace Core {
namespace Datatypes {

  class SCISHARE GeometryObject : public Datatype
  {
  public:
    explicit GeometryObject(DatatypeConstHandle dh);
    GeometryObject(const GeometryObject& other);
    GeometryObject& operator=(const GeometryObject& other);
    DatatypeConstHandle get_underlying() const;
    virtual GeometryObject* clone() const { return new GeometryObject(*this); }

    std::string objectName;     ///< Name of this object. Should be unique 
                                ///< across all modules in the network.

    // Could require rvalue references...
    struct SpireVBO
    {
      struct AttributeData
      {
        AttributeData(const std::string& nameIn, size_t sizeIn, bool normalizeIn = false) :
            name(nameIn),
            sizeInBytes(sizeIn),
            normalize(normalizeIn)
        {}

        std::string name;
        size_t      sizeInBytes;
        bool        normalize;
      };

      SpireVBO(const std::string& vboName, const std::vector<AttributeData> attribs,
               std::shared_ptr<CPM_VAR_BUFFER_NS::VarBuffer> vboData,
               const Core::Geometry::BBox& bbox) :
          name(vboName),
          attributes(attribs),
          data(vboData),
          boundingBox(bbox)
      {}

      std::string                           name;
      std::vector<AttributeData>            attributes;
      std::shared_ptr<CPM_VAR_BUFFER_NS::VarBuffer> data; // Change to unique_ptr w/ move semantics (possibly).
      Core::Geometry::BBox                  boundingBox;
    };

    struct SpireIBO
    {
      enum PRIMITIVE
      {
        POINTS,
        LINES,
        TRIANGLES,
      };

      SpireIBO(const std::string& iboName, PRIMITIVE primIn, size_t iboIndexSize,
               std::shared_ptr<CPM_VAR_BUFFER_NS::VarBuffer> iboData) :
          name(iboName),
          indexSize(iboIndexSize),
          prim(primIn),
          data(iboData)
      {}

      std::string                           name;
      size_t                                indexSize;
      PRIMITIVE                             prim;
      std::shared_ptr<CPM_VAR_BUFFER_NS::VarBuffer> data; // Change to unique_ptr w/ move semantics (possibly).
    };

    std::list<SpireVBO> mVBOs;  ///< Array of vertex buffer objects.
    std::list<SpireIBO> mIBOs;  ///< Array of index buffer objects.

    /// Defines a Spire object 'pass'.
    struct SpireSubPass
    {
      SpireSubPass(const std::string& name, const std::string& vbo, 
                   const std::string& ibo, const std::string& program) :
          passName(name),
          vboName(vbo),
          iboName(ibo),
          programName(program)
      {}

      std::string   passName;
      std::string   vboName;
      std::string   iboName;
      std::string   programName;

      struct Uniform
      {
        enum UniformType
        {
          UNIFORM_SCALAR,
          UNIFORM_VEC4
        };

        Uniform(const std::string& nameIn, float d) :
            name(nameIn),
            type(UNIFORM_SCALAR),
            data(d, 0.0f, 0.0f, 0.0f)
        {}

        Uniform(const std::string& nameIn, const glm::vec4& vec) :
            name(nameIn),
            type(UNIFORM_VEC4),
            data(vec)
        {}

        std::string   name;
        UniformType   type;
        glm::vec4     data;
      };

      std::vector<Uniform> mUniforms;

      void addUniform(const std::string& name, float scalar)
      {
        mUniforms.push_back(Uniform(name, scalar));
      }

      void addUniform(const std::string& name, const glm::vec4& vector)
      {
        mUniforms.push_back(Uniform(name, vector));
      }
    };

    /// List of passes to setup.
    std::list<SpireSubPass>  mPasses;

    /// Optional colormap name.
    boost::optional<std::string> mColorMap;

    double mLowestValue;    ///< Lowest value a field takes on.
    double mHighestValue;   ///< Highest value a field takes on.

    /// \xxx  Possibly implement a list of global uniforms. Only do this if
    ///       there is a clear need for global uniforms.

  private:
    DatatypeConstHandle data_;
  };

  typedef boost::shared_ptr<GeometryObject> GeometryHandle;

}}}


#endif
