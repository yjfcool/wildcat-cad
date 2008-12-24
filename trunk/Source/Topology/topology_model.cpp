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
#include <Topology/topology_model.h>
#include <Topology/topology_model_internal.h>
#include <Topology/topology_types.h>
#include <Geometry/geometric_types.h>
#include <Geometry/geometric_line.h>
#include <Geometry/nurbs_curve.h>


/***********************************************~***************************************************/


WCTopologyModel::WCTopologyModel(const WCTopologyModel &model) : ::WCSerializeableObject(), _shellList() {
	//Just do a simple copy
	_CopyTopologyModel(this, model._shellList);
}


/***
 *	This method requires two passes through the XML.  Because all of the topology objects are so inter-
 *	 referenced they must first all be created and loaded into the Serialization Dictionary.  Then they
 *	 can actually be populated with the correct reference data.
***/
WCTopologyModel::WCTopologyModel(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : ::WCSerializeableObject(),
	_shellList() {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTopologyModel::WCTopologyModel - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Load faceUses (just create and place in dictionary)
	XMLCh* xmlString = xercesc::XMLString::transcode("FaceUse");
	xercesc::DOMNodeList *faceList = element->getElementsByTagName(xmlString);
	xercesc::XMLString::release(&xmlString);
	xercesc::DOMElement* child;
	std::map<xercesc::DOMElement*,WSFaceUse*> faces;
	//Loop through all faces - first pass
	for (WPUInt index=0; index < faceList->getLength(); index++) {
		//Get the indexed node
		xercesc::DOMNode* tmpNode = faceList->item(index);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			child = (xercesc::DOMElement*)tmpNode;
			//Get GUID and register it
			WCGUID guid = WCSerializeableObject::GetStringAttrib(child, "guid");
			//Create new face
			WSFaceUse *face = new WSFaceUse();
			//Insert into GUID dictionary
			dictionary->InsertGUID(guid, face);
			//Insert into faces
			faces.insert( std::make_pair(child, face) );
		}
	}	
	
	//Load loopUses (just create and place in dictionary)
	xmlString = xercesc::XMLString::transcode("LoopUse");
	xercesc::DOMNodeList *loopList = element->getElementsByTagName(xmlString);
	xercesc::XMLString::release(&xmlString);
	std::map<xercesc::DOMElement*,WSLoopUse*> loops;
	//Loop through all loops - first pass
	for (WPUInt index=0; index < loopList->getLength(); index++) {
		//Get the indexed node
		xercesc::DOMNode* tmpNode = loopList->item(index);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			child = (xercesc::DOMElement*)tmpNode;
			//Get GUID and register it
			WCGUID guid = WCSerializeableObject::GetStringAttrib(child, "guid");
			//Create new loop
			WSLoopUse *loop = new WSLoopUse();
			//Insert into GUID dictionary
			dictionary->InsertGUID(guid, loop);
			//Insert into loops
			loops.insert( std::make_pair(child, loop) );
		}
	}

	//Load edgeUses (just create and place in dictionary)
	xmlString = xercesc::XMLString::transcode("EdgeUse");
	xercesc::DOMNodeList *edgeList = element->getElementsByTagName(xmlString);
	xercesc::XMLString::release(&xmlString);
	std::map<xercesc::DOMElement*,WSEdgeUse*> edges;
	//Loop through all edges - first pass
	for (WPUInt index=0; index < edgeList->getLength(); index++) {
		//Get the indexed node
		xercesc::DOMNode* tmpNode = edgeList->item(index);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			child = (xercesc::DOMElement*)tmpNode;
			//Get GUID and register it
			WCGUID guid = WCSerializeableObject::GetStringAttrib(child, "guid");
			//Create new edge
			WSEdgeUse *edge = new WSEdgeUse();
			//Insert into GUID dictionary
			dictionary->InsertGUID(guid, edge);
			//Insert into edges
			edges.insert( std::make_pair(child, edge) );
		}
	}
	
	//Load vertexUses (just create and place in dictionary)
	xmlString = xercesc::XMLString::transcode("VertexUse");
	xercesc::DOMNodeList *vertexList = element->getElementsByTagName(xmlString);
	xercesc::XMLString::release(&xmlString);
	std::map<xercesc::DOMElement*,WSVertexUse*> vertices;
	//Loop through all vertices - first pass
	for (WPUInt index=0; index < vertexList->getLength(); index++) {
		//Get the indexed node
		xercesc::DOMNode* tmpNode = vertexList->item(index);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			child = (xercesc::DOMElement*)tmpNode;
			//Get GUID and register it
			WCGUID guid = WCSerializeableObject::GetStringAttrib(child, "guid");
			//Create new vertex
			WSVertexUse *vertex = new WSVertexUse();
			//Insert into GUID dictionary
			dictionary->InsertGUID(guid, vertex);
			//Insert into vertices
			vertices.insert( std::make_pair(child, vertex) );
		}
	}

	//Restore shells here
	xmlString = xercesc::XMLString::transcode("TopologyShell");
	xercesc::DOMNodeList *shellList = element->getElementsByTagName(xmlString);
	xercesc::XMLString::release(&xmlString);
	xercesc::DOMElement* shellElem;
	//Loop through all shells
	for (WPUInt index=0; index < shellList->getLength(); index++) {
		//Get the indexed node
		xercesc::DOMNode* tmpNode = shellList->item(index);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			shellElem = (xercesc::DOMElement*)tmpNode;
			WSTopologyShell* shell = _LoadTopologyShell(shellElem, dictionary);
			//If shell exists, add it to the list
			if (shell) this->_shellList.push_back(shell);
		}
	}

	//Restore faces here
	std::map<xercesc::DOMElement*,WSFaceUse*>::iterator faceIter;
	for (faceIter = faces.begin(); faceIter != faces.end(); faceIter++) {
		//Load the face
		_LoadTopologyFace( (*faceIter).second, (*faceIter).first, dictionary);
	}

	//Restore loops here
	std::map<xercesc::DOMElement*,WSLoopUse*>::iterator loopIter;
	for (loopIter = loops.begin(); loopIter != loops.end(); loopIter++) {
		//Load the loop
		_LoadTopologyLoop( (*loopIter).second, (*loopIter).first, dictionary);
	}

	//Restore edges here
	std::map<xercesc::DOMElement*,WSEdgeUse*>::iterator edgeIter;
	for (edgeIter = edges.begin(); edgeIter != edges.end(); edgeIter++) {
		//Load the edge
		_LoadTopologyEdge( (*edgeIter).second, (*edgeIter).first, dictionary);
	}

	//Restore vertices here
	std::map<xercesc::DOMElement*,WSVertexUse*>::iterator vertexIter;
	for (vertexIter = vertices.begin(); vertexIter != vertices.end(); vertexIter++) {
		//Load the vertex
		_LoadTopologyVertex( (*vertexIter).second, (*vertexIter).first, dictionary);
	}
}


WCTopologyModel::~WCTopologyModel() {
	//Delete all shells
	std::list<WSTopologyShell*>::iterator listIter;
	for (listIter = this->_shellList.begin(); listIter != this->_shellList.end(); listIter++) {
		//Delete the shells
		_DeleteTopologyShell( *listIter );
	}
}


void WCTopologyModel::AddShell(WSTopologyShell* shell) {
	//Should check to see if shell is already in model
	//...
	//Just add it into the list for now
	this->_shellList.push_back(shell);
}


xercesc::DOMElement* WCTopologyModel::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("TopologyModel");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Loop through all shells to build element
	std::list<WSTopologyShell*>::const_iterator listIter;
	xercesc::DOMElement* shellElement;
	for (listIter = this->_shellList.begin(); listIter != this->_shellList.end(); listIter++) {
		//Serialize the shells
		shellElement = _SerializeTopologyShell(*listIter, document, dictionary);
		//Add element as child (as appropriate)
		if (shellElement) element->appendChild(shellElement);
	}
	//Return the primary element
	return element;
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCTopologyModel &model) {
	out << "Topology Model (" << &model << ")\n";
	//Print all shells
	std::list<WSTopologyShell*>::const_iterator listIter = model._shellList.begin();
	for (; listIter != model._shellList.end(); listIter++) {
		//Print the shells
		_PrintTopologyShell(out, *listIter, 1);
	}
	return out;
}


/***********************************************~***************************************************/

