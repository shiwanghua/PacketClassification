//
// Created by swh on 23-1-11.
//

#include "PTTree.h"

/*
 *	MIT License
 *
 *	Copyright(c) 2022 ShangHai Jiao Tong Univiersity CIT Laboratory.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this softwareand associated documentation files(the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions :
 *
 *	The above copyright noticeand this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

uint8_t maskHash[33][4];

PTtree::PTtree(vector<uint8_t>& list, int _portField) : layerFields(list), portField(_portField), pTree(NULL), aTree(NULL), totalNodes(0){}

PTtree::~PTtree()
{
	if (pTree != NULL) {
		switch (layerFields.size())
		{
		case 3: {
			freeStaticNode((IpNode_static*)pTree);
			break;
		}
		default:
			freeNode((IpNode*)pTree);
			break;
		}
	}
	if (aTree != NULL) {
		for (auto&& c : aTree->child) {
			PortNode_static* pnode = (PortNode_static*)c.second;
			for (auto&& leaf : pnode->child) {
				delete(leaf.second);
			}
			delete(pnode);
		}
		delete(aTree);
	}
}

void PTtree::freeStaticNode(IpNode_static* node)
{
	if (node->childType) {
		for (auto&& c : node->child) {
			if (c.pointer != NULL)delete((LeafNode*)c.pointer);
		}
	}
	else {
		for (auto&& c : node->child) {
			if (c.pointer != NULL)freeStaticNode((IpNode_static*)c.pointer);
		}
	}
	delete(node);
}

void PTtree::freeNode(IpNode* node)
{
	if (node->childType) {
		for (auto&& t : node->tableList) {
			for (int i = 0; i < t.child.size(); ++i)delete((LeafNode*)(t.child[i].second));
		}
	}
	else {
		for (auto&& t : node->tableList) {
			for (int i = 0; i < t.child.size(); ++i)freeNode((IpNode*)(t.child[i].second));
		}
	}
	delete(node);
}

void PTtree::insert(rule& r)
{
	if (r.source_mask < 4 && r.destination_mask < 4) { //inser in assit tree
		if (aTree == NULL) {
			aTree = new ProtoNode();
			++totalNodes;
		}
		int proto = r.protocol[1];
		int proto_idx = aTree->table[proto];
		int lport_idx, hport_idx;
		if (portField == 0) { lport_idx = r.source_port[0] / 2, hport_idx = r.source_port[1] / 2; }
		else { lport_idx = r.destination_port[0] / 2, hport_idx = r.destination_port[1] / 2; }
		if (proto_idx == -1) {
			aTree->table[proto] = aTree->child.size();
			PortNode_static* pnode = new PortNode_static(portNodeList.size());
			++totalNodes;
			if (lport_idx == hport_idx) pnode->table[lport_idx] = 0;
			else pnode->table[32768] = 0;
			LeafNode* lnode = new LeafNode();
			++totalNodes;
			aLeafNodeList.emplace_back(lnode);
			lnode->rule_.emplace_back(r);
			pnode->child.emplace_back(pair<uint32_t, LeafNode*>(r.PRI, lnode));
			aTree->child.emplace_back(pair<uint32_t, void*>(r.PRI, pnode));
			portNodeList.emplace_back(pnode);
		}
		else {
			PortNode_static* pnode = (PortNode_static*)aTree->child[proto_idx].second;
			if (r.PRI < aTree->child[proto_idx].first)aTree->child[proto_idx].first = r.PRI;
			int c_id;
			if (lport_idx == hport_idx)c_id = lport_idx;
			else c_id = 32768;
			int le_id = pnode->table[c_id];
			if (le_id == -1) {
				pnode->table[c_id] = pnode->child.size();
				LeafNode* lnode = new LeafNode();
				++totalNodes;
				aLeafNodeList.emplace_back(lnode);
				lnode->rule_.emplace_back(r);
				pnode->child.emplace_back(pair<uint32_t, LeafNode*>(r.PRI, lnode));
			}
			else {
				if (r.PRI < pnode->child[le_id].first)pnode->child[le_id].first = r.PRI;
				LeafNode* lnode = pnode->child[le_id].second;
				lnode->rule_.emplace_back(r);
			}
		}
	}
	else { // insert in PTtree
		switch(layerFields.size())
		{
		case 3: {
			if (pTree == NULL) {
				pTree = new IpNode_static(layerFields[0], 0, 0, 0);
				ipNodeList.emplace_back(pTree);
				++totalNodes;
			}
			IpNode_static* node = (IpNode_static*)pTree;
			int layer = 0;
			unsigned int mask, ip;
			while (layer < 2) {
				switch (node->field)
				{
				case 0:
					mask = maskHash[(unsigned int)r.source_mask][0];
					ip = (unsigned int)r.source_ip[3];
					break;
				case 1:
					mask = maskHash[(unsigned int)r.source_mask][1];
					ip = (unsigned int)r.source_ip[2];
					break;
				case 2:
					mask = maskHash[(unsigned int)r.source_mask][2];
					ip = (unsigned int)r.source_ip[1];
					break;
				case 3:
					mask = maskHash[(unsigned int)r.source_mask][3];
					ip = (unsigned int)r.source_ip[0];
					break;
				case 4:
					mask = maskHash[(unsigned int)r.destination_mask][0];
					ip = (unsigned int)r.destination_ip[3];
					break;
				case 5:
					mask = maskHash[(unsigned int)r.destination_mask][1];
					ip = (unsigned int)r.destination_ip[2];
					break;
				case 6:
					mask = maskHash[(unsigned int)r.destination_mask][2];
					ip = (unsigned int)r.destination_ip[1];
					break;
				case 7:
					mask = maskHash[(unsigned int)r.destination_mask][3];
					ip = (unsigned int)r.destination_ip[0];
					break;
				default:
					break;
				}
				int ip_idx = mask == 8 ? ip : 256;
				if (node->child[ip_idx].pointer == NULL) {
					IpNode_static* newchild = new IpNode_static(layerFields[layer + 1], 0, layer + 1, ipNodeList.size());
					node->child[ip_idx].pointer = newchild;
					node->child[ip_idx].pri = r.PRI;
					ipNodeList.emplace_back(newchild);
					++totalNodes;
					node = newchild;
				}
				else
				{
					if (r.PRI < node->child[ip_idx].pri)node->child[ip_idx].pri = r.PRI;
					node = (IpNode_static*)node->child[ip_idx].pointer;
				}
				++layer;
			}
			// process leafnode
			node->childType = 1;
			switch (node->field)
			{
			case 0:
				mask = maskHash[(unsigned int)r.source_mask][0];
				ip = (unsigned int)r.source_ip[3];
				break;
			case 1:
				mask = maskHash[(unsigned int)r.source_mask][1];
				ip = (unsigned int)r.source_ip[2];
				break;
			case 2:
				mask = maskHash[(unsigned int)r.source_mask][2];
				ip = (unsigned int)r.source_ip[1];
				break;
			case 3:
				mask = maskHash[(unsigned int)r.source_mask][3];
				ip = (unsigned int)r.source_ip[0];
				break;
			case 4:
				mask = maskHash[(unsigned int)r.destination_mask][0];
				ip = (unsigned int)r.destination_ip[3];
				break;
			case 5:
				mask = maskHash[(unsigned int)r.destination_mask][1];
				ip = (unsigned int)r.destination_ip[2];
				break;
			case 6:
				mask = maskHash[(unsigned int)r.destination_mask][2];
				ip = (unsigned int)r.destination_ip[1];
				break;
			case 7:
				mask = maskHash[(unsigned int)r.destination_mask][3];
				ip = (unsigned int)r.destination_ip[0];
				break;
			default:
				break;
			}
			int ip_idx = mask == 8 ? ip : 256;
			if (node->child[ip_idx].pointer == NULL) {
				LeafNode* newchild = new LeafNode();
				newchild->rule_.emplace_back(r);
				node->child[ip_idx].pointer = newchild;
				node->child[ip_idx].pri = r.PRI;
				pLeafNodeList.emplace_back(newchild);
				++totalNodes;
			}
			else
			{
				if (r.PRI < node->child[ip_idx].pri)node->child[ip_idx].pri = r.PRI;
				LeafNode* ln = (LeafNode*)node->child[ip_idx].pointer;
				ln->rule_.emplace_back(r);
			}
			break;
		}
		default: {
			if (pTree == NULL) {
				pTree = new IpNode(layerFields[0], 0, 0, 0);
				ipNodeList.emplace_back(pTree);
				++totalNodes;
			}
			IpNode* node = (IpNode*)pTree;
			int totalLayer = layerFields.size();
			int layer = 0;
			unsigned int mask, ip;
			while (layer < totalLayer - 1) {
				switch (node->field)
				{
				case 0:
					mask = maskHash[(unsigned int)r.source_mask][0];
					ip = (unsigned int)r.source_ip[3] >> (8 - mask);
					break;
				case 1:
					mask = maskHash[(unsigned int)r.source_mask][1];
					ip = (unsigned int)r.source_ip[2] >> (8 - mask);
					break;
				case 2:
					mask = maskHash[(unsigned int)r.source_mask][2];
					ip = (unsigned int)r.source_ip[1] >> (8 - mask);
					break;
				case 3:
					mask = maskHash[(unsigned int)r.source_mask][3];
					ip = (unsigned int)r.source_ip[0] >> (8 - mask);
					break;
				case 4:
					mask = maskHash[(unsigned int)r.destination_mask][0];
					ip = (unsigned int)r.destination_ip[3] >> (8 - mask);
					break;
				case 5:
					mask = maskHash[(unsigned int)r.destination_mask][1];
					ip = (unsigned int)r.destination_ip[2] >> (8 - mask);
					break;
				case 6:
					mask = maskHash[(unsigned int)r.destination_mask][2];
					ip = (unsigned int)r.destination_ip[1] >> (8 - mask);
					break;
				case 7:
					mask = maskHash[(unsigned int)r.destination_mask][3];
					ip = (unsigned int)r.destination_ip[0] >> (8 - mask);
					break;
				default:
					break;
				}
				if (node->tableList.empty()) { // do not have table, create
					IpTable t(mask);
					IpNode* newchild = new IpNode(layerFields[layer + 1], 0, layer + 1, ipNodeList.size());
					t.pri = r.PRI;
					t.table[ip] = 0;
					t.child.emplace_back(pair<uint32_t,void*>(r.PRI, newchild));
					node->tableList.emplace_back(t);
					node = newchild;
					ipNodeList.emplace_back(newchild);
					++totalNodes;
				}
				else
				{
					list<IpTable>::iterator it = node->tableList.begin();
					for (; it != node->tableList.end(); ++it) {
						if (mask == it->mask) {  // have table
							if (it->pri > r.PRI) it->pri = r.PRI;
							if (it->table[ip] == -1) { // creat child
								IpNode* newchild = new IpNode(layerFields[layer + 1], 0, layer + 1, ipNodeList.size());
								it->table[ip] = it->child.size();
								it->child.emplace_back(pair<uint32_t,void*>(r.PRI, newchild));
								node = newchild;
								ipNodeList.emplace_back(newchild);
								++totalNodes;
								break;
							}
							else {
								if (it->child[it->table[ip]].first > r.PRI)it->child[it->table[ip]].first = r.PRI;
								node = (IpNode*)(it->child[it->table[ip]].second);
								break;
							}
						}
						if (mask > it->mask) { // find the site
							break;
						}
					}
					if (it == node->tableList.end() || mask != it->mask) { // creat table
						IpTable t(mask);
						IpNode* newchild = new IpNode(layerFields[layer + 1], 0, layer + 1, ipNodeList.size());
						t.pri = r.PRI;
						t.table[ip] = t.child.size();
						t.child.emplace_back(pair<uint32_t, void*>(r.PRI, newchild));
						node->tableList.emplace(it, t);
						node = newchild;
						ipNodeList.emplace_back(newchild);
						++totalNodes;
					}
				}
				++layer;
			}
			// process leafnode
			node->childType = 1;
			switch (node->field)
			{
			case 0:
				mask = maskHash[(unsigned int)r.source_mask][0];
				ip = (unsigned int)r.source_ip[3] >> (8 - mask);
				break;
			case 1:
				mask = maskHash[(unsigned int)r.source_mask][1];
				ip = (unsigned int)r.source_ip[2] >> (8 - mask);
				break;
			case 2:
				mask = maskHash[(unsigned int)r.source_mask][2];
				ip = (unsigned int)r.source_ip[1] >> (8 - mask);
				break;
			case 3:
				mask = maskHash[(unsigned int)r.source_mask][3];
				ip = (unsigned int)r.source_ip[0] >> (8 - mask);
				break;
			case 4:
				mask = maskHash[(unsigned int)r.destination_mask][0];
				ip = (unsigned int)r.destination_ip[3] >> (8 - mask);
				break;
			case 5:
				mask = maskHash[(unsigned int)r.destination_mask][1];
				ip = (unsigned int)r.destination_ip[2] >> (8 - mask);
				break;
			case 6:
				mask = maskHash[(unsigned int)r.destination_mask][2];
				ip = (unsigned int)r.destination_ip[1] >> (8 - mask);
				break;
			case 7:
				mask = maskHash[(unsigned int)r.destination_mask][3];
				ip = (unsigned int)r.destination_ip[0] >> (8 - mask);
				break;
			default:
				break;
			}
			if (node->tableList.empty()) { // do not have table, create
				IpTable t(mask);
				LeafNode* newchild = new LeafNode();
				newchild->rule_.emplace_back(r);
				t.pri = r.PRI;
				t.table[ip] = 0;
				t.child.emplace_back(pair<uint32_t, void*>(r.PRI, newchild));
				node->tableList.emplace_back(t);
				pLeafNodeList.emplace_back(newchild);
				++totalNodes;
			}
			else
			{
				list<IpTable>::iterator it = node->tableList.begin();
				for (; it != node->tableList.end(); ++it) {
					if (mask == it->mask) {  // have table
						if (it->pri > r.PRI) it->pri = r.PRI;
						if (it->table[ip] == -1) { // creat child
							LeafNode* newchild = new LeafNode();
							newchild->rule_.emplace_back(r);
							it->table[ip] = it->child.size();
							it->child.emplace_back(pair<uint32_t, void*>(r.PRI, newchild));
							pLeafNodeList.emplace_back(newchild);
							++totalNodes;
							break;
						}
						else {
							if (it->child[it->table[ip]].first > r.PRI)it->child[it->table[ip]].first = r.PRI;
							((LeafNode*)(it->child[it->table[ip]].second))->rule_.emplace_back(r);
							break;
						}
					}
					if (mask > it->mask) { // find the site
						break;
					}
				}
				if (it == node->tableList.end() || mask != it->mask) { // creat table
					IpTable t(mask);
					LeafNode* newchild = new LeafNode();
					newchild->rule_.emplace_back(r);
					t.pri = r.PRI;
					t.table[ip] = t.child.size();
					t.child.emplace_back(pair<uint32_t, void*>(r.PRI, newchild));
					node->tableList.emplace(it, t);
					pLeafNodeList.emplace_back(newchild);
					++totalNodes;
				}
			}
			break;
		}
		}
	}
}

bool PTtree::remove(rule& r)
{
	if (r.source_mask < 4 && r.destination_mask < 4) { //remove in assit tree
		if (aTree == NULL) {
			return false;
		}
		int proto = r.protocol[1];
		int proto_idx = aTree->table[proto];
		int lport_idx, hport_idx;
		if (portField == 0) { lport_idx = r.source_port[0] / 2, hport_idx = r.source_port[1] / 2; }
		else { lport_idx = r.destination_port[0] / 2, hport_idx = r.destination_port[1] / 2; }
		if (proto_idx == -1) {
			return false;
		}
		else {
			PortNode_static* p_node = (PortNode_static*)aTree->child[proto_idx].second;
			int c_id;
			if (lport_idx == hport_idx)c_id = lport_idx;
			else c_id = 32768;
			int le_id = p_node->table[c_id];
			if (le_id == -1) {
				return false;
			}
			else {
				LeafNode* lnode = p_node->child[le_id].second;
				for (int i = 0; i < lnode->rule_.size(); ++i) {
					if (lnode->rule_[i].PRI == r.PRI) {
						lnode->rule_.erase(lnode->rule_.begin() + i);
						return true;
					}
				}
				return false;
			}
		}
	}
	else { // remove in PTtree
		if (pTree == NULL) {
			return false;
		}
		switch (layerFields.size())
		{
		case 3: {
			IpNode_static* node = (IpNode_static*)pTree;
			int layer = 0;
			unsigned int mask, ip;
			while (layer < 2) {
				switch (node->field)
				{
				case 0:
					mask = maskHash[(unsigned int)r.source_mask][0];
					ip = (unsigned int)r.source_ip[3];
					break;
				case 1:
					mask = maskHash[(unsigned int)r.source_mask][1];
					ip = (unsigned int)r.source_ip[2];
					break;
				case 2:
					mask = maskHash[(unsigned int)r.source_mask][2];
					ip = (unsigned int)r.source_ip[1];
					break;
				case 3:
					mask = maskHash[(unsigned int)r.source_mask][3];
					ip = (unsigned int)r.source_ip[0];
					break;
				case 4:
					mask = maskHash[(unsigned int)r.destination_mask][0];
					ip = (unsigned int)r.destination_ip[3];
					break;
				case 5:
					mask = maskHash[(unsigned int)r.destination_mask][1];
					ip = (unsigned int)r.destination_ip[2];
					break;
				case 6:
					mask = maskHash[(unsigned int)r.destination_mask][2];
					ip = (unsigned int)r.destination_ip[1];
					break;
				case 7:
					mask = maskHash[(unsigned int)r.destination_mask][3];
					ip = (unsigned int)r.destination_ip[0];
					break;
				default:
					break;
				}
				int ip_idx = mask == 8 ? ip : 256;
				if (node->child[ip_idx].pointer == NULL) return false;
				else node = (IpNode_static*)node->child[ip_idx].pointer;
				++layer;
			}
			// process leafnode
			node->childType = 1;
			switch (node->field)
			{
			case 0:
				mask = maskHash[(unsigned int)r.source_mask][0];
				ip = (unsigned int)r.source_ip[3];
				break;
			case 1:
				mask = maskHash[(unsigned int)r.source_mask][1];
				ip = (unsigned int)r.source_ip[2];
				break;
			case 2:
				mask = maskHash[(unsigned int)r.source_mask][2];
				ip = (unsigned int)r.source_ip[1];
				break;
			case 3:
				mask = maskHash[(unsigned int)r.source_mask][3];
				ip = (unsigned int)r.source_ip[0];
				break;
			case 4:
				mask = maskHash[(unsigned int)r.destination_mask][0];
				ip = (unsigned int)r.destination_ip[3];
				break;
			case 5:
				mask = maskHash[(unsigned int)r.destination_mask][1];
				ip = (unsigned int)r.destination_ip[2];
				break;
			case 6:
				mask = maskHash[(unsigned int)r.destination_mask][2];
				ip = (unsigned int)r.destination_ip[1];
				break;
			case 7:
				mask = maskHash[(unsigned int)r.destination_mask][3];
				ip = (unsigned int)r.destination_ip[0];
				break;
			default:
				break;
			}
			int ip_idx = mask == 8 ? ip : 256;
			if (node->child[ip_idx].pointer == NULL) return false;
			else
			{
				LeafNode* ln = (LeafNode*)node->child[ip_idx].pointer;
				for (int i = 0; i < ln->rule_.size(); ++i) {
					if (ln->rule_[i].PRI == r.PRI) {
						ln->rule_.erase(ln->rule_.begin() + i);
						return true;
					}
				}
				return false;
			}
		}
		default: {
			if (pTree == NULL) {
				return false;
			}
			IpNode* node = (IpNode*)pTree;
			int totalLayer = layerFields.size();
			int layer = 0;
			unsigned int mask, ip;
			while (layer < totalLayer - 1) {
				switch (node->field)
				{
				case 0:
					mask = maskHash[(unsigned int)r.source_mask][0];
					ip = (unsigned int)r.source_ip[3] >> (8 - mask);
					break;
				case 1:
					mask = maskHash[(unsigned int)r.source_mask][1];
					ip = (unsigned int)r.source_ip[2] >> (8 - mask);
					break;
				case 2:
					mask = maskHash[(unsigned int)r.source_mask][2];
					ip = (unsigned int)r.source_ip[1] >> (8 - mask);
					break;
				case 3:
					mask = maskHash[(unsigned int)r.source_mask][3];
					ip = (unsigned int)r.source_ip[0] >> (8 - mask);
					break;
				case 4:
					mask = maskHash[(unsigned int)r.destination_mask][0];
					ip = (unsigned int)r.destination_ip[3] >> (8 - mask);
					break;
				case 5:
					mask = maskHash[(unsigned int)r.destination_mask][1];
					ip = (unsigned int)r.destination_ip[2] >> (8 - mask);
					break;
				case 6:
					mask = maskHash[(unsigned int)r.destination_mask][2];
					ip = (unsigned int)r.destination_ip[1] >> (8 - mask);
					break;
				case 7:
					mask = maskHash[(unsigned int)r.destination_mask][3];
					ip = (unsigned int)r.destination_ip[0] >> (8 - mask);
					break;
				default:
					break;
				}
				list<IpTable>::iterator it = node->tableList.begin();
				for (; it != node->tableList.end(); ++it) {
					if (mask == it->mask) {  // have table
						if (it->table[ip] == -1) { // no child
							return false;
						}
						else {
							node = (IpNode*)(it->child[it->table[ip]].second);
							break;
						}
					}
					if (mask > it->mask) { // find the site
						break;
					}
				}
				if (it == node->tableList.end() || mask != it->mask) { // no table
					return false;
				}
				++layer;
			}
			// process leafnode
			node->childType = 1;
			switch (node->field)
			{
			case 0:
				mask = maskHash[(unsigned int)r.source_mask][0];
				ip = (unsigned int)r.source_ip[3] >> (8 - mask);
				break;
			case 1:
				mask = maskHash[(unsigned int)r.source_mask][1];
				ip = (unsigned int)r.source_ip[2] >> (8 - mask);
				break;
			case 2:
				mask = maskHash[(unsigned int)r.source_mask][2];
				ip = (unsigned int)r.source_ip[1] >> (8 - mask);
				break;
			case 3:
				mask = maskHash[(unsigned int)r.source_mask][3];
				ip = (unsigned int)r.source_ip[0] >> (8 - mask);
				break;
			case 4:
				mask = maskHash[(unsigned int)r.destination_mask][0];
				ip = (unsigned int)r.destination_ip[3] >> (8 - mask);
				break;
			case 5:
				mask = maskHash[(unsigned int)r.destination_mask][1];
				ip = (unsigned int)r.destination_ip[2] >> (8 - mask);
				break;
			case 6:
				mask = maskHash[(unsigned int)r.destination_mask][2];
				ip = (unsigned int)r.destination_ip[1] >> (8 - mask);
				break;
			case 7:
				mask = maskHash[(unsigned int)r.destination_mask][3];
				ip = (unsigned int)r.destination_ip[0] >> (8 - mask);
				break;
			default:
				break;
			}
			list<IpTable>::iterator it = node->tableList.begin();
			for (; it != node->tableList.end(); ++it) {
				if (mask == it->mask) {  // have table
					if (it->table[ip] == -1) { // no child
						return false;
					}
					else {
						LeafNode* ln = ((LeafNode*)(it->child[it->table[ip]].second));
						for (int i = 0; i < ln->rule_.size(); ++i) {
							if (ln->rule_[i].PRI == r.PRI) {
								ln->rule_.erase(ln->rule_.begin() + i);
								return true;
							}
						}
						break;
					}
				}
				if (mask > it->mask) { // find the site
					break;
				}
			}
			return false;
		}
		}
	}
}

int PTtree::search(message& p)
{
	unsigned int pSip, pDip;
	unsigned char pProto;
	unsigned short pSport, pDport;
	pProto = p.protocol;
	memcpy(&pSip, p.source_ip, 4);
	memcpy(&pDip, p.destination_ip, 4);
	pSport = p.source_port;
	pDport = p.destination_port;

	unsigned int mip[4];
	for (int i = 0; i < layerFields.size(); ++i) {
		switch (layerFields[i])
		{
		case 0:
			mip[i] = (unsigned int)p.source_ip[3];
			break;
		case 1:
			mip[i] = (unsigned int)p.source_ip[2];
			break;
		case 2:
			mip[i] = (unsigned int)p.source_ip[1];
			break;
		case 3:
			mip[i] = (unsigned int)p.source_ip[0];
			break;
		case 4:
			mip[i] = (unsigned int)p.destination_ip[3];
			break;
		case 5:
			mip[i] = (unsigned int)p.destination_ip[2];
			break;
		case 6:
			mip[i] = (unsigned int)p.destination_ip[1];
			break;
		case 7:
			mip[i] = (unsigned int)p.destination_ip[0];
			break;
		default:
			break;
		}
	}
	unsigned int res = 0xFFFFFFFF;

	// search in pTree
	switch (layerFields.size())
	{
	case 3: {
		IpNode_static* node_1 = (IpNode_static*)pTree;
		unsigned int i_1[2] = { mip[0], 256 };
		for (int i = 0; i < 2; ++i) {
			if (node_1->child[i_1[i]].pointer == NULL || node_1->child[i_1[i]].pri > res)continue;
			IpNode_static* node_2 = (IpNode_static*)node_1->child[i_1[i]].pointer;
			unsigned int i_2[2] = { mip[1], 256 };
			for (int j = 0; j < 2; ++j) {
				if (node_2->child[i_2[j]].pointer == NULL || node_2->child[i_2[j]].pri > res)continue;
				IpNode_static* node_3 = (IpNode_static*)node_2->child[i_2[j]].pointer;
				unsigned int i_3[2] = { mip[2], 256 };
				for (int k = 0; k < 2; ++k) {
					if (node_3->child[i_3[k]].pointer == NULL || node_3->child[i_3[k]].pri > res)continue;
					LeafNode* ln = (LeafNode*)node_3->child[i_3[k]].pointer;
					for (auto&& r : ln->rule_) {
						if (res < r.PRI)break;
						if (pProto != r.protocol[1] && r.protocol[0] != 0)continue; // check protocol
						if (pDport < r.destination_port[0] || r.destination_port[1] < pDport)continue;  // if destination port not match, check next
						if (pSport < r.source_port[0] || r.source_port[1] < pSport)continue;  // if source port not match, check next
						unsigned int m_bit = 32 - (unsigned int)r.destination_mask;  // comput the bit number need to move
						unsigned int _ip;
						if (m_bit != 32) {
							memcpy(&_ip, r.destination_ip, 4);
							if (pDip >> m_bit != _ip >> m_bit)continue;  // if destination ip not match, check next
						}
						m_bit = 32 - (unsigned int)r.source_mask;  // comput the bit number need to move
						if (m_bit != 32) {
							memcpy(&_ip, r.source_ip, 4);
							if (pSip >> m_bit != _ip >> m_bit)continue;  // if source ip not match, check next
						}
						res = r.PRI;
						break;
					}
				}
			}
		}
		break;
	}
	case 4: {
		IpNode* node_1 = (IpNode*)pTree;
		unsigned int ip_idx = 0;
		for (list<IpTable>::iterator it_1 = node_1->tableList.begin(); it_1 != node_1->tableList.end(); ++it_1) {
			ip_idx = mip[0] >> (8 - it_1->mask);
			if (it_1->pri > res || it_1->table[ip_idx] == -1 || it_1->child[it_1->table[ip_idx]].first > res)continue;
			IpNode* node_2 = (IpNode*)(it_1->child[it_1->table[ip_idx]].second);
			for (list<IpTable>::iterator it_2 = node_2->tableList.begin(); it_2 != node_2->tableList.end(); ++it_2) {
				ip_idx = mip[1] >> (8 - it_2->mask);
				if (it_2->pri > res || it_2->table[ip_idx] == -1 || it_2->child[it_2->table[ip_idx]].first > res)continue;
				IpNode* node_3 = (IpNode*)(it_2->child[it_2->table[ip_idx]].second);
				for (list<IpTable>::iterator it_3 = node_3->tableList.begin(); it_3 != node_3->tableList.end(); ++it_3) {
					ip_idx = mip[2] >> (8 - it_3->mask);
					if (it_3->pri > res || it_3->table[ip_idx] == -1 || it_3->child[it_3->table[ip_idx]].first > res)continue;
					IpNode* node_4 = (IpNode*)(it_3->child[it_3->table[ip_idx]].second);
					for (list<IpTable>::iterator it_4 = node_4->tableList.begin(); it_4 != node_4->tableList.end(); ++it_4) {
						ip_idx = mip[3] >> (8 - it_4->mask);
						if (it_4->pri > res || it_4->table[ip_idx] == -1 || it_4->child[it_4->table[ip_idx]].first > res)continue;
						LeafNode* ln = (LeafNode*)(it_4->child[it_4->table[ip_idx]].second);
						for (auto&& r : ln->rule_) {
							if (res < r.PRI)break;
							if (pProto != r.protocol[1] && r.protocol[0] != 0)continue; // check protocol
							if (pDport < r.destination_port[0] || r.destination_port[1] < pDport)continue;  // if destination port not match, check next
							if (pSport < r.source_port[0] || r.source_port[1] < pSport)continue;  // if source port not match, check next
							unsigned int m_bit = 32 - (unsigned int)r.destination_mask;  // comput the bit number need to move
							unsigned int _ip;
							if (m_bit != 32) {
								memcpy(&_ip, r.destination_ip, 4);
								if (pDip >> m_bit != _ip >> m_bit)continue;  // if destination ip not match, check next
							}
							m_bit = 32 - (unsigned int)r.source_mask;  // comput the bit number need to move
							if (m_bit != 32) {
								memcpy(&_ip, r.source_ip, 4);
								if (pSip >> m_bit != _ip >> m_bit)continue;  // if source ip not match, check next
							}
							res = r.PRI;
							break;
						}
					}
				}
			}
		}
		break;
	}
	default:
		break;
	}

	if (aTree != NULL) {
		int proto_idx[2] = { aTree->table[pProto],aTree->table[0] };
		int port_idx[2];
		for (int i = 0; i < 2; ++i) {
			if (proto_idx[i] != -1 && res > aTree->child[proto_idx[i]].first) {
				PortNode_static* pnode = (PortNode_static*)aTree->child[proto_idx[i]].second;
				if(portField == 0) port_idx[0] = pnode->table[pSport / 2];
				else port_idx[0] = pnode->table[pDport / 2];
				port_idx[1] = pnode->table[32768];
				for (int j = 0; j < 2; ++j) {
					if (port_idx[j] != -1 && res > pnode->child[port_idx[j]].first) {
						LeafNode* ln = pnode->child[port_idx[j]].second;
						for (auto&& r : ln->rule_) {
							if (res < r.PRI)break;
							if (pDport < r.destination_port[0] || r.destination_port[1] < pDport)continue;  // if destination port not match, check next
							if (pSport < r.source_port[0] || r.source_port[1] < pSport)continue;  // if source port not match, check next
							unsigned int m_bit = 32 - (unsigned int)r.destination_mask;  // comput the bit number need to move
							unsigned int _ip;
							if (m_bit != 32) {
								memcpy(&_ip, r.destination_ip, 4);
								if (pDip >> m_bit != _ip >> m_bit)continue;  // if destination ip not match, check next
							}
							m_bit = 32 - (unsigned int)r.source_mask;  // comput the bit number need to move
							if (m_bit != 32) {
								memcpy(&_ip, r.source_ip, 4);
								if (pSip >> m_bit != _ip >> m_bit)continue;  // if source ip not match, check next
							}
							res = r.PRI;
							break;
						}
					}
				}
			}
		}
	}
	return res;
}

int PTtree::search_with_log(message& p, ACL_LOG& log)
{
	unsigned int pSip, pDip;
	unsigned char pProto;
	unsigned short pSport, pDport;
	pProto = p.protocol;
	memcpy(&pSip, p.source_ip, 4);
	memcpy(&pDip, p.destination_ip, 4);
	pSport = p.source_port;
	pDport = p.destination_port;

	unsigned int mip[4];
	for (int i = 0; i < layerFields.size(); ++i) {
		switch (layerFields[i])
		{
		case 0:
			mip[i] = (unsigned int)p.source_ip[3];
			break;
		case 1:
			mip[i] = (unsigned int)p.source_ip[2];
			break;
		case 2:
			mip[i] = (unsigned int)p.source_ip[1];
			break;
		case 3:
			mip[i] = (unsigned int)p.source_ip[0];
			break;
		case 4:
			mip[i] = (unsigned int)p.destination_ip[3];
			break;
		case 5:
			mip[i] = (unsigned int)p.destination_ip[2];
			break;
		case 6:
			mip[i] = (unsigned int)p.destination_ip[1];
			break;
		case 7:
			mip[i] = (unsigned int)p.destination_ip[0];
			break;
		default:
			break;
		}
	}
	unsigned int res = 0xFFFFFFFF;

	// search in pTree
	if (pTree != NULL) {
		switch (layerFields.size())
		{
		case 3: {
			IpNode_static* node_1 = (IpNode_static*)pTree;
			unsigned int i_1[2] = { mip[0], 256 };
			log.ipNodeList.emplace_back(node_1);
			for (int i = 0; i < 2; ++i) {
				if (node_1->child[i_1[i]].pointer == NULL || node_1->child[i_1[i]].pri > res)continue;
				IpNode_static* node_2 = (IpNode_static*)node_1->child[i_1[i]].pointer;
				unsigned int i_2[2] = { mip[1], 256 };
				log.ipNodeList.emplace_back(node_2);
				for (int j = 0; j < 2; ++j) {
					if (node_2->child[i_2[j]].pointer == NULL || node_2->child[i_2[j]].pri > res)continue;
					IpNode_static* node_3 = (IpNode_static*)node_2->child[i_2[j]].pointer;
					unsigned int i_3[2] = { mip[2], 256 };
					log.ipNodeList.emplace_back(node_3);
					for (int k = 0; k < 2; ++k) {
						if (node_3->child[i_3[k]].pointer == NULL || node_3->child[i_3[k]].pri > res)continue;
						LeafNode* ln = (LeafNode*)node_3->child[i_3[k]].pointer;
						log.pLeafNodeList.emplace_back(ln);
						for (auto&& r : ln->rule_) {
							++log.rules;
							if (res < r.PRI)break;
							if (pProto != r.protocol[1] && r.protocol[0] != 0)continue; // check protocol
							if (pDport < r.destination_port[0] || r.destination_port[1] < pDport)continue;  // if destination port not match, check next
							if (pSport < r.source_port[0] || r.source_port[1] < pSport)continue;  // if source port not match, check next
							unsigned int m_bit = 32 - (unsigned int)r.destination_mask;  // comput the bit number need to move
							unsigned int _ip;
							if (m_bit != 32) {
								memcpy(&_ip, r.destination_ip, 4);
								if (pDip >> m_bit != _ip >> m_bit)continue;  // if destination ip not match, check next
							}
							m_bit = 32 - (unsigned int)r.source_mask;  // comput the bit number need to move
							if (m_bit != 32) {
								memcpy(&_ip, r.source_ip, 4);
								if (pSip >> m_bit != _ip >> m_bit)continue;  // if source ip not match, check next
							}
							res = r.PRI;
							break;
						}
					}
				}
			}
			log.tables += log.ipNodeList.size();
			break;
		}
		case 4: {
			IpNode* node_1 = (IpNode*)pTree;
			unsigned int ip_idx = 0;
			log.ipNodeList.emplace_back(node_1);
			for (list<IpTable>::iterator it_1 = node_1->tableList.begin(); it_1 != node_1->tableList.end(); ++it_1) {
				ip_idx = mip[0] >> (8 - it_1->mask);
				if (it_1->pri > res) continue;
				++log.tables;
				if (it_1->table[ip_idx] == -1 || it_1->child[it_1->table[ip_idx]].first > res)continue;
				IpNode* node_2 = (IpNode*)(it_1->child[it_1->table[ip_idx]].second);
				log.ipNodeList.emplace_back(node_2);
				for (list<IpTable>::iterator it_2 = node_2->tableList.begin(); it_2 != node_2->tableList.end(); ++it_2) {
					ip_idx = mip[1] >> (8 - it_2->mask);
					if (it_2->pri > res) continue;
					++log.tables;
					if (it_2->table[ip_idx] == -1 || it_2->child[it_2->table[ip_idx]].first > res)continue;
					IpNode* node_3 = (IpNode*)(it_2->child[it_2->table[ip_idx]].second);
					log.ipNodeList.emplace_back(node_3);
					for (list<IpTable>::iterator it_3 = node_3->tableList.begin(); it_3 != node_3->tableList.end(); ++it_3) {
						ip_idx = mip[2] >> (8 - it_3->mask);
						if (it_3->pri > res) continue;
						++log.tables;
						if (it_3->table[ip_idx] == -1 || it_3->child[it_3->table[ip_idx]].first > res)continue;
						IpNode* node_4 = (IpNode*)(it_3->child[it_3->table[ip_idx]].second);
						log.ipNodeList.emplace_back(node_4);
						for (list<IpTable>::iterator it_4 = node_4->tableList.begin(); it_4 != node_4->tableList.end(); ++it_4) {
							ip_idx = mip[3] >> (8 - it_4->mask);
							if (it_4->pri > res) continue;
							++log.tables;
							if (it_4->table[ip_idx] == -1 || it_4->child[it_4->table[ip_idx]].first > res)continue;
							LeafNode* ln = (LeafNode*)(it_4->child[it_4->table[ip_idx]].second);
							log.pLeafNodeList.emplace_back(ln);
							for (auto&& r : ln->rule_) {
								++log.rules;
								if (res < r.PRI)break;
								if (pProto != r.protocol[1] && r.protocol[0] != 0)continue; // check protocol
								if (pDport < r.destination_port[0] || r.destination_port[1] < pDport)continue;  // if destination port not match, check next
								if (pSport < r.source_port[0] || r.source_port[1] < pSport)continue;  // if source port not match, check next
								unsigned int m_bit = 32 - (unsigned int)r.destination_mask;  // comput the bit number need to move
								unsigned int _ip;
								if (m_bit != 32) {
									memcpy(&_ip, r.destination_ip, 4);
									if (pDip >> m_bit != _ip >> m_bit)continue;  // if destination ip not match, check next
								}
								m_bit = 32 - (unsigned int)r.source_mask;  // comput the bit number need to move
								if (m_bit != 32) {
									memcpy(&_ip, r.source_ip, 4);
									if (pSip >> m_bit != _ip >> m_bit)continue;  // if source ip not match, check next
								}
								res = r.PRI;
								break;
							}
						}
					}
				}
			}
			break;
		}
		default:
			break;
		}
	}

	if (aTree != NULL) {
		int proto_idx[2] = { aTree->table[pProto],aTree->table[0] };
		int port_idx[2];
		for (int i = 0; i < 2; ++i) {
			if (proto_idx[i] != -1 && res > aTree->child[proto_idx[i]].first) {
				PortNode_static* pnode = (PortNode_static*)aTree->child[proto_idx[i]].second;
				if (portField == 0) port_idx[0] = pnode->table[pSport / 2];
				else port_idx[0] = pnode->table[pDport / 2];
				port_idx[1] = pnode->table[32768];
				log.portNodeList.emplace_back(pnode);
				for (int j = 0; j < 2; ++j) {
					if (port_idx[j] != -1 && res > pnode->child[port_idx[j]].first) {
						LeafNode* ln = pnode->child[port_idx[j]].second;
						log.aLeafNodeList.emplace_back(ln);
						for (auto&& r : ln->rule_) {
							++log.rules;
							if (res < r.PRI)break;
							if (pDport < r.destination_port[0] || r.destination_port[1] < pDport)continue;  // if destination port not match, check next
							if (pSport < r.source_port[0] || r.source_port[1] < pSport)continue;  // if source port not match, check next
							unsigned int m_bit = 32 - (unsigned int)r.destination_mask;  // comput the bit number need to move
							unsigned int _ip;
							if (m_bit != 32) {
								memcpy(&_ip, r.destination_ip, 4);
								if (pDip >> m_bit != _ip >> m_bit)continue;  // if destination ip not match, check next
							}
							m_bit = 32 - (unsigned int)r.source_mask;  // comput the bit number need to move
							if (m_bit != 32) {
								memcpy(&_ip, r.source_ip, 4);
								if (pSip >> m_bit != _ip >> m_bit)continue;  // if source ip not match, check next
							}
							res = r.PRI;
							break;
						}
					}
				}
			}
		}
	}

	log.tables += log.portNodeList.size() + 1;
	log.innerNodes = log.ipNodeList.size() + log.portNodeList.size() + 1;
	log.leafNodes = log.pLeafNodeList.size() + log.aLeafNodeList.size();

	return res;
}

bool PTtree::update(vector<rule>& rules, int num, struct timespec& t1, struct timespec& t2)
{
	int ruleNum = rules.size();
	vector<rule> newRule;
	for (int i = 0; i < num; ++i) {
		rule r = rules[i];
		r.PRI = ruleNum++;
		newRule.emplace_back(r);
	}

	clock_gettime(CLOCK_REALTIME, &t1);
	// remove
	for (int i = 0; i < num; ++i) {
		bool res = this->remove(rules[i]);
		if (!res) {
			fprintf(stderr, "error-can not find rule! Remove rules failed!");
			return res;
		}
	}
	// insert
	for (int i = 0; i < num; ++i) {
		this->insert(newRule[i]);
	}
	clock_gettime(CLOCK_REALTIME, &t2);
	return true;
}

void PTtree::print_node_info(int level, int rules)
{
	std::cout << "|- Total nodes num:          " << this->totalNodes << std::endl;
	std::cout << "|- Total inner nodes num:    " << this->ipNodeList.size() + this->portNodeList.size() + 1 << std::endl;
	std::cout << "|- Total leaf nodes num:     " << this->pLeafNodeList.size() + this->aLeafNodeList.size() << std::endl;
	std::cout << "|- IpNode num:               " << this->ipNodeList.size() << std::endl;
	std::cout << "|- pTree leaf nodes num:     " << this->pLeafNodeList.size() << std::endl;
	std::cout << "|- ProtoNode num:            " << this->portNodeList.size() << std::endl;
	std::cout << "|- aTree leaf nodes num:     " << this->aLeafNodeList.size() << std::endl;
	std::cout << "|- Average leaf node size:   " << (double)rules / (double)(this->pLeafNodeList.size() + this->aLeafNodeList.size()) << std::endl;
	if (level > 1) {
		double equ_1 = 0;
		double rang_1to100 = 0;
		double lager_100 = 0;
		int max_leaf = 0;
		for (auto leaf : this->pLeafNodeList) {
			if (leaf->rule_.size() > max_leaf)max_leaf = leaf->rule_.size();
			if (leaf->rule_.size() == 1)++equ_1;
			else if (leaf->rule_.size() < 100) ++rang_1to100;
			else ++lager_100;
		}
		for (auto leaf : this->aLeafNodeList) {
			if (leaf->rule_.size() > max_leaf)max_leaf = leaf->rule_.size();
			if (leaf->rule_.size() == 1)++equ_1;
			else if (leaf->rule_.size() < 100) ++rang_1to100;
			else ++lager_100;
		}
		std::cout << "|- Leaf node size->1:        " << equ_1 << std::endl;
		std::cout << "|- Leaf node size->(1, 100]: " << rang_1to100 << std::endl;
		std::cout << "|- Leaf node size->(100, +]: " << lager_100 << std::endl;
		std::cout << "|- Max leaf node size:       " << max_leaf << std::endl;

		if (level > 2) {
			FILE* fp = NULL;
			std::cout << "|- Write pTree inner node infomation to pInnerNode_info.txt...\n";
			fp = fopen("pInnerNode_info.txt", "w");
			fprintf(fp, "IpNode [ID LAYER FIELD TABLE_NUM CHILD_NUM]\n\n");
			switch (this->layerFields.size())
			{
			case 3: {
				for (auto& node : this->ipNodeList) {
					IpNode_static* n = (IpNode_static*)node;
					int c_num = 0;
					for (int i = 0; i < 257; ++i)if (n->child[i].pointer != NULL)++c_num;
					fprintf(fp, "%u\t%u\t%u\t1\t%d\n", n->id, n->layer, n->field, c_num);
				}
				break;
			}
			default: {
				for (auto& node : this->ipNodeList) {
					IpNode* n = (IpNode*)node;
					int c_num = 0;
					list<IpTable>::iterator it = n->tableList.begin();
					for (; it != n->tableList.end(); ++it)c_num += it->child.size();
					fprintf(fp, "%u\t%u\t%u\t%u\t%d\n", n->id, n->layer, n->field, n->tableList.size(), c_num);
				}
				break;
			}
			}
			fclose(fp);

			std::cout << "|- Write pTree leaf node infomation to pLeafNode_info.txt...\n";
			fp = fopen("pLeafNode_info.txt", "w");
			fprintf(fp, "Leaf Node [ID]\n|- Rule [PRI SIP DIP SPORT DPORT PROTOCOL]\n");
			for (int i = 0; i < this->pLeafNodeList.size(); ++i) {
				fprintf(fp, "\n%d\t%u\n", i, this->pLeafNodeList[i]->rule_.size());
				for (auto r : this->pLeafNodeList[i]->rule_)
					fprintf(fp, "|- %u\t%u.%u.%u.%u/%u\t\t%u.%u.%u.%u/%u\t\t%u:%u\t\t%u:%u\t\t%u\n", r.PRI, r.source_ip[3], r.source_ip[2], r.source_ip[1], r.source_ip[0], r.source_mask,
						r.destination_ip[3], r.destination_ip[2], r.destination_ip[1], r.destination_ip[0], r.destination_mask, r.source_port[0], r.source_port[1],
						r.destination_port[0], r.destination_port[1], r.protocol[1]);
			}
			fclose(fp);

			std::cout << "|- Write aTree inner node infomation to aInnerNode_info.txt...\n";
			fp = fopen("aInnerNode_info.txt", "w");
			fprintf(fp, "Protocol Node [ID TABLE_NUM CHILD_NUM]\n\n");
			fprintf(fp, "0\t1\t%u\n\n", this->aTree->child.size());
			fprintf(fp, "Port Node [ID TABLE_NUM CHILD_NUM]\n\n");
			for (auto node : this->portNodeList) {
				PortNode_static* n = (PortNode_static*)node;
				fprintf(fp, "%u\t2\t%u\n", n->id, n->child.size());
			}
			fclose(fp);

			std::cout << "|- Write aTree leaf node infomation to aLeafNode_info.txt...\n";
			fp = fopen("aLeafNode_info.txt", "w");
			fprintf(fp, "Leaf Node [ID SIZE]\n|- Rule [PRI SIP DIP SPORT DPORT PROTOCOL]\n");
			for (int i = 0; i < this->aLeafNodeList.size(); ++i) {
				fprintf(fp, "\n%d\t%u\n", i, this->aLeafNodeList[i]->rule_.size());
				for (auto r : this->aLeafNodeList[i]->rule_)
					fprintf(fp, "|- %u\t%u.%u.%u.%u/%u\t\t%u.%u.%u.%u/%u\t\t%u:%u\t\t%u:%u\t\t%u\n", r.PRI, r.source_ip[3], r.source_ip[2], r.source_ip[1], r.source_ip[0], r.source_mask,
						r.destination_ip[3], r.destination_ip[2], r.destination_ip[1], r.destination_ip[0], r.destination_mask, r.source_port[0], r.source_port[1],
						r.destination_port[0], r.destination_port[1], r.protocol[1]);
			}
			fclose(fp);
		}
	}
}

size_t PTtree::get_ipNode_mem(IpNode* node)
{
	size_t sum = sizeof(IpNode);
	for (list<IpTable>::iterator it = node->tableList.begin(); it != node->tableList.end(); ++it) {
		sum = sum + sizeof(IpTable) + sizeof(short) * it->table.size() + sizeof(pair<uint32_t,void*>) * it->child.size();
	}
	return sum;
}

size_t PTtree::get_leafNode_mem(LeafNode* node)
{
	size_t sum = sizeof(LeafNode) + sizeof(rule) * node->rule_.size();
	return sum;
}

size_t PTtree::get_static_mem(IpNode_static* node)
{
	size_t sum = sizeof(IpNode_static);
	if (node->childType == 0) {
		for (int i = 0; i < 257; ++i) {
			if (node->child[i].pointer != NULL)sum += get_static_mem((IpNode_static*)node->child[i].pointer);
		}
	}
	else {
		for (int i = 0; i < 257; ++i) {
			if (node->child[i].pointer != NULL)sum += get_leafNode_mem((LeafNode*)node->child[i].pointer);
		}
	}
	return sum;
}

size_t PTtree::get_mem(IpNode* node)
{
	size_t sum = get_ipNode_mem(node);
	if (node->childType == 0) {
		for (list<IpTable>::iterator it = node->tableList.begin(); it != node->tableList.end(); ++it) {
			for (int i = 0; i < it->child.size(); ++i) {
				sum += get_mem((IpNode*)(it->child[i].second));
			}
		}
	}
	else {
		for (list<IpTable>::iterator it = node->tableList.begin(); it != node->tableList.end(); ++it) {
			for (int i = 0; i < it->child.size(); ++i) {
				sum += get_leafNode_mem((LeafNode*)(it->child[i].second));
			}
		}

	}
	return sum;
}

size_t PTtree::mem()
{
	size_t sum = 0;
	if (aTree != NULL) {
		sum += sizeof(ProtoNode) + sizeof(short) * aTree->table.size() + sizeof(pair<uint32_t, PortNode_static*>) * aTree->child.size();
		for (auto&& c : aTree->child) {
			PortNode_static* pnode = (PortNode_static*)c.second;
			sum += sizeof(PortNode_static) + sizeof(pair<uint32_t, LeafNode*>) * pnode->child.size();
			for (auto&& leaf : pnode->child) {
				sum += sizeof(LeafNode) + leaf.second->rule_.size() * sizeof(rule);
			}
		}
	}
	switch (layerFields.size())
	{
	case 3: {
		return sum + get_static_mem((IpNode_static*)pTree);
	}
	default:
		return sum + get_mem((IpNode*)pTree);
	}
}

void PTtree::analyse_ruleset(vector<rule>& list)
{
	char ip_name[8][6] = { "sip_1","sip_2","sip_3" ,"sip_4" ,"dip_1" ,"dip_2" ,"dip_3","dip_4" };
	double alpha[9];
	for (int i = 0; i < 9; ++i)alpha[i] = pow(2, i) / 511;

	FILE* fp = NULL;
	fp = fopen("ruleset_analyse.txt", "w");
	vector<vector<vector<int>>> counter;
	counter.resize(8);
	for (int i = 0; i < 8; ++i) {
		counter[i].resize(9);
		for (int j = 0; j < 9; ++j) {
			counter[i][j].resize((int)pow(2, j));
			for (auto& ele : counter[i][j])ele = 0;
		}
	}
	for (auto& _r : list) {
		int k = 4;
		unsigned int mask = _r.source_mask;
		for (int j = 0; j < 4; j++) {
			unsigned int value = _r.source_ip[--k];
			int _m = maskHash[mask][j];
			value = value >> (8 - _m);
			++counter[j][_m][value];
		}
		k = 4;
		mask = _r.destination_mask;
		for (int j = 4; j < 8; j++) {
			unsigned int value = _r.destination_ip[--k];
			int _m = maskHash[mask][j - 4];
			value = value >> (8 - _m);
			++counter[j][_m][value];
		}
	}
	vector<vector<double>> gini(8, vector<double>(9, 0));
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 9; ++j) {
			int c_size = counter[i][j].size();
			double _y = 0;
			vector<double> _yy(c_size + 1, 0);
			for (int k = 0; k < c_size; ++k) {
				_y += counter[i][j][k];
			}
			if (_y != 0) {
				double tmp = 0;
				vector<int> tmp_y(counter[i][j]);
				sort(tmp_y.begin(), tmp_y.end());
				for (int k = 0; k < c_size; ++k) {
					tmp += tmp_y[k];
					_yy[k + 1] = tmp / _y;
					//printf("%lf ", _yy[k + 1]);
				}
				//printf("\n");
				double sum = 0, a = 0;
				for (int k = 0; k < c_size; ++k) {
					a = (_yy[k] + _yy[k + 1]) / 2.0 / c_size;
					sum += a;
				}
				gini[i][j] = (0.5 - sum) / 0.5;
			}
		}
		gini[i][0] = 1;
	}
	for (int i = 0; i < 8; ++i) {
		double _g = 0;
		fprintf(fp, "%s\n", ip_name[i]);
		for (int j = 0; j < 9; ++j) {
			fprintf(fp, "\tmask=%d", j);
			for (auto ele : counter[i][j]) {
				fprintf(fp, " %d", ele);
			}
			fprintf(fp, " gini=%lf\n", gini[i][j]);
			_g += (alpha[j] * gini[i][j]);
		}
		fprintf(fp, "total_gini=%lf\n", _g);
	}
	fclose(fp);
}

int check_correct(rule& a, message& b)
{
	if (a.protocol[0] != 0 && (uint32_t)a.protocol[1] != b.protocol)return 0;
	int mask = 32 - (uint32_t)a.source_mask;
	uint32_t sip, dip;
	memcpy(&sip, a.source_ip, 4); memcpy(&dip, b.source_ip, 4);
	if (mask != 32 && (sip >> mask) != (dip >> mask))return 0;
	mask = 32 - (uint32_t)a.destination_mask;
	memcpy(&sip, a.destination_ip, 4); memcpy(&dip, b.destination_ip, 4);
	if (mask != 32 && (sip >> mask) != (dip >> mask))return 0;
	if (b.source_port < a.source_port[0] || b.source_port > a.source_port[1])return 0;
	if (b.destination_port < a.destination_port[0] || b.destination_port > a.destination_port[1])return 0;
	return 1;
}

int simple_search(vector<rule>& rules, message& b)
{
	for (auto&& a : rules) {
		if (a.protocol[0] != 0 && (uint32_t)a.protocol[1] != b.protocol)continue;
		int mask = 32 - (uint32_t)a.source_mask;
		uint32_t sip, dip;
		memcpy(&sip, a.source_ip, 4); memcpy(&dip, b.source_ip, 4);
		if (mask != 32 && (sip >> mask) != (dip >> mask))continue;
		mask = 32 - (uint32_t)a.destination_mask;
		memcpy(&sip, a.destination_ip, 4); memcpy(&dip, b.destination_ip, 4);
		if (mask != 32 && (sip >> mask) != (dip >> mask))continue;
		if (b.source_port < a.source_port[0] || b.source_port > a.source_port[1])continue;
		if (b.destination_port < a.destination_port[0] || b.destination_port > a.destination_port[1])continue;
		return a.PRI;
	}
	return -1;
}

void setmaskHash()
{
	for (int i = 0; i < 33; ++i) {
		int j = 0;
		for (; j < i / 8; ++j)maskHash[i][j] = 8;
		if (j < 4) {
			maskHash[i][j] = i % 8;
			for (++j; j < 4; ++j)maskHash[i][j] = 0;
		}
	}
}
