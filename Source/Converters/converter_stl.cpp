/*******************************************************************************
 * Copyright (c) 2007, 2008, CerroKai Development
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of CerroKai Development nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY CerroKai Development ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CerroKai Development BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ********************************************************************************/


/*** Included Header Files ***/
#include "Converters/converter_stl.h"


/***********************************************~***************************************************/


bool WCConverterSTL::ExecuteExport(const std::string &filename, WCPart *part) {
	//Make sure there is at least one surface
	if ((part->SurfaceMap().size() == 0) || !part->TopologyModel()) {
		CLOGGER_INFO(WCLogManager::RootLogger(), "WCConverterSTL::Export - No surfaces found in Part.");
		return false;
	}
	//Try to open file for output
	std::ofstream file(filename.c_str());
	if (!file) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConverterSTL::Export - Unable to create file.");
		return false;
	}

	//Write out the file information
	file << "solid " << filename << std::endl;

	//Get the topology model
	WSFaceUse *firstFace, *nextFace;
	WPUInt numTriangles;
	GLint index1, index2, index3;
	WCVector4 vertex1, vertex2, vertex3, norm;
	WCNurbsSurface *nurbs;
	WCTrimmedNurbsSurface *trim;
	GLfloat *vertexBuffer;
	GLint *indexBuffer;
	std::list<WSTopologyShell*> shellList = part->TopologyModel()->ShellList();
	for (std::list<WSTopologyShell*>::iterator listIter = shellList.begin(); listIter != shellList.end(); listIter++) {
		WPUInt lodU = 2, lodV = 2;
		//Get the first face in the list of faces
		firstFace = (*listIter)->faceUses;
		nextFace = firstFace;
		//Go through each surface of each shell
		do {
			//See if the face is a NURBS surface
			trim = dynamic_cast<WCTrimmedNurbsSurface*> (nextFace->surface);
			if (trim) {
				//Generate a tessellation of the surface
				//...
			}
			//Must be a trimmed surface
			else {
				//Cast to a NURBS surface
				nurbs = dynamic_cast<WCNurbsSurface*> (nextFace->surface);
				//Generate a client-side bufferes of the surface
				std::vector<GLfloat*> bufferList = nurbs->GenerateClientBuffers(lodU, lodV, true);
				vertexBuffer = bufferList.at(0);
				indexBuffer = (GLint*) bufferList.at(3);
				//Determine number of triangles
				numTriangles = (lodU - 1) * (lodV - 1) * 2;
				//Go through the index and retrieve vertices
				for (WPUInt i=0; i<numTriangles; i++) {
					//Get the three indices
					index1 = indexBuffer[i*3] * 4;
					index2 = indexBuffer[i*3+1] * 4;
					index3 = indexBuffer[i*3+2] * 4;
					vertex1 = WCVector4(vertexBuffer[index1], vertexBuffer[index1+1], vertexBuffer[index1+2]);
					vertex2 = WCVector4(vertexBuffer[index2], vertexBuffer[index2+1], vertexBuffer[index2+2]);
					vertex3 = WCVector4(vertexBuffer[index3], vertexBuffer[index3+1], vertexBuffer[index3+2]);
					//Calculate the norm
					WCVector4 vec1 = vertex2 - vertex1;
					WCVector4 vec2 = vertex3 - vertex1;
					norm = vec2.CrossProduct(vec1).Normalize(true);
					//Invert normal if surface is not aligned with faceUse
					if (!nextFace->orientation) norm *= -1.0;
					//Output facet to file
					file << "facet normal " << std::fixed << norm.I() << " " << norm.J() << " " << norm.K() << std::endl;
					file << "   outer loop\n";
					file << "      vertex " << std::fixed << vertex1.I() << " " << vertex1.J() << " " << vertex1.K() << std::endl;
					file << "      vertex " << std::fixed << vertex2.I() << " " << vertex2.J() << " " << vertex2.K() << std::endl;
					file << "      vertex " << std::fixed << vertex3.I() << " " << vertex3.J() << " " << vertex3.K() << std::endl;
					file << "   endloop\n";
					file << "endfacet\n";
				}
				//Dispose of the buffers
				nurbs->ReleaseBuffers(bufferList);
			}
				
			//Go to the next face
			nextFace = nextFace->next;
		//Only contine until the end of the list of faces
		} while (nextFace != firstFace);
	}

	//Close the solid information
	file << "endsolid " << filename << std::endl;
	//Close the file	
	file.close();
	return true;
}



bool WCConverterSTL::Export(WCFeature *feature) {
	//Try to cast feature to a part
	WCPart *part = dynamic_cast<WCPart*>(feature);
	//Make sure it worked
	if (!part) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConverterSTL::Export - Non-Part passed.");
		return false;
	}
	//Get the file name
	std::string filename = "output.stl";
	//Execute the export
	return this->ExecuteExport(filename, part);
}


/***********************************************~***************************************************/

