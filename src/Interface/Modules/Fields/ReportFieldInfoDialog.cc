/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2012 Scientific Computing and Imaging Institute,
   University of Utah.

   License for the specific language governing rights and limitations under
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

#include <Interface/Modules/Fields/ReportFieldInfoDialog.h>
#include <Core/Algorithms/Field/ReportFieldInfoAlgorithm.h>
#include <Dataflow/Network/ModuleStateInterface.h>  //TODO: extract into intermediate

using namespace SCIRun::Gui;
using namespace SCIRun::Dataflow::Networks;
using namespace SCIRun::Core::Algorithms::Fields;


ReportFieldInfoDialog::ReportFieldInfoDialog(const std::string& name, ModuleStateHandle state,
  QWidget* parent /* = 0 */)
  : ModuleDialogGeneric(state, parent)
{
  setupUi(this);
  setWindowTitle(QString::fromStdString(name));
  fixSize();

  buttonBox->setVisible(false);
}

void ReportFieldInfoDialog::pullAndDisplayInfo() 
{
  auto info = optional_any_cast_or_default<ReportFieldInfoAlgorithm::Outputs>(state_->getTransientValue("ReportedInfo"));

  std::ostringstream ostr;
  ostr << "Type: " << info.type << std::endl;
  ostr << "Center: " << info.center << std::endl;
  ostr << "Size: " << info.size << std::endl;
  ostr << "Data min,max: " << info.dataMin << " , " << info.dataMax << std::endl;
  ostr << "# nodes: " << info.numnodes_ << std::endl;
  ostr << "# elements: " << info.numelements_ << std::endl;
  ostr << "# data: " << info.numdata_ << std::endl;
  ostr << "Data location: " << info.dataLocation << std::endl;
  ostr << "Dims (x,y,z): " << info.dims << std::endl;
  ostr << "Geometric size: " << info.geometricSize << std::endl;

  fieldInfoTextEdit_->setPlainText(ostr.str().c_str());
}
