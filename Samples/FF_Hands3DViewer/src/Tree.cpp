#include "Tree.h"
#include "pxchanddata.h"

Node<PointData>::Node(PointData jointData)
{
	m_nodeValue = jointData;
}
//===========================================================================//

Node<PointData>::Node() {}

//===========================================================================//

Node<PointData>::~Node() {}

//===========================================================================//

void Node<PointData>::add(Node<PointData> node)
{
	m_nodes.push_back(node);
}

//===========================================================================//

PointData Node<PointData>::getNodeValue()
{
	return m_nodeValue;
}

//===========================================================================//

std::vector<Node<PointData>> Node<PointData>::getChildNodes()
{
	return m_nodes;
}

//===========================================================================//

Tree<PointData>::Tree(Node<PointData> rootNode)
{
	m_root = rootNode;
}

//===========================================================================//

void Tree<PointData>::setRoot(Node<PointData> rootNode)
{
	m_root = rootNode;
}

//===========================================================================//

Node<PointData> Tree<PointData>::getRoot()
{
	return m_root;
}

//===========================================================================//

Tree<PointData>::Tree() {}

//===========================================================================//

Tree<PointData>::~Tree() {}