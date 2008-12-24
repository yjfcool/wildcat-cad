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


#ifndef __TOPOLOGY_MODEL_INTERNAL_H__
#define __TOPOLOGY_MODEL_INTERNAL_H__


/*** Included Header Files ***/
#include <Topology/wtpkl.h>
#include <Topology/topology_types.h>


/*** Locally Defined Values ***/
//None

	
/***********************************************~***************************************************/


//Load from XML-block Methods
void _LoadTopologyVertex(WSVertexUse* vertex, xercesc::DOMElement *element, WCSerialDictionary *dictionary);
void _LoadTopologyEdge(WSEdgeUse* edge, xercesc::DOMElement *element, WCSerialDictionary *dictionary);
void _LoadTopologyLoop(WSLoopUse* loop, xercesc::DOMElement *element, WCSerialDictionary *dictionary);
void _LoadTopologyFace(WSFaceUse* face, xercesc::DOMElement *element, WCSerialDictionary *dictionary);
WSTopologyShell* _LoadTopologyShell(xercesc::DOMElement *element, WCSerialDictionary *dictionary);


//Copy Topology Hierarchy Methods
void _CopyTopologyModel(WCTopologyModel* destination, const std::list<WSTopologyShell*> &source);


//Serialize to XML-block Methods
xercesc::DOMElement* _SerializeTopologyVertex(WSVertexUse *vertex, xercesc::DOMDocument *doc, WCSerialDictionary *dict);
xercesc::DOMElement* _SerializeTopologyEdge(WSEdgeUse *edge, xercesc::DOMDocument *doc, WCSerialDictionary *dict);
xercesc::DOMElement* _SerializeTopologyLoop(WSLoopUse *loop, xercesc::DOMDocument *doc, WCSerialDictionary *dict);
xercesc::DOMElement* _SerializeTopologyFace(WSFaceUse *face, xercesc::DOMDocument *doc, WCSerialDictionary *dict);
xercesc::DOMElement* _SerializeTopologyShell(WSTopologyShell *shell, xercesc::DOMDocument *doc, WCSerialDictionary *dict);


//Print to Output Methods
void _PrintTopologyVertex(std::ostream &out, WSVertexUse *vertexUse, const unsigned int &depth);
void _PrintTopologyEdge(std::ostream &out, WSEdgeUse *edgeUse, const unsigned int &depth);
void _PrintTopologyLoop(std::ostream &out, WSLoopUse *loopUse, const unsigned int &depth);
void _PrintTopologyFace(std::ostream &out, WSFaceUse *faceUse, const unsigned int &depth);
void _PrintTopologyShell(std::ostream &out, WSTopologyShell *shell, const unsigned int &depth);


//Delete Topology Hierarchy Methods
void _DeleteTopologyShell(WSTopologyShell *shell);


/***********************************************~***************************************************/


#endif //__TOPOLOGY_MODEL_H__

